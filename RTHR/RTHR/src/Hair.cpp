#pragma once
#include "pch.h"
#include "Hair.h"

namespace RTHR {
	Hair::Hair(wchar_t const* dir, uint16 width, uint16 length)
	{
		m_width = width;
		m_length = length;
		m_wispCount = 0;
		//loadHairVertex(dir);
	}

	Hair::Hair(GeometryType aType, shared_ptr<DX::DeviceResources> device, float aSize, uint16 width, uint16 length)
	{
		size = aSize;
		m_width = width;
		m_length = length;
		type = aType;
		m_device = device;

		CreateDeviceDependentResources();
	}

	int Hair::getLength()
	{
		return m_length;
	}

	int Hair::getWidth()
	{
		return m_width;
	}

	int Hair::getWispCount()
	{
		return m_wispCount;
	}

	void Hair::setWispCount(uint16 count)
	{
		//Reduces chance of wisp count being a drain on resources
		if (count > MAXUINT16 >> 2)
			m_wispCount = MAXUINT16 >> 2;
		else
			m_wispCount = count;
	}


	
	void Hair::genStrands(const shared_ptr<VertexCollection> vert)
	{
#pragma region Hair Extrusion		
		// Initialize the list of hair strand vertices
		vector<VertexPositionColor> strands = vector<VertexPositionColor>();

		// Iterates over the vertices in the geometry extruding points along normal direction
		// The distance between vertices will be resolved on GPU for LOD however an arbitrary 1
		// is chosen for now
		size_t size = vert->size();
		vector<uint32> roots(size);
		for (size_t i = 0; i < size; i++)
		{
			Vector3 direction = vert->at(i).normal;
			Vector3 position = vert->at(i).position;
			strands.push_back(VertexPositionColor(position, Colors::Brown));
			for (uint32_t j = 1; j < m_length; j++)
			{
				//In the future, this should use UV texture map in order to paint length weight onto the object
				position += direction;
				strands.push_back(VertexPositionColor(position, Colors::Brown));
			}
			roots[i] = strands.size();
		}

		vertexCount = strands.size();

#pragma endregion

#pragma region Buffer Initializations
		/***************************************************************************/
		/* Hair guidestrand buffer initialization */
		/***************************************************************************/
		// Description of the vertex buffer
		D3D11_BUFFER_DESC vertBufferDesc;
		vertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertBufferDesc.ByteWidth = sizeof(VertexPositionColor) * vertexCount;
		vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertBufferDesc.CPUAccessFlags = 0;
		vertBufferDesc.MiscFlags = 0;

		// Fill in vertex buffer subresource data
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &(strands[0]);
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// Description of the index buffer
		D3D11_BUFFER_DESC indxBufDesc;
		indxBufDesc.Usage = D3D11_USAGE_DEFAULT;
		indxBufDesc.ByteWidth = sizeof(unsigned long) * size;
		indxBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indxBufDesc.CPUAccessFlags = 0;
		indxBufDesc.MiscFlags = 0;

		// Fill in index buffer subresource data
		D3D11_SUBRESOURCE_DATA indxData;
		indxData.pSysMem = &roots;
		indxData.SysMemPitch = 0;
		indxData.SysMemSlicePitch = 0;
		
		HRESULT hr = m_device->GetD3DDevice()->CreateBuffer(&vertBufferDesc, &initData, &strandsVB);
		
		if (FAILED(hr))
			throw exception("Creation of the strand vertex buffer failed.");

		DirectX::SetDebugObjectName(strandsVB.Get(), "Hair");

		hr = m_device->GetD3DDevice()->CreateBuffer(&indxBufDesc, &indxData, &strandsIB);

		if (FAILED(hr))
			throw exception("Creation of strand index buffer failed.");

		DirectX::SetDebugObjectName(strandsIB.Get(), "Hair");

#pragma endregion
	}

