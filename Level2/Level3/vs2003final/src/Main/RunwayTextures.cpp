#include "stdafx.h"
#include "RunwayTextures.h"
#include "TermPlan.h"

#include <CommonData/tga.h>

#include <assert.h>

const char* RunwayTextures::sm_szFileNames[] = {
	"pa_taxiway.tga",
	"pa_centerline.tga",
	"pa_threshold.tga",
	"pa_threshold2.tga",
	"pa_0c.tga",
	"pa_1c.tga",
	"pa_2c.tga",
	"pa_3c.tga",
	"pa_4c.tga",
	"pa_5c.tga",
	"pa_6c.tga",
	"pa_7c.tga",
	"pa_8c.tga",
	"pa_9c.tga",
	"pa_1l.tga",
	"pa_2l.tga",
	"pa_3l.tga",
	"pa_0r.tga",
	"pa_1r.tga",
	"pa_2r.tga",
	"pa_3r.tga",
	"pa_4r.tga",
	"pa_5r.tga",
	"pa_6r.tga",
	"pa_7r.tga",
	"pa_8r.tga",
	"pa_9r.tga",
	"pa_C.tga",
	"pa_L.tga",
	"pa_R.tga",
	"pa_tz_three.tga",
	"pa_tz_two_a.tga",
	"pa_tz_two_b.tga",
	"pa_tz_one_a.tga",
	"pa_tz_one_b.tga",
	"pa_aim.tga",
	
	
};

const int RunwayTextures::sm_nTextureCount = NTEXTURECOUNT;




bool RunwayTextures::GetTexture( TextureEnum _type, unsigned int* _data, int& _width, int& _height, int& _colors )
{
	//return false;
	assert(_type < RunwayTextures::GetNumTextures());
	const char* szFileName = sm_szFileNames[_type];

	static char szFullPath[256];
	strcpy(szFullPath, ((CTermPlanApp*)AfxGetApp())->GetTexturePath());
	strcat(szFullPath, "\\Runway\\");
	strcat(szFullPath, szFileName);

	// TRACE("runway file: %s\n", szFullPath);

	tgaInfo* pInfo = tgaLoad(szFullPath);

	if(0 != pInfo->status) return false;

	_width = pInfo->width;
	_height = pInfo->height;
	_colors = pInfo->pixelDepth/8;

	memcpy(_data, pInfo->imageData, _width*_height*_colors);

	tgaDestroy(pInfo);	

	return true;
}
