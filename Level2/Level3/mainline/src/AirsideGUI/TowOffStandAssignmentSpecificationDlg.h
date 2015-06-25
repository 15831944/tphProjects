#pragma once
#include "MFCExControl/CoolTree.h"
#include "MFCExControl/ListCtrlEx.h"
#include "NodeViewDbClickHandler.h"
#include "InputAirside/TowoffStandStrategyContainer.h"
#include <MFCExControl/SplitterControl.h>
#include <MFCExControl/XTResizeDialogEx.h>

class CTowOffStandAssignmentDataList;
class CTowOffStandAssignmentData;
class CTowOffPriorityEntry;
class CTowOffStandReturnToStandEntry;
class PreferrdTowoffStandData;

// CTowOffStandAssignmentSpecificationDlg dialog

class CTowOffStandAssignmentSpecificationDlg : public CXTResizeDialogEx
{
	DECLARE_DYNAMIC(CTowOffStandAssignmentSpecificationDlg)

	enum TreeNodeType
	{
		ND_FLTTYPE = 0,
		ND_DEPLANETIME,
		ND_ENPLANETIME,
		ND_PARKSTAND,
		ND_TOWORNOT,
		ND_TOWOFFCRITERIA,
		ND_LEAVE_ARR_STAND_TIME,
		ND_LEAVE_ARR_STAND_TIME_TYPE,
		ND_TOWOFFCRITERIA_TIME,
		ND_BACKTO_ARR_STAND,
		ND_PRIOIRITY,
		ND_RETURNTO_STAND,
		ND_RETURNTO_STAND_EMPLANEMENT_TIME,
		ND_PREFERRED_TOWOFF,
		ND_TIMERANGE_STAND,
		ND_CONDITION_STAND,
		ND_CONDITION_OPERATION,
		ND_CONDITION_TIME,
		ND_FLIGHTTYPE_STAND,
		ND_FLIGHTTYPE_ITEM,
	};

	class TowNodeData
	{
	public:
		TowNodeData()
			: dwData(NULL)
			, emNodeType(ND_FLTTYPE)
			, nStandID(-1)
			, dwExtraData(NULL)
			, dwExtraParentData(NULL)
		{

		}
		~TowNodeData()
		{

		}
	public:
		DWORD dwData;
		TreeNodeType emNodeType;
		int nStandID;
		DWORD dwExtraData;
		DWORD dwExtraParentData;

		//------------------------------------------------------------
		// To ParkStand:
		//    dwData                 - CTowOffStandAssignmentData*
		//    dwExtraData            - ignore
		//    dwExtraParentData      - ignore
		// 
		// To PriorityStand:
		//    dwData                 - CTowOffStandAssignmentData*
		//    dwExtraData            - CTowOffPriorityEntry*
		//    dwExtraParentData      - ignore
		// 
		// To ReturnToStand:
		//    dwData                 - CTowOffStandAssignmentData*
		//    dwExtraData            - CTowOffStandReturnToStandEntry*
		//    dwExtraParentData      - CTowOffPriorityEntry*
		//------------------------------------------------------------
	};
public:
	CTowOffStandAssignmentSpecificationDlg(int nProjID,PSelectFlightType pSelectFlightType, PSelectNewFlightType pSelectNewFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent = NULL);   // standard constructor
	virtual ~CTowOffStandAssignmentSpecificationDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_TOWOFFSTANDASSIGNMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	BOOL SetTreeCtrlItemData(HTREEITEM hItem, TowNodeData* pNodeData);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCmdNew();
	afx_msg void OnCmdDelete();
	afx_msg void OnCmdEdit();
	afx_msg void OnAddStrategy();
	afx_msg void OnDeleteStrategy();
	afx_msg void OnBnClickedSave();
	afx_msg void OnTvnSelchangedTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult );
protected:
	void InitToolBar(void);
	void SetTreeContent(void);
	void UpdateToolBar(void);
	void AddFlightTypeNode();
	void AddPriorityNode(HTREEITEM hItem);

	void DeleteAllChildItemsOfTreeCtrl(HTREEITEM hParent);
	void AddTowOffStandAssignmentItemsToTreeCtrl(COOLTREE_NODE_INFO& cniInitialized,
		CTowOffStandAssignmentDataList* pTowOffStandAssignList);

	void AddOneTowOffStandAssignmentItemToTreeCtrl(COOLTREE_NODE_INFO& cniInitialized,
		CTowOffStandAssignmentData *pTowOffStandAssign);

	void AddItemsOfTowOrNotToTreeCtrl(HTREEITEM hTowItem, COOLTREE_NODE_INFO& cniInitialized,
		CTowOffStandAssignmentData *pTowOffStandAssign);

	void AddItemsOfTowoffCriteriaToTreeCtrl(HTREEITEM hTowoffItem, COOLTREE_NODE_INFO& cniInitialized,
		CTowOffStandAssignmentData *pTowOffStandAssign);

	void AddPreferredTowoffToTreeCtrl(HTREEITEM hPreferredItem,COOLTREE_NODE_INFO& cniInitialized,
		CTowOffStandAssignmentData* pTowOffStandAssign);

	void AddPriorityItemsToTreeCtrl(HTREEITEM hTowoffItem, COOLTREE_NODE_INFO& cniInitialized,
		CTowOffStandAssignmentData *pTowOffStandAssign);

	void AddOnePriorityItemToTreeCtrl(HTREEITEM hTowoffItem, COOLTREE_NODE_INFO& cniInitialized,
		CTowOffPriorityEntry* pPriorityEntry,
		CTowOffStandAssignmentData *pTowOffStandAssign,
		int nIndex);

	void AddReturnToStandItemsToTreeCtrl(HTREEITEM hPriorityItem, COOLTREE_NODE_INFO& cniInitialized,
		CTowOffPriorityEntry* pPriorityEntry,
		CTowOffStandAssignmentData *pTowOffStandAssign);

	void AddOneReturnToStandItemToTreeCtrl(HTREEITEM hPriorityItem, COOLTREE_NODE_INFO& cniInitialized,
		CTowOffStandReturnToStandEntry* pReturnToStandEntry,
		CTowOffPriorityEntry* pPriorityEntry,
		CTowOffStandAssignmentData *pTowOffStandAssign);
	
	void AddOnePreferredTypeToTreeCtrl(HTREEITEM hItem,COOLTREE_NODE_INFO& cniInitialized,
		PreferTowoffFlightType* pPreferredFlightType,
		PreferrdTowoffStandData* pPreferredTowoffData,
		CTowOffStandAssignmentData* pTowOffStandAssign);
	void InitListCtrlHeader();
	void LoadListContent();

	void UpdateSplitterRange();
	void SetAllControlsResize();
	void DoResize(int delta,UINT nIDSplitter);
	bool IsHaveDefaultTowOffStrategy();

	bool SaveData();
private:
	CToolBar          m_ToolBar;
	CToolBar			m_wndToolBar;//head toolbar to operate for strategy name
	CCoolTree	    m_wndTreeCtrl;
	CListCtrlEx		m_wndListCtrl;


	CTowOffStandAssignmentDataList    *m_pTowOffStandAssignmentDataList;
	TowoffStandStrategyContainer m_towoffStrategyConstainer;
	PSelectNewFlightType             m_pSelectNewFlightType;
	PSelectFlightType					m_pSelectFlightType;
	int                               m_nProjID;
	CAirportDatabase                  *m_pAirportDatabase;
	std::vector<TowNodeData*>         m_listNodeData;
	CSplitterControl m_wndSplitter;
};
