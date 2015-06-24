#pragma once
#include "afxwin.h"
#include "Common/ALTObjectID.h"
#include "MFCExControl/CoolTree.h"
#include "MFCExControl/ListCtrlEx.h"
#include <MFCExControl/SplitterControl.h>
#include "Landside/LandsideResidentVehilceRoute.h"
#include <deque>

class InputTerminal;
class InputLandside;
class LandsideFacilityLayoutObject;

class ResidentVehicleRouteFlow;

namespace
{
	const UINT ID_VEHICLE_NEW = 10;
	const UINT ID_VEHICLE_DEL = 11;

	const UINT ID_VERTICAL_SPLITTER = 15;
	const UINT ID_HORIZONTAL_SPLITTER = 16;
}

class CDlgBaseResident : public CDialog
{
	DECLARE_DYNAMIC(CDlgBaseResident)

public:
	class TreeNodeData
	{
	public:
		enum NodeType
		{
			Group_Facility,
			Single_Facility
		};
		TreeNodeData()
			:m_pLandsideObject(NULL)
			,m_emTye(Group_Facility)
		{

		}
		~TreeNodeData()
		{

		}

		LandsideFacilityLayoutObject* m_pLandsideObject;
		NodeType m_emTye;
	};

	struct TreeNodeInfor
	{
		HTREEITEM m_hTreeNode;
		bool m_bBold;
		COLORREF m_color;
		ResidentVehicleRouteFlow* m_pVehicleRouteFlow;

		TreeNodeInfor()
		{
			m_bBold = false;
			m_pVehicleRouteFlow = NULL;
		}

		bool operator == (const TreeNodeInfor& treeNode)const
		{
			if (treeNode.m_hTreeNode == m_hTreeNode)
			{
				return true;
			}
			return false;
		}
	};
	CDlgBaseResident(InputTerminal* pInTerm, InputLandside* pInputLandside,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBaseResident();

	// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDEVEHICLEOPERATIONPLAN };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

	virtual void InitListCtrlHeader();
	virtual void FllListCtrlContent() = 0;
	virtual void FillPlanTreeContent();

	virtual CString GetTitle()const = 0;

	virtual void InsertVehicleRouteItem(HTREEITEM hItem,ResidentVehicleRoute* pVehicleRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow) = 0;

	virtual void FillFacilityTreeContent();
	void InsertFacilityTreeItem(LandsideFacilityLayoutObject* pLandsideObject,HTREEITEM hItem,int nLevel);

	void InitToolbar();


	bool TypeNeedToShow( int nType ) const;
	void AddObjectToTree( LandsideFacilityLayoutObject* pLandsideObject,HTREEITEM hObjRoot,ALTObjectID objName );
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);

	void AdjustPlanTreeItemPercent(HTREEITEM hItem);
	void DeleteRouteItem(HTREEITEM hItem);

	void DoResize(int cx,int cy);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBegindragTreeFacility(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnLvnPlanTreeChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeletePlanTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddDestination();
	afx_msg void OnInsertBefore();
	afx_msg void OnInsertAfter();
	afx_msg void OnPruneBrance();
	afx_msg void OnRemoveFromBranch();
	afx_msg void OnRemoveFromAllBranch();
	afx_msg void OnModifyPercent();
	afx_msg void OnEvenPercent();
	afx_msg void OnPickFromMap();
	afx_msg void OnCopyFacility();
	afx_msg void OnAddCopyFacility();
	afx_msg void OnInsertCopyFacility();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClickTreeProcess( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	bool FacilitySelect()const;
	ResidentVehicleRouteFlow* GetVehicleRouteFlow();
	virtual ResidentVehicleRouteFlow* GetVehicleRouteFlow(HTREEITEM hItem) = 0;
	bool FindTimeRangeItem(HTREEITEM hItem, HTREEITEM& hTimeRangeItem);
	bool FindRouteRoot(HTREEITEM hItem, HTREEITEM& hRootItem);
	virtual ResidentVehicleRoute CreateVehicleRoute(HTREEITEM hItem);
	void ReloadVehicleRoute(HTREEITEM hRouteRoot);

	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);

	void PopupRouteMenu(CPoint point);
	void SetCopyFlowDestion(ResidentVehicleRouteFlow* pVehicleRouteFlow, const LandsideFacilityObject& facilityObject, ResidentVehicleRoute vehicleRoute );
	virtual int PopChildMenu() = 0;
	virtual void LayoutFigure();
	virtual void ReadInputData() = 0;
	bool IfHaveCircle( HTREEITEM _testItem,const LandsideFacilityObject& facilityObject );
protected:
	InputLandside*  m_pInputLandside;
	InputTerminal*  m_pInTerm;
	//timerange item list
	std::vector<HTREEITEM> m_vTimeRange;
	//route list
	std::vector<HTREEITEM> m_vRoute;
	//control member
	CSplitterControl m_wndSplitter1;
	CSplitterControl m_wndSplitter2;

	CListCtrlEx m_wndListCtrl;
	CTreeCtrl  m_wndFacilityCtrl;
	CCoolTree m_wndPlanCtrl;
	CToolBar   m_wndVehicleBar;
	CToolBar m_wndPlanBar;

	HTREEITEM m_hExtItem;
	HTREEITEM m_hParkLotItem;
	HTREEITEM m_hInterSectItem;
	HTREEITEM m_hDecisionItem;
	HTREEITEM m_hBusStationItem;
	HTREEITEM m_hRoundaboutItem;
	HTREEITEM m_hCurbsideItem;
	HTREEITEM m_hTaxiQueueItem;
	HTREEITEM m_hTaxiPoolItem;

	HTREEITEM     m_hItemDragSrc;   
	HTREEITEM	  m_hItemDragDes;
	CImageList*   m_pDragImage;      
	CImageList		m_imageList;
	bool m_bDragging;

	std::vector<int> m_vLandsideObjectType;
	std::vector<TreeNodeData*>m_vTreeNodes;

	ResidentVehicleRoute m_startRoute;

	//the processors copy from one own flow to another flow
	std::deque<TreeNodeInfor> m_vCopyItem;
	std::vector<ResidentVehicleRoute> m_vCopyProcessors;
	LandsideFacilityLayoutObject* m_pPickLayoutObject;
	virtual void OnOK();
};
