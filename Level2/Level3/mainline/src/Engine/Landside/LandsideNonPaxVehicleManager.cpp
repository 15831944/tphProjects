#include "StdAfx.h"
#include "LandsideNonPaxVehicleManager.h"
#include "../EVENT.H"
#include "../ARCportEngine.h"
#include "../LandsideSimulation.h"
#include "LandsideNonPaxVehicleInSim.h"
#include "landside/ResidentVehiclTypeContainer.h"

#include "Landside/InputLandside.h"
#include "Landside\VehicleAssignNonPaxRelated.h"
#include "landside/NonPaxVehicleAssignment.h"
#include "Landside/LandsideVehicleTypeNode.h"


void LandsideNonPaxVehicleManager::Start( const ElapsedTime&t  )
{	
	UpdateStartTime(t);
	ScheduleNext();
}



class LandsideNonPaxVehicleEntryEvent : public Event
{
public:
	LandsideNonPaxVehicleEntryEvent(const ElapsedTime& t, LandsideNonPaxVehicleManager* pManager)
	:m_pManager(pManager)
	{
		setTime(t);
	}

	// Main event processing routine for each Event type.
	virtual int process (CARCportEngine* pEngine )
	{	
		NonPaxVehicleEntryInfo info;
		if(m_pManager->Pop(info))
		{
			LandsideNonPaxVehicleInSim * pVehicle = new LandsideNonPaxVehicleInSim(info);
			pVehicle->InitBirth(pEngine);					
			pEngine->GetLandsideSimulation()->AddVehicle(pVehicle);
			m_pManager->ScheduleNext();
		}		
		return TRUE;
	}

	//For shut down event
	virtual int kill (void){ return TRUE; }
	//It returns event's name
	virtual const char *getTypeName (void) const{ return _T("NonPax Vehicle Entry Event"); }
	//It returns event type
	virtual int getEventType (void) const{  return NormalEvent; }
protected:
	LandsideNonPaxVehicleManager * m_pManager;
};

void LandsideNonPaxVehicleManager::ScheduleNext()
{
	if(m_vPlanVehicles.empty())
		return ;
	ElapsedTime nextT;
	if(HasMore(nextT))
	{
		LandsideNonPaxVehicleEntryEvent* pEvt = new LandsideNonPaxVehicleEntryEvent(nextT,this);	
		pEvt->addEvent();
	}
}


bool LandsideNonPaxVehicleManager::Init( CARCportEngine* pEngine,ElapsedTime&t )
{
	LandsideSimulation* pSim = pEngine->GetLandsideSimulation();
	m_vPlanVehicles.clear();

	NonPaxVehicleAssignment* pNonPaxVehicleAssignment = pSim->GetLandsideNonPaxVehicleAssignment();

	int StartDay = pEngine->GetLandsideSimulation()->m_estSimulationStartTime.GetDay();
	int endDay = pEngine->GetLandsideSimulation()->m_estSimulationEndTime.GetDay();
	int Day = endDay - StartDay + 1;
	for(int i =0;i<pNonPaxVehicleAssignment->GetItemCount();i++)
	{
		NonPaxVehicleStrategy* pData =  pNonPaxVehicleAssignment->GetItem(i);

		for(int iTime = 0; iTime < pData->GetTimeEntryCount(); iTime++)
		{
			VehicleTimeEntry* pEntry = pData->GetTimeEntryItem(iTime);
			for (int iEntry = 0; iEntry < pEntry->GetvehicleTypeCount(); iEntry++)
			{
				VehicleTypeEntry* pVehicleEntry = pEntry->GetvehicleTypeItem(iEntry);
				int nCount = pVehicleEntry->GetVehicleNumber();

				if (pData->IsDaily())
				{		
					ElapsedTime tDayInterval(24*60*60l);
					for (int k = 0; k < Day; k++)
					{		
						ElapsedTime tDayTime = tDayInterval * (double)k;
						for(int j=0;j<nCount;++j)
						{	
							ElapsedTime tStartTime;
							ElapsedTime intervalTime = pEntry->GetTimeRange().GetEndTime() - pEntry->GetTimeRange().GetStartTime();
					
							long lStarTime = intervalTime* ((double)rand()/RAND_MAX);
							tStartTime = tDayTime + pEntry->GetTimeRange().GetStartTime() + tStartTime.setPrecisely(lStarTime);
							
							NonPaxVehicleEntryInfo info;
							info.iDemandID = pData->GetID();
							info.iDemandType = VehicleDemandAndPoolData::Demand_Strategy;
							info.tEntryTime = tStartTime;

							info.InitName(pVehicleEntry->GetVehicleType(),pEngine);
											
							if(info.InitNoResidentRoute(pEngine))
								m_vPlanVehicles.push_back(info);
						}	
					}
				} 
				else
				{
					for(int j=0;j<nCount;++j)
					{			
						ElapsedTime tStartTime;
						ElapsedTime intervalTime = pEntry->GetTimeRange().GetEndTime() - pEntry->GetTimeRange().GetStartTime();
						long lStarTime = intervalTime* ((double)rand()/RAND_MAX);
						tStartTime = pEntry->GetTimeRange().GetStartTime() + tStartTime.setPrecisely(lStarTime);

						NonPaxVehicleEntryInfo info;
						info.iDemandID = pData->GetID();
						info.iDemandType = VehicleDemandAndPoolData::Demand_Strategy;
						info.tEntryTime = tStartTime;
						info.InitName(pVehicleEntry->GetVehicleType(),pEngine);

						if( info.InitNoResidentRoute(pEngine))
						{
							m_vPlanVehicles.push_back(info);
						}
					}
				}
			}
		}

	}

	InitEntryTime(pEngine);
	return HasMore(t);	
}

void LandsideNonPaxVehicleManager::UpdateStartTime( const ElapsedTime& t )
{
	for(VehicleInfoList::iterator itr =m_vPlanVehicles.begin(); itr!=m_vPlanVehicles.end();++itr){
		NonPaxVehicleEntryInfo& info = *itr;
		if(info.IsBirthAtParkingLot()){
			info.tEntryTime = t;
		}
	}
	m_vPlanVehicles.sort();
}

bool LandsideNonPaxVehicleManager::Pop( NonPaxVehicleEntryInfo& front )
{
	if(m_vPlanVehicles.empty())
		return false;

	front = *m_vPlanVehicles.begin();
	m_vPlanVehicles.pop_front();
	return true;
}

void LandsideNonPaxVehicleManager::InitEntryTime( CARCportEngine* pEngine )
{
	VehicleInfoList validlist;
	for(VehicleInfoList::iterator i=m_vPlanVehicles.begin();i!=m_vPlanVehicles.end();i++)
	{
		NonPaxVehicleEntryInfo& info = *i;
		if( info.InitEntryTimeOfVehicle(pEngine) )
		{
			validlist.push_back(info);
		}
	}
	m_vPlanVehicles = validlist;
	m_vPlanVehicles.sort();
}

bool LandsideNonPaxVehicleManager::HasMore( ElapsedTime& nextT )
{
	if(!m_vPlanVehicles.empty())
	{
		nextT = m_vPlanVehicles.front().tEntryTime;
		return true;
	}
	return false;
}
