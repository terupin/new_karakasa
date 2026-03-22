#include "Camera.h"
#include "Input.h"
#include <Windows.h>

using namespace DirectX;

void Camera::Update()
{
	float moveSpeed = 0.03f;
	float rotSpeed = 0.02f;

	if (Input::GetKey(VK_LEFT)) m_yaw -= rotSpeed;
	if (Input::GetKey(VK_RIGHT)) m_yaw += rotSpeed;
	if (Input::GetKey(VK_UP)) m_pitch += rotSpeed;
	if (Input::GetKey(VK_DOWN)) m_pitch -= rotSpeed;

	const float limit = XM_PIDIV2 - 0.1f;
	if (m_pitch > limit) m_pitch = limit;
	if (m_pitch < -limit) m_pitch = -limit;

	XMVECTOR forward = XMVectorSet(
		cosf(m_pitch) * sinf(m_yaw),
		sinf(m_pitch),
		cosf(m_pitch) * cosf(m_yaw),
		0.0f
	);

	forward = XMVector3Normalize(forward);

	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));

	XMVECTOR pos = XMLoadFloat3(&m_position);

	if (Input::GetKey('W')) pos += forward * moveSpeed;
	if (Input::GetKey('S')) pos -= forward * moveSpeed;
	if (Input::GetKey('D')) pos += right * moveSpeed;
	if (Input::GetKey('A')) pos -= right * moveSpeed;

	if (Input::GetKey('E')) pos += up * moveSpeed;
	if (Input::GetKey('Q')) pos -= up * moveSpeed;

	XMStoreFloat3(&m_position, pos);
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




