#include "HairEffect.h"
#include "EffectCommon.h"

using namespace DirectX;

namespace RTHR
{

	struct HairEffectConstants
	{
		XMVECTOR diffuseColor;
		XMVECTOR emissiveColor;
		XMVECTOR specularColorAndPower;

		XMVECTOR lightDirection[IEffectLights::MaxDirectionalLights];
		XMVECTOR lightDiffuseColor[IEffectLights::MaxDirectionalLights];
		XMVECTOR lightSpecularColor[IEffectLights::MaxDirectionalLights];

		XMVECTOR eyePosition;

		XMVECTOR fogColor;
		XMVECTOR fogVector;

		XMMATRIX world;
		XMVECTOR worldInverseTranspose[3];
		XMMATRIX worldViewProj;
	};

	static_assert((sizeof(HairEffectConstants) % 16) == 0, "CB size not padded correctly");


	struct HairEffectTraits
	{
		typedef HairEffectConstants ConstantBufferType;

		static const int VertexShaderCount = 20;
		static const int PixelShaderCount = 10;
		static const int ShaderPermutationCount = 32;
	};


	class HairEffect::Impl : public EffectBase<HairEffectTraits>
	{
	public:
		Impl(_In_ ID3D11Device* device);

		bool vertexColorEnabled;
		bool lightingEnabled;
		bool textureEnabled;
		bool preferPerPixelLighting;

		EffectLights lights;
		
		void Apply(_In_ ID3D11DeviceContext* deviceContext);
	};

	HairEffect::Impl::Impl(_In_ ID3D11Device* device)
		: EffectBase(device),
		vertexColorEnabled(false)
	{
		
	}

	void HairEffect::Impl::Apply(_In_ ID3D11DeviceContext* deviceContext)
	{
		matrices.SetConstants(dirtyFlags, constants.worldViewProj);

		lights.SetConstants(dirtyFlags, matrices, constants.world, constants.worldInverseTranspose, constants.eyePosition, constants.diffuseColor, constants.emissiveColor, lightingEnabled);

		if (textureEnabled)
		{
			ID3D11ShaderResourceView* textures[1] = { texture.Get() };

			deviceContext->PSSetShaderResources(0, 1, textures);
		}

		//set shaders and constant buffers
		ApplyShaders(deviceContext, GetCurrentShaderPermutation());
	}

	/**********************************************************************/
	/*Constructors*********************************************************/
	/**********************************************************************/
	//Public constructor
	HairEffect::HairEffect(_In_ ID3D11Device* device)
		: pImpl(new Impl(device))
	{
	}

	// Move constructor
	HairEffect::HairEffect(HairEffect&& moveFrom)
		: pImpl(std::move(moveFrom.pImpl))
	{
	}

	// Move assignment
	HairEffect& HairEffect::operator= (HairEffect&& moveFrom)
	{
		pImpl = std::move(moveFrom.pImpl);
		return *this;
	}

	// Public destructor
	HairEffect::~HairEffect()
	{
	}

	void HairEffect::Apply(_In_ ID3D11DeviceContext* deviceContext)
	{
		pImpl->Apply(deviceContext);
	}

	void HairEffect::GetVertexShaderBytecode(_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
	{
//		pImpl->GetVertexShaderBytecode()
	}

	void XM_CALLCONV HairEffect::SetWorld(FXMMATRIX value)
	{
		pImpl->matrices.world = value;

		pImpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::WorldInverseTranspose | EffectDirtyFlags::FogVector;
	}

	void XM_CALLCONV HairEffect::SetView(FXMMATRIX value)
	{
		pImpl->matrices.view = value;

		pImpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj | EffectDirtyFlags::EyePosition | EffectDirtyFlags::FogVector;
	}

	void XM_CALLCONV HairEffect::SetProjection(FXMMATRIX value)
	{
		pImpl->matrices.projection = value;

		pImpl->dirtyFlags |= EffectDirtyFlags::WorldViewProj;
	}

	void XM_CALLCONV HairEffect::SetDiffuseColor(FXMVECTOR value)
	{
		pImpl->lights.diffuseColor = value;

		pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
	}

	void XM_CALLCONV HairEffect::SetEmissiveColor(FXMVECTOR value)
	{
		pImpl->lights.emissiveColor = value;

		pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
	}

	void XM_CALLCONV HairEffect::SetSpecularColor(FXMVECTOR value)
	{
		// Set xyz to new value, but preserve existing w (specular power)
		pImpl->constants.specularColorAndPower = XMVectorSelect(pImpl->constants.specularColorAndPower, value, g_XMSelect1110);

		pImpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
	}

	void HairEffect::SetSpecularPower(float value)
	{
		// Set w to new value, but preserve existing specular color
		pImpl->constants.specularColorAndPower = XMVectorSetW(pImpl->constants.specularColorAndPower, value);

		pImpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
	}

	void HairEffect::DisableSpecular()
	{
		// Set specular color to black

		pImpl->constants.specularColorAndPower = g_XMIdentityR3;

		pImpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
	}

	void HairEffect::SetAlpha(float value)
	{
		pImpl->lights.alpha = value;

		pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
	}

	void HairEffect::SetLightingEnabled(bool value)
	{
		pImpl->lightingEnabled = value;

		pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
	}

	void HairEffect::SetPerPixelLighting(bool value)
	{
		pImpl->preferPerPixelLighting = value;
	}

	void XM_CALLCONV HairEffect::SetAmbientLightColor(FXMVECTOR value)
	{
		pImpl->lights.ambientLightColor = value;

		pImpl->dirtyFlags |= EffectDirtyFlags::MaterialColor;
	}

	void HairEffect::SetLightEnabled(int whichLight, bool value)
	{
		pImpl->dirtyFlags |= pImpl->lights.SetLightEnabled(whichLight, value, pImpl->constants.lightDiffuseColor, pImpl->constants.lightSpecularColor);
	}

	void XM_CALLCONV HairEffect::SetLightDirection(int whichLight, FXMVECTOR value)
	{
		EffectLights::ValidateLightIndex(whichLight);

		pImpl->constants.lightDirection[whichLight] = value;

		pImpl->dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
	}

	void XM_CALLCONV HairEffect::SetLightDiffuseColor(int whichLight, FXMVECTOR value)
	{
		pImpl->dirtyFlags |= pImpl->lights.SetLightDiffuseColor(whichLight, value, pImpl->constants.lightDiffuseColor);
	}

	void XM_CALLCONV HairEffect::SetLightSpecularColor(int whichLight, FXMVECTOR value)
	{
		pImpl->dirtyFlags |= pImpl->lights.SetLightSpecularColor(whichLight, value, pImpl->constants.lightSpecularColor);
	}

	void HairEffect::EnableDefaultLighting()
	{
		EffectLights::EnableDefaultLighting(this);
	}

	void HairEffect::SetVertexColorEnabled(bool value)
	{
		pImpl->vertexColorEnabled = value;
	}

	void HairEffect::SetTextureEnabled(bool value)
	{
		pImpl->textureEnabled = value;
	}

	void HairEffect::SetTexture(_In_opt_ ID3D11ShaderResourceView* value)
	{
		pImpl->texture = value;
	}
}