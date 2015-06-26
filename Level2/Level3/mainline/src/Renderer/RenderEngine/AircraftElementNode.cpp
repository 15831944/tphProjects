#include "stdafx.h"
#include "./AircraftElementNode.h"
#include "./AircraftLayoutScene.h"
#include <inputonboard/DeckPlacement.h>
#include <InputOnBoard/Deck.h>
#include <InputOnBoard/AircaftLayOut.h>
#include <InputOnBoard/AircraftPlacements.h>
#include <InputOnBoard/OnboardCorridor.h>
#include "selectionmanager.h"
#include <common/ARCStringConvert.h>
#include ".\RenderProcessor3D.h"
#include <common/point.h>
#include "./shapebuilder.h"
#include "Render3DScene.h"
#include "ogreutil.h"
#include <common/ARCPipe.h>
#include "materialdef.h"

//#define MATERIAL_GROUP _T("AircraftLayout_Mat_Group")

using namespace Ogre;

CString CAircraftElmentNode::GetNodeName( CAircraftElementShapePtr pShape )
{
	const static CString Prefix = _T("AircraftElementNode/");
	return Prefix + ARCStringConvert::toString(int(pShape));
}


void CAircraftElmentNodeList::Update(  CAircraftLayout3DNode& layoutNode )
{
	CAircraftElmentShapeDisplayList& vFlrList = layoutNode.GetLayout()->GetPlacements()->mvElementsList;

	NodeList newList;
	for(int i=0;i<(int)vFlrList.Count();i++)
	{
		CAircraftElementShapePtr pElm = vFlrList.GetItem(i);
		NodeTypePtr item = FindItem(pElm);
		if(item.get())
		{
			newList.push_back(item);
		}
		else 
		{	
			CString strNodeName = layoutNode.GetIDName()+CAircraftElmentNode::GetNodeName(pElm);
			Ogre::SceneNode* pNewNode = OgreUtil::createOrRetrieveSceneNode(strNodeName,layoutNode.GetScene());
			NodeTypePtr newItem( new CAircraftElmentNode(pElm,pNewNode) );
			newItem->Update(layoutNode);
			newList.push_back(newItem);			
		}
		
	}
	mvNodeList = newList;	
}

void CAircraftElmentNodeList::UpdateElement( CAircraftElementShapePtr pelm , CAircraftLayout3DNode& scene )
{
	NodeTypePtr ptr = FindItem(pelm);
	if(ptr.get())
	{
		ptr->Update(scene);
	}
}

void CAircraftElmentNodeList::UpdateElements( const CAircraftElmentShapeDisplayList& elmlist, CAircraftLayout3DNode& scene )
{
	for(int i=0;i<elmlist.Count();i++)
	{
		UpdateElement(elmlist.GetItem(i),scene);
	}
}

void CAircraftElmentNodeList::UpdateElements( const ALTObjectID& grpid, CAircraftLayout3DNode& scene )
{
	for(int i=0;i<GetCount();i++)
	{
		CAircraftElmentNodeList::NodeTypePtr ptr=  GetItem(i);
		if(ptr->GetElement()->GetName().idFits(grpid))
		{
			ptr->Update(scene);
		}
	}
}
void CAircraftElmentNode::Update( CAircraftLayout3DNode& layoutNode )
{	
	if(!mpElement)
		return;

	CAircaftLayOut* playout = layoutNode.GetLayout();
	//attach self to scene
	AttachToScene(layoutNode);	
	
	CString strType = mpElement->GetType();
	if(strType==CSeat::TypeString)
	{
		UpdateSeat(layoutNode);
	}
	else if(strType == COnBoardStair::TypeString)
	{
		UpdateStair(layoutNode);
	}
	else if(strType == COnBoardEscalator::TypeString)
	{
		UpdateEscaltor(layoutNode);
	}
	else if(strType == COnBoardElevator::TypeString)
	{
		UpdateElevator(layoutNode);
	}
	else if(strType == COnBoardWall::TypeString)
	{
		UpdateWallShape(layoutNode);
	}
	else if(strType == COnboardSurface::TypeString)
	{
		UpdateArea(layoutNode);
	}
	else if(strType == COnboardPortal::TypeString)
	{
		UpdatePotral(layoutNode);
	}
	else if(strType == COnboardCorridor::TypeString)
	{
		UpdateCorridor(layoutNode);
	}
	else if(strType == CAircraftElmentShapeDisplay::UndefinedType) //only for shape
	{
		m_shapeNode = UpdateShape(layoutNode,mpElement->GetShapePos());		
		m_shapeNode.AttachTo(*this);
		SetPosition(mpElement->GetShapePos());
	}
	else 
	{
		UpdateDeckElm(layoutNode);
	}
	
	
}

