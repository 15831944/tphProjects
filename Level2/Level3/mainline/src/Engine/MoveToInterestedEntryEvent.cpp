#include "stdafx.h"
#include "MoveToInterestedEntryEvent.h"
#include "TerminalMobElementBehavior.h"
#include "Inputs\MISCPROC.H"
#include "FIXEDQ.H"
#include "Inputs\Pipe.h"
#include "PERSON.H"
#include "Inputs\PipeDataSet.h"
#include "ARCportEngine.h"
#include "Inputs\PipeInterSectionPoint.h"
#include "Inputs\PipeGraph.h"

MoveToInterestedEntryEvent::MoveToInterestedEntryEvent()
{
	m_pipePointList.clear();
}

MoveToInterestedEntryEvent::~MoveToInterestedEntryEvent()
{

}

void MoveToInterestedEntryEvent::init(MobileElement *aMover, ElapsedTime eventTime, bool bMoLog)
{
	MobileElementMovementEvent::init(aMover, eventTime , bMoLog);
}

// processor event
int MoveToInterestedEntryEvent::process(CARCportEngine *_pEngine)
{
	Person* pPerson = (Person*)mobileElement;
	TerminalMobElementBehavior* pTerminalBehavior = 
		(TerminalMobElementBehavior*)pPerson->getBehavior(MobElementBehavior::TerminalBehavior);
	Processor* pProc = pTerminalBehavior->getProcessor();
	FixedQueue* pFixQ = (FixedQueue*)pProc->GetQueue();
	ASSERT(pFixQ->isFixed() == 'Y');
	int nPerson = pPerson->getID();
	if(!m_pipePointList.empty())
	{
		pPerson->setState(WalkOnPipe);
		pPerson->writeLogEntry(time, false);
		pPerson->setState(MoveToInterestedEntryPoint);
		pTerminalBehavior->setDestination(m_pipePointList.front());
		m_pipePointList.erase(m_pipePointList.begin());
		MoveToInterestedEntryEvent* pEvent = new MoveToInterestedEntryEvent;
		pEvent->init(mobileElement, time + pTerminalBehavior->moveTime(), false);
		pEvent->m_pipePointList = m_pipePointList;
		pEvent->addEvent();
		pPerson->SetPrevEventTime(pEvent->getTime());
	}
	else
	{
		int nCurEntryPoint = pFixQ->getFixQEntryPointIndex();
		if(pTerminalBehavior->getEntryPointCorner() != nCurEntryPoint)
		{
			calculateMovingPipe(pTerminalBehavior->getDest(), pFixQ->corner(nCurEntryPoint));
			pTerminalBehavior->setEntryPointCorner(nCurEntryPoint);
			MoveToInterestedEntryEvent* pEvent = new MoveToInterestedEntryEvent;
			pEvent->init(mobileElement, time, false);
			pEvent->m_pipePointList = m_pipePointList;
			pEvent->addEvent();
			pPerson->SetPrevEventTime(pEvent->getTime());
			return 0;
		}
		pPerson->setState(MoveToQueue);
		Point ptDest =  pFixQ->corner(nCurEntryPoint);
		pPerson->writeLogEntry(time, false);
		pTerminalBehavior->setDestination(ptDest);
		pProc->addToQueue((Person*)mobileElement, time + pTerminalBehavior->moveTime());
		pPerson->generateEvent(time + pTerminalBehavior->moveTime(), false);
	}
	return 0;
}

void MoveToInterestedEntryEvent::calculateMovingPipe(Point ptFrom, Point ptTo)
{
	Person* pPerson = (Person*)mobileElement;
	int nPerson = pPerson->getID();
	TerminalMobElementBehavior* spTerminalBehavior = 
		(TerminalMobElementBehavior*)pPerson->getBehavior(MobElementBehavior::TerminalBehavior);
	pPerson->setState(MoveToQueue);
	Processor* pProc = spTerminalBehavior->getProcessor();
	FixedQueue* pFixQ = (FixedQueue*)pProc->GetQueue();
    CFlowItemEx* pFlowItem = findFlowItem();
	if(pFlowItem == NULL)
		return;
	switch(pFlowItem->GetTypeOfUsingPipe())
	{
	case USE_NOTHING:
		break;
	case USE_USER_SELECTED_PIPES: // user pipe has some problem, use system pipe instead.
                                  // mobile element will walk along the pipe list until the end of it, then
                                  // move to the destination point.
// 		{
// 			calculateUserPipe(ptFrom, ptTo, pFlowItem);
// 		}
// 		break;
	case USE_PIPE_SYSTEM:
		{
			calculateSystemPipe(ptFrom, ptTo);
		}
		break;
	default:
		break;
	}
	return;
}

