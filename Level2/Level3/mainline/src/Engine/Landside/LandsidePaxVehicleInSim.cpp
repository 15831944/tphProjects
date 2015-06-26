#include "StdAfx.h"
#include "LandsidePaxVehicleInSim.h"
#include "..\TurnaroundVisitor.h"
#include "..\TURNARND.H"
#include "..\VISITOR.H"
#include "..\PAX.H"
#include "..\ARCportEngine.h"
#include "..\GroupLeaderInfo.h"
#include "..\DeparturePaxLandsideBehavior.h"
#include "..\ArrivalPaxLandsideBehavior.h"


int LandsidePaxVehicleInSim::GenerateDeparturePax( CARCportEngine*pEngine )
{
	Person* p_pax = NULL ;
	int nGenCount =0;

	for(size_t i=0;i<m_entryInfo.vPaxEntryList.size();i++)
	{
		MobLogEntry& logentry = m_entryInfo.vPaxEntryList[i];

		if(!logentry.isDeparture())
		{
			continue;
		}

		if(!logentry.getOwnStart())
		{
			ASSERT(0);
			//i = i + (logentry.getGroupSize() - 1) ;
			continue ;
		}

		if(logentry.isTurnaround())
		{
			if(logentry.GetMobileType()>0)
				p_pax = new TurnaroundVisitor(logentry, pEngine);
			else
				p_pax = new TurnaroundPassenger ( logentry, pEngine );
		}
		else
		{
			if(logentry.GetMobileType()>0)
				p_pax = new PaxVisitor ( logentry, pEngine );
			else
				p_pax = new Passenger (logentry, pEngine );
		} 
		pEngine->m_simBobileelemList.Register( p_pax );

		if (logentry.GetMobileType() != 2)
		{
			p_pax->newGroupFollower();
			//i = i + logentry.getGroupSize() - 1;
		}
		else
		{
			((CGroupLeaderInfo*)p_pax->m_pGroupInfo)->SetGroupLeader(p_pax);
		}

		if(logentry.GetMobileType() == 0) // the pax is the 
		{
			DeparturePaxLandsideBehavior* pBehavior = new DeparturePaxLandsideBehavior( p_pax);			
			p_pax->setBehavior(pBehavior);
			p_pax->setState(EntryLandside);	
			pBehavior->InitializeBehavior();
			pBehavior->setVehicle(this);
			pBehavior->performanceMove(curTime(),true);			
			nGenCount++;
		}
	}
	return  nGenCount;
}

LandsidePaxVehicleInSim::LandsidePaxVehicleInSim( const PaxVehicleEntryInfo& entryInfo )
:m_entryInfo(entryInfo)
{
	m_TypeName = entryInfo.vehicleType;
}



int LandsidePaxVehicleInSim::GenerateArrivalPax( CARCportEngine* pEngine )
{
	Person* p_pax = NULL ;
	int nGenCount =0;

	for(size_t i=0;i<m_entryInfo.vPaxEntryList.size();i++)
	{
		MobLogEntry& logentry = m_entryInfo.vPaxEntryList[i];

		if(!logentry.isArrival()){
			continue;
		}

		if(!logentry.getOwnStart())
		{
			i = i + (logentry.getGroupSize() - 1) ;
			continue ;
		}

		if(logentry.isTurnaround())
		{
			if(logentry.GetMobileType()>0)
				p_pax = new TurnaroundVisitor(logentry, pEngine);
			else
				p_pax = new TurnaroundPassenger ( logentry, pEngine );
		}
		else
		{
			if(logentry.GetMobileType()>0)
				p_pax = new PaxVisitor ( logentry, pEngine );
			else
				p_pax = new Passenger (logentry, pEngine );
		} 
		pEngine->m_simBobileelemList.Register( p_pax );

		if (logentry.GetMobileType() != 2)
		{
			p_pax->newGroupFollower();
			i = i + logentry.getGroupSize() - 1;
		}
		else
		{
			((CGroupLeaderInfo*)p_pax->m_pGroupInfo)->SetGroupLeader(p_pax);
		}

		if(logentry.GetMobileType() == 0) // the pax is the 
		{
			ArrivalPaxLandsideBehavior* pBehavior = new ArrivalPaxLandsideBehavior( p_pax);			
			p_pax->setBehavior(pBehavior);
			p_pax->setState(EntryLandside);	
			pBehavior->InitializeBehavior();
			pBehavior->setVehicle(this);

			pBehavior->GenerateEvent( MAX(curTime(),logentry.getEntryTime()) );
			//pBehavior->performanceMove(curTime(),true);			
			nGenCount++;
		}
	}
	return  nGenCount;
}

bool LandsidePaxVehicleInSim::IsPaxsVehicle( int nPaxId ) const
{
	for(size_t i=0;i<m_entryInfo.vPaxEntryList.size();i++){
		if(m_entryInfo.vPaxEntryList[i].getID() == nPaxId )
			return true;
	}
	return false;
}

bool LandsidePaxVehicleInSim::CellPhoneCall(CARCportEngine* pEngine) const
{
	for(size_t i=0;i<m_entryInfo.vPaxEntryList.size();i++)
	{
		int paxid = m_entryInfo.vPaxEntryList[i].getID();
		if(IsPaxOnVehicle(paxid))
			continue;

		 MobileElement* pMob = pEngine->m_simBobileelemList.GetAvailableElement(paxid);
		 if (pMob == NULL)
			return false;
		 
		LandsideBaseBehavior* pBehave = pMob->getLandsideBehavior();
		if (pBehave == NULL)
			return false;
		
		if (pBehave->toArrBehavior() == NULL)
			return false;
		

		LandsideResourceInSim* pResource = pBehave->getResource();
		if (pResource == NULL)
			return false;

		if (pResource->toCurbSide() == NULL)
			return false;
	}
	return true;
}

void LandsidePaxVehicleInSim::NotifyPaxMoveInto( CARCportEngine*pEngine, LandsideBaseBehavior* pPassenger, const ElapsedTime& eTime )
{
	/*if(m_entryInfo.isPublic())
	{
		ASSERT(false);
	}
	else*/
	{
		AddOnPax(pPassenger);
		Activate(eTime);
	}
}

bool LandsidePaxVehicleInSim::HavePaxGetOff( LandsideResourceInSim* pRes ) const
{
	ElapsedTime curT = curTime();
	for(size_t i=0;i<m_vOnVehiclePax.size();i++)
	{  //search in the on vehicle pax list, check if they need to get off
		LandsideBaseBehavior* pBehavior = m_vOnVehiclePax[i];
		if(pBehavior->IsGetOff(pRes))
			return true;
	}
	return false;
}



void LandsidePaxVehicleInSim::CallPaxGetOff( LandsideResourceInSim* pRes )
{
	ElapsedTime  toffset =  pRes->getLayoutObject()->GetServiceTime(m_TypeName);

	for(size_t i=0;i<m_vOnVehiclePax.size();i++)
	{  //search in the on vehicle pax list, check if they need to get off
		LandsideBaseBehavior* pBehavior = m_vOnVehiclePax[i];
		if(pBehavior->IsGetOff(pRes))
		{
			ElapsedTime t;
			t.setPrecisely(toffset.getPrecisely() *(i+1));
			pBehavior->OnVehicleParkAtPlace(this ,pRes, curTime() +  t);
		}
	}
}

