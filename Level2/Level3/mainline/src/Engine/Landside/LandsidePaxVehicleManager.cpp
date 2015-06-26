#include "StdAfx.h"
#include "LandsidePaxVehicleManager.h"
#include "..\ARCportEngine.h"
#include "Landside\LandsideVehicleAssignment.h"
#include "..\LandsideSimulation.h"
#include "Landside\InputLandside.h"
#include "Landside\LandsideVehicleTypeNode.h"
#include "LandsidePrivateVehicleInSim.h"
#include "LandsideTaxiInSim.h"
#include "LandsidePublicExtBusInSim.h"
#include "..\LogContainer.h"


void LandsidePaxVehicleManager::InitEntryTime(CARCportEngine* pEngine)
{
	VehicleInfoList validlist;
	for(VehicleInfoList::iterator i=m_vPlanVehicles.begin();i!=m_vPlanVehicles.end();i++)
	{
		PaxVehicleEntryInfo& info = *i;
		if( info.InitEntryTimeOfVehicle(pEngine) )
		{
			validlist.push_back(info);
		}
	}
	m_vPlanVehicles = validlist;
	m_vPlanVehicles.sort();
}



void LandsidePaxVehicleManager::Start(const ElapsedTime& t)
{
	UpdateStartTime(t);	
	//add this to event list
	ScheduleNext();
}



bool LandsidePaxVehicleManager::Init( CARCportEngine *pEngine,ElapsedTime& t )
{
	//
	PaxLog * pPaxLog = pEngine->getPaxLog();
	//CPaxBulkInfo* pBulkInfo = pEngine->getTerminal()->m_pPaxBulkInfo;
	LandsideVehicleAssignContainer* pVehicleAssign =  pEngine->GetLandsideSimulation()->getInput()->getPaxVehicleAssignment();


	//generate public scheduled vehicle entry
	for(int nPaxIndex=0;nPaxIndex<pVehicleAssign->GetItemCount();nPaxIndex++)
	{
		LandsideVehicleAssignEntry* pVehicleEntry= pVehicleAssign->GetItem(nPaxIndex);
		if ( pVehicleEntry->getType() == LandsideVehicleAssignEntry::AssignType_Public && pVehicleEntry->isScheduled() )
		{
			//daily
			if(pVehicleEntry->isDaily())
			{
				int fDay = pEngine->GetLandsideSimulation()->m_estSimulationStartTime.GetDay();
				int tDay = pEngine->GetLandsideSimulation()->m_estSimulationEndTime.GetDay();
				for(int iDay=fDay;iDay<tDay+1;iDay++)
				{
					ElapsedTime tTimeIter = pVehicleEntry->getStartTime();
					while(tTimeIter < pVehicleEntry->getEndTime() )
					{
						ElapsedTime t = tTimeIter;
						t.SetDay(iDay);
						//add vehicle
						CreateNewVehicleInfo(pVehicleEntry,t,pEngine);
						
						tTimeIter += pVehicleEntry->getFrequency();
					}
				}
			}
			else //no daily
			{				
				if(pVehicleEntry->getStartTime()< pVehicleEntry->getEndTime() && pVehicleEntry->getFrequency() > ElapsedTime(0L) )
				{
					ElapsedTime tTimeIter = pVehicleEntry->getStartTime();
					while(tTimeIter < pVehicleEntry->getEndTime() )
					{
						//add vehicle
						CreateNewVehicleInfo(pVehicleEntry,tTimeIter,pEngine);
						tTimeIter += pVehicleEntry->getFrequency();
					}
				}				
			}
		}
	}

	//sort public bus by entry time, so the pax who take the pax bus will take the first available bus to entry landside simulation system
	m_vPlanVehicles.sort();
	//  generate pax 
	MobLogEntry logentry;
	for(int nPaxIndex=0;nPaxIndex<pPaxLog->getCount();++nPaxIndex)
	{
		pPaxLog->getItem(logentry,nPaxIndex);

		if(!logentry.getOwnStart())
		{
			nPaxIndex = nPaxIndex + (logentry.getGroupSize() - 1) ;
			continue;
		}
		if(!logentry.isTurnaround())
		{
			if(logentry.GetMobileType()<=0)
			{	
				//
				CMobileElemConstraint paxType = PaxVehicleEntryInfo::getConstrain(logentry,pEngine);
				LandsideVehicleAssignEntry* pVehicleEntry = pVehicleAssign->FindVehicleEntry(paxType);
				if(!pVehicleEntry)
				{
					logentry.setBirthExceptionCode(BEC_LANDSIDENOVEHICLECOVERED);
					pPaxLog->updateItem(logentry, nPaxIndex);
					//pax in the group also need to set the marker
					for (int nFollower = 1; nFollower < logentry.getGroupSize(); ++ nFollower)//start with 1, the pax is one person in group
					{
						int nFollowerIndex = nFollower + nPaxIndex;
						MobLogEntry logentryFollower;
						pPaxLog->getItem(logentryFollower,nFollowerIndex);
						logentryFollower.setBirthExceptionCode(BEC_LANDSIDENOVEHICLECOVERED);
						pPaxLog->updateItem(logentryFollower, nFollowerIndex);
					}
				}
				else if(!AssignPaxToVehicleInfo(pVehicleEntry,logentry,pEngine))
				{
					logentry.setBirthExceptionCode(BEC_LANDSIDEVEHICLEOVERCAPACITY);
					pPaxLog->updateItem(logentry, nPaxIndex);
					//pax in the group also need to set the marker
					for (int nFollower = 1; nFollower < logentry.getGroupSize(); ++ nFollower)
					{
						int nFollowerIndex = nFollower + nPaxIndex;
						MobLogEntry logentryFollower;
						pPaxLog->getItem(logentryFollower,nFollowerIndex);
						logentryFollower.setBirthExceptionCode(BEC_LANDSIDEVEHICLEOVERCAPACITY);
						pPaxLog->updateItem(logentryFollower, nFollowerIndex);
					}
				}

				//skip the group members and move to next passenger who is leader in group 
				nPaxIndex = nPaxIndex + logentry.getGroupSize() - 1;
			}
		}			
	}	

	InitEntryTime(pEngine); //init entry time of the private vehicle

	if(!m_vPlanVehicles.empty())
	{
		t= m_vPlanVehicles.begin()->tEntryTime;
		return true;
	}
	return false;	
}

