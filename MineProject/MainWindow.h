#pragma once
#include <stack>

#include "BaseWindow.hpp"
#include "Draw.h"
#include "map.h"

class MainWindow : public BaseWindow<MainWindow> {
	constexpr static size_t m_window_padding = 25;
	constexpr static size_t m_window_gap = 3;

	D2D1_POINT_2F MovePoint;

private:
	Mine::DrawTool* dt;
	Mine::TextTool* tt;
	Mine::IMap* map;

private:
	LRESULT OnLButtonDown(D2D1_POINT_2F pos);
	LRESULT OnRButtonDown(D2D1_POINT_2F pos);
	LRESULT OnMouseMove(D2D1_POINT_2F pos);
	LRESULT OnSize();
	LRESULT OnCreate();
	LRESULT OnPaint();

	void InitializeMap();

	HWND beforeWindow;

public:
	virtual LPCWSTR ClassName() const { return L"MineCreator"; }
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void SetMap(Mine::IMap* map) {
		if (this->map) {
			delete this->map;
		}
		this->map = map;
		InitializeMap();
	}

	MainWindow(HWND beforeHwnd)
		: MovePoint{}, dt{}, tt{}, beforeWindow{beforeHwnd}
	{
	
	}
	virtual ~MainWindow() {
		delete dt;
		delete tt;
		delete map;

	}
};