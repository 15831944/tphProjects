#ifndef __RUNWAYTEXTURES_H__
#define __RUNWAYTEXTURES_H__

#pragma once

#include <vector>
#include "../Common/ARCVector.h"

class RunwayTextures
{
public:
	enum TextureEnum {
		taxiway = 0,
		centerline,
		threshold,
		threshold2,
		marking0C,
		marking1C,
		marking2C,
		marking3C,
		marking4C,
		marking5C,
		marking6C,
		marking7C,
		marking8C,
		marking9C,
		marking1L,
		marking2L,
		marking3L,
		marking0R,
		marking1R,
		marking2R,
		marking3R,
		marking4R,
		marking5R,
		marking6R,
		marking7R,
		marking8R,
		marking9R,
		markingC,
		markingL,
		markingR,
		markingTD3,
		markingTD2a,
		markingTD2b,
		markingTD1a,
		markingTD1b,
		markingAIM,
		
		NTEXTURECOUNT,            //count of the texture
	};

	static const int GetNumTextures() { return sm_nTextureCount; }

	static bool GetTexture( TextureEnum _type, unsigned int* _data, int& _width, int& _height, int& _colors );

	typedef std::vector<ARCVector2> BlockGeometryType;
	typedef std::vector<ARCVector2> BlockTexCoordType;
	typedef int BlockTextureIDType;
	typedef struct {
		BlockTextureIDType	texture_id;
		BlockGeometryType	vertex_coords;
		BlockTexCoordType	texture_coords;
	} BlockType;

private:
	static const char* sm_szFileNames[];
	static const int sm_nTextureCount;
};


#endif