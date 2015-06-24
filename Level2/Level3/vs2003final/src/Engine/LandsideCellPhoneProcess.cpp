#include "StdAfx.h"
#include ".\landsidecellphoneprocess.h"
#include "ARCportEngine.h"
#include "landside/LandsidePaxVehicleInSim.h"
#include "../Landside/LandsideResidentVehilceRoute.h"
#include "LandsideSimulation.h"
#include "LandsideResourceManager.h"
#include "../Landside/LandsideParkingLot.h"
#include "./LandsideVehicleLeadToInSimManager.h" 
#include "LandsideBaseBehavior.h"

LandsideCellPhoneProcess::LandsideCellPhoneProcess(LandsidePaxVehicleInSim* pVehicle,ResidentVehicleRouteList* pRoute)
:m_pVehicleInSim(pVehicle)
,m_pVehicleRoute(pRoute)
{
}

LandsideCellPhoneProcess::~LandsideCellPhoneProcess(void)
{
}

ResidentVehicleRoute* LandsideCellPhoneProcess::GetNextVehicleRoute(CARCportEngine* pEngine,ResidentVehicleRouteFlow* pFlow)
{
	LandsideSimulation* m_pLandsideSim = pEngine->GetLandsideSimulation();
	LandsideVehicleLeadToInSimManager* pLeadTo = m_pLandsideSim->GetLandsideLeadToManager();
	ResidentVehicleRoute* pRoute = GetCellPhoneParkingLot(pEngine);
	if (pRoute)
	{
		m_UnAvaibelRouteItems.push_back(pRoute);
		//ALTObjectID srcAltID;
		//if (GetRouteObjectName(pEngine,m_pVehicleRoute->GetFacilityObject().GetFacilityID(),srcAltID))
		//{
			//ALTObjectID desAltID;
			////if (GetRouteObjectName(pEngine,pRoute->GetFacilityObject()->GetFacilityID(),desAltID))
			//{
				//PaxVehicleEntryInfo paxInfo = m_pVehicleInSim->getEntryInfo();
				//CMobileElemConstraint mobCon;
				//mobCon.initMobType(paxInfo.vPaxEntryList.begin()->GetMobDesc());
				//if(pLeadTo->CanLeadTo(mobCon,m_pVehicleInSim->getName(),srcAltID,desAltID))
				//{
					if(!m_pVehicleInSim->CellPhoneCall(pEngine))
					{
						return pRoute;
					}
				//}
			//}
		//}
	}

	//need more considering
	PaxVehicleEntryInfo paxInfo = m_pVehicleInSim->getEntryInfo();
	if(!paxInfo.vPaxEntryList.empty())
	{	
		int paxid = paxInfo.vPaxEntryList.front().getID();
		if( MobileElement* pMob = pEngine->m_simBobileelemList.GetAvailableElement(paxid) )
		{
			if(MobElementBehavior* pBeh =  pMob->getCurrentBehavior() )
			{
				if(pBeh->getBehaviorType() == MobElementBehavior::LandsideBehavior)
				{
					LandsideBaseBehavior* pLandBeh = (LandsideBaseBehavior*)pBeh;
					if(pLandBeh->getResource() && pLandBeh->getState()==WaitingForVehicle)
					{
						LandsideResourceInSim* pAtRes = pLandBeh->getResource();
						int layoutID = pAtRes->getLayoutObject()->getID();
						if(ResidentVehicleRoute* pNext =  pFlow->getChildLeadtoObject(layoutID,m_pVehicleRoute))
						{
							return pNext;
						}
					}
				}
			}
		}

	}	

	
	return m_pVehicleRoute->GetChildRouteRandom(&m_UnAvaibelRouteItems);
}

