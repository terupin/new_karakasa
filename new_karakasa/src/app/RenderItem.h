#pragma once
#include "Mesh.h"
#include "Transform.h"

struct RenderItem
{
	Mesh* mesh = nullptr;
	Transform transform;
	bool visible = true;
};