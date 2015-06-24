#pragma once

#include "MFCExControl/CoolTree.h"
#include "../MFCExControl/XTResizeDialog.h"


class LandsideFacilityLayoutObjectList;
class InputLandside;

const UINT ID_TOOLBAR_NEW     = 10001;
const UINT ID_TOOLBAR_DELETE     = 10002;
class CDlgLandsideStrategy :  public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgLandsideStrategy)
public:
	CDlgLandsideStrategy(CWnd* pParent = NULL);
	~CDlgLandsideStrategy(void);

	class TreeNodeData
	{
	public:
		enum TreeNodeType
		{
			VEHICLE_Node,
			CURBSIDE_Node,
			PARKINGLOT_Node,
			FIRST_Node,
			PRIORITY_Node,
			FINALPRIORIY_Node,
			STOP_Node,
			SPOT_Node,
			RETURN_Node,
			RECYCLE_Node,
			PARKING_Node,
		};

		TreeNodeData()
			:m_emType(VEHICLE_Node)
		{

		}
		~TreeNodeData()
		{

		}

		DWORD m_dwData;
		TreeNodeType m_emType;
	};
	// Dialog Data
	enum { IDD = IDD_DIALOG_CURBSIDESTRATEGY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point );
	afx_msg void OnBtnSave();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeleteTreeItem( NMHDR *pNMHDR, LRESULT *pResult );
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLeftDoubleClick(WPARAM wParam, LPARAM lParam);

	void InitToolBar(void);
	void LoadTreeContent();

	virtual void SetTitleHeard() = 0;
	virtual void SetTreeContent() = 0;
	virtual void ReadData() = 0;
	virtual void SaveData() = 0;
	virtual void OnAddTreeItem() = 0;
	virtual void OnDeleteTreeItem() = 0;
	virtual void OnDisplayMenu(TreeNodeData* pNodeData,CPoint point) = 0;
	virtual void UpdateToolbar(HTREEITEM hItem) = 0;
	virtual void UpdateDeleteOperationToolbar() = 0;
	virtual void HandleTreeMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void HandLDoubleClickMessage(WPARAM wParam, LPARAM lParam) = 0;

protected:
	void DeleteTreeNodeData(TreeNodeData* pNodeData);
	void ClearTreeNodeData();
	LandsideFacilityLayoutObjectList* GetLandsideLayoutObjectList();
	InputLandside* GetInputLandside();
	bool FindFinalPriorityItem(HTREEITEM hItem, HTREEITEM& hFinalItem);
protected:
	CToolBar  m_wndToolbar;
	CCoolTree m_wndTreeCtrl;

	std::vector<TreeNodeData*> m_vNodeData;
	std::vector<TreeNodeData::TreeNodeType> m_canDeleteNode;
	std::vector<TreeNodeData::TreeNodeType> m_canNewNode;
};
