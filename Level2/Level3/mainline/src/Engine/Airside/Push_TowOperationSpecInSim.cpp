#include "StdAfx.h"
#include ".\push_towoperationspecinsim.h"
#include "AirportResourceManager.h"
#include "IntersectionNodeInSim.h"
#include "TaxiwayResourceManager.h"
#include "RunwayResourceManager.h"
#include "FlightGroundRouteResourceManager.h"
#include "../../InputAirside/TowOperationSpecification.h"
#include "../../InputAirside/TowingRoute.h"
#include "StandInSim.h"
#include "AirsideFlightInSim.h"
#include "../../Common/AirportDatabase.h"
#include "RunwayInSim.h"
#include "StandInSim.h"
#include "AirsideResource.h"
#include "CommonInSim.h"
#include "TowTruckServiceRequest.h"
#include "TaxiToReleasePointInSim.h"
#include "TaxiOutboundRouteInSim.h"
#include "AirportResourceManager.h"
#include "DeiceResourceManager.h"
#include "AirTrafficController.h"
#include "VehicleRouteInSim.h"
#include "VehicleStretchInSim.h"
#include "AirsideVehicleInSim.h"
#include "TaxiwayToPoolRouteFinder.h"
#include "StartPositionInSim.h"
#include "../ARCportEngine.h"
#include "../Airside/AirsideSimulation.h"

Push_TowOperationSpecInSim::Push_TowOperationSpecInSim(int nPrjID,AirportResourceManager* pAirportRes, CAirportDatabase* pAirportDB)
{
	m_pPushTowOperationSpec = new CTowOperationSpecification(nPrjID, pAirportDB);
	m_pPushTowOperationSpec->ReadData();

	m_pTowingRouteList = new CTowingRouteList();
	m_pTowingRouteList->ReadData();
	
	m_pAirportRes = pAirportRes;
	m_pTaxiwayToStretchRouteFinder = pAirportRes->GetTaxiwayToNearestStretchRouteFinder();
}

Push_TowOperationSpecInSim::~Push_TowOperationSpecInSim(void)
{
	delete m_pPushTowOperationSpec;
	m_pPushTowOperationSpec = NULL;

	delete m_pTowingRouteList;
	m_pTowingRouteList = NULL;

}

//TOWOPERATIONTYPE Push_TowOperationSpecInSim::GetStandOperationType(StandInSim* pStand, AirsideFlightInSim* pFlight)
//{
//	CTowOperationSpecFltTypeData* pData = GetFlightTowTruckOperationSpecs(pStand, pFlight);
//	if (pData == NULL)
//		return TOWOPERATIONTYPE_NOOPERATION;
//
//	//if (pFlight->GetTowOffStandType() == FREEUPSTAND)		//if flight parking time exceed max parking time
//	//{
//	//	if (pStand == pFlight->GetIntermediateParking() && pFlight->m_bTowingToDepStand)	//on intermediate stand and need be towing to dep stand
//	//	{
//	//		StandInSim* pDepStand = pFlight->m_pDepParkingStand;
//	//		CReleasePoint* pReleaseData = GetTowTruckOperationSpecs(pStand, pDepStand, pFlight);
//
//	//		if (pReleaseData == NULL)
//	//			return TOWOPERATIONTYPE_NOOPERATION;
//
//	//		//if (pStand == pDepStand)			// departure stand and intermediate stand are same one
//	//		//	return TOWOPERATIONTYPE_NOOPERATION;
//
//	//		//if (pStand->IsPushbackFlight(pFlight))
//	//		//	return TOWOPERATIONTYPE_PUSHANDTOW;
//
//	//		return pData->getOperationType();
//	//	}
//	//	else
//	//	{
//	//		if (pFlight->m_bTowingToIntStand && pFlight->GetIntermediateParking())		//on arrival stand and need be towing to intermediate stand
//	//		{
//	//			CReleasePoint* pReleaseData = GetTowTruckOperationSpecs(pStand, pFlight->GetIntermediateParking(), pFlight);
//
//	//			if (pReleaseData == NULL)
//	//				return TOWOPERATIONTYPE_NOOPERATION;
//
//	//			//if (pStand == pFlight->GetIntermediateParking())		//arrival stand and intermediate stand are same one
//	//			//	return TOWOPERATIONTYPE_NOOPERATION;
//
//	//			//if (pStand->IsPushbackFlight(pFlight))
//	//			//	return TOWOPERATIONTYPE_PUSHANDTOW;
//
//	//			return pData->getOperationType();
//	//		}
//	//	}
//	//}
//
//	//if (pFlight->GetTowOffStandType() == REPOSITIONFORDEP /*&& pStand != pFlight->GetPlanedParkingStand(DEP_PARKING)*/)
//	//{
//	//	StandInSim* pDepStand = pFlight->m_pDepParkingStand;
//	//	CReleasePoint* pReleaseData = GetTowTruckOperationSpecs(pStand, pDepStand, pFlight);
//
//	//	if (pReleaseData == NULL)
//	//		return TOWOPERATIONTYPE_NOOPERATION;
//
//	//	return pData->getOperationType();
//
//	//	//if (pStand->IsPushbackFlight(pFlight))
//	//	//	return TOWOPERATIONTYPE_PUSHANDTOW;
//
//	//	//return TOWOPERATIONTYPE_PULLANDTOW;
//	//}
//
//	//if(!pFlight->IsDeparture())
//	//	return TOWOPERATIONTYPE_NOOPERATION;
//
//	////CTowOperationSpecFltTypeData* pData = GetFlightTowTruckOperationSpecs(pStand, pFlight);
//	////if (pData)
//
//		return pData->getOperationType();
//
//
//	//return TOWOPERATIONTYPE_NOOPERATION;
//}

