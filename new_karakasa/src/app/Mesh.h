#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class Mesh
{
public:
	struct Vertex
	{
		float x, y, z;
		float nx, ny, nz;
		float r, g, b, a;
	};

	bool CreateTriangle(ID3D11Device* device);
	bool CreateBox(ID3D11Device* device);
	void Draw(ID3D11DeviceContext* context)const;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb;
	UINT m_vertexCount = 0;
};