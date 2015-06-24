#pragma once
#include "../MFCExControl/ARCBaseTree.h"

// CLayoutView view
class CAircaftLayOut ;
class CDeck ;
class CAircraftElement;
class CDeckManager ;
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
	~CLayoutNodeData() 
	{

	}

public:
	int m_Ty ;
	void* m_wData ;
	UINT m_resouceID ;
};

class CLayoutView : public CView
{
	DECLARE_DYNCREATE(CLayoutView)

protected:
	CLayoutView();           // protected constructor used by dynamic creation
	virtual ~CLayoutView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	CARCBaseTree m_wndTreeCtrl;
	CAircaftLayOut* m_layout ;
	std::vector<CLayoutNodeData*> m_NodeData ;
	HTREEITEM m_hRightClkItem ;
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

	void CLayoutView::ReloadDeck() ;
protected:
	enum Nodetype 
	{
		TY_NODE_LAYOUT = 0,
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


		TY_NODE_VERTICALDEVICES,
		TY_NODE_VERTICALDEVICES_STAIRS,
		TY_NODE_VERTICALDEVICES_ESCALATOR,
		TY_NODE_VERTICALDEVICES_ELEVATOR,


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
	void OnDeckActive() ;
	void OnIsolate() ;
	void OnDeckHideOrShow() ;
	void OnDeckGridOn() ;
	void OnIfThickness() ;
	void OnVisibLeregions() ;
	void OnInvisibLeregion() ;
	void OnComment() ;

public:
	void AddPlacement(CAircraftElement *pElement);
	HTREEITEM GetPlacementRoot(CAircraftElement *pElement);
	void UpdatePlacement(CAircraftElement *pElement);
	void DeletePlacement(CAircraftElement *pElement);

protected:
	void LoadPlacement(HTREEITEM hRootItem);
	void LoadSeat(HTREEITEM hRootItem);
	void ReloadSeat();
	void LoadStorage(HTREEITEM hRootItem);
	void LoadCabinDivider(HTREEITEM hRootItem);
	void LoadGalleys(HTREEITEM hRootItem);
	void LoadDoors(HTREEITEM hRootItem);
	void LoadEmergencyExits(HTREEITEM hRootItem);
	

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
public:
	afx_msg void OnOnBoardObjectProperties();
	afx_msg void OnOnBoardObjectDisplayProperties();
	afx_msg void OnOnBoardObjectDelete();
	afx_msg void OnOnBoardBbjectComment();
	afx_msg void OnOnBoardBbjectHelp();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
		void OnEditDeck() ;
		void OnDeckComment() ;
		void OnDeckHelp() ;
};































