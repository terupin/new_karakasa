#pragma once
#define NOMINMAX
#include<Windows.h>
#include<d3d11.h>
#include <cstdint>
#include<wrl/client.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

class Window {
public:
	bool Create(HINSTANCE hInst, int width, int height, const wchar_t* title);
	int Run();
	HWND GetHandle() const { return m_hwnd; }

private:
	HWND m_hwnd = nullptr;
	int m_width = 0;
	int m_height = 0;

	XMFLOAT3 m_camPos = { 0.0f,0.0f,-2.0f };
	float m_camYaw = 0.0f;
	float m_camPitch = 0.0f;

	Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cb;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthTex;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	bool InitD3D();
	void Render();
	bool InitTriangle();
	void UpdateCamera();
	XMMATRIX GetViewMatrix() const;
	void DrawTriangle(const XMMATRIX& W, const XMMATRIX& V, const XMMATRIX& P);
};