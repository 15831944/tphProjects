#include "stdafx.h"
#include "LandsideVehicleEntryInfo.h"
#include "LandsideSimulation.h"
#include "ARCportEngine.h"
#include "..\Landside\InputLandside.h"
#include "..\Landside\LandsideResidentRelatedContainer.h"
#include "LandsideLayoutObjectInSim.h"
#include "LandsideResourceManager.h"
#include "..\Landside\NonPaxVehicleAssignment.h"
#include "..\Landside\VehicleOperationNonPaxRelatedContainer.h"
#include "..\Landside\LandsideVehicleAssignment.h"
#include "..\Landside\LandsideEntryOffsetData.h"
#include "..\Inputs\MobileElemConstraint.h"
#include "TERMINAL.H"
#include "Landside\LandsideVehicleTypeNode.h"
#include "Landside\VehicleGroupProperty.h"

bool VehicleEntryInfo::InitNoResidentRoute(CARCportEngine* pEngine )
{
	LandsideSimulation* pSim = pEngine->GetLandsideSimulation();
	pPlan = NULL;
	pFirstRoute = NULL;
	NonResidentVehicleTypeContainer* pNonresidentSpec = pSim->getInput()->getNonResidentVehiclePlan();
	pPlan =  pNonresidentSpec->FindMatchPlan(tEntryTime,vehicleType);
	if(!pPlan)
	{
		CString sError;
		sError.Format(_T("Can not find Operation Plan for Vehicle %s "), vehicleType.toString().GetString() );
		pEngine->SendSimMessage(sError);
		return false;
	}

	ResidentVehicleRouteList* pStartRoute = pPlan->GetStartRoute();
	if(pStartRoute)
	{
		pFirstRoute = pStartRoute->GetChildRouteRandom();		
	}

	if(!pFirstRoute)
	{
		CString sError;
		sError.Format(_T("The Operation Plan for Vehicle %s is not define right "), vehicleType.toString().GetString() );
		pEngine->SendSimMessage(sError);
		return false;
	}

	return true;
}

bool VehicleEntryInfo::IsBirthAtParkingLot() const
{
	if(pFirstRoute)
	{
		if(LandsideFacilityObject* pObj = pFirstRoute->GetFacilityObject()  )
		{
			return pObj->GetType() ==  LandsideFacilityObject::Landside_Parklot_Facility;
		}
	}
	return false;
}

bool VehicleEntryInfo::operator<( const VehicleEntryInfo& other ) const
{
	return tEntryTime<other.tEntryTime;
}

VehicleEntryInfo::VehicleEntryInfo()
{
	pFirstRoute = NULL;
	pPlan = NULL;
	//pStartRoute = NULL;
}

void VehicleEntryInfo::InitName( const CString& name,CARCportEngine *pEngine )
{
	CString vehicleName = name;
	//throw error if can not find defined name
	LandsideVehicleBaseNode* pNameNode = pEngine->GetLandsideSimulation()->getInput()->getVehicleTypeNames()->FindNode(vehicleName);
	if(pNameNode)
	{
		pNameNode = pNameNode->GetRandomLeaf();
		if(pNameNode)
			vehicleName = pNameNode->GetFullName();
	}
	//	
	vehicleType.fromString(vehicleName);
}

//////////////////////////////////////////////////////////////////////////
bool NonPaxVehicleEntryInfo::InitEntryTimeOfVehicle( CARCportEngine* pEngine )
{
	LandsideSimulation* pSim = pEngine->GetLandsideSimulation();//LandsideSimulation* pSim 
	NonPaxVehicleAssignment* pNonPaxVehicleAssignment = pSim->GetLandsideNonPaxVehicleAssignment();
	ASSERT(pNonPaxVehicleAssignment);
	if(pFirstRoute == NULL || LandsideFacilityObject::Landside_EntryExit_Facility!= pFirstRoute->GetFacilityObject()->GetType())
		return true;

	LandsideLayoutObjectInSim* pObj = pSim->GetResourceMannager()->getLayoutObjectInSim(pFirstRoute->GetFacilityObject()->GetFacilityID());
	if(pObj)
	{
		ElapsedTime offsetT  = pNonPaxVehicleAssignment->FindMatchOffset(pObj->getInput()->getName(),iDemandID);
		tEntryTime -= offsetT;	
		tEntryTime = MAX(ElapsedTime(0L),tEntryTime);
	}		
	return true;
}

