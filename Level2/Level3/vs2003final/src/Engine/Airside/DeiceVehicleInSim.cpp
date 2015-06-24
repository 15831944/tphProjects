#include "StdAfx.h"
#include ".\DeiceVehicleInSim.h"

#include "./AirsideFlightInSim.h"
#include ".\StandLeadInLineInSim.h"
#include "standinsim.h"
#include ".\StandLeadOutLineInSim.h"
#include "VehicleRouteResourceManager.h"
#include ".\vehicleMoveEvent.h"
#include "./DeiceVehicleServiceRequest.h"
#include <common/Point2008.h>

void DeiceVehicleInSim::ResetServiceCapacity()
{
	m_fluidleft = m_pVehicleSpecificationItem->getCapacity();
}

DeiceVehicleInSim::DeiceVehicleInSim( int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem )
: AirsideVehicleInSim(id,nPrjID,pVehicleSpecItem)
{
	ResetServiceCapacity();
	mServState = NoService;
	mServType = DeIce;
}

void DeiceVehicleInSim::SetServiceFlight( AirsideFlightInSim* pFlight, double vStuffPercent )
{
	//get route from to the service place
	FlightDeiceStrategyDecision& fltdeicestragty = pFlight->GetDeiceDecision();
	
	//get at resource
	AirsideResource * AtResource = GetResource();
	if( AtResource && AtResource->GetType() == AirsideResource::ResType_StandLeadInLine )
	{
		StandLeadInLineInSim* pleadInLine = (StandLeadInLineInSim* )AtResource;
		AtResource = pleadInLine->GetStandInSim();
	}
	else if(AtResource && AtResource->GetType() == AirsideResource::ResType_StandLeadOutLine )
	{
		StandLeadOutLineInSim* pleadOutLine = (StandLeadOutLineInSim*)AtResource;
		AtResource = pleadOutLine->GetStandInSim();
	}


	//get dest resource
	AirsideResource *destResource = fltdeicestragty.m_deicePlace;
	if( destResource && destResource->GetType() == AirsideResource::ResType_StandLeadInLine )
	{
		StandLeadInLineInSim* pleadInLine = (StandLeadInLineInSim* )destResource;
		destResource = pleadInLine->GetStandInSim();
	}
	else if(destResource && destResource->GetType() == AirsideResource::ResType_StandLeadOutLine )
	{
		StandLeadOutLineInSim* pleadOutLine = (StandLeadOutLineInSim*)destResource;
		destResource = pleadOutLine->GetStandInSim();
	}

	VehicleRouteInSim resltRoute;
	if( !m_pRouteResMan->GetVehicleRoute(AtResource,destResource,resltRoute) )
	{
		if( AtResource && destResource )
		{
			CString strWarn;
			CString destName = destResource->PrintResource();
			CString resName = AtResource->PrintResource();
			strWarn.Format("Can't Find Route From %s to %s", resName, destName);
			//DiagWarning(this,strWarn);
		}
	}
	else
	{
		m_Route = resltRoute;
	}

	
	m_Route.SetMode(OnMoveToService);
	SetMode(OnMoveToService);
	m_pServiceFlight = pFlight;
	m_bAvailable = false;
	
	VehicleMoveEvent* newEvent = new VehicleMoveEvent(this);
	if (m_pResource->GetType() == AirsideResource::ResType_VehiclePool)
	{		
		newEvent->setTime( m_tLeavePoolTime );
	}else
	{
		newEvent->setTime(GetTime());
	}
	SetMode(OnBeginToService);
	WirteLog(GetPosition(),GetSpeed(),GetTime());
	SetMode(OnMoveToService);
	WirteLog(GetPosition(),GetSpeed(),GetTime());
	GenerateNextEvent(newEvent);
	
}

bool OtherFlightReadyForDeice(AirsideVehicleInSim* pVehicle, DeiceVehicleServiceRequest* pReqest)
{
	/*for(int i=0;i<pReqest->getVehicleCount();i++)
	{
		DeiceVehicleInSim* theVehicle = pReqest->getVehicle(i);
		if(theVehicle && theVehicle!= pVehicle )
		{
			if(theVehicle->mServState < DeiceVehicleInSim::ReadyForService )
			{
				return false;
			}
		}
	}*/
	return true;
}

void CallOtherVehicleReadyService(AirsideVehicleInSim* pVehicle, DeiceVehicleServiceRequest* pReqest, const ElapsedTime& t)
{
	for(int i=0;i<pReqest->getVehicleCount();i++)
	{
		DeiceVehicleInSim* theVehicle = pReqest->getVehicle(i);
		if(theVehicle && theVehicle!= pVehicle )
		{
			if(theVehicle->mServState == DeiceVehicleInSim::ReadyForService )
			{
				theVehicle->ServiceFlight(t);
			}
		}
	}
}

