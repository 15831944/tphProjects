#include "StdAfx.h"
#include ".\runwayexitassignmentstrategiesinsim.h"

#include "./Common/elaptime.h"
#include "AirsideFlightInSim.h"
#include "./Common/AirportDatabase.h"
#include "./InputAirside/RunwayExitStrategies.h"
#include "./InputAirside/FlightTypeRunwayExitStrategyItem.h"
#include "./InputAirside/RunwayExitStandStrategyItem.h"
#include "./InputAirside/TimeRangeRunwayExitStrategyItem.h"
#include "RunwayInSim.h"
#include "StandResourceManager.h"
#include "FlightPerformanceManager.h"
#include "RunwaySegInSim.h"
#include "NotifyRunwayWaveCrossEvent.h"
#include "RunwayCrossSpecificationInSim.h"
#include "SimulationErrorShow.h"
#include "AirTrafficController.h"
#include "FlightPerformancesInSim.h"
#include "TaxiwayResourceManager.h"

CRunwayExitAssignmentStrategiesInSim::CRunwayExitAssignmentStrategiesInSim(void)
{
	m_pRunwayExitStrategies = NULL;
	m_pTaxiwayResManager = NULL;
}

CRunwayExitAssignmentStrategiesInSim::~CRunwayExitAssignmentStrategiesInSim(void)
{
	delete m_pRunwayExitStrategies;
	m_pRunwayExitStrategies = NULL;

	std::vector<TaxiConditionInSim *>::iterator iter = m_vTaxiConditionInSim.begin();
	for (; iter != m_vTaxiConditionInSim.end(); ++ iter)
	{
		delete *iter;
	}
	m_vTaxiConditionInSim.clear();
}

void CRunwayExitAssignmentStrategiesInSim::Init( int nPrj, CAirportDatabase* pAirportDatabase, TaxiwayResourceManager *pTaxiwayResManager)
{
	m_pRunwayExitStrategies = new RunwayExitStrategies(nPrj);
	m_pRunwayExitStrategies->SetAirportDB(pAirportDatabase);
	m_pRunwayExitStrategies->ReadData();

	ASSERT(pTaxiwayResManager);
	m_pTaxiwayResManager = pTaxiwayResManager;

}

std::vector<RunwayExitInSim*> VerifyFlightExits(const std::vector<RunwayExitInSim*>& vExit,AirsideFlightInSim* pFlight ,bool bCheckCanHold)
{
	if(pFlight && bCheckCanHold)
	{
		std::vector<RunwayExitInSim*> vRet;
		for(int i=0;i<(int)vExit.size();i++)
		{
			RunwayExitInSim* pExit = vExit.at(i);
			if(pExit && pExit->CanHoldFlight(pFlight) )
			{
				vRet.push_back(pExit);
			}
		}
		return vRet;
	}
	return vExit;
}