CTowOperationSpecFltTypeData* Push_TowOperationSpecInSim::GetFlightTowTruckOperationSpecs(StandInSim* pOriginStand, AirsideFlightInSim* pFlight)
{
	int nCount = m_pPushTowOperationSpec->GetItemCount();

	ALTObjectID standName = pOriginStand->GetStandInput()->GetObjectName();
	for (int i = 0; i < nCount; i++)
	{
		CTowOperationSpecStand* pData = m_pPushTowOperationSpec->GetItem(i);

		if (!standName.idFits(pData->GetStandGroup().getName()))
			continue;

		int nFltCount = pData->GetElementCount();

		for (int j = 0; j < nFltCount; j++)
		{
			CTowOperationSpecFltTypeData* pFltData = pData->GetItem(j);
			if (pFlight->fits(pFltData->GetFltConstraint()))
				return pFltData;
		}

	}
	return NULL;
}

CReleasePoint* Push_TowOperationSpecInSim::GetTowTruckOperationSpecs(StandInSim* pOriginStand, StandInSim* pDestStand, AirsideFlightInSim* pFlight)
{
	CTowOperationSpecFltTypeData* pFltData = GetFlightTowTruckOperationSpecs(pOriginStand,pFlight);
	if (pFltData == NULL)
		return NULL;

	int nCount = pFltData->GetElementCount();
	
	ALTObjectID destName = pDestStand->GetStandInput()->GetObjectName();
	for (int i = 0; i < nCount; i++)
	{
		CReleasePoint* pData = pFltData->GetItem(i);
		
		if (pData->GetEnumDestType() != TOWOPERATIONDESTINATION_STAND)
			continue;

		ALTObjectGroup standGroup;
		standGroup.ReadData(pData->GetObjectID());

		if (destName.idFits(standGroup.getName()))
			return pData;
	}
	return NULL;
}

CReleasePoint* Push_TowOperationSpecInSim::GetTowTruckOperationSpecs(StandInSim* pOriginStand, LogicRunwayInSim* pDepRunway, AirsideFlightInSim* pFlight)
{
	CTowOperationSpecFltTypeData* pFltData = GetFlightTowTruckOperationSpecs(pOriginStand,pFlight);
	if (pFltData == NULL)
		return NULL;

	int nCount = pFltData->GetElementCount();

	for (int i = 0; i < nCount; i++)
	{
		CReleasePoint* pData = pFltData->GetItem(i);

		if (pData->GetEnumDestType() != TOWOPERATIONDESTINATION_RUNWAYTHRESHOLD)
			continue;

		if(pDepRunway->GetRunwayInSim()->GetRunwayID() == pData->GetObjectID())
		{
			if (pDepRunway->GetRunwayInSim()->GetRunwayInput()->GetMarking1().c_str() == pData->getName())
				return pData;
		}
	}
	return NULL;
}

CReleasePoint* Push_TowOperationSpecInSim::GetTowTruckOperationSpecs(StandInSim* pOriginStand, TaxiInterruptLineInSim* pInterruptLine, AirsideFlightInSim* pFlight)
{
	CTowOperationSpecFltTypeData* pFltData = GetFlightTowTruckOperationSpecs(pOriginStand,pFlight);
	if (pFltData == NULL)
		return NULL;

	int nCount = pFltData->GetElementCount();

	for (int i = 0; i < nCount; i++)
	{
		CReleasePoint* pData = pFltData->GetItem(i);

		if (pData->GetEnumDestType() != TOWOPERATIONDESTINATION_TAXIWAYHOLDSHORTLINE)
			continue;

		if(pInterruptLine->GetInterruptLineInput()->GetID() == pData->GetObjectID())
		{
			return pData;
		}
	}
	return NULL;
}

CReleasePoint* Push_TowOperationSpecInSim::GetTowTruckOperationSpecs(StandInSim* pOriginStand, DeicePadInSim* pDeicePad, AirsideFlightInSim* pFlight)
{
	CTowOperationSpecFltTypeData* pFltData = GetFlightTowTruckOperationSpecs(pOriginStand,pFlight);
	if (pFltData == NULL)
		return NULL;

	ALTObjectID destName = pDeicePad->GetInput()->GetObjectName();
	int nCount = pFltData->GetElementCount();
	for (int i = 0; i < nCount; i++)
	{
		CReleasePoint* pData = pFltData->GetItem(i);

		if (pData->GetEnumDestType() != TOWOPERATIONDESTINATION_DEICINGPAD)
			continue;

		ALTObjectGroup padGroup;
		padGroup.ReadData(pData->GetObjectID());

		if (destName.idFits(padGroup.getName()))
			return pData;
	}
	return NULL;
}

CReleasePoint* Push_TowOperationSpecInSim::GetTowTruckOperationSpecs( StandInSim* pOriginStand, StartPositionInSim* pStartPos, AirsideFlightInSim* pFlight )
{
	CTowOperationSpecFltTypeData* pFltData = GetFlightTowTruckOperationSpecs(pOriginStand,pFlight);
	if (pFltData == NULL)
		return NULL;

	ALTObjectID destName = pStartPos->GetStartPosition()->GetObjectName();
	int nCount = pFltData->GetElementCount();
	for (int i = 0; i < nCount; i++)
	{
		CReleasePoint* pData = pFltData->GetItem(i);

		if (pData->GetEnumDestType() != TOWOPERATIONDESTINATION_STARTPOSITION)
			continue;

		ALTObjectGroup startPosGroup;
		startPosGroup.ReadData(pData->GetObjectID());

		if (destName.idFits(startPosGroup.getName()))
			return pData;
	}
	return NULL;
}

