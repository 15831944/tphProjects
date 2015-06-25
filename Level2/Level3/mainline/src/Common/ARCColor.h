// ARCColor.h: interface for the ARCColor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCCOLOR_H__12F7246E_90F2_4117_B142_BD4D1BF682CB__INCLUDED_)
#define AFX_ARCCOLOR_H__12F7246E_90F2_4117_B142_BD4D1BF682CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "commondll.h"
#include "ARCMathCommon.h"

typedef unsigned char COLORUNIT;

class ARCColor4;

class COMMON_TRANSFER ARCColor3  
{
public:
	static const COLORUNIT WHITE[];
	static const COLORUNIT BLACK[];
	static const COLORUNIT GREY_BACKGROUND;
	static const float GREY_BACKGROUND_FLOAT;
	static const COLORUNIT GREY_VISIBLEREGION;
	static const float GREY_VISIBLEREGION_FLOAT;
	static const float GREYVALUE_VISREGIONOUTLINE_FLOAT;
	static const ARCColor3 Red;
	static const ARCColor3 White;
	static const ARCColor3 Black;
	
	ARCColor3() {}
	ARCColor3(COLORUNIT _r, COLORUNIT _g, COLORUNIT _b)
		{ n[RED]=_r; n[GREEN]=_g; n[BLUE]=_b; }
	explicit ARCColor3(COLORUNIT _g)
		{ n[RED]=_g; n[GREEN]=_g; n[BLUE]=_g; }
	explicit ARCColor3(const COLORUNIT* _pc)
		{ n[RED] = _pc[RED]; n[GREEN] = _pc[GREEN]; n[BLUE] = _pc[BLUE]; }
	explicit ARCColor3(const ARCColor4& _c);

	ARCColor3(const ARCColor3& _c)
		{ n[RED]=_c.n[RED]; n[GREEN]=_c.n[GREEN]; n[BLUE]=_c.n[BLUE]; }

	ARCColor3(COLORREF _c)
		{ n[RED]=GetRValue(_c); n[GREEN]=GetGValue(_c); n[BLUE]=GetBValue(_c); }
	
	//~ARCColor3();

	COLORUNIT& operator [](int idx)
		{ return n[idx]; }
	const COLORUNIT& operator [](int idx) const
		{ return n[idx]; }

	ARCColor3& operator	=(const ARCColor3& _c)
		{ n[RED]=_c.n[RED]; n[GREEN]=_c.n[GREEN]; n[BLUE]=_c.n[BLUE]; return *this; }

	operator COLORUNIT*() { return n; }
	operator const COLORUNIT*() const { return n; }
	operator COLORREF() const { return RGB(n[RED],n[GREEN],n[BLUE]); }

private:
	COLORUNIT n[3];
};



class COMMON_TRANSFER ARCColor4
{
public:
	static const COLORUNIT WHITE[];
	static const COLORUNIT BLACK[];
	static const COLORUNIT BACKGROUND[];
	static const COLORUNIT VISIBLEREGION[];
	static const COLORUNIT VISIBLEREGIONOUTLINE[];
	
	ARCColor4() {}										//default constructor
	ARCColor4(COLORUNIT _r, COLORUNIT _g, COLORUNIT _b, COLORUNIT _a)
		{ n[RED]=_r; n[GREEN]=_g; n[BLUE]=_b; n[ALPHA]=_a; }
	ARCColor4(COLORUNIT _r, COLORUNIT _g, COLORUNIT _b)
		{ n[RED]=_r; n[GREEN]=_g; n[BLUE]=_b; n[ALPHA]=255; }
	ARCColor4(COLORUNIT _g, COLORUNIT _a)							//constructor _g is grey value
		{ n[RED]=_g; n[GREEN]=_g; n[BLUE]=_g; n[ALPHA]=_a; }
	explicit ARCColor4(COLORUNIT _g)
		{ n[RED]=_g; n[GREEN]=_g; n[BLUE]=_g; n[ALPHA]=255; }
	explicit ARCColor4(const COLORUNIT* _pc, int _count=4);
	ARCColor4(const ARCColor4& _c)
		{ n[RED]=_c[RED]; n[GREEN]=_c[GREEN]; n[BLUE]=_c[BLUE]; n[ALPHA]=_c[ALPHA]; }
	explicit ARCColor4(const ARCColor3& _c);
	
	ARCColor4(COLORREF _c)
		{ n[RED]=GetRValue(_c); n[GREEN]=GetGValue(_c); n[BLUE]=GetBValue(_c); n[ALPHA]=255; }

	//~ARCColor4();

	COLORUNIT& operator [](int idx)
		{ return n[idx]; }
	const COLORUNIT& operator [](int idx) const
		{ return n[idx]; }

	ARCColor4& operator	=(const ARCColor4& _c)
		{ n[RED]=_c[RED]; n[GREEN]=_c[GREEN]; n[BLUE]=_c[BLUE]; n[ALPHA]=_c[ALPHA]; return *this; }

	operator COLORUNIT*() { return n; }
	operator const COLORUNIT*() const { return n; }
	operator COLORREF() const { return RGB(n[RED],n[GREEN],n[BLUE]); }

private:
	COLORUNIT n[4];
};



#endif // !defined(AFX_ARCCOLOR_H__12F7246E_90F2_4117_B142_BD4D1BF682CB__INCLUDED_)
