#pragma once

#include "../AirsideReport/CARC3DChart.h"
#include "afxwin.h"
class CTermPlanDoc;

// CAirsideReportGraphView form view
class AirsideFlightConflictPara;
class CAirsideReportGraphView : public CFormView
{
	DECLARE_DYNCREATE(CAirsideReportGraphView)

protected:
	CAirsideReportGraphView();           // protected constructor used by dynamic creation
	virtual ~CAirsideReportGraphView();

public:
	enum { IDD = IDD_DIALOG_REPORT_GRAPH2 };


public:
	void ResetAllContent();


public:
	CStatic	m_toolbarcontenter;
	CComboBox	m_comboChartType;
	CComboBox	m_comboChartSelect;
	CARC3DChart	m_MSChartCtrl;
	void InitToolbar();
	CToolBar m_ToolBar;
	RECTL m_rclBounds;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void InitRunwayUtilizationReport();
	void InitFlightConflictSummaryReportFilter(AirsideFlightConflictPara *pParam );
	void InitTaxiwayDelayReport();

private:
    void DeleteComboboxAllItemData(CComboBox& pComboBox);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTermPlanDoc* GetDocument(){ return (CTermPlanDoc*) m_pDocument; }
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelColorBtn();
	afx_msg void OnPrintBtn();
	afx_msg void OnSelchangeChartSelectCombo();

protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnCbnSelendokChartTypeCombo();
protected:
	CStatic m_StaSubText;
	CComboBox m_ComBoxSubType;
	CComboBox m_ComBoxSubType1;
	CComboBox m_ComBoxSubType2;
	CComboBox m_ComBoxSubType3;
	CComboBox m_ComBoxSubType4;
public:
	afx_msg void OnSelChangerChartSubType();
	afx_msg void OnSelChangerChartSubTypes();
};


