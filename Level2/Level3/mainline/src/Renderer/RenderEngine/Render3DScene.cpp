#include "StdAfx.h"
#include ".\render3dscene.h"
#include "CustomizedRenderQueue.h"
#include "Render3DSceneTags.h"
#include "RenderFillet3D.h"
#include "3DTerminalNode.h"
#include "RenderLandside3D.h"
#include "3DAirsideNode.h"
#include "InputAirside\IntersectionNodesInAirport.h"
#include "Common\IARCportLayoutEditContext.h"
#include "InputAirside\InputAirside.h"
#include "InputAirside\ALTAirportLayout.h"
#include "Mouse3DState.h"
#include "SceneState.h"
#include "Common\IEnvModeEditContext.h"
#include "Landside\LandsideLayoutObject.h"
#include "SceneNodeQueryData.h"
#include "Landside\LandsideQueryData.h"
#include "CommonData\tga.h"
#include "CommonData\Textures.h"
#include "boost\bind\bind.hpp"
#include "RenderConvey3D.h"
#include "Common\TempTracerData.h"
#include "Landside\ILandsideEditContext.h"
#include "Landside\LandsideIntersectionNode.h"
#include "Landside\InputLandside.h"
#include "LandIntersection3D.h"
#include "LandsideTrafficLight3D.h"
#include "Common\LANDSIDE_BIN.h"
#include "LandsideCrosswalk3D.h"
#include "FloorSurfaceRender.h"
// 

#define FloorsName _T("AllFloors")
#define AnimationPaxList _T("AnimationPaxList")
#define EditModelCamera _T("EditModelCamera")
#define EditModelName _T("EditModel")
#define LevelsName _T("AllLevels")


using namespace Ogre;
using namespace std;
const static CString sTerminal = "Terminal";
const static CString sLandside = "Landside";
const static CString sAirside = "Airside";


#pragma warning(disable:4244)
#pragma warning(disable:4305)

CRender3DScene::CRender3DScene()
	: mpEditor(NULL)
{


	m_lastAnimaTime = 0;
	


	m_pConveyTexture = NULL;

	//m_renderTags = new CRender3DSceneTags();
	//m_sharpScene = new CSharpModeBackGroundScene();

	m_pMouse3DOperation = NULL;
	m_pRenderFilletTaxiway3DInAirport = NULL;
	m_pFloorSurfaceRender = NULL;

}

CRender3DScene::~CRender3DScene()
{
	// must clear all shared pointers before Destroy() in CRender3DScene::~CRender3DScene()
	//delete m_sharpScene;
	C3DRootSceneNode root = mpScene->getRootSceneNode();
	delete root.GetImplPool();
	delete root.GetTagManager();
	if(m_pRenderFilletTaxiway3DInAirport!=NULL)
		delete m_pRenderFilletTaxiway3DInAirport;
	if(m_pFloorSurfaceRender!=NULL)
	{
		mpScene->removeRenderObjectListener(m_pFloorSurfaceRender);
		mpScene->removeRenderQueueListener(m_pFloorSurfaceRender);
	}

}
static void SetupCamLight(SceneManager* mSceneMgr)
{
	Ogre::Light* l = OgreUtil::createOrRetrieveLight( "Camera Light", mSceneMgr);	
	//l->setSpecularColour( ColourValue::White );
	l->setDiffuseColour( /*ColourValue::White*/ ColourValue(0.9f, 0.9f, 0.9f));
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(-Vector3::UNIT_Z);		
	l->setVisible(true);
	//l->setSpotlightRange(Degree(30), Degree(50));
}

static void SetupShadow(SceneManager* mSceneMgr)
{
	Ogre::Light* mSunLight = OgreUtil::createOrRetrieveLight( "Sun Light", mSceneMgr );	

	mSunLight->setType(Light::LT_DIRECTIONAL);
	mSunLight->setPosition(0,0,130000);
	Vector3 dir;
	dir = -mSunLight->getPosition();
	dir.normalise();
	mSunLight->setDirection(dir);
	mSunLight->setDiffuseColour(/*ColourValue::White*/ColourValue(0.9f, 0.9f, 0.9f));
	//mSunLight->setSpecularColour(ColourValue::White);
	
	// In D3D, use a 1024x1024 shadow texture
	//mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	//mSceneMgr->setShadowTextureSettings(1024, 2);	
	//mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));

}



