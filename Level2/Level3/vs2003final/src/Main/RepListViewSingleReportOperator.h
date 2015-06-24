#pragma once
#include "replistviewbaseoperator.h"
class ARCGridCtrl;

//////////////////////////////////////////////////////////////////////////
//used for single report list view

class CRepListViewSingleReportOperator :
	public CRepListViewBaseOperator
{
public:
	CRepListViewSingleReportOperator(ARCGridCtrl *pListCtrl, CARCReportManager* pARCReportManager,CRepListView *pListView);
	~CRepListViewSingleReportOperator(void);

public:
	virtual void LoadReport(LPARAM lHint, CObject* pHint);


	void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	CARCReportManager* GetARCReportManager();

	void GetFileReportTypeAndExtension();
	void LoadListData();
	void SetListHeader();

	CString GetAbsDateTime(LPCTSTR elaptimestr, BOOL needsec = TRUE);
	
	virtual void OnListviewExport();


	virtual void LoadReportProcessFlow(CMathResultManager* pManager, enum ENUM_REPORT_TYPE _enumRepType);
	virtual void ShowReportProcessFlow(MathResult& result);

	CRepListView *getListView() const;
protected:
	int m_nReportFileType;
	CString m_sExtension;
	std::vector<bool> m_vFlag;
	bool m_bSummary;
	int m_nColumnCount;


};