CAircraftElmentNode::CAircraftElmentNode(const CAircraftElementShapePtr& pElement, Ogre::SceneNode* pNode ): AutoDetachNodeObject(pNode)
{
	mpElement = pElement;
}



void CAircraftElmentNode::AttachToScene( CAircraftLayout3DNode& layoutNode )
{
	if(CDeck* pDeck = mpElement->GetDeck())
	{
		CBaseFloor3DList::NodeTypePtr deckNode = layoutNode.mDeckList.FindItem(pDeck);
		if(deckNode.get()){
			AttachTo(*deckNode.get());			
		}
		else
			ASSERT(FALSE);
	}	
	else
	{
		AttachTo(layoutNode);
	}
}



C3DNodeObject CAircraftElmentNode::UpdateShape(CAircraftLayout3DNode& layoutNode,const ARCVector3& vCenter /*= ARCVector3(0,0,0) */ )
{
	CString sChild = GetIDName() + _T("/Shape");
	C3DNodeObject childNodeObj = OgreUtil::createOrRetrieveSceneNode(sChild,GetScene());
	childNodeObj.Detach();
	DispProp& dsp = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE];
	if(!dsp.bOn)
		return childNodeObj;

	Entity* pEnt = layoutNode.GetFurnishingShapeEntity(sChild,mpElement->mShapeID);
	//MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(dsp.cColor);
	if(pEnt)
	{
		pEnt->setUserAny( Any(mpElement) );
		//REG_SELECT(pEnt);		
		//if(!matPtr.isNull())
		//	pEnt->setMaterial(matPtr);
		childNodeObj.AddObject(pEnt,true);		
	}	

	childNodeObj.SetRotation( ARCVector3(0,0,mpElement->GetShapeRotate()) );
	childNodeObj.SetScale( mpElement->GetShapeScale() );
	childNodeObj.SetPosition( mpElement->GetShapePos() - vCenter );
	return childNodeObj;
}

