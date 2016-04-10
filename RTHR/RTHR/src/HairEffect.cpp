//#include "pch.h"
//#include "HairEffect.h"
//
//using namespace DirectX;
//
//struct HairEffectConstants
//{
//	XMVECTOR diffuseColor;
//	XMVECTOR emissiveColor;
//	XMVECTOR specularColorAndPower;
//
//	XMVECTOR lightDirection[IEffectLights::MaxDirectionalLights];
//	XMVECTOR lightDiffuseColor[IEffectLights::MaxDirectionalLights];
//	XMVECTOR lightSpecularColor[IEffectLights::MaxDirectionalLights];
//
//	XMVECTOR eyePosition;
//
//	XMMATRIX world;
//	XMVECTOR worldInverseTranspose[3];
//	XMMATRIX worldViewProj;
//};
//
//static_assert((sizeof(HairEffectConstants) % 16) == 0, "CB size not padded correctly");
//
//
//struct HairEffectTraits
//{
//	typedef HairEffectConstants ConstantBufferType;
//
//	static const int VertexShaderCount = 0;
//	static const int PixelShaderCount = 0;
//	static const int ShaderPermutationCount = 0;
//};
//
//
//class HairEffect::Impl : public EffectBase<HairEffectTraits>
//{
//public:
//	Impl(_In_ ID3D11Device* device);
//
//	bool vertexColorEnabled;
//	bool lightingEnabled;
//	bool textureEnabled;
//	bool preferPerPixelLighting;
//
//	EffectLights lights;
//
//	int GetCurrentShaderPermutation() const;
//
//	void Apply(_In_ ID3D11DeviceContext* deviceContext);
//};
//
//HairEffect::Impl::Impl(_In_ ID3D11Device* device)
//	: EffectBase(device),
//	vertexColorEnabled(false), 
//	preferPerPixelLighting(false),
//	textureEnabled(false),
//	lightingEnabled(false)
//{
////	static_assert(_countof(EffectBase<HairEffectTraits>::VertexShaderIndices) == HairEffectTraits::ShaderPermutationCount, "array/max mismatch");
////	static_assert(_countof(EffectBase<HairEffectTraits>::VertexShaderBytecode) == HairEffectTraits::VertexShaderCount, "array/max mismatch");
////	static_assert(_countof(EffectBase<HairEffectTraits>::PixelShaderBytecode) == HairEffectTraits::PixelShaderCount, "array/max mismatch");
////	static_assert(_countof(EffectBase<HairEffectTraits>::PixelShaderIndices) == HairEffectTraits::ShaderPermutationCount, "array/max mismatch");
//
//	lights.InitializeConstants(constants.specularColorAndPower, constants.lightDirection, constants.lightDiffuseColor, constants.lightSpecularColor);
//}
//
//int HairEffect::Impl::GetCurrentShaderPermutation() const
//{
//	int permutation = 0;
//
//	if (vertexColorEnabled)
//	{
//		throw new std::exception("This shader has not been implemented");
//	}
//
//	if (textureEnabled)
//	{
//		throw new std::exception("This shader has not been implemented");
//	}
//
//	if (lightingEnabled)
//	{
//		if (preferPerPixelLighting)
//		{
//			throw new std::exception("This shader has not been implemented");
//		}
//		else if (!lights.lightEnabled[1] && !lights.lightEnabled[2])
//		{
//			throw new std::exception("This shader has not been implemented");
//		}
//		else
//		{
//			throw new std::exception("This shader has not been implemented");
//		}
//	}
//		
//	return permutation;
//}
//
//
//// Sets the program state into the D3D device
//void HairEffect::Impl::Apply(_In_ ID3D11DeviceContext* deviceContext)
//{
//	matrices.SetConstants(dirtyFlags, constants.worldViewProj);
//
//	lights.SetConstants(dirtyFlags, matrices, constants.world, constants.worldInverseTranspose, constants.eyePosition, constants.diffuseColor, constants.emissiveColor, lightingEnabled);
//
//	if (textureEnabled)
//	{
//		ID3D11ShaderResourceView* textures[1] = { texture.Get() };
//
//		deviceContext->PSSetShaderResources(0, 1, textures);
//	}
//}
//
///**********************************************************************/
///*Constructors*********************************************************/
///**********************************************************************/
////Public constructor
//HairEffect::HairEffect(_In_ ID3D11Device* device)
//	: pImpl(new Impl(device))
//{
//}
//
//// Move constructor
//HairEffect::HairEffect(HairEffect&& moveFrom)
//	: pImpl(std::move(moveFrom.pImpl))
//{
//}
//
//// Move assignment
//HairEffect& HairEffect::operator= (HairEffect&& moveFrom)
//{
//	pImpl = std::move(moveFrom.pImpl);
//	return *this;
//}
//
//// Public destructor
//HairEffect::~HairEffect()
//{
//}
//
//void HairEffect::Apply(_In_ ID3D11DeviceContext* deviceContext)
//{
//	pImpl->Apply(deviceContext);
//
//	ApplyShaders(deviceContext);
//}
//
//void HairEffect::GetVertexShaderBytecode(_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
//{
//	
//}
//
//void XM_CALLCONV HairEffect::SetWorld(FXMMATRIX value)
//{
//	pImpl->matrices.world = value;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::FogVector;
//}
//
//void XM_CALLCONV HairEffect::SetView(FXMMATRIX value)
//{
//	pImpl->matrices.view = value;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
//}
//
//void XM_CALLCONV HairEffect::SetProjection(FXMMATRIX value)
//{
//	pImpl->matrices.projection = value;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj;
//}
//
//void XM_CALLCONV HairEffect::SetDiffuseColor(FXMVECTOR value)
//{
//	pImpl->lights.diffuseColor = value;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
//}
//
//void XM_CALLCONV HairEffect::SetEmissiveColor(FXMVECTOR value)
//{
//	pImpl->lights.emissiveColor = value;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
//}
//
//void XM_CALLCONV HairEffect::SetSpecularColor(FXMVECTOR value)
//{
//	// Set xyz to new value, but preserve existing w (specular power)
//	pImpl->constants.specularColorAndPower = XMVectorSelect(pImpl->constants.specularColorAndPower, value, g_XMSelect1110);
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
//}
//
//void HairEffect::SetSpecularPower(float value)
//{
//	// Set w to new value, but preserve existing specular color
//	pImpl->constants.specularColorAndPower = XMVectorSetW(pImpl->constants.specularColorAndPower, value);
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
//}
//
//void HairEffect::DisableSpecular()
//{
//	// Set specular color to black
//
//	pImpl->constants.specularColorAndPower = g_XMIdentityR3;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
//}
//
//void HairEffect::SetAlpha(float value)
//{
//	pImpl->lights.alpha = value;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
//}
//
//void HairEffect::SetLightingEnabled(bool value)
//{
//	pImpl->lightingEnabled = value;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
//}
//
//void HairEffect::SetPerPixelLighting(bool value)
//{
//	pImpl->preferPerPixelLighting = value;
//}
//
//void XM_CALLCONV HairEffect::SetAmbientLightColor(FXMVECTOR value)
//{
//	pImpl->lights.ambientLightColor = value;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
//}
//
//void HairEffect::SetLightEnabled(int whichLight, bool value)
//{
//	pImpl->dirtyFlags |= pImpl->lights.SetLightEnabled(whichLight, value, pImpl->constants.lightDiffuseColor, pImpl->constants.lightSpecularColor);
//}
//
//void XM_CALLCONV HairEffect::SetLightDirection(int whichLight, FXMVECTOR value)
//{
//	EffectLights::ValidateLightIndex(whichLight);
//
//	pImpl->constants.lightDirection[whichLight] = value;
//
//	pImpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
//}
//
//void XM_CALLCONV HairEffect::SetLightDiffuseColor(int whichLight, FXMVECTOR value)
//{
//	pImpl->dirtyFlags |= pImpl->lights.SetLightDiffuseColor(whichLight, value, pImpl->constants.lightDiffuseColor);
//}
//
//void XM_CALLCONV HairEffect::SetLightSpecularColor(int whichLight, FXMVECTOR value)
//{
//	pImpl->dirtyFlags |= pImpl->lights.SetLightSpecularColor(whichLight, value, pImpl->constants.lightSpecularColor);
//}
//
//void HairEffect::EnableDefaultLighting()
//{
//	EffectLights::EnableDefaultLighting(this);
//}
//
//void HairEffect::SetVertexColorEnabled(bool value)
//{
//	pImpl->vertexColorEnabled = value;
//}
//
//void HairEffect::SetTextureEnabled(bool value)
//{
//	pImpl->textureEnabled = value;
//}
//
//void HairEffect::SetTexture(_In_opt_ ID3D11ShaderResourceView* value)
//{
//	pImpl->texture = value;
//}
//
//void HairEffect::ApplyShaders(_In_ ID3D11DeviceContext* context)
//{
//	auto vertexShader = GetVertexShader();
//	auto pixelShader = GetPixelShader();
//	auto computeShader = GetComputeShader();
//	auto geometryShader = GetGeometryShader();
//}
//
//ID3D11VertexShader* HairEffect::InitVertexShader()
//{
//	if (!strandsVS)
//	{
//		auto loadVSTask = DX::ReadDataAsync(VSPath);
//		
//		auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
//		{
//			DX::ThrowIfFailed(
//				device->CreateVertexShader(
//					&fileData[0],
//					fileData.size(),
//					nullptr,
//					&strandsVS
//				)
//			);
//
//			DX::ThrowIfFailed(
//				device->CreateInputLayout(
//					VertexPositionNormColor::InputElements,
//					VertexPositionNormColor::InputElementsCount,
//					&fileData[0],
//					fileData.size(),
//					&inputLayout
//				)
//			)
//		});
//	}
//	return strandsVS.Get();
//}
//
//ID3D11PixelShader* HairEffect::GetPixelShader()
//{
//	return strandsPS.Get();
//}
//
//ID3D11GeometryShader* HairEffect::GetGeometryShader()
//{
//	return strandsGS.Get();
//}
//
//ID3D11ComputeShader* HairEffect::GetComputeShader()
//{
//	return strandsCS.Get();
//}
//
//const int EffectBase<HairEffectTraits>::VertexShaderIndices[] =
//{
//	0, // basic
//	//1, // vertex color
//	//2, // texture
//	//3, // texture + vertex color
//};