#if !defined(AFX_FLOWCHARTPANE_H__19CAEEEC_E8E3_4FDD_9449_D2972181940C__INCLUDED_)
#define AFX_FLOWCHARTPANE_H__19CAEEEC_E8E3_4FDD_9449_D2972181940C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlowChartPane.h : header file
//
#include "FlowChartwnd.h"
#include "MathFlow.h"
#include "TermPlanDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CFlowChartPane view

class CFlowChartPane : public CView
{
public:
	CFlowChartPane();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFlowChartPane)

// Attributes
public:
	CFlowChartWnd m_wndFlowChart;
	COleDropTarget m_oleDropTarget;	//to support drag & drop of shapes
	OLE_COLOR m_color;


// Operations
public:
	BOOL UpdateProcess(const CString &strOld, const CString &strNew);
	BOOL DeleteProcess(const CString &strName);
	CMathFlow* GetCurMathFlowPtr();
	void SetMathFlowPtr(const std::vector<CMathFlow>* pFlowVect);
	const std::vector<CMathFlow>* GetMathFlowPtr() const
	{
		return m_pFlowVect;
	}

	void SetDocumentPtr(CDocument* pDoc)
	{
		m_pMyDocument = (CTermPlanDoc*)pDoc;
		this->m_pDocument = pDoc;
	}
	CTermPlanDoc* GetDocumentPtr() const
	{
		return m_pMyDocument;
	}
	void SetFlowName(const CString& strName);

	const CString& GetFlowName() const
	{
		return m_strFlowName;
	}

	const CString& GetFlowChartFileName() const
	{
		return m_strFlowChartFilePath;
	}

	void InitFlowChart();
	BOOL SaveFlowChartData();

	virtual ~CFlowChartPane();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlowChartPane)
	public:
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL
// Implementation
protected:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFlowChartPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	afx_msg void OnAddItem();
	afx_msg void OnEditItem();
	afx_msg void OnDelItem();
	afx_msg void OnProp();
	afx_msg void OnPrint();

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()

	LPDISPATCH m_curItem;
	CString m_strFlowChartFilePath;
	CString m_strFlowName;
	CString m_strProcessName;
	CTermPlanDoc* m_pMyDocument;

	void SetBoxColor(LPDISPATCH pBox);
	void SetArrowsOfBoxColor(LPDISPATCH pBox);

	void OnBoxDeleted(LPDISPATCH pBox);
	void OnArrowDeleted(LPDISPATCH arrow);
	void OnBoxClicked(LPDISPATCH pBox, long button, long x, long y);
	void OnArrowClicked(LPDISPATCH pBox, long button, long x, long y);
	void OnBoxDblClicked(LPDISPATCH box, long button, long x, long y);
	void OnArrowDblClicked(LPDISPATCH arrow, long button, long x, long y);
	void OnArrowCreated(LPDISPATCH pBox);
	void OnArrowOrgChanged(LPDISPATCH arrow, LPDISPATCH obj);
	void OnArrowDestChanged(LPDISPATCH arrow, LPDISPATCH obj);

	const std::vector<CMathFlow>* m_pFlowVect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOWCHARTPANE_H__19CAEEEC_E8E3_4FDD_9449_D2972181940C__INCLUDED_)
