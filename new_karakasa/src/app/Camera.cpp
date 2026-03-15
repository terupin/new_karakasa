#include "Camera.h"
#include <Windows.h>

using namespace DirectX;

void Camera::Update()
{
	float moveSpeed = 0.03f;
	float rotSpeed = 0.02f;

	if (GetAsyncKeyState(VK_LEFT) & 0x8000) m_yaw -= rotSpeed;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) m_yaw += rotSpeed;
	if (GetAsyncKeyState(VK_UP) & 0x8000) m_pitch += rotSpeed;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) m_pitch -= rotSpeed;

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

	if (GetAsyncKeyState('W') & 0x8000) pos += forward * moveSpeed;
	if (GetAsyncKeyState('S') & 0x8000) pos -= forward * moveSpeed;
	if (GetAsyncKeyState('D') & 0x8000) pos += right * moveSpeed;
	if (GetAsyncKeyState('A') & 0x8000) pos -= right * moveSpeed;

	if (GetAsyncKeyState('E') & 0x8000) pos += up * moveSpeed;
	if (GetAsyncKeyState('Q') & 0x8000) pos -= up * moveSpeed;

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