TowTruckServiceRequest* Push_TowOperationSpecInSim::GetFlightTowingServiceRequest(AirsideFlightInSim* pFlight,AirsideResource* pCurRes/*,TowTruckServiceRequest* pRequest*/)
{
	if (pCurRes->GetType() != AirsideResource::ResType_Stand)
		return NULL;

	StandInSim* pStand = (StandInSim*)pCurRes;
	CTowOperationSpecFltTypeData* pData = GetFlightTowTruckOperationSpecs(pStand, pFlight);
	if (pData == NULL)
		return NULL;
	

	TOWOPERATIONTYPE Optype = pData->getOperationType();

	if (Optype == TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY)	//push back only
	{
		TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
		return pRequest;
	}

	if (pFlight->GetTowOffStandType() == FREEUPSTAND && pFlight->GetIntermediateParking())		//if flight parking time exceed max parking time
	{
		if (pStand == pFlight->GetIntermediateParking() && pFlight->m_bTowingToDepStand)	//from intermediate to departure
		{
			StandInSim* pDest = pFlight->GetPlanedParkingStand(DEP_PARKING);
			if (pDest == NULL || (pDest&& !pDest->GetLock(pFlight)))
			{				
				pFlight->GetAirTrafficController()->AssignOperationParkingStand(pFlight,DEP_PARKING);
				if (pFlight->m_pDepParkingStand == NULL)
				{
					pFlight->SetParkingStand(pStand,DEP_PARKING);
					return NULL;
				}
			}

			CReleasePoint* pReleaseData = GetTowTruckOperationSpecs(pStand, pDest,pFlight);
			if (pReleaseData == NULL)
				return NULL;

			TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
			InitRequestRouteInfo(pReleaseData,pFlight,pStand,pDest,pRequest);

			//else		//user undefined
			//{
			//	IntersectionNodeInSim* pNodeIn = pStand->GetMaxDistOutNode();
			//	IntersectionNodeInSim* pNodeOut = pDest->GetMinDistInNode();

			//	FlightGroundRouteDirectSegList vSegments;
			//	vSegments.clear();
			//	double dPathWeight = (std::numeric_limits<double>::max)();
			//	m_pAirportRes->getGroundRouteResourceManager()->GetRoute(pNodeIn,pNodeOut,pFlight,0,vSegments,dPathWeight);

			//	TaxiRouteInSim* pRoute = new TaxiToReleasePointRouteInSim(OnTowToDestination,pCurRes,pDest);
			//	pRoute->AddTaxiwaySegList(vSegments);

			//	pFlight->SetTowingRoute(pRoute);
			//	pRequest->m_pReleasePoint = pDest;

			//	GetTowtruckReturnRoute(NULL, pFlight,  pDest, pRequest->m_ReturnRoute);
			//}
			return pRequest;
		}

		if (pStand == pFlight->m_pArrParkingStand && pFlight->m_bTowingToIntStand && pFlight->IsArrivingOperation() )	//from arrival to intermediate
		{
			StandInSim* pDest = pFlight->GetIntermediateParking();
			CReleasePoint* pReleaseData = GetTowTruckOperationSpecs(pStand, pDest,pFlight);
			if (pReleaseData == NULL)
				return NULL;

			TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
			InitRequestRouteInfo(pReleaseData,pFlight,pStand,pDest,pRequest);

			//else		//user undefined
			//{
			//	IntersectionNodeInSim* pNodeIn = pStand->GetMaxDistOutNode();
			//	IntersectionNodeInSim* pNodeOut = pDest->GetMinDistInNode();

			//	FlightGroundRouteDirectSegList vSegments;
			//	vSegments.clear();
			//	double dPathWeight = (std::numeric_limits<double>::max)();
			//	m_pAirportRes->getGroundRouteResourceManager()->GetRoute(pNodeIn,pNodeOut,pFlight,0,vSegments,dPathWeight);

			//	TaxiRouteInSim* pRoute = new TaxiToReleasePointRouteInSim(OnTowToDestination,pStand,pDest);
			//	pRoute->AddTaxiwaySegList(vSegments);

			//	pFlight->SetTowingRoute(pRoute);
			//	pRequest->m_pReleasePoint = pDest;

			//	GetTowtruckReturnRoute(NULL, pFlight,  pDest, pRequest->m_ReturnRoute);
			//}
			return pRequest;
		}
	}

	if (pFlight->GetPlanedParkingStand(DEP_PARKING) != pFlight->GetPlanedParkingStand(ARR_PARKING))		//to dep stand
	{
		if (pStand == pFlight->m_pArrParkingStand && pFlight->m_bTowingToDepStand)	//from arr stand to departure
		{
			StandInSim* pDest = pFlight->GetPlanedParkingStand(DEP_PARKING);
			if (pDest == NULL || (pDest&& !pDest->GetLock(pFlight)))
			{				
				pFlight->GetAirTrafficController()->AssignOperationParkingStand(pFlight,DEP_PARKING);
				if (pFlight->m_pDepParkingStand == NULL)
				{
					pFlight->SetParkingStand(pStand,DEP_PARKING);
					return NULL;
				}
			}

			CReleasePoint* pReleaseData = GetTowTruckOperationSpecs(pStand, pDest,pFlight);
			if (pReleaseData == NULL)
				return NULL;

			TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
			InitRequestRouteInfo(pReleaseData,pFlight,pStand,pDest,pRequest);
			//else		//user undefined
			//{
			//	IntersectionNodeInSim* pNodeIn = pStand->GetMaxDistOutNode();
			//	IntersectionNodeInSim* pNodeOut = pDest->GetMinDistInNode();

			//	FlightGroundRouteDirectSegList vSegments;
			//	vSegments.clear();
			//	double dPathWeight = (std::numeric_limits<double>::max)();
			//	m_pAirportRes->getGroundRouteResourceManager()->GetRoute(pNodeIn,pNodeOut,pFlight,0,vSegments,dPathWeight);

			//	TaxiRouteInSim* pRoute = new TaxiToReleasePointRouteInSim(OnTowToDestination,pStand,pDest);
			//	pRoute->AddTaxiwaySegList(vSegments);

			//	pFlight->SetTowingRoute(pRoute);
			//	pRequest->m_pReleasePoint = pDest;
			//}
			return pRequest;
		}
	}

	LogicRunwayInSim* pLogicRunway = pFlight->GetAndAssignTakeoffRunway();
	if (pLogicRunway)
	{
		//check deicing pad
		AirsideResource* pResource = pFlight->GetDeiceDecision().m_deicePlace;
		if (pResource && pResource->GetType() == AirsideResource::ResType_DeiceStation)
		{
			DeicePadInSim* pDeicePad = (DeicePadInSim*)pResource;
			CReleasePoint* pReleaseData = GetTowTruckOperationSpecs(pStand, pDeicePad,pFlight);	
			if (pReleaseData)	//need deicing service	
			{
				TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
				InitRequestRouteInfo(pReleaseData,pFlight,pStand,pDeicePad,pRequest);
				return pRequest;
			}
		}

		CReleasePoint* pReleaseData = GetTowTruckOperationSpecs(pStand, pLogicRunway,pFlight);
		if (pReleaseData)		//if defined the runway as release point
		{
			TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
			InitRequestRouteInfo(pReleaseData,pFlight,pStand,pLogicRunway,pRequest);
			return pRequest;
		}
		else			//Interrupt line or start position
		{
			size_t nCount = pData->GetElementCount();
			for (size_t i = 0; i < nCount; i++)
			{
				pReleaseData = pData->GetItem(i);
				TOWOPERATIONDESTINATION eDestType = pReleaseData->GetEnumDestType();
				if (eDestType != TOWOPERATIONDESTINATION_STARTPOSITION && eDestType != TOWOPERATIONDESTINATION_TAXIWAYHOLDSHORTLINE)
					continue;

				if (eDestType ==  TOWOPERATIONDESTINATION_TAXIWAYHOLDSHORTLINE)
				{
					TaxiInterruptLineInSim* pLine = m_pAirportRes->GetTaxiInterruptLineList().GetHoldShortLineById(pReleaseData->GetObjectID());
					if (pLine == NULL)
						continue;

					TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
					InitRequestRouteInfo(pReleaseData,pFlight,pStand,pLine,pRequest);
					return pRequest;
				}
				else
				{
					ALTObjectGroup altGroup;
					altGroup.ReadData(pReleaseData->GetObjectID());

					//std::vector<ALTObject> vObj;
					//altGroup.GetObjects(vObj);

					//if (vObj.empty())
					//	continue;

					//ALTObject obj = vObj.at(0);
					std::vector<StartPositionInSim*> vStartPos;
					m_pAirportRes->GetStartPosition(altGroup.getName(), vStartPos);
					if (vStartPos.empty())
						continue;

					TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
					InitRequestRouteInfo(pReleaseData,pFlight,pStand,vStartPos.at(0),pRequest);
					return pRequest;
				}

			}
			//TaxiRouteInSim* pOutboundRoute = pFlight->GetOutBoundRoute();

			//std::vector<StartPositionInSim*> vStartPositions;
			//vStartPositions.clear();
			//pOutboundRoute->GetTaxiStartPositionsInRoute(vStartPositions);

			//if (!vStartPositions.empty())
			//{
			//	size_t nCount = vStartPositions.size();
			//	for (size_t i =0; i < nCount; i++)
			//	{
			//		StartPositionInSim* pStartPos = vStartPositions.at(i);
			//		pReleaseData = GetTowTruckOperationSpecs(pStand, pStartPos,pFlight);
			//		if (pReleaseData == NULL)
			//			continue;

			//		TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
			//		InitRequestRouteInfo(pReleaseData,pFlight,pStand,pStartPos,pRequest);
			//		return pRequest;
			//	}
			//}

			//std::vector<TaxiInterruptLineInSim*> vInterruptLines;
			//vInterruptLines.clear();
			//pOutboundRoute->GetTaxiInterruptLinesInRoute(vInterruptLines);

			//if (!vInterruptLines.empty())
			//{
			//	int nCount = vInterruptLines.size();
			//	for (int i =0; i < nCount; i++)
			//	{
			//		TaxiInterruptLineInSim* pInterruptLine = vInterruptLines.at(i);
			//		pReleaseData = GetTowTruckOperationSpecs(pStand, pInterruptLine,pFlight);
			//		if (pReleaseData == NULL)
			//			continue;

			//		TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, pFlight);
			//		InitRequestRouteInfo(pReleaseData,pFlight,pStand,pInterruptLine,pRequest);
			//		return pRequest;
			//	}
			//}
		}

	}

	return NULL;
	//if (pRequest->m_ServiceType == TOWOPERATIONTYPE_PUSHANDTOW)		//no defined release point, only push back
	//	pRequest->m_ServiceType = TOWOPERATIONTYPE_PUSHBACKONLY;
}

