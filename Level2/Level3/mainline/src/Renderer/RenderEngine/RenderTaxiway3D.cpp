#include "StdAfx.h"
#include ".\rendertaxiway3d.h"
#include "ShapeBuilder.h"
#include <Common/ARCPath.h>

#include <InputAirside/ALTObjectDisplayProp.h>
#include "OgreConvert.h"
#include "CustomizedRenderQueue.h"
#include "InputAirside/IntersectionNodesInAirport.h"
#include "InputAirside/ALTAirportLayout.h"
#include "3DAirsideNode.h"
#include "Common/IARCportLayoutEditContext.h"
#include "InputAirside/InputAirside.h"
#include "ManualObjectUtil.h"
using namespace Ogre;

double CRenderTaxiway3D::m_dCenterlineZOffset = 5.0;

void CRenderTaxiway3D::Build(Taxiway* pTaxiway, ManualObject* pObj)
{
	pObj->clear();
	pObj->setRenderQueueGroup(RenderObject_Surface);

	const CPath2008& path2008 = pTaxiway->GetPath();
	int nCount = path2008.getCount();
	if(nCount < 2)
		return;

	TaxiwayDisplayProp dispProp;
	dispProp.ReadData(pTaxiway->getID());
	TaxiwayDisplayProp* pDispProp = &dispProp;
// 	TaxiwayDisplayProp* pDispProp = (TaxiwayDisplayProp*)pTaxiway->getDisplayProp();

	ARCPath3 drawPath(path2008);

	Ogre::ColourValue clrTransparent(1.0, 1.0, 1.0, 0.0);
	if (pDispProp->m_dpShape.bOn)
	{
		ProcessorBuilder::DrawTexturePath(pObj, _T("Airside/Taxiway"), clrTransparent, drawPath, pTaxiway->GetWidth()*1.05, 0.0);
	}

	int airportID =  C3DAirportNode(GetParent()).GetAirportID();
	ALTAirportLayout* airport  = GetRoot().GetModel()->OnQueryInputAirside()->GetAirportLayoutByID(airportID);
	SetRelatFillets(airport->m_vFilletTaxiways,pTaxiway);
	DistanceUnit fromDist=0;
	DistanceUnit toDist = path2008.GetTotalLength(); 
	if(m_vFilletPoints.size())
	{
		//begin
		const FilletPtRef& pFilletBegin = m_vFilletPoints.front();
		if( pFilletBegin.GetTaxiwayDist(pTaxiway->getID()) <  pFilletBegin.GetDistInObj() )
		{
			fromDist = pFilletBegin.GetDistInObj();
		}
		//end
		const FilletPtRef& pFilletEnd = m_vFilletPoints.back();
		if(pFilletEnd.GetTaxiwayDist(pTaxiway->getID()) > pFilletEnd.GetDistInObj() )
		{
			toDist = pFilletEnd.GetDistInObj();
		}
	}
	ARCPath3 cullPath(GetSubPath(pTaxiway->GetPath(), fromDist, toDist));

	ManualObject* pLineObj = _GetOrCreateManualObject(GetName() + _T("-CenterLine"));//OgreUtil::createOrRetrieveManualObject(GetIDName() + _T("-CenterLine"), GetScene());
	pLineObj->setRenderQueueGroup(RenderObject_Mark);
	ProcessorBuilder::DrawColorPath(pLineObj, OgreConvert::GetColor(pDispProp->m_dpCenterLine.cColor),
		Ogre::OgreUtil::ClrMat_TaxiwayCenterLine, cullPath, 50, false, m_dCenterlineZOffset);
	AddObject(pLineObj,true);

	//RenderTaxiwayMarking
	CPoint2008 markPos; double rotAngle;
	pTaxiway->GetMarkingPosition(markPos,rotAngle);
	ManualObjectUtil drawobj(pObj);
	drawobj.beginText(pTaxiway->GetWidth());
	drawobj.setColor(ARCColor4(255,255,255,255));
	drawobj.setIdentTransform();
	drawobj.rotate(ARCVector3::UNIT_Z,rotAngle);
	drawobj.translate(markPos);
	drawobj.drawCenterAligmentText(pTaxiway->GetMarking().c_str());
	drawobj.end();
	//UpdateIntersectionNodes(pTaxiway, m_pAirport3D->m_vAirportNodes);
}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderTaxiway3D, Taxiway);

