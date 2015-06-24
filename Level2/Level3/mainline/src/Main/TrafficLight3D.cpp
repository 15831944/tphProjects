#include "StdAfx.h"
#include "../InputAirside/TrafficLight.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include ".\TrafficLight3D.h"
#include "../Common/ARCPipe.h"
#include ".\3DView.h"
#include "Floors.h"
#include "TermPlanDoc.h"
#include "../InputAirside/Stretch.h"
#include "../Common/DistanceLineLine.h"
#include "../InputAirside/IntersectionNode.h"

CTrafficLight3D::CTrafficLight3D(int nID):ALTObject3D(nID)
{
	m_pObj = new TrafficLight();
	//m_pDisplayProp = new TrafficLightDisplayProp();
	m_pModel = NULL;
	m_pModel2D = NULL;

}

CTrafficLight3D::~CTrafficLight3D(void)
{
}

void CTrafficLight3D::CalculateRotation()
{
	int nIntersectID = GetTrafficLight()->GetIntersecID();
	CPath2008 m_path = GetTrafficLight()->GetPath();
	IntersectionNode intersection;
	intersection.ReadData(nIntersectID);
	std::vector<StretchIntersectItem*> vIntersectItem = intersection.GetStretchIntersectItem();
	m_vDegree.clear();
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
		int k = 0;
		for (; k<m_temp.getCount(); k++)
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

void CTrafficLight3D::DrawOGL( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}
	CalculateRotation();
    glDisable(GL_POLYGON_OFFSET_FILL);
	const CPath2008& path = GetTrafficLight()->GetPath();
	if(path.getCount()<1)return;
	for (int i=0; i<path.getCount(); i++)
	{
		ARCVector3 position = path.getPoint(i);
		CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_AirSide);
		position[VZ] = floors.getVisibleAltitude( position[VZ] );
 
		glPushMatrix();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glTranslated(position[VX],position[VY],position[VZ]);
		if (m_vDegree.size()>0)
		{
			glRotated(m_vDegree[i], 0.0, 0.0 ,1.0);
		}

//		glRotated(GetTrafficLight()->GetRotation(), 0.0, 0.0, 1.0);
		//glScaled(m_pProc2->GetScale()[VX], m_pProc2->GetScale()[VY], m_pProc2->GetScale()[VZ]);

		if(!m_pModel)
			m_pModel =  pView->getModelRes().getModel(C3DModelResource::TrafficlightModel);
		if(!m_pModel2D)
			m_pModel2D = pView->getModelRes().getModel(C3DModelResource::Trafficlight2DModel);

		if(pView->GetCamera()->GetProjectionType() == C3DCamera::perspective )
		{	
			if(m_pModel)
				m_pModel->Render(pView);
		}
		else 
		{
			if(m_pModel2D)
				m_pModel2D->Render(pView);
		}

		glDisable(GL_CULL_FACE);
		glPopMatrix();
	}
}


ARCPoint3 CTrafficLight3D::GetLocation( void ) const
{
	TrafficLight * pTrafficLight = (TrafficLight*)m_pObj.get();
	const CPath2008& path = pTrafficLight->GetPath();
	if(path.getCount() < 1)return (ARCPoint3(0,0,0));
	return path.getPoint(0);
}

ARCPoint3 CTrafficLight3D::GetSubLocation(int index)const
{
	TrafficLight * pTrafficLight = (TrafficLight *)m_pObj.get();
	const CPath2008& path = pTrafficLight->GetPath();
	if (path.getCount()<1 || path.getCount()<index)
	{
		return ARCPoint3(0,0,0);
	}
	return path.getPoint(index);
}

TrafficLight * CTrafficLight3D::GetTrafficLight()
{
	return (TrafficLight*) GetObject();
}


void CTrafficLight3D::DrawSelect( C3DView * pView )
{
	//glLoadName( pView->GetSelectionMap().NewSelectable( this));
	DrawOGL(pView);
}

void CTrafficLight3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	CPath2008& path = const_cast<CPath2008&>(GetTrafficLight()->GetPath());
	path.DoOffset(dx,dy,dz);
}

void CTrafficLight3D::RenderEditPoint( C3DView * pView )
{
	glDisable(GL_TEXTURE_2D);
	if(!IsInEdit()) return;
	if(IsNeedSync()){
		DoSync();
	}	
	for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++){
		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}
}

void CTrafficLight3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetTrafficLight()->GetPath().getCount();

	CPath2008& path = const_cast<CPath2008&>(GetTrafficLight()->GetPath());
	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,path,i) );
	}

	ALTObject3D::DoSync();	
}

void CTrafficLight3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	CPath2008& path = const_cast<CPath2008&>(GetTrafficLight()->GetPath());
	path.DoOffset(-center[VX],-center[VY],-center[VZ]);
	path.Rotate(dx);
	path.DoOffset(center[VX],center[VY],center[VZ]);
}