void CRender3DScene::Setup(IARCportLayoutEditContext* pEditContext)
{
	mpEditor = pEditContext;

	if(!MakeSureInited())
	{
		ASSERT(FALSE);
		return;
	}
	CRenderSceneRootNode rootNode = GetRoot();
	rootNode.setRenderScene(this);
	//set up sun lighting	
	getSceneManager()->setAmbientLight(/*ColourValue::White*/ColourValue(0.3f, 0.3f, 0.3f));
	//SetupShadow(getSceneManager());
	SetupCamLight(getSceneManager());

	RenderQueue* mRenderQueue = mpScene->getRenderQueue();
	mRenderQueue->getQueueGroup(RenderObject_Base)->setShadowsEnabled(false);
	mRenderQueue->getQueueGroup(RenderFloors_Solid)->setShadowsEnabled(false);

	mRenderQueue->getQueueGroup(RenderFloors_SurfaceMap)->setShadowsEnabled(false);
	mRenderQueue->getQueueGroup(RenderObject_Surface)->setShadowsEnabled(false);
	mRenderQueue->getQueueGroup(RenderObject_Mark)->setShadowsEnabled(false);
	mRenderQueue->getQueueGroup(RenderObject_EditHelper)->setShadowsEnabled(false);
	mRenderQueue->getQueueGroup(RenderObject_AnimationObject)->setShadowsEnabled(false);

 	m_pFloorSurfaceRender = FloorSurfaceRender::getIntance();
	m_pFloorSurfaceRender->SetStencilValue(0);
 	mpScene->addRenderQueueListener(m_pFloorSurfaceRender);
 	mpScene->addRenderObjectListener(m_pFloorSurfaceRender);
	//UpdateProcessorTagShow(mpEditor->IsShowText());

	///
	//m_picLine3D = AddSceneNode(ModelPickLine);
	m_animPaxList.SetRootNode(rootNode);
	m_flightList.SetRootNode(rootNode);
	m_vehicleList.SetRootNode(rootNode);
	m_animElevatorList.SetRootNode(rootNode);
	m_animTransList.SetRootNode(rootNode);
	m_animResourceList.SetRootNode(rootNode);
	m_animlsVehcile3DList.SetRootNode(rootNode);

	
	//setup terminal
	{
		C3DRootSceneNode rootNode  = mpScene->getRootSceneNode();
		rootNode.SetModel(pEditContext);
		rootNode.SetImplPool(new ISceneNodeImplPool());
		rootNode.SetTagManager(new CRender3DSceneTags(/*mpEditor*/));
		
		//terminal
		C3DTerminalNode terminalNode = rootNode.GetCreateChildNode(sTerminal);
		terminalNode.UpdateAll(pEditContext );		

		//landside
		CRenderLandside3D landside3D = rootNode.GetCreateChildNode(sLandside);
		landside3D.Setup( pEditContext );	

		//Airside
		C3DAirsideNode airside3D = rootNode.GetCreateChildNode(sAirside);
		airside3D.UpdateAll(pEditContext);
	}

	//FilletTaxiway
	m_pRenderFilletTaxiway3DInAirport = new CRenderFilletTaxiway3DInAirport(this);
	IntersectionNodesInAirport lstIntersecNode;
	InputAirside* airside = pEditContext->OnQueryInputAirside();
	if(!airside)return;
	ALTAirportLayout* pairport = airside->GetActiveAirport();
	int nAirportID = pairport->getID();
	lstIntersecNode.ReadData(nAirportID);
	m_pRenderFilletTaxiway3DInAirport->Init(nAirportID,lstIntersecNode);
	m_pRenderFilletTaxiway3DInAirport->Update();
}

Ogre::Plane CRender3DScene::getFocusPlan() const
{
	double dheight = 0;
	if(IEnvModeEditContext* pCtx = GetEditContext()){
		dheight = pCtx->GetWorkingHeight();
	}
	return Ogre::Plane(Vector3::UNIT_Z,Vector3(0.0, 0.0, dheight) );
}

