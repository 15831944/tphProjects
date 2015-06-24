#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "LoadedStateAndShapeListCtrl.h"

class CCarryonOwnership;
class InputTerminal;
class PaxLoadedStatesAndShapesList;

// DlgPaxLoadedStatesAndShapes dialog

class DlgPaxLoadedStatesAndShapes : public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgPaxLoadedStatesAndShapes)

public:
	DlgPaxLoadedStatesAndShapes(InputTerminal * pInterm,CCarryonOwnership* pCarryonOwnership,CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgPaxLoadedStatesAndShapes();

// Dialog Data
	enum { IDD = IDD_DLGPAXLOADEDSTATESANDSHAPES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

private:
	void OnListItemChanged(int nItem, int nSubItem);
	void InitListCtrl();
	void InitListBox();
	void SetListCtrl();

public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangePaxType();

	afx_msg void OnToolbarPaxTypeAdd();
	afx_msg void OnToolbarPaxTypeDel();
	afx_msg void OnToolbarPaxTypeEdit();

	afx_msg void OnDataAdd();
	afx_msg void OnDataDel();

protected:
	CListBox m_lisPaxType;
	LoadedStateAndShapeListCtrl m_wndListCtrl;
	CToolBar m_wndPaxToolBar;
	CToolBar m_wndDataToolBar;
	InputTerminal *m_pInterm;
	PaxLoadedStatesAndShapesList* m_pPaxLoadedStatesAndShapes;
	CCarryonOwnership* m_pCarryonOwnership;

};
