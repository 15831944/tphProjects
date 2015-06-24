// RepListView.h : header file
//
#pragma once
#include "../MFCExControl/SortableHeaderCtrl.h"
//#include "../MFCExControl/ARCGridCtrl.h"
#include "reports\ReportType.h"
#include "PrintableListCtrl.h"
#include "MathResultManager.h"
class CReportParameter;
class CRepListViewBaseOperator;
class CRepListViewSingleReportOperator;
class CRepListViewMultiRunReportOperator;
class CRepListView : public CFormView
{
protected:
	CRepListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRepListView)

public:
	enum { IDD = IDD_DIALOG_REPORT_LISTGRID };
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint); //update file type, file extension, etc;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual ~CRepListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);				//print all of the items
	void PrintSelectedItems(CDC *pDC, CPrintInfo *pInfo);	//print the selected items only
	void ResetAllContent();
	void LoadReportProcessFlow(CMathResultManager* pManager, enum ENUM_REPORT_TYPE _enumRepType);
	void ShowReportProcessFlow(MathResult& result);

	CSortableHeaderCtrl& GetSortableHeaderCtrl(){ return m_ctlHeaderCtrl;}
protected:
	
	CReportParameter* GetReportParameter();

	//CString GetAbsDateTime(LPCTSTR elaptimestr, BOOL needsec = TRUE);
	//std::vector<bool> m_vFlag;
	//void Load();
	//void SetHeader();
	//bool m_bSummary;
	//int m_nColumnCount;
	//{{AFX_MSG(CRepListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListviewMaximize();
	afx_msg void OnListviewNormal();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnListviewPrint();					//choose OnPrint() or PrintSelectedItems()
	afx_msg void OnListviewExport();				//Export a existing file to a new location
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSortableHeaderCtrl m_ctlHeaderCtrl;
	//int m_nReportFileType;
	CString m_sHeader;
	CString m_sFoot;
	//CString m_sExtension;
	CRepListViewBaseOperator *m_pRepListViewBaseOperator;
	CListCtrl m_wndListWnd;
	//ARCGridCtrl m_wndARCGrid;
};
