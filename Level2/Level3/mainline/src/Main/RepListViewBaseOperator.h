#pragma once

/************************************************************************
	this class is used for Report List View,
	responsibility for showing list data

************************************************************************/

class CARCReportManager;
class CListCtrl;
class CRepListView;
class CTermPlanDoc;
class CReportParameter;
class CMathResultManager;
struct MathResult;
class CRepListViewBaseOperator
{
public:
	CRepListViewBaseOperator(CListCtrl *pListCtrl,CARCReportManager *pReportManager,CFormView *pListView);
	~CRepListViewBaseOperator(void);



	virtual void LoadReport(LPARAM lHint, CObject* pHint) = 0;

	//move from RepListView
	void ResetAllContent();
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnListviewExport();

	virtual void LoadReportProcessFlow(CMathResultManager* pManager, enum ENUM_REPORT_TYPE _enumRepType);
	virtual void ShowReportProcessFlow(MathResult& result);

protected:
	CListCtrl& GetListCtrl();
	CReportParameter* GetReportParameter();
	CTermPlanDoc *GetTermPlanDoc();
	void Invalidate(BOOL bErase = TRUE);

protected:
	CListCtrl *m_pListCtrl;
	CARCReportManager *m_pReportManager;
	CFormView *m_pListView;
	
};
