#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"

struct RenderItem
{
	Mesh* mesh = nullptr;
	Transform transform;
	Material material;
	bool visible = true;
};