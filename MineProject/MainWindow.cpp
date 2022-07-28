#include "MainWindow.h"

LRESULT MainWindow::OnLButtonDown(D2D1_POINT_2F pos)
{
	D2D1_SIZE_U mapSize = map->GetMapSize();
	D2D1_SIZE_F blockSize = map->GetBlockSize();
	Mine::Block* bTemp;
	for (size_t y = 0; y < mapSize.height; ++y) {
		for (size_t x = 0; x < mapSize.width; ++x) {
			if (m_window_padding + (m_window_gap + blockSize.width) * x < pos.x && pos.x < m_window_padding + (m_window_gap + blockSize.width) * x + blockSize.width &&
				m_window_padding + (m_window_gap + blockSize.height) * y < pos.y && pos.y < m_window_padding + (m_window_gap + blockSize.height) * y + blockSize.height) {
				bTemp = map->GetBlock(x, y);

				// PICKED는 클릭할 수 없음
				if (bTemp->GetState() == Mine::Block::BLOCK_STATE::PICKED) {
					y = mapSize.height;
					break;
				}

				bTemp->SetState(Mine::Block::BLOCK_STATE::OPENED);

				if (bTemp->isBomb() == true) {
					InvalidateRect(Window(), nullptr, false);
					MessageBoxW(Window(), L"지뢰 밟았어요", L"지뢰찾기", 0 | MB_ICONEXCLAMATION);
					SendMessage(Window(), WM_CLOSE, 0, 0);
					return 0;
				}
				else {
					if (!bTemp->GetAroundBomb()) {
						std::stack<POINT> visitList;
						POINT curPos{x,y}, tempPos;
						
						bool** visit = new bool*[mapSize.height];
						for (size_t i = 0; i < mapSize.height; ++i) {
							visit[i] = new bool[mapSize.width]{};
						}
						visitList.push(curPos);
						static POINT offset[8]{
							{-1,-1},{-1,0},{-1,1},
							{0,-1},{0,1},
							{1,-1},{1,0},{1,1}
						};

						do {
							curPos = visitList.top();
							visitList.pop();

							if (visit[curPos.y][curPos.x]) continue;	// 이미 지나간 길은 패스
							bTemp = map->GetBlock(curPos.x, curPos.y);

							visit[curPos.y][curPos.x] = true;
							if(bTemp->GetState() != Mine::Block::BLOCK_STATE::PICKED)
								bTemp->SetState(Mine::Block::BLOCK_STATE::OPENED);

							if (bTemp->GetAroundBomb()) continue;	// 주변에 폭탄이 있으면 패스

							for (int i = 0; i < 8; ++i) {
								tempPos.x = curPos.x + offset[i].x;
								tempPos.y = curPos.y + offset[i].y;
								if (map->isAvailable(tempPos.x, tempPos.y) && bTemp->GetState() != Mine::Block::BLOCK_STATE::PICKED) { // 범위 안인 경우에만
									visitList.push(tempPos);
								}
							}


						} while (!visitList.empty());

						for (size_t i = 0; i < mapSize.height; ++i) {
							delete[] visit[i];
						}
						delete[] visit;

					}

					y = mapSize.height;
					break;
				}


			}
		}
	}


	InvalidateRect(Window(), nullptr, false);

	if (map->isClear()) {
		MessageBoxW(Window(), L"지뢰찾기 성공!", L"지뢰찾기", 0 | MB_ICONINFORMATION);
		SendMessage(Window(), WM_CLOSE, 0, 0);
	}

	return 0;
}

LRESULT MainWindow::OnRButtonDown(D2D1_POINT_2F pos)
{
	D2D1_SIZE_U mapSize = map->GetMapSize();
	D2D1_SIZE_F blockSize = map->GetBlockSize();
	Mine::Block* bTemp;
	for (size_t y = 0; y < mapSize.height; ++y) {
		for (size_t x = 0; x < mapSize.width; ++x) {
			if (m_window_padding + (m_window_gap + blockSize.width) * x < pos.x && pos.x < m_window_padding + (m_window_gap + blockSize.width) * x + blockSize.width &&
				m_window_padding + (m_window_gap + blockSize.height) * y < pos.y && pos.y < m_window_padding + (m_window_gap + blockSize.height) * y + blockSize.height) {
				bTemp = map->GetBlock(x, y);
				switch (bTemp->GetState()) {
					case Mine::Block::BLOCK_STATE::CLOSED:
						bTemp->SetState(Mine::Block::BLOCK_STATE::PICKED);
						break;
					case Mine::Block::BLOCK_STATE::PICKED:
						bTemp->SetState(Mine::Block::BLOCK_STATE::CLOSED);
						break;
				}

				y = mapSize.height;
				break;

			}
		}
	}

	InvalidateRect(Window(), nullptr, false);

	if (map->isClear()) {
		MessageBoxW(Window(), L"지뢰찾기 성공!", L"지뢰찾기", 0 | MB_ICONINFORMATION);
		SendMessage(Window(), WM_CLOSE, 0, 0);
	}

	return 0;
}

LRESULT MainWindow::OnMouseMove(D2D1_POINT_2F pos)
{
	MovePoint = pos;
	InvalidateRect(Window(), nullptr, false);
	return 0;
}

LRESULT MainWindow::OnSize()
{
	dt->OnSize();
	InvalidateRect(Window(), nullptr, 0);
	return 0;
}

LRESULT MainWindow::OnCreate()
{
	dt = new Mine::DrawTool{Window()};
	tt = new Mine::TextTool{};

	return 0;
}

