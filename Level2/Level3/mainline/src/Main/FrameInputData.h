#if !defined(AFX_FRAMEINPUTDATA_H__18DEFF23_D047_450F_B172_668102EA1C90__INCLUDED_)
#define AFX_FRAMEINPUTDATA_H__18DEFF23_D047_450F_B172_668102EA1C90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameInputData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrameInputData frame

class CFrameInputData : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFrameInputData)
protected:
	CFrameInputData();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameInputData)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual ~CFrameInputData();

	// Generated message map functions
	//{{AFX_MSG(CFrameInputData)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEINPUTDATA_H__18DEFF23_D047_450F_B172_668102EA1C90__INCLUDED_)
