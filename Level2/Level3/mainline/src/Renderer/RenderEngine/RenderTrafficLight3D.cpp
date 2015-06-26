#include "StdAfx.h"
#include ".\rendertrafficlight3d.h"
#include "InputAirside/IntersectionNode.h"
//#include "RenderAirport3D.h"
#include "Render3DScene.h"
#include "Common/ARCStringConvert.h"
#include "Common/DistanceLineLine.h"
#include "InputAirside/Stretch.h"

using namespace Ogre;

void CRenderTrafficLight3D::CalculateRotation(TrafficLight* pTrafficLight)
{
	int nIntersectID = pTrafficLight->GetIntersecID();
	CPath2008 m_path = pTrafficLight->GetPath();
	IntersectionNode intersection;
	intersection.ReadData(nIntersectID);
	std::vector<StretchIntersectItem*> vIntersectItem = intersection.GetStretchIntersectItem();
	m_vDegree.clear();
	StretchIntersectItem  stretchItem;
	for (int i=0; i<m_path.getCount(); i++)
	{
		CPoint2008 position = m_path.getPoint(i);
		DistanceUnit minDistance = ARCMath::DISTANCE_INFINITE;
		// Get the Stretch information
		for (int j=0; j<(int)vIntersectItem.size(); j++)
		{
			DistancePointPath3D  pDistance;
			DistanceUnit disTance = pDistance.GetSquared(position, vIntersectItem[j]->GetStretch()->GetPath());
			if (disTance<minDistance)
			{
				stretchItem = *vIntersectItem[j];
				minDistance = disTance;
			}			
		}
		//Get the Rotation of one light
		if(stretchItem.GetUID()==-1)
			return;
		double realDist = stretchItem.GetDistInItem();
		CPath2008 m_temp = stretchItem.GetStretch()->GetPath();

		if (m_temp.getCount()<2)
		{
			return;
		}

		double accdist = 0;
		double thisdist = 0;
		for (int k=0; k<m_temp.getCount(); k++)
		{
			if (accdist >= realDist)
			{
				break;
			}
			if (k+1>=m_temp.getCount()) return;
			thisdist = m_temp.getPoint(k).distance3D(m_temp.getPoint(k+1));
			accdist += thisdist;
		}
		double m_degree = 0.0;
		if (k-1>=0)
		{
			CPoint2008 pos_head = m_temp.getPoint(k-1);
			CPoint2008 pos_trail = m_temp.getPoint(k);
			CPoint2008 pos_inter = m_temp.GetDistPoint(realDist);
			CLine2008 line(pos_head,pos_trail);
			CPoint2008 pos_line = line.getProjectPoint(position); 

			if (pos_inter.distance3D(pos_head)>pos_line.distance3D(pos_head))
			{
				ARCVector3 vHeadDir = ((ARCVector3)pos_head - (ARCVector3)pos_inter).Normalize();
				ARCVector3 vObjDir(0.0,1.0,0.0);
				double Theta = ARCMath::RadiansToDegrees(acos(vObjDir.dot(vHeadDir)));
				ARCVector3 vNormalDir = cross(vObjDir,vHeadDir).Normalize();
				if (vNormalDir[VZ]>0)
					m_degree = 180+Theta;
				else
					m_degree = 180-Theta;
			}
			else
			{
				ARCVector3 vTrailDir = ((ARCVector3)pos_trail-(ARCVector3)pos_inter).Normalize();
				ARCVector3 vObjDir(0.0,1.0,0.0);
				double Theta = ARCMath::RadiansToDegrees(acos(vObjDir.dot(vTrailDir)));
				ARCVector3 vNormalDir = cross(vObjDir,vTrailDir).Normalize();
				if (vNormalDir[VZ]>0)
					m_degree = 180+Theta;
				else
					m_degree = 180- Theta;
			}
		}
		else
		{
			CPoint2008 pos_head = m_temp.getPoint(k);
			CPoint2008 pos_trail = m_temp.getPoint(k+1);
			CPoint2008 pos_inter = m_temp.GetDistPoint(realDist);
			CLine2008 line(pos_head,pos_trail);
			CPoint2008 pos_line = line.getProjectPoint(position); 

			if(pos_inter.distance3D(pos_trail)>pos_line.distance3D(pos_trail))
			{
				ARCVector3 vTrailDir = ((ARCVector3)pos_trail-(ARCVector3)pos_inter).Normalize();
				ARCVector3 vObjDir(0.0,1.0,0.0);
				double Theta = ARCMath::RadiansToDegrees(acos(vObjDir.dot(vTrailDir)));
				ARCVector3 vNormalDir = cross(vObjDir,vTrailDir).Normalize();
				if (vNormalDir[VZ]>0)
					m_degree = 180+Theta;
				else
					m_degree = 180- Theta;
			}
			else
				return;
		}

		m_vDegree.push_back(m_degree);

	}

}

C3DSceneNode CRenderTrafficLight3D::AddOneTrafficLightModel(int idx )
{
	CString sName = GetName() + ARCStringConvert::toString(idx);
	C3DSceneNode shape3D = GetCreateChildNode( sName );
	
	Entity * pEnt = shape3D._GetOrCreateEntity(sName,_T("Trafficlight.mesh"));
	if(pEnt)
		shape3D.AddObject(pEnt);
	return shape3D;
}


void CRenderTrafficLight3D::Build(TrafficLight* pTrafficLight, ManualObject* pObj)
{
	RemoveAllChildNodes();

	CalculateRotation(pTrafficLight);
	const CPath2008& path = pTrafficLight->GetPath();
	int nPathCount = path.getCount();
	for(int i=0;i<nPathCount;i++){
		C3DSceneNode shape3D = AddOneTrafficLightModel(i);
		ARCVector3 position = path.getPoint(i);
		shape3D.SetPosition(position);
		shape3D.SetRotation(ARCVector3(0.0, 0.0, (int)m_vDegree.size()>i ? m_vDegree[i] : 0.0));
	}

}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderTrafficLight3D, TrafficLight);

void CRenderTrafficLight3D::Update3D( ALTObjectDisplayProp* pDispObj )
{

}

void CRenderTrafficLight3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* pObj = _GetOrCreateManualObject(GetName());
	Build((TrafficLight*)pBaseObj,pObj);
	AddObject(pObj);
}
