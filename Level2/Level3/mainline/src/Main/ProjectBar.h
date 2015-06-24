// ProjectBar.h: interface for the CProjectBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTBAR_H__F4B4CB68_8949_4CA5_9795_CE1B56B06A37__INCLUDED_)
#define AFX_PROJECTBAR_H__F4B4CB68_8949_4CA5_9795_CE1B56B06A37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBar24X24.h"
#include "XPStyle/ComboBoxXP.h"
class CProjectBar : public CToolBar24X24  
{
public:
	CComboBox* GetComboBoxProj();
	CComboBoxXP m_cbUint3;
	CProjectBar();
	virtual ~CProjectBar();
protected:


	// Generated message map functions
	//{{AFX_MSG(CProjectBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};

#endif // !defined(AFX_PROJECTBAR_H__F4B4CB68_8949_4CA5_9795_CE1B56B06A37__INCLUDED_)
