#include "levelSelectWindow.h"

LRESULT LvSelectWindow::OnCreate()
{
	dt = new Mine::DrawTool{Window()};
	tt = new Mine::TextTool{};

	{
		window = new MainWindow{Window()};
		if (!window->Create(L"지뢰찾기", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX & ~WS_SIZEBOX)){
			throw "게임 윈도우를 생성할 수 없습니다.";
		}
	}

	RECT windowRect, windowSize, clientRect;
	GetWindowRect(Window(), &windowRect);
	GetClientRect(Window(), &clientRect);

	windowSize.left = windowSize.top = 0;
	windowSize.right = windowRect.right - windowRect.left;
	windowSize.bottom = windowRect.bottom - windowRect.top;
	
	m_window_padding = D2D1::Point2F(20, 20);
	m_window_gap = D2D1::Point2F(10, 0);
	ctrlText[0] = L"Easy";
	ctrlText[1] = L"Middle";
	ctrlText[2] = L"Hard";
	ctrlText[3] = L"Hell";

	for (int i = 0; i < 4; ++i) {
		ctrlSize[i] = D2D1::SizeF(116, 32);
		ctrlList[i] = D2D1::Point2F(m_window_padding.x + (116 + m_window_gap.x) * i, m_window_padding.y);
	}

	SetWindowPos(Window(), nullptr, windowRect.left, windowRect.top,
		m_window_padding.x * 2 + (windowSize.right - clientRect.right) + (ctrlSize[0].width + m_window_gap.x) * 4 - m_window_gap.x,
		m_window_padding.y * 2 + (windowSize.bottom - clientRect.bottom) + (ctrlSize[0].height + m_window_gap.y ) - m_window_gap.y, 
		
		SWP_NOMOVE);

	OnSize();

	return 0;
}

LRESULT LvSelectWindow::OnLButtonDown(D2D1_POINT_2F pos)
{
	for (int i = 0; i < 4; ++i) {
		if (ctrlList[i].x < pos.x && pos.x < ctrlList[i].x + ctrlSize[i].width &&
			ctrlList[i].y < pos.y && pos.y < ctrlList[i].y + ctrlSize[i].height) {
			if (window) {
				switch (i) {
					case 0:
						window->SetMap(new Mine::MapEasy{});
						break;
					case 1:
						window->SetMap(new Mine::MapNormal{});

						break;
					case 2:
						window->SetMap(new Mine::MapHard{});

						break;
					case 3:
						window->SetMap(new Mine::MapHell{});
						break;
				}

				ShowWindow(window->Window(), SW_SHOWNORMAL);
				ShowWindow(Window(), SW_HIDE);
			}
		}
	}

	return 0;
}

LRESULT LvSelectWindow::OnMouseMove(D2D1_POINT_2F pos)
{
	MovePoint = pos;
	InvalidateRect(Window(), nullptr, false);
	return 0;
}

LRESULT LvSelectWindow::OnPaint()
{
	HRESULT hr;
	Mine::DrawSet dSet{4};
	Mine::DrawObjectText* textObj;
	for (int i = 0; i < 4; ++i) {
		if (ctrlList[i].x < MovePoint.x && MovePoint.x < ctrlList[i].x + ctrlSize[i].width &&
			ctrlList[i].y < MovePoint.y && MovePoint.y < ctrlList[i].y + ctrlSize[i].height) {
			textObj = new Mine::DrawObjectText{const_cast<wchar_t*>(ctrlText[i]),tt->CreateTextFormat(L"D2Coding", 16.f), ctrlList[i], ctrlSize[i], D2D1::ColorF(0x000000),D2D1::ColorF(0xAAAAAA)};
		}
		else {
			textObj = new Mine::DrawObjectText{const_cast<wchar_t*>(ctrlText[i]),tt->CreateTextFormat(L"D2Coding", 16.f), ctrlList[i], ctrlSize[i], D2D1::ColorF(0x000000),D2D1::ColorF(0x888888)};

		}
		dSet << textObj;

	}

	hr = dt->OnPaint(dSet);

	return hr;
}

LRESULT LvSelectWindow::OnSize()
{
	dt->OnSize();
	return 0;
}

LRESULT LvSelectWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_CLOSE == uMsg) {
		PostQuitMessage(0);
		return 0;
	}
	else if (WM_DESTROY == uMsg) {
		DestroyWindow(Window());
		return 0;
	}

	switch (uMsg) {
		case WM_CREATE: return OnCreate();
		case WM_LBUTTONDOWN: return OnLButtonDown(D2D1::Point2F(LOWORD(lParam), HIWORD(lParam)));
		case WM_MOUSEMOVE: return OnMouseMove(D2D1::Point2F(LOWORD(lParam), HIWORD(lParam)));
		case WM_PAINT: return OnPaint();
	}

	return DefWindowProc(Window(), uMsg, wParam, lParam);
}
