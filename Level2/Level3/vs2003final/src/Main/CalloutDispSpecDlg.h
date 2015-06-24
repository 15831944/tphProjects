#pragma once

#include "CalloutDispSpecsListCtrl.h"
#include "AirsideGUI/NodeViewDbClickHandler.h"

class CalloutDispSpecs;

class InputTerminal;

// CCalloutDispSpecDlg dialog
class CCalloutDispSpecDlg : public CDialog
{
	// Construction
public:
	CCalloutDispSpecDlg(PFuncSelectFlightType pSelFltType, int nProjID, CTermPlanDoc* pTermPlanDoc, CWnd* pParent = NULL);
	enum { IDD = IDD_DIALOG_PAXDISPPROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();

	void InitNameSetListHeader();
	void ResetDataListContent();
	CalloutDispSpecItem* GetSelCalloutDispSpecItem();
	CalloutDispSpecDataItem* GetSelCalloutDispSpecDataItem();

	void InitToolbar();
	CString GetProjPath();
	void ResetNameListContent();
	void UpdateUIState();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNameAdd();
	afx_msg void OnNameDel();
	afx_msg void OnDataAdd();
	afx_msg void OnDataDel();
	afx_msg void OnDataEdit();
	afx_msg void OnClickListDispprop(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedListNameSets(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditListNameSets(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

private:
	CStatic	m_toolbarcontenter;
	CStatic	m_toolbarcontenter2;
	CToolBar m_ToolBar;
	CToolBar m_ToolBar2;

	CListCtrl	m_listNamedSets;
	CCalloutDispSpecsListCtrl m_listctrlProp;

	CButton	m_btnBarFrame;
	CButton	m_btnBarFrame2;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CButton	m_btnSave;

	BOOL m_bNewName;

	PFuncSelectFlightType m_pSelFltType;
	int m_nProjID;
	CTermPlanDoc* m_pTermPlanDoc;
	CalloutDispSpecs& m_calloutDispSpecs;

	enum ToolbarCmdID
	{
		ID_NAME_ADD = 40000,
		ID_NAME_DEL,
		ID_NAME_EDIT,

		ID_DATA_ADD,
		ID_DATA_DEL,
		ID_DATA_EDIT,
	};
};
