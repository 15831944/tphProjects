#pragma once
#include "../AirsideReport/CARC3DChart.h"


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
	CARC3DChart	m_MSChartCtrl;
	CComboBox	m_comboCategory;
public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


