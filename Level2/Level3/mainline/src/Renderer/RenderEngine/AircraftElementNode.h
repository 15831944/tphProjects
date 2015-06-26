#pragma once
#include <Renderer/RenderEngine/3DNodeObject.h>
#include "KeyNodeList.h"
#include <InputOnBoard/AircraftElement.h>
#include <InputOnBoard/AircraftElmentShapeDisplay.h>

class CDeck;
class CDeckManager;
class CAircaftLayOut;
class CAircraftElmentShapeDisplay;
class CAircraftLayout3DNode;



class CAircraftElmentNode : public AutoDetachNodeObject
{
public:
	CAircraftElmentNode(){  }
	CAircraftElmentNode(const CAircraftElementShapePtr& pElement, Ogre::SceneNode* pNode);

	virtual void Update( CAircraftLayout3DNode& layoutNode );
	//
	virtual void AttachToScene(CAircraftLayout3DNode& layoutNode);
	CAircraftElementShapePtr getKey()const{ return mpElement; }	
	CAircraftElementShapePtr GetElement()const{ return mpElement; }	


	//node name
	static CString GetNodeName(CAircraftElementShapePtr pShape);

	void UpdateTransform();
	void UpdateDisplay();
protected:
	CAircraftElementShapePtr mpElement;	
	C3DNodeObject UpdateShape(CAircraftLayout3DNode& layoutNode, const ARCVector3& vCenter = ARCVector3(0,0,0) );
	C3DNodeObject m_shapeNode;

	CString GetShapeObjName(){  return  GetIDName()  + _T("/Shape"); }
	void UpdateSeat(CAircraftLayout3DNode& layoutNode);
	void UpdateStair(CAircraftLayout3DNode& layoutNode);
	void UpdateEscaltor(CAircraftLayout3DNode& layoutNode );
	void UpdateElevator(CAircraftLayout3DNode& layoutNode );
	void UpdateWallShape(CAircraftLayout3DNode& layoutNode);
	void UpdateArea(CAircraftLayout3DNode& layoutNode);
	void UpdatePotral(CAircraftLayout3DNode& layoutNode);
	void UpdateCorridor(CAircraftLayout3DNode& layoutNode);

	void UpdateDeckElm(CAircraftLayout3DNode& layoutNode);
};



class CAircraftElmentNodeList : public Key3DNodeList<CAircraftElementShapePtr,CAircraftElmentNode>
{
public:
	void Update( CAircraftLayout3DNode& layoutNode);	

	void UpdateElement( CAircraftElementShapePtr pelm , CAircraftLayout3DNode& scene);
	void UpdateElements( const CAircraftElmentShapeDisplayList& elmlist, CAircraftLayout3DNode& scene );
	void UpdateElements( const ALTObjectID& grpid, CAircraftLayout3DNode& scene) ;
	//NodeTypePtr HasObject(Ogre::MovableObject* pobj); 
};