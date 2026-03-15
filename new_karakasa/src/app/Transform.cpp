#include "Transform.h"

using namespace DirectX;

XMMATRIX Transform::GetMatrix() const
{
	XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX Rx = XMMatrixRotationX(rotation.x);
	XMMATRIX Ry = XMMatrixRotationY(rotation.y);
	XMMATRIX Rz = XMMatrixRotationZ(rotation.z);
	XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);

	return S * Rx * Ry * Rz * T;
}
