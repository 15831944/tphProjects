#pragma once
#include "Landside/SequentialLightSyn.h"
#include "MFCExControl/CoolTree.h"
#include "../AirsideGUI/UnitPiece.h"

// CDlgSequentialLightSyn dialog
class LandsideFacilityLayoutObjectList;
class CTermPlanDoc;
class CDlgSequentialLightSyn : public CXTResizeDialog , public CUnitPiece
{
	DECLARE_DYNAMIC(CDlgSequentialLightSyn)

public:
	class TreeNodeData
	{
	public:
		enum TreeNodeType
		{
			RuleName_Node,
			IntersectionList_Node,
			Intersection_Node,
			Speed_Node,
		};

		TreeNodeData()
			:m_emType(RuleName_Node)
		{

		}
		~TreeNodeData()
		{

		}

		DWORD m_dwData;
		TreeNodeType m_emType;
	};

	CDlgSequentialLightSyn(CTermPlanDoc *tmpDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSequentialLightSyn();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEQUENTIALLIGHTSYN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);


	afx_msg void OnBtnSave();
	afx_msg void OnCmdNewItem();
	afx_msg void OnAddIntersect();
	afx_msg void OnDeleteIntersection();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	DECLARE_MESSAGE_MAP()


	void InsertRuleItem(HTREEITEM hItem,SequentialLightRule* pRule);
	void InsertNodeItem(HTREEITEM hItem,SequetailLightNodeRoute* pNode);
	void OnInitToolbar();
	void UpdateToolBarState();
	void LoadTreeContent();

	void DeleteTreeNodeData(TreeNodeData* pNodeData);
	void ClearTreeNodeData();
	int GetProjectID()const;
	LandsideFacilityLayoutObjectList* GetLandsideLayoutObjectList();
private:
	CToolBar  m_wndToolbar;
	CCoolTree m_wndTreeCtrl;
	std::vector<TreeNodeData*> m_vNodeData;
	SequentialLightSyn m_sequentialLightSyn;
	CTermPlanDoc* m_pDoc;
};