bool NonPaxVehicleEntryInfo::InitNoResidentRoute( CARCportEngine* pEngine )
{
	LandsideSimulation* pSim = pEngine->GetLandsideSimulation();
	pPlan = NULL;
	pFirstRoute = NULL;
	VehicleOperationNonPaxRelatedContainer* pNonPaxRelatedCon = pSim->GetLandsideNonPaxRelatedCon();
	pPlan =  pNonPaxRelatedCon->FindMatchPlan(tEntryTime,iDemandType,iDemandID);
	if(!pPlan)
	{
		CString sError;
		sError.Format(_T("Can not find Operation Plan for Vehicle %s "), vehicleType.toString().GetString() );
		pEngine->SendSimMessage(sError);
		return false;
	}

	ResidentVehicleRouteList* pStartRoute = pPlan->GetStartRoute();
	if(pStartRoute)
	{
		pFirstRoute = pStartRoute->GetChildRouteRandom();		
	}

	if(!pFirstRoute)
	{
		CString sError;
		sError.Format(_T("The Operation Plan for Vehicle %s is not define right "), vehicleType.toString().GetString() );
		pEngine->SendSimMessage(sError);
		return false;
	}


	return true;
}



//////////////////////////////////////////////////////////////////////////

bool PaxVehicleEntryInfo::InitEntryTimeOfVehicle( CARCportEngine* pEngine)
{

	if(!InitNoResidentRoute(pEngine))
	{
		return false;
	}	


	LandsideSimulation* pSim = pEngine->GetLandsideSimulation();//LandsideSimulation* pSim 
	LandsideEntryOffset::OffsetEntries* pOffsets =  pSim->getInput()->getLandEntryOffsetData();


	if(vPaxEntryList.empty())
	{
		double ramdMin = pVehicleAssignEntry->getDstOffset()->GetProbDistribution()->getRandomValue();
		ElapsedTime tOffset;
		tOffset.set( (long)ramdMin*60L );
		tEntryTime -= tOffset;
		tEntryTime = MAX(ElapsedTime(0L),tEntryTime);
		return true;	
	}

	if(LandsideFacilityObject::Landside_EntryExit_Facility!= pFirstRoute->GetFacilityObject()->GetType())
		return true;

	LandsideLayoutObjectInSim* pObj = pSim->GetResourceMannager()->getLayoutObjectInSim(pFirstRoute->GetFacilityObject()->GetFacilityID());
	if(pObj)
	{
		CMobileElemConstraint mobconst = getConstrain(vPaxEntryList.front(),pEngine);
		//mobconst.initMobType(vPaxEntryList.begin()->GetMobDesc());
		ElapsedTime offsetT  = pOffsets->FindMatchOffset(pObj->getInput()->getName(),mobconst,tEntryTime);
		tEntryTime -= offsetT;	
		tEntryTime = MAX(ElapsedTime(0L),tEntryTime);
	}		
	return true;
}



const MobLogEntry* PaxVehicleEntryInfo::GetLogEntry( int nId ) const
{
	for(size_t i=0;i< vPaxEntryList.size();i++)
	{
		if(nId == vPaxEntryList[i].getID())
			return &vPaxEntryList[i];
	}
	return NULL;
}

MobLogEntry* PaxVehicleEntryInfo::GetLogEntry( int nId )
{
	for(size_t i=0;i< vPaxEntryList.size();i++)
	{
		if(nId == vPaxEntryList[i].getID())
			return &vPaxEntryList[i];
	}
	return NULL;
}

bool PaxVehicleEntryInfo::isPublic()const
{
	if(pVehicleAssignEntry)
		return pVehicleAssignEntry->getType()==LandsideVehicleAssignEntry::AssignType_Public;
	return false;
}

bool PaxVehicleEntryInfo::InitNoResidentRoute( CARCportEngine* pEngine )
{
	LandsideSimulation* pSim = pEngine->GetLandsideSimulation();
	pPlan = NULL;
	pFirstRoute = NULL;
	NonResidentVehicleTypeContainer* pNonresidentSpec = pSim->getInput()->getNonResidentVehiclePlan();

	if( !vPaxEntryList.empty() )
	{
		CMobileElemConstraint paxType = getConstrain(vPaxEntryList.front(),pEngine);
		pPlan = pNonresidentSpec->FindMatchPlan(tEntryTime,vehicleType, paxType);		
	}
	else if(pVehicleAssignEntry)
	{
		pPlan = pNonresidentSpec->FindMatchPlan(tEntryTime,vehicleType, pVehicleAssignEntry->getPaxType());
	}

	if(!pPlan)
	{
		CString sError;
		sError.Format(_T("Can not find Operation Plan for Vehicle %s "), vehicleType.toString().GetString() );
		pEngine->SendSimMessage(sError);
		return false;
	}

	ResidentVehicleRouteList* pStartRoute = pPlan->GetStartRoute();
	if(pStartRoute)
	{
		pFirstRoute = pStartRoute->GetChildRouteRandom();		
	}

	if(!pFirstRoute)
	{
		CString sError;
		sError.Format(_T("The Operation Plan for Vehicle %s is not define right "), vehicleType.toString().GetString() );
		pEngine->SendSimMessage(sError);
		return false;
	}


	return true;
}

