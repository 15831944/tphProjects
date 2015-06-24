#pragma once

#include "coolbtn.h"
#include "../common/StartDate.h"
#include "TermPlanDoc.h"
#include "../AirsideReport/Parameters.h"
#include "../common/FLT_CNST.H"
#include "../AirsideReport/AirsideReportNode.h"
#include "../MFCExControl/ARCTreeCtrlEx.h"
#include "AirsideReControlViewTreePerformer.h"
#include "../AirsideReport\TypeDefs.h"
#include "../AirsideReport/RunwayExitParameter.h"
// CAirsideRepControlView form view
const int iEdgeWidth		= 5; 
const int iToolBarHeight	= 34;

class CAirsideVehicleOperParameter ;
class  CTaxiwayDelayParameters ;
class CRunwayExitParameter;
class CAirsideFlightFuelBurnParmater ;
class CAirsideRepControlView : public CFormView
{
	DECLARE_DYNCREATE(CAirsideRepControlView)

protected:
	CAirsideRepControlView();           // protected constructor used by dynamic creation
	virtual ~CAirsideRepControlView();

public:
	enum { IDD = IDD_DIALOG_REPORT_SPECS };

public:
	enum repControlTreeNodeType
	{
		repControlTreeNodeType_Unkown = 0,
		repControlTreeNodeType_FltTypeRoot,
		repControlTreeNodeType_FltType,
		repControlTreeNodeType_NodeFromToRoot,
		repControlTreeNodeType_FromRoot,
		repControlTreeNodeType_FromNode,
		repControlTreeNodeType_ToRoot,
		repControlTreeNodeType_ToNode,
		repControlTreeNodeType_NodesRoot,
		repControlTreeNodeType_Node,
		repControlTreeNodeType_RunwayMarkRoot,
		repControlTreeNodeType_RunwayMarkNode,
		repControlTreeNodeType_ClassificationRoot,
		repControlTreeNodeType_ClassificationNode,
		repControlTreeNodeType_ReportingAreaRoot,
		repControlTreeNodeType_ReportingAreaNode,
		//repControlTreeNodeType_SectorRoot,
		//repControlTreeNodeType_SectorNode,
		//vehicle type 
		repControlTreeNodeType_VehicleType ,
		repControlTreeNodeType_VehicleTypeNode,
		repControlTreeNodeType_Pool,
		repControlTreeNodeType_PoolNode ,
		//taxiway 
		repControlTreeNodeType_Taxiways,
		repControlTreeNodeType_Taxiway ,
		repControlTreeNodeType_TaxiWayFrom,
		repControlTreeNodeType_TaxiwayTo,
		repControlTreeNodeType_TaxiIntersection,
		//runway exit
		repControlTreeNodeType_RunwayExits,
		repControlTreeNodeType_Exit_Runway,
		repControlTreeNodeType_Exit_ItemRoot,
		repControlTreeNodeType_Exit_Item,
		repControlTreeNodeType_Exit_Oper,
		//flight fuel burn report
		repControlTreeNodeType_FromToNode,

	};

	class repControlTreeNodeData
	{
	public:
		repControlTreeNodeData(repControlTreeNodeType type)
		{
			nodeType = type;
			m_Data = NULL ;
			nClassification = -1;

		}
		repControlTreeNodeData(repControlTreeNodeType type,FlightConstraint& cons)
		{
			nodeType = type;
			fltCons = cons;
			m_Data = NULL ;
			nClassification = -1;

		}
		repControlTreeNodeData(repControlTreeNodeType type,CAirsideReportNode& node)
		{
			nodeType = type;
			nodeObject = node;
			m_Data = NULL ;
			nClassification = -1;

		}

		repControlTreeNodeData(repControlTreeNodeType type,CAirsideReportRunwayMark& node)
		{
			nodeType = type;
			runwayMark = node;
			nClassification = -1;

		}

		repControlTreeNodeData(repControlTreeNodeType type,int nIntValue)
		{
			nodeType = type;
			nClassification = nIntValue;

		}

		~repControlTreeNodeData(){};
	public:
		repControlTreeNodeType nodeType;
		FlightConstraint fltCons;
		CAirsideReportNode nodeObject;
		DWORD  m_Data ;
		//if repControlTreeNodeType_RunwayMarkRoot
		CAirsideReportRunwayMark runwayMark;
		ALTObjectID objID;

		//repControlTreeNodeType_ClassificationNode
		int nClassification;
	};

public:
	void ResetAllContent();

	//data 
public:
	CParameters *m_pParameter;


protected:
	HTREEITEM m_hRootFlightType;
	HTREEITEM m_hRootNodeToNode;
	HTREEITEM m_hRootNodes;
	HTREEITEM m_hRootRunways;
	HTREEITEM m_hRootClassification;
	HTREEITEM m_hRootReportingArea;
	//HTREEITEM m_hRootSector;
	HTREEITEM m_hSelectItem ;

