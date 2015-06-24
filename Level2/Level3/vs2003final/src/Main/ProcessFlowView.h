#if !defined(AFX_PROCESSFLOWVIEW_H__606CB11C_E669_4B45_8260_E7A62DC76647__INCLUDED_)
#define AFX_PROCESSFLOWVIEW_H__606CB11C_E669_4B45_8260_E7A62DC76647__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessFlowView.h : header file
//

#include "DotNetTabWnd.h"
#include "..\common\ProjectManager.h"
#include "MathFlow.h"

/////////////////////////////////////////////////////////////////////////////
// CProcessFlowView view

class CProcessFlowView : public CView
{
protected:
	CProcessFlowView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CProcessFlowView)
public:
	void SetMathFlowPtr(std::vector<CMathFlow>* pFlowVect);
	const std::vector<CMathFlow>* GetMathFlowPtr() const
	{
		return m_pFlowVect;
	}
	BOOL SaveFlowChartData();

	BOOL InitTabWnd();
	CDotNetTabWnd m_wndDotNetTabWad;

	BOOL AddFlowChartPane(const CString& strFlowName);
	BOOL DeleteFlowChartPane(const CString& strFlowName);
	BOOL ModifyFlowChartPane(const CString& strOldName, const CString& strNewName);
// Attributes
public:
// Operations
public:
	BOOL UpdateProcess(const CString& strOld, const CString& strNew);
	BOOL DeleteProcess(const CString& strName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessFlowView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CProcessFlowView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CProcessFlowView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LRESULT OnChildDelete(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	std::vector<CMathFlow>* m_pFlowVect;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSFLOWVIEW_H__606CB11C_E669_4B45_8260_E7A62DC76647__INCLUDED_)