void LandsidePaxVehicleManager::UpdateStartTime( const ElapsedTime& t )
{
	for(VehicleInfoList::iterator itr =m_vPlanVehicles.begin(); itr!=m_vPlanVehicles.end();++itr)
	{
		PaxVehicleEntryInfo& info = *itr;
		if(info.IsBirthAtParkingLot())
		{
			info.tEntryTime = t;
		}
	}
	m_vPlanVehicles.sort();
}


bool LandsidePaxVehicleManager::AssignPaxToVehicleInfo(LandsideVehicleAssignEntry* pVehicleAssignEntry,MobLogEntry& paxEntry, CARCportEngine* pEngine)
{
	//CHierachyName sVehicleType;
	//sVehicleType.fromString(pVehicleAssignEntry->GetVehicleName());
	//init individual vehicles
	if(pVehicleAssignEntry->getType()==LandsideVehicleAssignEntry::AssignType_Individual)//private
	{
		PaxVehicleEntryInfo*vehicleInfo = CreateNewVehicleInfo(pVehicleAssignEntry,paxEntry.getEntryTime(),pEngine);	
		vehicleInfo->AddPaxEntry(paxEntry);
	}
	//taxi  or  bus
	if(pVehicleAssignEntry->getType()==LandsideVehicleAssignEntry::AssignType_Public)
	{
		if(paxEntry.isDeparting())
		{
			if(pVehicleAssignEntry->isScheduled()) //ext bus
			{
				//find the public schedule entry and to the close vehicle
				PaxVehicleEntryInfo* vehicleInfo = FindtheCloseVehicleInfo(pVehicleAssignEntry,paxEntry);
				//ASSERT(vehicleInfo != NULL);
				if(vehicleInfo)
					vehicleInfo->AddPaxEntry(paxEntry);
				else//failed to find available vehicle
					return false;
			}
			else  //taxi
			{
				PaxVehicleEntryInfo*vehicleInfo = CreateNewVehicleInfo(pVehicleAssignEntry,paxEntry.getEntryTime(),pEngine);	
				vehicleInfo->AddPaxEntry(paxEntry);				
			}
		}		
	}

	return true;
}

