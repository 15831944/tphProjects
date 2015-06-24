#pragma once
#include "../Landside/LandsideLaneTollCriteria.h"
#include "../MFCExControl/CoolTree.h"
#include "../CommonData/Fallback.h"
#include "MFCExControl/XTResizeDialog.h"

// CDlgLaneTollCriteria dialog
class CTermPlanDoc;
class CDlgLaneTollCriteria : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgLaneTollCriteria)
	class TreeNodeData
	{
	public:
		enum TreeNodeType
		{
			VehicleType_Node,
			Character_Node,
			TimeRange_Node,
			LaneToll_Node,
			Facility_Node,
			Lane_Node,
			Service_Node,
		};

		TreeNodeData()
			:m_emType(VehicleType_Node)
		{

		}
		~TreeNodeData()
		{

		}

		DWORD m_dwData;
		TreeNodeType m_emType;
	};

public:
	CDlgLaneTollCriteria(CTermPlanDoc* pDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLaneTollCriteria();

// Dialog Data
	enum { IDD = IDD_DIALOG_LANETOLLCRITERIA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBtnSave();
	afx_msg LRESULT OnLeftDoubleClick( WPARAM wParam, LPARAM lParam );
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//tree operation
	void InsertVehicleAllocationItem(HTREEITEM hItem,LandsideVehicleAllocation* pVehicle);
	void InsertCharacterstiscItem(HTREEITEM hItem,LandsideCharactersitscAllocation* pChara);
	void InsertTimeWindowItem(HTREEITEM hItem,LandsideTimeRangeAllocation* pTimeRange);
	void InsertLaneTollItem(HTREEITEM hItem,LandsideLaneTollData* pData);

public:
	void OnInitToolbar();
	void UpdateToolBarState();
	void LoadTreeContent();
private:
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	void GetFallBackReasonData(FallbackData& data);
	FallbackReason GetFallBackReason();	

	void DeleteTreeNodeData( TreeNodeData* pNodeData );
	void ClearTreeNode();
private:
	CToolBar  m_wndToolbar;
	CCoolTree m_wndTreeCtrl;
	CTermPlanDoc* m_pDoc;
	std::vector<TreeNodeData*> m_vNodeData;
	LandsideLaneTollCriteria m_laneTollCriteria;

};
