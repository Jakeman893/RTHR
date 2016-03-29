#pragma once
#include "pch.h"
#include "SimpleMath.h"
#include "GeometricPrimitive.h"

#include "Common\DeviceResources.h"
#include "Common\DirectXHelper.h"
#include "DirectXHelpers.h"

#include "Common\ShaderStructures.h"

#include "GeometricTypes.h"
#include "PrimitiveBatch.h"
#include "Effects.h"
#include "CommonStates.h"

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
		void Draw(FXMMATRIX world, CXMMATRIX view, CXMMATRIX proj, 
					FXMVECTOR color = Colors::White, ID3D11ShaderResourceView* texture = (ID3D11ShaderResourceView*) nullptr,
					bool wireframe = false, std::function<void()> setCustomState = nullptr);

		//Creates the resources that the device needs to work
		void CreateDeviceDependentResources();

		// Resets the geometry object
		void Reset();

	private:
		bool initDone = false;

		float size;

		GeometryType type;

		// Width of strand
		uint16 m_width;

		// Length between vertices
		uint16 m_length;

		// Count of wisps
		uint16 m_wispCount;
		
		// Holds the geometric solid that the strands are on
		unique_ptr<GeometricPrimitive> m_geometry;
		
		// Extrudes the hair strands to the specified length over the GeometricPrimitive's vertices
		void genStrands(const shared_ptr<VertexCollection> vert);

		shared_ptr<DX::DeviceResources> m_device;

		// Model View Projection
		ModelViewProjectionConstantBuffer MVP;

		/**************************************************************/
		/*Device Specific Pointers*************************************/
		/**************************************************************/
		//Vertex Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> strandsVB;
		//Index Buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> strandsIB;
		//Model view projection buffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> ModViewProjB;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> strandsVS;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> strandsPS;
		//Microsoft::WRL::ComPtr<ID3D11GeometryShader> strandsGS;
		//Microsoft::WRL::ComPtr<ID3D11ComputeShader> strandsCS;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

		/**************************************************************/
		/*Shader Strings for Loading***********************************/
		/**************************************************************/
		const wstring VS = L"HairVertex.cso";
		const wstring PS = L"HairPixel.cso";
		//const wstring GS = L"GeometryShader.cso";
		//const wstring CS = L"ComputeShader.cso";


		UINT vertexCount;
	};
}