void MoveToInterestedEntryEvent::calculateUserPipe(Point ptFrom, Point ptTo, CFlowItemEx* pFlowItem)
{
	Person* pPerson = (Person*)mobileElement;
	int nPerson = pPerson->getID();
	TerminalMobElementBehavior* spTerminalBehavior = 
		(TerminalMobElementBehavior*)pPerson->getBehavior(MobElementBehavior::TerminalBehavior);
	Processor* pProcessor = spTerminalBehavior->getProcessor();
	FixedQueue* pFixQ = (FixedQueue*)pProcessor->GetQueue();
	ASSERT(pFixQ->isFixed() == 'Y');
	InputTerminal* pTerm = (InputTerminal*)pPerson->GetTerminal();
	std::vector<int> vPipeList1 = pFlowItem->GetPipeVector();
	int nPipeCount = vPipeList1.size();
	if(nPipeCount == 0)
		return;

	std::vector<int> vPipeList2;
	for(int i=0; i<nPipeCount; i++)
	{
		CPipe* pPipe = pTerm->m_pPipeDataSet->GetPipeAt(vPipeList1[i]);
		if(pPipe->GetZ() == spTerminalBehavior->getPoint().getZ())
		{
			vPipeList2.push_back(vPipeList1[i]);
		}
	}
	nPipeCount = vPipeList2.size();
	if(nPipeCount == 0)
		return;

	// get the index and intersection of the source processor with pipe.
	int nOldState = pPerson->getState();

	CPointToPipeXPoint entryPoint;
	CPointToPipeXPoint exitPoint;
	std::vector<CMobPipeToPipeXPoint> vMidPoint;	// num count should be nPipeCount - 1

	CPipe* pPipe1 = NULL;
	CPipe* pPipe2 = NULL;

	for(int ii=0; ii<nPipeCount; ii++)
	{
		if(ii == 0)
		{
			pPipe1 = pTerm->m_pPipeDataSet->GetPipeAt(vPipeList2[0]);
			entryPoint = pPipe1->GetIntersectionPoint(ptFrom);

			if(nPipeCount == 1)
			{
				exitPoint = pPipe1->GetIntersectionPoint(ptTo);
			}
			else
			{
				pPipe2 = pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[1] );
				CMobPipeToPipeXPoint midPt;
				if(pPipe1->GetIntersectionPoint(pPipe2, entryPoint, midPt))
				{
					vMidPoint.push_back(midPt);
				}
				else
				{
					//throw new NoValidFlowError (m_pPerson);
					pPerson->kill(time);
					throw new ARCPipeNotIntersectError(pPipe1->GetPipeName(), pPipe2->GetPipeName(), 
						"", spTerminalBehavior->ClacTimeString(time));
				}
			}
		}
		else if(ii == nPipeCount-1)
		{
			exitPoint = pPipe1->GetIntersectionPoint(ptTo);
			vMidPoint[vMidPoint.size()-1].SetOutInc(exitPoint);
		}
		else
		{
			pPipe2 = pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[ii+1] );
			CMobPipeToPipeXPoint midPt;
			if( pPipe1->GetIntersectionPoint( pPipe2, vMidPoint[vMidPoint.size()-1], midPt ) )
			{
				vMidPoint[vMidPoint.size()-1].SetOutInc( midPt );
				vMidPoint.push_back( midPt );
			}
			else
			{
				//throw new NoValidFlowError (m_pPerson);
				pPerson->kill(time);
				throw new ARCPipeNotIntersectError( pPipe1->GetPipeName(), pPipe2->GetPipeName(), 
					"", spTerminalBehavior->ClacTimeString(time));
			}
		}
		pPipe1 = pPipe2;
	}

	ElapsedTime eventTime = time;

	// process entry point
	CPipe* pPipe = pTerm->m_pPipeDataSet->GetPipeAt(vPipeList2[0]);

	std::vector<CTimePointOnSideWalk> pointList;
	int nPercent = random(100);
	int nMidPoint = vMidPoint.size();

	if(nMidPoint == 0)
	{
		pPipe->GetPointListForLog(vPipeList2[0], entryPoint, exitPoint, nPercent, pointList);
		m_pipePointList.insert(m_pipePointList.end(), pointList.begin(), pointList.end());		
		pointList.clear();
	}
	else
	{
		pPipe->GetPointListForLog(vPipeList2[0], entryPoint, vMidPoint[0], nPercent,pointList);
		m_pipePointList.insert(m_pipePointList.end(), pointList.begin(), pointList.end());

		pointList.clear();
		// process mid point 
		for(int i=1; i<nMidPoint; i++ )
		{
			pPipe = pTerm->m_pPipeDataSet->GetPipeAt(vPipeList2[i]);
			if(vMidPoint[i-1].OrderChanged())
				nPercent = 100 - nPercent;

			pPipe->GetPointListForLog(vPipeList2[i],vMidPoint[i-1], vMidPoint[i], nPercent ,pointList);

			m_pipePointList.insert(m_pipePointList.end(), pointList.begin(), pointList.end());
			pointList.clear();
		}

		// process exit point
		pPipe = pTerm->m_pPipeDataSet->GetPipeAt(vPipeList2[nPipeCount-1]);
		if(vMidPoint[nPipeCount-2].OrderChanged())
			nPercent = 100 - nPercent;
		pPipe->GetPointListForLog(vPipeList2[nPipeCount-1], vMidPoint[nMidPoint-1], exitPoint, nPercent, pointList);
        m_pipePointList.insert(m_pipePointList.end(), pointList.begin(), pointList.end());
		pointList.clear();
	}
}

