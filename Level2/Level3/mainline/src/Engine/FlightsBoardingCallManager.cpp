/*
	Note:
		1. when flight arrive stand, air side Sim Engine known flight's departure time,
		it's planned time not actual departure time.
*/
#include "StdAfx.h"
#include "FlightsBoardingCallManager.h"
#include "FlightItemBoardingCallMgr.h"
#include "proclist.h"
#include "Inputs\IN_TERM.H"
#include "Inputs\fltdata.h"
#include "inputs\schedule.h"
#include "inputs\flight.h"
#include "boardcal.h"
#include "Hold.h"
#include "SimEngineConfig.h"
#include "FlightDelaysUtility.h"
#include "../Inputs/SinglePaxTypeFlow.h"
#include "../Inputs/AllPaxTypeFlow.h"
#include "../Inputs/PaxFlowConvertor.h"
#include "../Inputs/SubFlowList.h"
#include "../Inputs/SubFlow.h"
#include "../Common/ARCTracker.h"
#include <Common/ProbabilityDistribution.h>
#include "Inputs/BoardingCallFlightTypeDatabase.h"

FlightsBoardingCallManager::FlightsBoardingCallManager(void)
:m_pFlightSchedule(NULL)
{
}

FlightsBoardingCallManager::~FlightsBoardingCallManager(void)
{
	ResetContext();
}

void FlightsBoardingCallManager::ResetContext()
{
	m_pFlightSchedule = NULL;

	for (size_t i=0; i<m_vectFltItemBoardingCallMgr.size(); i++)
	{
		delete m_vectFltItemBoardingCallMgr.at(i);
		m_vectFltItemBoardingCallMgr.at(i) = NULL;
	}
	m_vectFltItemBoardingCallMgr.clear();

}

void FlightsBoardingCallManager::Initialize(const ProcessorList *p_procs, const FlightSchedule *pSchedule, 
											const FlightData *p_data, InputTerminal* _pInTerm)
{
	PLACE_METHOD_TRACK_STRING();
	ResetContext();
	m_pFlightSchedule = pSchedule;

	LoadDefaultBoardingCalls(p_procs, p_data, _pInTerm);

	//active default
	if( !simEngineConfig()->isSimAirsideMode())
	{
		// use flight delay when sim terminal mode only.
		if(simEngineConfig()->isSimTerminalModeAlone())
			FlightDelaysUtility::impactFlightDelaysEffectToBoardingCall( this, pSchedule);
		ScheduleBoardingCallEventsDirectly();
	}
	else
	{
		ScheduleSecondBoardingCallEvents();
	}
}


bool FlightsBoardingCallManager::CheckBoardingCallValid(const Flight* pFlight,const HoldingArea* pHoldArea,InputTerminal* _pInTerm)
{
	ASSERT(pFlight);

	ProcessorID ProcID;
	ProcID = *(pHoldArea->getID());
	std::vector<ProcessorID> ProcIDList;
	GetProcIDInSubFlowList(ProcID,ProcIDList,_pInTerm);

	CPaxFlowConvertor tempFlowConvetor;
	tempFlowConvetor.SetInputTerm( _pInTerm );
	CAllPaxTypeFlow tempAllPaxFlow;
	tempAllPaxFlow.FromOldToDigraphStructure( tempFlowConvetor );
	int iPaxFlowCount = tempAllPaxFlow.GetSinglePaxTypeFlowCount();
	for( int i=0; i<iPaxFlowCount; ++i )
	{
		CSinglePaxTypeFlow* pSingleFlow = tempAllPaxFlow.GetSinglePaxTypeFlowAt( i );
		const CMobileElemConstraint* pConstrint = pSingleFlow->GetPassengerConstrain();
		CSinglePaxTypeFlow operaterFlow( _pInTerm );
		operaterFlow.SetPaxConstrain( pConstrint );
		if( pConstrint->GetTypeIndex() != 0 )
			continue;

		tempAllPaxFlow.BuildHierarchyFlow( tempFlowConvetor, i , operaterFlow );


		if(operaterFlow.IfFits(ProcID)|| IsSubFlowInPaxFlow(operaterFlow,ProcIDList))
		{
 			const BoardingCallFlightTypeDatabase* pFlightTypeDB = _pInTerm->flightData->GetStageInformation(pHoldArea->getStageID()-1);
			if (pFlightTypeDB == NULL)
				continue;
 
			int flightTypeCount = pFlightTypeDB->getCount();
			for (int j = 0; j < flightTypeCount; j++)
			{
				const FlightConstraint* pFlightConstraint = pFlightTypeDB->getConstraint(j);
				const FlightConstraint& fltCons(*pConstrint);
				if (pFlightConstraint->fits(pFlight->getType('D'))\
					&&pFlightConstraint->fits(fltCons)\
					&& fltCons.fits(pFlight->getType('D')))
				{
					ALTObjectID standID = pFlight->getDepStand();
					if (standID.IsBlank())
					{
						return false;
					}
				}
			}
		}
	}
	
	return true;
}