LRESULT MainWindow::OnPaint()
{
	HRESULT hr;
	if (map) {
		D2D1_SIZE_U mapSize = map->GetMapSize();
		D2D1_SIZE_F blockSize = map->GetBlockSize();
		D2D1_POINT_2F blockPos;
	
		Mine::DrawSet drawSet{mapSize.width * mapSize.height + 1};
		Mine::DrawObjectRectangle* rectObj;
		Mine::DrawObjectText* textObj;
		Mine::Block* bTemp;

		for (size_t y = 0; y < mapSize.height; ++y) {
			for (size_t x = 0; x < mapSize.width; ++x) {
				bTemp = map->GetBlock(x, y);
				blockPos = D2D1::Point2F(x * (blockSize.width + m_window_gap) + m_window_padding, y * (blockSize.height + m_window_gap) + m_window_padding);

				switch (bTemp->GetState()) {
					case Mine::Block::BLOCK_STATE::CLOSED:
						if (blockPos.x < MovePoint.x && MovePoint.x < blockPos.x + blockSize.width &&
							blockPos.y < MovePoint.y && MovePoint.y < blockPos.y + blockSize.height) {
							rectObj = new Mine::DrawObjectRectangle{blockPos, blockSize, Mine::DrawObjectRectangle::DrawOption::inner, D2D1::ColorF(0xf0be37), D2D1::ColorF(0x333333)};

						}
						else rectObj = new Mine::DrawObjectRectangle{blockPos, blockSize, Mine::DrawObjectRectangle::DrawOption::inner, D2D1::ColorF(0x9e7400), D2D1::ColorF(0x333333)};

						drawSet << rectObj;
						break;
					case Mine::Block::BLOCK_STATE::OPENED:
						if (bTemp->isBomb() == true) {
							rectObj = new Mine::DrawObjectRectangle{blockPos, blockSize, Mine::DrawObjectRectangle::DrawOption::inner, D2D1::ColorF(0xff0000), D2D1::ColorF(0x333333)};
							drawSet << rectObj;
						}
						else {
							if (bTemp->GetAroundBomb()) {
								wchar_t str[2];
								swprintf_s(str,2, L"%d", bTemp->GetAroundBomb());
								static unsigned int color[4]{
									0xfff0cf,
									0xffba8f,
									0x934bb8,
									0xb03552,
								};

								textObj = new Mine::DrawObjectText{str,tt->CreateTextFormat(L"D2Coding", 16.f),blockPos, blockSize, D2D1::ColorF(0x000000), D2D1::ColorF(color[(bTemp->GetAroundBomb() - 1) / 2])};
								drawSet << textObj;
							}
							else {
								rectObj = new Mine::DrawObjectRectangle{blockPos, blockSize, Mine::DrawObjectRectangle::DrawOption::inner, D2D1::ColorF(0xfffed6), D2D1::ColorF(0x000000)};
								drawSet << rectObj;
							}
						}
						break;
					case Mine::Block::BLOCK_STATE::PICKED:
						if (blockPos.x < MovePoint.x && MovePoint.x < blockPos.x + blockSize.width &&
							blockPos.y < MovePoint.y && MovePoint.y < blockPos.y + blockSize.height) {
							rectObj = new Mine::DrawObjectRectangle{blockPos, blockSize, Mine::DrawObjectRectangle::DrawOption::inner, D2D1::ColorF(0xa4d0ed), D2D1::ColorF(0x000000)};
						}
						else {
							rectObj = new Mine::DrawObjectRectangle{blockPos, blockSize, Mine::DrawObjectRectangle::DrawOption::inner, D2D1::ColorF(0x489fd9), D2D1::ColorF(0x000000)};
						}
						drawSet << rectObj;

						break;
				}

			}
		}

		hr = dt->OnPaint(drawSet);
	}


	return hr;
}

void MainWindow::InitializeMap()
{
	//map = new Mine::MapHard{};
	RECT windowRect, clientRect, windowSize{};
	D2D1_SIZE_U mapSize = map->GetMapSize();
	D2D1_SIZE_F blockSize = map->GetBlockSize();
	
	GetWindowRect(Window(), &windowRect);
	GetClientRect(Window(), &clientRect);

	windowSize.left = windowSize.top = 0;
	windowSize.right = windowRect.right - windowRect.left;
	windowSize.bottom = windowRect.bottom - windowRect.top;


	SetWindowPos(Window(), nullptr, windowRect.left, windowRect.top,
		m_window_padding*2 + (windowSize.right - clientRect.right) + (blockSize.width + m_window_gap) * mapSize.width - m_window_gap,
		m_window_padding*2 + (windowSize.bottom - clientRect.bottom) + (blockSize.height + m_window_gap) * mapSize.height - m_window_gap,
		SWP_NOMOVE);

	OnSize();
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CLOSE) {
		ShowWindow(beforeWindow, SW_SHOWNORMAL);
		ShowWindow(Window(), SW_HIDE);
		return 0;
	}
	else if (uMsg == WM_DESTROY) {
		
		return 0;
	}

	switch (uMsg) {
		case WM_CREATE: return OnCreate();
		case WM_PAINT: return OnPaint();
		case WM_LBUTTONDOWN: return OnLButtonDown(D2D1::Point2F(LOWORD(lParam), HIWORD(lParam)));
		case WM_RBUTTONDOWN: return OnRButtonDown(D2D1::Point2F(LOWORD(lParam), HIWORD(lParam)));
		case WM_MOUSEMOVE: return OnMouseMove(D2D1::Point2F(LOWORD(lParam), HIWORD(lParam)));
	}

	return DefWindowProc(Window(), uMsg, wParam, lParam);
}
