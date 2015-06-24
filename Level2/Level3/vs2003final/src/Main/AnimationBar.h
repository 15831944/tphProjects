// AnimationBar.h: interface for the CAnimationBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATIONBAR_H__111DB49B_1E5D_490A_B5FC_F694158F8C40__INCLUDED_)
#define AFX_ANIMATIONBAR_H__111DB49B_1E5D_490A_B5FC_F694158F8C40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBar24X24.h"
#include "XPSTYLE\ComboBoxXP.h"	// Added by ClassView

class CAnimationBar : public CToolBar24X24  
{
public:
	CComboBox* GetAnimPaxCB();
	CComboBoxXP m_cbPax;
	CAnimationBar();
	virtual ~CAnimationBar();
protected:


	// Generated message map functions
	//{{AFX_MSG(CAnimationBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelChangeSeledPaxComboBox();
	//}}AFX_MSG
	afx_msg BOOL OnDropDownPickSpeed(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_ANIMATIONBAR_H__111DB49B_1E5D_490A_B5FC_F694158F8C40__INCLUDED_)
