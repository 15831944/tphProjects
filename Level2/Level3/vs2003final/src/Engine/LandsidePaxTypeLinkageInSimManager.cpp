#include "StdAfx.h"
#include ".\landsidepaxtypelinkageinsimmanager.h"
#include "Landside\LandsideResidentRelatedContainer.h"
#include "Landside\LandsideLayoutObject.h"
#include "Inputs\MobileElemConstraint.h"

LandsidePaxTypeLinkageInSimManager::LandsidePaxTypeLinkageInSimManager(void)
{
	m_vPaxTypeLinkage.OwnsElements(1);
}

LandsidePaxTypeLinkageInSimManager::~LandsidePaxTypeLinkageInSimManager(void)
{
	m_vPaxTypeLinkage.clear();
}

void LandsidePaxTypeLinkageInSimManager::Initialize( NonResidentVehicleTypeContainer *pNonResidentFlow, const LandsideFacilityLayoutObjectList& layoutObjectList )
{
	int nCount =  pNonResidentFlow->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		NonResidentVehicleTypeData *pVehicleTypeData = pNonResidentFlow->GetItem(nItem);
		if (pVehicleTypeData == NULL)
			continue;
		
		ProcessPaxTypeData(pVehicleTypeData, layoutObjectList);
		
	}
	
}

void LandsidePaxTypeLinkageInSimManager::ProcessPaxTypeData(NonResidentVehicleTypeData *pVehicleTypeData, const LandsideFacilityLayoutObjectList& layoutObjectList  )
{
	CMobileElemConstraint& paxType = pVehicleTypeData->getPaxType();
	CHierachyName vehicleType;
	vehicleType.fromString(pVehicleTypeData->GetVehicleType());

	LandsidePaxTypeLinkageInSim *pPaxTypeLinkage = getEqualItem(paxType);
	if(pPaxTypeLinkage == NULL)
	{
		pPaxTypeLinkage = new LandsidePaxTypeLinkageInSim(paxType);
		m_vPaxTypeLinkage.addItem(pPaxTypeLinkage);

		
	}
	
	LandsidePaxVehicleLinkageInSim* pPaxVehicleLinkage = getPaxVehicleItem(vehicleType,paxType);
	if (pPaxVehicleLinkage == NULL)
	{
		pPaxVehicleLinkage = new LandsidePaxVehicleLinkageInSim(paxType,vehicleType);
		m_vPaxVehicleLinkage.addItem(pPaxVehicleLinkage);
	}

	int nTimeCount = pVehicleTypeData->GetCount();
	for (int nTime = 0; nTime < nTimeCount; ++ nTime)
	{
		NonResidentVehicleTypePlan* pTimePlan = pVehicleTypeData->GetItem(nTime);
		ProcessTimeData(pTimePlan, pPaxTypeLinkage,layoutObjectList);
		ProcessTimeData(pTimePlan,pPaxVehicleLinkage,layoutObjectList);

	}
}

