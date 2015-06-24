#include "StdAfx.h"
#include ".\landsideresidentpaxdirectionmanager.h"
#include "Landside\ResidentVehiclTypeContainer.h"
#include "LandsideResourceManager.h"
#include "LandsidePaxTypeLinkageInSimManager.h"
#include "LandsideBusStationInSim.h"
#include "LandsideParkingLotInSim.h"




LandsideResidentPaxDirectionManager::LandsideResidentPaxDirectionManager(void)
{
}

LandsideResidentPaxDirectionManager::~LandsideResidentPaxDirectionManager(void)
{
}

void LandsideResidentPaxDirectionManager::initialize( ResidentRelatedVehiclTypeContainer* plstResidentFlow, LandsideResourceManager* pResourceManager )
{
	if (plstResidentFlow == NULL)
		return ;

	int nCount =  plstResidentFlow->GetItemCount();

	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		ResidenRelatedtVehicleTypeData*  pVehicleTypeData = plstResidentFlow->GetItem(nItem);
		if(pVehicleTypeData)
		{
			ProcessVehicleType(pVehicleTypeData, pResourceManager);

		}
	}

}
void LandsideResidentPaxDirectionManager::ProcessVehicleType( ResidenRelatedtVehicleTypeData*  pVehicleTypeData, LandsideResourceManager* pResourceManager )
{
	int nCount = pVehicleTypeData->GetCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		ResidentRelatedVehicleTypePlan* pVehicleTypePlan = pVehicleTypeData->GetItem(nItem);
		if(pVehicleTypeData)
		{
			ProcessVehicleTypePlan(pVehicleTypePlan, pResourceManager);
		}
	}

}
void LandsideResidentPaxDirectionManager::ProcessVehicleTypePlan(ResidentRelatedVehicleTypePlan* pVehicleTypePlan, LandsideResourceManager* pResourceManager )
{ 
//	int nCount =  pVehicleTypePlan->GetCount();
	ResidentVehicleRouteFlow* pVehicleRouteFlow = pVehicleTypePlan->GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;

	ResidentVehicleRouteList* pRotueList = pVehicleTypePlan->GetStartRoute();
	if (pRotueList == NULL)
		return;
	
	int nCount = pRotueList->GetDestCount();
	std::vector<LandsideFacilityObject> vFacilityObject;
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		ResidentVehicleRoute* pRoute = pRotueList->GetDestObjectInfo(nItem);
		if(pRoute == NULL)
			continue;
	
		std::vector<LandsideLayoutObjectInSim *> vBusStationDirections;
		ProcessRouteData(pRoute,pVehicleRouteFlow, pResourceManager, vBusStationDirections,vFacilityObject, pVehicleTypePlan);

		//add directions
		//generate lead to structure
		LandsidePaxTypeLinkageTimeItemInSim sampleTimeLeadTo;
		sampleTimeLeadTo.setTimeRange(pVehicleTypePlan->GetTimeRange());
		int nStationCount = static_cast<int>(vBusStationDirections.size());
		
		
		int nStation = 0;
		for (; nStation < nStationCount; ++ nStation )
		{
			LandsideLayoutObjectInSim *pBusSation = vBusStationDirections[nStation];//it could be parking lot or bus station
			if(pBusSation == NULL)
				continue;
			
			sampleTimeLeadTo.AddObject(pBusSation->getInput()->getName());
		}

		//put the direction to each object
		nStation = 0;
		for (; nStation < nStationCount; ++ nStation )
		{
			LandsideLayoutObjectInSim *pBusSation = vBusStationDirections[nStation];//it could be parking lot or bus station
			if(pBusSation == NULL)
				continue;
		
			LandsidePaxTypeLinkageTimeItemInSim* pTimeLeadTo = new LandsidePaxTypeLinkageTimeItemInSim(sampleTimeLeadTo);
			pBusSation->getPaxLeadToLinkage()->AddItem(pTimeLeadTo);

		}
	}
}

void LandsideResidentPaxDirectionManager::ProcessRouteData(ResidentVehicleRoute *pOriginalRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow,
														   LandsideResourceManager* pResourceManager, 
														   std::vector<LandsideLayoutObjectInSim *>& vBusStationDirections,
														   std::vector<LandsideFacilityObject> vFacilityObject,
														   ResidentRelatedVehicleTypePlan* pVehicleTypePlan)
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
	if ( enumType == LandsideFacilityObject::Landside_BusStation_Facility )
	{
		int nObjectID = pObject->GetFacilityID();
		if(nObjectID >= 0)
		{
			LandsideBusStationInSim* pBusStationInSim = pResourceManager->getBusStationByID(nObjectID);
			if(pBusStationInSim)
			{
				//add the direction in the list
				vBusStationDirections.push_back(pBusStationInSim);
				if(pBusStationInSim->getEmbeddedParkingLotInSim())
					vBusStationDirections.push_back(pBusStationInSim->getEmbeddedParkingLotInSim());
			}
		}
	}
	else if(enumType == LandsideFacilityObject::Landside_ServiceStart_Facility)
	{
		LandsideFacilityObject* pFacility = pVehicleTypePlan->GetServiceStartObject();
		if(pFacility != NULL)
		{
			LandsideBusStationInSim* pBusStationInSim = pResourceManager->getBusStationByID(pFacility->GetFacilityID());
			if(pBusStationInSim)
			{
				//add the direction in the list
				vBusStationDirections.push_back(pBusStationInSim);
				if(pBusStationInSim->getEmbeddedParkingLotInSim())
					vBusStationDirections.push_back(pBusStationInSim->getEmbeddedParkingLotInSim());
			}			
		}
	}

	//its children
	//its children
	ResidentVehicleRouteList* pChildRouteList = pVehicleRouteFlow->GetDestVehicleRoute(*pOriginalRoute->GetFacilityObject());
	if (pChildRouteList == NULL)
		return;

	vFacilityObject.push_back(*pObject);
	int nItemCount = pChildRouteList->GetDestCount();
	for ( int nItem = 0; nItem < nItemCount; ++ nItem)
	{
		ResidentVehicleRoute* pItem = pChildRouteList->GetDestObjectInfo(nItem);
		if(pItem == NULL)
			continue;
		ProcessRouteData(pItem, pVehicleRouteFlow,pResourceManager, vBusStationDirections,vFacilityObject, pVehicleTypePlan);

	}
	vFacilityObject.pop_back();
}








