#include "Window.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

struct Vertex
{
	float x, y, z;
	float r, g, b, a;
};


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
	//スワップチェーン（画面表示用の表裏バッファ管理）の設定
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

	// GPU描画に必要な3点セットをまとめて作成
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr, 0,
		D3D11_SDK_VERSION,
		&scd,
		m_swapChain.GetAddressOf(),
		m_device.GetAddressOf(),
		&featureLevel,
		m_context.GetAddressOf()
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

	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)m_width;
	vp.Height = (FLOAT)m_height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_context->RSSetViewports(1, &vp);

	if (!InitTriangle())
		return false;

	return true;
}

void Window::Render()
{
	const float clearColor[4] = { 0.1f, 0.2f, 0.8f, 1.0f };
	m_context->ClearRenderTargetView(m_rtv.Get(), clearColor);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* vbs[] = { m_vb.Get() };

	m_context->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
	m_context->IASetInputLayout(m_inputLayout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_context->VSSetShader(m_vs.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);

	m_context->Draw(3, 0);

	m_swapChain->Present(1, 0);
}

bool Window::InitTriangle()
{
	// 1) 頂点データ
	Vertex verts[3] =
	{
		{  0.0f,  0.5f, 0.0f,  1,0,0,1 },
		{  0.5f, -0.5f, 0.0f,  0,1,0,1 },
		{ -0.5f, -0.5f, 0.0f,  0,0,1,1 },
	};

	//シェーダーをコンパイル
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3DBlob> errBlob;

	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(
		L"shaders/Basic.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VSMain",
		"vs_5_0",
		compileFlags, 0,
		vsBlob.GetAddressOf(),
		errBlob.GetAddressOf()
	);
	if (FAILED(hr))
	{
		if (errBlob)
			MessageBoxA(nullptr, (char*)errBlob->GetBufferPointer(), "PS Compile Error", MB_OK);
		return false;
	}

	errBlob.Reset();
	hr = D3DCompileFromFile(
		L"shaders/Basic.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSMain",
		"ps_5_0",
		compileFlags, 0,
		psBlob.GetAddressOf(),
		errBlob.GetAddressOf()
	);
	if (FAILED(hr))
	{
		if (errBlob)
			MessageBoxA(nullptr, (char*)errBlob->GetBufferPointer(), "PS Compile Error", MB_OK);
		return false;
	}


	//シェーダーオブジェクト作成
	hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vs.GetAddressOf());
	if (FAILED(hr)) return false;

	hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_ps.GetAddressOf());
	if (FAILED(hr)) return false;

	//Input Layer
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = m_device->CreateInputLayout(
		layout,
		(UINT)_countof(layout),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		m_inputLayout.GetAddressOf()
	);
	if (FAILED(hr)) return false;

	//VertexBufferの作成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(verts);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = verts;

	hr = m_device->CreateBuffer(&bd, &initData, m_vb.GetAddressOf());
	if (FAILED(hr)) return false;

	return true;
}



