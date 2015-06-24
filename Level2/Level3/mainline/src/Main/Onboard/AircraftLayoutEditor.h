#pragma once
#include <inputonboard\aircraftlayouteditcontext.h>

class CAircraftFurnishingModel;
class COnBoardObjectBaseDlg;
class CAircraftLayoutScene;
class CAircraftLayoutEditor :
	public CAircraftLayoutEditContext
{
public:
	CAircraftLayoutEditor(CDocument* );
	~CAircraftLayoutEditor(void);

	CAddDeckCommand* OnNewDeck();
	CDelDeckCommand* OnDelDeck(CDeck* pDeck);

	CModifyElementsPropCmd* OnSelectElementProperties( CWnd* pFromWnd);
	CModifyElementsPropCmd* OnElementProperties(CAircraftElmentShapeDisplay* pElm, CWnd* pFromWnd);
	CModifyElementsPropCmd* OnElementDisplayProperties(const CAircraftElmentShapeDisplayList& vlist, CWnd* pFromWnd);
	CDelAircraftElementsCmd* OnDelElement(const CAircraftElmentShapeDisplayList& vlist );

	CAddAircraftElementsCmd* OnNewElement(ARCVector3 dropPos, CAircraftFurnishingModel* pFurnishing, CWnd* pFromWnd );
	void OnCopySelectElements();
	bool OnDeleteSelectElements();

	void OnUndo();
	void OnRedo();	
	
	//return the select row name
	bool OnSelectSeat();
	bool OnSelectSeatRow();
	bool OnSelectSeatGroup();


	void OnSeatCopy();
	void OnSeatDelete();
	void OnEditSeatRow( CWnd* pFromWnd );
	void OnEditSeatGroup(CWnd* pFromWnd);

	void DoArrayElements(const ARCPath3& vPts, int nCopies);

	void OnChangeUndefinedToSeat(CWnd* pFromWnd);
	void OnChangeUndefinedToStorage(CWnd* pFromWnd);
	void OnChangeUndefinedToDoor( CWnd* pFromWnd);
	void OnChangeUndefinedToGalley( CWnd* pFromWnd);	
	void OnChangeUndefinedToEmergencyExit( CWnd* pFromWnd);
	void OnChangeUndefinedToStair( CWnd* pFromWnd);
	void OnChangeUndefinedToEscaltor(CWnd* pFromWnd );
	void OnChangeUndefinedToElevator( CWnd* pFromWnd);

	void OnNewCorridor(CWnd*pFromWnd);
	void OnNewStair(CWnd*pFromWnd);
	void OnNewEscalator(CWnd*pFromWnd);
	void OnNewElevator(CWnd*pFromWnd);
	void OnNewWall(CWnd*pFromWnd);
	void OnNewSurface(CWnd*pFromWnd);
	void OnNewPortal(CWnd*pFromWnd);


	CPoint mLastRightButtonDownPt;

	CMoveShapesCmd* StartMoveSelectShapes( CMoveShapesCmd::Move_TYPE t );
	CScaleShapesCmd* StartScaleSelectShapes(CScaleShapesCmd::Scale_TYPE t);
	CRotateShapesCmd* StartRotateSelectShapes();
	CMoveShapesZ0Cmd* DoMoveShapesZ0();
	CModiftShapesTranCmd* OnEditShapeTranform(CWnd* pFromWnd);

	void CancelCurEdit(CAircraftLayoutScene* pScene);

	static bool GetElementMenu(CAircraftElementShapePtr pelm, int& resId, int& menuIdx);
public:
	virtual InputOnboard* GetInputOnboard()const;	
	CDelAircraftElementsCmd* OnDelElement( CAircraftElementShapePtr pelm);
protected:
	int GetProjectID()const;
	CAddAircraftElementsCmd* OnAddNewElement( CAircraftElementShapePtr pelm);
	void NotifyOperations(CAircraftLayoutEditCommand* pOP,CView* pexcludeView = NULL);
	void OnCopySeatRow();
	void OnDeletSeatRow();
	void OnDeleteSeatGroup();
	COnBoardObjectBaseDlg* GetElementDlg(CAircraftElement* pElm,CWnd* pFromWnd);

	virtual double GetActiveFloorHeight()const;
};