
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameInputData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrameProcessDefine frame

class CFrameProcessDefine : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFrameProcessDefine)
protected:
	CFrameProcessDefine();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameProcessDefine)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);	
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual ~CFrameProcessDefine();

	// Generated message map functions
	//{{AFX_MSG(CFrameProcessDefine)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