void MoveToInterestedEntryEvent::calculateSystemPipe(Point ptFrom, Point ptTo)
{
	Person* pPerson = (Person*)mobileElement;
	TerminalMobElementBehavior* spTerminalBehavior = 
		(TerminalMobElementBehavior*)pPerson->getBehavior(MobElementBehavior::TerminalBehavior);
	int nPerson = pPerson->getID();
	Processor* pProcessor = spTerminalBehavior->getProcessor();
	FixedQueue* pFixQ = (FixedQueue*)pProcessor->GetQueue();
	ASSERT(pFixQ->isFixed() == 'Y');
	InputTerminal* pTerm = (InputTerminal*)pPerson->GetTerminal();

	// if not same floor, need not move in pipe
	int ptFromFloor = (int)(ptFrom.getZ() / SCALE_FACTOR);
	int ptToFloor = (int)(ptTo.getZ() / SCALE_FACTOR);
	if (ptFromFloor != ptToFloor)
		return;

	// if no pipe , need not to move in pipe
	CPipeGraphMgr* pPipeMgr = pTerm->m_pPipeDataSet->m_pPipeMgr;
	if (!pPipeMgr->checkCanMoveByPipe(ptToFloor))
		return;


	CGraphVertexList shortestPath;
	if (!pPipeMgr->getShortestPathFromLib(ptFrom, ptTo, shortestPath))
		return;

	int nVertexCount = shortestPath.getCount();
	if (nVertexCount < 3)
		return;

	// get the real path, and write log
	PTONSIDEWALK logPointList;
	int iPercent = random(100);
	pTerm->m_pPipeDataSet->GetPointListForLog(shortestPath, iPercent, logPointList);

	m_pipePointList.insert(m_pipePointList.begin(), logPointList.begin(), logPointList.end()); 
}

CFlowItemEx* MoveToInterestedEntryEvent::findFlowItem()
{
	TerminalMobElementBehavior* spTerminalBehavior = 
		(TerminalMobElementBehavior*)((Person*)mobileElement)->getBehavior(MobElementBehavior::TerminalBehavior);
	CProcessorDistributionWithPipe* pFlowItemList = 
		(CProcessorDistributionWithPipe*)spTerminalBehavior->getProDistribution();
	std::vector<CFlowItemEx>& pipeIndexList = ((CProcessorDistributionWithPipe*)pFlowItemList)->GetPipeVector();

	Processor* pProc = spTerminalBehavior->getProcessor();
	FixedQueue* pFixQ = (FixedQueue*)pProc->GetQueue();
	ASSERT(pFixQ->isFixed() == 'Y');
	int nProCount = pFlowItemList->GetDestCount();
	for(int i=0; i<nProCount; i++)
	{
		ProcessorID *pProcID = pFlowItemList->getGroup(i);
		CString str1 = pProc->getIDName();
		CString str2 = pProcID->GetIDString();
		if(pProcID->idFits(*pProc->getID()))
			return &pipeIndexList.at(i);
	}
	return NULL;
}
