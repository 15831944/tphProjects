#pragma once
#include "./InputOnBoard/OnBoardAnalysisCondidates.h"
class CTermPlanDoc;
class CARCBaseTree;
class COnBoardAnalysisCandidates;
class CAircaftLayOut ;
class CDeck ;
class CSection ;
#define TYPE_LAYOUT 0x0010 
#define TYPE_DECK 0x0011
#define TYPE_SECTION 0x0012
#define TYPE_3DMODEL 0x0013
#define TYPE_PLACEMENT 0x0014
// COnBoardMsview view
namespace MSV
{
	enum emOnBoardNodeType
	{
		OnBoardNodeType_Normal = 0,
		OnBoardNodeType_Dlg
	};
	enum emOnBoardNodeType_Normal
	{
		OnBoardNodeType_NormalStart = 0XEA, // !!! start with strange number, avoid the default 0 conflict

		Normal_SeatClasses = OnBoardNodeType_NormalStart,
		Normal_FlightData,
		Normal_PassengerSeatNum,

		OnBoardNodeType_NormalEnd, // add enumeration before this
	};
	enum emOnBoardNodeType_Dlg
	{
		OnBoardNodeType_DlgStart = OnBoardNodeType_NormalEnd,

		//flight type specification
		Dlg_OnBoardFlightSchedule = OnBoardNodeType_DlgStart,
		Dlg_OnBoardDatabaseFlight,
		Dlg_OnBoardDatabaseAirportSector,
		Dlg_OnBoardDatabaseAirline,
		Dlg_OnBoardDatabaseAircraft,
		//Stand and Gate
		Dlg_OnBoardStandAssignment,
		Dlg_OnBoardArrGateAssignment,
		Dlg_OnBoardDepGateAssignment,

		//Boarding call
		Dlg_OnBoardBoardingCall,
		Dlg_OnBoardPaxTypeDefine,
		Dlg_OnBoardPaxDestr,

		//Kinetics and space
		Dlg_OnBoardKineticsAndSpace_Speed,
		Dlg_OnBoardKineticsAndSpace_FreeInStep,
		Dlg_OnBoardKineticsAndSpace_CongestionInStep,
		Dlg_OnBoardKineticsAndSpace_QueueInStep,
		Dlg_OnBoardKineticsAndSpace_FreeSideStep,
		Dlg_OnBoardKineticsAndSpace_CongestionSideStep,
		Dlg_OnBoardKineticsAndSpace_QueueSideStep,
		Dlg_OnBoardKineticsAndSpace_InteractionDistance,
		Dlg_OnBoardKineticsAndSpace_AffinityFreeDistance,
		Dlg_OnBoardKineticsAndSpace_AffinityCongestionDistance,
		Dlg_OnBoardKineticsAndSpace_Sqeezability,

		Dlg_OnBoardPaxDisp,
		Dlg_OnBoardCarryonOwnership,
		Dlg_OnBoardCarryonVolumn,
		Dlg_AffinityGroup,
		Dlg_CarryonStoragePriorities,

		Dlg_UnimpededStateTransTime,
		Dlg_DocDisplayTime,
		Dlg_UnexpectedBehavior,
		Dlg_TargetLocations,
		Dlg_SeatingPreference,
		Dlg_OnBoardCabinCrewGeneration,
		Dlg_WayFindingIntervention,		
		Dlg_ArrSeatAssignment,
		Dlg_DepSeatAssignment,
		Dlg_TransitionTimeModifier,
		Dlg_OnBoardCrewPaxInteraction,
		Dlg_PaxCabinAssignment,
		Dlg_SeatTypeDefine,
		Dlg_SeatBlockDefine,

		//simulation
		Dlg_OnboardSimSetting,
		Dlg_RunSimulation,

		Dlg_OnboardSeatClassLoadFactor,
		Dlg_OnBoardLoadFactor,
		Dlg_CrewAssignment,
		Dlg_LAGTIME,
		Dlg_DeplanementSequ,
		Dlg_EntryDoorOperationSpec,
		Dlg_ExitDoorOperationSpec,

		// Analysis Parameters
		Dlg_OnboardCalloutDispSpec,


		OnBoardNodeType_DlgEnd,  // add enumeration before this
	};

	class COnBoardNodeData
	{

	public:
		COnBoardNodeData(emOnBoardNodeType nodetype)
		{
			nodeType = nodetype;
			nSubType = 0;
			nIDResource = IDR_CTX_DEFAULT;
			dwData = 0;
			strDesc = "";

			bHasExpand = false;
			nOtherData = -1;

		}
		virtual ~COnBoardNodeData(){}

		virtual int get_type() { return -1; }
	public:
		//node type
		emOnBoardNodeType nodeType;// the node type,enum 
		int nSubType; //mostly the enum dlg, or the object type
		//menu id
		UINT nIDResource;//the menu id

		DWORD dwData;//mostly,the object id
		CString strDesc;//description
		bool bHasExpand;//the Item is expand or not, working in loading object
		int nOtherData; //mostly, The airport id, airspace id, topography id