//update seat
#define HLEN 10.0f
#define LEN 100.0f
void CAircraftElmentNode::UpdateSeat(CAircraftLayout3DNode& layoutNode)
{
	CSeat* pSeat = mpElement->IsSeat();
	ASSERT(pSeat);		
	DispProp& pdp = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE];
	{//update seat area

		CString sSeatArea = GetIDName()  + _T("/Area");
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sSeatArea,GetScene());
		OgreUtil::DetachMovableObject(pObj);
		if(pObj&& pdp.bOn )
		{
			pObj->clear();		
			//draw square line
			pObj->begin(VERTEXCOLOR_LIGTHOFF,RenderOperation::OT_LINE_STRIP);
			Real hfWidth = pSeat->GetWidth()*0.5f;
			Real hfLen = pSeat->GetLength()*0.5f;
			pObj->position(hfLen,hfWidth,0);
			pObj->colour(OGRECOLOR(pdp.cColor));
			pObj->position(-hfLen,hfWidth,0);
			pObj->position(-hfLen,-hfWidth,0);
			pObj->position(hfLen,-hfWidth,0);
			pObj->position(hfLen,hfWidth,0);
			pObj->end();			
					
			//.rotate(pSeat->GetRotation())
			//get direction arrow
			DispProp& pdpArrow = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_DIRECTINARROW];
			if(pdpArrow.bOn)
			{	
				Point ptFrom(0,0,0);
				Point ptTo(pSeat->GetLength() * 0.75,0,0);
				//ptTo.rotate(pSeat->GetRotation());
				Point ptArrow(pSeat->GetLength() * 0.75 + 5,0,0);
				//ptArrow.rotate(pSeat->GetRotation());

				Path arrowPath;
				arrowPath.init(2);
				arrowPath[0] = ptFrom;
				arrowPath[1] = ptTo;
				ProcessorBuilder::DrawPath(pObj,&arrowPath,OGRECOLOR(pdpArrow.cColor));

				ProcessorBuilder::DrawArrow(pObj,
					ptTo,
					ptArrow,
					OGRECOLOR(pdpArrow.cColor),
					8,
					10);
			}

			pObj->setUserAny( Any(mpElement) );
			//REG_SELECT(pObj);
			AddObject(pObj,true);
		}
	}	

	//update shape
	UpdateDeckElm(layoutNode);

}

//update stair
void CAircraftElmentNode::UpdateStair(CAircraftLayout3DNode& layoutNode )
{
	if(!mpElement)
		return;
	CAircaftLayOut* playout = layoutNode.GetLayout();
	CDeckManager* pDeckList = playout->GetDeckManager();
	COnBoardStair* pStair = mpElement->IsStair();
	if(!pStair || !pDeckList)
	{
		return;
	}
	//draw shape	
	RemoveChild(); //remove origne shape node
	CPath2008 realPath = pDeckList->GetRealPath(pStair->GetPathService());
	ARCVector3 vCenter;
	if(realPath.getCount())
	{
		vCenter = realPath.getPoint(0);
		realPath.DoOffset(-vCenter[VX],-vCenter[VY],-vCenter[VZ]);

		CString sShape = GetIDName()  + _T("/Shape");
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sShape,GetScene());
		DispProp& pdp = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE];
		if(pObj&& pdp.bOn )
		{
			pObj->clear();
			CShapeBuilder::DrawStair(pObj, pdp.cColor,ARCPipe(realPath,pStair->GetWidth()) );
			pObj->setUserAny( Any(mpElement) );			
			AddObject(pObj,true);
		}		
	}
	else
	{
		vCenter = mpElement->GetShapePos();
	}
	SetPosition(vCenter);
}

void CAircraftElmentNode::UpdateEscaltor(CAircraftLayout3DNode& layoutNode)
{
	if(!mpElement)
		return; 

	CAircaftLayOut* playout = layoutNode.GetLayout();
	COnBoardEscalator* pEscaltor = (COnBoardEscalator*)mpElement->mpElement;
	CDeckManager* pDeckList = playout->GetDeckManager();
	
	//draw shape
	RemoveChild(); 
	CPath2008 realPath = pDeckList->GetRealPath(pEscaltor->GetPathService());
	ARCVector3 vCenter;
	if(realPath.getCount()){
		vCenter = realPath.getPoint(0);
		realPath.DoOffset(-vCenter[VX],-vCenter[VY],-vCenter[VZ]);

		CString sShape = GetIDName()  + _T("/Shape");
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sShape,GetScene());
		OgreUtil::DetachMovableObject(pObj);
		DispProp& pdp = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE];
		if(pObj&& pdp.bOn )
		{
			pObj->clear();
			CShapeBuilder::DrawEscalator(pObj, ARCPipe(realPath,pEscaltor->GetWidth()),pdp.cColor );
			pObj->setUserAny( Any(mpElement) );
			//REG_SELECT(pObj);
			AddObject(pObj,true);
		}		
	}
	else 
	{
		vCenter = mpElement->GetShapePos();
	}
	SetPosition(vCenter);

}

