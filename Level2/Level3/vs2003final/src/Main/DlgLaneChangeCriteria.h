#pragma once
#include <MFCExControl/CoolTree.h>
#include "Landside/NewLaneChangeCriteriaContainer.h"

class CTermPlanDoc;
class InputTerminal;
// CDlgLaneChangeCriteria dialog

class CDlgLaneChangeCriteria : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgLaneChangeCriteria)

public:
	CDlgLaneChangeCriteria(CTermPlanDoc* pDoc, InputTerminal* pInTerm,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLaneChangeCriteria();

	class TreeNodeData
	{
	public:
		enum TreeNodeType
		{
			STRETCH_Node,
			INTERSECTION_Node,
			APPROACHVEHICLE_Node,
			RANDOMPERCENT_Node,
			START_Node,
			RANDOMVEHICLE_Node,
			CYCLETIME_Node,
			OVERTAKING_Node,
			OVERTAKEN_Node,
			SLOWERPERCENT_Node
		};

		TreeNodeData()
			:m_emType(STRETCH_Node)
		{

		}
		~TreeNodeData()
		{

		}

		DWORD m_dwData;
		TreeNodeType m_emType;
	};
// Dialog Data
	enum { IDD = IDD_DIALOG_LANECHANGECRITERIA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	

	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnLvnTreeItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBtnSave();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point );
	afx_msg void OnLbnSelchangeListBox();
	afx_msg LRESULT OnLeftDoubleClick( WPARAM wParam, LPARAM lParam );
	LRESULT DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

	void OnInitToolbar();
	void SetListBoxContent();
	void SetTreeContent(landsideLaneChangeCriteria::NewCriteriaType emType);
	void UpdateToolBarState();

	void InsertApproachItem(LandsideApproachCriteria* pCriteria);
	void InsertRandomItem(LandsideRandomCriteria* pCriteria);
	void InsertOvertakingItem(LandsideOvertakingCriteria* pCriteria);

	void InsertOvertakingSlowerVehicleItem(HTREEITEM hItem,LandsideOverTakingVehicle* pOvertaking);
	void InsertOvertakenSlowerVehicleItem(HTREEITEM hItem,LandsideOvertakenVehicle* pOvertaken);

	HTREEITEM InsertStretchItem(landsideLaneChangeCriteria* pCriteria);


	void InsertIntersectionItem(HTREEITEM hItem,LandsideApproachIntersection* pIntersection);
	void InsertApproachVehicleItem(HTREEITEM hItem, LandsideApproachVehicle* pVehicle);

	void InsertRandomVehicle(HTREEITEM hItem, LandsideRandomVehicle* pVehicle);

private:
	void DeleteTreeNodeData( TreeNodeData* pNodeData );
	void ClearTreeNode();
private:
	CListBox  m_wndListCtrl;
	CCoolTree    m_wndTreeCtrl;
	CToolBar       m_wndToolBar;

	//input data 
	NewLaneChangeCriteriaContainer m_laneChangeCriteria;
	CTermPlanDoc* m_pDoc;
	InputTerminal* m_pInTerm;
	std::vector<TreeNodeData*> m_vNodeData;
	std::vector<TreeNodeData::TreeNodeType> m_canDeleteNode;
	std::vector<TreeNodeData::TreeNodeType> m_canNewNode;
};
