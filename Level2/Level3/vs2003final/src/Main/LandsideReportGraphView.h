#pragma once
#include "../AirsideReport/CARC3DChart.h"

class CTermPlanDoc;
// LandsideReportGraphView form view

class LandsideReportGraphView : public CFormView
{
	DECLARE_DYNCREATE(LandsideReportGraphView)

protected:
	LandsideReportGraphView();           // protected constructor used by dynamic creation
	virtual ~LandsideReportGraphView();

public:
	enum { IDD = IDD_DIALOG_REPORT_GRAPH_LANDSIDE };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void ResetAllContent();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CStatic	m_toolbarcontenter;
	CComboBox	m_comboChartType;
	CComboBox	m_comboChartSelect;
	CComboBox m_comSubType;
	CARC3DChart	m_MSChartCtrl;
	void InitToolbar();
	CToolBar m_ToolBar;
	RECTL m_rclBounds;


protected:

	CTermPlanDoc* GetDocument(){ return (CTermPlanDoc*) m_pDocument; }

public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnSelColorBtn();
	afx_msg void OnPrintBtn();

	afx_msg void OnSelchangeChartSelectCombo();
	afx_msg void OnGraphviewMaximax();
	afx_msg void OnGraphviewNormal();
	afx_msg void OnCbnSelendokChartTypeCombo();








};