BOOL CRender3DScene::OnMouseMove( const MouseSceneInfo& mouseInfo )
{

	if(m_pMouse3DOperation)
	{
		m_pMouse3DOperation->OnMouseMove(*this,mouseInfo);
		return TRUE;
	}
	else if(mpEditor->IsEditable())
	{
		if(mouseInfo.mnFlags&MK_LBUTTON)
		{
			CRenderScene3DNode nodeObj = GetSel3DNodeObj(mState->m_lastLBtnDownInfo);
			if( nodeObj )
			{					
				if(nodeObj.IsInDragging() )
				{						
					nodeObj.OnDrag(mState->m_LastMoseMoveInfo, mouseInfo);	

				}
				else if(nodeObj.IsDraggable())
				{
					nodeObj.StartDrag(mState->m_lastLBtnDownInfo,mouseInfo);											
				}
			}
		}				
	}
	else{
	}
	C3DSceneBase::OnMouseMove( mouseInfo );
	return FALSE;
}

BOOL CRender3DScene::OnLButtonDown( const MouseSceneInfo& mouseInfo)
{	
	//RenderVehicleTraces();

	return C3DSceneBase::OnLButtonDown(mouseInfo);
}

BOOL CRender3DScene::OnLButtonUp( const MouseSceneInfo& mouseInfo )
{	
	if(m_pMouse3DOperation)
	{
		m_pMouse3DOperation->OnPick(*this, mouseInfo);
	}
	else if(mpEditor->IsEditable())
	{
		//check if it is in drag
		CRenderScene3DNode nodeObj = GetSel3DNodeObj(mState->m_lastLBtnDownInfo);
		if( nodeObj )
		{
			if(nodeObj.IsInDragging())
			{
				nodeObj.EndDrag();						
			}
			
		}
		CLayoutObject3DNode LayoutObj = GetSel3DNodeObj(mState->m_lastLBtnDownInfo);
		LandsideFacilityLayoutObject* landsideobject = LayoutObj.getLayOutObject();
		if (landsideobject)
		{
			GetEditContext()->selectObject(landsideobject,false);
		} 
		else
		{
			GetEditContext()->selectObject(landsideobject,false);
		}
	}
	else
	{
		CLandsideVehicle3D nodeObj = GetSel3DNodeObj(mState->m_lastLBtnDownInfo);
		const Any& userdata = nodeObj.GetAny();
		const SceneNodeQueryData*  pQueryData = any_cast<SceneNodeQueryData>(&userdata);
		if (pQueryData)
		{
			//SceneNodeQueryData* VehicleQueryData = pQueryData;
			if (pQueryData->eNodeType == SceneNode_Vehicle)
			{
				int id = any_cast<int>(pQueryData->nodeData);
				DEAL_LISTENER_HANDLER(OnClick3DNodeObject(&nodeObj, SceneNode_Vehicle,id));
			}		
			
		}		
	}


	return C3DSceneBase::OnLButtonUp(mouseInfo);
}


BOOL CRender3DScene::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{

	if(m_pMouse3DOperation)
	{
		m_pMouse3DOperation->OnLastPick(*this,mouseInfo);
	}
	else if(mpEditor->IsEditable())
	{
		CRenderScene3DNode node = GetSel3DNodeObj(mouseInfo);
		node.OnRButtonDown(mouseInfo);
		if(SceneNodeQueryData* pData = node.getQueryData())
		{	
			GetEditContext()->setRClickItemData(*pData).SetData(Key3DSceneNodeName,node.GetIDName());	
		}
		CLayoutObject3DNode LayoutObj = GetSel3DNodeObj(mouseInfo);
		LandsideFacilityLayoutObject* landsideobject = LayoutObj.getLayOutObject();
		if (landsideobject)
		{
			GetEditContext()->selectObject(landsideobject,false);
		} 
		else
		{
			GetEditContext()->selectObject(landsideobject,false);
		}
	}

	return C3DSceneBase::OnRButtonDown(mouseInfo);
}





void CRender3DScene::UpdateDrawing()
{
	DEAL_LISTENER_HANDLER(OnUpdateDrawing());
}

void CRender3DScene::FlipLayoutLocked()
{
	GetEditContext()->flipLockView();
	if (GetEditContext()->IsViewLocked())
	{
		GetEditContext()->DoneCurEditOp();
	}
}



//void CRender3DScene::UpdateFloorAltitude(CRenderFloor* floor )
//{	
//	C3DFloorNode floorNode = GetSceneNode(floor->GetUID());
//	floorNode.UpdateAltitude(floor->Altitude());
//	//update landside objects
//	if(GetEditContext()->toLandside()){
//
//	}
//}

