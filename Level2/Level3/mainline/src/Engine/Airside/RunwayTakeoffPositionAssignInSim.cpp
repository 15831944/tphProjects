#include "StdAfx.h"
#include ".\runwaytakeoffpositionassigninsim.h"
#include "InputAirside/RunwayTakeOffPosition.h"
#include "RunwayInSim.h"
#include "common/elaptime.h"
#include "AirsideFlightInSim.h"
#include "common/AirportDatabase.h"
#include "AirTrafficController.h"
#include "AirportResourceManager.h"
#include "AirsideResourceManager.h"
#include "RunwayResourceManager.h"
#include "SimulationErrorShow.h"

const int MAX_QUEUE = 100;
CRunwayTakeoffPositionAssignInSim::CRunwayTakeoffPositionAssignInSim(void)
{

}

CRunwayTakeoffPositionAssignInSim::~CRunwayTakeoffPositionAssignInSim(void)
{
}

void CRunwayTakeoffPositionAssignInSim::Init(int nPrjId,CAirportDatabase* pDatabase,RunwayResourceManager * pRunwayResManager)
{
	m_pRunwayTakeoffPosition = new CRunwayTakeOffPosition(nPrjId);
	m_pRunwayTakeoffPosition->SetAirportDB(pDatabase);
	m_pRunwayTakeoffPosition->ReadData();

	ShowErrorMessageAboutTakeoffPos(pRunwayResManager);
}

RunwayExitInSim* CRunwayTakeoffPositionAssignInSim::GetTakeoffPositionByFlightType(LogicRunwayInSim* pPort, AirsideFlightInSim* pFlight)
{

	//LogicRunwayInSim* pPort = pFlight->GetTakeoffRunway();
	if (m_mapTakeoffPosList.find(pPort)== m_mapTakeoffPosList.end()) //can not find
	{
		GetRunwayTakeoffPositions(pPort);
	}

	//std::vector<int> vTakeoffPosID;
	std::vector<RunwayExitDescription> vExitList;
	vExitList.clear();

	for (int i =0; i < int(m_mapTakeoffPosList[pPort].size()); i++)
	{
		int nCount = (m_mapTakeoffPosList[pPort].at(i))->GetTimeCount();
		for (int j =0; j < nCount; j++)
		{
			CRunwayTakeOffTimeRangeData* pTimeRangeData = (m_mapTakeoffPosList[pPort].at(i))->GetTimeItem(j);
			if (pFlight->GetTime() >= pTimeRangeData->GetStartTime() && pFlight->GetTime() <= pTimeRangeData->GetEndTime())
			{
				for (int k =0; k < pTimeRangeData->GetFltTypeCount(); k++)
				{

					FlightConstraint& fltCnst = pTimeRangeData->GetFltTypeItem(k)->GetFltType();

					if(	pFlight->fits(fltCnst) )
					{
						CRunwayTakeOffPositionData* pIntersectData = m_mapTakeoffPosList[pPort].at(i);
						vExitList.push_back( pIntersectData->GetRunwayExitDescription());
						break;

					}
				}
			}
		}
	}
	
	//std::vector<int> vLinkNodeIDs;
	IntersectionNodeInSim * pStandNode = NULL;
	if (pFlight->GetOperationParkingStand())
	{
		pStandNode = pFlight->GetOperationParkingStand()->GetMaxDistOutNode();
	}


	//for(size_t i=0;i< vTakeoffPosID.size();i++)
	//{
	//	IntersectionNodeInSim * pNode = pAirportRes->GetIntersectionNodeList().GetNodeByID(vTakeoffPosID[i]);
	//	if( pAirportRes->getTaxiwayResource()->bLinked(pStandNode, pNode, pFlight) )
	//	{
	//		vLinkNodeIDs.push_back(vTakeoffPosID[i]);
	//	}
	//}

	std::vector<RunwayExitInSim* > vValidExits;
	vValidExits.clear();
	if(vExitList.size())
	{
		for(int i=0;i<pPort->GetExitCount();i++)
		{
			RunwayExitInSim * pExitInSim = pPort->GetExitAt(i);
			for (int j = 0; j < (int)vExitList.size(); j++)
			{
				RunwayExitDescription runwayDes = vExitList.at(j);
				if (pExitInSim->GetExitInput().EqualToRunwayExitDescription(runwayDes))
				{
					vValidExits.push_back(pExitInSim);
				}
			}
		}
	}

	//Assign same side exit
	int nMinQueueLength = MAX_QUEUE; 
	RunwayExitInSim* pTakeoffPos = NULL;

	for(size_t i=0;i< vValidExits.size();i++)
	{
		RunwayExitInSim * ptheExit = vValidExits[i];
		if( pStandNode && ptheExit->IsLeft() != pPort->IsLeftPoint( pStandNode->GetDistancePoint(0)) )
			continue;
		int nQueueLength = pPort->GetTakeoffPostionQueueLength(ptheExit);
		if (nQueueLength < nMinQueueLength)
		{
			pTakeoffPos = ptheExit;
			nMinQueueLength = nQueueLength;
		}
	}

	//if no suitable takeoff pos, assign a default exit	
	if(!vValidExits.empty() && pTakeoffPos == NULL)
	{
		return vValidExits[0];
	}

	//if no available takeoff pos,give a random exit 
	if (vValidExits.empty())
	{		
		std::vector<RunwayExitInSim*> vLinkedExits;
		vLinkedExits.clear();

		for(int i= 0 ;i< pPort->GetExitCount();i++)
		{
			RunwayExitInSim * pTheExit = pPort->GetExitAt(i);			
			//if( pAirportRes->getTaxiwayResource()->bLinked(pStandNode,pTheExit->GetTaxiwaySeg()->GetEntryNode(),pFlight) )
			{
				vLinkedExits.push_back(pTheExit);
			}
		}
		for(size_t i =0 ;i <vLinkedExits.size();i++)
		{
			RunwayExitInSim * pTheExit = pPort->GetExitAt(i);
			if( pTheExit->IsLeft() == pPort->IsLeftPoint(pFlight->GetPosition()) )
			{
				return pTheExit;
			}
		}
		if(!vLinkedExits.empty())
		{
			return vLinkedExits[0];
		}
	}

	
	return pTakeoffPos;
}

