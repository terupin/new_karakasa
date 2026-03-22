#pragma once
#include "RenderItem.h"
#include <DirectXMath.h>

class Player
{
public:
	RenderItem renderItem;
	DirectX::XMFLOAT3 velocity = { 0.0f,0.0f,0.0f };

	bool onGround = false;

	void Update(float dt);
};

