#pragma once
#include "pch.h"
#include "SimpleMath.h"
#include <string>
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace RTHR
{
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

	// Structure used to define the coordinate frame of the hair
	struct coordFrame
	{
		Vector3 x;
		Vector3 y;
		Vector3 z;
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

	enum RENDERTYPE
	{
		INSTANCED_DEPTH,
		INSTANCED_DENSITY,
		INSTANCED_NORMAL_HAIR,
		INSTANCED_INTERPOLATED_COLLISION,
		INSTANCED_HAIR_DEPTHPASS,
		INSTANCED_COLLISION_RESULTS,
		SOATTRIBUTES,
		INSTANCED_DEPTH_DOM,
	};

	enum INTERP_MODEL
	{
		HYBRID,
		MULTI_HYBRID,
		MULTISTRAND,
		SINGLESTRAND,
		NO_HAIR,
		NUM_INTERPOLATE_MODELS
	};

	enum HairStyle
	{
		SHORT,
		MEDIUM,
		LONG,
		NONE,
	};

	// The hair class that allows for the additon of a hair scalp to the scene with realistic hair
	// simulated
	class Hair
	{
	public:
		// Width and height of the hair 
		int width;
		int height;

		// Count of wisps
		int wispCount;

		// List of the guideStrand hair vertexes
		std::vector<HairVertex*> guideStrands;

		// List of indices in the mesh
		int* indices;

		// Loads the hair data from the provided .txt file
		bool loadHairVertex(string dir, HairStyle style);
	};
}