	//dialog
public:
	CComboBox	m_comboEndDay;
	CComboBox	m_comboStartDay;
	CARCTreeCtrlEx	m_treePaxType;
	CButton	m_staticTime;
	CStatic	m_staticStartTime;
	CStatic	m_staticEndTime;
	CCoolBtn	m_btnMulti;
	CButton	m_btLoad;
	CDateTimeCtrl	m_dtctrlThreshold;
	CDateTimeCtrl	m_dtctrlInterval;
	CSpinButtonCtrl	m_spinThreshold;
	CStatic	m_staticArea;
	CComboBox	m_comboAreaPortal;
	CButton	m_staticThreshold;
	CButton	m_staticType;
	CStatic	m_staticInterval;
	CStatic	m_staProc;
	CStatic	m_staPax;
	CStatic	m_toolbarcontenter2;
	CStatic	m_toolbarcontenter1;
	CButton	m_staticProListLabel;
	CListBox	m_lstboProType;
	CButton	m_staticTypeListLabel;
	CButton	m_btnCancel;
	CDateTimeCtrl	m_dtctrlStartTime;
	CDateTimeCtrl	m_dtctrlEndTime;
	CDateTimeCtrl   m_dtctrlStartDate;
	CDateTimeCtrl   m_dtctrlEndDate;
	CButton	m_radioDetailed;
	CButton	m_radioSummary;
	CListBox	m_lstboPaxType;
	CEdit	m_edtThreshold;
	CButton	m_chkThreshold;
	CButton	m_btnApply;
	CButton m_btnLoadDefault;
	COleDateTime	m_oleDTEndTime;
	COleDateTime	m_oleDTStartTime;
	COleDateTime	m_oleDTEndDate;
	COleDateTime	m_oleDTStartDate;
	int		m_nRepType;
	UINT	m_nInterval;
	COleDateTime	m_oleDTInterval;
	float	m_fThreshold;
	COleDateTime	m_oleThreshold;
	bool bShowLoadButton;


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


	CToolBar m_ProToolBar;
	CToolBar m_PasToolBar;

	HTREEITEM m_hItemTo;
	HTREEITEM m_hItemFrom;
	HTREEITEM m_hItemPaxType;


	CStartDate m_startDate;
protected:
	void DoSizeWindow( int cx, int cy );
	void SizeDownCtrl(int cx, int cy );
	void InitToolbar();
	void InitializeControl();
	void ShowOrHideControls();

	void InitializeTree();
	void InitializeDefaultTreeData();
	void InitDefaultData();
	void EnableControl();

	BOOL  SaveData();
	void LoadData();

	void UpdateControlContent();

	CTermPlanDoc* GetDocument(){ return (CTermPlanDoc*) m_pDocument; }


	BOOL IsRunwayMarkSelected(HTREEITEM hRunwayMarkRoot,CAirsideReportRunwayMark& reportRunwayMark );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonReportDefault();
	afx_msg void OnButtonCancel();
	afx_msg void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolBarDel(CCmdUI *pCmdUI);
    void ClickMuiButton() ;

protected:
	void OnSaveReport() ;
	void OnLoadReport() ;
	void InitMuiButtom() ;
	afx_msg void OnReportSavePara();
	afx_msg void OnReportLoadPara();
//	void OnGenerateReport() ;
protected:
	void SetVehicleType(CAirsideVehicleOperParameter* pParam , HTREEITEM _item);
	void SetPoolID(CAirsideVehicleOperParameter* pParam , HTREEITEM _item) ;
	BOOL IsClassificationSelected( HTREEITEM hACClassRoot,int nClass );
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	AirsideReControlView::CTreePerformer * GetTreePerformer();
	reportType GetReportType();
	BOOL IsObjectSelected(HTREEITEM hObjRoot,repControlTreeNodeType nodeType,const ALTObjectID& altID);
protected:
	//the tree control's performer, it can be different for different report type	
	AirsideReControlView::CTreePerformer *m_pTreePerformer;
protected:

	BOOL IsTaxiwaySelect(HTREEITEM _htaxiwayRootItem,CString _SelTaxi) ;
	bool SetTaxiwayItem(HTREEITEM _item,CTaxiwayDelayParameters* _taxiwayParameter) ;
	void AddRunwayExitReportExitItem(HTREEITEM _RunwayItem);
	void AddRunwayExitReportRunwayItem(HTREEITEM _rootItem);
	void UpdateExitOperationItem(HTREEITEM _Item , int _type);
	void SetRunwayExitReportNode(HTREEITEM _item , CRunwayExitParameter* _Parm);
	BOOL CheckTheRunwayExitIsSelect(HTREEITEM _item , int _id);
protected:
	void SetFlightFuelBurnReportNodeData(HTREEITEM _NodeData , CAirsideFlightFuelBurnParmater* _Parm) ;
protected:
	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) ;
	void AddExitItem(RunwayExitRepDefineData_Exit& exitItem ,HTREEITEM _item);
	void AddRunwayItem(RunwayExitRepDefineData_Runway* _runwayData);
};