std::vector<RunwayExitInSim*> CRunwayExitAssignmentStrategiesInSim::GetRunwayExitByFlightType( AirsideFlightInSim *pFlight,CAirportDatabase *pAirportDatabase ,StandResourceManager* pStandResManager ,bool bCheckCanHold)
{
	StandInSim* pStand = pFlight->GetOperationParkingStand();
	if (pStand == NULL)
		pStand = pFlight->GetPlanedParkingStand(ARR_PARKING);

	LogicRunwayInSim* pLogicRunway = pFlight->GetLandingRunway();
	int nCount = m_pRunwayExitStrategies->GetStrategyCount();

	if (pStand == NULL && nCount > 0&&!pFlight->IsCircuitFlight())
	{
		CString strWarn;
		strWarn.Format("No assigned parking stand, without available runway exit strategy!") ;
		CString strError = _T("DEFINE ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);	
	}

	ALTObjectID standName;
	if(pStand)
		standName = pStand->GetStandInput()->GetObjectName();

	FlightTypeRunwayExitStrategyItem* pItem = NULL;
	RunwayExitStandStrategyItem* pStandItem = NULL;

	for ( int i =0; i < nCount; i++)
	{
		FlightConstraint fltCnst = m_pRunwayExitStrategies->GetStrategyItem(i)->getFlightType();
		if(	pFlight->fits(fltCnst) )
		{
			pItem = m_pRunwayExitStrategies->GetStrategyItem(i);
			if (pItem)
			{
				int nItemCount = pItem->GetRunwayExitStandStrategyCount();
				CString strName;
				for (int j =0; j < nItemCount; j++)
				{
					RunwayExitStandStrategyItem* pData = pItem->GetRunwayExitStandStrategyItem(j);
					strName = pData->getStandFamilyName();
					if (strName.CompareNoCase("All") ==0)
					{
						pStandItem = pData;
						break;
					}

					ALTObjectID objName(strName);
					if (standName.idFits(objName))
					{
						pStandItem = pData;
						break;
					}
				}
			}
			if (pStandItem)//find the best item
				break;
		}	
	}

	if (pStandItem)
	{
		TimeRangeRunwayExitStrategyItem* pTimeItem = NULL;
		nCount = pStandItem->GetTimeRangeStrategyItemCount();
		for (int i =0; i < nCount; i++)
		{
			ElapsedTime tStart = pStandItem->GetTimeRangeStrategyItem(i)->getStartTime();
			ElapsedTime tEnd = pStandItem->GetTimeRangeStrategyItem(i)->getEndTime();
			if (pFlight->GetTime() >= tStart && pFlight->GetTime() <= tEnd)
			{
				pTimeItem = pStandItem->GetTimeRangeStrategyItem(i); 
				break;
			}
		}

		if (pTimeItem)
		{
			switch(pTimeItem->getStrategyType())
			{
				case MaxBrakingFirstOnEitherSide:
					 return VerifyFlightExits(GetMaxBrakingFirstExitOnEitherSide(pFlight,pLogicRunway),pFlight,bCheckCanHold);						 
					
				case MaxBrakingFirstOnSameSideAsArrivalGate:
					 return VerifyFlightExits( GetMaxBrakingFirstExitOnSameSideAsArrivalGate(pFlight,pLogicRunway,pStandResManager),pFlight,bCheckCanHold);

				case NormalBrakingExitOnEitherSide:
					 return VerifyFlightExits(GetNormalBrakingExitOnEitherSide(pFlight, pLogicRunway),pFlight,bCheckCanHold);

				case NormalBrakingExitOnSameSide:
					 return VerifyFlightExits(GetNormalBrakingExitOnSameSideAsArrivalGate(pFlight, pLogicRunway,pStandResManager),pFlight,bCheckCanHold);

				case ManagedExit:
					 return GetManagedExit(pFlight,pLogicRunway,pTimeItem, bCheckCanHold);

				default:
					break;
			}
		}
	}

	if (nCount > 0)
	{
		CString strWarn;
		strWarn.Format("No available exits for the flight according to Runway Exit Assignment Strategies") ;
		CString strError = _T("DEFINE ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);	
	}

	std::vector<RunwayExitInSim*> vExits;
	vExits.clear();
	vExits = VerifyFlightExits( GetNormalBrakingExitOnSameSideAsArrivalGate(pFlight,pLogicRunway,pStandResManager),pFlight,bCheckCanHold);

	if (vExits.empty())
	{
		 vExits = VerifyFlightExits( GetMaxBrakingFirstExitOnSameSideAsArrivalGate(pFlight,pLogicRunway,pStandResManager),pFlight,bCheckCanHold);
	}

	return vExits;
}

bool CRunwayExitAssignmentStrategiesInSim::IsBackTrackOp(AirsideFlightInSim* pFlight)
{
	LogicRunwayInSim* pLogicRunway = pFlight->GetLandingRunway();
	StandInSim* pStand = pFlight->GetOperationParkingStand();
	if (pStand == NULL)
		pStand = pFlight->GetPlanedParkingStand(ARR_PARKING);

	if (pStand == NULL)
		return false;

	FlightTypeRunwayExitStrategyItem* pItem = NULL;
	RunwayExitStandStrategyItem* pStandItem = NULL;

	int nCount = m_pRunwayExitStrategies->GetStrategyCount();
	for ( int i =0; i < nCount; i++)
	{
		FlightConstraint fltCnst = m_pRunwayExitStrategies->GetStrategyItem(i)->getFlightType();
		if(	pFlight->fits(fltCnst) )
		{
			pItem = m_pRunwayExitStrategies->GetStrategyItem(i);
			if (pItem)
			{
				int nItemCount = pItem->GetRunwayExitStandStrategyCount();
				for (int j =0; j < nItemCount; j++)
				{
					RunwayExitStandStrategyItem* pData = pItem->GetRunwayExitStandStrategyItem(j);
					CString strName = pData->getStandFamilyName();
					if (strName.CompareNoCase("All") ==0)
					{
						pStandItem = pData;
						break;
					}

					ALTObjectID objName(strName);
					if (pStand->GetStandInput()->GetObjectName().idFits(objName))
					{
						pStandItem = pData;
						break;
					}
				}
			}
			if(pStandItem)
				break;

		}	
	}

	if (pStandItem)
	{
		TimeRangeRunwayExitStrategyItem* pTimeItem = NULL;
		nCount = pStandItem->GetTimeRangeStrategyItemCount();
		for (int i =0; i < nCount; i++)
		{
			ElapsedTime tStart = pStandItem->GetTimeRangeStrategyItem(i)->getStartTime();
			ElapsedTime tEnd = pStandItem->GetTimeRangeStrategyItem(i)->getEndTime();
			if (pFlight->GetTime() >= tStart && pFlight->GetTime() <= tEnd)
			{
				pTimeItem = pStandItem->GetTimeRangeStrategyItem(i); 
				break;
			}
		}

		if (pTimeItem)
		{
			int nRunwayID = pFlight->GetLandingRunway()->GetRunwayInSim()->GetRunwayInput()->getID();
			int nRunwayMark = pFlight->GetLandingRunway()->getLogicRunwayType();

			if(pTimeItem->getStrategyType() == ManagedExit && pTimeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::ProbabilityBased)
			{
				RunwayExitStrategyPercentItem* pRwyExitItem = pTimeItem->GetRunwayExitPercentItem(nRunwayID,nRunwayMark);
				RunwayExitPercentVector vExit = pRwyExitItem->getItems();

				int nCount = int(vExit.size());
				for ( int i = 0; i< nCount; i++ )
				{
					if (vExit[i]->getExitID() == pFlight->GetRunwayExit()->GetID())
						return vExit[i]->IsBacktrack();				 
				}
			}
			else
			{
				RunwayExitStrategyPriorityItem* pRwyExitItem = pTimeItem->GetRunwayExitPriorityItem(nRunwayID,nRunwayMark);
				RunwayExitStrategyPriorityVector vExit = pRwyExitItem->getItems();

				int nCount = int(vExit.size());
				for ( int i = 0; i< nCount; i++ )
				{
					if (vExit[i]->getExitID() == pFlight->GetRunwayExit()->GetID())
						return vExit[i]->IsBacktrack();				 
				}
			}
		}
	}
	return false;
}

std::vector<RunwayExitInSim*> CRunwayExitAssignmentStrategiesInSim::GetMaxBrakingFirstExitOnEitherSide(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway)
{
	double maxDel = GetFlightMaxDecelOnRunway(pFlight);
	double maxAcuteSpeed = pFlight->GetPerformance()->getMaxExitSpeedAcute(pFlight);
	double max90degSpeed = pFlight->GetPerformance()->getMaxExitSpeed90deg(pFlight);
	double maxHighSpeed = pFlight->GetPerformance()->getMaxExitSpeedHigh(pFlight);
	double landingSpeed = pFlight->GetPerformance()->getLandingSpeed(pFlight);

	double minAcuteDist = (landingSpeed*landingSpeed - maxAcuteSpeed*maxAcuteSpeed)/(2* maxDel);
	double min90degDist = (landingSpeed*landingSpeed - max90degSpeed*max90degSpeed)/(2* maxDel);
	double minHighDist = (landingSpeed*landingSpeed - maxHighSpeed*maxHighSpeed)/(2* maxDel);
	double dTouchDist = pFlight->GetTouchDownDistAtRuwnay(pLogicRunway);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);

	std::vector<RunwayExitInSim *> vAvailableExit;
	vAvailableExit.clear();
	int nCount = pLogicRunway->GetExitCount();
	for (int i = 0; i < nCount; i++)
	{
		RunwayExitInSim *pRunwayExit = pLogicRunway->GetExitAt(i);
		double dAngle = pLogicRunway->GetExitAt(i)->GetAngle();
		double dExitDist = pLogicRunway->GetExitAt(i)->GetExitRunwayPos(); 

		if (dAngle >90.0 && dAngle < 180.0 )
			if (dTouchDist + minAcuteDist <= dExitDist)
					vAvailableExit.push_back(pRunwayExit);

		if(dAngle == 90.0)
			if (dTouchDist + min90degDist <= dExitDist)
					vAvailableExit.push_back(pRunwayExit);

		if (dAngle < 90.0 && dAngle > 0.0)
			if (dTouchDist + minHighDist <= dExitDist)
					vAvailableExit.push_back(pRunwayExit);
	}

	return vAvailableExit;
}

std::vector<RunwayExitInSim*> CRunwayExitAssignmentStrategiesInSim::GetMaxBrakingFirstExitOnSameSideAsArrivalGate(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,StandResourceManager* pStandResManager )
{
	double maxDel = GetFlightMaxDecelOnRunway(pFlight);
	double maxAcuteSpeed = pFlight->GetPerformance()->getMaxExitSpeedAcute(pFlight);
	double max90degSpeed = pFlight->GetPerformance()->getMaxExitSpeed90deg(pFlight);
	double maxHighSpeed = pFlight->GetPerformance()->getMaxExitSpeedHigh(pFlight);
	double landingSpeed = pFlight->GetPerformance()->getLandingSpeed(pFlight);

	double minAcuteDist = (landingSpeed*landingSpeed - maxAcuteSpeed*maxAcuteSpeed)/(2* maxDel);
	double min90degDist = (landingSpeed*landingSpeed - max90degSpeed*max90degSpeed)/(2* maxDel);
	double minHighDist = (landingSpeed*landingSpeed - maxHighSpeed*maxHighSpeed)/(2* maxDel);
	double dTouchDist = pFlight->GetTouchDownDistAtRuwnay(pFlight->GetLandingRunway());//pFlight->GetPerformance()->getDistThresToTouch(pFlight);

	CPoint2008 _point;
	int nCount = pLogicRunway->GetExitCount();
	if (pFlight->GetPlanedParkingStand(ARR_PARKING))
		_point = pFlight->GetPlanedParkingStand(ARR_PARKING)->GetDistancePoint(0);
	else if (pStandResManager->GetStandCount())
		_point = pStandResManager->GetStandByIdx(0)->GetDistancePoint(0);
	bool IsleftStand = pLogicRunway->GetRunwayInSim()->GetRunwayInput()->IsPointLeft(RUNWAY_MARK(pLogicRunway->getLogicRunwayType()),_point);

	std::vector<RunwayExitInSim *> vAvailableExit;
	vAvailableExit.clear();

	for (int i = 0; i < nCount; i++)
	{
		RunwayExitInSim *pRunwayExit = pLogicRunway->GetExitAt(i);
		bool IsleftExit = pLogicRunway->GetExitAt(i)->IsLeft();
		if (IsleftStand == IsleftExit)
		{
			double dExitDist = pLogicRunway->GetExitAt(i)->GetExitRunwayPos();
			double dAngle = pLogicRunway->GetExitAt(i)->GetAngle();


			if ( dAngle >90.0 && dAngle < 180.0 )
				if (dTouchDist + minAcuteDist <= dExitDist)
				{
					vAvailableExit.push_back(pRunwayExit);
				}

			if(dAngle == 90.0)
				if (dTouchDist + min90degDist <= dExitDist)
				{
					vAvailableExit.push_back(pRunwayExit);
				}

			if (dAngle < 90.0 && dAngle > 0.0)
				if (dTouchDist + minHighDist <= dExitDist)
				{
					vAvailableExit.push_back(pRunwayExit);		
				}
		}
	}
	return vAvailableExit;
}

std::vector<RunwayExitInSim*> CRunwayExitAssignmentStrategiesInSim::GetNormalBrakingExitOnEitherSide(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway)
{
	double NormalDel = GetFlightNormalDecelOnRunway(pFlight);
	double maxAcuteSpeed = pFlight->GetPerformance()->getMaxExitSpeedAcute(pFlight);
	double max90degSpeed = pFlight->GetPerformance()->getMaxExitSpeed90deg(pFlight);
	double maxHighSpeed = pFlight->GetPerformance()->getMaxExitSpeedHigh(pFlight);
	double landingSpeed = pFlight->GetPerformance()->getLandingSpeed(pFlight);

	double minAcuteDist = (landingSpeed*landingSpeed - maxAcuteSpeed*maxAcuteSpeed)/(2* NormalDel);
	double min90degDist = (landingSpeed*landingSpeed - max90degSpeed*max90degSpeed)/(2* NormalDel);
	double minHighDist = (landingSpeed*landingSpeed - maxHighSpeed*maxHighSpeed)/(2* NormalDel);
	double dTouchDist = pFlight->GetTouchDownDistAtRuwnay(pLogicRunway);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);
	

	std::vector<RunwayExitInSim *> vAvailableExit;
	vAvailableExit.clear();

	int nCount = pLogicRunway->GetExitCount();
	for (int i = 0; i < nCount; i++)
	{

		RunwayExitInSim *pRunwayExit = pLogicRunway->GetExitAt(i);
		double dAngle = pLogicRunway->GetExitAt(i)->GetAngle();
		double dExitDist = pLogicRunway->GetExitAt(i)->GetExitRunwayPos(); 

		if ( dAngle >90.0 && dAngle < 180.0)
			if (dTouchDist + minAcuteDist <= dExitDist)
			{
				vAvailableExit.push_back(pRunwayExit);
			}

		if(dAngle == 90.0)
			if (dTouchDist + min90degDist <= dExitDist)
			{
				vAvailableExit.push_back(pRunwayExit);			
			}

		if (dAngle < 90.0 && dAngle > 0.0)
			if (dTouchDist + minHighDist <= dExitDist)
			{
				vAvailableExit.push_back(pRunwayExit);	
			}
	}

	return vAvailableExit;
}

