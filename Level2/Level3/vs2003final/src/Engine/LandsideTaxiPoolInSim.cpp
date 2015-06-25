#include "StdAfx.h"
#include ".\LandsideTaxiPoolInSim.h"
#include ".\laneseginsim.h"
#include "LandsideResourceManager.h"
#include "Landside/LandsideTaxiPool.h"
#include "PaxLandsideBehavior.h"
#include "Landside/VehicleItemDetail.h"
#include "LandsideTaxiQueueInSim.h"
#include "Landside/LandsideTaxiInSim.h"
#include "Landside/FacilityTaxiPoolBehavior.h"
#include "ARCportEngine.h"
#include "LandsideSimulation.h"

LandsideTaxiPoolInSim::~LandsideTaxiPoolInSim(void)
{
}
CString LandsideTaxiPoolInSim::print() const
{
	return getInput()->getName().GetIDString();
}


LandsideTaxiPoolInSim::LandsideTaxiPoolInSim( LandsideTaxiPool *pStation )
:LandsideLayoutObjectInSim(pStation)
{
	m_dTotalLength = getTaxiPoolInput()->getControlPath().getBuildPath().GetTotalLength(); 
	m_pAllRes = NULL;
	m_dsepInPool  = 150;
	int iLaneNum = getTaxiPoolInput()->getLaneNum();
	for(int i=0;i<iLaneNum;i++)
	{
		CPath2008* newPath = new CPath2008();
		GetPoolLanePath(i,newPath);
		m_pLanes.push_back(newPath);
	}
}

LandsideLayoutObjectInSim* LandsideTaxiPoolInSim::getLayoutObject() const
{
	return const_cast<LandsideTaxiPoolInSim*>(this);
}

void LandsideTaxiPoolInSim::InitRelateWithOtherObject( LandsideResourceManager* pRes )
{
	m_pAllRes = pRes;
	int entrysId = getTaxiPoolInput()->GetEntryStretch().GetStrethID();
	LandsideStretchInSim* pEntryStretch = pRes->getStretchByID(entrysId);
	ASSERT(pEntryStretch);
	if(pEntryStretch)
	{
		int nlanecount = pEntryStretch->GetLaneCount();
		for(int i=0;i<nlanecount;i++)
		{
			LandsideLaneInSim* pLane = pEntryStretch->GetLane(i);
			LandsideLaneExit* pLaneExit = new LandsideLaneExit();
	
			pLaneExit->SetPosition(pLane,pLane->GetLength() );
			pLaneExit->SetToRes(this);		
			m_vLaneExits.push_back(pLaneExit);
			pLane->AddLaneNode(pLaneExit);
		}
	}

	int exitid = getTaxiPoolInput()->GetExitStretch().GetStrethID();
	LandsideStretchInSim* pExitStretch = pRes->getStretchByID(exitid);
	ASSERT(pExitStretch);
	if(pExitStretch)
	{
		int nLaneCount = pExitStretch->GetLaneCount();
		for(int i=0;i<nLaneCount;i++)
		{
			LandsideStretchLaneInSim* pLane = pExitStretch->GetLane(i);
			LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
			pLaneEntry->SetPosition(pLane,0);
			pLaneEntry->SetFromRes(this);
			m_vLaneEntries.push_back(pLaneEntry);
			pLane->AddLaneNode(pLaneEntry);

		}
	}

}

void LandsideTaxiPoolInSim::InitGraphNodes( LandsideRouteGraph* pGraph ) const
{

}

void LandsideTaxiPoolInSim::InitGraphLink( LandsideRouteGraph* pGraph ) const
{

}

LandsideVehicleInSim* LandsideTaxiPoolInSim::GetVehicleBefore( LandsideVehicleInSim* pVeh ) const
{
	if(m_vInResVehicle.empty())
		return NULL;

	VehicleList::const_iterator itr = std::find(m_vInResVehicle.begin(),m_vInResVehicle.end(),pVeh);
	if(itr==m_vInResVehicle.end())
	{
		return m_vInResVehicle.back();
	}
	if(itr!=m_vInResVehicle.begin())
	{
		itr--;
		return *itr;
	}
	return NULL;
}

LandsideVehicleInSim* LandsideTaxiPoolInSim::GetHeadVehicle() const
{
	if(m_vInResVehicle.empty())
		return NULL;
	return m_vInResVehicle.front();
}

bool LandsideTaxiPoolInSim::GetPoolLanePath( int i,CPath2008* lanePath )
{
	CPath2008& centerPath  = getTaxiPoolInput()->getControlPath().getBuildPath();
	int nPtCount = centerPath.getCount();
	lanePath->Resize(nPtCount);
	double dWidth = getTaxiPoolInput()->getLaneNum()*getTaxiPoolInput()->getLaneWidth();
	ARCPipe pipe(centerPath, dWidth);
	for(int j=0;j<nPtCount;j++)
	{		
		ARCVector3 vPt;
		double drat = (0.5+i)/getTaxiPoolInput()->getLaneNum();
		vPt  = pipe.m_sidePath1[j]*(1.0-drat) + pipe.m_sidePath2[j]*drat;				
		(*lanePath)[j] = CPoint2008(vPt.x,vPt.y,vPt.z);
	}
	return true;
}