CMobileElemConstraint PaxVehicleEntryInfo::getConstrain( MobLogEntry& m_logEntry,CARCportEngine* pEngine )
{
	Terminal* m_pTerm = pEngine->getTerminal();

	CMobileElemConstraint m_type(m_pTerm);	

	// get flight related info
	//int flightIndex;
	if (m_logEntry.isDeparting())
	{
		int m_ilightIndex = m_logEntry.getDepFlight();
		Flight *aFlight = pEngine->m_simFlightSchedule.getItem (m_ilightIndex);
		(FlightConstraint&)m_type = aFlight->getType('D');		
		m_type.setFlightIndex(m_ilightIndex);		
	}
	else
	{
		int m_ilightIndex = m_logEntry.getArrFlight();
		Flight *aFlight = pEngine->m_simFlightSchedule.getItem (m_ilightIndex);
		(FlightConstraint&)m_type = aFlight->getType('A');
		m_type.setFlightIndex(m_ilightIndex);

	}

	m_type.initMobType(m_logEntry.GetMobDesc());
	m_type.getNonpaxCount().init();

	return m_type;
}

bool PaxVehicleEntryInfo::isTaxi() const
{
	if(pVehicleAssignEntry)
	{
		if(pVehicleAssignEntry->getType()== LandsideVehicleAssignEntry::AssignType_Public)
		{
			if(!pVehicleAssignEntry->isScheduled())
			{
				return true;
			}
		}
	}
	return false;
}

bool PaxVehicleEntryInfo::isPrivate() const
{
	if(pVehicleAssignEntry)
	{
		if(pVehicleAssignEntry->getType()== LandsideVehicleAssignEntry::AssignType_Individual)
		{
			return true;
		}
	}
	return false;
}

bool PaxVehicleEntryInfo::isScheduleBus() const
{
	if(pVehicleAssignEntry)
	{
		if(pVehicleAssignEntry->getType()== LandsideVehicleAssignEntry::AssignType_Public)
		{
			if(pVehicleAssignEntry->isScheduled())
			{
				return true;
			}
		}
	}
	return false;
}

void PaxVehicleEntryInfo::AddPaxEntry( const MobLogEntry& mobEntry )
{
	vPaxEntryList.push_back(mobEntry);
}

void PaxVehicleEntryInfo::setVehicleAssignEntry( LandsideVehicleAssignEntry* p )
{
	pVehicleAssignEntry = p;
}

LandsideVehicleAssignEntry* PaxVehicleEntryInfo::getVehicleAssignEntry() const
{
	return pVehicleAssignEntry;
}

PaxVehicleEntryInfo::PaxVehicleEntryInfo()
{
	pVehicleAssignEntry = NULL;
}

int PaxVehicleEntryInfo::getPaxCountInEntryList() const
{
	int nTotalPaxCount = 0;
	std::vector<MobLogEntry>::const_iterator iter = vPaxEntryList.begin();
	for (; iter != vPaxEntryList.end(); ++ iter)
	{
		const MobLogEntry& paxEntry = *iter;
		nTotalPaxCount += paxEntry.getGroupSize();
	}

	return nTotalPaxCount;
}

bool PaxVehicleEntryInfo::IsVacant( int nCount ) const
{
	int nCurCount = getPaxCountInEntryList();

	ASSERT(m_pVehicleProperty != NULL);
	if(m_pVehicleProperty && m_pVehicleProperty->GetCapacity() < nCurCount + nCount)
		return false;

	return true;
}

LandsideVehicleProperty* PaxVehicleEntryInfo::getVehicleProperty() const
{
	return m_pVehicleProperty;
}

void PaxVehicleEntryInfo::setVehicleProperty( LandsideVehicleProperty* pVehicleProp )
{
	m_pVehicleProperty = pVehicleProp;
}