std::vector<RunwayExitInSim*> CRunwayExitAssignmentStrategiesInSim::GetNormalBrakingExitOnSameSideAsArrivalGate(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,StandResourceManager* pStandResManager )
{
	double NormalDel = GetFlightNormalDecelOnRunway(pFlight);
	double maxAcuteSpeed = pFlight->GetPerformance()->getMaxExitSpeedAcute(pFlight);
	double max90degSpeed = pFlight->GetPerformance()->getMaxExitSpeed90deg(pFlight);
	double maxHighSpeed = pFlight->GetPerformance()->getMaxExitSpeedHigh(pFlight);
	double landingSpeed = pFlight->GetPerformance()->getLandingSpeed(pFlight);

	double minAcuteDist = (landingSpeed*landingSpeed - maxAcuteSpeed*maxAcuteSpeed)/(2* NormalDel);
	double min90degDist = (landingSpeed*landingSpeed - max90degSpeed*max90degSpeed)/(2* NormalDel);
	double minHighDist = (landingSpeed*landingSpeed - maxHighSpeed*maxHighSpeed)/(2* NormalDel);
	double dTouchDist = pFlight->GetTouchDownDistAtRuwnay(pLogicRunway);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);

	int nCount = pLogicRunway->GetExitCount();

	CPoint2008 _point;
	if (pFlight->GetPlanedParkingStand(ARR_PARKING))
		_point = pFlight->GetPlanedParkingStand(ARR_PARKING)->GetDistancePoint(0);
	else if (pStandResManager->GetStandCount())
		_point = pStandResManager->GetStandByIdx(0)->GetDistancePoint(0);

	bool IsleftStand = pLogicRunway->GetRunwayInSim()->GetRunwayInput()->IsPointLeft(RUNWAY_MARK(pLogicRunway->getLogicRunwayType()),_point);
	std::vector<RunwayExitInSim *> vAvailableExit;
	vAvailableExit.clear();

	for (int i = 0; i < nCount; i++)
	{
		RunwayExitInSim *pCurExit = pLogicRunway->GetExitAt(i);
		ASSERT(pCurExit != NULL);

		// assert the exit is open or closed
		if(pCurExit->CanServeFlight(pFlight) == false)
			continue;

		bool IsleftExit = pCurExit->IsLeft();
		if (IsleftStand == IsleftExit)
		{
			RunwayExitInSim *pRunwayExit = pLogicRunway->GetExitAt(i);
			double dAngle = pCurExit->GetAngle();
			double dExitDist = pCurExit->GetExitRunwayPos(); 

			if ( dAngle >90.0 && dAngle < 180.0)
				if (dTouchDist + minAcuteDist <= dExitDist)
				{
					vAvailableExit.push_back(pRunwayExit);			
				}

			if(dAngle == 90.0)
				if (dTouchDist + min90degDist <= dExitDist)
				{
					vAvailableExit.push_back(pRunwayExit);
				}

			if (dAngle < 90.0 && dAngle > 0.0)
				if (dTouchDist + minHighDist <= dExitDist)
				{
					vAvailableExit.push_back(pRunwayExit);
				}
		}
	}
	return vAvailableExit;

}

