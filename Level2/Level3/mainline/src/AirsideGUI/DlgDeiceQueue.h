#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgDeiceQueue dialog
class DeicePadQueueList;
class DeicePadQueueItem;
class DeicePadQueueRoute;
class ALTObject;
class CAirportGroundNetwork;

class CDlgDeiceQueue : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgDeiceQueue)

public:
	CDlgDeiceQueue(int nProjID,int nAirportID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDeiceQueue();

// Dialog Data
	enum { IDD = IDD_DIALOG_DEICEING_QUEUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLbnSelchangeListDeicepad();
	afx_msg void OnTvnSelchangedTreeRoutepath(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSave();

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_wndListbox;
	CARCTreeCtrl m_wndTreeCtrl;
	CToolBar m_wndDeicePadToolbar;
	CToolBar m_wndQueueToolbar;
	CStringList m_strDeicePadList;
	int m_nProjID;
	int m_nAirportID;
	DeicePadQueueList* m_QueueList;
	DeicePadQueueItem* m_pCurDeiceItem;
	DeicePadQueueRoute* m_pCurRoute;
	ALTObjectList m_vPadList;

public:
	void GetAllDeicePad();
	void OnInitToolbar();
	void OnNewDeicePad();
	void OnDelDeicePad();
	void OnNewTaxiWayPath();
	void OnDelTaxiWayPath();
	int GetTreeCount(HTREEITEM hItem);
	HTREEITEM GetParentItem(HTREEITEM hItem,int lCount);
	void SetComboBoxItem(CComboBox* pCB);
	void DisplayQueue(DeicePadQueueItem* pDeiceItem);
	void DisplayDeicePad();
	void OnChangeToolbarStatus();
	int GetIndexByTree(HTREEITEM hItem);
	void setTreeItemText(HTREEITEM hItem,ALTObject* AltObj,CString strItemText);
};
