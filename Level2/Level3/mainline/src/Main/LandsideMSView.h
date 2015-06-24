#pragma once

#include ".\MFCExControl\ARCBaseTree.h"

class CTermPlanDoc;
class CTVNode;
class CChildFrameSplit;
class InputTerminal;
class CPlacement;
class CReportParameter;
class CPaxFlowDlg;
class CCoolTreeEx;
class CFloors;
class CLandsidePortals;
class CLandsidePortal;
class CLandsideAreas;
class CLandsideArea;
class CTreeItemMSVNode;

//modeless dialog
class CDlgLandsideEntryOffset;
class CDlgNonPassengerRelatedVehicleAssignment;
class CDlgSpeedControl;
class CDlgParkingSpotSpec;
class CDlgLaneAvailabilitySpecification;
class CDlgLaneChangeCriteria;
class CDlgIntersectionTrafficControlManagement;
class CDlgLandsideVehicleOperationPlan;
class CDlgLandsideNonRelatedPlan;
class CFacilityDataSheet;
class CDlgVehicleBehaviorDistritbution;
class CDlgInitCondition;
class CSimEngSettingDlg;
class CDlgVehicleOperationNonRelated;
class CDlgCurbsideStrategy;
class CDlgParkingLotStrategy;
class CDlgLaneTollCriteria;
namespace airside_engine
{
	class Simulation;
	class FlightList;
	class FlightEvent;
}

class ILandsideEditCommand;
namespace MSV
{
	class CNodeData;
	class CNodeDataAllocator;
}

class CLandsideEditContext;
// CLandsideMSView view
class CLandsideMSView : public CView
{
	DECLARE_DYNCREATE(CLandsideMSView)

protected:
	CLandsideMSView();           // protected constructor used by dynamic creation
	virtual ~CLandsideMSView();

	HTREEITEM m_hRightClkItem;
	HTREEITEM m_selItem;
	HTREEITEM m_hLevelRoot;
	HTREEITEM m_hLayout;
	HTREEITEM m_hPortalRoot;
	HTREEITEM m_hLandsideAreaRoot;
//  	CTreeItemMSVNode *m_hPortalRoot;
	CImageList				m_imgList;
	//CTVNode					*m_pDragNode;
	CImageList				m_ilNodes;
	CARCBaseTree			m_wndTreeCtrl;
	MSV::CNodeDataAllocator *    m_pNodeDataAllocator;
	

private:
	CARCBaseTree& GetTreeCtrl(){ return m_wndTreeCtrl;}

public:
	CTermPlanDoc* GetDocument();
	InputTerminal* GetInputTerminal();
	//CPlacement& GetPlaceMent() const;
	//CString GetProjectPath() const;
	int GetProjectID();
	CChildFrameSplit* GetMDIChildOwner() { return (CChildFrameSplit*)(GetOwner()->GetOwner()); };

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	 CDlgLandsideEntryOffset* m_pLandsideEntryOffsetDlg;
	 CDlgNonPassengerRelatedVehicleAssignment* m_pVehicleAssignmentNonPaxRelatedDlg;
	 CDlgSpeedControl* m_pSpeedControlDlg;
	 CDlgParkingSpotSpec* m_pParkingSpotSpecDlg;
	 CDlgLaneAvailabilitySpecification* m_pLaneAvailabilitySpecificationDlg;
	 CDlgLaneTollCriteria* m_pLaneTollGateDlg;
	 CDlgLaneChangeCriteria* m_pLaneChangeCriteriaDlg;
	 CDlgIntersectionTrafficControlManagement* m_pIntersectionTrafficCtrlDlg;
	 CDlgCurbsideStrategy* m_pCurbsideStrategyDlg;
	 CDlgParkingLotStrategy* m_pParkingLotstrategyDlg;
	 CDlgLandsideVehicleOperationPlan* m_pLandsideVehicleOperationPlanDlg;
	 CDlgLandsideNonRelatedPlan* m_pLandsideNonRelatedPlanDlg;
	 CDlgVehicleOperationNonRelated* m_pVehicleOperationNonRelatedDlg;
	 CFacilityDataSheet* m_FacilityDataSheet;
	 CDlgVehicleBehaviorDistritbution* m_pServiceTimeDlg;
	 CDlgInitCondition* m_pInitConDitionDlg;
	 CSimEngSettingDlg* m_pSimEngSettingDlg;
public:
	//HTREEITEM AddItem(CTVNode* pNode, CTVNode* pAfterNode = NULL);
	static void UpdatePopMenu(CCmdTarget* pThis, CMenu* pPopMenu);

	virtual void OnInitialUpdate();
	void InitTree();

	void OnNewLandsideProc(UINT menuID);	
	//void AddPortals(CLandsidePortals &portals);
	//void AddPortalItem(CLandsidePortal *portal);
	//void EditPortalItem(CLandsidePortal *portal);
	//void DelPortalItem(CLandsidePortal *portal);

	//void AddLandsideAreas(CLandsideAreas &LandsideAreas);
	//void AddLandsideAreaItem(CLandsideArea *LandsideArea);
	//void EditLandsideAreaItem(CLandsideArea *LandsideArea);
	//void DelLandsideAreaItem(CLandsideArea *LandsideArea);
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnCommandUpdate(ILandsideEditCommand* pCmd);

	//HTREEITEM AddItem(const CString& strNode,HTREEITEM hParent = TVI_ROOT,HTREEITEM hInsertAfter = TVI_LAST,int nImage = ID_NODEIMG_DEFAULT,int nSelectImage = ID_NODEIMG_DEFAULT );
protected:
	void PumpMessages();
	LRESULT OnEndLabelEdit(WPARAM wParam, LPARAM lParam);
	LRESULT OnBeginLabelEdit(WPARAM wParam, LPARAM lParam);

protected:
	bool VersionCheck();
	//CTVNode* FindNode(HTREEITEM hItem);
	void DeleteAllChildren(HTREEITEM hItem);
	void DeleteFirstChild(HTREEITEM hItem);
	void RenameSelected();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnCtxRename();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	
	afx_msg LRESULT OnSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemExpanding(WPARAM wParam, LPARAM lParam);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	afx_msg void OnCtxProcproperties();
	afx_msg void OnCtxDeleteGroupProc();
	afx_msg void OnCtxLanddeleteproc();
	afx_msg void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCtxComments();
	afx_msg void OnCtxLandprocproperties();
	afx_msg void OnCtxDisplayProp();

	afx_msg void OnPaint();

	afx_msg void OnNewLandsidePortal();
	afx_msg void OnEditLandsidePortal();
	afx_msg void OnDelLandsidePortal();
	afx_msg void OnLandsidePortalColor();

	afx_msg void OnNewLandsideArea();
	afx_msg void OnEditLandsideArea();
	afx_msg void OnDelLandsideArea();
	afx_msg void OnLandsideAreaColor();

	afx_msg void OnFloorPictureInsert();
	afx_msg void OnFloorPictureHide();
	afx_msg void OnFloorPictureShow();
	afx_msg void OnFloorPictureRemove();

	afx_msg void OnSetLandsideLayoutOptions();

	afx_msg void OnLinkToTerminalFloors( UINT nID );

private:
	void SelectNode(MSV::CNodeData* pNodeDat);
	void NewLevelItem(CFloors& floor,int idx);
	void DeleteLevelItem(CFloors& floor,int idx);
	void ActiveLevelItem(int idx);
	CLandsideEditContext* GetEditContext();
};

#ifndef _DEBUG 
inline CTermPlanDoc* CLandsideMSView::GetDocument()
{ return (CTermPlanDoc*) m_pDocument; }
#endif