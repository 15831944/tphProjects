// ARCColor.cpp: implementation of the ARCColor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARCColor.h"

static const COLORUNIT GREYVALUE_BACKGROUND = 166;
static const COLORUNIT GREYVALUE_VISREGION = 153;
static const COLORUNIT GREYVALUE_VISREGIONOUTLINE = 96;

const COLORUNIT ARCColor3::WHITE[] = { 255, 255, 255 };
const COLORUNIT ARCColor3::BLACK[] = { 0, 0, 0 };
const COLORUNIT ARCColor3::GREY_BACKGROUND = GREYVALUE_BACKGROUND;
const COLORUNIT ARCColor3::GREY_VISIBLEREGION = GREYVALUE_VISREGION;
const float ARCColor3::GREY_BACKGROUND_FLOAT = GREYVALUE_BACKGROUND/255.0f;
const float ARCColor3::GREY_VISIBLEREGION_FLOAT = GREYVALUE_VISREGION/255.0f;
const float ARCColor3::GREYVALUE_VISREGIONOUTLINE_FLOAT = GREYVALUE_VISREGIONOUTLINE/255.0f;

//ARCColor3::~ARCColor3()
//{
//}

const ARCColor3 ARCColor3::Red(255,0,0);
const ARCColor3 ARCColor3::White(255,255,255);
const ARCColor3 ARCColor3::Black(0,0,0);

ARCColor3::ARCColor3(const ARCColor4& _c)
{
	n[RED]=_c[RED]; n[GREEN]=_c[GREEN]; n[BLUE]=_c[BLUE];
}


const COLORUNIT ARCColor4::WHITE[] = { 255, 255, 255, 255 };
const COLORUNIT ARCColor4::BLACK[] = { 0, 0, 0, 255 };
const COLORUNIT ARCColor4::BACKGROUND[] = { GREYVALUE_BACKGROUND, GREYVALUE_BACKGROUND, GREYVALUE_BACKGROUND, 255 };
const COLORUNIT ARCColor4::VISIBLEREGION[] = { GREYVALUE_VISREGION, GREYVALUE_VISREGION, GREYVALUE_VISREGION, 255 };
const COLORUNIT ARCColor4::VISIBLEREGIONOUTLINE[] = { GREYVALUE_VISREGIONOUTLINE, GREYVALUE_VISREGIONOUTLINE, GREYVALUE_VISREGIONOUTLINE, 255 };

//ARCColor4::~ARCColor4()
//{
//}

//ARCColor4::ARCColor4(const unsigned char* _pc, int _count)
//{ 
//	for(int i=0; i<_count; i++) {
//		n[i] = _pc[i];
//	}
//}


ARCColor4::ARCColor4(const ARCColor3& _c)
{ 
	n[RED]=_c[RED]; n[GREEN]=_c[GREEN]; n[BLUE]=_c[BLUE]; n[ALPHA]=255;
}

ARCColor4::ARCColor4( const COLORUNIT* _pc, int _count/*=4*/ )
{
	for(int i=0; i<_count; i++) {
		n[i] = _pc[i];
	}
}
