#include "Scene.h"
#include "Input.h"
#include "AABB.h"
#include <cfloat>

using namespace DirectX;

void Scene::Create(Mesh* triangleMesh, Mesh* boxMesh)
{
	items.clear();

	RenderItem obj1;
	obj1.mesh = triangleMesh;
	obj1.transform.position = { -0.8f, 0.0f, 0.6f };
	obj1.material.baseColor = { 1.0f, 0.3f, 0.3f, 1.0f };
	obj1.material.specStrength = 1.0f;
	obj1.material.shininess = 8.0f;
	obj1.solid = false;
	items.push_back(obj1);

	RenderItem obj2;
	obj2.mesh = boxMesh;
	obj2.transform.position = { 0.8f, 0.0f, 0.0f };
	obj2.material.baseColor = { 0.3f, 0.8f, 1.0f, 1.0f };
	obj2.material.specStrength = 2.0f;
	obj2.material.shininess = 32.0f;
	obj2.solid = true;
	items.push_back(obj2);

	RenderItem ground;
	ground.mesh = boxMesh;
	ground.transform.position = { 0.0f, -1.0f, 0.0f };
	ground.transform.scale = { 10.0f, 0.2f, 10.0f };
	ground.material.baseColor = { 0.4f, 0.8f, 0.4f, 1.0f };
	ground.material.specStrength = 0.2f;
	ground.material.shininess = 4.0f;
	ground.solid = true;
	items.push_back(ground);

	player.renderItem.mesh = boxMesh;
	player.renderItem.transform.position = { 0.0f, 0.5f, 0.0f };
	player.renderItem.transform.scale = { 1.0f, 2.0f, 1.0f };
	player.renderItem.material.baseColor = { 1.0f, 1.0f, 0.3f, 1.0f };
	player.renderItem.material.specStrength = 2.5f;
	player.renderItem.material.shininess = 64.0f;
}

void Scene::Update(float dt)
{
	//カメラの回転
	camera.UpdateRotation();

	//移動を取得
	XMFLOAT3 moveDelta = player.GetMoveDelta(dt, camera);

	//移動方向にプレイヤーの向きを向ける
	if (moveDelta.x != 0.0f || moveDelta.z != 0.0f)
	{
		float angle = atan2f(moveDelta.x, moveDelta.z);
		player.renderItem.transform.rotation.y = angle;
	}

	//X方向移動と衝突判定
	float oldX = player.renderItem.transform.position.x;
	player.renderItem.transform.position.x += moveDelta.x;

	AABB playerBox = MakeAABB(player.renderItem.transform);

	for (const auto& item : items)
	{
		if (!item.visible || item.mesh == nullptr || !item.solid)
			continue;

		AABB itemBox = MakeAABB(item.transform);
		if (Intersects(playerBox, itemBox))
		{
			player.renderItem.transform.position.x = oldX;
			break;
		}
	}

	//Z方向移動と衝突判定
	float oldZ = player.renderItem.transform.position.z;
	player.renderItem.transform.position.z += moveDelta.z;

	playerBox = MakeAABB(player.renderItem.transform);

	for (const auto& item : items)
	{
		if (!item.visible || item.mesh == nullptr || !item.solid)
			continue;

		AABB itemBox = MakeAABB(item.transform);

		if (Intersects(playerBox, itemBox))
		{
			player.renderItem.transform.position.z = oldZ;
			break;
		}
	}

	//縦移動
	XMFLOAT3 prevPos = player.renderItem.transform.position;
	player.UpdateVertical(dt);

	bool landed = false;
	float bestGroundTop = -FLT_MAX;

	AABB playerBox = MakeAABB(player.renderItem.transform);

	float playerHalfY = player.renderItem.transform.scale.y * 0.5f;
	float prevBottom = prevPos.y - playerHalfY;
	float currBottom = player.renderItem.transform.position.y - playerHalfY;

	bool falling = (player.velocity.y <= 0.0f);

	for (const auto& item : items)
	{
		if (!item.visible || item.mesh == nullptr || !item.solid)
			continue;

		AABB itemBox = MakeAABB(item.transform);
		float groundTop = itemBox.max.y;
		bool overtlapXZ = OverlapsXZ(playerBox, itemBox);

		if (falling && OverlapsXZ && prevBottom >= groundTop && currBottom <= groundTop)
		{
			if (groundTop > bestGroundTop)
			{
				bestGroundTop = groundTop;
				landed = true;
			}
		}
	}

	if (landed)
	{
		player.renderItem.transform.position.y = bestGroundTop + playerHalfY;
		player.velocity.y = 0.0f;
	}

	player.onGround = landed;

	XMFLOAT3 target = player.renderItem.transform.position;
	target.y += 1.0f;
	camera.Follow(target, 6.0f, 2.0f);

}