void Push_TowOperationSpecInSim::InitRequestRouteInfo(CReleasePoint* pData,AirsideFlightInSim* pFlight, StandInSim* pOrigin,AirsideResource* pDest, TowTruckServiceRequest* pRequest)
{	
	//outbound route
	FlightGroundRouteDirectSegList Taxisegs;
	GetTowingRoute(pData,pFlight,pOrigin,pDest,Taxisegs);

	TaxiToReleasePointRouteInSim* pRoute = new TaxiToReleasePointRouteInSim(OnTowToDestination,pOrigin,pDest);
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pRoute->AddTaxiwaySegList(Taxisegs,bCyclicGroundRoute);

	if (pDest->GetType() == AirsideResource::ResType_InterruptLine)
	{
		int nNodeId = ((TaxiInterruptLineInSim*)pDest)->GetInterruptLineInput()->GetFrontIntersectionNodeID();
		IntersectionNodeInSim* pNodeOut = m_pAirportRes->GetIntersectionNodeList().GetNodeByID(nNodeId);
		int nNodeId2 = ((TaxiInterruptLineInSim*)pDest)->GetInterruptLineInput()->GetBackIntersectionNodeID();
		IntersectionNodeInSim* pNodeOut2 = m_pAirportRes->GetIntersectionNodeList().GetNodeByID(nNodeId2);

		TaxiwaySegInSim* pSeg = m_pAirportRes->getTaxiwayResource()->GetTaxiwaySegment(pNodeOut,pNodeOut2);

		int nCount = Taxisegs.size();
		if (nCount >0)
		{
			TaxiwayDirectSegInSim* pDirSeg = (TaxiwayDirectSegInSim*)(Taxisegs.at(nCount -1));
			if (pDirSeg->GetEntryNode() == pNodeOut2)		//the last segment in route is the segment which has the interrupt line
			{
				int nNum = pRoute->GetItemCount();
				TaxiRouteItemInSim& pItem = pRoute->ItemAt(nNum -1);
				pItem.m_distFrom =0;
				pItem.m_distTo -= ((TaxiInterruptLineInSim*)pDest)->GetInterruptLineInput()->GetDistToFrontIntersectionNode();
			}
			else
			{
				TaxiwayDirectSegInSim* pDirSeg = NULL;
				if (pSeg->GetPositiveDirSeg()->GetEntryNode() == pNodeOut)
					pDirSeg = pSeg->GetPositiveDirSeg();
				else
					pDirSeg = pSeg->GetNegativeDirSeg();

				TaxiRouteItemInSim routeItem(pDirSeg, 0, ((TaxiInterruptLineInSim*)pDest)->GetInterruptLineInput()->GetDistToFrontIntersectionNode());
				pRoute->AddItem(routeItem);
			}		
		}
	}	

	if (pDest->GetType() == AirsideResource::ResType_StartPosition)
	{
		TaxiwaySegInSim* pSeg = ((StartPositionInSim*)pDest)->GetAttachedTaxiwaySegment();
		double dDistToNode1 = pSeg->GetPositiveDirSeg()->GetPath().GetPointDist(((StartPositionInSim*)pDest)->GetPosition());
		int nCount = Taxisegs.size();
		if (nCount >0)
		{
			TaxiwayDirectSegInSim* pDirSeg = (TaxiwayDirectSegInSim*)(Taxisegs.at(nCount -1));
			if (pDirSeg->GetEntryNode() == pSeg->GetNode2())		//the last segment in route is the segment which has the start position
			{
				int nNum = pRoute->GetItemCount();
				TaxiRouteItemInSim& pItem = pRoute->ItemAt(nNum -1);
				pItem.m_distFrom =0;
				pItem.m_distTo -= dDistToNode1;
			}
			else
			{
				TaxiwayDirectSegInSim* pDirSeg = pSeg->GetPositiveDirSeg();

				TaxiRouteItemInSim routeItem(pDirSeg, 0, dDistToNode1);
				pRoute->AddItem(routeItem);
			}		
		}
		else
		{
			IntersectionNodeInSim* pNodeIn = pOrigin->GetMaxDistOutNode();
			IntersectionNodeInSim* pNodeOut = ((StartPositionInSim*)pDest)->GetAttachedTaxiwaySegment()->GetNode1();
			if (pNodeIn == pNodeOut)
			{
				TaxiwayDirectSegInSim* pDirSeg = pSeg->GetPositiveDirSeg();

				TaxiRouteItemInSim routeItem(pDirSeg, 0, dDistToNode1);
				pRoute->AddItem(routeItem);
			}
		}
	}
	pFlight->SetTowingRoute(pRoute);
	pRequest->m_pReleasePoint = pDest;

	//plan route to runway	
	FlightGroundRouteDirectSegInSim* pLastSeg = pRoute->getLastSeg();
	if(pRequest->m_ServiceType == TOWOPERATIONTYPE_PUSHBACKTORELEASEPOINT)
		pLastSeg = pLastSeg->GetOppositeSegment();
	FlightGroundRouteDirectSegList vseg;
	pFlight->GetAirTrafficController()->getOutBoundSeg(pDest,pLastSeg,pFlight,vseg);
	pRoute->SetPostRoute(vseg);


	//return route
	GetTowtruckReturnRoute(pData, pFlight,  pDest, pRequest);

}

