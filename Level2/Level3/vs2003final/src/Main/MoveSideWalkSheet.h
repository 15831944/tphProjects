#if !defined(AFX_MOVESIDEWALKSHEET_H__3CB365BA_6C5A_4DB2_A0E8_AA1BDD95D8E9__INCLUDED_)
#define AFX_MOVESIDEWALKSHEET_H__3CB365BA_6C5A_4DB2_A0E8_AA1BDD95D8E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoveSideWalkSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMoveSideWalkSheet

class CMoveSideWalkSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMoveSideWalkSheet)

// Construction
public:
	CMoveSideWalkSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMoveSideWalkSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoveSideWalkSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMoveSideWalkSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMoveSideWalkSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CPropertyPage* m_pProperPageArr [ 2];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVESIDEWALKSHEET_H__3CB365BA_6C5A_4DB2_A0E8_AA1BDD95D8E9__INCLUDED_)
