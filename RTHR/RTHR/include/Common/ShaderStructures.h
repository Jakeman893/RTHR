#pragma once

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#else
#include <d3d11_1.h>
#endif

// VS 2010/2012 do not support =default =delete
#ifndef DIRECTX_CTOR_DEFAULT
#if defined(_MSC_VER) && (_MSC_VER < 1800)
#define DIRECTX_CTOR_DEFAULT {}
#define DIRECTX_CTOR_DELETE ;
#else
#define DIRECTX_CTOR_DEFAULT =default;
#define DIRECTX_CTOR_DELETE =delete;
#endif
#endif

#include <DirectXMath.h>

using namespace DirectX;

namespace RTHR
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		ModelViewProjectionConstantBuffer() DIRECTX_CTOR_DEFAULT

		ModelViewProjectionConstantBuffer(XMFLOAT4X4 const& model, XMFLOAT4X4 const& view, XMFLOAT4X4 const& projection)
			: model(model),
			  view (view),
			  projection(projection)
		{ }

		ModelViewProjectionConstantBuffer(const XMMATRIX model, const XMMATRIX view, const XMMATRIX projection)
		{
			XMStoreFloat4x4(&this->model, model);
			XMStoreFloat4x4(&this->view, view);
			XMStoreFloat4x4(&this->projection, projection);
		}

		XMFLOAT4X4 model;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;

		static const int InputElementCount = 3;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};
}