//
//template <typename NodeDataType>
//static inline void SetSceneNodeQueryData(C3DNodeObject& node, SceneNodeType eNodeType, NodeDataType nodeData)
//{
//	CRender3DScene::SetSceneNodeQueryData(node, SceneNodeQueryData(eNodeType, Ogre::Any(nodeData)));
//}
//static inline SceneNodeQueryData GetSceneNodeQueryData(C3DNodeObject& node)
//{
//return Ogre::any_cast<SceneNodeQueryData>(node.GetAny());
//}

//************************************
// Method:    AddGUIDQueryableSceneNode
// FullName:  CRender3DScene::AddGUIDQueryableSceneNode
// Access:    public 
// Returns:   Ogre::SceneNode*
// Qualifier:
// Parameter: const CGuid & guid
// Parameter: const GUID & cls_guid
// Remark:		Add queryable SceneNode to current Scene,
//				store object guid as the SceneNode's unique name,
//				store class guid as SceneNode's (user) name
//************************************
//Ogre::SceneNode* CRender3DScene::AddGUIDQueryableSceneNode( const CGuid& guid, const GUID& cls_guid )
//{
//	Ogre::SceneNode* pNode = AddSceneNode(guid);
//	if (pNode)
//	{
//		::SetSceneNodeQueryData(C3DNodeObject(pNode), SceneNode_useGUID, GUIDQueryData(guid, cls_guid));
//		return pNode;
//	}
//	return NULL;
//}

//************************************
// Method:    ReferToParentWhenQuery
// FullName:  CRender3DScene::ReferToParentWhenQuery
// Access:    public 
// Returns:   void
// Qualifier: const
// Parameter: C3DNodeObject nodeObj
// Remark:    
//************************************
//void CRender3DScene::ReferToParentWhenQuery( C3DNodeObject nodeObj )
//{
//	SetSceneNodeQueryData(nodeObj, SceneNodeQueryData(SceneNode_RefParent));
//}

//************************************
// Method:    Query3DObject
// FullName:  CRender3DScene::Query3DObject
// Access:    public 
// Returns:   CTerminal3DObject::SharedPtr
// Qualifier:
// Parameter: C3DNodeObject org3DObj
// Remark:    Query object from a C3DNodeObject, refer to CRender3DScene::AddGUIDQueryableSceneNode for more info
//************************************
//C3DNodeObject* CRender3DScene::Query3DObject( C3DNodeObject org3DObj, SceneNodeQueryData& nodeQueryData )
//{
//
//	while (GetSceneNodeQueryData(org3DObj).eNodeType == SceneNode_RefParent)
//	{
//		org3DObj = org3DObj.GetParent();
//	}
//	nodeQueryData = GetSceneNodeQueryData(org3DObj);
//	switch (nodeQueryData.eNodeType)
//	{
//	case SceneNode_useGUID:
//		{
//			//GUIDQueryData& guidData = Ogre::any_cast<GUIDQueryData>(nodeQueryData.nodeData);
//			//return QueryGUID3DObject(guidData.objGUID, guidData.clsGUID).get();
//		}
//		break;
//	case SceneNode_Pax:
//		{
//			int nPaxID = Ogre::any_cast<int>(nodeQueryData.nodeData);
//			return m_animPaxList.GetMob3D(nPaxID);
//		}
//		break;
//	case SceneNode_Flight:
//		{
//			int nFlightID = Ogre::any_cast<int>(nodeQueryData.nodeData);
//			return m_flightList.GetMob3D(nFlightID);
//		}
//		break;
//	case SceneNode_Vehicle:
//		{
//			int nVehicleID = Ogre::any_cast<int>(nodeQueryData.nodeData);
//			return m_vehicleList.GetMob3D(nVehicleID);
//		}
//		break;
//	default:
//		{
//			//ASSERT(FALSE);
//		}
//		break;
//	}
//	return NULL;
//}
//
tgaInfo* CRender3DScene::GetTextureInfo( unsigned int nID ) const
{
	if (nID>=m_vecTextures.size())
	{
		ASSERT(FALSE);
		return NULL;
	}
	tgaInfo* pInfo = m_vecTextures[nID];
	ASSERT(pInfo && TGA_OK == pInfo->status);
	return pInfo;
}