std::vector<RunwayExitInSim*> CRunwayExitAssignmentStrategiesInSim::GetManagedExit(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,TimeRangeRunwayExitStrategyItem* pItem,bool bCheckCanHold)
{
	int nRunwayID = pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
	int nRunwayMark = pFlight->GetLandingRunway()->getLogicRunwayType();

	if(pItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::ProbabilityBased)
	{
		RunwayExitStrategyPercentItem *pRunwayExitItem = pItem->GetRunwayExitPercentItem(nRunwayID,nRunwayMark);
		return GetManagedPercentExit(pFlight, pLogicRunway, pRunwayExitItem, bCheckCanHold);
	}

	RunwayExitStrategyPriorityItem* pRwyItem = pItem->GetRunwayExitPriorityItem(nRunwayID,nRunwayMark);
	return GetManagedPriorityExit(pFlight, pLogicRunway, pRwyItem, bCheckCanHold);
	
}

std::vector<RunwayExitInSim*> CRunwayExitAssignmentStrategiesInSim::GetManagedPercentExit( AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,RunwayExitStrategyPercentItem* pRwyItem,bool bCheckCanHold )
{
	std::vector<RunwayExitPercentItem *> vExitItem = pRwyItem->getItems();

	//get all available exit for the sake of landing distance

	//calculate rolling distance
	double landspd = pFlight->GetPerformance()->getLandingSpeed(pFlight);
	double dTouchDownDistance = pFlight->GetTouchDownDistAtRuwnay(pLogicRunway);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);
	double dMaxDecel = pFlight->GetAirTrafficController()->GetRunwayExitAssignmentStrategiesInSim()->GetFlightMaxDecelOnRunway(pFlight);

	//get available exit, remove the item landing distance does not fit
	std::vector<RunwayExitPercentItem *> vAvailableExitItem;
	int nExitItemCount = static_cast<int>(vExitItem.size());
	for(int nExitItem = 0; nExitItem < nExitItemCount; ++nExitItem)
	{	
		RunwayExitPercentItem * pExitItem = vExitItem[nExitItem] ;
		if(pExitItem == NULL || pLogicRunway == NULL)
			continue;

		int nExitID = pExitItem->getExitID();

		if( pExitItem->IsBacktrack() )
		{
			vAvailableExitItem.push_back(pExitItem);
		}
		else
		{
			RunwayExitInSim* pAvailableExit = pLogicRunway->GetExitByID(nExitID);
			if(pAvailableExit)
			{
				double dExitDistanceInRunway = pAvailableExit->GetExitRunwayPos();

				double dExitSpeed = 1000;

				if (pAvailableExit->GetAngle() < 90.0)
				{
					dExitSpeed = pFlight->GetPerformance()->getMaxExitSpeedHigh(pFlight);
				}
				if (pAvailableExit->GetAngle() == 90.0)
				{
					dExitSpeed = pFlight->GetPerformance()->getMaxExitSpeed90deg(pFlight);
				}
				if (pAvailableExit->GetAngle() > 90.0)
				{
					dExitSpeed = pFlight->GetPerformance()->getMaxExitSpeedAcute(pFlight);
				}
				double dMinLandingDistance = (landspd*landspd - dExitSpeed*dExitSpeed)/(2.0* dMaxDecel);

				//+1 for double to int miscount
				if(dExitDistanceInRunway > dMinLandingDistance + dTouchDownDistance + 1)
					vAvailableExitItem.push_back(pExitItem);					
			}					
		}	
	}

	if(vAvailableExitItem.size() == 0)//all defined runway exit are not fit
		vAvailableExitItem = vExitItem;


	std::vector<RunwayExitInSim*> vAvailableExit;

	//check can hold flight exit, after landing if exit is not available
	if(bCheckCanHold)
	{
		std::vector<RunwayExitPercentItem*> vCanHoldExits;
		std::vector<RunwayExitInSim*> vExitsInSim;
		RunwayExitInSim* pFirstHavePercentExit = NULL;
		float dTotalAvaPercent = 0;

		for(int i=0;i<(int)vAvailableExitItem.size();i++)
		{
			RunwayExitPercentItem* pPercentItem = vAvailableExitItem.at(i);
			RunwayExitInSim* pExitInSim = pLogicRunway->GetExitByID(pPercentItem->getExitID() );
			if(pPercentItem->m_fPercent>0) //get first have percent item
			{
				if(!pFirstHavePercentExit)
				{				
					pFirstHavePercentExit = pExitInSim;
				}
				else
				{
					if(pExitInSim->GetPosAtRunway() < pFirstHavePercentExit->GetPosAtRunway() )
					{					
						pFirstHavePercentExit = pExitInSim;
					}
				}
			}

			if(pExitInSim && pExitInSim->CanHoldFlight(pFlight) && pPercentItem->m_fPercent >0 )
			{
				vCanHoldExits.push_back(pPercentItem);
				vExitsInSim.push_back(pExitInSim);
				dTotalAvaPercent += pPercentItem->m_fPercent;
			}		

		}

		//get from percent;
		if(dTotalAvaPercent>1)
		{
			int dRandPercent = rand()%(int)dTotalAvaPercent;
			int dPercent = 0;
			int nExitID = -1;
			for ( int i = 0; i< int(vCanHoldExits.size()); i++ )
			{
				if (dRandPercent>= dPercent && dRandPercent < dPercent + (int)vCanHoldExits[i]->m_fPercent )
				{
					RunwayExitInSim* pExit  = vExitsInSim[i];	
					vAvailableExit.push_back(pExit);
					return vAvailableExit;
				}
				dPercent += (int)vCanHoldExits[i]->m_fPercent;
			}
		}
		

		if(!vExitsInSim.empty())
		{
			vAvailableExit.push_back(vExitsInSim.front());
			return vAvailableExit;
		}
		//get from exit after have percent flight ??
		/*if(pFirstHavePercentExit)
		{
		for(int i=0;i<pLogicRunway->GetExitCount();i++)
		{
		RunwayExitInSim* pExit = pLogicRunway->GetExitAt(i);
		if(pExit->GetPosAtRunway()>= pFirstHavePercentExit->GetPosAtRunway() && pExit->CanHoldFlight(pFlight) )
		{
		vAvailableExit.push_back(pExit);
		}
		}
		return vAvailableExit;
		}*/

	}
	else//assign runway exit, first time
	{
		//get runway exit available
		double dTotalPercentLeft = 0.0;
		for ( int i = 0; i< int(vAvailableExitItem.size()); i++ )
		{
			dTotalPercentLeft += vAvailableExitItem[i]->m_fPercent;
		}	

		double dRandPercent = 0.0;
		ASSERT(dTotalPercentLeft >= 0.0);
		if(dTotalPercentLeft > 0.0)
		{
			int nTotalPercent =(int)dTotalPercentLeft;
			if(nTotalPercent > 0)
			{
				dRandPercent = double(rand()%nTotalPercent);
			}
		}

		//select one available exit
		double dPercent = 0.0;
		int nExitID = -1;
		for ( int i = 0; i< int(vAvailableExitItem.size()); i++ )
		{
			if (dRandPercent>= dPercent && dRandPercent < dPercent + vAvailableExitItem[i]->m_fPercent )
			{
				nExitID = vAvailableExitItem[i]->getExitID();
				break;
			}
			dPercent += vAvailableExitItem[i]->m_fPercent;
		}

		
		std::vector<RunwayExitInSim*> vAvailableExit;
		RunwayExitInSim* pExit = pLogicRunway->GetExitByID(nExitID);
		if(pExit)
			vAvailableExit.push_back(pExit);
		
		return vAvailableExit;	

	}


	return vAvailableExit;
}

