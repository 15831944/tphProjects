// PaxFlowTreeCtrl.h: interface for the CPaxFlowTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXFLOWTREECTRL_H__1C1949C1_628D_4ACD_A1B0_649E106563D8__INCLUDED_)
#define AFX_PAXFLOWTREECTRL_H__1C1949C1_628D_4ACD_A1B0_649E106563D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PrintableTreeCtrl.h"

class CPaxFlowTreeCtrl : public CPrintableTreeCtrl  
{
public:
	// Construction
	CPaxFlowTreeCtrl();
	virtual ~CPaxFlowTreeCtrl();
	int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
	BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxFlowTreeCtrl)
protected:
	virtual void PreSubclassWindow();
	
	
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CPaxFlowTreeCtrl)
	
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PAXFLOWTREECTRL_H__1C1949C1_628D_4ACD_A1B0_649E106563D8__INCLUDED_)
