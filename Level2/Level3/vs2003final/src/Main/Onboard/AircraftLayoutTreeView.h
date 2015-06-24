#pragma once
#include <MFCExControl/ARCBaseTree.h>
#include <map>
// CLayoutView view
class CAircaftLayOut ;
class CDeck ;
class CAircraftElmentShapeDisplay;
class CAircraftElmentShapeDisplayList;
class CDeckManager ;
class ALTObjectID;
class CEditOperationCommand;


#define  IDC_LAYOUTVIEW_TREE 0x01
#define  ON_REFRESH_LAYOUTTREE WM_USER + 50 
class CLayoutNodeData
{
public:
	CLayoutNodeData()
		:m_Ty(0),
		m_wData(NULL),
		m_resouceID(0)
	{

	} 
public:
	int m_Ty ;
	void* m_wData ;
	UINT m_resouceID ;
};

class CLayoutDataList
{
public:
	CLayoutNodeData* newItem(){ CLayoutNodeData* pnew = new CLayoutNodeData; m_NodeData.push_back(pnew); return pnew; }
	std::vector<CLayoutNodeData*> m_NodeData;
	void Clear()
	{
		for (int i = 0 ; i < (int)m_NodeData.size() ; i++)
		{
			delete m_NodeData[i] ;
		}
		m_NodeData.clear() ;
	}
	~CLayoutDataList(){
		Clear();
	}
};



class CAircraftLayoutEditor;
class CAircraftLayoutEditCommand;

class CAircraftLayoutTreeView : public CView
{
	DECLARE_DYNCREATE(CAircraftLayoutTreeView)

protected:
	CAircraftLayoutTreeView();           // protected constructor used by dynamic creation
	virtual ~CAircraftLayoutTreeView();
	

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	CARCBaseTree m_wndTreeCtrl;
	CAircaftLayOut* m_layout ;
	CLayoutDataList m_NodeData ;
	HTREEITEM m_hRightClkItem ;

	CAircraftLayoutEditor* GetEditor();
	int GetProjectID();
protected:

	void OnInitialUpdate() ;

protected:
	CARCBaseTree& GetTreeCtrl();
	void InitLayoutTree() ;

	int OnCreate(LPCREATESTRUCT lpcs) ;
	void OnSize(UINT nType, int cx, int cy) ;
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	void OnUpdateBarShowShowHide(CCmdUI* pCmdUI)  ;
	void OnUpdateBarGridOnOff(CCmdUI* pCmdUI) ;

	void OnUpdateActiveItem(CCmdUI* pCmdUI) ;
	void OnUpdateThicknessOnOff(CCmdUI* pCmdUI) ;

	void CAircraftLayoutTreeView::ReloadDeck() ;

	CImageList m_imgList;
protected:
	enum Nodetype 
	{
		TY_NODE_UNKNOW=0,
		TY_NODE_LAYOUT,
		TY_NODE_DECKS,
		TY_NODE_DECK,


		TY_NODE_PLACEMENTS,
		TY_NODE_PLACEMENTS_GROUP,
		TY_NODE_PLACEMENTS_DATA,
		TY_NODE_PLACEMENTS_SEATS,
		TY_NODE_PLACEMENTS_STORAGE,
		TY_NODE_PLACEMENTS_CABINDIVIDER,
		TY_NODE_PLACEMENTS_GALLEYS,
		TY_NODE_PLACEMENTS_DOORS,
		TY_NODE_PLACEMENTS_EMERGENCYEXITS,
		TY_NODE_PLACEMENTS_CORRIDOR,

		TY_NODE_VERTICALDEVICES,
		TY_NODE_VERTICALDEVICES_STAIRS,
		TY_NODE_VERTICALDEVICES_ESCALATOR,
		TY_NODE_VERTICALDEVICES_ELEVATOR,

		TY_NODE_UNDEFINED_SHAPE,

		TY_NODE_SURFACEAREAS,
		TY_NODE_SURFACEAREAS_WALLS,
		TY_NODE_SURFACEAREAS_AREAS,
		TY_NODE_SURFACEAREAS_PORTALS,


		TY_NODE_PROCESSORBEHAVIORS,
		TY_NODE_PROCESSORBEHAVIORS_ELEVATORS,
		TY_NODE_PROCESSORBEHAVIORS_ESCALATORS

	};

	std::map<Nodetype, HTREEITEM> m_mapPlacementRoot;
	std::map<DWORD_PTR, HTREEITEM> m_mapPlacementNode;


protected:
	void AddDeckNodeTree(CDeck* _deck,int index,HTREEITEM _item) ;
	void LoadDecks(CDeckManager* _manger , HTREEITEM _item) ;
	HTREEITEM AddItem(const CString& strNode,HTREEITEM hParent = TVI_ROOT,HTREEITEM hInsertAfter = TVI_LAST,int nImage = ID_NODEIMG_DEFAULT,int nSelectImage = ID_NODEIMG_DEFAULT);
protected:
	DECLARE_MESSAGE_MAP()
public:
	void SetLayout(CAircaftLayOut* _layout) { m_layout = _layout ;} ;
	void OnReFeshTree();
	void OnDeckProperies() ;

