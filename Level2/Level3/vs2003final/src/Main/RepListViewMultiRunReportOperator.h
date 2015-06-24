#pragma once
#include "replistviewbaseoperator.h"
#include "MultiRunsReportDataLoader.h"
class ARCGridCtrl;
namespace MultiRunsReport
{
	struct PaxData;
}
class CSortableHeaderCtrl;
//used for multiple run report list view
class CRepListViewMultiRunReportOperator :
	public CRepListViewBaseOperator
{
public:
	CRepListViewMultiRunReportOperator(ARCGridCtrl *pListCtrl, CARCReportManager* pARCReportManager,CRepListView *pListView);
	~CRepListViewMultiRunReportOperator(void);

public:
	virtual void LoadReport(LPARAM lHint, CObject* pHint);
	virtual void OnListviewExport();


	CRepListView * getListView() const;

protected:
	void SetSummaryPaxDataContents(MultiRunsReport::Summary::PaxDataList& dataList, std::vector<int>& vSimResults);
	void SetDetailedPaxDataContents(MultiRunsReport::Detail::PaxDataList& dataList, std::vector<int>& vSimResults);
	void SetDetailedTimeDataContents(MultiRunsReport::Detail::TimeDataList& dataList, int _nRangeCount );
	void SetDetailedOccupancyDataContents(MultiRunsReport::Detail::OccupnacyDataList& dataList, int _nRangeCount );
	void SetSummaryQueueTimeContents(const CString& strPaxType);
	void SetSummaryCriticalQueueContents();
	void SetSummaryTimeServiceContents(const CString& strPaxType);
	void SetDetailOccupancyDataWithNoRanger(MultiRunsReport::Detail::OccupnacyDataList& , int _nRangeCount) ;
	void GetFileReportTypeAndExtension();


	//CSortableHeaderCtrl m_ctlHeaderCtrl;
	CSortableHeaderCtrl* GetListHeaderCtrl();
	int m_nReportFileType;
	CString m_sHeader;
	CString m_sFoot;
	CString m_sExtension;
	std::vector<bool> m_vFlag;
	int m_nColumnCount;



};