void Push_TowOperationSpecInSim::GetTowingRoute(CReleasePoint* pData,AirsideFlightInSim* pFlight, StandInSim* pOrigin,AirsideResource* pDest, FlightGroundRouteDirectSegList& vSegments)
{
	IntersectionNodeInSim* pNodeIn = pOrigin->GetMaxDistOutNode();
	IntersectionNodeInSim * pNodeOut = NULL;

	if (pDest->GetType() == AirsideResource::ResType_Stand)
		pNodeOut = ((StandInSim*)pDest)->GetMinDistInNode();
	else if (pDest->GetType() == AirsideResource::ResType_DeiceStation)
		pNodeOut = ((DeicePadInSim*)pDest)->m_pEntryNode;
	else if (pDest->GetType() == AirsideResource::ResType_LogicRunway)
		pNodeOut = pFlight->GetAndAssignTakeoffPosition()->GetRouteSeg()->GetEntryNode();
	else if (pDest->GetType() == AirsideResource::ResType_InterruptLine)
	{
		int nNodeId = ((TaxiInterruptLineInSim*)pDest)->GetInterruptLineInput()->GetFrontIntersectionNodeID();
		pNodeOut = m_pAirportRes->GetIntersectionNodeList().GetNodeByID(nNodeId);
	}
	else if (pDest->GetType() == AirsideResource::ResType_StartPosition)
	{
		pNodeOut = ((StartPositionInSim*)pDest)->GetAttachedTaxiwaySegment()->GetNode1();
	}

	CTowOperationRoute* pTowRoute = pData->GetOutBoundRoute();

	if (pTowRoute)
	{
		IntersectionNodeInSim* pStartNode = NULL;
		IntersectionNodeInSim* pEndNode = NULL;

		FlightGroundRouteDirectSegList vRouteItems;
		vRouteItems.clear();
		GetRouteItemList(pTowRoute,vRouteItems);

		int nSize = vRouteItems.size();
		if (nSize >0)
		{
			pStartNode = vRouteItems[0]->GetEntryNode();		
			pEndNode = vRouteItems[nSize -1]->GetExitNode();
		}

		FlightGroundRouteDirectSegList vItems;
		if (pStartNode)
		{
			vItems.clear();
			double dPathWeight = (std::numeric_limits<double>::max)();
			m_pAirportRes->getGroundRouteResourceManager()->GetRoute(pNodeIn,pStartNode,pFlight,0,vItems,dPathWeight);
			if (!vItems.empty())
				vSegments.insert(vSegments.end(),vItems.begin(),vItems.end());
		}

		vSegments.insert(vSegments.end(),vRouteItems.begin(),vRouteItems.end());

		if (pEndNode)
		{
			vItems.clear();
			double dPathWeight = (std::numeric_limits<double>::max)();
			m_pAirportRes->getGroundRouteResourceManager()->GetRoute(pEndNode,pNodeOut,pFlight,0,vItems,dPathWeight);
			if (!vItems.empty())
				vSegments.insert(vSegments.end(),vItems.begin(),vItems.end());
		}

		return;
	}

	//no proper route, find shortest path
	double dPathWeight = (std::numeric_limits<double>::max)();
	m_pAirportRes->getGroundRouteResourceManager()->GetRoute(pNodeIn,pNodeOut,pFlight,0,vSegments,dPathWeight);


}

