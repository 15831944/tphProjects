#include "StdAfx.h"
#include ".\aircraftlayout3dnode.h"
#include <InputOnBoard/AircraftLayoutEditContext.h>
#include <InputOnBoard/AircaftLayOut.h>
#include <InputOnBoard/AircraftPlacements.h>
#include <Inputonboard/Deck.h>
#include "ogreutil.h"
#include <common/ARCStringConvert.h>
#include ".\AircraftLayoutScene.h"
#include <common\FileInDB.h>
#include <InputOnBoard\CInputOnboard.h>
#include "RenderEngine.h"
#include "Engine/GroundInSim.h"
#include "Engine/OnboardDeckGroundInSim.h"
#include "MaterialDef.h"


using namespace Ogre;

void CAircraftLayout3DNode::UpdateDecks( CDeckManager* pDeckMan )
{
	mDeckList.Update(pDeckMan->getList(),*this);
}

void CAircraftLayout3DNode::UpdateDeck( CDeck* pdeck )
{
	CBaseFloor3DList::NodeTypePtr item = mDeckList.FindItem(pdeck);
	if(item.get())
	{
		item->Update(*this);
	}
}
#define STR_DECKSNODE _T("DecksSceneNode")
//void CAircraftLayout3DNode::Update( CAircaftLayOut* playout )
//{		
//	
//	//UpdateAircraftModel(scene);
//	UpdateDecks(playout->GetDeckManager());
//}

void CAircraftLayout3DNode::UpdateAddDelElements()
{	
	mElmentsList.Update(*this);
}




//void CAircraftLayout3DNode::UpdateAircraftModel( CAircraftLayoutScene& scene )
//{	
//	bool bShow = scene.GetEditLayout()->IsShowAC();
//	mAircraftNode.Detach();
//	if(bShow)
//	{
//		Ogre::Entity* pEnt = scene.GetAircraftShapeEntity(mAircraftNode.GetIDName(), scene.GetEditLayout()->GetAcType() );
//		ASSERT(pEnt);
//		if(pEnt)
//		{
//			pEnt->setMaterialName(_T("FadeClose_CG") );
//			pEnt->setRenderQueueGroup(RENDER_QUEUE_7);
//			mAircraftNode.AddObject(pEnt);
//			mAircraftNode.AttachTo(*this);
//		}
//	}
//}

CAircraftLayout3DNode::CAircraftLayout3DNode( Ogre::SceneNode* pNode ) :C3DNodeObject(pNode)
{
	//CString sNodeName = GetIDName() + _T("/AircraftModel");
	//mAircraftNode  = OgreUtil::createOrRetrieveSceneNode(sNodeName,GetScene());
}

CAircraftLayout3DNode::CAircraftLayout3DNode()
{
	m_pLoadLayout = NULL;
	m_pInputOnboard = NULL;
}
CString CAircraftLayout3DNode::GetNodeName( CAircaftLayOut* layout )
{
	const static CString Prefix = _T("LayoutNode");
	return Prefix + ARCStringConvert::toString((int)layout);
}

C3DNodeObject CAircraftLayout3DNode::GetDeckNode( CDeck* pdeck )
{
	CBaseFloor3DList::NodeTypePtr item = mDeckList.FindItem(pdeck);
	if(item.get())
	{
		return *item.get();
	}
	return C3DNodeObject();
}

void CAircraftLayout3DNode::LoadLayout(InputOnboard* pInputOnbaord, CAircaftLayOut* playout, C3DNodeObject& parentNode )
{
	m_pInputOnboard = pInputOnbaord;
	m_pLoadLayout = playout;

	UpdateDecks(playout->GetDeckManager());    
	UpdateAddDelElements();
	AttachTo(parentNode);
}

Ogre::Entity* CAircraftLayout3DNode::GetFurnishingShapeEntity( CString entName, const CGuid& gid )
{
	CString strMeshName = _T("Cube60.mesh");
	CModel* pModel = m_pInputOnboard->GetFurnishingModel(gid);
	if(pModel)
	{
		if(!pModel->MakeSureFileOpen()){ ASSERT(FALSE); }
			ResourceFileArchive::AddDirectory(pModel->msTmpWorkDir,(CString)pModel->mguid); //make sure resource added
		strMeshName = pModel->GetMeshFileName();
	}
	return OgreUtil::createOrRetrieveEntity(entName,strMeshName,GetScene());
}


void CAircraftLayout3DNode::DrawDeckCells(int nDeckIndex, const CString& strDeckName, std::vector<std::pair<ARCColor3, Path > >& vCells )
{

	CBaseFloor3DList::NodeTypePtr item = mDeckList.GetItem(nDeckIndex);
	if(item.get())
	{
		CString strID;
		strID.Format("Floor %d",(int)item->getKey());
		Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(strID,GetScene());
		for (int j = 0; j <static_cast<int>(vCells.size()); j++)
		{
			std::pair<ARCColor3, Path >& cell = vCells[j];
			ColourValue c = OGRECOLOR(cell.first);
			//draw cell with color
			{
				pObj->begin(VERTEXCOLOR_LIGHTON,RenderOperation::OT_TRIANGLE_LIST);
				for (int n = 0; n < cell.second.getCount(); n++)
				{
					Vector3 thisPos = OGREVECTOR(cell.second.getPoint(n));
					pObj->position(thisPos);
					pObj->colour(c);
				}
				pObj->quad(0,1,2,3);
				pObj->end();
			}
		}
		item->AddObject(pObj);
	}
}

































