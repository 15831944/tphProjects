#pragma once
#include "afxcmn.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "../InputAirside/TowOperationSpecification.h"
#include "NodeViewDbClickHandler.h"
#include "../InputAirside/TowingRoute.h"
#include "../MFCExControl/XTResizeDialog.h"



// CDlgPushBackAndTowOperationSpec dialog

class AIRSIDEGUI_API CDlgPushBackAndTowOperationSpec : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgPushBackAndTowOperationSpec)

public:
	CDlgPushBackAndTowOperationSpec(int ProID ,PFuncSelectFlightType selFlightTy,CAirportDatabase* pAirportDB,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPushBackAndTowOperationSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_PUSHBACKANDTOWOPERATION };

public:

	enum enumNodeType
	{
		nodeType_None = 0,
		nodeType_Stand,
		nodeType_FlightType,
		nodeType_Operation,
		nodeType_ReleasePoint,
		nodeType_OutBoundRoute,
		nodeType_OutBoundRouteType,
		nodeType_OutBoundItem,
		nodeType_ReturnRoute,
		nodeType_ReturnRouteType,
		nodeType_ReturnRouteItem,
	};
	class CTreeItemData
	{
	public:
		CTreeItemData()
		{
			itemType = nodeType_None; 
			pStandData = NULL;
			pFlightTypeData = NULL;
			pTowRouteItem = NULL;
			pReleasePoint = NULL;
		}

		enumNodeType itemType;
		CTowOperationSpecStand *pStandData;
		CTowOperationSpecFltTypeData *pFlightTypeData;
		CReleasePoint* pReleasePoint;
		CTowOperationRouteItem *pTowRouteItem;

	};

protected:
	int m_nProjID;
	CAirportDatabase* m_pAirportDatabase;
	PFuncSelectFlightType m_FunSelectFlightType;
	CTowOperationSpecification m_towOperationSpec;

	std::vector<Runway *> m_lstRunway;
	std::vector<Taxiway *> m_lstTaxiway;
protected:
	void InitToolBar();
	void UpdateToolBarState();

	void LoadTree();

	//
	void OnAddStand();
	void OnDeleteStand();
	void OnEditStand();

	void OnAddFlightType(HTREEITEM hStandItem);
	void OnDeleteFlightType();
	void OnEditFlightType();

	void OnAddReleasePoint(HTREEITEM hSel);
	void OnDeleteReleasePoint();
	void OnEditReleasePoint();

	//return the operation node
	HTREEITEM AddOperationNode(HTREEITEM hFlighttypeItem);
	HTREEITEM AddReleaseNode(HTREEITEM hOperationNode);

	HTREEITEM  AddOutBoundRoute(HTREEITEM hReleasePointItem); 
	HTREEITEM  AddOutBoundRouteType(HTREEITEM hOutBoundRouteItem); 
	
	HTREEITEM AddReturnRoute(HTREEITEM hReleasePointItem);
	HTREEITEM AddReturnRouteType(HTREEITEM hReturnRouteItem);

	void OnAddTowRouteItem();
	void AddTowRouteItem(CTowOperationRouteItem *pItem, HTREEITEM hParentItem);
	CString GetRouteItemName(CTowOperationRouteItem *pItem);

	void OnDelTowRouteItem();
	

protected:
	enumNodeType GetItemType(HTREEITEM hItem);
	CTowOperationSpecStand * GetItemStandData(HTREEITEM hItem);
	CTowOperationSpecFltTypeData * GetItemFltTypeData(HTREEITEM hItem);
	CTowOperationRouteItem * GetItemRouteItemData(HTREEITEM hItem);	
	CReleasePoint* GetItemReleasePointData(HTREEITEM hItem);

protected:
	CTowingRouteList m_towingRouteList;
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CARCTreeCtrl m_treeCtrl;
	CToolBar m_toolbar;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnToolBarAdd();
	afx_msg void OnToolBarDel();
	afx_msg void OnToolBarEdit();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
