#include "Player.h"
#include "Input.h"

using namespace DirectX;

void Player::Update(float dt)
{
	float moveSpeed = 3.0f;

	if (Input::GetKey('A'))
		renderItem.transform.position.x -= moveSpeed * dt;

	if (Input::GetKey('D'))
		renderItem.transform.position.x += moveSpeed * dt;

	//ジャンプ
	if (Input::GetKeyDown(VK_SPACE) && onGround)
	{
		velocity.y = 5.0f;
		onGround = false;
	}

	//重力
	velocity.y -= 9.8f * dt;

	renderItem.transform.position.y += velocity.y + dt;

	// 仮の地面
	if (renderItem.transform.position.y < 0.5f)
	{
		renderItem.transform.position.y = 0.5f;
		velocity.y = 0.0f;
		onGround = true;
	}
}