bool SortExitsByPriority(RunwayExitPriorityItem* pFirst, RunwayExitPriorityItem* pSecond)
{
	if (pFirst->getPriorityIndex() <= pSecond->getPriorityIndex())
		return true;

	return false;
}

std::vector<RunwayExitInSim*> CRunwayExitAssignmentStrategiesInSim::GetManagedPriorityExit( AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,RunwayExitStrategyPriorityItem* pItem,bool bCheckCanHold )
{
	std::vector<RunwayExitInSim*> vExits;

	std::vector<RunwayExitPriorityItem*> vPriorityExits = pItem->getItems();
	std::sort(vPriorityExits.begin(), vPriorityExits.end(), SortExitsByPriority);

	int nCount = (int)vPriorityExits.size();
	for (int i =0; i < nCount; i++)
	{
		RunwayExitPriorityItem* pExitItem = vPriorityExits.at(i);
		RunwayExitInSim* pExitInSim = pLogicRunway->GetExitByID(pExitItem->getExitID());
		if(pExitInSim == NULL)
			continue;

		if(bCheckCanHold ) // the  runway exit is not available
		{
			if(!pExitInSim->CanHoldFlight(pFlight))
				continue;
		}

		if(i!=nCount-1)
		{
			//check if arrival stand is occupied by other flight
			if(pExitItem->GetConditionData().GetArrStandOccupied())
			{
				StandInSim *pArrStand = pFlight->GetPlanedParkingStand(ARR_PARKING);
				if(pArrStand != NULL)
				{
					AirsideFlightInSim *pLockedFlight = pArrStand->GetLockedFlight();
					if(pLockedFlight != NULL && pLockedFlight != pFlight)
						continue;
				}
			}
			//check if the related taxiway is have flight in head to head operation
			bool bTaxiConditionAvailable = true;

			if(pExitItem->GetConditionData().GetOperationStatus())
			{
				int nConditionCount = pExitItem->GetConditionData().getTaxiConditionCount();
				for (int nCondition = 0; nCondition < nConditionCount; ++ nCondition)//for each condition
				{
					TaxiSegmentData *pTaxiCondition = pExitItem->GetConditionData().getTaxiCondition(nCondition);

					if(!IsTaxiConditionAvaiable(pFlight, pTaxiCondition))
					{
						bTaxiConditionAvailable = false;
						break;
					}
				}
			}
			if(!bTaxiConditionAvailable)
				continue;
		}
		//available exit
		vExits.push_back(pExitInSim);
	}

	return vExits;
}
bool CRunwayExitAssignmentStrategiesInSim::IsTaxiConditionAvaiable(AirsideFlightInSim* pFlight,TaxiSegmentData *pTaxiCondition)
{
	TaxiConditionInSim *pCondition = NULL;

	int nInitializeCount = static_cast<int>(m_vTaxiConditionInSim.size()); 
	for (int nCondition = 0; nCondition < nInitializeCount; ++ nCondition)
	{
		TaxiConditionInSim *pTempCondition = m_vTaxiConditionInSim.at(nCondition);
		if(pTempCondition->getTaxiCondition() == pTaxiCondition)
		{
			pCondition = pTempCondition;
			break;;
		}
	}

	if(pCondition == NULL)
	{
		pCondition = new TaxiConditionInSim;
		pCondition->Init(pTaxiCondition, m_pTaxiwayResManager);
		m_vTaxiConditionInSim.push_back(pCondition);
	}
	ASSERT(pCondition != NULL);
	return pCondition->IsAvailable(pFlight);
	
}

