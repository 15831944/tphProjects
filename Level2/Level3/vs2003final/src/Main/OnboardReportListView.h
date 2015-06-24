#pragma once

#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"
// OnboardReportListView form view

class OnboardBaseParameter;
class CRepListViewBaseOperator;


class OnboardReportListView : public CFormView
{
	DECLARE_DYNCREATE(OnboardReportListView)

protected:
	OnboardReportListView();           // protected constructor used by dynamic creation
	virtual ~OnboardReportListView();

public:
	enum { IDD = IDD_DIALOG_REPORT_LISTGRID_ONBOARD };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void ResetAllContent();


protected:
	OnboardBaseParameter* GetReportParameter();


protected:
		CXListCtrl m_wndListCtrl;
		CSortableHeaderCtrl m_ctlHeaderCtrl;

		CString m_sHeader;
		CString m_sFoot;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);


public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListviewMaximize();
	afx_msg void OnListviewNormal();
	afx_msg void OnListviewPrint();					//choose OnPrint() or PrintSelectedItems()
	afx_msg void OnListviewExport();				//Export a existing file to a new location


	CRepListViewBaseOperator *m_pRepListViewBaseOperator;

protected:
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	void PrintSelectedItems(CDC *pDC, CPrintInfo *pInfo);	//print the selected items only


};


