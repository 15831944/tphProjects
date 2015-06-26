#include "StdAfx.h"
#include ".\landsideresourceinsim.h"
#include <algorithm>
#include "LandsideVehicleInSim.h"
#include "Common/ARCRay.h"
#include "Common/Pollygon2008.h"
#include "common/ARCPipe.h"
#include "Common/Line2008.h"
#include <common/ARCMathCommon.h>

bool LandsideResourceInSim::bHasVehicle( LandsideVehicleInSim* pVechile ) const
{
	return m_vInResVehicle.bHas(pVechile);
}

void LandsideResourceInSim::AddInResVehicle( LandsideVehicleInSim* pVehicle )
{
	m_vInResVehicle.add(pVehicle);
}

void LandsideResourceInSim::RemoveInResVehicle( LandsideVehicleInSim* pVehicle )
{
	m_vInResVehicle.remove(pVehicle);	
}

LandsideVehicleInSim* LandsideResourceInSim::GetInResVehicle( int idx ) const
{
	return m_vInResVehicle[idx];
}

int LandsideResourceInSim::GetVehicleIndex( LandsideVehicleInSim* pVehicle ) const
{
	std::vector<LandsideVehicleInSim*>::const_iterator itrFind  = std::find(m_vInResVehicle.begin(),m_vInResVehicle.end(),pVehicle);
	return itrFind - m_vInResVehicle.begin();
}

void LandsideResourceInSim::RemoveInResVehicles( LandsideVehicleList& vVehicles )
{
	for(size_t i=0;i<vVehicles.size();i++)
		RemoveInResVehicle(vVehicles[i]);
}

void LandsideResourceInSim::OnVehicleEnter( LandsideVehicleInSim* pVehicle, DistanceUnit dist, const ElapsedTime& t )
{
	AddInResVehicle(pVehicle);
}

void LandsideResourceInSim::OnVehicleExit( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	RemoveInResVehicle(pVehicle);
}

CLandsideWalkTrafficObjectInSim* LandsideResourceInSim::getBestTrafficObjectInSim( const CPoint2008& pt,LandsideResourceManager* allRes ) 
{	
	return NULL;
}	

void LandsideResourceInSim::GetInsidePathOfPolygon( const CPath2008& path, const CPollygon2008& polygon,std::vector<CPath2008>& resultPathList ) const
{
	int nCount = path.getCount() - 1;
	std::vector<CPoint2008> vPointList;
	for (int i = 0; i < nCount; i++)
	{
		const CPoint2008& p1 = path.getPoint(i);
		const CPoint2008& p2 = path.getPoint(i+1);
		CLine2008 testLine;
		testLine.init (p1, p2);
		if (polygon.contains2(p1) && polygon.contains2(p2))//p1 p2 inside
		{
			vPointList.push_back(p1);
		}
		else if(polygon.contains2(p1)) //p1 inside, p2 outside
		{
			for (int j = 0; j < polygon.getCount(); j++)
			{
				const CPoint2008& pt1 = polygon.getPoint(j);
				const CPoint2008& pt2 = polygon.getPoint((j+1)%polygon.getCount());
				CLine2008 polySegment;
				polySegment.init (pt1, pt2);
				if (testLine.intersects(polySegment))
				{
					CPoint2008 intersect = testLine.intersection (polySegment);
					vPointList.push_back(p1);
					vPointList.push_back(intersect);

					CPath2008 pathInside;
					pathInside.init((int)vPointList.size(),&vPointList[0]);
					vPointList.clear();
					resultPathList.push_back(pathInside);
					break;
				}

			}
		}
		else if (polygon.contains2(p2))//p1 outside, p2 inside
		{
			for (int j = 0; j < polygon.getCount(); j++)
			{
				const CPoint2008& pt1 = polygon.getPoint(j);
				const CPoint2008& pt2 = polygon.getPoint((j+1)%polygon.getCount());
				CLine2008 polySegment;
				polySegment.init (pt1, pt2);
				if (testLine.intersects(polySegment))
				{
					CPoint2008 intersect = testLine.intersection (polySegment);
					vPointList.push_back(intersect);
					vPointList.push_back(p2);
					break;
				}

			}
		}
		else //p1 and p2 outside but intersect with polygon
		{
			for (int j = 0; j < polygon.getCount(); j++)
			{
				const CPoint2008& pt1 = polygon.getPoint(j);
				const CPoint2008& pt2 = polygon.getPoint((j+1)%polygon.getCount());
				CLine2008 polySegment;
				polySegment.init (pt1, pt2);
				if (testLine.intersects(polySegment))
				{
					CPoint2008 intersect = testLine.intersection (polySegment);
					vPointList.push_back(intersect);
				}

				if (vPointList.size() == 2)
				{
					CPath2008 pathInside;
					pathInside.init(2,&vPointList[0]);
					vPointList.clear();
					resultPathList.push_back(pathInside);
					break;
				}
			}
		}
	}

	if (!vPointList.empty())
	{
		vPointList.push_back(path.getPoint(nCount));
		CPath2008 pathInside;
		pathInside.init((int)vPointList.size(),&vPointList[0]);
		vPointList.clear();
		resultPathList.push_back(pathInside);
	}
}

StretchSpeedControlData* LandsideResourceInSim::GetSpeedControl( double dist ) const
{
	if(LandsideLayoutObjectInSim* pobj = getLayoutObject() ){
		return pobj->GetSpeedControlData();
	}
	return NULL;		 
}

CPoint2008 LandsideResourceInSim::GetPaxWaitPos() const
{
	ASSERT(FALSE); 
	return CPoint2008(0,0,0);
}

void LandsideResourceInSim::PassengerMoveInto( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime )
{
	//ASSERT(0);
}

void LandsideResourceInSim::PassengerMoveOut( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime )
{
	//ASSERT(0);
}

LandsideVehicleInSim* LandsideResourceInSim::GetAheadVehicleByDistInRes( LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist )
{
	LandsideVehicleInSim* pRet = NULL;
	DistanceUnit minDist=0; 

	for(int i=0;i<GetInResVehicleCount();i++)
	{
		LandsideVehicleInSim* pVeh = GetInResVehicle(i);
		if(pVeh!=mpVehicle)
		{
			if(pVeh->getLastState().getLandsideRes() == this )
			{
				double otherDist = pVeh->getLastState().distInRes;
				if(otherDist > distInLane )
				{
					if(!pRet  || otherDist < minDist){
						minDist = otherDist;
						pRet = pVeh;
					}
				}

			}
		}
	}
	aheadDist = minDist;
	return pRet;
}

CString LandsideResourceInSim::print() const
{
	if(LandsideLayoutObjectInSim* play = getLayoutObject() )
	{
		return play->getName().GetIDString();
	}
	return _T("Unknown");
}

