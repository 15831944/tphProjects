#include "StdAfx.h"
#include ".\renderfillet3d.h"

#include <Common/ShapeGenerator.h>

#include <InputAirside/ALTObjectDisplayProp.h>

#include <boost/bind.hpp>

#include "OgreConvert.h"
#include "ShapeBuilder.h"
#include "SelectionManager.h"
#include "Render3DScene.h"
#include "CustomizedRenderQueue.h"
#include "RenderTaxiway3D.h"

using namespace Ogre;


CRenderFilletPoint3D::CRenderFilletPoint3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, FilletTaxiway& filletTaxiway, int idx )
	: C3DSceneNodeObject(pNode, p3DScene)
	, m_filletTx(filletTaxiway)
	, m_idx(idx)
{

}
ARCPoint3 CRenderFilletPoint3D::GetLocation() const
{
	return m_idx == 0 ? m_filletTx.GetFilletPoint1Pos() : m_filletTx.GetFilletPoint2Pos();
}

FilletPoint& CRenderFilletPoint3D::GetFilletPt()
{
	return m_idx == 0 ? m_filletTx.GetFilletPoint1() : m_filletTx.GetFilletPoint2();
}

DistanceUnit CRenderFilletPoint3D::GetDistInObj() const
{
	return m_idx == 0 ? m_filletTx.GetFilletPt1Dist() : m_filletTx.GetFilletPt2Dist();
}

IntersectionNode& CRenderFilletPoint3D::GetIntersectionNode()
{
	return m_filletTx.GetIntersectNode();
}

void CRenderFilletPoint3D::Update()
{

}

CRenderTaxiwayFillet3D::CRenderTaxiwayFillet3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, FilletTaxiway& filetTaxiway )
	: C3DSceneNodeObject(pNode, p3DScene)
	, m_filetTaxway(filetTaxiway)
{
	m_filetPt1 = CRenderFilletPoint3D::SharedPtr(new CRenderFilletPoint3D(CreateNewChild().GetSceneNode(), m_p3DScene, m_filetTaxway,0));
	m_filetPt2 = CRenderFilletPoint3D::SharedPtr(new CRenderFilletPoint3D(CreateNewChild().GetSceneNode(), m_p3DScene, m_filetTaxway,1));
// 	m_bInEditMode = false;
	GenSmoothPath();

}

void CRenderTaxiwayFillet3D::GenSmoothPath()
{
	std::vector<CPoint2008> vCtrlPts;
	m_filetTaxway.GetControlPoints(vCtrlPts);	


	const int nCtrlCnt = (int)vCtrlPts.size();
	if(nCtrlCnt < 2 )
		return ;

	m_vPath.clear();
	if(nCtrlCnt==2)
	{
		m_vPath.push_back(vCtrlPts[0]);
		m_vPath.push_back(vCtrlPts[1]);
	}
	else
	{
		std::vector<ARCVector3> _inPts;	
		_inPts.reserve(4);
		_inPts.push_back(vCtrlPts[0]);

		DistanceUnit dist = vCtrlPts[0].distance3D( vCtrlPts[nCtrlCnt-1] );
		ARCVector3 vBegin = vCtrlPts[1] - vCtrlPts[0];  
		if(vBegin.Length()>ARCMath::EPSILON)
		{
			vBegin.SetLength(dist*.5);
			_inPts.push_back(vCtrlPts[0] + vBegin);
		}
		ARCVector3 vEnd = vCtrlPts[nCtrlCnt-2] - vCtrlPts[nCtrlCnt-1];
		if(vEnd.Length()>ARCMath::EPSILON)
		{
			vEnd.SetLength(dist*.5);
			_inPts.push_back(vCtrlPts[nCtrlCnt-1] + vEnd);
		}
		_inPts.push_back(vCtrlPts[nCtrlCnt-1] );

		if(_inPts.size()>2)
		{
			SHAPEGEN.GenBezierPath(_inPts,m_vPath,10);
		}
		else
		{
			m_vPath.push_back(vCtrlPts[0]);
			m_vPath.push_back(vCtrlPts[nCtrlCnt-1]);
		}
		//ARCVector3 fromExt = m_filetTaxway.GetFilletPoint1Ext();
		//ARCVector3 toExt = m_filetTaxway.GetFilletPoint2Ext();
		//m_vPath.insert(m_vPath.begin(),fromExt);
		//m_vPath.push_back(toExt);
	}

}