IntersectionNodeInSim* Push_TowOperationSpecInSim::GetReturnRouteItemList( CTowOperationRoute* pRoute, FlightGroundRouteDirectSegList& vRouteItems )
{
	ASSERT(pRoute);

	if (pRoute == NULL)
		return NULL;

	int nBranchCount = pRoute->GetElementCount();
	if (nBranchCount <=0)
		return NULL;

	int nRandomIdx = 0;
	if (nBranchCount > 1)
		nRandomIdx = rand()%nBranchCount;

	CTowOperationRouteItem* pFirstItem = pRoute->GetItem(nRandomIdx);

	int nFirstIntersectionID = -1;
	int nSecondIntersectionID = -1;
	int nLastObjectID = -1;

	std::vector<int>  vIntersectionID;
	while(pFirstItem)
	{
		if (pFirstItem->GetEnumType() ==CTowOperationRouteItem::ItemType_TowRoute)
		{
			FlightGroundRouteDirectSegList vSegments;
			vSegments.clear();
			CTowingRoute* pTowRoute = m_pTowingRouteList->GetTowingRouteByID(pFirstItem->GetObjectID());
			GetRouteItemsFromTowingRoute(pTowRoute,nFirstIntersectionID,vSegments, nLastObjectID);
			vRouteItems.insert(vRouteItems.end(),vSegments.begin(),vSegments.end());
			int nSize =vSegments.size();
			if (nSize >0)
				nFirstIntersectionID = vSegments[nSize -1]->GetExitNode()->GetID();
		}
		else
		{
			nFirstIntersectionID = pFirstItem->GetIntersectionID();
		}

		if (nFirstIntersectionID >= 0)
		{
			vIntersectionID.push_back(nFirstIntersectionID);
		}

		nBranchCount = pFirstItem->GetElementCount();
		if (nBranchCount <= 0)
		{
			if (nFirstIntersectionID >= 0 && vRouteItems.empty())	//only defined an entry node
			{
				return m_pAirportRes->GetIntersectionNodeList().GetNodeByID(nFirstIntersectionID);
			}

			if (nFirstIntersectionID < 0 && vRouteItems.empty())
			{
				if (vIntersectionID.empty() == false)
				{
					int nItersectionNodeID = vIntersectionID.back();
					return m_pAirportRes->GetIntersectionNodeList().GetNodeByID(nItersectionNodeID);
				}
			}
			return NULL;
		}

		if (nBranchCount > 1)
			nRandomIdx = rand()%nBranchCount;
		else
			nRandomIdx = 0;

		CTowOperationRouteItem* pNextItem = pFirstItem->GetItem(nRandomIdx);
		if (pNextItem->GetObjectID() == nLastObjectID)		//same taxiway with last item
			continue;

		nSecondIntersectionID = pNextItem->GetIntersectionID();

		FlightGroundRouteDirectSegList vSegmentList;
		vSegmentList.clear();
		if(pFirstItem->GetEnumType() ==CTowOperationRouteItem::ItemType_Runway)
			m_pAirportRes->getRunwayResource()->GetRunwaySegment(nLastObjectID,nFirstIntersectionID,nSecondIntersectionID,vSegmentList);
		else
			m_pAirportRes->getTaxiwayResource()->GetTaxiwaySegment(nLastObjectID,nFirstIntersectionID,nSecondIntersectionID,vSegmentList);

		vRouteItems.insert(vRouteItems.end(),vSegmentList.begin(),vSegmentList.end());

		pFirstItem = pNextItem;
		nLastObjectID = pFirstItem->GetObjectID();

	}
	return NULL;
}