		class TypeSubTypeMatcher
		{
		public:
			TypeSubTypeMatcher(emOnBoardNodeType t, int st)
				: nodeType(t)
				, nSubType(st)
			{

			}

			bool operator()(const COnBoardNodeData* pData)
			{
				return pData->nodeType == nodeType && pData->nSubType == nSubType;
			}

			emOnBoardNodeType nodeType;// the node type,enum 
			int nSubType; //mostly the enum dlg, or the object type

		};
	};
class COnBoardMSView : public CView
{
	DECLARE_DYNCREATE(COnBoardMSView)

protected:
	COnBoardMSView();           // protected constructor used by dynamic creation
	virtual ~COnBoardMSView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
	HTREEITEM m_hRightClkItem;
	int m_nProjID;
	CARCBaseTree m_wndTreeCtrl;

	void InitTree();
	HTREEITEM AddItem(const CString& strNode,HTREEITEM hParent = TVI_ROOT,BOOL bGray = FALSE ,HTREEITEM hInsertAfter = TVI_LAST,int nImage = ID_NODEIMG_DEFAULT,int nSelectImage = ID_NODEIMG_DEFAULT);
	HTREEITEM AddARCBaseTreeItem(const CString& strNode,HTREEITEM hParent = TVI_ROOT,NODE_EDIT_TYPE net = NET_NORMAL,NODE_TYPE nt = NT_NORMAL,HTREEITEM hInsertAfter = TVI_LAST,int nImage = ID_NODEIMG_DEFAULT,int nSelectImage = ID_NODEIMG_DEFAULT);
	void insertFlightOnTree(HTREEITEM hParentItem,CFlightTypeCandidate* pFlightType,COnboardFlight* pFlightAnalysis,CString strFlight);
protected:
	CImageList m_imgList;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBeginLabelEdit(WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	//afx_msg void OnLoadACInfoConfiguretion();
	//afx_msg void OnDefineACInfoConfiguretion();
	//afx_msg void OnSaveACInfoConfiguretionAs();
	//afx_msg void OnDefineAC3DModelShell();
	//afx_msg void OnLoadAC3DModelShell();	
	afx_msg void OnDefineSchedule();
	afx_msg void OnDefineFlightDB();
	afx_msg void OnDefineAirportSector();
	afx_msg void OnDefineAirlineGroup();
	afx_msg void OnDefineACType();
	afx_msg void OnDefineStandAssignment();
	//afx_msg void OnSaveAC3DModelShellAs();
	afx_msg void OnAddFlightTypeCandidate();
	afx_msg void OnDefineFltRemove();
	afx_msg void OnDefineCabin();
	afx_msg void OnDefineCabinCopy();
	afx_msg void OnDefineCabinPaste();
	afx_msg void OnDefineCabinInFlt();
	afx_msg void OnDefineCabinFltACType();
	afx_msg void OnDefineCabinChangeName();
	afx_msg void OnDefineCabinRemove();
	afx_msg void OnDefinCabinSpecify();
	afx_msg void OnRemoveFlight();
//	afx_msg void OnUpdateCopyCain(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePasteCabin(CCmdUI* pCmdUI);
//	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnAddFlightToFltTypeCandidate();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnDraw(CDC* pDC){}
	CARCBaseTree& GetTreeCtrl(){ return m_wndTreeCtrl;}
	void OnInitOnBoardAnalysis(HTREEITEM hItem);
	void AddSeatClassesLoadFactors( HTREEITEM hFlight, COnboardFlight* pFlightData );

	HTREEITEM getLevelItem(HTREEITEM hItem,int nLevel);
public:
	CTermPlanDoc *GetARCDocument();
	//Chloe
	HTREEITEM m_cur_select_item;
	//void add_actype_node(const char* name);

	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	void OnUpdateEx(UINT message, WPARAM wParam, LPARAM lParam); // discarded
	void AddAC3DModelNode();
	void Create3dModelNode(const std::string& name);
	bool SelectFlightType(FlightConstraint& fltcons,CWnd* pParent);

private:
	CString m_strData;
	ItemEditType m_emEditType;
	COnBoardAnalysisCandidates* m_pOnBoardList;

	enum RefTreeItemTag
	{
		Item_OnboardAnalysisCandidates = 0,

		RefTreeItem_Count,
	};
	HTREEITEM m_hRefTreeItems[RefTreeItem_Count];
//public:
//	afx_msg void OnLoadAirCraftLayOut() ;
//	afx_msg void OnImportAircraftLayOut() ;
//	afx_msg void OnExportAircraftLayOut() ;
//
//	//////////////////////////////////////////////////////////////////////////
//	afx_msg void OnLayOutReName();
////	afx_msg void OnLayOutReDefineActype();
//	afx_msg void OnLayoutAddDeck() ;
//	afx_msg void OnLayOutDelDeck() ;
//	//////////////////////////////////////////////////////////////////////////
//void OnInitOnBoardLayOut(HTREEITEM hItem) ;
//void OnInitOnBoardLayOutNode(HTREEITEM hitem) ;
//void AddAircraftLayOutTreeNode(CAircaftLayOut* _layout , HTREEITEM hitem) ;
//void OnInitOnBoardLayoutDeckNode(CDeck* _deck , HTREEITEM hitem) ;
//void OnInitOnBoardLayOutSectionNode(CSection* _section , HTREEITEM hitem);
//void OnInit3DModelNode(CString& acty,HTREEITEM _item) ;
//void OnInitPlacementNode(HTREEITEM _item) ;
//   //////////////////////////////////////////////////////////////////////////
//	afx_msg void OnDeckNewSection() ;
//	afx_msg void OnDeckDelSection() ;
//	afx_msg void OnDeckReName() ;
//	afx_msg void OnDeckActive() ;
//	afx_msg void OnDeckProperty() ;
//	afx_msg void OnDeckIsolater() ;
//	afx_msg void OnDeckHide() ;
//	afx_msg void OnDeckGirdOn() ;
//	afx_msg void OnDeckThicknesson() ;
//	afx_msg void OnDeckVisibleRegion() ;
//	afx_msg void OnDeckInVisibleRegion() ;
//	afx_msg void OnDeckColorForMonochrome() ;
//	afx_msg void OnDeckColor_Vivid() ;
//	afx_msg void OnDeckColor_Desaturated() ;
//	afx_msg void OnDeckDelete() ;
//	afx_msg void OnDeckComment() ;
//	afx_msg void OnDeckHelp() ;
//
//    //////////////////////////////////////////////////////////////////////////
//	afx_msg void OnSectionRename();
//	afx_msg void OnSectionPosition() ;
//	afx_msg void OnSectionDefine() ;
//	afx_msg void OnSectionDelete() ;
//	afx_msg void OnSectionGrid() ;
	//////////////////////////////////////////////////////////////////////////
	void OnEndlabelTree(NMHDR* pNMHDR, LRESULT* pResult) ;

	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnSelectConfigureInteror();
	afx_msg void OnConfigureInteror() ;
	afx_msg void OnUpdateSeatClasses() ;

protected:
	//CString FormatAircraftModelNodeName(const CString& acty) ;
	//CAircaftLayOut* GetLayOutFormTreeNode(HTREEITEM _item) ;
	//CDeck* GetDeckFromTreeNode(HTREEITEM _item) ;
	//void AddFlightIDNodeForlayout(COnboardFlight* _flightAnalysis ,HTREEITEM _item) ;
	//void DeleteFlightTYNodeForlayout(CFlightTypeCandidate* _flightAnalysis ) ;
	//void AddFlightTYNodeForlayout(CFlightTypeCandidate* _pFltData,HTREEITEM _item);
    void DeleteFlightTYNodeForlayoutMap(CFlightTypeCandidate* pFltData) ;
	CString FormatFlightIdNodeForCandidate(const CString& _configName,COnboardFlight* pFlight) ;
	bool RearrangeFlightToNewCandidate(COnboardFlight* pFlight, CFlightTypeCandidate* pNewCandidate);
	void RemoveOnboardFlight(COnboardFlight* pFlight,HTREEITEM hCandidatesItem);

	// update flight seat number which configured the layout, if NULL, update all
	void UpdateFlightSeatNum(CAircaftLayOut* _layout = NULL);

	// algorithms
	typedef std::vector<HTREEITEM> HTREEITEM_VECTOR;

	template <class NodeDataMatcher>
		HTREEITEM FindTreeChildItem(HTREEITEM hParent, NodeDataMatcher ndm, bool bCascaded = false)
	{
		if (GetTreeCtrl().ItemHasChildren(hParent))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hParent);

			while (hChildItem)
			{
				hNextItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
				COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hChildItem);
				if (pNodeData && ndm(pNodeData))
					return hChildItem;

				if (bCascaded)
				{
					HTREEITEM hFoundChildItem = FindTreeChildItem(hChildItem, ndm, bCascaded);
					if (hFoundChildItem)
						return hFoundChildItem;
				}

				hChildItem = hNextItem;
			}
		}
		return NULL;
	}

	template <class NodeDataMatcher>
		void FindTreeChildItems(HTREEITEM hParent, NodeDataMatcher ndm, HTREEITEM_VECTOR& listItems, bool bCascaded = false)
	{
		if (GetTreeCtrl().ItemHasChildren(hParent))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hParent);

			while (hChildItem)
			{
				hNextItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
				COnBoardNodeData* pNodeData = (COnBoardNodeData*)GetTreeCtrl().GetItemData(hChildItem);
				if (pNodeData && ndm(pNodeData))
					listItems.push_back(hChildItem);

				if (bCascaded)
					FindTreeChildItems(hChildItem, ndm, listItems, bCascaded);

				hChildItem = hNextItem;
			}
		}
	}

	void DeleteTreeAllChildItems(HTREEITEM hParent)
	{
		if (GetTreeCtrl().ItemHasChildren(hParent))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hParent);

			while (hChildItem != NULL)
			{
				hNextItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
				GetTreeCtrl().DeleteItem(hChildItem);
				hChildItem = hNextItem;
			}
		}

	}
};
}



