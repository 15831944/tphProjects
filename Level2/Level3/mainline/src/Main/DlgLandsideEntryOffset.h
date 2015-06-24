#pragma once

#include <MFCExControl\XTResizeDialogEx.h>
#include "AirsideObjectTreeCtrl.h"
#include "TreeCtrlEx.h"
class InputLandside;
// CDlgLandsideEntryOffset dialog
namespace LandsideEntryOffset{
	class OffsetEntries;
}

class CDlgLandsideEntryOffset : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgLandsideEntryOffset)

public:
	CDlgLandsideEntryOffset(InputLandside* pInput ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLandsideEntryOffset();

// Dialog Data
	enum { IDD = IDD_DIALOG_LANDENTRYOFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);


protected:
	CAirsideObjectTreeCtrl m_ctrlTree;
	CToolBar m_wndToolBar;
	InputLandside* m_pInputLandside;
	BOOL m_bDataChanged;
	void LoadTree();


	void UpdateToolBar();


	void EditItem(HTREEITEM hitem);

	LandsideEntryOffset::OffsetEntries* getEntryList();
public:
	afx_msg void OnNewItem();
	afx_msg void OnDelItem();
	afx_msg void OnEditItem();
	afx_msg void OnNewPaxType();
	afx_msg void OnNewTimeWindow();
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult); 
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	//afx_msg void OnNMDblclkTreeEntryoffset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnPropTreeDoubleClick(WPARAM, LPARAM);
protected:
	virtual void OnOK();

	void SaveData();

	void OnDataChange(BOOL b);
	LandsideEntryOffset::OffsetEntries* m_pOffsetEntries;
public:
	afx_msg void OnBnClickedButtonSave();
};
