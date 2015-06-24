// ConDBListCtrlWithCheckBox.h: interface for the CConDBListCtrlWithCheckBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONDBLISTCTRLWITHCHECKBOX_H__646459B0_B26A_40EC_82C1_CB0C1371BC5E__INCLUDED_)
#define AFX_CONDBLISTCTRLWITHCHECKBOX_H__646459B0_B26A_40EC_82C1_CB0C1371BC5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ConDBExListCtrl.h"
#define UM_ITEM_CHECK_STATUS_CHANGED WM_USER+256

class CConDBListCtrlWithCheckBox : public CConDBExListCtrl  
{
	// Construction
public:
	CConDBListCtrlWithCheckBox();
	virtual ~CConDBListCtrlWithCheckBox();
// Operations
public:
	
protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConDBListCtrlWithCheckBox)
public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	BOOL GetItemCheck(int nItem);
	void SetItemCheck(int nItem,BOOL bCheck);
	void GetVisibleRect(CRect& r);
	CSize m_sizeCheck;
	HBITMAP m_hBitmapCheck;

	ConstraintDatabase* m_pImpactSpeedDB;
	ConstraintDatabase* m_pImpactInstepDB;
	ConstraintDatabase* m_pSideStepDB;
	ConstraintDatabase* m_pEmerImpactDB;

	void SetImpactSpeedDB( ConstraintDatabase* _pConDB, int _nForceItemData=0 );
	void SetImpactInstepDB( ConstraintDatabase* _pConDB, int _nForceItemData=0 );
	void SetSideStepDB( ConstraintDatabase* _pConDB, int _nForceItemData=0 );
	void SetEmerImpactDB( ConstraintDatabase* _pConDB, int _nForceItemData=0 );


	//Reload data from the memory to the list control
	virtual void ReloadData( Constraint* _pSelCon );
	// Generated message map functions
protected:
	//{{AFX_MSG(CConDBListCtrlWithCheckBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnCheckStateChanged( WPARAM wParam, LPARAM lParam );
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif // !defined(AFX_CONDBLISTCTRLWITHCHECKBOX_H__646459B0_B26A_40EC_82C1_CB0C1371BC5E__INCLUDED_)