const CPath2008& LandsideTaxiPoolInSim::GetPath() const
{
	return getTaxiPoolInput()->getControlPath().getBuildPath();
}

LandsideVehicleInSim* LandsideTaxiPoolInSim::GetVehicleAfter( LandsideVehicleInSim* pVeh ) const
{
	if(m_vInResVehicle.empty())
		return NULL;

	VehicleList::const_iterator itr = std::find(m_vInResVehicle.begin(),m_vInResVehicle.end(),pVeh);
	if(itr== m_vInResVehicle.end())
		return NULL;
	itr++;
	if(itr==m_vInResVehicle.end())
		return NULL;

	return *itr;
}

void LandsideTaxiPoolInSim::NotifyVehicleArrival( LandsideVehicleInSim* pV )
{
	//tell taxiq can fetch cars
	for(size_t i=0;i<m_pAllRes->m_vTaxiQueues.size();i++)
	{
		LandsideTaxiQueueInSim* pQ = m_pAllRes->m_vTaxiQueues.at(i);
		pQ->UpdateOrderingVehicle(pV->curTime());
	}
}

int LandsideTaxiPoolInSim::SentVehicleToQueue( const CString& vehicleType, int nCount,LandsideTaxiQueueInSim* pQ,CARCportEngine* pEngine )
{
	
	int nSendNum = 0;
	for(size_t i=0;i<m_vInResVehicle.size();i++)
	{
		LandsideVehicleInSim* pV = m_vInResVehicle.at(i);
		LandsideTaxiInSim* pTaxi = pV->toTaxi();
		if(!pTaxi)
			break;
		if( pTaxi->getSerivceQueue())
			continue;


		if(!pV->getName().fitIn(vehicleType))
			break;	
		if(!pTaxi->CanServeTaxiQ(this, pQ))
			break;

		pTaxi->StartServiceQueue(pQ);
		nSendNum++;
		if(nSendNum>=nCount)
			break;
	}
	if(nSendNum>0)
		_UpdateOrderingVehicle(pEngine);

	return nSendNum;
}

void LandsideTaxiPoolInSim::OnVehicleExit( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	__super::OnVehicleExit(pVehicle,t);
	LandsideVehicleInSim* pV = GetHeadVehicle();
	if(pV){
		pV->Continue();
	}
	UpdateOrderingVehicle(t);

}

bool LandsideTaxiPoolInSim::IsHaveCapacityAndFull( LandsideVehicleInSim*pVeh ) const
{
	if(m_vInResVehicle.empty())
		return false;

	VehicleList::const_reverse_iterator itr = m_vInResVehicle.rbegin();
	for(;itr!=m_vInResVehicle.rend();++itr)
	{
		LandsideVehicleInSim* pVehicle = *itr;
		if(pVehicle->getLastState().getLandsideRes()!=this)
			continue;
		
		DistanceUnit distEnd = pVehicle->getLastState().distInRes - pVeh->GetLength()*1.5 - m_dsepInPool;
		if(distEnd < 0)
		{
			return true;
		}
		else 
			return false;
	}

	return false;
	
}

void LandsideTaxiPoolInSim::OnVehicleEnter( LandsideVehicleInSim* pVehicle, DistanceUnit dist,const ElapsedTime& t )
{
	__super::OnVehicleEnter(pVehicle,dist,t);
	m_vOrderingVehicles.Remove(pVehicle);
}


void LandsideTaxiPoolInSim::_UpdateOrderingVehicle( CARCportEngine* pEngine )
{
	CFacilityBehavior* pBevhor  = GetBehavior();
	if(!pBevhor)
		return;
	CFacilityTaxiPoolBehavior* pTaxiQBehavior = (CFacilityTaxiPoolBehavior*)pBevhor;
	int nCount = GetNonServiceTaxiCount() + m_vOrderingVehicles.GetCount();

	int nOrderLevel = pTaxiQBehavior->GetRecordLevel();
	int nOrderQuant = pTaxiQBehavior->GetRecordQuantity();
	//int nCapacity = pTaxiQBehavior->getCapacity();
	if( nCount <= nOrderLevel )
	{
		//do ordering count
		for (int i=0;i<nOrderQuant;i++)
		{
			NonPaxVehicleEntryInfo info;
			info.iDemandID = getID();
			info.iDemandType = VehicleDemandAndPoolData::Demand_LayoutObject;
			info.InitName(pTaxiQBehavior->GetVehicleType(),pEngine);

			if( info.InitNoResidentRoute(pEngine) )
			{
				LandsideTaxiInSim *pVehicle = new LandsideTaxiInSim(info);
				if( pVehicle->InitBirth(pEngine) )
				{
					pEngine->GetLandsideSimulation()->AddVehicle(pVehicle);
					m_vOrderingVehicles.push_back(pVehicle);
				}
				else
				{
					cpputil::autoPtrReset(pVehicle);
				}
			}			
		}		
	}

}

int LandsideTaxiPoolInSim::GetNonServiceTaxiCount() const
{
	int nRetCount = 0;
	for(size_t i=0;i<m_vInResVehicle.size();i++)
	{
		LandsideVehicleInSim* pV = m_vInResVehicle.at(i);
		if(LandsideTaxiInSim* pTaxi = pV->toTaxi())
		{
			if( !pTaxi->getSerivceQueue() )
			{
				nRetCount++;
			};
		}
	}
	return nRetCount;
}
