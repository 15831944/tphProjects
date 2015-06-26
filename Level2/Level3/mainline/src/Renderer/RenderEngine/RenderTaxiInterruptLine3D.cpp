#include "StdAfx.h"
#include ".\rendertaxiinterruptline3d.h"

#include <InputAirside/Taxiway.h>
#include <Common/Path2008.h>
#include <Common/ARCPath.h>

#include "ShapeBuilder.h"
#include "Render3DScene.h"
#include "CustomizedRenderQueue.h"

using namespace Ogre;


CRenderTaxiInterruptLine3D::CRenderTaxiInterruptLine3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScnene, CTaxiInterruptLine* pLine, size_t nIndex )
	: CAirside3DObject(pNode, p3DScnene, pLine->getGuid())
	, m_dDistanceToInterNode(pLine->GetDistToFrontIntersectionNode())
	, m_strName(pLine->GetName())
	, m_nItem((int)nIndex)
{
	m_FrontIntersectionNode.ReadData(pLine->GetFrontIntersectionNodeID());
	m_BackIntersectionNode.ReadData(pLine->GetBackIntersectionNodeID());
}

void CRenderTaxiInterruptLine3D::MoveLocation( DistanceUnit dx, DistanceUnit dy, DistanceUnit dz/*=0*/ )
{
	std::vector<TaxiwayIntersectItem*> vTaxiwayItems1 = m_FrontIntersectionNode.GetTaxiwayIntersectItemList();
	std::vector<TaxiwayIntersectItem*> vTaxiwayItems2 = m_BackIntersectionNode.GetTaxiwayIntersectItemList();
	std::vector<TaxiwayIntersectItem*>::const_iterator ite = std::find_first_of(
		vTaxiwayItems1.begin(), vTaxiwayItems1.end(), vTaxiwayItems2.begin(), vTaxiwayItems2.end(),
		IntersectItem::IDComparor());

	if( vTaxiwayItems1.end() != ite )
	{
		TaxiwayIntersectItem* pIntersectItem = *ite;
		CPath2008 taxiPath = pIntersectItem->GetTaxiway()->GetPath();
		float relateLen = taxiPath.GetDistIndex(pIntersectItem->GetDistInItem() + m_dDistanceToInterNode);
		int npt = (int)relateLen;
		if(npt>=taxiPath.getCount()-1)
			npt = taxiPath.getCount()-2;
		ARCVector3 vdir = taxiPath.getPoint(npt+1)-taxiPath.getPoint(npt);
		vdir[VZ]=0;
		vdir.Normalize();
		double newDis = vdir.dot(ARCVector3(dx,dy,dz))/vdir.Length();
		m_dDistanceToInterNode += newDis;
		double fullLength = taxiPath.GetTotalLength();
		if(pIntersectItem->GetDistInItem() + m_dDistanceToInterNode > fullLength)
			m_dDistanceToInterNode = fullLength - pIntersectItem->GetDistInItem();
		if(pIntersectItem->GetDistInItem() + m_dDistanceToInterNode <0)
			m_dDistanceToInterNode = - pIntersectItem->GetDistInItem();
	}
}

void CRenderTaxiInterruptLine3D::FlushChangeOf()
{
	CTaxiInterruptLine* holdShortLineNode;
	TaxiInterruptLineList  holdshortlines;
	holdshortlines.ReadData(-1);
	holdShortLineNode = holdshortlines.GetItem(m_nItem);
	holdShortLineNode->SetDistToFrontIntersectionNode(m_dDistanceToInterNode);    

	holdShortLineNode->UpdateData();
}
void CRenderTaxiInterruptLine3D::Update3D(CTaxiInterruptLine* /*pLine*/, int nUpdateCode /*= Terminal3DUpdateAll*/)
{
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(mpNode->getName().c_str(),GetScene());
	Build(pObj);
	pObj->setVisibilityFlags(NormalVisibleFlag);
	// 	REG_SELECT(pObj);
	OgreUtil::AttachMovableObject(pObj,mpNode);
	UpdateSelected(GetSelected());
}

void CRenderTaxiInterruptLine3D::Build( Ogre::ManualObject* pObj )
{
	std::vector<TaxiwayIntersectItem*> vTaxiwayItems1 = m_FrontIntersectionNode.GetTaxiwayIntersectItemList();
	std::vector<TaxiwayIntersectItem*> vTaxiwayItems2 = m_BackIntersectionNode.GetTaxiwayIntersectItemList();
	std::vector<TaxiwayIntersectItem*>::const_iterator ite = std::find_first_of(
		vTaxiwayItems1.begin(), vTaxiwayItems1.end(), vTaxiwayItems2.begin(), vTaxiwayItems2.end(),
		IntersectItem::IDComparor());

	if( vTaxiwayItems1.end() != ite )
	{
		TaxiwayIntersectItem* pIntersectItem = *ite;
		Taxiway* pTaxiway = pIntersectItem->GetTaxiway();
		CPath2008 pTaxiwayPath = pTaxiway->GetPath();
		double fullLength = pTaxiwayPath.GetTotalLength();
		if(pIntersectItem->GetDistInItem()+m_dDistanceToInterNode>fullLength)
			m_dDistanceToInterNode = fullLength - pIntersectItem->GetDistInItem();
		if(pIntersectItem->GetDistInItem()+m_dDistanceToInterNode<0)
			m_dDistanceToInterNode = - pIntersectItem->GetDistInItem();
		CPoint2008 ptHoldShort = pTaxiwayPath.GetDistPoint(pIntersectItem->GetDistInItem()+m_dDistanceToInterNode);
		float relateLen = pTaxiwayPath.GetDistIndex(pIntersectItem->GetDistInItem()+m_dDistanceToInterNode);
		int npt = (int)relateLen;
		int nptnext;
		if(npt >= pTaxiwayPath.getCount() -1 )
		{
			nptnext = pTaxiwayPath.getCount() -2;
			npt = nptnext +1;
		}
		else 
		{
			nptnext = npt +1;
		}

		CPoint2008 dir = CPoint2008(pTaxiwayPath.getPoint(npt) - pTaxiwayPath.getPoint(nptnext));
		dir.setZ(0);
		dir.Normalize();
		dir.length(300/2);

		ARCPath3 drawPath;
		drawPath.push_back(ptHoldShort - dir);
		drawPath.push_back(ptHoldShort + dir);
		ColourValue clrYellow(1.0, 1.0, 0.0, 1.0);
		ProcessorBuilder::DrawTextureRepeatedPath(pObj, _T("Airside/HoldPosition"), clrYellow, drawPath,
			pTaxiway->GetWidth(), 0.0, pTaxiway->GetWidth()/250.0/*One clip per 2.5m*/);
		pObj->setRenderQueueGroup(RenderObject_Mark);
	}
}


//CRenderTaxiInterruptLine3D* CRenderTaxiInterruptLine3DList::CreateNew3D( CTaxiInterruptLine* pData, size_t nIndex ) const
//{
//	return new CRenderTaxiInterruptLine3D(m_p3DScene->AddGUIDQueryableSceneNode(pData), m_p3DScene, pData, nIndex);
//}