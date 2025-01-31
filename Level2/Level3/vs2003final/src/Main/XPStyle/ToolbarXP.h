///////////////////////////////////////////////////////////////////////////////
//
// ToolBarXP.h: definition of the CToolBarXP class.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ComboBoxXP.h"	// Added by ClassView
#define ID_COMBOBOX			100
///////////////////////////////////////////////////////////////////////////////
#define UM_SELCHAGNE_SCALE_COMBOBOX	WM_USER+1
class CToolBarXP : public CToolBar
{
public:
    CToolBarXP ();
	friend class CFrameCtrl;
// Operations
public:
	void SetSelect(DWORD dItemData);
	int AddItem2CMBWithAI(int nTickWidth,BOOL bSelecting=TRUE);
	void ShowBtn(UINT nID,BOOL bShow);
	CToolTipCtrl m_toolTipCtrl;
	BOOL CreateComboBox();
	CComboBoxXP m_cmbScale;
	BOOL LoadToolBar (UINT nIDResource);
    bool InsertControl (int nIndex, CWnd& Ctrl);
// Implementation
protected:
    // Use the BYTE type because we need 3 states (not a boolean !)
    BYTE m_bCheckVisibility;

protected:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CToolBarXP)
    virtual LRESULT DefWindowProc (UINT nMsg, WPARAM wParam, LPARAM lParam);
    virtual CSize CalcDynamicLayout (int nLength, DWORD nMode);
  //}}AFX_VIRTUAL

    // Message Handlers
protected:
    //{{AFX_MSG(CToolBarXP)
    afx_msg void OnPaint();
    afx_msg void OnButtonDropDown(NMHDR*, LRESULT*);
	afx_msg void OnSelChangeZoomComboBox();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP();
    DECLARE_DYNAMIC(CToolBarXP);
};

#define DROPDOWN(item) ((WORD)-item)

#define IS_STDBTN(button)        (((button).fsStyle&(TBSTYLE_SEP|TBSTYLE_CHECKGROUP))==TBSTYLE_BUTTON)
#define IS_CHECKBTN(button)      (((button).fsStyle&(TBSTYLE_SEP|TBSTYLE_CHECKGROUP))==TBSTYLE_CHECK)
#define IS_GROUPBTN(button)      (((button).fsStyle&(TBSTYLE_SEP|TBSTYLE_CHECKGROUP))==TBSTYLE_CHECKGROUP)
#define IS_DROPDOWN(button)      (((button).fsStyle&TBSTYLE_DROPDOWN)==TBSTYLE_DROPDOWN)
#define IS_SEPARATOR(button)     (((button).fsStyle&TBSTYLE_SEP)&&((button).idCommand==0))
#define IS_CONTROL(button)       (((button).fsStyle&TBSTYLE_SEP)&&((button).idCommand!=0))
#define IS_CHECKED(button)       (((button).fsState&TBSTATE_CHECKED)==TBSTATE_CHECKED)
#define IS_ENABLED(button)       (((button).fsState&TBSTATE_ENABLED)==TBSTATE_ENABLED)
#define IS_INDETERMINATE(button) (((button).fsState&TBSTATE_INDETERMINATE)==TBSTATE_INDETERMINATE)
#define IS_PRESSED(button)       (((button).fsState&TBSTATE_ENABLED)==TBSTATE_PRESSED)
#define IS_VISIBLE(button)       (((button).fsState&TBSTATE_HIDDEN)==0)
#define IS_WRAP(button)          (((button).fsState&TBSTATE_WRAP)==TBSTATE_WRAP)

///////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
