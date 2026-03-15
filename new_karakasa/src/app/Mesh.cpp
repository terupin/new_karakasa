#include "Mesh.h"
#include <iterator>

bool Mesh::CreateTriangle(ID3D11Device* device)
{
	Vertex verts[3] =
	{
		{  0.0f,  0.5f, 0.0f,   0,0,-1,   1,0,0,1 },
		{  0.5f, -0.5f, 0.0f,   0,0,-1,   0,1,0,1 },
		{ -0.5f, -0.5f, 0.0f,   0,0,-1,   0,0,1,1 },
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(verts);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initDate = {};
	initDate.pSysMem = verts;

	HRESULT hr = device->CreateBuffer(&bd, &initDate, m_vb.GetAddressOf());
	if (FAILED(hr))return false;

	m_vertexCount = 3;
	return true;
}

bool Mesh::CreateBox(ID3D11Device* device)
{
	Vertex verts[] =
	{
		// Front (+Z)
		{ -0.5f, -0.5f,  0.5f,   0, 0, 1,   1,0,0,1 },
		{ -0.5f,  0.5f,  0.5f,   0, 0, 1,   1,0,0,1 },
		{  0.5f,  0.5f,  0.5f,   0, 0, 1,   1,0,0,1 },

		{ -0.5f, -0.5f,  0.5f,   0, 0, 1,   1,0,0,1 },
		{  0.5f,  0.5f,  0.5f,   0, 0, 1,   1,0,0,1 },
		{  0.5f, -0.5f,  0.5f,   0, 0, 1,   1,0,0,1 },

		// Back (-Z)
		{  0.5f, -0.5f, -0.5f,   0, 0,-1,   0,1,0,1 },
		{  0.5f,  0.5f, -0.5f,   0, 0,-1,   0,1,0,1 },
		{ -0.5f,  0.5f, -0.5f,   0, 0,-1,   0,1,0,1 },

		{  0.5f, -0.5f, -0.5f,   0, 0,-1,   0,1,0,1 },
		{ -0.5f,  0.5f, -0.5f,   0, 0,-1,   0,1,0,1 },
		{ -0.5f, -0.5f, -0.5f,   0, 0,-1,   0,1,0,1 },

		// Left (-X)
		{ -0.5f, -0.5f, -0.5f,  -1, 0, 0,   0,0,1,1 },
		{ -0.5f,  0.5f, -0.5f,  -1, 0, 0,   0,0,1,1 },
		{ -0.5f,  0.5f,  0.5f,  -1, 0, 0,   0,0,1,1 },

		{ -0.5f, -0.5f, -0.5f,  -1, 0, 0,   0,0,1,1 },
		{ -0.5f,  0.5f,  0.5f,  -1, 0, 0,   0,0,1,1 },
		{ -0.5f, -0.5f,  0.5f,  -1, 0, 0,   0,0,1,1 },

		// Right (+X)
		{  0.5f, -0.5f,  0.5f,   1, 0, 0,   1,1,0,1 },
		{  0.5f,  0.5f,  0.5f,   1, 0, 0,   1,1,0,1 },
		{  0.5f,  0.5f, -0.5f,   1, 0, 0,   1,1,0,1 },

		{  0.5f, -0.5f,  0.5f,   1, 0, 0,   1,1,0,1 },
		{  0.5f,  0.5f, -0.5f,   1, 0, 0,   1,1,0,1 },
		{  0.5f, -0.5f, -0.5f,   1, 0, 0,   1,1,0,1 },

		// Top (+Y)
		{ -0.5f,  0.5f,  0.5f,   0, 1, 0,   1,0,1,1 },
		{ -0.5f,  0.5f, -0.5f,   0, 1, 0,   1,0,1,1 },
		{  0.5f,  0.5f, -0.5f,   0, 1, 0,   1,0,1,1 },

		{ -0.5f,  0.5f,  0.5f,   0, 1, 0,   1,0,1,1 },
		{  0.5f,  0.5f, -0.5f,   0, 1, 0,   1,0,1,1 },
		{  0.5f,  0.5f,  0.5f,   0, 1, 0,   1,0,1,1 },

		// Bottom (-Y)
		{ -0.5f, -0.5f, -0.5f,   0,-1, 0,   0,1,1,1 },
		{ -0.5f, -0.5f,  0.5f,   0,-1, 0,   0,1,1,1 },
		{  0.5f, -0.5f,  0.5f,   0,-1, 0,   0,1,1,1 },

		{ -0.5f, -0.5f, -0.5f,   0,-1, 0,   0,1,1,1 },
		{  0.5f, -0.5f,  0.5f,   0,-1, 0,   0,1,1,1 },
		{  0.5f, -0.5f, -0.5f,   0,-1, 0,   0,1,1,1 },
	};

	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(verts);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = verts;

	HRESULT hr = device->CreateBuffer(&bd, &initData, m_vb.GetAddressOf());
	if (FAILED(hr)) return false;

	m_vertexCount = static_cast<UINT>(std::size(verts));
	return true;
}

void Mesh::Draw(ID3D11DeviceContext* context) const
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* vbs[] = { m_vb.Get() };

	context->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
	context->Draw(m_vertexCount, 0);
}