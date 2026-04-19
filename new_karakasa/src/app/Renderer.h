#pragma once
#include "Scene.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

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
	float pad1;
};

struct CBMaterial
{
	DirectX::XMFLOAT4 baseColor;
	float specStrength;
	float shininess;
	DirectX::XMFLOAT2 pad;
};

class Renderer
{
public:
	bool Initialize(ID3D11Device* device);
	void Render(
		ID3D11DeviceContext* context,
		const Scene& scene,
		ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* dev,
		int width,
		int height);

private:
	void DrawRenderItem(
		ID3D11DeviceContext* context,
		const RenderItem& item,
		const DirectX::XMMATRIX& V,
		const DirectX::XMMATRIX& P);

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbTransform;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbLight;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbMaterial;
};