void CRunwayTakeoffPositionAssignInSim::CheckRunwayExitOfTakaoffPos(LogicRunwayInSim* pLogicRunway)
{
	Runway* pRunway = pLogicRunway->GetRunwayInSim()->GetRunwayInput();
	RUNWAY_MARK nRunwayMark = (pLogicRunway->getLogicRunwayType());

	CRunwayTakeOffRunwayWithMarkData* pLogicRunwayData = NULL;
	for (int nRunwayIdx = 0; nRunwayIdx < m_pRunwayTakeoffPosition->GetAirportItem(0)->GetRunwayCount(); nRunwayIdx++)
	{
		if (m_pRunwayTakeoffPosition->GetAirportItem(0)->GetRunwayItem(nRunwayIdx)->getRunway()->getID() == pRunway->getID())
		{
			if (nRunwayMark == RUNWAYMARK_FIRST)
				pLogicRunwayData = m_pRunwayTakeoffPosition->GetAirportItem(0)->GetRunwayItem(nRunwayIdx)->getRunwayMark1Data();
			else
				pLogicRunwayData = m_pRunwayTakeoffPosition->GetAirportItem(0)->GetRunwayItem(nRunwayIdx)->getRunwayMark2Data();
			break;				
		}
	}

	CRunwayTakeOffPositionData* pIntersectionData = NULL;
	if(pLogicRunwayData)
	{
		int nCount = pLogicRunwayData->GetRwyTakeoffPosCount();
		for (int i =0; i < nCount; i++)
		{
			pIntersectionData = pLogicRunwayData->GetRwyTakeoffPosItem(i);
			if (pIntersectionData->GetEligible())
			{
				bool bFind = false;
				for (int j = 0; j < pLogicRunway->GetExitCount(); j++)
				{
					RunwayExitInSim * pTheExit = pLogicRunway->GetExitAt(j);
					if (pTheExit->GetExitInput().EqualToRunwayExitDescription(pIntersectionData->GetRunwayExitDescription()))
					{
						bFind = true;
						break;
					}
				}

				if (!bFind)
				{
					CString strWarn = _T("");
					strWarn.Format(_T("Can not find runway exit: %s of runway: %s and taxiway: %s "),pIntersectionData->GetRunwayExitDescription().GetName(),\
						pIntersectionData->GetRunwayExitDescription().GetRunwayName(),pIntersectionData->GetRunwayExitDescription().GetTaxiwayName());
					CString strError = _T("DEFINE ERROR");
					AirsideSimErrorShown::SimWarning(NULL,strWarn,strError);
				}
			}			
		}
	}
}

