#pragma once
#include <Renderer/RenderEngine/3DScene.h>
#include <Renderer/RenderEngine/Grid3D.h>
#include <Renderer/RenderEngine/3DCADMap.h>
#include <Renderer/RenderEngine/3DNodeObject.h>
#include <renderer/RenderEngine/BaseFloor3D.h>
#include <common\ListenerCommon.h>
#include <Renderer\RenderEngine\AircraftElementNode.h>
#include <common/ARCPath.h>
#include <renderer/RenderEngine/Lines3D.h>
#include "PathEdit3DNode.h"
#include "AircraftLayout3DNode.h"
#include "./SceneOverlayDisplay.h"
#include "AircraftModel3DNode.h"
#include <Common/3DViewCameras.h>

class CAircraftLayoutEditContext;
class CAircraftLayoutEditCommand;
class CDeck;
class CDeckManager;
class CAircaftLayOut;
class CAircraftElmentShapeDisplay;
class  CAircraftLayout3DNode;
class CAircraftLayoutScene;

//display overlays to a 
class RENDERENGINE_API CAircraftLayoutOverlayDisplay  : public CSceneOverlayDisplay
{
public:
	CAircraftLayoutOverlayDisplay();

	virtual void PreDrawTo(Ogre::Viewport* );
	virtual void EndDrawTo(Ogre::Viewport* ); //close all open overlays

	void UpdateElmentsTags(CAircraftLayoutScene* playout);
protected:	
	Ogre::Overlay* UpdateElementText(CAircraftElmentShapeDisplay*);
	void UpdatePos(Ogre::Viewport* );
	
	CAircraftLayoutScene* mpLayoutScene;
	std::vector<Ogre::Overlay*> mOpenOverlays;
};



///
class RENDERENGINE_API CAircraftLayoutScene : public C3DSceneBase 
{
public :
	BEGINE_LISTENER_DECLARE()	
		LISTEN_METHOD_DECLARE( void OnFinishPickPoints(const ARCPath3&) )
		LISTEN_METHOD_DECLARE( void OnFinishDistanceMesure(double) )
		LISTEN_METHOD_DECLARE( void OnSceneChange() )
		LISTEN_METHOD_DECLARE( void OnRButtonDownOnElement(CAircraftElementShapePtr, const CPoint&) )
		LISTEN_METHOD_DECLARE( void OnRButtonDownOnEditPath(CAircraftElementShapePtr, int nPathIndex, int nNodeIndex, const CPoint&) )
	END_LISTENER_DECLARE()

	enum emMouseState
	{
		_default,
		_getonepoint,
		_gettwopoints, 
		_getmanypoints,
		_getonerect,	
		_distancemeasure,
		_moveElements,
		_rotateElements,
	};

	CAircraftLayoutScene();
	void Setup(CAircraftLayoutEditContext* pEdCtx); //setup scene camera

	void SetupCameraLight();//only once camera light

	BOOL OnUpdateOperation(CAircraftLayoutEditCommand* pOP);
	void OnDeckChange(CDeck* pdeck);
	virtual BOOL OnLButtonDown(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnMouseMove(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnLButtonUp(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnRButtonUp(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnRButtonDown(const MouseSceneInfo& mouseInfo);
	virtual BOOL IsSelectObject()const;
	virtual Ogre::Plane getFocusPlan()const;


	//distance measure
	void StartDistanceMeasure();
	void EndDistanceMeasure();	
	//pick points and rect
	void StartPickOnPoint();
	void StartPickTwoPoint();
	void StartPickManyPoint();
	void StartPickRect();
	void EndPickPoints();

	//

	//get mesh name from shape name
	//Ogre::Entity* GetAircraftShapeEntity(CString entName,const CString& actype);
	CAircaftLayOut* GetEditLayout();


	void OnUpdateSelectChange();

	bool AddEditPointToAircraftElement(CAircraftElmentShapeDisplay* pathRelElem, int nPathIndex, const ARCVector3& posNewPoint);
	bool RemoveEditPointFromAircraftElement(CAircraftElmentShapeDisplay* pathRelElem, int nPathIndex, int nNodeIndex);

public:
	void UpdateSelectRedSquares();
	//void OnEnableEditSeatArrayPath(); //do something when enable seat array path
	//

public:
	ARCPath3 m_vMousePosList;
	emMouseState mMouseState;
	void ResetMouseState();	
	void UpdateGridACShow();
	void UpdateDeckAltitude(CDeck *pDeck);
	void UpdateEditPath();

public:
	CGrid3D mGridXY; //ground grid
	CLines3D mPickLine;	
	//BOOL m_bDragStarted;
	Ogre::MovableObject* m_pDragObject;
	bool m_bOnDragging;
	void OnBeginDrag(const MouseSceneInfo& moveInfo);
	void OnEndDrag(const MouseSceneInfo& moveInfo);
	void OnDragObject(const MouseSceneInfo& moveInfo);
	void ClearDragInfo();

	
	CAircraftLayoutEditContext* mpEditContext;
	//CEditSeatGroupNode mEditSeatGroup;
	CAircraftModel3DNode m_acModel3DNode;
	CAircraftLayout3DNode mLayout3DNode;
	CEditMultiPath3DNode m_pathNode; //edit path node
	
	//
	CAircraftLayoutOverlayDisplay overlaytext;

protected:
	void OnUpdateSeatGroupChange();
};





