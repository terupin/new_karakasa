#pragma once
#include<DirectXMath.h>

class Camera
{
public:
	void UpdateRotation();
	void Follow(const DirectX::XMFLOAT3& target, float distance, float height);

	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMFLOAT3 GetPosition() const { return m_position; }
	DirectX::XMVECTOR GetForward() const;
	DirectX::XMVECTOR GetRight() const;

private:
	DirectX::XMFLOAT3 m_position = { 0.0f,0.0f,-3.0f };

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
};