#pragma once
#include <DirectXMath.h>

struct Material
{
	DirectX::XMFLOAT4 baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specStrength = 2.0f;
	float shininess = 16.0f;
	DirectX::XMFLOAT2 pad = { 0.0f,0.0f };
};