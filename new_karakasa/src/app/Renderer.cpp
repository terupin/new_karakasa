#include "Renderer.h"
#include <d3Dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using Microsoft::WRL::ComPtr;

bool Renderer::Initialize(ID3D11Device* device)
{
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
		psBlob.GetAddressOf(),
		errBlob.GetAddressOf()
		):
		if (FAILED(hr)) return false;

	hr = device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		nullptr,
		m_ps.GetAddressOf());
	if (FAILED(hr)) return false;

	D3D11_BUFFER_DESC cbd{};
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	cbd.ByteWidth = sizeof(CBTransform);
	hr = device->CreateBuffer(&cbd, nullptr, m_cbTransform.GetAddressOf());
	if (FAILED(hr)) return false;

	cbd.ByteWidth = sizeof(CBLight);
	hr = device->CreateBuffer(&cbd, nullptr, m_cbLight.GetAddressOf());
	if (FAILED(hr)) return false;

	cbd.ByteWidth = sizeof(CBMaterial);
	hr = device->CreateBuffer(&cbd, nullptr, m_cbMaterial.GetAddressOf());
	if (FAILED(hr)) return false;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		layout,
		(UINT)_countof(layout),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		m_inputLayout.GetAddressOf());
	if (FAILED(hr)) return false;

	return true
}

void Renderer::Render(ID3D11DeviceContext* context, const Scene& scene, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dev, int width, int height)
{
	const float clearColor[4] = { 0.1f, 0.2f, 0.8f, 1.0f };
	context->ClearRenderTargetView(rtv, clearColor);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(m_vs.Get(), nullptr, 0);
	context->PSSetShader(m_ps.Get(), nullptr, 0);

	XMMATRIX V = scene.camera.GetViewMatrix();

	float fovY = XM_PIDIV4;
	float aspect = (float)width / (float)height;
	XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspect, 0.1f, 100.0f);

	CBLight cbL{};
	cbL.lightDir = XMFLOAT3(0.3f, -1.0f, 0.5f);
	cbL.lightColor = XMFLOAT4(1.2f, 1.2f, 1.2f, 1.0f);
	cbL.ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	cbL.cameraPos = scene.camera.GetPosition();
	context->UpdateSubresource(m_cbLight.Get(), 0, nullptr, &cbL, 0, 0);

	ID3D11Buffer* vsCBs[] = { m_cbTransform.Get() };
	context->VSGetConstantBuffers(0, 1, vsCBs);

	ID3D11Buffer* psCBs[] =
	{
		m_cbTransform.Get(),
		m_cbLight.Get(),
		m_cbMaterial.Get()
	};

	context->PSSetConstantBuffers(0, 3, psCBs);

	static float angle = 0.01f;
	angle += 0.01f;

	if (scene.items.size() > 0)
	{
		const_cast<RenderItem&>(scene.items[0]).transform.rotation.y = angle;
	}
	if (scene.items.size() > 1)
	{
		const_cast<RenderItem&>(scene.items[1]).transform.rotation.y = angle;
	}

	for (const auto& item : scene.items)
	{
		DrawRenderItem(context, item, V, P);
	}

	DrawRenderItem(context, scene.player.renderItem, V, P);
}

void Renderer::DrawRenderItem(ID3D11DeviceContext* context, const RenderItem& item, const DirectX::XMMATRIX& V, const DirectX::XMMATRIX& P)
{
	if (!item.visible || item.mesh == nullptr)
		return;

	CBTransform cbT{};
	XMStoreFloat4x4(&cbT.world, XMMatrixTranspose(item.transform.GetMatrix()));
	XMStoreFloat4x4(&cbT.viewProj, XMMatrixTranspose(V * P));
	context->UpdateSubresource(m_cbTransform.Get(), 0, nullptr, &cbM, 0, 0);

	CBMaterial cbM{};
	cbM.baseColor = item.material.baseColor;
	cbM.specStrength = item.material.specStrength;
	cbM.shininess = item.material.shininess;
	context->UpdateSubresource(m_cbMaterial.Get(), 0, nullptr, &cbM, 0, 0);

	item.mesh->Draw(context);
}