void CAircraftElmentNode::UpdateElevator(CAircraftLayout3DNode& layoutNode)
{
	if(!mpElement)
		return; 
	COnBoardElevator* pElevator = (COnBoardElevator*)mpElement->mpElement;
	CAircaftLayOut* playout = layoutNode.GetLayout();
	CDeckManager* pDeckList = playout->GetDeckManager();	
	RemoveChild(); 
	//std::vector<double> vDeckHeights = pDeckList->GetDeckRealZ();
	int iMinFloor = pElevator->GetMinFloor();
	int iMaxFloor = pElevator->GetMaxFloor();
	std::vector<double> vDrawHeights = pDeckList->GetDeckVisualZ();
	for(int i=iMinFloor;i<=iMaxFloor;i++)
	{
		if(i < (int)vDrawHeights.size() && i>=0)
		{
			vDrawHeights.push_back(vDrawHeights.at(i));
		}
	}
	

	ARCVector3 vCenter;
	if(pElevator->GetPathService().getCount() )
	{
		vCenter = pElevator->GetPathService().getPoint(0);
		if(vDrawHeights.size())
			vCenter[VZ] = *vDrawHeights.begin();
		else
			vCenter[VZ] = 0;
	}
	else 
	{
		vCenter = mpElement->GetShapePos();
	}
	
	DispProp& pdp = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE];
	
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetShapeObjName(),GetScene());
	OgreUtil::DetachMovableObject(pObj);
	if(pObj&& pdp.bOn )
	{
		pObj->clear();
		


		pObj->setUserAny( Any(mpElement) );		
		AddObject(pObj,true);
	}		
	

	SetPosition(vCenter);
}

void CAircraftElmentNode::UpdateWallShape(CAircraftLayout3DNode& layoutNode)
{

	if(!mpElement)
		return; 

	COnBoardWall* pWall = (COnBoardWall*)mpElement->mpElement;

	
	CPath2008 path = pWall->getPath();
	double cX=0;double cY=0;
	for(int i=0;i<path.getCount();i++) //set z to 0
	{
		if(i==path.getCount()/2)
		{
			cX = path[i].getX(); cY=path[i].getY();
		}
		path[i].setZ(0);
	}
	path.DoOffset(-cX,-cY,0);
	
	
	RemoveChild(); 
	CString sShape = GetIDName()  + _T("/Shape");
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sShape,GetScene());
	OgreUtil::DetachMovableObject(pObj);
	DispProp& dsp = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE];
	MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(dsp.cColor);
	if(pObj&& dsp.bOn && !matPtr.isNull() )
	{
		pObj->clear();
		CShapeBuilder::DrawWallShape(pObj,matPtr->getName().c_str(), ARCPipe(path,pWall->getWidth()),pWall->getHeight() );
		pObj->setUserAny( Any(mpElement) );		
		//REG_SELECT(pObj);
		AddObject(pObj,true);
	}	
	SetPosition(ARCVector3(cX,cY,0));	
}

void CAircraftElmentNode::UpdateArea(CAircraftLayout3DNode& layoutNode)
{
	if(!mpElement)
		return; 

	COnboardSurface* pArea = (COnboardSurface*)mpElement->mpElement;

	RemoveChild(); 
	CPath2008 path = pArea->getPath();
	double cX=0;double cY=0;
	for(int i=0;i<path.getCount();i++) //set z to 0
	{
		cX+= path[i].getX(); cY+=path[i].getY();
		path[i].setZ(0);
	}
	cX/=path.getCount(); cY/=path.getCount();
	path.DoOffset(-cX,-cY,0);


	CString sShape = GetIDName()  + _T("/Shape");
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sShape,GetScene());
	OgreUtil::DetachMovableObject(pObj);
	DispProp& dsp = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE];

	CString strText = _T("Surface/")+pArea->getTexture();
	strText.MakeLower();
	//MaterialPtr matPtr = OgreUtil::createOrRetrieveMaterial(strText);
	if(pObj&& dsp.bOn )
	{
		pObj->clear();
		CShapeBuilder::DrawArea(pObj,strText,path,dsp.cColor);
		pObj->setUserAny( Any(mpElement) );		
		//REG_SELECT(pObj);
		AddObject(pObj,true);
	}		
	SetPosition(ARCVector3(cX,cY,0));
}

