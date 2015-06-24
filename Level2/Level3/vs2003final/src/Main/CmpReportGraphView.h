#pragma once
#include "..\AirsideReport\CARC3DChart.h"
#include ".\compare\CmpReport.h"

class CCmpReportGraphView : public CFormView
{
	DECLARE_DYNCREATE(CCmpReportGraphView)

protected:
	CCmpReportGraphView();
	virtual ~CCmpReportGraphView();

public:
	enum { IDD = IDD_CMPREPORTGRAPHVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	CARC3DChart	m_3DChart;
	CComboBox	m_comboCategory;
public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	CCmpReport* m_pCmpReport;
	int m_categoryType;
	DECLARE_MESSAGE_MAP()
};