void LandsidePaxTypeLinkageInSimManager::ProcessTimeData(NonResidentVehicleTypePlan *pOrginalTimePlan, LandsidePaxTypeLinkageInSim *pPaxTypeLinkage, const LandsideFacilityLayoutObjectList& layoutObjectList  )
{
	LandsidePaxTypeLinkageTimeItemInSim *pTimeRangeLinkage = new LandsidePaxTypeLinkageTimeItemInSim;
	pTimeRangeLinkage->setTimeRange(pOrginalTimePlan->GetTimeRange());
	pPaxTypeLinkage->AddItem(pTimeRangeLinkage);

	//int nRouteCount =  pOrginalTimePlan->GetCount();
	ResidentVehicleRouteFlow* pVehicleRouteFlow = pOrginalTimePlan->GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;
	
	ResidentVehicleRouteList* pStartRouteList = pVehicleRouteFlow->GetStartRoute();
	if (pStartRouteList == NULL)
		return;	

	int nRouteCount = pStartRouteList->GetDestCount();
	std::vector<LandsideFacilityObject> vFacilityObject;
	for (int nRoute = 0; nRoute < nRouteCount; ++ nRoute)
	{
		ResidentVehicleRoute* pRoute = pStartRouteList->GetDestObjectInfo(nRoute);
		ProcessRouteData(pRoute,pVehicleRouteFlow,vFacilityObject, pTimeRangeLinkage,layoutObjectList);
	}

}
void LandsidePaxTypeLinkageInSimManager::ProcessRouteData(ResidentVehicleRoute *pOriginalRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow,
														  std::vector<LandsideFacilityObject> vFacilityObject,
														  LandsidePaxTypeLinkageTimeItemInSim *pTimeRangeLinkage, 
														  const LandsideFacilityLayoutObjectList& layoutObjectList )
{

	//the route item itself
	if(pOriginalRoute == NULL)
		return;
	LandsideFacilityObject *pObject = pOriginalRoute->GetFacilityObject();
	if(pObject == NULL)
		return;

	//check whether has circle
	if (std::find(vFacilityObject.begin(),vFacilityObject.end(),*pObject) != vFacilityObject.end())
		return;
	
	vFacilityObject.push_back(*pObject);
	LandsideFacilityObject::FacilityType enumType = pObject->GetType();
	//Landside_Intersection_Facility,
	//	Landside_EntryExit_Facility,
	//	Landside_Roundabout_Facility,
	//	Landside_Turnoffs_Facility,
	//	Landside_CloverLeafs_Facility,
	//	Landside_TollGate_Facility,
	//	Landside_Parklot_Facility,
	//	Landside_Curbside_Facility,
	//	Landside_BusStation_Facility,
	//	Landside_HomeBase_Facility,
	//	Landside_ServiceStart_Facility,
	//	Landside_TaxiQueue_Facility,
	//	Landside_TaxiPool_Facility,
	if (enumType == LandsideFacilityObject::Landside_Curbside_Facility ||
		enumType == LandsideFacilityObject::Landside_Parklot_Facility ||
		enumType == LandsideFacilityObject::Landside_BusStation_Facility ||
		enumType == LandsideFacilityObject::Landside_TaxiQueue_Facility)
	{
		int nObjectID = pObject->GetFacilityID();
		if(nObjectID >= 0)
		{
			const LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByID(nObjectID);
			pTimeRangeLinkage->AddObject(pLandsideObject->getName());
		}
	}

	//its children
	ResidentVehicleRouteList* pChildRouteList = pVehicleRouteFlow->GetDestVehicleRoute(*pOriginalRoute->GetFacilityObject());
	if (pChildRouteList == NULL)
		return;
	
	int nItemCount = pChildRouteList->GetDestCount();
	for ( int nItem = 0; nItem < nItemCount; ++ nItem)
	{
		ResidentVehicleRoute* pItem = pChildRouteList->GetDestObjectInfo(nItem);
		if(pItem == NULL)
			continue;
		ProcessRouteData(pItem, pVehicleRouteFlow,vFacilityObject,pTimeRangeLinkage,layoutObjectList);

	}
	vFacilityObject.pop_back();
}

LandsidePaxTypeLinkageInSim* LandsidePaxTypeLinkageInSimManager::getEqualItem( const CMobileElemConstraint& paxType )
{
	int nCount = m_vPaxTypeLinkage.getCount();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		LandsidePaxTypeLinkageInSim *pItem = m_vPaxTypeLinkage.getItem(nItem);
		if (pItem == NULL)
			continue;
		
		if( *(pItem->getPaxType()) == paxType)
			return pItem;
	}

	return NULL;

}

LandsidePaxTypeLinkageInSim* LandsidePaxTypeLinkageInSimManager::getBestFitItem( const CMobileElemConstraint& paxType )
{
	int nCount = m_vPaxTypeLinkage.getCount();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		LandsidePaxTypeLinkageInSim *pItem = m_vPaxTypeLinkage.getItem(nItem);
		if (pItem == NULL)
			continue;

		if( pItem->getPaxType()->fits(paxType))
			return pItem;
	}

	return NULL;
}

LandsidePaxVehicleLinkageInSim* LandsidePaxTypeLinkageInSimManager::getBestFitItem( const CMobileElemConstraint& paxType, const CHierachyName& vehicleType )
{
	int nCount = m_vPaxVehicleLinkage.getCount();
	for (int i = 0; i < nCount; i++)
	{
		LandsidePaxVehicleLinkageInSim *pItem = m_vPaxVehicleLinkage.getItem(i);
		if (pItem == NULL)
			continue;

		if (pItem->fit(paxType,vehicleType))
			return pItem;
	}
	return NULL;
}

LandsidePaxVehicleLinkageInSim* LandsidePaxTypeLinkageInSimManager::getPaxVehicleItem( const CHierachyName& vehicleType,const CMobileElemConstraint& paxType )
{
	int nCount = m_vPaxVehicleLinkage.getCount();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		LandsidePaxVehicleLinkageInSim *pItem = m_vPaxVehicleLinkage.getItem(nItem);
		if (pItem == NULL)
			continue;

		if( pItem->Equal(paxType,vehicleType))
			return pItem;
	}

	return NULL;
}




//////////////////////////////////////////////////////////////////////////
//LandsidePaxTypeLinkageInSim

LandsidePaxTypeLinkageInSim::LandsidePaxTypeLinkageInSim(const CMobileElemConstraint& paxType)
:m_paxConstrait(paxType)
{

}

LandsidePaxTypeLinkageInSim::LandsidePaxTypeLinkageInSim()
{

}

LandsidePaxTypeLinkageInSim::~LandsidePaxTypeLinkageInSim()
{
	int nCount = static_cast<int>(m_vTimeRangeLinkage.size());
	for(int nItem = 0; nItem < nCount; ++ nItem)
	{
		delete m_vTimeRangeLinkage[nItem];
	}
	m_vTimeRangeLinkage.clear();
}