bool FlightsBoardingCallManager::GetProcIDInSubFlowList(const ProcessorID& ProcID,std::vector<ProcessorID>& ProIDList,InputTerminal* _pInTerm)
{
	CSubFlow* pSubFlow = NULL;
	int nCount = _pInTerm->m_pSubFlowList->GetProcessUnitCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		pSubFlow = _pInTerm->m_pSubFlowList->GetProcessUnitAt(nIndex);
		CSinglePaxTypeFlow* pInterFlow = NULL;
		ProcessorID id;
		id.SetStrDict(_pInTerm->inStrDict);
		if(pSubFlow)
		{
			pInterFlow = pSubFlow->GetInternalFlow();
			if(pInterFlow->IfFits(ProcID))
			{
				id.setID(pSubFlow->GetProcessUnitName());
				ProIDList.push_back(id);
			}
		}
	}
	return true;
}

bool FlightsBoardingCallManager::IsSubFlowInPaxFlow(const CSinglePaxTypeFlow& pSingleFlow,const std::vector<ProcessorID>& ProIDList)
{
	int nCount = (int)ProIDList.size();
	for (int i = 0; i < nCount; i++)
	{
		const ProcessorID& procID = ProIDList.at(i);
		if (pSingleFlow.IfFits(procID))
		{
			return true;
		}
	}
	return false;
}

