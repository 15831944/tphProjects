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
	CListCtrl m_wndListCtrl;
	CSortableHeaderCtrl m_ctlHeaderCtrl;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChooseMenu(UINT nID);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	CCmpReport* m_pCmpReport;
	int m_categoryType;
};