double CRunwayExitAssignmentStrategiesInSim::GetFlightNormalDecelOnRunway(AirsideFlightInSim* pFlight)
{
	double dNormalLandingDist =pFlight->GetPerformance()->getMinLandingDist(pFlight) +  (pFlight->GetPerformance()->getMaxLandingDist(pFlight) -pFlight->GetPerformance()->getMinLandingDist(pFlight))* pFlight->GetRandomValue(); 	
	double dLangingSpd = pFlight->GetPerformance()->getLandingSpeed(pFlight);

	return dLangingSpd*dLangingSpd/(2.0* dNormalLandingDist);
}

double CRunwayExitAssignmentStrategiesInSim::GetFlightMaxDecelOnRunway(AirsideFlightInSim* pFlight)
{
	double dMinLandingDist = pFlight->GetPerformance()->getMinLandingDist(pFlight);
	double dLangingSpd = pFlight->GetPerformance()->getLandingSpeed(pFlight);

	return dLangingSpd*dLangingSpd/(2.0* dMinLandingDist);
}

bool CRunwayExitAssignmentStrategiesInSim::IsExitAvailable( RunwayExitInSim *pRunwayExit )
{
	FlightGroundRouteDirectSegInSim *pGroundSeg = pRunwayExit->GetRouteSeg();
	if(pGroundSeg->GetType() == AirsideResource::ResType_RunwayDirSeg)
	{
		RunwayDirectSegInSim *pRunwaySeg = (RunwayDirectSegInSim *)pGroundSeg;

		RUNWAY_MARK runwayMark = pRunwaySeg->GetLogicRunwayType();
		LogicRunwayInSim * pRelatedRunway = pRunwaySeg->GetRunwaySeg()->GetLogicRunway(runwayMark);
		if(pRelatedRunway->GetDepartureQueueLength() != 0)
			return false;
	}
	

	return true;;
}