//inline CPoint2008 GetVehicleServicePoint(AirsideResource* pRes, Distance dist, AirsideFlightInSim* pFlight)
//{	
//
//	if(pRes && pRes->GetType() == AirsideResource::ResType_StandLeadInLine)
//	{
//		
//	}
//	if(pRes && pRes->GetType() == AirsideResource::ResType_StandLeadOutLine)
//	{
//
//	}
//	if(pRes && pRes->GetType() == AirsideResource::ResType_DeiceStation)
//	{
//
//	}
//}
//
//inline CPoint2008 GetVehicleWaitPoint(AirsideResource* pRes)
//{
//	if(pRes && pRes->GetType() == AirsideResource::ResType_StandLeadInLine)
//	{
//		StandLeadInLineInSim* pInLine = (StandLeadInLineInSim* )pRes;
//
//	}
//	if(pRes && pRes->GetType() == AirsideResource::ResType_StandLeadOutLine)
//	{
//
//	}
//	if(pRes && pRes->GetType() == AirsideResource::ResType_DeiceStation)
//	{
//
//	}
//}
//assume four service point
CPoint2008 GetDeiceServicePoint(AirsideFlightInSim* pFlight, const ElapsedTime& t,int idx)
{
	CPoint2008 fltPos = pFlight->GetPosition(t);
	ARCVector3 fltDir = (pFlight->GetPosition( pFlight->GetCurState().m_tTime ) - pFlight->GetPosition(pFlight->GetPreState().m_tTime));
	fltDir.Normalize();

	ARCVector3 fltLeftDir = fltDir;
	fltLeftDir.RotateXY(90);
		
	if(idx ==0)
	{
		return fltPos + fltLeftDir * pFlight->GetWingspan() * 0.5 + fltDir * pFlight->GetLength() * 0.1;
	}
	if( idx == 1)
	{
		return fltPos - fltLeftDir * pFlight->GetWingspan() * 0.5 + fltDir * pFlight->GetLength() * 0.1;
	}
	if( idx == 3)
	{
		return fltPos + fltLeftDir * pFlight->GetWingspan() * 0.5 - fltDir * pFlight->GetLength() * 0.1;
	}
	
	return fltPos - fltLeftDir * pFlight->GetWingspan() * 0.5 - fltDir * pFlight->GetLength() * 0.1;
}

#include ".\DeiceResourceManager.h"
CPoint2008 GetTempParkingServicePoint(AirsideFlightInSim* pFlight, AirsideResource* pDestRes, const ElapsedTime& t, int idx)
{

	ARCVector3 fltDir;
	CPoint2008 fltPos;
	CPath2008 inPath;
	if(pDestRes->GetType() == AirsideResource::ResType_DeiceStation)
	{
		DeicePadInSim* pPad = (DeicePadInSim*)pDestRes;
		inPath = pPad->GetInPath();		
	}
	else if(pDestRes && pDestRes->GetType() == AirsideResource::ResType_StandLeadInLine)
	{
		StandLeadInLineInSim* pLeadInLine = (StandLeadInLineInSim* )pDestRes;
		inPath = pLeadInLine->GetPath();
	}
	else if(pDestRes && pDestRes->GetType() == AirsideResource::ResType_StandLeadOutLine)
	{
		StandLeadOutLineInSim* pOUtLine = (StandLeadOutLineInSim*)pDestRes;
		inPath = pOUtLine->GetPath();
	}
	
	fltPos = inPath.getPoint( inPath.getCount()-1);		
	fltDir = inPath.GetIndexDir( (float)inPath.getCount()-1);	
	fltDir.Normalize();
	ARCVector3 fltLeftDir = fltDir;
	fltLeftDir.RotateXY(90);

	if(idx ==0)
	{
		return fltPos + fltLeftDir * pFlight->GetWingspan()  + fltDir * pFlight->GetLength() * 0.2;
	}
	if( idx == 1)
	{
		return fltPos - fltLeftDir * pFlight->GetWingspan()  + fltDir * pFlight->GetLength() * 0.2;
	}
	if( idx == 3)
	{
		return fltPos + fltLeftDir * pFlight->GetWingspan()  - fltDir * pFlight->GetLength() * 0.2;
	}	
	return fltPos - fltLeftDir * pFlight->GetWingspan()  - fltDir * pFlight->GetLength() * 0.2;
	
}

