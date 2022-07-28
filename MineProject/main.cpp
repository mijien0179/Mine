#define _CRT_SECURE_NO_WARNINGS
#include "MainWindow.h"
#include "levelSelectWindow.h"

#pragma warning(suppress:28251)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	LvSelectWindow win{};

#if _DEBUG

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	std::ios::sync_with_stdio();

#endif

	if (!win.Create(L"지뢰찾기 레벨 선택", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX & ~WS_SIZEBOX)) {
		return -1;
	}

	ShowWindow(win.Window(), nCmdShow);
	UpdateWindow(win.Window());

	MSG msg;

	while (GetMessageW(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return msg.wParam;
}