void CRender3DScene::ReloadTextures()
{
	ReleaseTextures();
	CString texturePath = GetPath(TexturePath);
	int nTextureCount = Textures::GetNumTextures();
	m_vecTextures.reserve(nTextureCount);
	for(int i=0;i<nTextureCount;i++)
	{
		m_vecTextures.push_back(Textures::GetTextureInfo((Textures::TextureEnum)i, texturePath));
	}
}

void CRender3DScene::ReleaseTextures()
{
	std::for_each(m_vecTextures.begin(), m_vecTextures.end(), boost::BOOST_BIND(&Textures::ReleaseTextureInfo, _1));
	m_vecTextures.clear();
}

CString CRender3DScene::GetPath( PathType type ) const
{
	return mpEditor->GetTexturePath();
}
void CRender3DScene::SetSceneNodeQueryData( C3DNodeObject& node, const SceneNodeQueryData& data )
{
	node.SetAny(Ogre::Any(data));
}

void CRender3DScene::SetSceneNodeQueryDataInt( C3DNodeObject& node, SceneNodeType eNodeType, int nData )
{
	SetSceneNodeQueryData(node, SceneNodeQueryData(eNodeType, Ogre::Any(nData)));
}



void CRender3DScene::UpdateAnim( long t )
{
	m_lastAnimaTime = t;
	
	//update convey animation
	if(!m_pConveyTexture)
		m_pConveyTexture = CRenderConvey3D::GetTextureUnit();
	if(m_pConveyTexture)
	{
		double dVs = t * 0.002;
		m_pConveyTexture->setTextureVScroll((int)dVs-dVs);
	}
}


void CRender3DScene::StartAnim( long t )
{

}




//void CRender3DScene::DetachSceneNode( const CString& guid )
//{
//	String strId = guid.GetString();
//	if( getSceneManager()->hasSceneNode(strId) )
//	{
//		SceneNode* pNode = getSceneManager()->getSceneNode(strId);
//		OgreUtil::RemoveSceneNode(pNode);
//	}
//}



void CRender3DScene::BeginMouseOperation( HWND fallbackWnd, LPARAM lParam )
{
	EndMouseOperation();	
	IRenderSceneMouseState* pSate = IRenderSceneMouseState::createState(lParam);
	if(pSate)
	{
		pSate->SetFallbackWnd(fallbackWnd,lParam);
		m_pMouse3DOperation = pSate;
	}
	CRenderLandside3D landside3d = GetSceneNode(sLandside);
	landside3d.OnUpdatePickMode(getMouseOperation());
	UpdateDrawing();
}


bool CRender3DScene::UpdateSelectRedSquares()
{
	//mSelectSquare.HideAll();
	if(IEnvModeEditContext* pContext = GetEditContext())
	{
		int nCount = pContext->getSelectCount();
		for(int i=0;i<nCount;i++)
		{
			ILayoutObject* pObj = pContext->getSelectObject(i);
			C3DNodeObject objNode = GetLayoutObjectNode(pObj->getGuid());
			MovableObject* pRedSquare = mSelectSquare.GetObject(i);
			if (pRedSquare->isVisible())
			{
				pRedSquare->setVisible(false);
			} 
			else
			{
				pRedSquare->setVisible(true);
				objNode.AddObject(pRedSquare);
			}
			
		}
		if (nCount == 0)
		{
			MovableObject* predsquare = mSelectSquare.GetObject(0);
			predsquare->setVisible(false);
		}
		return nCount>0;		
	}	
	return false;
}

char* Line_Types[9]={ 
	"solid1", "solid2", "solid3",
	"dash1", "dash2", "dash3",
	"dot1", "dot2", "dot3"
};
void CRender3DScene::RenderVehicleTraces(CTempTracerData* tempVehicleTracerData,bool mShow)
{
	GetRoot().RemoveAllObject();
	if (mShow)
	{
		int nVehicleCount = tempVehicleTracerData->GetTrackCount();
		for (int i=0;i<nVehicleCount;i++)
		{

			CString sName;
			sName.Format("Vehicle Trace %d",i);
			//if (mShow)
			{ 
				std::vector<CTrackerVector3>& track = tempVehicleTracerData->GetTrack(i);
				int nEventCount = (int)track.size();
				if(nEventCount>0)
				{
					COLORREF mColor = tempVehicleTracerData->GetColor(i);
					int mLineType = tempVehicleTracerData->GetLineType(i);
					ManualObject* manual=OgreUtil::createOrRetrieveManualObject(sName,getSceneManager());
					manual->clear();
					manual->begin(Line_Types[mLineType], RenderOperation::OT_LINE_STRIP);
					manual->position(track[0].x,track[0].y,track[0].z+0.01*i+10);
					manual->colour(OGRECOLOR(ARCColor3(mColor),1.0f));
					manual->textureCoord(0,0.5);
					long mLength = 0;
					for (int j=1;j<nEventCount;j++)
					{
						mLength += track[j-1].DistanceTo(track[j]);
						manual->position(track[j].x,track[j].y,track[j].z+0.01*i+10);				
						manual->textureCoord(mLength*0.01,0.5);
					}
					manual->end();
					GetRoot().AddObject(manual);
				}
			}
			/*else
			{
				getSceneManager()->destroyManualObject(sName.GetString());
			}*/
		}
	}
}

