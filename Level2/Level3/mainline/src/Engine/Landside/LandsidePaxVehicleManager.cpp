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
	for(int i=0;i<pVehicleAssign->GetItemCount();i++)
	{
		LandsideVehicleAssignEntry* pVehicleEntry= pVehicleAssign->GetItem(i);
		if ( pVehicleEntry->getType() == LandsideVehicleAssignEntry::AssignType_Public && pVehicleEntry->isScheduled() )
		{
			//daily
			if(pVehicleEntry->isDaily())
			{
				int fDay = pEngine->GetLandsideSimulation()->m_estSimulationStartTime.GetDay();
				int tDay = pEngine->GetLandsideSimulation()->m_estSimulationEndTime.GetDay();
				for(int iDay=fDay;iDay<tDay+1;iDay++){
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


	
	//  generate pax 
	MobLogEntry logentry;
	for(int i=0;i<pPaxLog->getCount();++i)
	{
		pPaxLog->getItem(logentry,i);

		if(!logentry.getOwnStart())
		{
			i = i + (logentry.getGroupSize() - 1) ;
			continue ;
		}
		if(!logentry.isTurnaround())
		{
			if(logentry.GetMobileType()<=0)
			{	
				//
				CMobileElemConstraint paxType = PaxVehicleEntryInfo::getConstrain(logentry,pEngine);
				LandsideVehicleAssignEntry* pVehicleEntry = pVehicleAssign->FindVehicleEntry(paxType);
				if(!pVehicleEntry)
					continue;  // should have warning
				AddPaxToVehicleInfo(pVehicleEntry,logentry,pEngine);
			}				
			if (logentry.GetMobileType() != 2)
			{					
				i = i + logentry.getGroupSize() - 1;
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
	for(VehicleInfoList::iterator itr =m_vPlanVehicles.begin(); itr!=m_vPlanVehicles.end();++itr){
		PaxVehicleEntryInfo& info = *itr;
		if(info.IsBirthAtParkingLot()){
			info.tEntryTime = t;
		}
	}
	m_vPlanVehicles.sort();
}


void LandsidePaxVehicleManager::AddPaxToVehicleInfo(LandsideVehicleAssignEntry* pVehicleAssignEntry,MobLogEntry& paxEntry, CARCportEngine* pEngine)
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
				PaxVehicleEntryInfo* vehicleInfo = FindtheCloseVehicleInfo(pVehicleAssignEntry,paxEntry.getEntryTime());
				vehicleInfo->AddPaxEntry(paxEntry);
			}
			else  //taxi
			{
				PaxVehicleEntryInfo*vehicleInfo = CreateNewVehicleInfo(pVehicleAssignEntry,paxEntry.getEntryTime(),pEngine);	
				vehicleInfo->AddPaxEntry(paxEntry);				
			}
		}		
	}			
}

PaxVehicleEntryInfo* LandsidePaxVehicleManager::CreateNewVehicleInfo( LandsideVehicleAssignEntry* pVehicleAssignEntry, const ElapsedTime& tEntry, CARCportEngine *pEngine)
{
	CString vehicleName = pVehicleAssignEntry->GetVehicleName();
	//
	PaxVehicleEntryInfo info;
	info.InitName(vehicleName,pEngine);

	info.tEntryTime = tEntry;
	info.setVehicleAssignEntry(pVehicleAssignEntry);

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

PaxVehicleEntryInfo* LandsidePaxVehicleManager::FindtheCloseVehicleInfo( LandsideVehicleAssignEntry* pVehicleAssignEntry,const ElapsedTime& tEntry )
{
	PaxVehicleEntryInfo* clost = NULL;
	for(VehicleInfoList::iterator itr = m_vPlanVehicles.begin();itr!=m_vPlanVehicles.end();++itr)
	{
		PaxVehicleEntryInfo& info = *itr;
		if( info.getVehicleAssignEntry() == pVehicleAssignEntry )
		{
			if(!clost)
			{
				clost  = &info;
				continue;
			}
			else
			{
				ElapsedTime tToClost = tEntry - clost->tEntryTime;
				ElapsedTime tToNew = tEntry  - info.tEntryTime;
				if(  abs(tToNew.getPrecisely()) < abs(tToClost.getPrecisely()))
				{
					clost = &info;
					continue;
				}
			}
		}
	}
	return clost;
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