ARCPoint3 CRenderTaxiwayFillet3D::GetLocation() const
{
	return ARCPoint3(m_filetPt1->GetLocation() + m_filetPt2->GetLocation() + m_filetTaxway.GetIntersectNode().GetPosition()) /3.0;
}

void CRenderTaxiwayFillet3D::Update() 
{
// 	GenSmoothPath();

	if(m_vPath.size()<2)
		return;

	ColourValue clr(1.0, 1.0, 1.0, 0.0);

	SceneManager* pScene = GetScene();
	const CString str = mpNode->getName().c_str();
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(str,pScene);
	pObj->setRenderQueueGroup(RenderObject_Surface);
	//pObj->setRenderQueueGroup(RenderObject_Mark);

	TaxiwayDisplayProp dispProp;
	dispProp.ReadData(m_filetTaxway.GetID());
	TaxiwayDisplayProp* pDispProp = &dispProp;

	if(mpNode)
	{
		if(!mpNode->isInSceneGraph())
		{
			ASSERT(false);
		}
	}
	ProcessorBuilder::DrawTexturePath(pObj, _T("Airside/Taxiway"), clr, m_vPath, m_filetTaxway.GetWidth()*1.05, 0.0);

	pObj->setVisibilityFlags(NormalVisibleFlag);
// 	REG_SELECT(pObj);
	OgreUtil::AttachMovableObject(pObj,mpNode);


	ManualObject* pLineObj = OgreUtil::createOrRetrieveManualObject(GetIDName() + _T("-CenterLine"), GetScene());
	pLineObj->setRenderQueueGroup(RenderObject_Mark);
	ProcessorBuilder::DrawColorPath(pLineObj, OgreConvert::GetColor(pDispProp->m_dpCenterLine.cColor),
		Ogre::OgreUtil::ClrMat_TaxiwayCenterLine, m_vPath, 50, false, CRenderTaxiway3D::m_dCenterlineZOffset);
	pLineObj->setVisibilityFlags(NormalVisibleFlag);
// 	REG_SELECT(pLineObj);
	OgreUtil::AttachMovableObject(pLineObj,mpNode);

}

void CRenderFilletTaxiway3DInAirport::Init( int nARPID, const IntersectionNodesInAirport& nodeList )
{
	m_nARPID = nARPID;
	m_vFilletTaxiways.ReadData(nARPID,nodeList);
// 	m_bInEdit = false;
// 	m_iInEditFillet = -1;

	m_vFillTaxiway3D.clear();
	for(int i=0;i<m_vFilletTaxiways.GetCount();i++)
	{
		if( m_vFilletTaxiways.ItemAt(i).IsActive())
		{			
			m_vFillTaxiway3D.push_back(
				CRenderTaxiwayFillet3D::SharedPtr(new CRenderTaxiwayFillet3D(m_p3DScene->GetRoot().CreateNewChild().GetSceneNode(), m_p3DScene, m_vFilletTaxiways.ItemAt(i)))
				);
		}
	}

}

void CRenderFilletTaxiway3DInAirport::UpdateFillets( const IntersectionNodesInAirport& nodelist,const std::vector<int>& vChangeNodeIndexs )
{
	m_vFillTaxiway3D.clear();

	m_vFilletTaxiways.UpdateFillets( m_nARPID, nodelist, vChangeNodeIndexs);

	for(int i=0;i<m_vFilletTaxiways.GetCount();i++)
	{
		if( m_vFilletTaxiways.ItemAt(i).IsActive())
		{			
			m_vFillTaxiway3D.push_back(
				CRenderTaxiwayFillet3D::SharedPtr(new CRenderTaxiwayFillet3D(m_p3DScene->GetRoot().CreateNewChild().GetSceneNode(), m_p3DScene, m_vFilletTaxiways.ItemAt(i)))
				);
		}
	}

}

CRenderTaxiwayFillet3D* CRenderFilletTaxiway3DInAirport::FindFillet3D( const FilletTaxiway& filet )
{
	for(int i=0;i< (int)m_vFillTaxiway3D.size();i++)
	{
		CRenderTaxiwayFillet3D * pfillet3D = m_vFillTaxiway3D.at(i).get();
		if(pfillet3D->m_filetTaxway.IsOverLapWith(filet))
		{
			return pfillet3D;
		}
	}
	return NULL;
}

void CRenderFilletTaxiway3DInAirport::Update()
{
	std::for_each(m_vFillTaxiway3D.begin(), m_vFillTaxiway3D.end(), boost::BOOST_BIND(&CRenderTaxiwayFillet3D::Update, _1));
}