IEnvModeEditContext* CRender3DScene::GetEditContext()const
{
	ASSERT(mpEditor);
	return mpEditor->GetCurContext();
}



bool CRender3DScene::OnUpdate( LPARAM lHint, CObject* pHint )
{
	//for now we have landside only
	CRenderLandside3D landside3D = GetSceneNode(sLandside);
	if(landside3D)
		return landside3D.OnUpdate(lHint,pHint);

	return true;
}

C3DNodeObject CRender3DScene::GetLayoutObjectNode( const CGuid& guid )
{
	return OgreUtil::createOrRetrieveSceneNode(guid,getSceneManager());
}

void CRender3DScene::EndMouseOperation()
{
	if(m_pMouse3DOperation)
	{
		cpputil::autoPtrReset(m_pMouse3DOperation);
		CRenderLandside3D landside3D = GetSceneNode(sLandside);
		landside3D.OnUpdatePickMode(getMouseOperation());
	}	
	
	GetEditContext()->ClearTempData();
	//update mode
	GetEditContext()->DoneCurEditOp();	
}


BOOL CRender3DScene::OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
{
	if( mpEditor->IsEditable() )
	{
		CRenderScene3DNode node = GetSel3DNodeObj(mouseinfo);
		node.OnLButtonDblClk(mouseinfo);
		UpdateDrawing();
	}
	return C3DSceneBase::OnLButtonDblClk(mouseinfo);
}



void CRender3DScene::StopAnim()
{
	
	m_animPaxList.UpdateAllMobile(false);
	m_animElevatorList.UpdateAllMobile(false);
	m_animTransList.UpdateAllMobile(false);
	m_animResourceList.UpdateAllMobile(false);
	m_animlsVehcile3DList.UpdateAllMobile(false);

	m_flightList.UpdateAllMobile(false);
	m_vehicleList.UpdateAllMobile(false);

}

