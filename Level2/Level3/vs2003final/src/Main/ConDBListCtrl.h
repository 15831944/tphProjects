#if !defined(AFX_CONDBLISTCTRL_H__0701664B_F8B7_4EBE_906F_4B737AD1AC23__INCLUDED_)
#define AFX_CONDBLISTCTRL_H__0701664B_F8B7_4EBE_906F_4B737AD1AC23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConDBListCtrl.h : header file
//

#include "..\MFCExControl\ListCtrlEx.h"
#include "..\inputs\con_db.h"
#include "..\inputs\GateLagTimeDB.h"
#include "../Common/ProbDistEntry.h"
enum CON_TYPE { FLIGHT_CON, PAX_CON };

extern const CString CS_NEW_ITEM;
#define UM_LIST_CONTENT_CHANGED WM_USER+128
/////////////////////////////////////////////////////////////////////////////
// CConDBListCtrl window

class CConDBListCtrl : public CListCtrlEx
{
// Construction
public:
	CConDBListCtrl();

// Attributes
public:

// Operations
public:
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConDBListCtrl)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL m_bIsDefault;
	CGateLagTimeDB* m_pGLTConDB;
	ConstraintDatabase* GetConDB() { return m_pConDB; }

	void SetInputTerminal( InputTerminal* _pInTerm ){ m_pInTerm = _pInTerm; }

	//Delete the entry from the memory & then reload data to the list control
	bool DeleteEntry();

	// Enhance method RemoveEntry, able to delete multi-selection.
	bool DeleteMultiEntry(void);

	//Set various DB connections, e.g, flight delays & so on
	void SetCurConDB( ConstraintDatabase* _pConDB, int _nForceItemData=0 );

	//Add new entry to the memonry & then reload data to the list control
	virtual bool AddEntry(enum CON_TYPE _enumConType, Constraint* _pNewCon);

	//Reload data from the memory to the list control
	virtual void ReloadData( Constraint* _pSelCon );
    CProbDistEntry *getSelectProbDist();

	virtual ~CConDBListCtrl();
	

	// Generated message map functions
protected:
	int m_nForceItemData;
	CProbDistEntry* m_Selectpde;
	InputTerminal* m_pInTerm;
	ConstraintDatabase* m_pConDB;
	
	void SelectDataList(int _nIdx);
	int GetSelectedItem();
	
	//{{AFX_MSG(CConDBListCtrl)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONDBLISTCTRL_H__0701664B_F8B7_4EBE_906F_4B737AD1AC23__INCLUDED_)
