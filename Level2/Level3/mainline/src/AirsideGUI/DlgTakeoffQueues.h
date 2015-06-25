#pragma once
#include "../InputAirside/TakeoffQueues.h"
#include "../InputAirside/HoldPosition.h"
#include "..\MFCExControl\CoolTree.h"
#include "..\MFCExControl\SimpleToolTipListBox.h"
#include "..\MFCExControl\ARCTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"


typedef struct
{
	int nOterObjID;
	int nIndex;
}combdata;

class CAirportDatabase;
class CDlgTakeoffQueues : public CXTResizeDialog
{
public:
	CDlgTakeoffQueues(int nProjID, CAirportDatabase* pAirportDB, CWnd* pParent = NULL);
	virtual ~CDlgTakeoffQueues();

	enum { IDD = IDD_TAKEOFFQUEUE  };

protected:
	DECLARE_MESSAGE_MAP()

	CARCTreeCtrl	m_wndQueueNodesTree;
	int			m_nProjID;
	CSimpleToolTipListBox m_TakeoffListBox;
	CToolBar m_TakeoffToolBar;
	CToolBar m_TakeoffQueueToolBar;
	int m_nTakeoffPosiCount;
	TakeoffQueues m_TakeoffQueues;
	CStringList m_strListTakeoffQueues;
	CStringList m_strListNode;
	std::vector<IntersectionNode> m_vNodeList;
	std::vector<IntersectionNode> m_vNodeListTemp;
	std::vector<combdata> m_vCombdata;
	CAirportDatabase* m_pAirportDB;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSave();
	void InitTakeoffToolBar();
	afx_msg void OnNewTakeoff();
	afx_msg void OnDelTakeoff();
	afx_msg void OnNewQueue();
	afx_msg void OnDelQueue();
	//afx_msg void OnEditQueue();
	void UpdateToolBar();
	void GetListByTaxiwayID(int nTaxiwayID);
	void InitQueueNodesTree();
	void InitTakeoffListBox();
	void TraverseOne(HTREEITEM hRootTreeItem,int nParentID,HTREEITEM& hTreeItem);
	void TraverseAll(HTREEITEM hRootTreeItem,int nParentID,HTREEITEM& hTreeItem);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLbnSelchangeListTakeoff();

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTvnSelchangedTreeQueuenodes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTreeQueuenodes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
