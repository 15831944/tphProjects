#if !defined(AFX_CARSCHEDULESHEET_H__B392FD96_23EB_4A70_95F6_EBE287F24B4E__INCLUDED_)
#define AFX_CARSCHEDULESHEET_H__B392FD96_23EB_4A70_95F6_EBE287F24B4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CarScheduleSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCarScheduleSheet
#include "..\inputs\AllCarSchedule.h"
class CCarScheduleSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCarScheduleSheet)

// Construction
public:
	CCarScheduleSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CCarScheduleSheet(LPCTSTR pszCaption, TrainTrafficGraph* _pTrafficGraph,CAllCarSchedule* _pDocCarSchedule,CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCarScheduleSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCarScheduleSheet();

	// Generated message map functions
protected:
	CPropertyPage* m_pProcDataPage[2];
	//{{AFX_MSG(CCarScheduleSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARSCHEDULESHEET_H__B392FD96_23EB_4A70_95F6_EBE287F24B4E__INCLUDED_)
