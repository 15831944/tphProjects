// CoolTreeEx.h: interface for the CCoolTreeEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COOLTREEEX_H__1A9653FC_52C2_4332_A050_6BBD71484EEB__INCLUDED_)
#define AFX_COOLTREEEX_H__1A9653FC_52C2_4332_A050_6BBD71484EEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\MFCExControl\CoolTree.h"

class CCoolTreeEx : public CCoolTree  
{
#ifdef _DEBUG
	//skip assert because of dual-mapped CWnd
	virtual void AssertValid() const {};
	//if a non-CTC derived CTreeCtrl class is used
	// then make sure the AssertValid doesn't call
	// the base class
#endif
	//need access to m_pfnSuper
	friend class CNodeView;
};

#endif // !defined(AFX_COOLTREEEX_H__1A9653FC_52C2_4332_A050_6BBD71484EEB__INCLUDED_)
