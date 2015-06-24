#pragma once
#include "..\Landside\IntersectionTrafficCtrlList.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\Main\ListBoxEx.h"
#include "TermPlanDoc.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgIntersectionTrafficCtrl dialog

class CDlgIntersectionTrafficControlManagement : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgIntersectionTrafficControlManagement)

public:
	CDlgIntersectionTrafficControlManagement(CTermPlanDoc *tmpDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgIntersectionTrafficControlManagement();

// Dialog Data
	enum { IDD = IDD_DIALOG_INTERSECTIONNODE_TRAFFICCONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CIntersectionTrafficControlManagement *m_pIntersectionTrafficCtrlList;
	CToolBar m_toolBarIntersection;
	CToolBar m_toolBarItem;
 	CListBox m_listIntersectionNode;
 	CListCtrlEx m_listCtrlItem;
	CTermPlanDoc *m_pDoc;
public:
	
	void OnInitToolBar();
	void OnInitItemList();
	void SetIntersectionListContent();
	void SetItemListContent();
	void OnItemUp();
	void OnItemDown();

	int IntersectionExist(CString strName);

	afx_msg void OnBnClickedBnSave();
	afx_msg void OnBnClickedOk();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickSyn();

	afx_msg void OnLbnSelchangeListIntersectionnode();
// 	afx_msg void OnHdnItemdblclickListTrafficcontrol(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListTrafficcontrol(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListTrafficcontrol(NMHDR *pNMHDR, LRESULT *pResult);


};
