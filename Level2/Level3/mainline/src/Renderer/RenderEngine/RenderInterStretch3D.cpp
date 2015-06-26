#include "StdAfx.h"
#include ".\renderinterstretch3d.h"

#include <Common/ShapeGenerator.h>

#include <boost/bind.hpp>

#include "ShapeBuilder.h"
#include "SelectionManager.h"
#include "Render3DScene.h"

using namespace Ogre;


CRenderInterStretch3DPoint::CRenderInterStretch3DPoint( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, IntersectedStretch& point, int mid )
	: C3DSceneNodeObject(pNode, p3DScene)
	, m_stretch3D(point)
	, m_index(mid)
{

}

IntersectedStretchPoint& CRenderInterStretch3DPoint::GetIntersectedStretchPt()
{
	return m_index==0 ? m_stretch3D.GetFristIntersectedStretchPoint() : m_stretch3D.GetSecondIntersectedStretchPoint();
}

IntersectionNode& CRenderInterStretch3DPoint::GetIntersectionNode()
{
	return m_stretch3D.GetIntersectionNode();
}

DistanceUnit CRenderInterStretch3DPoint::GetDistInObj() const
{
	return m_index==0 ? m_stretch3D.GetFristPointDisInPath() : m_stretch3D.GetSecondPointDisInPath();
}

void CRenderInterStretch3DPoint::Update()
{

}

ARCPoint3 CRenderInterStretch3DPoint::GetLocation() const
{
	return m_index == 0 ? m_stretch3D.GetFristIntersectedStretchPointPos() : m_stretch3D.GetSecondIntersectedStretchPointPos();
}


CRenderInterStretch3D::CRenderInterStretch3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, IntersectedStretch& interStretch )
	: C3DSceneNodeObject(pNode, p3DScene)
	, m_interStretch(interStretch)
{
	m_fristPoint = CRenderInterStretch3DPoint::SharedPtr(
		new CRenderInterStretch3DPoint(CreateNewChild().GetSceneNode(), m_p3DScene, interStretch, 0)
		);
	m_secondPoint = CRenderInterStretch3DPoint::SharedPtr(
		new CRenderInterStretch3DPoint(CreateNewChild().GetSceneNode(), m_p3DScene, interStretch, 1)
		);

	GenSmoothStretchPath();
}

ARCPoint3 CRenderInterStretch3D::GetLocation() const
{
	return (m_fristPoint->GetLocation() + m_secondPoint->GetLocation()/* + interStretch.GetIntersectNode().GetPosition()*/) /3.0;
}

void CRenderInterStretch3D::GenSmoothStretchPath()
{
	std::vector<CPoint2008> vCtrlPoints;
	std::vector<ARCVector3> vInputPoints;
	//m_vPath.clear();

	if (m_interStretch.GetControlPoint(vCtrlPoints) && vCtrlPoints.size()>=2)
	{
		int elementCount = static_cast<int>(vCtrlPoints.size());
		ARCVector3 vBegin = vCtrlPoints[1]-vCtrlPoints[0];
		ARCVector3 vEnd = vCtrlPoints[elementCount-2]-vCtrlPoints[elementCount-1];
		DistanceUnit disTance = vCtrlPoints[0].distance3D(vCtrlPoints[elementCount-1]);
		vBegin.SetLength(disTance*0.5);
		vEnd.SetLength(disTance*0.5);
		vInputPoints.reserve(4);
		vInputPoints.push_back(vCtrlPoints[0]);
		vInputPoints.push_back(vCtrlPoints[0] + vBegin);
		vInputPoints.push_back(vCtrlPoints[elementCount-1] +vEnd);
		vInputPoints.push_back(vCtrlPoints[elementCount-1]);

		m_vPath.clear();
		SHAPEGEN.GenBezierPath(vInputPoints,m_vPath,10);

		ARCVector3 m_fristExt = m_interStretch.GetExtentFristPoint();
		ARCVector3 m_secondExt = m_interStretch.GetExtentSecondPoint();

		m_vPath.insert(m_vPath.begin(),m_fristExt);
		m_vPath.push_back(m_secondExt);
	}
}

void CRenderInterStretch3D::Update()
{
	if(m_vPath.size()<2)
		return;

	SceneManager* pScene = GetScene();
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(mpNode->getName().c_str(),pScene);

	ColourValue clr(1.0, 1.0, 1.0, 1.0);
	ProcessorBuilder::DrawTexturePath(pObj, _T("Airside/road"), clr, m_vPath,
		m_interStretch.GetFristWidth(), m_interStretch.GetSecondWidth(), 0.0);

	pObj->setVisibilityFlags(NormalVisibleFlag);
// 	REG_SELECT(pObj);
	OgreUtil::AttachMovableObject(pObj,mpNode);


}