	void Hair::Draw(FXMMATRIX world, CXMMATRIX view, CXMMATRIX proj, FXMVECTOR color, 
					ID3D11ShaderResourceView* texture, bool wireframe, std::function<void()> setCustomState)
	{
		if (!initDone)
			return;
#pragma region Draw Hair
		auto context = m_device->GetD3DDeviceContext();

		MVP = ModelViewProjectionConstantBuffer(world, view, proj);

		context->UpdateSubresource1(
			ModViewProjB.Get(),
			0,
			NULL,
			&MVP,
			0,
			0,
			0
			);

		UINT stride = sizeof(VertexPositionColor);
		UINT offset = 0;

		context->IASetVertexBuffers(
			0,
			1,
			strandsVB.GetAddressOf(),
			&stride,
			&offset
			);

		context->IASetIndexBuffer(
			strandsIB.Get(),
			DXGI_FORMAT_R16_UINT,
			0
			);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		context->IASetInputLayout(inputLayout.Get());

		//Attach vertex shader
		context->VSSetShader(
			strandsVS.Get(),
			nullptr,
			0
			);

		context->VSSetConstantBuffers1(
			0,
			1,
			ModViewProjB.GetAddressOf(),
			nullptr,
			nullptr
			);

		//Attach pixel shader
		context->PSSetShader(
			strandsPS.Get(),
			nullptr,
			0
			);

		// Draw
		context->Draw(vertexCount, 0);
#pragma endregion

		m_geometry->Draw(world, view, proj, color, texture, wireframe, setCustomState);

	}

	void Hair::CreateDeviceDependentResources()
	{
		//Load Shaders
		auto loadVSTask = DX::ReadDataAsync(VS);
		auto loadPSTask = DX::ReadDataAsync(PS);

#pragma region Vertex Shader
		//After vertex shader file loaded create shader and input layout
		auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
			DX::ThrowIfFailed(
				m_device->GetD3DDevice()->CreateVertexShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					&strandsVS
					)
				);

			DX::ThrowIfFailed(
				m_device->GetD3DDevice()->CreateInputLayout(
					VertexPositionColor::InputElements,
					VertexPositionColor::InputElementCount,
					&fileData[0],
					fileData.size(),
					&inputLayout
					)
				);
		});
#pragma endregion

#pragma region Pixel Shader
		auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
		{
			DX::ThrowIfFailed(
				m_device->GetD3DDevice()->CreatePixelShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					&strandsPS
					)
				);

			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
			DX::ThrowIfFailed(
				m_device->GetD3DDevice()->CreateBuffer(
					&constantBufferDesc,
					nullptr,
					&ModViewProjB)
				);
		});
#pragma endregion

#pragma region Geometry Shader
		//TODO: Load a geometry shader
#pragma endregion

#pragma region Compute Shader
		//TODO: Load a compute shader
#pragma endregion

#pragma region Geometry Intialization
		ID3D11DeviceContext* context = m_device->GetD3DDeviceContext();

		switch (type)
		{
		case(BOX) :
			m_geometry = GeometricPrimitive::CreateBox(context, XMFLOAT3(size, size, size));
			break;
		case(CUBE) :
			m_geometry = GeometricPrimitive::CreateCube(context, size);
			break;
		case(SPHERE) :
			m_geometry = GeometricPrimitive::CreateSphere(context, size);
			break;
		case(GEOSPHERE) :
			m_geometry = GeometricPrimitive::CreateGeoSphere(context, size);
			break;
		case(CYLINDER) :
			m_geometry = GeometricPrimitive::CreateCylinder(context, size);
			break;
		case(CONE) :
			m_geometry = GeometricPrimitive::CreateCone(context, size);
			break;
		case(TORUS) :
			m_geometry = GeometricPrimitive::CreateTorus(context, size);
			break;
		case(TETRAHEDRON) :
			m_geometry = GeometricPrimitive::CreateTetrahedron(context, size);
			break;
		case(OCTAHEDRON) :
			m_geometry = GeometricPrimitive::CreateOctahedron(context, size);
			break;
		case(DOCDECAHEDRON) :
			m_geometry = GeometricPrimitive::CreateDodecahedron(context, size);
			break;
		case(ICOSAHEDRON) :
			m_geometry = GeometricPrimitive::CreateIcosahedron(context, size);
			break;
		case(TEAPOT) :
			m_geometry = GeometricPrimitive::CreateTeapot(context, size);
			break;
		default:
			throw invalid_argument("The provided type is not implemented!");
			break;
		}

		const shared_ptr<VertexCollection> vert = m_geometry->getVertices();

		genStrands(vert);
#pragma endregion
		initDone = true;
	}

	void Hair::Reset()
	{
		strandsVB.Reset();
		strandsIB.Reset();
		strandsVS.Reset();
		strandsIB.Reset();
		inputLayout.Reset();
		m_geometry.reset();
		ModViewProjB.Reset();
	}
}