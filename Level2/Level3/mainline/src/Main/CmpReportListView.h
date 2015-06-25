#pragma once

#include "..\MFCExControl\SortableHeaderCtrl.h"
#include "..\MFCExControl\XListCtrl.h"
#include "..\Compare\CmpReport.h"
#include "Resource.h"


class CCmpReportListView : public CFormView
{
	DECLARE_DYNCREATE(CCmpReportListView)

protected:
	CCmpReportListView();           // protected constructor used by dynamic creation
	virtual ~CCmpReportListView();

public:
	enum { IDD = IDD_CMPREPORTLISTVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CCmpReport* m_pCmpReport;
	int m_categoryType;
	CXListCtrl m_wndListCtrl;
	CSortableHeaderCtrl m_ctlHeaderCtrl;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString m_sHeader;
	CString m_sFoot;
	CString m_sExtension;
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChooseMenu(UINT nID);

	afx_msg void OnMenuItemMax();
	afx_msg void OnMenuItemNormal();
	afx_msg void OnMenuItemPrint();
	afx_msg void OnMenuItemExport();
	afx_msg void OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);

	void OnPrint(CDC* pDC, CPrintInfo* pInfo);				//print all of the items
	void PrintSelectedItems(CDC *pDC, CPrintInfo *pInfo);	//print the selected items only
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL ExportListCtrlToCvsFile(ArctermFile& _file);
	BOOL ExportListData(ArctermFile& _file);
};


