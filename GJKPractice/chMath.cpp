#include "chMath.h"
#include "chLiteBody.h"

void Chan::makeAABB(const chLiteBody* body, c2AABB* out)
{
	c2AABB aabb;
	aabb.min = ChVector2(Chreal_max);
	aabb.max = ChVector2(-Chreal_max);

	ChVector2 halfExtents = body->width * ChReal(0.5);
	ChVector2 v[4] =
	{
		ChVector2(halfExtents.x, halfExtents.y),
		ChVector2(-halfExtents.x, halfExtents.y),
		ChVector2(-halfExtents.x, -halfExtents.y),
		ChVector2(halfExtents.x, -halfExtents.y)
	};

	ChMat22 Rot(body->rotation);

	for (unsigned i = 0; i < 4; ++i)
	{
		v[i] = body->position + Rot * v[i];
		aabb.min = Min(aabb.min, v[i]);
		aabb.max = Max(aabb.max, v[i]);
	}

	*out = aabb;
}