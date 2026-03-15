#pragma once
#define NOMINMAX
#include "Windows.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h"
#include <d3d11.h>
#include <cstdint>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


#pragma comment(lib,"d3dcompiler.lib")

struct CBTransform
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 viewProj;
};

struct CBLight
{
	DirectX::XMFLOAT3 lightDir;
	float pad0;

	DirectX::XMFLOAT4 lightColor;
	DirectX::XMFLOAT4 ambient;

	DirectX::XMFLOAT3 cameraPos;
	float specStrength;

	float shininess;
	DirectX::XMFLOAT3 pad1; //16ÉoÉCÉgã´äEçáÇÌÇπ
};

struct RenderItem
{
	Mesh* mesh = nullptr;
	Transform transform;
};

class Window {
public:
	bool Create(HINSTANCE hInst, int width, int height, const wchar_t* title);
	int Run();
	HWND GetHandle() const { return m_hwnd; }

private:
	HWND m_hwnd = nullptr;
	int m_width = 0;
	int m_height = 0;

	Camera m_camera;
	Mesh m_triangleMesh;
	Mesh m_boxMesh;

	RenderItem m_obj1;
	RenderItem m_obj2;

	Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthTex;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbTransform;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbLight;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	bool InitD3D();
	void Render();
	bool InitResources();
	void DrawRenderItem(const RenderItem& item,
		const DirectX::XMMATRIX& V,
		const DirectX::XMMATRIX& P);
};