void FlightsBoardingCallManager::LoadDefaultBoardingCalls(const ProcessorList *p_procs, const FlightData *_fltData, InputTerminal* _pInTerm)
{

	if (!p_procs->getProcessorsOfType (GateProc))
		return;

	CMobileElemConstraint mobElemConst(_pInTerm);
	Flight *pFlight;

	double percent;
	BoardingCallEvent *event;
	ProcessorArray vHoldingAreas;

	_pInTerm->procList->getProcessorsOfType (HoldAreaProc, vHoldingAreas);
	int iHoldAreaCount = vHoldingAreas.getCount();
	int iFltCount = m_pFlightSchedule->getCount();
	for (int i = 0; i < iFltCount; i++)
	{
		pFlight = m_pFlightSchedule->getItem(i);
		if (pFlight->isDeparting())
		{
			ALTObjectID standID = pFlight->getDepStand();

			ProcessorID procID;
			procID.SetStrDict(_pInTerm->inStrDict);
			procID.setID(standID.GetIDString());
 			if (procID.isBlank())
				continue;

			std::map<int,ElapsedTime>& mapLastCalls  = pFlight->GetLastCallOfEveryStage();
			mapLastCalls.clear();
			FlightConstraint& fltConst = pFlight->getType ('D');
			ElapsedTime fltDepTime = pFlight->getDepTime();
			for( int k=0; k<iHoldAreaCount; ++k )
			{
				HoldingArea* pHoldArea = ( HoldingArea* )vHoldingAreas.getItem( k );
				int iStage = pHoldArea->getStageID();
				if( iStage <= 0 )
					continue;
 				if(!CheckBoardingCallValid(pFlight, pHoldArea, _pInTerm))
 				{
 					char buf[128];
 					pFlight->getFullID(buf,'D');
 					throw new ARCSystemError("Flight (" + CString(buf) + ")  using boarding call must have a departure stand","",fltDepTime.printTime());
 				}

				if( mapLastCalls.find( iStage ) == mapLastCalls.end() )//not exist
				{
					BoardingCallFlightTypeDatabase* pFltTypeDB = _fltData->GetStageInformation(iStage -1);
					int ifltTypeCount = pFltTypeDB->getCount();
					for(int iFlt=0; iFlt<ifltTypeCount; iFlt++)
					{
						if(pFltTypeDB->getConstraint(iFlt)->fits(fltConst))
						{
							BoardingCallStandDatabase* pStandDB = ((BoardingCallFlightTypeEntry*)pFltTypeDB->getItem(iFlt))->GetStandDatabase();
							int standCount = pStandDB->getCount();
							for(int iStand=0; iStand<standCount; iStand++)
							{
								if(pStandDB->getItem(iStand)->getID()->idFits(procID))
								{
									BoardingCallPaxTypeDatabase* pPaxTypeDB = ((BoardingCallStandEntry*)pStandDB->getItem(iStand))->GetPaxTypeDatabase();
									int paxTypeCount = pPaxTypeDB->getCount();
									for(int iPax=0; iPax<paxTypeCount; iPax++)
									{
										BoardingCallPaxTypeEntry* pPaxTypeEntry = (BoardingCallPaxTypeEntry*)pPaxTypeDB->getItem(iPax);
										mobElemConst = *((CMobileElemConstraint*)pPaxTypeEntry->getConstraint());
										mobElemConst.MergeFlightConstraint(&(pFlight->getType ('D')));
										std::vector<BoardingCallTrigger>* vTrigger = pPaxTypeEntry->GetTriggersDatabase();
										int triggerCount = vTrigger->size();
										ElapsedTime tempTime;
										double releasedProp = 0.0;
										for(int iStage=0; iStage<triggerCount; iStage++)
										{
											tempTime = fltDepTime + vTrigger->at(iStage).GetTriggerTime();
											
											if(iStage < triggerCount-1)
											{
												percent = vTrigger->at(iStage).GetTriggerProportion() / (100.0 - releasedProp);
												releasedProp += vTrigger->at(iStage).GetTriggerProportion();
											}
											else/* the 'residual' trigger. */
											{
												percent = 1.0f;
												mapLastCalls.insert(std::map<int,ElapsedTime>::value_type(iStage, tempTime));
											}
											event = new BoardingCallEvent;
											event->setTime (tempTime);
											event->init ((float)percent, NULL, mobElemConst, iStage);
											
											LoadDefaultBoardingCallEventToFltItem(pFlight, event);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void FlightsBoardingCallManager::LoadDefaultBoardingCallEventToFltItem(const Flight* pFlight, BoardingCallEvent* pEvent)
{
	FlightItemBoardingCallMgr* pItem = GetFlightItemBoardingCallMgr(pFlight);

	if(pItem == NULL)
	{
		int nFltIndex = pFlight->getFlightIndex();
		ASSERT(nFltIndex != -1);
		pItem = new FlightItemBoardingCallMgr(nFltIndex);
		pItem->SetPlannedDepTime(pFlight->getDepTime());
		m_vectFltItemBoardingCallMgr.push_back(pItem);
	}

	pItem->AddBoardingCallEvent(pEvent);

}

//int FlightsBoardingCallManager::GetFlightIndex(const Flight* pFlight)
//{
//	int nFltIndex = 0;
//	int numFlights = m_pFlightSchedule->getCount();
//
//	for (; nFltIndex < numFlights; nFltIndex++)
//	{
//		Flight* pItem = m_pFlightSchedule->getItem(nFltIndex);
//		if(pItem->IsEqual(pFlight))
//			break;
//	}
//	if(nFltIndex<numFlights)
//		return nFltIndex;
//
//	return -1;
//}

FlightItemBoardingCallMgr* FlightsBoardingCallManager::GetFlightItemBoardingCallMgr(const Flight* pFlight)
{
	int nFltIndex = pFlight->getFlightIndex();
	if(nFltIndex == -1)	
		return NULL;

	for (size_t i=0; i<m_vectFltItemBoardingCallMgr.size(); i++)
	{
		FlightItemBoardingCallMgr* pItem = m_vectFltItemBoardingCallMgr.at(i);
		if(pItem->GetFlightIndex() == nFltIndex)
			return pItem;
	}

	return NULL;
}

void FlightsBoardingCallManager::ScheduleBoardingCallEvents(const Flight* pFlight, const ElapsedTime& actualDepTime,const ElapsedTime& depTime)
{
	if(m_pFlightSchedule == NULL)
		return;

	FlightItemBoardingCallMgr* pItem = GetFlightItemBoardingCallMgr(pFlight);
	
	if(pItem == NULL || pItem->IsStarted())
		return;

	pItem->ScheduleBoardingCallEvents(actualDepTime,depTime);
}

void FlightsBoardingCallManager::ScheduleFltWithoutStandBoardingCallEvents()
{
	
	for (size_t i=0; i<m_vectFltItemBoardingCallMgr.size(); i++)
	{
		FlightItemBoardingCallMgr* pItem = m_vectFltItemBoardingCallMgr.at(i);
		Flight* pFlight = m_pFlightSchedule->getItem(pItem->GetFlightIndex());
		ASSERT(pFlight != NULL);

		// flight's boarding call will be invoke by AirsideSimEngine
		if(pFlight->isArriving())
			continue;

		pItem->ScheduleDirectly(); 
	}
}

void FlightsBoardingCallManager::ScheduleBoardingCallEventsDirectly()
{
	for (size_t i=0; i<m_vectFltItemBoardingCallMgr.size(); i++)
	{
		m_vectFltItemBoardingCallMgr.at(i)->ScheduleDirectly();
	}
}

void FlightsBoardingCallManager::ScheduleSecondBoardingCallEvents()
{
	for (size_t i=0; i<m_vectFltItemBoardingCallMgr.size(); i++)
	{
		m_vectFltItemBoardingCallMgr.at(i)->ScheduleSecondDirectly();
	}
}





