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

	Hair::Hair(GeometryType type, ID3D11DeviceContext* context, float size, uint16 width, uint16 length)
	{
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

	bool Hair::createHairStrands()
	{
		throw exception("Not Implemented!");
	}

	void Hair::Draw()
	{
		m_geometry->Draw()
	}
}