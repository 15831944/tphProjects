#pragma once
#include "replistviewbaseoperator.h"


class OnboardReportListView;


class COnboardRepListViewBaseOperator :
	public CRepListViewBaseOperator
{
public:
	COnboardRepListViewBaseOperator(CListCtrl *pListCtrl, CARCReportManager* pARCReportManager,OnboardReportListView *pListView);
	~COnboardRepListViewBaseOperator(void);
	
public:	
	virtual void LoadReport(LPARAM lHint, CObject* pHint);
	void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

protected:

	CARCReportManager* GetARCReportManager();
	void GetFileReportTypeAndExtension();
	void LoadListData();
	void SetListHeader();

	virtual void OnListviewExport();

	OnboardReportListView *getListView() const;
protected:
	int m_nReportFileType;
	CString m_sExtension;
	std::vector<bool> m_vFlag;
	bool m_bSummary;
	int m_nColumnCount;
};
