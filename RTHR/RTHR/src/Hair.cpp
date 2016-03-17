#pragma once
#include "pch.h"
#include "Hair.h"

namespace RTHR {
	Hair::Hair(wchar_t const* dir, uint16 width, uint16 length)
	{
		this->width = width;
		this->length = length;
		this->wispCount = 0;
		loadHairVertex(dir);
	}

	int Hair::getLength()
	{
		return length;
	}

	int Hair::getWidth()
	{
		return width;
	}

	int Hair::getWispCount()
	{
		return wispCount;
	}

	void Hair::setWispCount(uint16 count)
	{
		//Reduces chance of wisp count being a drain on resources
		if (count > MAXUINT16 >> 2)
			wispCount = MAXUINT16 >> 2;
		else
			wispCount = count;
	}

	void Hair::LoadVertices()
	{

	}


}