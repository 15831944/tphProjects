#include "StdAfx.h"
#include ".\landsideitinerantvehicleinsim.h"
#include "landside\Goal_PrivateVehicle.h"
#include "Landside\Goal_MoveInRoad.h"
#include <landside\CLandSideIterateVehicle.h>
#include <landside\VehicleItemDetail.h>
#include "..\common\ProbabilityDistribution.h"
#include "..\common\probdistmanager.h"
#include "..\Inputs\probab.h"
#include "ARCportEngine.h"
#include "LandsideSimulation.h"
#include <Landside\VehicleItemDetailList.h>

class Goal_ItinerantVehicle : public VehicleSequenceGoal
{
public:
	Goal_ItinerantVehicle(LandsideVehicleInSim* pEnt);;
	void Activate( CARCportEngine* pEngine );

};

Goal_ItinerantVehicle::Goal_ItinerantVehicle( LandsideVehicleInSim* pEnt ) :VehicleSequenceGoal(pEnt)
{
	m_sDesc = "Goal_ItinerantVehicle";
}

void Goal_ItinerantVehicle::Activate( CARCportEngine* pEngine )
{
	m_iStatus = gs_active;
	RemoveAllSubgoals();			
	//m_SubGoals.push_back(new Goal_BirthAtEntryNode(m_pOwner));		
	m_SubGoals.push_back(new Goal_LeaveLandside(m_pOwner));
}

LandsideItinerantVehicleInSim::LandsideItinerantVehicleInSim( CVehicleItemDetail* pVehicleType ) 
//:LandsideVehicleInSim(pVehicleType)
{
	//create the goal direct brain
}

void LandsideItinerantVehicleInSim::InitBirth( CARCportEngine *_pEngine )
{
	m_pBrain = new Goal_ItinerantVehicle(this);
	Activate(curTime());
	return false;
}

void LandsideItineratVehicleGenerator::InitSchedules( InputTerminal* pInTerm,CVehicleItemDetailList & typelist )
{
	for(int i=0;i<typelist.GetElementCount();i++)
	{
		CVehicleItemDetail* pType = typelist.GetItem(i);
		if(pType->getLandSideBaseType()!=VehicleType_Iterate)
		{
			continue;
		}
		LandSideIterateVehicle *tmpType=(LandSideIterateVehicle*)pType->getOperation(VehicleType_Iterate);
		if(tmpType)
		{
			for (int i=0;i<tmpType->getElementCount();i++)
			{
				LandSideVehicleTypeDistribution* pDist=   tmpType->getItem(i);

				int nCount = 0;
				CProbDistEntry* pPBEntry = _g_GetActiveProbMan( pInTerm )->getItemByName( pDist->getDistribution() );			
				if(pPBEntry)
				{
					nCount  = (int) pPBEntry->m_pProbDist->getRandomValue();
				}

				ElapsedTime dT = pDist->getRangeT() - pDist->getRangeF();


			}
		}
	}
	
	SortSchedule();
}



void LandsideItineratVehicleGenerator::ScheduleNext()
{
	if(!m_vSchedules.empty())
	{
		ItrVehicleGenSchedule& sch = *m_vSchedules.begin();
		pNextType = sch.pType;
		Activate(sch.mGenTime);
		m_vSchedules.pop_front();
	}
	else
	{
		//Terminate();
	}
}

void LandsideItineratVehicleGenerator::OnActive( CARCportEngine*pEngine )
{
	//generate 		
	/*LandsideItinerantVehicleInSim* pnewVehicle = new LandsideItinerantVehicleInSim(pNextType);
	pnewVehicle->InitBirth(pEngine);	
	pnewVehicle->InitLogEntry(NULL,-1,'A',pEngine->GetLandsideSimulation()->getOutput());
	m_vGeneratedVehicles.push_back(pnewVehicle);
	ScheduleNext();*/
}


