#pragma once
#include<DirectXMath.h>

class Camera
{
public:
	void Update();

	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMFLOAT3 GetPosition() const { return m_position; }

private:
	DirectX::XMFLOAT3 m_position = { 0.0f,0.0f,-3.0f };

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
};