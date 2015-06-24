// ViewBar.h: interface for the CViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWBAR_H__C7130391_193E_4BF8_90C2_8DB6C2D5307F__INCLUDED_)
#define AFX_VIEWBAR_H__C7130391_193E_4BF8_90C2_8DB6C2D5307F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBar24X24.h"

class CViewBar : public CToolBar24X24  
{
public:
	CComboBox m_cbUint;
	CViewBar();
	virtual ~CViewBar();
protected:


	// Generated message map functions
	//{{AFX_MSG(CViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_VIEWBAR_H__C7130391_193E_4BF8_90C2_8DB6C2D5307F__INCLUDED_)
