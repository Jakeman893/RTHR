#pragma once
#include "pch.h"
#include "SimpleMath.h"
#include <string>
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Hair
{
protected:
	static int width;
	static int height;

	// A structure that simply defines the position of a vertex in the hair
	struct HairVertex
	{
		Vector4 position;
	};

	// This structure is used to define the position, normal, and tangent to a particular strand's root
	// this is useful for extruding hair out of vertices from a particular "root"
	struct StrandRoot
	{
		Vector3 position;
		Vector3 tangent;
		Vector3 normal;
		Vector2 textCoord;
	};

	// The type of interpolation that is used when multiplying hair strands across the triangles
	enum INTERPOLATION_TYPE
	{
		HYBRID,
		MULTI_HYBRID,
		MULTISTRAND,
		SINGLESTRAND,
		NO_HAIR,

		NUM_INTERPOLATE_MODELS
	};
};