#pragma once

class CMSChart1;
class CRepGraphView;
class CTermPlanDoc;
class CComboBox;
struct MathResult;
class CMathResultManager;

enum MultiRunSummarySubReportType
{
	SubType_Average = 0,
	SubType_Minimum,
	SubType_Maximum,
	SubType_Q1,
	SubType_Q2,
	SubType_Q3,
	SubType_P1,
	SubType_P5,
	SubType_P10,
	SubType_P90,
	SubType_P95,
	SubType_P99,
	SubType_Sigma,


	//before this line
	SubType_Count
};
const CString MultiRunSummarySubReportTypeName[] = 
{
	_T("Average"),
	_T("Minimum"),
	_T("Maximum"),
	_T("Q1"),
	_T("Q2"),
	_T("Q3"),
	_T("P1"),
	_T("P5"),
	_T("P10"),
	_T("P90"),
	_T("P95"),
	_T("P99"),
	_T("Sigma"),



	//before this line
	_T("Count")
};

class CRepGraphViewBaseOperator
{
public:
	CRepGraphViewBaseOperator(CRepGraphView *pGraphView);
	~CRepGraphViewBaseOperator(void);


public:
	virtual void LoadReport(LPARAM lHint, CObject* pHint) = 0;

	virtual void ShowReportProcessFlow(MathResult& result);
	virtual void LoadReportProcessFlow(CMathResultManager *pManager, enum ENUM_REPORT_TYPE _enumRepType);

	virtual void OnSelchangeChartSelectCombo();
	virtual void OnCbnSelChangeReportSubType();
protected:
	CMSChart1& m_MSChartCtrl;
	CMSChart1& GetMSChartCtrl();
	CTermPlanDoc *GetTermPlanDoc();

	CComboBox& m_comboChartType;
	CComboBox& m_comboChartSelect;
	CComboBox& m_comboSubType;
protected:
	CRepGraphView *m_pGraphView;

};
