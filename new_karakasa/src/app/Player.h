#pragma once
#include "RenderItem.h"
#include "Camera.h"
#include <DirectXMath.h>

class Player
{
public:
	RenderItem renderItem;
	DirectX::XMFLOAT3 velocity = { 0.0f,0.0f,0.0f };

	bool onGround = false;

	void Update(float dt, const Camera& cam);
	DirectX::XMFLOAT3 GetMoveDelta(float dt, const Camera& cam) const;
	void UpdateVertical(float dt);

};

