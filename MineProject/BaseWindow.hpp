#pragma once
#include <Windows.h>

template<class DERIVED_TYPE>
class BaseWindow {
public:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		DERIVED_TYPE* pThis = nullptr;

		if (uMsg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pThis = reinterpret_cast<DERIVED_TYPE*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
			pThis->hWnd = hWnd;
		}
		else {
			pThis = reinterpret_cast<DERIVED_TYPE*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		if (pThis) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	bool Create(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle = 0,
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT,
		HWND hWndParent = nullptr, HMENU hMenu = nullptr
	) {
		WNDCLASS wc{};

		wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = ClassName();
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

		if (!RegisterClass(&wc)) throw "등록 실패";

		hWnd = CreateWindow(ClassName(), lpWindowName, dwStyle, x, y,
			nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(nullptr), this);

		return hWnd ? true : false;
	}

	inline HWND Window() const {
		return hWnd;
	}

	BaseWindow() : hWnd{}{}
	virtual ~BaseWindow(){}


protected:
	HWND hWnd;
	virtual LPCWSTR ClassName() const = 0;
	virtual LRESULT HandleMessage(UINT, WPARAM, LPARAM) = 0;

};