PaxVehicleEntryInfo* LandsidePaxVehicleManager::CreateNewVehicleInfo(LandsideVehicleAssignEntry* pVehicleAssignEntry, const ElapsedTime& tEntry, CARCportEngine *pEngine)
{
	CString vehicleName = pVehicleAssignEntry->GetVehicleName();
	//
	PaxVehicleEntryInfo info;
	info.InitName(vehicleName,pEngine);

	info.tEntryTime = tEntry;
	info.setVehicleAssignEntry(pVehicleAssignEntry);

	//Find vehilce property
	ASSERT(pEngine != NULL);
	ASSERT(pEngine->GetLandsideSimulation() != NULL);
	
	LandsideVehicleProperty* pVehicleProperty =  NULL;
	if(pEngine && pEngine->GetLandsideSimulation())
		pVehicleProperty = pEngine->GetLandsideSimulation()->getVehicleProp(info.vehicleType);
	ASSERT(pVehicleProperty != NULL);
	info.setVehicleProperty(pVehicleProperty);

	m_vPlanVehicles.push_back(info);

	return &(*m_vPlanVehicles.rbegin());
}

bool LandsidePaxVehicleManager::GetVehicleInfo( int nPax,PaxVehicleEntryInfo& vehicleInfo ) 
{
	for(VehicleInfoList::iterator i=m_vPlanVehicles.begin();i!=m_vPlanVehicles.end();++i)
	{
		PaxVehicleEntryInfo& info = *i;
		MobLogEntry* pLogEntry = info.GetLogEntry(nPax);
		if (pLogEntry)
		{
			vehicleInfo = info;
			return true;

		}
	}
	return false;
}

PaxVehicleEntryInfo* LandsidePaxVehicleManager::FindtheCloseVehicleInfo( LandsideVehicleAssignEntry* pVehicleAssignEntry,const MobLogEntry& paxEntry )
{
	PaxVehicleEntryInfo* closestVehicle = NULL;
	for(VehicleInfoList::iterator itr = m_vPlanVehicles.begin();itr!=m_vPlanVehicles.end();++itr)
	{
		PaxVehicleEntryInfo& plannedVehicleInfo = *itr;
		if( plannedVehicleInfo.getVehicleAssignEntry() == pVehicleAssignEntry )
		{
			if(plannedVehicleInfo.tEntryTime >= paxEntry.getEntryTime()) // the planned vehicle has been sorted by entry time, so find the first eligible one
			{
				if(plannedVehicleInfo.IsVacant(paxEntry.getGroupSize()))
				{
					closestVehicle = &plannedVehicleInfo;
					break;
				}
			}
		}
	}
	return closestVehicle;
}

int LandsidePaxVehicleManager::process( CARCportEngine* pEngine )
{
	ASSERT(!m_vPlanVehicles.empty());
	if(m_vPlanVehicles.empty())
		return FALSE;


	PaxVehicleEntryInfo info = *m_vPlanVehicles.begin();
	m_vPlanVehicles.pop_front();

	LandsidePaxVehicleInSim * pVehicle = NULL;
	if(info.isScheduleBus())
	{
		pVehicle = new LandsidePublicExtBusInSim(info);	
	}
	else if(info.isPrivate())
	{
		pVehicle = new LandsidePrivateVehicleInSim(info);
	}
	else if(info.isTaxi())  //TODO
	{
		pVehicle = new LandsideTaxiInSim(info);		
	}

	if( pVehicle && pVehicle->InitBirth(pEngine) )
	{
		pEngine->GetLandsideSimulation()->AddVehicle(pVehicle);
	}
	else
	{
		cpputil::autoPtrReset(pVehicle);
	}

	ScheduleNext();
	return FALSE;
}

void LandsidePaxVehicleManager::ScheduleNext()
{
	if( m_vPlanVehicles.empty() )
		return;

	PaxVehicleEntryInfo& info = *m_vPlanVehicles.begin();
	time = info.tEntryTime;
	addEvent();
}