RunwayExitInSim* CRunwayExitAssignmentStrategiesInSim::GetAvailableRunwayExit( AirsideFlightInSim *pFlight, CAirportDatabase *pAirportDatabase,
																			  StandResourceManager* pStandResManager,bool bCheckCanHold )
{
	std::vector<RunwayExitInSim*> vExits = GetRunwayExitByFlightType(pFlight,pAirportDatabase,pStandResManager,bCheckCanHold);

	int nCount = (int)vExits.size();
	std::vector<RunwayExitInSim*> vAvailableExittoRunways;
	for (int i =0; i < nCount; i++)
	{
		RunwayExitInSim* pRunwayExit = vExits.at(i);
		//if (!IsExitAvailable(pRunwayExit))
		//	continue;

		FlightGroundRouteDirectSegInSim *pGroundRoute = pRunwayExit->GetRouteSeg();
		if(pGroundRoute->GetType() == AirsideResource::ResType_RunwayDirSeg)
		{
			RunwayDirectSegInSim *pRunwaySeg = (RunwayDirectSegInSim *)pGroundRoute;

			RUNWAY_MARK runwayMark = pRunwaySeg->GetLogicRunwayType();
			LogicRunwayInSim * pRelatedLogicRunway = pRunwaySeg->GetRunwaySeg()->GetLogicRunway(runwayMark);

			if(pRelatedLogicRunway == pRunwayExit->GetLogicRunway() 
				|| pRelatedLogicRunway == pRunwayExit->GetLogicRunway()->GetOtherPort())//same runway cannot be runway exit
			{
				vExits.erase(vExits.begin()+i);
				nCount--;
				continue;
			}
			else
			{
				vAvailableExittoRunways.push_back( pRunwayExit);
				vExits.erase(vExits.begin()+i);
				nCount--;
				continue;
			}
		}
				

		//if (pRunwayExit->CanHoldFlight(pFlight))
		return pRunwayExit;

	}
	if(nCount >0)	// if with runway exit and no available exit, assign a random one
	{
		int nRand = rand()%nCount;
		return vExits.at(nRand);
	}

	if(vAvailableExittoRunways.size() > 0)
	{
		int nRand = rand()%vAvailableExittoRunways.size();
		return vAvailableExittoRunways.at(nRand);
	}

	vExits = GetMaxBrakingFirstExitOnEitherSide(pFlight,pFlight->GetLandingRunway()/*,pStandResManager*/);
	
	for(int i=0;i<(int)vExits.size();i++)
	{
		RunwayExitInSim* pExit = vExits.at(i);
		if(bCheckCanHold)
		{
			if(pExit->CanHoldFlight(pFlight))
				return pExit;	
		}	
		else
			return pExit;
	}

	//ASSERT(0);		//error
	return NULL;
}