bool LandsidePaxTypeLinkageInSim::operator<( const LandsidePaxTypeLinkageInSim& _anotherLinkage ) const
{
	return m_paxConstrait < _anotherLinkage.m_paxConstrait;
}

CMobileElemConstraint * LandsidePaxTypeLinkageInSim::getPaxType()
{
	return &m_paxConstrait;
}

bool LandsidePaxTypeLinkageInSim::GetAllObjectCanLeadTo( const ElapsedTime& eTime, std::vector<LandsideObjectLinkageItemInSim>& vAllObject) const
{
	int nCount = static_cast<int>(m_vTimeRangeLinkage.size());
	for(int nItem = 0; nItem < nCount; ++ nItem)
	{
		LandsidePaxTypeLinkageTimeItemInSim * pTimeLinkage = m_vTimeRangeLinkage[nItem];
		if (pTimeLinkage == NULL)
			continue;
		if(!pTimeLinkage->getTimeRange().isTimeInRange(eTime))
			continue;


		const std::vector<LandsideObjectLinkageItemInSim>& vThisObjects=  pTimeLinkage->getObjects();
		if(vThisObjects.size())
		{
			vAllObject.insert(vAllObject.begin(), vThisObjects.begin(), vThisObjects.end());
		}
	}

	return true;
}

bool LandsidePaxTypeLinkageInSim::GetAllObjectCanLeadTo( std::vector<LandsideObjectLinkageItemInSim>& vAllObject ) const
{
	int nCount = static_cast<int>(m_vTimeRangeLinkage.size());
	for(int nItem = 0; nItem < nCount; ++ nItem)
	{
		LandsidePaxTypeLinkageTimeItemInSim * pTimeLinkage = m_vTimeRangeLinkage[nItem];
		if (pTimeLinkage == NULL)
			continue;

		const std::vector<LandsideObjectLinkageItemInSim>& vThisObjects=  pTimeLinkage->getObjects();
		if(vThisObjects.size())
		{
			vAllObject.insert(vAllObject.begin(), vThisObjects.begin(), vThisObjects.end());
		}
	}

	return true;
}

bool LandsidePaxTypeLinkageInSim::CanLeadTo( const ElapsedTime& eTime, const ALTObjectID& altDestination ) const
{
	int nCount = static_cast<int>(m_vTimeRangeLinkage.size());
	for(int nItem = 0; nItem < nCount; ++ nItem)
	{
		LandsidePaxTypeLinkageTimeItemInSim * pTimeLinkage = m_vTimeRangeLinkage[nItem];
		if (pTimeLinkage == NULL)
			continue;
		if(!pTimeLinkage->getTimeRange().isTimeInRange(eTime))
			continue;


		if(pTimeLinkage->CanLeadTo(altDestination))
			return true;
	}
	return false;
}































LandsidePaxTypeLinkageTimeItemInSim::LandsidePaxTypeLinkageTimeItemInSim()
{

}

LandsidePaxTypeLinkageTimeItemInSim::LandsidePaxTypeLinkageTimeItemInSim( LandsidePaxTypeLinkageTimeItemInSim& _other )
{
	m_timeRange = _other.m_timeRange;
	m_vObject = _other.m_vObject;

}

LandsidePaxTypeLinkageTimeItemInSim::~LandsidePaxTypeLinkageTimeItemInSim()
{

}

bool LandsidePaxTypeLinkageTimeItemInSim::CanLeadTo( const ALTObjectID& altDestination ) const
{
	int nCount = m_vObject.size();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		if(m_vObject[nItem].CanLeadTo(altDestination))
			return true;
	}

	return false;

}


LandsidePaxVehicleLinkageInSim::LandsidePaxVehicleLinkageInSim( const CMobileElemConstraint& paxType,const CHierachyName& vehicleType )
:LandsidePaxTypeLinkageInSim(paxType)
,m_vehicleType(vehicleType)
{

}

LandsidePaxVehicleLinkageInSim::~LandsidePaxVehicleLinkageInSim()
{

}

bool LandsidePaxVehicleLinkageInSim::fit( const CMobileElemConstraint& paxType,const CHierachyName& vehicleType )
{
	if (vehicleType.fitIn(m_vehicleType)&& m_paxConstrait.fits(paxType))
	{
		return true;
	}

	return false;
}

bool LandsidePaxVehicleLinkageInSim::operator<( const LandsidePaxVehicleLinkageInSim& _anotherLinkage ) const
{
	if (m_vehicleType < _anotherLinkage.m_vehicleType)
	{
		return true;
	}
	else if (m_vehicleType == _anotherLinkage.m_vehicleType)
	{
		return m_paxConstrait < _anotherLinkage.m_paxConstrait;
	}

	return false;
}

bool LandsidePaxVehicleLinkageInSim::Equal( const CMobileElemConstraint& paxType,const CHierachyName& vehicleType )
{
	if (m_vehicleType == vehicleType&& m_paxConstrait == paxType)
	{
		return true;
	}

	return false;
}