void DeiceVehicleInSim::ServiceFlight( const ElapsedTime& tTime )
{
	if(GetMode() != OnService)
		return;

	NotifyOtherAgents(SimMessage().setTime(tTime));//NotifyObservers();
	AirsideFlightInSim* pServFlt = m_pServiceFlight;
	DeiceVehicleServiceRequest* pDeiceServRquest = pServFlt->getDeiceServiceRequest();
	if(!(pServFlt && pDeiceServRquest) )
	{
		ASSERT(FALSE);
		GetNextCommand();
		return;
	}

	int vIdx = pDeiceServRquest->geVehicleIndex(this);

	if(mServState == OnEnterDeiceArea ) //first enter the deice area
	{
		if(GetResource())
			GetResource()->SetExitTime(this,tTime);
		
		if( pServFlt->bReadyForDeice() )  //move to serve point directly
		{
			mServState = ReadyForService;
			CPoint2008 SerivePos = GetDeiceServicePoint(pServFlt,tTime,vIdx );
			DistanceUnit dist = GetPosition().distance( SerivePos);
			double vehicleSpeed = GetSpeed();
			ElapsedTime dT = ElapsedTime(dist/vehicleSpeed);
			SetTime(tTime+dT);
			SetPosition(SerivePos);
			SetSpeed(vehicleSpeed);
			SetResource( pServFlt->GetDeiceDecision().m_deicePlace);			
			WirteLog(SerivePos,vehicleSpeed, GetTime());
			_continueService();			
			CallOtherVehicleReadyService(this,pDeiceServRquest,GetTime());	
			return;
		}
		else
		{
			mServState = WaitForFlightAtTemp;
			CPoint2008 tempPoint =  GetTempParkingServicePoint(pServFlt,pServFlt->GetDeiceDecision().m_deicePlace, tTime,vIdx);
			DistanceUnit dist = GetPosition().distance( tempPoint);
			double vehicleSpeed = GetSpeed();
			ElapsedTime dT = ElapsedTime(dist/vehicleSpeed);
			SetTime(tTime+dT);
			SetPosition(tempPoint);
			SetSpeed(vehicleSpeed);
			WirteLog(tempPoint,vehicleSpeed, GetTime());			
			return;
		}
	}
	
	if(mServState == WaitForFlightAtTemp) //check to see service can start
	{
		
		if(pServFlt->bReadyForDeice())
		{
			WirteLog(GetPosition(),GetSpeed(),tTime);
			mServState = ReadyForService;
			SetResource( pServFlt->GetDeiceDecision().m_deicePlace);	
			CPoint2008 SerivePos = GetDeiceServicePoint(pServFlt,tTime,vIdx );
			DistanceUnit dist = GetPosition().distance( SerivePos);
			double vehicleSpeed = GetSpeed();
			ElapsedTime dT = ElapsedTime(dist/vehicleSpeed);
			SetTime(tTime+dT);
			SetPosition(SerivePos);
			SetSpeed(vehicleSpeed);
			WirteLog(SerivePos,vehicleSpeed, GetTime());
			_continueService();
			CallOtherVehicleReadyService(this,pDeiceServRquest,GetTime());
		}	
		return;
	}

	if(mServState== ReadyForService)
	{
		if( OtherFlightReadyForDeice(this, pDeiceServRquest) ) //start service
		{
			mServState = ServiceStart;
			SetTime(tTime);
			WirteLog( GetPosition(), GetSpeed(), GetTime());
			_continueService();
		}		
		return;
	}
	
	if( mServState == ServiceStart)
	{
		mServState = ServiceEnd;
		double FlowSpd = (mServType==DeIce)?pDeiceServRquest->mDeiceRequir.m_deiceFlowrate:pDeiceServRquest->mDeiceRequir.m_antiiceFlowrate;
		ElapsedTime servT = mdServFluid/max(1,FlowSpd)*60;
		SetTime(tTime+servT);
		m_fluidleft = 0;//decreaseFluid( mdServFluid );		

		WirteLog(GetPosition(),GetSpeed(),GetTime());
		_continueService();		
		return;
	}
	if(mServState == ServiceEnd)
	{		
		SetTime(tTime);
		pServFlt->VehicleServiceComplete(this);
		mServState = NoService;
		ReturnVehiclePool(NULL);//GetNextCommand();
	}

}

void DeiceVehicleInSim::_ChangeToService()
{
	VehicleServiceEvent* newEvent = new VehicleServiceEvent(this);
	SetMode(OnService);
	mServState = OnEnterDeiceArea;
	newEvent->setTime(GetTime());
	WirteLog(GetPosition(),GetSpeed(),GetTime());
	//newEvent->addEvent();
	GenerateNextEvent(newEvent);
}

void DeiceVehicleInSim::_continueService()
{
	VehicleServiceEvent* newEvent = new VehicleServiceEvent(this);
	SetMode(OnService);
	newEvent->setTime(GetTime());	
	GenerateNextEvent(newEvent);
}
