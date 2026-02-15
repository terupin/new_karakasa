#include "Window.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

bool Window::Create(HINSTANCE hInst, int width, int height, const wchar_t* title)
{
	m_width = width;
	m_height = height;

	const wchar_t* kClassName = L"KarakasaWindowClass";

	//ウィンドウクラス設定
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Window::WndProc;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = kClassName;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	//登録
	if (!RegisterClassEx(&wc))
		return false;

	//サイズ調整
	RECT rc = {};
	rc.right = static_cast<LONG>(m_width);
	rc.bottom = static_cast<LONG>(m_height);

	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rc, style, FALSE);

	//生成
	m_hwnd = CreateWindowEx(
		0,
		kClassName,
		title,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInst,
		nullptr);

	if (m_hwnd == nullptr)
		return false;

	//表示
	ShowWindow(m_hwnd, SW_SHOWNORMAL);
	UpdateWindow(m_hwnd);

	if (!InitD3D())
		return false;

	return true;
}

int Window::Run()
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

bool Window::InitD3D()
{
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 1;
	scd.BufferDesc.Width = (UINT)m_width;
	scd.BufferDesc.Height = (UINT)m_height;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = m_hwnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT flags = 0;

#if defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr, 0,
		D3D11_SDK_VERSION,
		&scd,
		&m_swapChain,
		&m_device,
		&featureLevel,
		&m_context
	);
	if (FAILED(hr)) return false;

	//バックバッファ
	ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(backBuffer.GetAddressOf())
	);
	if (FAILED(hr)) return false;

	hr = m_device->CreateRenderTargetView(
		backBuffer.Get(),
		nullptr,
		m_rtv.GetAddressOf()
	);
	if (FAILED(hr)) return false;

	//バインド
	ID3D11RenderTargetView* rtvs[] = { m_rtv.Get() };
	m_context->OMSetRenderTargets(1, rtvs, nullptr);







	return true;
}

void Window::Render()
{
	const float clearColor[4] = { 0.1f, 0.2f, 0.8f, 1.0f };
	m_context->ClearRenderTargetView(m_rtv.Get(), clearColor);
	m_swapChain->Present(1, 0);
}


