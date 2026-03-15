#include "Window.h"
#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;

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

	//深度バッファ（Depth Buffer）
	D3D11_TEXTURE2D_DESC depthDesc{};
	depthDesc.Width = (UINT)m_width;
	depthDesc.Height = (UINT)m_height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = m_device->CreateTexture2D(&depthDesc, nullptr, m_depthTex.GetAddressOf());
	if (FAILED(hr)) return false;

	//DSV作成
	hr = m_device->CreateDepthStencilView(m_depthTex.Get(), nullptr, m_dsv.GetAddressOf());
	if (FAILED(hr)) return false;


	//バインド
	ID3D11RenderTargetView* rtvs[] = { m_rtv.Get() };
	m_context->OMSetRenderTargets(1, rtvs, m_dsv.Get());

	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)m_width;
	vp.Height = (FLOAT)m_height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_context->RSSetViewports(1, &vp);

	if (!InitResources())
		return false;

	m_obj1.mesh = &m_mesh;
	m_obj2.mesh = &m_mesh;

	m_obj1.transform.position = { -0.2f, 0.0f, 0.6f };
	m_obj2.transform.position = { 0.2f, 0.0f, 0.0f };

	return true;
}

void Window::Render()
{
	//カメラ更新
	m_camera.Update();

	//クリア
	const float clearColor[4] = { 0.1f,0.2f,0.8f,1.0f };
	m_context->ClearRenderTargetView(m_rtv.Get(), clearColor);
	m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//パイプライン設定
	m_context->IASetInputLayout(m_inputLayout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_context->VSSetShader(m_vs.Get(), nullptr, 0);
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);


	//行列
	static float angle = 0.0f;
	angle += 0.01f;

	XMMATRIX V = m_camera.GetViewMatrix();

	//プロジェクション（遠近）
	float fovY = XM_PIDIV4; //45度
	float aspect = (float)m_width / (float)m_height; //アスペクト比
	XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspect, 0.1f, 100.0f);

	//b1 Light
	CBLight cbL{};
	cbL.lightDir = XMFLOAT3(0.3f, -0.6f, 1.0f);
	cbL.lightColor = XMFLOAT4(1, 1, 1, 1);
	cbL.ambient = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	cbL.cameraPos = m_camera.GetPosition();
	cbL.specStrength = 4.0f;
	cbL.shininess = 8.0f;
	m_context->UpdateSubresource(m_cbLight.Get(), 0, nullptr, &cbL, 0, 0);

	// VS/PS にセット
	ID3D11Buffer* vsCBs[] = { m_cbTransform.Get() };
	m_context->VSSetConstantBuffers(0, 1, vsCBs);

	ID3D11Buffer* psCBs[] = { m_cbLight.Get() };
	m_context->PSSetConstantBuffers(1, 1, psCBs);

	ID3D11Buffer* psCBs0[] = { m_cbTransform.Get() };
	m_context->PSSetConstantBuffers(0, 1, psCBs0);

	m_obj1.transform.rotation.y = angle;
	m_obj2.transform.rotation.y = angle;

	DrawRenderItem(m_obj1, V, P);
	DrawRenderItem(m_obj2, V, P);

	m_swapChain->Present(1, 0);
}

bool Window::InitResources()
{

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

	D3D11_BUFFER_DESC cbd{};
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	cbd.ByteWidth = sizeof(CBTransform);
	hr = m_device->CreateBuffer(&cbd, nullptr, m_cbTransform.GetAddressOf());
	if (FAILED(hr)) return false;

	cbd.ByteWidth = sizeof(CBLight);
	hr = m_device->CreateBuffer(&cbd, nullptr, m_cbLight.GetAddressOf());
	if (FAILED(hr)) return false;


	//Input Layer
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = m_device->CreateInputLayout(
		layout,
		(UINT)_countof(layout),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		m_inputLayout.GetAddressOf()
	);
	if (FAILED(hr)) return false;

	if (!m_mesh.CreateTriangle(m_device.Get()))
		return false;

	return true;
}

void Window::DrawRenderItem(const RenderItem& item, const DirectX::XMMATRIX& V, const DirectX::XMMATRIX& P)
{
	CBTransform cbT{};
	XMStoreFloat4x4(&cbT.world, XMMatrixTranspose(item.transform.GetMatrix()));
	XMStoreFloat4x4(&cbT.viewProj, XMMatrixTranspose(V * P));

	m_context->UpdateSubresource(m_cbTransform.Get(), 0, nullptr, &cbT, 0, 0);
	item.mesh->Draw(m_context.Get());
}
