#pragma once

#include "DirectRoutingListCtrlEx.h"
#include "DialogResize.h"
#include "Resource.h"
#include "..\InputAirside\DirectRouting.h"
#include "NodeViewDbClickHandler.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgDirectRoutingCriteria dialog

class CDlgDirectRoutingCriteria : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgDirectRoutingCriteria)

public:
	CDlgDirectRoutingCriteria(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDirectRoutingCriteria();


	int m_nProjID;
	CDirectRoutingListCtrlEx m_wndListCtrl;

	CToolBar m_wndToolBar;
	CDirectRoutingList* m_pDirectRoutingList;
	CStringList m_strListSegFrom;
	CStringList m_strListSegTo;
	CStringList m_strListSegMax;

	//typedef std::map<CString,int> SegMap;
	//typedef std::map<CString,int>::iterator SegMapIter;
	//SegMap m_SegFromMap;
	//SegMap m_SegToMap;
	//SegMap m_SegMaxMap;
	CString m_strSegFromFirst;
	CString m_strSegToFirst;
	CString m_strSegMaxFirst;
	int m_nSegFromIDFirst;
	int m_nSegToIDFirst;
	int m_nSegMaxIDFirst;

	virtual BOOL OnInitDialog();
	void InitToolBar();
	void InitListCtrl();
	void UpdateToolBar();

	// Dialog Data
	enum { IDD = IDD_DIALOG_DIRECTROUTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewItem();
	afx_msg void OnDeleteItem();
//	afx_msg void OnEditItem();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSave();
	void SetListContent();
	void GetStrListSegFrom();
	void GetStrListSegTo(int nSegFromID);
	void GetStrListSegMax(int nSegFromID,int nSegToID);
	bool IsInStrListSegFrom(CString str);
	bool IsInStrListSegTo(CString str);
	bool IsInStrListSegMax(CString str);
	void GetStrSegFirst();

public:
	afx_msg void OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMDblclkListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
};
