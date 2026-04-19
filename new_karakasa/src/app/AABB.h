#pragma once
#include "Transform.h"
#include <DirectXMath.h>

struct AABB
{
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
};

inline bool Intersects(const AABB& a, const AABB& b)
{
	if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
	if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
	if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
	return true;
}

inline AABB MakeAABB(const Transform& t)
{
	DirectX::XMFLOAT3 half =
	{
		t.scale.x * 0.5f,
		t.scale.y * 0.5f,
		t.scale.z * 0.5f
	};

	AABB box;
	box.min = {
		t.position.x - half.x,
		t.position.y - half.y + 0.1f,
		t.position.z - half.z
	};

	box.max = {
		t.position.x + half.x,
		t.position.y + half.y - 0.1f,
		t.position.z + half.z
	};

	return box;
}

inline bool OverlapsY(const AABB& a, const AABB& b)
{
	if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
	return true;
}

inline bool OverlapsXZ(const AABB& a, const AABB& b)
{
	if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
	if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
	return true;
}