void CRenderTaxiway3D::Update3D( ALTObjectDisplayProp* pDispObj)
{

}

void CRenderTaxiway3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* pObj = _GetOrCreateManualObject(GetName());
	Build((Taxiway*)pBaseObj,pObj);
	AddObject(pObj);
}

void CRenderTaxiway3D::RefreshHoldPosition3D(Taxiway* pTaxiway)
{
	/*m_vAllHoldPositions3D.clear();
	for(int i =0 ;i<(int)pTaxiway->m_vAllHoldPositions.size();i++)
	{
		HoldPosition& hold = pTaxiway->m_vAllHoldPositions[i];

		if( hold.GetIntersectNode().GetRunwayIntersectItemList().size() )
		{
			CRenderHoldPosition3D::SharedPtr pHoldPos(new CRenderTaxiwayHoldPosition3D(CreateNewChild().GetSceneNode(), m_p3DScene, pTaxiway, i));
			pHoldPos->Update();
			m_vAllHoldPositions3D.push_back(pHoldPos);
		}		
	}*/

}

void CRenderTaxiway3D::UpdateIntersectionNodes( Taxiway* pTaxiway, const IntersectionNodesInAirport& nodelist )
{
	/*m_vIntersectionNodes.clear();
	for(int i=0;i< nodelist.GetCount();i++)
	{
		IntersectionNode theNode = nodelist.NodeAt(i);
		if(theNode.HasObject( pTaxiway->getID() ) )
		{
			m_vIntersectionNodes.push_back( nodelist.NodeAt(i) );
		}
	}*/

	pTaxiway->InitHoldPositions(nodelist);
	RefreshHoldPosition3D(pTaxiway);
}


void CRenderTaxiway3D::UpdateHoldPositions( Taxiway* pTaxiway, ALTAirportLayout& airport, const FilletTaxiwaysInAirport& vFillets, const IntersectionNodesInAirport& nodelist)
{
	std::vector<Runway*> vRunways;
	airport.QueryDataObjectList<Runway>(vRunways);

	pTaxiway->UpdateHoldPositions(airport, vRunways, vFillets, nodelist);

	RefreshHoldPosition3D(pTaxiway);
}


void CRenderTaxiwayHoldPosition3D::UpdateLine()
{
	GetTaxiway()->GetHoldPositionLine(m_nid,m_line);
}

HoldPosition & CRenderTaxiwayHoldPosition3D::GetHoldPosition()
{
	return GetTaxiway()->m_vAllHoldPositions[m_nid];
}


struct FilletPtLess
{
	FilletPtLess(int taxiID){ m_nTaxiID = taxiID; }
	bool operator()(CRenderTaxiway3D::FilletPtRef& f1, CRenderTaxiway3D::FilletPtRef& f2)const{
		const IntersectionNode& node1 = f1.fillet->GetIntersectNode();
		const IntersectionNode& node2 = f2.fillet->GetIntersectNode();
		if (node1.GetID() == node2.GetID())
			return f1.GetDistInObj() < f2.GetDistInObj();
		return node1.GetDistance(m_nTaxiID) < node2.GetDistance(m_nTaxiID);
	}
	int m_nTaxiID;
};

void CRenderTaxiway3D::SetRelatFillets( const FilletTaxiwaysInAirport& vFillets, Taxiway* pTaxiway )
{
	m_vFilletPoints.clear();
	for(int i =0 ;i< vFillets.GetCount(); i++ )
	{
		const FilletTaxiway& fillet = vFillets.ItemAt(i);
		if( fillet.GetObject1ID() == pTaxiway->getID() )
		{
			m_vFilletPoints.push_back( FilletPtRef(fillet,true) );		
		}

		if( fillet.GetObject2ID() == pTaxiway->getID() )
		{
			m_vFilletPoints.push_back(  FilletPtRef(fillet,false) );
		}
	}

	std::sort(m_vFilletPoints.begin(), m_vFilletPoints.end(),FilletPtLess(pTaxiway->getID()));

}

double CRenderTaxiway3D::FilletPtRef::GetTaxiwayDist( int taxiwayId ) const
{
	return fillet->GetIntersectNode().GetDistance(taxiwayId);
}

double CRenderTaxiway3D::FilletPtRef::GetDistInObj() const
{
	if(fistPt)
	{
		return fillet->GetFilletPt1Dist();
	}
	else
	{
		return fillet->GetFilletPt2Dist();
	}
}

