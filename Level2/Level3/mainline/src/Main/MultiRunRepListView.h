#pragma once
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "MultiRunsReportDataLoader.h"

class ArctermFile;

namespace MultiRunsReport
{
	struct PaxData;
}

// CMultiRunRepListView view
class CMultiRunRepListView : public CListView
{
protected:
	DECLARE_DYNCREATE(CMultiRunRepListView)
	DECLARE_MESSAGE_MAP()
	CMultiRunRepListView();
	virtual ~CMultiRunRepListView();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);

	void SetSummaryPaxDataContents(MultiRunsReport::Summary::PaxDataList& dataList, std::vector<int>& vSimResults);
	void SetDetailedPaxDataContents(MultiRunsReport::Detail::PaxDataList& dataList, std::vector<int>& vSimResults);
	void SetDetailedTimeDataContents(MultiRunsReport::Detail::TimeDataList& dataList, int _nRangeCount );
	void SetDetailedOccupancyDataContents(MultiRunsReport::Detail::OccupnacyDataList& dataList, int _nRangeCount );
	void SetSummaryQueueTimeContents(const CString& strPaxType);
	void SetSummaryCriticalQueueContents();
	void SetSummaryTimeServiceContents(const CString& strPaxType);

	CSortableHeaderCtrl m_ctlHeaderCtrl;
	int m_nReportFileType;
	CString m_sHeader;
	CString m_sFoot;
	CString m_sExtension;
	std::vector<bool> m_vFlag;
	int m_nColumnCount;
void SetDetailOccupancyDataWithNoRanger(MultiRunsReport::Detail::OccupnacyDataList& , int _nRangeCount);
};


