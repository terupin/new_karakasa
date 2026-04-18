#pragma once
#include "RenderItem.h"
#include "Player.h"
#include "Camera.h"
#include <vector>

class Scene
{
public:
	Camera camera;
	Player player;
	std::vector<RenderItem> items;

	void Create(Mesh* triangleMesh, Mesh* boxMesh);
	void Update(float dt);
};