void CRender3DScene::SetIntersectionTrafficLight(int nIntersectionID,int nGroupID,LinkGroupState eState)
{
	if(!mpEditor)
		return;

	LandsideFacilityLayoutObject* pObj = mpEditor->GetLandsideContext()->getInput()->getObjectList().getObjectByID(nIntersectionID);
	LandsideIntersectionNode *intersectionNode =NULL;

	if(pObj && pObj->GetType() == ALT_LINTERSECTION)
	{
		intersectionNode = (LandsideIntersectionNode *)pObj;
	}
	if(!intersectionNode)
		return;

	LandIntersection3D intersection3D = OgreUtil::createOrRetrieveSceneNode(intersectionNode->getGuid(),getSceneManager());

	//LandsideIntersectionNode::LaneNodeGroupMap &laneNodeGroupMap=intersectionNode->GetLaneGroupMap();
	//LandsideIntersectionNode::Iter_LaneGroupMap it_laneGroupMap;
	//for (it_laneGroupMap=laneNodeGroupMap.begin();it_laneGroupMap!=laneNodeGroupMap.end();it_laneGroupMap++)
	LandsideIntersectionNode::LandsideStretchGroupMap &map_landsideStretchGroup=intersectionNode->GetLandsideStretchGroupMap();
	LandsideIntersectionNode::Iter_LandsideStretchGroupMap it_landsideStretchGroup;
	for (it_landsideStretchGroup=map_landsideStretchGroup.begin();it_landsideStretchGroup!=map_landsideStretchGroup.end();it_landsideStretchGroup++)
	{
		LandsideIntersectionNode::Iter_GroupLinkMap it_groupLinkMap=it_landsideStretchGroup->second.find(nGroupID);
		if(it_groupLinkMap!=it_landsideStretchGroup->second.end())
		{
			LandsideIntersectionNode::LinkageList nLinkageList = it_groupLinkMap->second;
			//int LinkageListSize = static_cast<int>(nLinkageList.size());
			//bool flag = intersectionNode->getLinkage(nLinkageList[0]).isCrossOver;
			//for (int i = 1;i<LinkageListSize;i++)
			//{
			//	flag^= intersectionNode->getLinkage(nLinkageList[i]).isCrossOver;
			//	//LandsideStretch *pStretch = intersectionNode->getLinkage(nLinkageList[i]).mLaneExit.m_pStretch;
			//	if(flag)
			//		return;
			//}
			bool isCrossOver = intersectionNode->getLinkage(nLinkageList[0]).isCrossOver;
			//bool isCrossOver  = true;
			//3D light
			{
				CString sName;
				sName.Format("%s-TrafficLight3DNode-%d",intersection3D.GetIDName(), it_landsideStretchGroup->first->getID());
				LandsideTrafficLight3D light3D = OgreUtil::createOrRetrieveSceneNode(sName, getSceneManager());
				switch (eState)
				{
				case LS_ACTIVE:
					{
						light3D.SetGreen(isCrossOver);
						break;
					}
				case LS_BUFFER:
					{
						light3D.SetYellow(isCrossOver);
						break;
					}
				case LS_CLOSE:
					{
						light3D.SetRed(isCrossOver);
						break;
					}
				default:
					{
						break;
					}
				}
			}
			{
				//2D Light
				CString sName;
				sName.Format("%s-TrafficLight2DNode-%d",intersection3D.GetIDName(), it_landsideStretchGroup->first->getID());
				LandsideTrafficLight3D light2D = OgreUtil::createOrRetrieveSceneNode(sName, getSceneManager());
				switch (eState)
				{
				case LS_ACTIVE:
					{
						light2D.SetGreen(isCrossOver);
						break;
					}
				case LS_BUFFER:
					{
						light2D.SetYellow(isCrossOver);
						break;
					}
				case LS_CLOSE:
					{
						light2D.SetRed(isCrossOver);
						break;
					}
				default:
					{
						break;
					}
				}
			}
		}
	}
}

void CRender3DScene::SetCrossWalkTrafficLight(int nCrossWalkID,Cross_LightState eState)
{
	LandsideCrosswalk * pCrossWalk=(LandsideCrosswalk *)(GetEditContext()->toLandside()->getInput()->getObjectList().getObjectByID(nCrossWalkID));
	LandsideCrosswalk3D cross3D = OgreUtil::createOrRetrieveSceneNode(pCrossWalk->getGuid(),getSceneManager());
	cross3D.SetLight(eState);
}

CString CRender3DScene::GetMouseHintText() const
{
	if(m_pMouse3DOperation)
	{
		return m_pMouse3DOperation->getHintText();
	}
	return "";
}

BOOL CRender3DScene::isInDistanceMeasure() const
{
	if(m_pMouse3DOperation)
	{
		return m_pMouse3DOperation->toDistanceMeasureState()!=NULL;
	}
	return FALSE;
}

IRenderSceneMouseState* CRender3DScene::getMouseOperation()const
{
	return m_pMouse3DOperation;
}

void CRender3DScene::SetupSharpRect()
{
	//setup material
	//setup rect
}

CRender3DSceneTags& CRender3DScene::Get3DTags()
{
	C3DRootSceneNode root = mpScene->getRootSceneNode();
	return *root.GetTagManager();
}

void CRender3DScene::UpdataTerminalProcessors(IARCportLayoutEditContext* pEdCtx)
{
	C3DRootSceneNode rootNode  = mpScene->getRootSceneNode();
	//terminal
	C3DTerminalNode terminalNode = rootNode.GetCreateChildNode(sTerminal);
	terminalNode.UpdateTerminalProcssor(pEdCtx );
}



//void CRender3DScene::UpdateProcessorTagShow( BOOL b )
//{
//	if(b)
//		addVisibilityFlags(ProcessorTagVisibleFlag);
//	else
//		removeVisibilityFlags(ProcessorTagVisibleFlag);
//}
//