IntersectionNodeInSim* Push_TowOperationSpecInSim::GetRouteItemList(CTowOperationRoute* pRoute, FlightGroundRouteDirectSegList& vRouteItems)
{
	ASSERT(pRoute);

	if (pRoute == NULL)
		return NULL;

	int nBranchCount = pRoute->GetElementCount();
	if (nBranchCount <=0)
		return NULL;

	int nRandomIdx = 0;
	if (nBranchCount > 1)
		nRandomIdx = rand()%nBranchCount;

	CTowOperationRouteItem* pFirstItem = pRoute->GetItem(nRandomIdx);

	int nFirstIntersectionID = -1;
	int nSecondIntersectionID = -1;
	int nLastObjectID = -1;

	while(pFirstItem)
	{
		if (pFirstItem->GetEnumType() ==CTowOperationRouteItem::ItemType_TowRoute)
		{
			FlightGroundRouteDirectSegList vSegments;
			vSegments.clear();
			CTowingRoute* pTowRoute = m_pTowingRouteList->GetTowingRouteByID(pFirstItem->GetObjectID());
			GetRouteItemsFromTowingRoute(pTowRoute,nFirstIntersectionID,vSegments, nLastObjectID);
			vRouteItems.insert(vRouteItems.end(),vSegments.begin(),vSegments.end());
			int nSize =vSegments.size();
			if (nSize >0)
				nFirstIntersectionID = vSegments[nSize -1]->GetExitNode()->GetID();
		}
		else
		{
			nFirstIntersectionID = pFirstItem->GetIntersectionID();
		}
		nBranchCount = pFirstItem->GetElementCount();
		if (nBranchCount <= 0)
		{
			if (nFirstIntersectionID >= 0 && vRouteItems.empty())	//only defined an entry node
			{
				return m_pAirportRes->GetIntersectionNodeList().GetNodeByID(nFirstIntersectionID);;
			}

			if (nFirstIntersectionID < 0 && vRouteItems.empty())
			{

			}
			return NULL;
		}

		if (nBranchCount > 1)
			nRandomIdx = rand()%nBranchCount;
		else
			nRandomIdx = 0;

		CTowOperationRouteItem* pNextItem = pFirstItem->GetItem(nRandomIdx);
		if (pNextItem->GetObjectID() == nLastObjectID)		//same taxiway with last item
			continue;

		nSecondIntersectionID = pNextItem->GetIntersectionID();

		FlightGroundRouteDirectSegList vSegmentList;
		vSegmentList.clear();
		if(pFirstItem->GetEnumType() ==CTowOperationRouteItem::ItemType_Runway)
			m_pAirportRes->getRunwayResource()->GetRunwaySegment(nLastObjectID,nFirstIntersectionID,nSecondIntersectionID,vSegmentList);
		else
			m_pAirportRes->getTaxiwayResource()->GetTaxiwaySegment(nLastObjectID,nFirstIntersectionID,nSecondIntersectionID,vSegmentList);

		vRouteItems.insert(vRouteItems.end(),vSegmentList.begin(),vSegmentList.end());

		pFirstItem = pNextItem;
		nLastObjectID = pFirstItem->GetObjectID();

	}
	return NULL;
}

void Push_TowOperationSpecInSim::GetRouteItemsFromTowingRoute(CTowingRoute* pGroundRoute, int nIntersectionID, FlightGroundRouteDirectSegList& vRouteItems, int& nLastTaxiwayID)
{
	ASSERT(pGroundRoute);

	if (pGroundRoute == NULL)
		return;

	CTaxiwayNode* pFirstItem = pGroundRoute->GetTaxiwayNodeByParentID(-1);
	int nFirstNodeID = nIntersectionID;

	while(pFirstItem)
	{
		int nFirstTaxiwayID = pFirstItem->GetTaxiwayID();
		CTaxiwayNode* pNextItem = pGroundRoute->GetTaxiwayNodeByParentID(pFirstItem->GetUniqueID());
		if (pNextItem == NULL)	//no child branch
		{
			nLastTaxiwayID = nFirstTaxiwayID;
			return;
		}

		int nNextTaxiwayID = pNextItem->GetTaxiwayID();

		IntersectionNodeInSim* pNode = m_pAirportRes->GetIntersectionNodeList().GetIntersectionNode(nFirstTaxiwayID,nNextTaxiwayID,0);

		ASSERT(pNode);
		if (pNode == NULL)
			return;

		FlightGroundRouteDirectSegList vSegments;
		vSegments.clear();
		m_pAirportRes->getTaxiwayResource()->GetTaxiwaySegment(nFirstTaxiwayID,nFirstNodeID,pNode->GetID(),vSegments);

		vRouteItems.insert(vRouteItems.end(), vSegments.begin(), vSegments.end());

		pFirstItem = pNextItem;
		nFirstNodeID = pNode->GetID();

	}

}

