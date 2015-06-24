#pragma once
#include "../MFCExControl/CoolTree.h"
#include "../Landside/LandsideInitCondition.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgInitCondition dialog
class InputLandside;
class LandsideFacilityLayoutObjectList;
class CDlgInitCondition : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgInitCondition)
	class TreeNodeData
	{
	public:
		enum TreeNodeType
		{
			Type_Node,
			Vehicle_Node,
			NumLot_Node,
			TimeRange_Node,
			DepartureDist_Node,
		};

		TreeNodeData()
			:m_emType(Type_Node)
		{

		}
		~TreeNodeData()
		{

		}

		DWORD m_dwData;
		TreeNodeType m_emType;
	};
public:
	CDlgInitCondition(InputLandside* pInputLandside,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInitCondition();

// Dialog Data
	enum { IDD = IDD_DIALOG_INITCONDITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnOK();

	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnAddInitCondition();
	afx_msg void OnDeleteCondition();
	afx_msg void OnBtnSave();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	HTREEITEM InsertConditionTypeTreeItem(HTREEITEM hItem,InitConditionType* pConditionType);
	void InsertConditionDataTreeItem(HTREEITEM hItem,InitConditionData* pConditionData);
	CProbDistEntry* ChangeProbDist( HTREEITEM hItem,const CString& strDist);
	void DeleteTreeNodeData( TreeNodeData* pNodeData );
	LandsideFacilityLayoutObjectList* GetLandsideLayoutObjectList();
public:
	void OnInitToolbar();
	void UpdateToolBarState();
	void LoadTreeContent();

private:
	CToolBar m_wndToolbar;
	CCoolTree m_wndTreeCtrl;
	InputLandside*	m_pInLandside;
	LandsideInitCondition m_initCondition;
	std::vector<TreeNodeData*> m_vNodeData;
};
