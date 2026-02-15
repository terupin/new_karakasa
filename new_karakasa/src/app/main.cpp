#include <Windows.h>
#include "Window.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
#if define(DEBUG) /|| defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Window window;
	if (!window.Create(hInst, 1280, 720, L"Karakasa"))
		return 0;

	MessageBox(nullptr, L"DX11 Start!", L"Test", MB_OK);

	return window.Run();
}
