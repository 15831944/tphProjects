#pragma once
#include ".\MFCExControl\CoolTree.h"
#include "..\Compare\CmpReport.h"
#include "Resource.h"
#include "CoolBtn.h"

class CCmpReportTreeView : public CFormView
{
	DECLARE_DYNCREATE(CCmpReportTreeView)

protected:
	CCmpReportTreeView();
	virtual ~CCmpReportTreeView();

public:
	virtual void OnDraw(CDC* pDC);
	enum { IDD = IDD_CMPREPORTTREEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

public:
	void SetCmpReport(CCmpReport* pCmpReport){ m_pCmpReport = pCmpReport; }
	void InitRootItems();
	void UpdateWholeTree();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	CCoolTree m_propTree;
	CButton m_btnRun;
	CButton m_btnCancel;
	CFont m_font;
	CCoolBtn	m_btnMulti;
	CImageList m_imageList;

	HTREEITEM m_hBasicInfo;
	HTREEITEM m_hProjName;
	HTREEITEM m_hProjDesc;
	HTREEITEM m_hModelRoot;
	HTREEITEM m_hReportRoot;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRun();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChooseMenu(UINT nID);


	CString GetRegularDateTime(LPCTSTR elaptimestr, bool needsec = true);
	void RunCompareReport();
	void AddModel();
	void AddReport();
	void EditReport();
	void DeleteSelectedModel();
	void DeleteReport();
	void DeleteAllModel();
	BOOL CheckData();
	void RemoveSubItems(HTREEITEM pItem);
	void UpdateSubItems(HTREEITEM pItem);
	void ReloadReportDetailSubItems(const CReportToCompare &report, HTREEITEM hReport);

	CMenu m_nMenu, *m_pSubMenu;

public:
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	void OnClickMultiBtn();
private:
	void InitCooltreeNodeInfo(CWnd* pParent,COOLTREE_NODE_INFO& CNI,BOOL bVerify=TRUE);
	void OnReportSavePara();
	void OnReportLoadPara();
	void OnReportSaveReport();
	void OnReportLoadReport();
	void SavePara();
	void LoadPara();
	void SaveReport();
	void LoadReport();
	void ChangeFocusReport();
protected:
	CCmpReport* m_pCmpReport;
};


