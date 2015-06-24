#if !defined(AFX_GLOBALDBSHEET_H__9E708758_F72F_4C28_8EB5_A7457A94A3D0__INCLUDED_)
#define AFX_GLOBALDBSHEET_H__9E708758_F72F_4C28_8EB5_A7457A94A3D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GlobalDBSheet.h : header file
//
// include
#include "common\ProjectManager.h"
#include <vector>

// declare
typedef std::vector< PROJECTINFO* > PROJECTINFO_LIST;

/////////////////////////////////////////////////////////////////////////////
// CGlobalDBSheet
class CGlobalDBSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CGlobalDBSheet)

// Construction
public:
	CGlobalDBSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CGlobalDBSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlobalDBSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGlobalDBSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGlobalDBSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CPropertyPage* m_PropertyPageArr[2];
	PROJECTINFO_LIST	m_allProjInfo;

private:
	// load all project info
	void loadAllProjectInfo( void );
	
	// clear all project info
	void clearAllProjectInfo( void );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBALDBSHEET_H__9E708758_F72F_4C28_8EB5_A7457A94A3D0__INCLUDED_)
