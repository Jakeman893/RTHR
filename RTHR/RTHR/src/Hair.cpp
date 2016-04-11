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
		geomSize = aSize;
		m_width = width;
		m_length = length;
		type = aType;
		m_device = device;

		matrices = EffectMatrices();
		states = make_unique<CommonStates>(m_device->GetD3DDevice());

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
		vector<VertexPositionNormalColor> strands = vector<VertexPositionNormalColor>();

		// Iterates over the vertices in the geometry extruding points along normal direction
		// The distance between vertices will be resolved on GPU for LOD however an arbitrary 1
		// is chosen for now
		size_t size = vert->size();
		vector<uint32> roots(size);
		for (size_t i = 0; i < size; i++)
		{
			Vector3 direction = vert->at(i).normal;
			Vector3 position = vert->at(i).position;
			strands.push_back(VertexPositionNormalColor(position, direction, Colors::Brown));
			for (uint32_t j = 1; j < m_length; j++)
			{
				//In the future, this should use UV texture map in order to paint length weight onto the object
				position += direction;
				strands.push_back(VertexPositionNormalColor(position, direction, Colors::Brown));
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
		vertBufferDesc.ByteWidth = sizeof(VertexPositionNormalColor) * vertexCount;
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

	void Hair::Draw(Matrix world, Matrix view, Matrix proj, Vector3 eye, FXMVECTOR color,
					ID3D11ShaderResourceView* texture, bool wireframe, std::function<void()> setCustomState)
	{
		if (!initDone)
			return;

		m_geometry->Draw(world, view, proj, color, texture, wireframe, setCustomState);

		auto context = m_device->GetD3DDeviceContext();
		dirtyFlags = EffectDirtyFlags::WorldViewProj;
#pragma region Update Constant Buffer
		HairEffectConstants update = HairEffectConstants();
		update.diffuseColor = color;
		update.eyePosition = eye;
		update.world = world;

		XMMATRIX worldViewProjConstant;

		matrices.world = world;
		matrices.view = view;
		matrices.projection = proj;
		matrices.SetConstants(dirtyFlags, worldViewProjConstant);

		update.worldViewProj = worldViewProjConstant;

		constBuf->SetData(context, update);

#pragma endregion
#pragma region Draw Hair

		UINT stride = sizeof(VertexPositionNormalColor);
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

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

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
			constBuf->GetBufferAddress(),
			nullptr,
			nullptr
			);

		//Attach pixel shader
		context->PSSetShader(
			strandsPS.Get(),
			nullptr,
			0
			);

#pragma region Rasterizer/DepthStencil/Blend/Sampler States

		// Sets the blend state of the primitive to be drawn (hair)
		context->OMSetBlendState(states->Additive(), Colors::Black, 0xFFFFFFFF);
		// Sets the depth stencil state
		context->OMSetDepthStencilState(states->DepthDefault(), 0);
		// Sets the rasterizer state
		context->RSSetState(states->Wireframe());

		auto samplerState = states->LinearWrap();
		context->PSSetSamplers(0, 1, &samplerState);

#pragma endregion

		// Draw
		context->Draw(vertexCount, 0);
#pragma endregion
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
					VertexPositionNormalColor::InputElements,
					VertexPositionNormalColor::InputElementCount,
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
		});
#pragma endregion

#pragma region Geometry Shader
		//TODO: Load a geometry shader
#pragma endregion

#pragma region Compute Shader
		//TODO: Load a compute shader
#pragma endregion

#pragma region Constant Buffer

		matrices.projection = Matrix::Identity;
		matrices.view = Matrix::Identity;
		matrices.world = Matrix::Identity;

		auto a = sizeof(HairEffectConstants);

		constBuf = make_unique<ConstantBuffer<HairEffectConstants>>(m_device->GetD3DDevice());

#pragma endregion

#pragma region Geometry Intialization
		ID3D11DeviceContext* context = m_device->GetD3DDeviceContext();

		switch (type)
		{
		case(BOX) :
			m_geometry = GeometricPrimitive::CreateBox(context, XMFLOAT3(geomSize, geomSize, geomSize));
			break;
		case(CUBE) :
			m_geometry = GeometricPrimitive::CreateCube(context, geomSize);
			break;
		case(SPHERE) :
			m_geometry = GeometricPrimitive::CreateSphere(context, geomSize);
			break;
		case(GEOSPHERE) :
			m_geometry = GeometricPrimitive::CreateGeoSphere(context, geomSize);
			break;
		case(CYLINDER) :
			m_geometry = GeometricPrimitive::CreateCylinder(context, geomSize);
			break;
		case(CONE) :
			m_geometry = GeometricPrimitive::CreateCone(context, geomSize);
			break;
		case(TORUS) :
			m_geometry = GeometricPrimitive::CreateTorus(context, geomSize);
			break;
		case(TETRAHEDRON) :
			m_geometry = GeometricPrimitive::CreateTetrahedron(context, geomSize);
			break;
		case(OCTAHEDRON) :
			m_geometry = GeometricPrimitive::CreateOctahedron(context, geomSize);
			break;
		case(DOCDECAHEDRON) :
			m_geometry = GeometricPrimitive::CreateDodecahedron(context, geomSize);
			break;
		case(ICOSAHEDRON) :
			m_geometry = GeometricPrimitive::CreateIcosahedron(context, geomSize);
			break;
		case(TEAPOT) :
			m_geometry = GeometricPrimitive::CreateTeapot(context, geomSize);
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
		constBuf.reset();
	}
}