	CDeck* GetCurrentSelDeck() ;
	CAircraftElmentShapeDisplay* GetCurrentSelElement();
	void OnDeckActive() ;
	void OnIsolate() ;
	void OnDeckHideOrShow() ;
	void OnDeckGridOn() ;
	void OnVisibLeregions() ;
	void OnInvisibLeregion() ;
	void OnComment() ;

public:
	void AddPlacement(CAircraftElmentShapeDisplay *pElement);
	HTREEITEM GetPlacementRoot(const CString& typeStr);
	void UpdatePlacement(CAircraftElmentShapeDisplay *pElement);
	void DeletePlacement(CAircraftElmentShapeDisplay *pElement);
	void AddPlacements(const CAircraftElmentShapeDisplayList& );
	void DelPlacements(const CAircraftElmentShapeDisplayList& );

protected:
	void LoadPlacement(HTREEITEM hRootItem);
	void LoadSeat(HTREEITEM hRootItem);
	void ReloadSeat();
	void LoadStorage(HTREEITEM hRootItem);
	void LoadCabinDivider(HTREEITEM hRootItem);
	void LoadGalleys(HTREEITEM hRootItem);
	void LoadDoors(HTREEITEM hRootItem);
	void LoadEmergencyExits(HTREEITEM hRootItem);
	void LoadCorridor(HTREEITEM hRootItem);	


	//under constructs
	void LoadUnderConstructs(HTREEITEM hRootItem);	

	//vertical device
	void LoadVerticalDevices(HTREEITEM hRootItem);
	void LoadStairs(HTREEITEM hRootItem);
	void LoadEscalator(HTREEITEM hRootItem);
	void LoadElevator(HTREEITEM hRootItem);

	
	//surface areas
	void LoadSurfaceAreas(HTREEITEM hRootItem);
	void LoadWalls(HTREEITEM hRootItem);
	void LoadAreas(HTREEITEM hRootItem);
	void LoadPortals(HTREEITEM hRootItem);


	//processor behavior
	void LoadProcessorBehavior(HTREEITEM hRootItem);
	void LoadBehaviorElevator(HTREEITEM hRootItem);
	void LoadBehaviorEscalator(HTREEITEM hRootItem);
	HTREEITEM FindChildNode(HTREEITEM hParentItem,const CString& strNodeText);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	void OnUpdateOperations(CAircraftLayoutEditCommand* pOP);
	void NotifyOperations(CAircraftLayoutEditCommand* pOP, bool toSelf = true);

	//find the type item
	HTREEITEM FindPlacementGroupItem(CString placeType,const ALTObjectID& grpName);

public:
	afx_msg void OnOnBoardObjectProperties();
	afx_msg void OnOnBoardObjectDisplayProperties();
	afx_msg void OnOnBoardObjectDelete();
	afx_msg void OnOnBoardBbjectComment();
	afx_msg void OnOnBoardBbjectHelp();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
protected:
		void OnEditDeck() ;
		void OnDeckComment() ;
		void OnDeckHelp() ;
public:
	afx_msg void OnCtxNewDeck();
	afx_msg void OnDeckDelete();
	afx_msg void OnDeckRename();
	afx_msg void OnCtxOpaquefloor();
	afx_msg void OnUpdateOpaquefloor(CCmdUI *pCmdUI);
	afx_msg void OnCtxFloorthicknessValue();
	afx_msg void OnDeckShowmap();
	afx_msg void OnUpdateDeckShowmap(CCmdUI *pCmdUI);
	afx_msg void OnMenuShAc();
	afx_msg void OnUpdateMenuShAc(CCmdUI *pCmdUI);
	afx_msg void OnMenuShGrid();
	afx_msg void OnUpdateMenuShGrid(CCmdUI *pCmdUI);
	afx_msg LRESULT OnBeginLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelChanged(WPARAM wParam, LPARAM lParam);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:


protected:
	CAircraftElmentShapeDisplayList GetItemElementsList(HTREEITEM hitem);	
	HTREEITEM FindRootNodeType(Nodetype ndType )const;
	static Nodetype GetElementTypeNode(CString strType);

public:
	afx_msg void OnOnboardElementAddWall();
	afx_msg void OnOnboardElementAddSurface();
	afx_msg void OnOnboardElementAddPortal();
	afx_msg void OnOnboardElementAddCorridor();
	afx_msg void OnOnboardElementAddStair();
	afx_msg void OnOnboardElementAddEscalator();
	afx_msg void OnOnboardElementAddElevator();

	bool AddCommand( CEditOperationCommand*pCmd );
	afx_msg void OnOnboardobjectLock();
	afx_msg void OnUpdateOnboardobjectLock(CCmdUI *pCmdUI);
};































