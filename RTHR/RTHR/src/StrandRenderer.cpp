#include "pch.h"
#include "StrandRenderer.h"

#include "Common\DirectXHelper.h"

using namespace RTHR;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the strand geometry
StrandRenderer::StrandRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_vertexCount(0),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void StrandRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	//This sample makes use of a right-handed coordinate system using row-major matrices
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	// Eye at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis
	static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

//Called once per frame
void StrandRenderer::Update(DX::StepTimer const & timer)
{

}


void StrandRenderer::Render()
{
	// Loading is asynchronous. Draw geometry after loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send to the graphics device.
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
		);

	//Each vertex is once instance of the VertexPositionColor struct
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
		);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
		);

	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
		);

	// Send the constant buffer to graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
		);

	//Attach pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
		);

	context->Draw(
		m_vertexCount,
		0
		);

	////Draw objects
	//context->DrawIndexed(
	//	m_indexCount,
	//	0,
	//	0
	//	);
}

// Initializes view parameters when window size changes
void StrandRenderer::CreateDeviceDependentResources()
{
	//Load shaders asynchronously
	auto loadVSTask = DX::ReadDataAsync(L"HairVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"HairPixelShader.cso");
	auto loadGSTask = DX::ReadDataAsync(L"HairGeometryShader.cso");

	// Once file is loaded, create shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		// Creates the vertex shader using the CPU
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);

		// Creates a description for the vertex structure
		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA},
		};

		// Defines the vertex structure in the GPU so it can be used
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
			);
	});

	//After pixel shader file loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		// Creates the pixel shader using the CPU
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
			);

		// Defines a constant buffer to be used by the GPU
		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		
		DX::ThrowIfFailed(
			// Creates the constant buffer on the GPU
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);
	});

	auto createGSTask = loadGSTask.then([this](const std::vector<byte>& fileData) {
		// Creates the geometry shader on the GPU
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateGeometryShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_geometryShader
				)
			);

		//TODO: Define and connect necessary constant buffers here
	});

	// Once both shaders are loaded, create the mesh.
	auto createMeshTask = (createPSTask && createVSTask).then([this]() {

		// Load mesh vertices. In this case, just a single strand
		static const VertexPositionColor lineVertices[] =
		{
			{ XMFLOAT3(-0.5f, 0, 0), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(0, -0.5f, 0), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(0.25f, 0, 0), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(0.75f, 0.5f, 0), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		};

		m_vertexCount = ARRAYSIZE(lineVertices);

		// Specifies the data for intializing a subresource
		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		// Initialization data
		vertexBufferData.pSysMem = lineVertices;
		// No meaning for non-textures
		vertexBufferData.SysMemPitch = 0;
		// No meaning for non-textures
		vertexBufferData.SysMemSlicePitch = 0;
		
		// Description of the vertex buffer
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(lineVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			// Provides the GPU with the buffer
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
				)
			);
	});

	// Once mesh is loaded, object is ready to be rendered.
	createMeshTask.then([this]() {
		m_loadingComplete = true;
	});
}

void StrandRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
	m_geometryShader.Reset();
}