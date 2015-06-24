#pragma once
#include "afxcmn.h"

#include "../MFCExControl/XListCtrl.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../AirsideReport/AirsideFlightDelayReport.h"
#include <vector>
using namespace std;

// CAirsideReportListView form view

class CAirsideReportListView : public CFormView
{
	DECLARE_DYNCREATE(CAirsideReportListView)

protected:
	CAirsideReportListView();           // protected constructor used by dynamic creation
	virtual ~CAirsideReportListView();

public:
	enum { IDD = IDD_AIRSIDEREPORTLISTVIEW };

public:
	void ResetAllContent();



#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	void PrintSelectedItems(CDC *pDC, CPrintInfo *pInfo);	//print the selected items only
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);				//print all of the items
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CXListCtrl m_lstCtrl;
	CSortableHeaderCtrl m_wndSortableHeaderCtrl;
	CString m_sHeader;
	CString m_sFoot;
	CString m_sExtension;
	CXListCtrl& GetListCtrl(){ return m_lstCtrl;}
	DECLARE_MESSAGE_MAP()
public:
	CAirsideFlightDelayReport::FltDelayItem GetFltTypeDelayResultItem(vector<CAirsideFlightDelayReport::FltDelayItem>& fltTypeDelayResult, int nIndex);
	CString GetObjectName(int objectType,int objID);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnXListCtrlCheckBoxClicked(WPARAM wParam,LPARAM lParam);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnAirsidereportlistMax();
	afx_msg void OnAirsidereportlistNormal();
	afx_msg void OnAirsidereportlistPrint();
	afx_msg void OnAirsidereportlistExport();
	afx_msg void OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
};


