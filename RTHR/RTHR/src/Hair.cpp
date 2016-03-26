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

	Hair::Hair(GeometryType type, shared_ptr<DX::DeviceResources> device, float size, uint16 width, uint16 length)
	{
		m_width = width;
		m_length = length;

		m_device = device;

		m_states = make_unique<CommonStates>(m_device->GetD3DDevice());
		m_effect = make_shared<BasicEffect>(m_device->GetD3DDevice());

		ID3D11DeviceContext* context = device->GetD3DDeviceContext();

		switch (type)
		{
		case(BOX) :
			m_geometry = GeometricPrimitive::CreateBox(context, XMFLOAT3(size,size,size));
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

		m_guideStrands = make_unique<Model>();
		// Generates the strands extruded from each vertex
		m_guideStrands->meshes.push_back(genStrands());
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

	shared_ptr<ModelMesh> Hair::genStrands()
	{
#pragma region Hair Extrusion
		// Retrieves vertex information for input geometry
		const shared_ptr<VertexCollection> vert = m_geometry->getVertices();
		
		// Initialize the list of hair strand vertices
		HairStrand strands = HairStrand();

		// Iterates over the vertices in the geometry extruding points along normal direction
		// The distance between vertices will be resolved on GPU for LOD however an arbitrary 1
		// is chosen for now
		size_t size = vert->size();
		vector<uint32> roots(size);
		for (size_t i = 0; i < size; i++)
		{
			Vector3 direction = vert->at(i).normal;
			Vector3 position = vert->at(i).position;
			strands.push_back(position);
			for (uint32_t j = 1; j < m_length; j++)
			{
				//In the future, this should use UV texture map in order to paint length weight onto the object
				position += direction;
				strands.push_back(position);
			}
			roots[i] = strands.size();
		}

#pragma endregion


#pragma region Buffer Initializations
		/***************************************************************************/
		/* Hair guidestrand buffer initialization */
		/***************************************************************************/
		//Guidestrand vertex buffer initialization
		ID3D11Buffer* g_pVertexBuffer;

		//Guidestrand root index buffer initialization
		ID3D11Buffer* g_pIndexBuffer;

		// Description of the vertex buffer
		D3D11_BUFFER_DESC vertBufferDesc;
		vertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertBufferDesc.ByteWidth = sizeof(strands) * 3;
		vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertBufferDesc.CPUAccessFlags = 0;
		vertBufferDesc.MiscFlags = 0;

		// Fill in vertex buffer subresource data
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &strands;
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
		
		HRESULT hr = m_device->GetD3DDevice()->CreateBuffer(&vertBufferDesc, &initData, &g_pVertexBuffer);


		if (FAILED(hr))
			throw exception("Creation of the strand vertex buffer failed.");

		hr = m_device->GetD3DDevice()->CreateBuffer(&indxBufDesc, &indxData, &g_pIndexBuffer);

		if (FAILED(hr))
			throw exception("Creation of strand index buffer failed.");

#pragma endregion

#pragma region Mesh Object Creation
		shared_ptr<ModelMesh> mesh = make_shared<ModelMesh>();
		ModelMeshPart strandMesh = ModelMeshPart();

		strandMesh.vertexBuffer = g_pVertexBuffer;

		strandMesh.indexBuffer = g_pIndexBuffer;

		strandMesh.indexCount = size * m_length;

		strandMesh.vertexStride = 1;

		strandMesh.primitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

		strandMesh.effect = m_effect;
		//TODO create input layout, effect, start index, vertexOffset, vertexStride, indexCount, vbDecl, isAlpha

		mesh->meshParts.push_back(make_unique<ModelMeshPart>(strandMesh));
#pragma endregion

		return mesh;
	}

	void Hair::Draw(FXMMATRIX world, CXMMATRIX view, CXMMATRIX proj, FXMVECTOR color, 
					ID3D11ShaderResourceView* texture, bool wireframe, std::function<void()> setCustomState)
	{
		m_geometry->Draw(world, view, proj, color, texture, wireframe, setCustomState);
		m_guideStrands->Draw(m_device->GetD3DDeviceContext(), *m_states, world, view, proj, true);
	}

	void Hair::Reset()
	{
		m_geometry.reset();
		m_states.reset();
	}
}