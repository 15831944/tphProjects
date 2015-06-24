// TVN.cpp: implementation of the CTVN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TVN.h"
#include "..\inputs\AreasPortals.h"
#include "resource.h"


////////////////////////////////////////////////////////
/////  CAreaNode
////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC( CAreaNode, CTVNode )

CAreaNode::CAreaNode(int idx)
{
	m_nIdx = idx;
	m_strName = _T("");
	m_nIDResource = IDR_CTX_AREA;
	m_iImageIdx = ID_NODEIMG_AREA;
}

CAreaNode::~CAreaNode()
{
}

////////////////////////////////////////////////////////
/////  CPortalNode
////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC( CPortalNode, CTVNode )

CPortalNode::CPortalNode(int idx)
{
	m_nIdx = idx;
	m_strName = _T("");
	m_nIDResource = IDR_CTX_PORTAL;
	m_iImageIdx = ID_NODEIMG_PORTAL;
}

CPortalNode::~CPortalNode()
{
}