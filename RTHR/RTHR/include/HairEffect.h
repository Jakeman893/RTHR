//#pragma once
//#include "pch.h"
//#include "Effects.h"
//#include "EffectCommon.h"
//#include "Common\DirectXHelper.h"
//
//using std::wstring;
//
////----------------------------------------------------------------------------------
//// Built-in shader supports optional texture mapping, vertex coloring, directional lighting, and fog.
//class HairEffect : public IEffect, public IEffectMatrices, public IEffectLights, public IEffectFog
//{
//public:
//	explicit HairEffect(_In_ ID3D11Device* device);
//	HairEffect(HairEffect&& moveFrom);
//	HairEffect& operator= (HairEffect&& moveFrom);
//	virtual ~HairEffect();
//
//	// IEffect methods.
//	void __cdecl Apply(_In_ ID3D11DeviceContext* deviceContext) override;
//
//	// Methods by which to retrieve bytecode
//	void __cdecl GetVertexShaderBytecode(_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength) override;
//
//	// Camera settings.
//	void XM_CALLCONV SetWorld(FXMMATRIX value) override;
//	void XM_CALLCONV SetView(FXMMATRIX value) override;
//	void XM_CALLCONV SetProjection(FXMMATRIX value) override;
//
//	// Material settings.
//	void XM_CALLCONV SetDiffuseColor(FXMVECTOR value);
//	void XM_CALLCONV SetEmissiveColor(FXMVECTOR value);
//	void XM_CALLCONV SetSpecularColor(FXMVECTOR value);
//	void __cdecl SetSpecularPower(float value);
//	void __cdecl DisableSpecular();
//	void __cdecl SetAlpha(float value);
//
//	// Light settings.
//	void __cdecl SetLightingEnabled(bool value) override;
//	void __cdecl SetPerPixelLighting(bool value) override;
//	void XM_CALLCONV SetAmbientLightColor(FXMVECTOR value) override;
//
//	void __cdecl SetLightEnabled(int whichLight, bool value) override;
//	void XM_CALLCONV SetLightDirection(int whichLight, FXMVECTOR value) override;
//	void XM_CALLCONV SetLightDiffuseColor(int whichLight, FXMVECTOR value) override;
//	void XM_CALLCONV SetLightSpecularColor(int whichLight, FXMVECTOR value) override;
//
//	void __cdecl EnableDefaultLighting() override;
//
//	// Fog settings.
//	void __cdecl SetFogEnabled(bool value) override;
//	void __cdecl SetFogStart(float value) override;
//	void __cdecl SetFogEnd(float value) override;
//	void XM_CALLCONV SetFogColor(FXMVECTOR value) override;
//
//	// Vertex color setting.
//	void __cdecl SetVertexColorEnabled(bool value);
//
//	// Texture setting.
//	void __cdecl SetTextureEnabled(bool value);
//	void __cdecl SetTexture(_In_opt_ ID3D11ShaderResourceView* value);
//
//	ID3D11VertexShader * InitVertexShader();
//
//	ID3D11PixelShader * InitPixelShader();
//
//	ID3D11GeometryShader * InitGeometryShader();
//
//	ID3D11ComputeShader * InitComputeShader();
//
//private:
//	// Private implementation.
//	class Impl;
//
//	ID3D11Device* device;
//
//	std::unique_ptr<Impl> pImpl;
//
//	// Prevent copying.
//	HairEffect(HairEffect const&) DIRECTX_CTOR_DELETE
//		HairEffect& operator= (HairEffect const&) DIRECTX_CTOR_DELETE
//
//	void ApplyShaders(ID3D11DeviceContext* context);
//
//	/**************************************************************/
//	/*Shader Strings for Loading***********************************/
//	/**************************************************************/
//	const wstring VSPath = L"HairVertex.cso";
//	const wstring PSPath = L"HairPixel.cso";
//	const wstring GSPath = L"GeometryShader.cso";
//	const wstring CSPath = L"ComputeShader.cso";
//	
//	/*************************************************************/
//	/*Shaders*****************************************************/
//	/*************************************************************/
//	static Microsoft::WRL::ComPtr<ID3D11VertexShader> strandsVS;
//	static Microsoft::WRL::ComPtr<ID3D11PixelShader> strandsPS;
//	static Microsoft::WRL::ComPtr<ID3D11GeometryShader> strandsGS;
//	static Microsoft::WRL::ComPtr<ID3D11ComputeShader> strandsCS;
//
//};