CRunwayExitAssignmentStrategiesInSim::TaxiConditionInSim::TaxiConditionInSim()
{
	m_pTaxiSeg = NULL;
}

CRunwayExitAssignmentStrategiesInSim::TaxiConditionInSim::~TaxiConditionInSim()
{

}

TaxiSegmentData * CRunwayExitAssignmentStrategiesInSim::TaxiConditionInSim::getTaxiCondition()
{
	return m_pTaxiSeg;
}

void CRunwayExitAssignmentStrategiesInSim::TaxiConditionInSim::Init( TaxiSegmentData *pTaxiSeg, TaxiwayResourceManager *pTaxiwayResManager )
{
	m_pTaxiSeg = pTaxiSeg;
	ASSERT(m_pTaxiSeg != NULL);
	if(m_pTaxiSeg == NULL)
		return;

	m_taxiwayDirectSegLst.clear();
	pTaxiwayResManager->GetTaxiwaySegment( m_pTaxiSeg->m_iTaxiwayID, m_pTaxiSeg->m_iStartNode, m_pTaxiSeg->m_iEndNode, m_taxiwayDirectSegLst);
}

bool CRunwayExitAssignmentStrategiesInSim::TaxiConditionInSim::IsAvailable( AirsideFlightInSim* pFlight )
{
	int nSegCount = static_cast<int>(m_taxiwayDirectSegLst.size());
	for (int nSeg = 0; nSeg < nSegCount; ++ nSeg)
	{
		FlightGroundRouteDirectSegInSim *pSegInSim = m_taxiwayDirectSegLst.at(nSeg);
		if(pSegInSim == NULL)
			continue;
		std::vector<AirsideFlightInSim* > vecFlights;
		pSegInSim->GetInPathAirsideFlightList(vecFlights);
		if (vecFlights.size())
		{
			return false;
		}
		FlightGroundRouteDirectSegInSim *pOppositeSegInSim = pSegInSim->GetOppositeSegment();
		if(pOppositeSegInSim == NULL)
			continue;
		
		pOppositeSegInSim->GetInPathAirsideFlightList(vecFlights);
		if (vecFlights.size())
		{
			return false;
		}	
	}

	return true;
}
