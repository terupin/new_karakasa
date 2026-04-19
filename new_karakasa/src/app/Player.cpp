#include "Player.h"
#include "Input.h"

using namespace DirectX;

void Player::Update(float dt, const Camera& cam)
{

}

DirectX::XMFLOAT3 Player::GetMoveDelta(float dt, const Camera& cam) const
{
	float moveSpeed = 15.0f;

	XMVECTOR forward = cam.GetForward();
	XMVECTOR right = cam.GetRight();

	XMVECTOR move = XMVectorZero();

	if (Input::GetKey('W')) move += forward;
	if (Input::GetKey('S')) move -= forward;
	if (Input::GetKey('D')) move += right;
	if (Input::GetKey('A')) move -= right;

	if (!XMVector3Equal(move, XMVectorZero()))
	{
		move = XMVector3Normalize(move) * moveSpeed * dt;
	}

	XMFLOAT3 delta{};
	XMStoreFloat3(&delta, move);
	return delta;
}

void Player::UpdateVertical(float dt)
{
	//ÉWÉÉÉìÉv
	if (Input::GetKeyDown(VK_SPACE) && onGround)
	{
		velocity.y = 5.0f;
		onGround = false;
	}

	//èdóÕ
	velocity.y -= 9.8f * dt;
	renderItem.transform.position.y += velocity.y * dt;
}
