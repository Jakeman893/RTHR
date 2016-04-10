#pragma once
#include "pch.h"
#include "SimpleMath.h"
#include "GeometricPrimitive.h"

#include "Common\DeviceResources.h"
#include "Common\DirectXHelper.h"
#include "DirectXHelpers.h"
#include "EffectCommon.h"

#include "Common\ShaderStructures.h"

#include "GeometricTypes.h"
#include "PrimitiveBatch.h"
#include "HairEffect.h"
#include "CommonStates.h"

#include "ConstantBuffer.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace RTHR
{
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

	struct HairEffectConstants
	{
		Color diffuseColor;
		//XMVECTOR emissiveColor;
		//XMVECTOR specularColorAndPower;

		//XMVECTOR lightDirection[3];
		//XMVECTOR lightDiffuseColor[3];
		//XMVECTOR lightSpecularColor[3];

		Vector3 eyePosition;

		Matrix world;
		XMVECTOR worldInverseTranspose[3];
		Matrix worldViewProj;
	};

	static_assert((sizeof(HairEffectConstants) % 16) == 0, "Constant Buffer must be 16 bit aligned: ");

	// The hair class that allows for the additon of a hair scalp to the scene with realistic hair
	// simulated
	class Hair
	{
	public:
		//Constructors for the hair object
		Hair();
		Hair(GeometryType aType, shared_ptr<DX::DeviceResources> device, float aSize, uint16 width, uint16 length);
		Hair(wchar_t const* dir, uint16 width, uint16 length);
		
		//Standard getter/setter functions
		int getWidth();
		int getLength();
		int getWispCount();

		// Will need to update constant buffer
		void setWispCount(uint16 count);

		// The rendering function to be called whenever a rendering call is made to the app
		void Draw(Matrix world, Matrix view, Matrix proj, Vector3 eye,
					FXMVECTOR color = Colors::White, ID3D11ShaderResourceView* texture = (ID3D11ShaderResourceView*) nullptr,
					bool wireframe = false, std::function<void()> setCustomState = nullptr);

		//Creates the resources that the device needs to work
		void CreateDeviceDependentResources();

		// Resets the geometry object
		void Reset();

	private:
		unique_ptr<CommonStates> states;

		int dirtyFlags;

		bool initDone = false;

		UINT vertexCount;

		// Width of strand
		uint16 m_width;

		// Length between vertices
		uint16 m_length;

		// Count of wisps
		uint16 m_wispCount;
		
		// Extrudes the hair strands to the specified length over the GeometricPrimitive's vertices
		void genStrands(const shared_ptr<VertexCollection> vert);

		/**************************************************************/
		/*Effect Structures********************************************/
		/**************************************************************/
		EffectMatrices matrices;

		EffectLights lights;

		/**************************************************************/
		/*Under hair geometry properties*******************************/
		/**************************************************************/
		float geomSize;
		GeometryType type;
		// Holds the geometric solid that the strands are on
		unique_ptr<GeometricPrimitive> m_geometry;

		/**************************************************************/
		/*Device Specific Pointers*************************************/
		/**************************************************************/
		//The device
		shared_ptr<DX::DeviceResources> m_device;
		//Vertex Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> strandsVB;
		//Index Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> strandsIB;
		// Constant buffer
		unique_ptr<ConstantBuffer<HairEffectConstants>> constBuf;

		/*************************************************************/
		/*Shaders*****************************************************/
		/*************************************************************/
		Microsoft::WRL::ComPtr<ID3D11VertexShader> strandsVS;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> strandsPS;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> strandsGS;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> strandsCS;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

		/**************************************************************/
		/*Shader Strings for Loading***********************************/
		/**************************************************************/
		const wstring VS = L"HairVertex.cso";
		const wstring PS = L"HairPixel.cso";
		const wstring GS = L"GeometryShader.cso";
		const wstring CS = L"ComputeShader.cso";
	};
}