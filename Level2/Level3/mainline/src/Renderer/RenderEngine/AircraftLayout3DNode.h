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
#include <inputonboard/AircraftElmentShapeDisplay.h>
class OnboardFlightInSim;
class InputOnboard;

class RENDERENGINE_API CAircraftLayout3DNode : public C3DNodeObject
{
public:
	CAircraftLayout3DNode();
	CAircraftLayout3DNode(Ogre::SceneNode* pNode);

	//void Update(CAircraftLayoutScene& scene);
	void UpdateDecks(CDeckManager* pDeckMan);
	void UpdateDeck(CDeck* pdeck);
	C3DNodeObject GetDeckNode(CDeck* pdeck);

	//void UpdateAircraftModel();
	void UpdateAddDelElements();	

	CBaseFloor3DList mDeckList;
	CAircraftElmentNodeList mElmentsList;


	void LoadLayout(InputOnboard* pInputOnbaord, CAircaftLayOut* playout, C3DNodeObject& parentNode);

	static CString GetNodeName(CAircaftLayOut* layout);

	CAircaftLayOut* GetLayout()const{ return m_pLoadLayout; } 
	Ogre::Entity* GetFurnishingShapeEntity( CString entName, const CGuid& gid );


	void DrawDeckCells(int nDeckIndex, const CString& strDeckName, std::vector<std::pair<ARCColor3, Path > >& vCells );
protected:
	CAircaftLayOut* m_pLoadLayout;
	InputOnboard* m_pInputOnboard;

};