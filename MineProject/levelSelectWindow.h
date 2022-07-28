#pragma once
#include "BaseWindow.hpp"
#include "Draw.h"
#include "MainWindow.h"

class LvSelectWindow : public BaseWindow<LvSelectWindow> {
	Mine::DrawTool* dt;
	Mine::TextTool* tt;

	D2D1_POINT_2F ctrlList[4];
	D2D1_SIZE_F ctrlSize[4];
	const wchar_t* ctrlText[4];
	void (LvSelectWindow::* fp[4])();
	
	MainWindow* window;

private:
	D2D1_POINT_2F MovePoint;

	D2D1_POINT_2F m_window_padding;
	D2D1_POINT_2F m_window_gap;

private:
	LRESULT OnCreate();
	LRESULT OnLButtonDown(D2D1_POINT_2F pos);
	LRESULT OnMouseMove(D2D1_POINT_2F pos);
	LRESULT OnPaint();
	LRESULT OnSize();

public:
	virtual LPCWSTR ClassName() const { return L"mine-find-level-select-window"; }
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LvSelectWindow() :BaseWindow{}, window{}, dt{}, tt{}, fp{}, MovePoint{}
	{

	}
	virtual ~LvSelectWindow() {
		delete dt;
		delete tt;
		if (window) {
			delete window;
			window = nullptr;
		}
	}

};