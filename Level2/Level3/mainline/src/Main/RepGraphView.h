#if !defined(AFX_REPGRAPHVIEW_H__CDCE85B9_A9D4_48BE_9E86_F738196D7FAD__INCLUDED_)
#define AFX_REPGRAPHVIEW_H__CDCE85B9_A9D4_48BE_9E86_F738196D7FAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RepGraphView.h : header file
//
//{{AFX_INCLUDES()
#include "mschart.h"
//}}AFX_INCLUDES

/////////////////////////////////////////////////////////////////////////////
// CRepGraphView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "resource.h"
#include "reports\ReportType.h"
#include "common/csv_file.h"
#include "common/termfile.h"
#include "TermPlanDoc.h"
#include "MathResultManager.h"
#include "afxwin.h"

class CRepGraphViewBaseOperator;

class CRepGraphView : public CFormView
{
protected:
	//ArctermFile chartFile;
	CRepGraphView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRepGraphView)

// Form Data
public:
	//{{AFX_DATA(CRepGraphView)
	enum { IDD = IDD_DIALOG_REPORT_GRAPH };
	CStatic	m_toolbarcontenter;
	CComboBox	m_comboChartType;
	CComboBox	m_comboChartSelect;
	CMSChart1	m_MSChartCtrl;
	//}}AFX_DATA

// Attributes
public:
	COLORREF m_ColorRef;
	RECTL m_rclBounds;

	CRect rectOld;
	CRepGraphViewBaseOperator *m_pRepGraphViewOperator;
// Operations
public:
	void ShowReportProcessFlow(MathResult& result);
	void LoadReportProcessFlow(CMathResultManager *pManager, enum ENUM_REPORT_TYPE _enumRepType);
//	void DealRowLabel(CString &strRowLabel);
	void ResetAllContent();

	CMSChart1& GetMSChartCtrl(){return m_MSChartCtrl;}
	CComboBox& GetComboCharType() {return m_comboChartType;}
	CComboBox& GetComboCharSelect(){return m_comboChartSelect;} 
	CComboBox& GetComboSubType(){return m_comSubType;} 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRepGraphView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//CString GetAbsDateTime(long seconds, BOOL needsec);
	//CString NoUnderline(CString _str);
	//void SpaceThroughputCount();
	//void PaxDensityM2Pax();
	//void PaxDensityPaxM2();
	//void PaxDensityCount();
	//void BagCountMinSummary();
	//void BagCountCombinedSummary();
	//void BagCountMaxSummary();
	//void BagCountAvgSummary();
	//void BagCountDetailed();
	//void BagWaitTimeSummary();
	//void BagWaitTimeDetailed();
	//void ActBKDownPerBKDetailed();
	//void ActBKDownTimeBKDetailed();
	//void ActBKDownPerBKSummary();
	//void ActBKDownTimeBKSummary();
	//void ServiceTimeSummary();
	//void ServiceTimeDetailed();
	//void DurationSummary();
	//void DurationDetailed();
	//void AvgQueueLenMaxQueueTimeHisDetailed();
	//void AvgQueueLenTimeMaxQueDetailed();
	//void AvgQueueLenDetailed();
	//void AvgQueueLenSummary();
	//void AvgQueueLenTimeMaxQueSummary();
	//void AvgQueueLenMaxQueTimeHisSummary();
	//
	//void QueueLengthCombinedSummary();
	//void QueueLengthMaxSummary();
	//void QueueLengthAvgSummary();
	//void QueueLengthMinSummary();
	//void QueueLengthDetailed();

	//void ConveyorWaitLengthDetailed();
	//void ConveyorWaitLengthMaxSummary();
	//void ConveyorWaitLengthAvgSummary();
	//void ConveyorWaitLengthMinSummary();
	//void ConveyorWaitLengthCombinedSummary();

	//void ThroughputProcPerHourSummary();
	//void ThroughputGroupPerHourSummary();
	//void ThroughputPaxPerProcSummary();
	//void ThroughputTotalPaxSummary();
	//void ThroughputAveTimeDetailed();
	//void ThroughputAvePaxDetailed();
	//void ThroughputTotalPaxDetailed();

	//void QueueTimeSummary();
	//void QueueTimeDetailed();

	//void ConveyorWatiTimeSummary();
	//void ConveyorWatiTimeDetailed();

	void InitToolbar();
	CToolBar m_ToolBar;
	//void PaxCountTimeRange();
	//void PaxCountTotalPaxCount();
	//void PaxCountMaxPaxCount();
	//void PaxCountPaxPeriod();
	//void PaxCountDetailed();
	//void CollisionCount();
	//void UtilizationPercentageSummary();
	//void UtilizationTimeAvailableSummary();
	//void UtilizationTimeSummary();
	//void UtilizationPercentageDetailed();
	//void UtilizationTimeAvailableDetailed();
	//void UtilizationTimeDetailed();
	//void DistanceTravelledDetailed();
	//void DistanceTravelledSummary();
	//void PaxLogCartCountDistribution();
	//void PaxLogBagCountDistribution();
	//void PaxLogGroupSizeDistribution();
	//CString FormatTime(int nMinute);
	//void PaxLogSysExitDistribution();
	//void PaxLogSysEntryDistribution();
	//void SetHeadXYTitle(CString strTitle, CString strXAxisTitle, CString strYAxisTitle,CString strFootnote);
	//void AcOpArrivalCount();
	//void AcOpDepartCount();
	//void AcOpTotal();
	//void BagDepartDistCount();
	//void BagArrivalDistCount();
	//void BagDeliveryCount();
	//void BagDeliverySummary();
	//void FireEvacuationDetailed();
	//void FireEvacuationSummary();
	//void MissFlightDetailed();
	//void BillboardExposureIncidenceDetailed();
	//void BillboardExposureIncidenceSummary();

	//void BillboardLinkedProcIncrementalVisitDetailed();
	//void BillboardLinkedProcIncrementalVisitSummary();
	//int m_nSummary;
	//void Load();
	//int GetInterval(char *time);
	//void getTime( const CString& strText, ElapsedTime& begineTime, ElapsedTime& endTime ) const;

	//LoadReportType GetLoadReportType(void) const;
	//const CString& GetUserLoadReportFile( void ) const;
	virtual ~CRepGraphView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CRepGraphView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelColorBtn();
	afx_msg void OnSelchangeChartTypeCombo();
	afx_msg void OnSelchangeChartSelectCombo();
	afx_msg void OnPrintBtn();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnGraphviewMaximax();
	afx_msg void OnGraphviewNormal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//int m_nReportFileType;
public:
	CComboBox m_comSubType;
	afx_msg void OnCbnSelChangeReportSubType();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPGRAPHVIEW_H__CDCE85B9_A9D4_48BE_9E86_F738196D7FAD__INCLUDED_)
