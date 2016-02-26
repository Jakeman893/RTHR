#pragma once

#include "Common\DeviceResources.h"
#include "Common\ShaderStructures.h"
#include "Common\StepTimer.h"

namespace RTHR
{
	class StrandRenderer
	{
	public:
		StrandRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

	private:
		// Cached pointer to device resources
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for geometry.
		ModelViewProjectionConstantBuffer m_constantBufferData;
		uint32 m_vertexCount;

		//Variables used with the rendering loop.
		bool	m_loadingComplete;
	};
}