void CAircraftElmentNode::UpdateCorridor(CAircraftLayout3DNode& layoutNode)
{
	if(!mpElement)
		return; 

	COnboardCorridor* pCorridor = (COnboardCorridor*)mpElement->mpElement;

	RemoveChild();

	CPath2008 path;
	if (!pCorridor->GetDrawPath(path))
		return;

	double cX=0;double cY=0;
	for(int i=0;i<path.getCount();i++) //set z to 0
	{
		cX+= path[i].getX(); cY+=path[i].getY();
		path[i].setZ(0);
	}
	cX/=path.getCount(); cY/=path.getCount();
	path.DoOffset(-cX,-cY,0);


	CString sShape = GetIDName()  + _T("/Shape");
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sShape,GetScene());
	OgreUtil::DetachMovableObject(pObj);
	DispProp& dsp = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE];

	CString strText = _T("Surface/")+pCorridor->GetTextureName();
	strText.MakeLower();
	//MaterialPtr matPtr = OgreUtil::createOrRetrieveMaterial(strText);
	if(pObj&& dsp.bOn )
	{
		pObj->clear();
		CShapeBuilder::DrawArea(pObj,strText,path,dsp.cColor);
		pObj->setUserAny( Any(mpElement) );		
// 		REG_SELECT(pObj);
		AddObject(pObj, true);
	}		
	SetPosition(ARCVector3(cX,cY,0));
}

void CAircraftElmentNode::UpdatePotral(CAircraftLayout3DNode& layoutNode)
{
	if(!mpElement)
		return; 

	COnboardPortal* pArea = (COnboardPortal*)mpElement->mpElement;


	CPath2008 path = pArea->getPath();
	double cX=0;double cY=0;
	for(int i=0;i<path.getCount();i++) //set z to 0
	{
		cX+= path[i].getX(); cY+=path[i].getY();
		path[i].setZ(0);
	}
	cX/=path.getCount(); cY/=path.getCount();
	path.DoOffset(-cX,-cY,0);

	RemoveChild(); 
	CString sShape = GetIDName()  + _T("/Shape");
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sShape,GetScene());
	OgreUtil::DetachMovableObject(pObj);
	DispProp& dsp = mpElement->mDspProps[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE];
	MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(dsp.cColor);
	if(pObj&& dsp.bOn && !matPtr.isNull() )
	{
		pObj->clear();
		CShapeBuilder::DrawPath(pObj,matPtr->getName().c_str(),path,  dsp.cColor);
		pObj->setUserAny( Any(mpElement) );		
		//REG_SELECT(pObj);
		AddObject(pObj,true);
	}		
	SetPosition(ARCVector3(cX,cY,0));
}

void CAircraftElmentNode::UpdateDeckElm(CAircraftLayout3DNode& layoutNode)
{
	if(!mpElement)return;
	if(!mpElement->mpElement)return;

	CDeckElement* pDeckElm = mpElement->mpElement->ToDeckElment();
	ASSERT(pDeckElm);	

	//update shape
	m_shapeNode = UpdateShape(layoutNode,pDeckElm->GetPosition());
	m_shapeNode.AttachTo(*this);	

	//set position
	SetPosition(pDeckElm->GetPosition());

	SetRotation(ARCVector3(0,0,pDeckElm->GetRotation()));

}

void CAircraftElmentNode::UpdateTransform()
{

}

void CAircraftElmentNode::UpdateDisplay()
{

}