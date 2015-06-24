// AircraftDispPropItem.cpp: implementation of the CAircraftDispPropItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AircraftDispPropItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAircraftDispPropItem::CAircraftDispPropItem( const CString& _csName )
{
	m_flightType.SetName( _csName );
	m_isVisible = TRUE;
	m_colorRef = RGB( 32, 32, 32 );
	m_isLeaveTrail = TRUE;
	m_shape.first = 4;
	m_shape.second = 0;
	m_linetype = ENUM_LINE_1PTSOLID;
	m_isLogoOn = FALSE;
	m_fdensity = 0.9f;
	m_fscalesize = 1.0f;
}

CAircraftDispPropItem::CAircraftDispPropItem( const CAircraftDispPropItem& _rhs )
{
	m_flightType = _rhs.m_flightType;
	m_isVisible = _rhs.m_isVisible;
	m_colorRef = _rhs.m_colorRef;
	m_isLeaveTrail = _rhs.m_isLeaveTrail;
	m_linetype = _rhs.m_linetype;
	m_isLogoOn = FALSE;
	m_fdensity = 0.9f;
}

CAircraftDispPropItem::~CAircraftDispPropItem()
{

}