void CRunwayTakeoffPositionAssignInSim::ShowErrorMessageAboutTakeoffPos(RunwayResourceManager * pRunwayResManager)
{
	for (int nRunwayResIdx = 0; nRunwayResIdx < pRunwayResManager->GetRunwayCount(); nRunwayResIdx++)
	{
		RunwayInSim* pRunwayInSim = pRunwayResManager->GetRunwayByIndex(nRunwayResIdx);
		LogicRunwayInSim* pLogicRunway1 = pRunwayInSim->GetLogicRunway1();
		CheckRunwayExitOfTakaoffPos(pLogicRunway1);

		LogicRunwayInSim* pLogicRunway2 = pRunwayInSim->GetLogicRunway2();
		CheckRunwayExitOfTakaoffPos(pLogicRunway2);
	}
}

void CRunwayTakeoffPositionAssignInSim::GetRunwayTakeoffPositions(LogicRunwayInSim* pLogicRunway)
{
	Runway* pRunway = pLogicRunway->GetRunwayInSim()->GetRunwayInput();
	RUNWAY_MARK nRunwayMark = (pLogicRunway->getLogicRunwayType());

	CRunwayTakeOffRunwayWithMarkData* pLogicRunwayData = NULL;

	for (int i = 0; i < m_pRunwayTakeoffPosition->GetAirportItem(0)->GetRunwayCount(); i++)
	{
		if (m_pRunwayTakeoffPosition->GetAirportItem(0)->GetRunwayItem(i)->getRunway()->getID() == pRunway->getID())
		{
			if (nRunwayMark == RUNWAYMARK_FIRST)
				pLogicRunwayData = m_pRunwayTakeoffPosition->GetAirportItem(0)->GetRunwayItem(i)->getRunwayMark1Data();
			else
				pLogicRunwayData = m_pRunwayTakeoffPosition->GetAirportItem(0)->GetRunwayItem(i)->getRunwayMark2Data();

			break;				
		}
	}

	CRunwayTakeOffPositionData* pIntersectionData = NULL;
	if(pLogicRunwayData)
	{
		int nCount = pLogicRunwayData->GetRwyTakeoffPosCount();
		for (int i =0; i < nCount; i++)
		{
			pIntersectionData = pLogicRunwayData->GetRwyTakeoffPosItem(i);
			if (pIntersectionData->GetEligible())
			{
				pIntersectionData = pLogicRunwayData->GetRwyTakeoffPosItem(i);
				m_mapTakeoffPosList[pLogicRunway].push_back(pIntersectionData);
			}			
		}


	}

}

BACKTRACKTYPE CRunwayTakeoffPositionAssignInSim::GetBacktrackTypeOfTakeoffPos(AirsideFlightInSim* pFlight)
{
	BACKTRACKTYPE tBacktrack = NoBacktrack;

	LogicRunwayInSim* pPort = pFlight->GetAndAssignTakeoffRunway();
	if (m_mapTakeoffPosList.find(pPort)== m_mapTakeoffPosList.end()) //can not find
	{
		GetRunwayTakeoffPositions(pPort);
	}

	for (int i =0; i < int(m_mapTakeoffPosList[pPort].size()); i++)
	{
		int nCount = (m_mapTakeoffPosList[pPort].at(i))->GetTimeCount();
		for (int j =0; j < nCount; j++)
		{
			CRunwayTakeOffTimeRangeData* pTimeRangeData = (m_mapTakeoffPosList[pPort].at(i))->GetTimeItem(j);

			if (pFlight->GetTime() >= pTimeRangeData->GetStartTime() && pFlight->GetTime() <= pTimeRangeData->GetEndTime())
				for (int k =0; k < pTimeRangeData->GetFltTypeCount(); k++)
				{

					FlightConstraint& fltCnst = pTimeRangeData->GetFltTypeItem(k)->GetFltType();

					if(	pFlight->fits(fltCnst) )
					{
						CRunwayTakeOffPositionData* pIntersectData = m_mapTakeoffPosList[pPort].at(i);
						if (pFlight->GetAndAssignTakeoffPosition()->GetExitInput().EqualToRunwayExitDescription(pIntersectData->GetRunwayExitDescription()))
						{
							CRunwayTakeOffTimeRangeFltType::BACKTRACK backtracktype = pTimeRangeData->GetFltTypeItem(k)->getBacktrakOp();
							if (backtracktype == CRunwayTakeOffTimeRangeFltType::MaxBacktrack)
								tBacktrack = MaxBacktrack;

							if (backtracktype == CRunwayTakeOffTimeRangeFltType::MinBacktrack)
								tBacktrack = MinBacktrack;

							return tBacktrack;
						}
					}
				}
		}
	}
	return tBacktrack;

}