CRenderInterStretch3DInAirport::CRenderInterStretch3DInAirport( CRender3DScene* p3DScene )
	: m_p3DScene(p3DScene)
{

}
void CRenderInterStretch3DInAirport::Init( int nAirport,const IntersectionNodesInAirport& nodeList )
{
// 	m_bInEdit = false;
// 	m_iInEdit = -1;
	m_nAirport = nAirport;
	m_intersectedStretchInAirport.ReadData(nAirport,nodeList);
	m_vIntersectedStretch3D.clear();
	for(int i=0; i<m_intersectedStretchInAirport.GetCount();i++)
	{
		m_vIntersectedStretch3D.push_back(
			CRenderInterStretch3D::SharedPtr(new CRenderInterStretch3D(m_p3DScene->GetRoot().CreateNewChild().GetSceneNode(), m_p3DScene, m_intersectedStretchInAirport.ItemAt(i)))
			);
	}
// 	FetchPositionOfLane();
}

// void CRenderInterStretch3DInAirport::FetchPositionOfLane()
// {
// 	m_vLane.first.clear();
// 	m_vLane.second.clear();
// 	BOOL bSelectFirst, bSelectSecond;
// 	if (m_intersectedStretchInAirport.GetCount()<1)
// 		return;
// 
// 	if (m_vLane.first.size()<1)
// 	{
// 		int id = m_intersectedStretchInAirport.ItemAt(0).GetFristIntersectedStretchPoint().GetIntersectItemID();
// 		IntersectedStretchPoint pInterstertch = m_intersectedStretchInAirport.ItemAt(0).GetFristIntersectedStretchPoint();
// 		m_vLane.first.push_back(id);
// 		m_vLane.second.push_back(pInterstertch);
// 	}
// 	for (int i=0; i<(int)m_intersectedStretchInAirport.GetCount();i++)
// 	{
// 		IntersectedStretch& pIntersected = m_intersectedStretchInAirport.ItemAt(i);
// 		int pointId1 = pIntersected.GetFristIntersectedStretchPoint().GetIntersectItemID();
// 		int pointId2 = pIntersected.GetSecondIntersectedStretchPoint().GetIntersectItemID();
// 		//IntersectItem * pItem1 = pIntersected.GetIntersectionNode().GetItemByID(pointId1);
// 		//IntersectItem * pItem2 = pIntersected.GetIntersectionNode().GetItemByID(pointId2);
// 		DistanceUnit dis1 = pIntersected.GetFristIntersectedStretchPoint().GetDistToIntersect()/*+pIntersected.GetFristIntersectedStretchPoint().GetDistToUser()*/;
// 		DistanceUnit dis2 = pIntersected.GetSecondIntersectedStretchPoint().GetDistToIntersect()/*+pIntersected.GetSecondIntersectedStretchPoint().GetDistToUser()*/;
// 		bSelectFirst = TRUE;
// 		for (int j=0; j<(int)m_vLane.first.size(); j++)
// 		{
// 			DistanceUnit dis  = m_vLane.second[j].GetDistToIntersect()/* + m_vLane.second[j].GetDistToUser()*/;
// 			if (m_vLane.first[j]==pointId1)
// 			{
// 				if ((dis1<0&&dis<0)||(dis1>0&&dis>0))
// 				{
// 					if (fabs(dis1)>fabs(dis))
// 					{
// 						m_vLane.second[j] = (pIntersected.GetFristIntersectedStretchPoint());
// 					}
// 					bSelectFirst = FALSE;
// 					break;
// 				}	
// 
// 			}
// 		}
// 
// 		if(bSelectFirst == TRUE)
// 		{
// 			m_vLane.first.push_back(pointId1);
// 			m_vLane.second.push_back((pIntersected.GetFristIntersectedStretchPoint()));
// 		}
// 
// 		bSelectSecond = TRUE;
// 		for (j=0; j<(int)m_vLane.first.size(); j++)
// 		{
// 			DistanceUnit dis  = m_vLane.second[j].GetDistToIntersect() /*+ m_vLane.second[j].GetDistToUser()*/;
// 			if (m_vLane.first[j]==pointId2)
// 			{
// 				if ((dis2<0&&dis<0)||(dis2>0&&dis>0))
// 				{
// 					if (fabs(dis2)>fabs(dis))
// 						m_vLane.second[j] = (pIntersected.GetSecondIntersectedStretchPoint());
// 					bSelectSecond = FALSE;
// 					break;
// 				}
// 			}
// 		}
// 
// 		if(bSelectSecond == TRUE)
// 		{
// 			m_vLane.first.push_back(pointId2);
// 			m_vLane.second.push_back((pIntersected.GetSecondIntersectedStretchPoint()));
// 		}
// 	}
// }

void CRenderInterStretch3DInAirport::Update()
{
	std::for_each(m_vIntersectedStretch3D.begin(), m_vIntersectedStretch3D.end(), boost::BOOST_BIND(&CRenderInterStretch3D::Update, _1));

}