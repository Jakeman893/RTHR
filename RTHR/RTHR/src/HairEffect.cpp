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
}