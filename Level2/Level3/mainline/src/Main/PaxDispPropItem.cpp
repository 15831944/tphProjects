// PaxDispPropItem.cpp: implementation of the CPaxDispPropItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PaxDispPropItem.h"

#include "PaxShapeDefs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxDispPropItem::CPaxDispPropItem( CString _csName )
{
	m_paxType.SetName( _csName );
	m_isVisible = TRUE;
	m_shape =  CPaxShapeDefs::DefaultShape;
	m_colorRef = RGB( 32, 32, 32 );
	m_isLeaveTrail = FALSE;
	m_linetype = ENUM_LINE_1PTSOLID;
	m_dDensity = 1.0;
}

CPaxDispPropItem::CPaxDispPropItem( const CPaxDispPropItem& _rhs )
{
	m_paxType = _rhs.m_paxType;
	m_isVisible = _rhs.m_isVisible;
	m_shape = _rhs.m_shape;
	m_colorRef = _rhs.m_colorRef;
	m_isLeaveTrail = _rhs.m_isLeaveTrail;
	m_linetype = _rhs.m_linetype;
	m_dDensity = _rhs.m_dDensity;
}

CPaxDispPropItem::~CPaxDispPropItem()
{

}
