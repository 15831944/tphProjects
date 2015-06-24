// ResourceDispPropItem.cpp: implementation of the CResourceDispPropItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResourceDispPropItem.h"

#include "PaxShapeDefs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CResourceDispPropItem::CResourceDispPropItem(int _nResPoolIdx)
{
	m_nResourcePoolIdx = _nResPoolIdx;
	m_isVisible = TRUE;
	m_shape = CPaxShapeDefs::DefaultShape;
	m_colorRef = RGB( 32, 32, 32 );
}

CResourceDispPropItem::CResourceDispPropItem()
{
	m_isVisible = TRUE;
	m_shape = CPaxShapeDefs::DefaultShape;
	m_colorRef = RGB( 32, 32, 32 );
}

CResourceDispPropItem::CResourceDispPropItem( const CResourceDispPropItem& _rhs )
{
	m_isVisible = _rhs.m_isVisible;
	m_shape = _rhs.m_shape;
	m_colorRef = _rhs.m_colorRef;
}

CResourceDispPropItem::~CResourceDispPropItem()
{
}