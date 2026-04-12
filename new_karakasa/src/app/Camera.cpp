#include "Camera.h"
#include "Input.h"
#include <Windows.h>

using namespace DirectX;

void Camera::UpdateRotation()
{
	float rotSpeed = 0.02f;

	if (Input::GetKey(VK_LEFT)) m_yaw -= rotSpeed;
	if (Input::GetKey(VK_RIGHT)) m_yaw += rotSpeed;
	if (Input::GetKey(VK_UP)) m_pitch += rotSpeed;
	if (Input::GetKey(VK_DOWN)) m_pitch -= rotSpeed;

	const float limit = XM_PIDIV2 - 0.1f;
	if (m_pitch > limit) m_pitch = limit;
	if (m_pitch < -limit) m_pitch = -limit;

}

void Camera::Follow(const DirectX::XMFLOAT3& target, float distance, float height)
{
	float x = target.x - cosf(m_pitch) * sinf(m_yaw) * distance;
	float y = target.y - sinf(m_pitch) * distance + height;
	float z = target.z - cosf(m_pitch) * cosf(m_yaw) * distance;

	m_position = { x,y,z };

}

XMMATRIX Camera::GetViewMatrix() const
{
	XMVECTOR pos = XMLoadFloat3(&m_position);

	XMVECTOR forward = XMVector3Normalize(XMVectorSet(
		cosf(m_pitch) * sinf(m_yaw),
		sinf(m_pitch),
		cosf(m_pitch) * cosf(m_yaw),
		0.0f
	));

	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	return XMMatrixLookToLH(pos, forward, up);
}

DirectX::XMVECTOR Camera::GetForward() const
{
	return XMVector3Normalize(
		XMVectorSet(
			cosf(m_pitch) * sinf(m_yaw),
			0.0f,
			cosf(m_pitch) * cosf(m_yaw),
			0.0f
		)
	);
}

DirectX::XMVECTOR Camera::GetRight() const
{
	XMVECTOR forward = GetForward();
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	return XMVector3Normalize(XMVector3Cross(up, forward));
}