ResidentVehicleRoute* LandsideCellPhoneProcess::GetCellPhoneParkingLot(CARCportEngine* pEngine) const
{
	for (int i = 0; i < m_pVehicleRoute->GetDestCount(); i++)
	{
		ResidentVehicleRoute* pRoute = m_pVehicleRoute->GetDestObjectInfo(i);
		if (pRoute == NULL)
			continue;
		
		if (std::find(m_UnAvaibelRouteItems.begin(),m_UnAvaibelRouteItems.end(),pRoute) != m_UnAvaibelRouteItems.end())
			continue;
		
		LandsideFacilityObject* pFacility = pRoute->GetFacilityObject();
		if (pFacility == NULL)
			continue;
		
		if (pFacility->GetType() == LandsideFacilityObject::Landside_Parklot_Facility)
		{
			int nObjectID = pFacility->GetFacilityID();
			LandsideLayoutObjectInSim* pLayoutObj = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(nObjectID);
			if (pLayoutObj->getType() == ALT_LPARKINGLOT)
			{
				LandsideParkingLot* pParkingLot = (LandsideParkingLot*)pLayoutObj->getInput();
				if (pParkingLot->IsCellPhone())
				{
					return pRoute;
				}
			}
		}
	}
	return NULL;
}

void LandsideCellPhoneProcess::UnAvaibleRoute( ResidentVehicleRoute* pRoute )
{
	if(std::find(m_UnAvaibelRouteItems.begin(),m_UnAvaibelRouteItems.end(),pRoute) == m_UnAvaibelRouteItems.end())
	{
		m_UnAvaibelRouteItems.push_back(pRoute);
	}
}

bool LandsideCellPhoneProcess::GetRouteObjectName( CARCportEngine* pEngine,int nFacility,ALTObjectID& objName ) const
{
	LandsideLayoutObjectInSim* pLayoutObjInSim = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(nFacility);
	if (pLayoutObjInSim)
	{
		LandsideFacilityLayoutObject* pLayoutObject = pLayoutObjInSim->getInput();
		objName = pLayoutObject->getName();
		return true;
	}
	return false;
}

LandsideIndividualArrivalVehicleConverger::LandsideIndividualArrivalVehicleConverger(const ALTObjectID& objID)
:m_ConvergerObject(objID)
{

}

LandsideIndividualArrivalVehicleConverger::~LandsideIndividualArrivalVehicleConverger()
{

}

LandsideIndividualArrivalPaxVehicleConvergerManager::LandsideIndividualArrivalPaxVehicleConvergerManager()
{

}

LandsideIndividualArrivalPaxVehicleConvergerManager::~LandsideIndividualArrivalPaxVehicleConvergerManager()
{
	for (size_t i = 0; i < m_vConverger.size(); i++)
	{
		delete m_vConverger[i];
	}
	m_vConverger.clear();
}

void LandsideIndividualArrivalPaxVehicleConvergerManager::InsertConverger( int nPaxID, int nVehicle,LandsideObjectLinkageItemInSim linkObject )
{
	for (size_t i = 0; i < m_vConverger.size(); i++)
	{
		LandsideIndividualArrivalVehicleConverger* pItem = m_vConverger[i];
		if (pItem->m_nPersonID == nPaxID)
		{
			return;
		}
	}

	LandsideIndividualArrivalVehicleConverger* pConverge = new LandsideIndividualArrivalVehicleConverger(linkObject.getObject());
	pConverge->m_nPersonID = nPaxID;
	pConverge->m_nVehicleID = nVehicle;
	pConverge->m_ConvergerObject = linkObject;
	m_vConverger.push_back(pConverge);
}

bool LandsideIndividualArrivalPaxVehicleConvergerManager::GetLinkageObject( int nPaxID,LandsideObjectLinkageItemInSim& objName ) const
{
	for (size_t i = 0; i < m_vConverger.size(); i++)
	{
		LandsideIndividualArrivalVehicleConverger* coverge = m_vConverger[i];
		if (coverge->m_nPersonID == nPaxID)
		{
			objName = coverge->m_ConvergerObject;
			return true;
		}
	}
	return false;
}
