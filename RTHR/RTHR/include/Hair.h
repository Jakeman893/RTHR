#pragma once
#include "pch.h"
#include "SimpleMath.h"
#include "VertexTypes.h"
#include "PrimitiveBatch.h"
#include <string>
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace RTHR
{
	// Redefinition for simpler readability
	typedef vector<Vector3> HairStrand;

	// A structure that simply defines the position of a vertex in the hair
	typedef Vector4 HairVertex;

	// This structure is used to define the position, normal, and tangent to a particular strand's root
	// this is useful for extruding hair out of vertices from a particular "root"
	struct StrandRoot
	{
		Vector3 position;
		Vector3 tangent;
		Vector3 normal;
		Vector2 textCoord;
		int length;
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
		HYBRID_TYPE,
		MULTI_HYBRID_TYPE,
		MULTISTRAND_TYPE,
		SINGLESTRAND_TYPE,
		NO_HAIR_TYPE,

		NUM_INTERPOLATE_MODELS_TYPE
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

	// The hair class that allows for the additon of a hair scalp to the scene with realistic hair
	// simulated
	class Hair
	{
	public:
		//Constructors for the hair object
		Hair();
		Hair(wchar_t const* dir, uint16 width, uint16 length);
		
		//Standard getter/setter functions
		int getWidth();
		int getLength();
		int getWispCount();

		// Will need to update constant buffer
		void setWispCount(uint16 count);

		void LoadVertices();

	private:
		// Width of strand
		uint16 width;

		// Length between vertices
		uint16 length;

		// Count of wisps
		uint16 wispCount;

		// List of the guideStrand hair vertexes
		std::vector<HairStrand> guideStrands;

		// The texture coords of each hair strand
		vector<Vector2> hairTexture;

		// Loads the hair data from the provided .txt file
		bool loadHairVertex(wchar_t const* dir);

		//PrimitiveBatch<VertexPositionNormalTangentColorTexture> a;
	};
}