//--------------------------------------------------------------------------------------------
//Summary:
//		Get tow truck finished service to return route
//Parameters:
//		vSegments[out]: tow truck require to stretch path
//		vTowSegments[out]: have no start segment they are the same, other different
//--------------------------------------------------------------------------------------------
void Push_TowOperationSpecInSim::GetReturnRoute(CReleasePoint* pData,AirsideFlightInSim* pFlight,AirsideResource* pOrigin, FlightGroundRouteDirectSegList& vSegments,FlightGroundRouteDirectSegList& vTowSegments)
{
	IntersectionNodeInSim* pNodeIn = NULL;
	IntersectionNodeInSim * pNodeOut = NULL;
	TaxiwayDirectSegInSim* pStartSeg = NULL;

	if (pOrigin->GetType() == AirsideResource::ResType_Stand)
		pNodeIn = ((StandInSim*)pOrigin)->GetMaxDistOutNode();
	else if (pOrigin->GetType() == AirsideResource::ResType_DeiceStation)
		pNodeIn = ((DeicePadInSim*)pOrigin)->m_pExitNode;
	else if (pOrigin->GetType() == AirsideResource::ResType_LogicRunway)
		pNodeIn = pFlight->GetAndAssignTakeoffPosition()->GetRouteSeg()->GetEntryNode();
	else if (pOrigin->GetType() == AirsideResource::ResType_InterruptLine)
	{
		int nNodeId = ((TaxiInterruptLineInSim*)pOrigin)->GetInterruptLineInput()->GetFrontIntersectionNodeID();
		pNodeIn = m_pAirportRes->GetIntersectionNodeList().GetNodeByID(nNodeId);

		int nNodeId2 = ((TaxiInterruptLineInSim*)pOrigin)->GetInterruptLineInput()->GetBackIntersectionNodeID();

		pStartSeg = m_pAirportRes->getTaxiwayResource()->GetTaxiwayDirectSegment(nNodeId2,nNodeId,((TaxiInterruptLineInSim*)pOrigin)->GetInterruptLineInput()->GetTaxiwayID());

	}
	else if (pOrigin->GetType() == AirsideResource::ResType_StartPosition)
	{
		pNodeIn = ((StartPositionInSim*)pOrigin)->GetAttachedTaxiwaySegment()->GetNode1();
		pStartSeg = ((StartPositionInSim*)pOrigin)->GetAttachedTaxiwaySegment()->GetPositiveDirSeg();
	}

	if (pData)
	{
		CTowOperationRoute* pRetRoute = pData->GetReturnRoute();
		if (pRetRoute)
		{
			IntersectionNodeInSim* pStartNode = NULL;
			IntersectionNodeInSim* pEndNode = NULL;

			FlightGroundRouteDirectSegList vRouteItems;
			vRouteItems.clear();
		//	GetRouteItemList(pRetRoute,vRouteItems);
			IntersectionNodeInSim* pIntersectionNode = GetReturnRouteItemList(pRetRoute,vRouteItems);
			int nSize = vRouteItems.size();
			if (nSize >0)
			{
				pStartNode = vRouteItems[0]->GetEntryNode();		
				pEndNode = vRouteItems[nSize -1]->GetExitNode();

				FlightGroundRouteDirectSegList vItems;
				if (pStartNode)
				{
					vItems.clear();
					double dPathWeight = (std::numeric_limits<double>::max)();
					m_pAirportRes->getGroundRouteResourceManager()->GetRoute(pNodeIn,pStartNode,pFlight,0,vItems,dPathWeight);
					if (!vItems.empty())
					{
						if (vItems[0]->GetOppositeSegment() != pStartSeg && pStartSeg)
						{
							vSegments.push_back(pStartSeg);
						}
						vSegments.insert(vSegments.end(),vItems.begin(),vItems.end());
					}
					else
					{
						if (pStartSeg)
						{
							if (pStartSeg->GetEntryNode() == pStartNode)
							{
								vSegments.push_back(pStartSeg);
							}
							else if (pStartSeg->GetExitNode() == pStartNode)
							{
								vSegments.push_back(pStartSeg->GetOppositeSegment());
							}
						}
					
					}
				}

				vSegments.insert(vSegments.end(),vRouteItems.begin(),vRouteItems.end());

				if (pEndNode)
				{
					vItems.clear();
					m_pTaxiwayToStretchRouteFinder->GetShortestPathFromTaxiIntersectionToNearestStretch(pEndNode,vItems);
					if (!vItems.empty())
						vSegments.insert(vSegments.end(),vItems.begin(),vItems.end());
				}
				vTowSegments = vSegments;
				return;
			}
			else if (pIntersectionNode)
			{
				if (pStartSeg)
				{
					if (pStartSeg->GetEntryNode() == pIntersectionNode)
					{
						vSegments.push_back(pStartSeg);
					}
					else if (pStartSeg->GetExitNode() == pIntersectionNode)
					{
						vSegments.push_back(pStartSeg->GetOppositeSegment());
					}
				}
				
				FlightGroundRouteDirectSegList vItems;
				m_pTaxiwayToStretchRouteFinder->GetShortestPathFromTaxiIntersectionToNearestStretch(pIntersectionNode,vItems);
				if (!vItems.empty())
					vSegments.insert(vSegments.end(),vItems.begin(),vItems.end());
				
				vTowSegments = vSegments;
				return;
			}
		}
	}

	//no define
	if (pOrigin->GetType() == AirsideResource::ResType_Stand && ((StandInSim*)pOrigin)->GetOutLaneCount() >0)
		return;

	if (pOrigin->GetType() == AirsideResource::ResType_DeiceStation && ((DeicePadInSim*)pOrigin)->GetOutLaneCount() >0)
		return;

	m_pTaxiwayToStretchRouteFinder->GetShortestPathFromTaxiIntersectionToNearestStretch(pNodeIn,vSegments);

	vTowSegments = vSegments;
	if (!vSegments.empty()&& pStartSeg)
	{
		if (vSegments[0]->GetOppositeSegment() != pStartSeg && vSegments[0] != pStartSeg)
			vSegments.insert(vSegments.begin(),pStartSeg);
	}
}

bool Push_TowOperationSpecInSim::GetTowtruckReturnRoute( CReleasePoint* pData, AirsideFlightInSim* pFlight, AirsideResource* pOrigin, TowTruckServiceRequest* pRequest)
{
	FlightGroundRouteDirectSegList vSegments;
	vSegments.clear();
	GetReturnRoute(pData,pFlight,pOrigin,vSegments,pRequest->m_vSegments);

	std::vector<VehicleRouteNodePairDist> vNodePairs;
	m_pAirportRes->GetVehicleReturnRouteFromTaxiRoute(vSegments, vNodePairs);
	pRequest->m_ReturnRoute.AddData(vNodePairs);

	//AirsideResource* pFrom = NULL;
	//int nSize = vSegments.size();
	//if (nSize >0)
	//{
	//	TaxiwaySegInSim* pSeg = dynamic_cast<TaxiwaySegInSim *>(vSegments.at(nSize -1));
	//	if(pSeg != NULL)
	//	{
	//		int nCount = pSeg->getIntersectLaneCount();
	//		if (nCount >0)
	//			pFrom = pSeg->getIntersectLane(0);
	//	}
	//}

	//if (pFrom == NULL)
	//	pFrom = pOrigin;

	//VehicleRouteInSim route;
	//m_pAirportRes->getVehicleRouteRes()->GetVehicleRoute(pFrom,pDest,route);
	//route.CopyDataToRoute(resltRoute);

	if (pRequest->m_ReturnRoute.IsEmpty())
		return false;

	return true;
}



