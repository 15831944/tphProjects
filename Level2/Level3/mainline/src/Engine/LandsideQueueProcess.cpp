#include "StdAfx.h"
#include ".\landsidequeueprocess.h"
#include "PaxLandsideBehavior.h"
#include "PERSON.H"
#include "LandsidePaxSericeInSim.h"

LandsideQueueBase::LandsideQueueBase(const CPath2008 path)
:m_qCorners(path)
{

}

LandsideQueueBase::~LandsideQueueBase()
{

}

bool LandsideQueue::StepItValid( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys )
{
	PaxLandsideBehavior* pLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	ASSERT(pLandsideBehavior);
	ARCVector3 locationPt;
	ARCVector3 destPt;
	locationPt = pLandsideBehavior->getPoint();
	destPt = pLandsideBehavior->getDest();

 	if (isHead(pLandsideBehavior))
 		return true;

	Person* pPerson = pLandsideBehavior->getPerson();
	ASSERT(pPerson);
	double dInStep = pPerson->getInStep();

	int nSegment = pQueueProcess->GetSegment();
	if (nSegment == QUEUE_OVERFLOW)
	{
		PaxLandsideBehavior *pPreLandsideBehavior = NULL;
		int idx = m_approachList.findElement(pLandsideBehavior->GetPersonID());
		if (idx == 0)
		{
			pPreLandsideBehavior = GetTailBehavior();
		}
		else
		{
			pPreLandsideBehavior = m_approachList.getItem(idx - 1);
		}
		if (pPreLandsideBehavior == NULL)
			return true;

		LandsidePaxQueueProcess* pPreQueueProcess = pLandsideQueueSys->FindPaxQueueProcess(pPreLandsideBehavior);
		ASSERT(pPreQueueProcess);

		if (!pPreQueueProcess->GetStuck())
		{
			return true;
		}

		ARCVector3 locationNextPt = pPreLandsideBehavior->getPoint();
		ARCVector3 destNextPt = pPreLandsideBehavior->getDest();
		if (pPreQueueProcess->GetSegment() == QUEUE_OVERFLOW)
		{
			double dDist = locationPt.DistanceTo(destNextPt);
			if (dDist >= dInStep*1.05)
			{
				return true;
			}
		}
		else
		{
			if (pPreQueueProcess->GetSegment() == cornerCount() - 1)
			{
				double dDist = destNextPt.DistanceTo(destPt);
				//if(destNextPt.DistanceTo(corner(cornerCount() - 1)) > dInStep)
				if (dDist > dInStep)
					return true;
				return false;
			}
			return  true;
		}

	
		return false;
	}

	int idx = m_waitList.findElement(pLandsideBehavior->GetPersonID());
	if (idx == INT_MAX)
	{
		idx = m_waitList.getCount();
	}

	PaxLandsideBehavior *pPreLandsideBehavior = PeekWait(idx - 1);
	ASSERT(pPreLandsideBehavior);
	LandsidePaxQueueProcess* pPreQueueProcess = pLandsideQueueSys->FindPaxQueueProcess(pPreLandsideBehavior);
	ASSERT(pPreQueueProcess);
	ARCVector3 locationNextPt = pPreLandsideBehavior->getPoint();
	ARCVector3 destNextPt = pPreLandsideBehavior->getDest();

	if (!pPreQueueProcess->GetStuck())
	{
		return true;
	}
	ARCVector3 vector;
	if (nSegment == pPreQueueProcess->GetSegment())
	{
		double dDist = locationPt.DistanceTo(destNextPt);
		if (dDist >= 1.05*dInStep)
		{
			return true;
		}
	}
	else if (nSegment - 1== pPreQueueProcess->GetSegment())
	{
		double dNextDist = destNextPt.DistanceTo(corner(pPreQueueProcess->GetSegment()));
		double dCurtDist = destPt.DistanceTo(corner(pPreQueueProcess->GetSegment()));
		if (dNextDist>= 1.05*dInStep)
		{
			return true;
		}
		else if (dCurtDist >= 1.05*dInStep)
		{
			return true;	
		}
	}
	else
	{
		double dDist = locationPt.DistanceTo(destNextPt);
		if (dDist >= 1.05*dInStep)
		{
			return true;
		}
	}
	
	return false;

}

int LandsideQueue::CalculateSegment( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys )
{
	PaxLandsideBehavior* pLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	ASSERT(pLandsideBehavior);
	ARCVector3 locationPt;
	ARCVector3 destPt;
	locationPt = pLandsideBehavior->getPoint();
	destPt = pLandsideBehavior->getDest();

	//retrieve pre person
	int idx = m_waitList.findElement(pLandsideBehavior->GetPersonID());
	if (idx == INT_MAX)
	{
		idx = m_waitList.getCount();
	}

	PaxLandsideBehavior *pPreLandsideBehavior = PeekWait(idx - 1);
	ASSERT(pPreLandsideBehavior);
	LandsidePaxQueueProcess* pPreQueueProcess = pLandsideQueueSys->FindPaxQueueProcess(pPreLandsideBehavior);
	ASSERT(pPreQueueProcess);
	double dSpeed = pPreLandsideBehavior->getPerson()->getSpeed();

	ARCVector3 locationNextPt = pPreLandsideBehavior->getPoint();
	ARCVector3 destNextPt = pPreLandsideBehavior->getDest();

	Person* pPerson = pLandsideBehavior->getPerson();
	ASSERT(pPerson);
	double dInStep = pPerson->getInStep();

	if (destNextPt.DistanceTo(corner(pPreQueueProcess->GetSegment()))> dInStep)
		return pQueueProcess->GetSegment() - 1;
	return pQueueProcess->GetSegment();
}

ARCVector3 LandsideQueue::GetNextStep(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys)
{
	PaxLandsideBehavior* pLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	ASSERT(pLandsideBehavior);
	ARCVector3 locationPt;
	ARCVector3 destPt;
	locationPt = pLandsideBehavior->getPoint();
	destPt = pLandsideBehavior->getDest();

	if (isHead(pLandsideBehavior))
		return corner (0);

	int nSegment = pQueueProcess->GetSegment();
	Person* pPerson = pLandsideBehavior->getPerson();
	ASSERT(pPerson);
	double dInStep = pPerson->getInStep();
	//retrieve pre person
	if (nSegment == QUEUE_OVERFLOW)
	{
 		PaxLandsideBehavior *pPreLandsideBehavior = NULL;
 		int idx = m_approachList.findElement(pLandsideBehavior->GetPersonID());
 		if (idx == 0)
 		{
 			pPreLandsideBehavior = GetTailBehavior();
 		}
 		else
 		{
 			pPreLandsideBehavior = m_approachList.getItem(idx - 1);
 		}
 
		if (pPreLandsideBehavior == NULL)
		{
			RemoveApproach(pLandsideBehavior);
			pQueueProcess->SetSegment(cornerCount() -1);
			return corner(cornerCount() -1);
		}

 		LandsidePaxQueueProcess* pPreQueueProcess = pLandsideQueueSys->FindPaxQueueProcess(pPreLandsideBehavior);
 		ASSERT(pPreQueueProcess);
 
 		int preSegment = pPreQueueProcess->GetSegment();
 		if (pPreQueueProcess->GetSegment() == QUEUE_OVERFLOW)
 		{
 			ARCVector3 locationNextPt = pPreLandsideBehavior->getPoint();
 			ARCVector3 destNextPt = pPreLandsideBehavior->getDest();
 
 		//	ARCVector3 vector = locationPt - destNextPt;
			ARCVector3 vector = corner(cornerCount() - 1) - corner(cornerCount() - 2);
 			vector.SetLength(dInStep);
 			return destNextPt + vector;
 		}
 		else if(pPreQueueProcess->GetSegment() != cornerCount() - 1)
 		{
			RemoveApproach(pLandsideBehavior);
 			pQueueProcess->SetSegment(cornerCount() -1);
 			return corner(cornerCount() -1);
 		}
 		else
 		{
			/*RemoveApproach(pLandsideBehavior);
			pQueueProcess->SetSegment(cornerCount() -1);*/
 			ARCVector3 destNextPt = pPreLandsideBehavior->getDest();
			double dToSegment = destNextPt.DistanceTo(corner(cornerCount() - 1));
			if (dToSegment >= dInStep)
			{
				RemoveApproach(pLandsideBehavior);
				pQueueProcess->SetSegment(cornerCount() -1);
				return corner(cornerCount() -1);
			}
			else
			{
				ARCVector3 vector = corner(cornerCount() - 1) - corner(cornerCount() - 2);
				double dDist = destNextPt.DistanceTo(destPt);
				if (dDist > dInStep)
				{
					ARCVector3 newDest;
					vector.SetLength(dInStep);
					newDest = destNextPt + vector;

					if (destPt.DistanceTo(corner(cornerCount() - 1)) < newDest.DistanceTo(destPt))
					{
						RemoveApproach(pLandsideBehavior);
						pQueueProcess->SetSegment(cornerCount() -1);
					}
					return newDest;
				}
			}
 			
 			return destPt;
 		}
	}
	
	int idx = m_waitList.findElement(pLandsideBehavior->GetPersonID());
	if (idx == INT_MAX)
	{
		idx = m_waitList.getCount();
	}
	PaxLandsideBehavior *pPreLandsideBehavior = PeekWait(idx - 1);
	ASSERT(pPreLandsideBehavior);
	LandsidePaxQueueProcess* pPreQueueProcess = pLandsideQueueSys->FindPaxQueueProcess(pPreLandsideBehavior);
	ASSERT(pPreQueueProcess);
	double dSpeed = pPreLandsideBehavior->getPerson()->getSpeed();
	
	ARCVector3 locationNextPt = pPreLandsideBehavior->getPoint();
	ARCVector3 destNextPt = pPreLandsideBehavior->getDest();

	ARCVector3 vector;
	if (nSegment == pPreQueueProcess->GetSegment())
		vector = corner (pPreQueueProcess->GetSegment()) - destNextPt;
	else if (nSegment-1 == pPreQueueProcess->GetSegment())
	{
		int nCurSegment = CalculateSegment(pQueueProcess,pLandsideQueueSys);
		if (nCurSegment != nSegment)
		{
			pQueueProcess->SetSegment(nCurSegment);
			return corner(nCurSegment);
		}
		else
		{
			double dDist = destNextPt.DistanceTo(corner(pPreQueueProcess->GetSegment())) + destPt.DistanceTo(corner(pPreQueueProcess->GetSegment()));
			if (dDist > dInStep)
			{
				dDist -= dInStep;
				vector = corner(nSegment - 1) - corner(nSegment);
				vector.SetLength(dDist);
				return destPt+vector;
			}
			return destPt;
		}
	}
	else
	{
		pQueueProcess->SetSegment(nSegment - 1);
		return corner(nSegment - 1);
	}

	vector.SetLength(dInStep);
	return destNextPt + vector;
}

bool LandsideQueue::isHead( PaxLandsideBehavior* pLandsideBehavior )
{
	if (m_waitList.IsEmpty())
		return false;
	
	return m_waitList.front() == pLandsideBehavior ? true : false;
}

PaxLandsideBehavior* LandsideQueue::GetLandsideBehavior( int idx ) const
{
	return m_waitList.getItem(idx);
}

PaxLandsideBehavior* LandsideQueue::GetHeadBehavior() const
{
	if(!m_waitList.IsEmpty())
		return m_waitList.front();
	return NULL;
}

PaxLandsideBehavior* LandsideQueue::GetTailBehavior() const
{
	if(!m_waitList.IsEmpty())
		return m_waitList.back();
	return NULL;
}

void LandsideQueue::RemoveApproach( PaxLandsideBehavior* pLandsideBehavior )
{
	int index = m_approachList.findElement (pLandsideBehavior->GetPersonID());
	if (index != INT_MAX)
	{
		m_approachList.removeItem (index);
	}
	int iWaitIdx = m_waitList.findElement(pLandsideBehavior->GetPersonID());
	if (iWaitIdx == INT_MAX)
	{
		m_waitList.addItem(pLandsideBehavior);
	}
}

void LandsideQueue::RemoveWait( PaxLandsideBehavior* pLandsideBehavior )
{
	int index = m_waitList.findElement (pLandsideBehavior->GetPersonID());
	if (index != INT_MAX)
	{
		m_waitList.removeItem (index);
	}
}

void LandsideQueue::StartQueueMove( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	PaxLandsideBehavior* pLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	if (pLandsideBehavior)
	{
		if (StepItValid(pQueueProcess,pLandsideQueueSys))
		{
			ARCVector3 ptDest;
			ptDest = GetNextStep(pQueueProcess,pLandsideQueueSys);
			pLandsideBehavior->setDestination(ptDest);
			pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
			ElapsedTime eTime = time + pLandsideBehavior->moveTime() + ElapsedTime(0.1);
			pQueueProcess->GenerateQueueEvent(eTime);

			if (isTail (pLandsideBehavior))
			{
				UpdateApproach(eTime,pLandsideQueueSys);
			}
			else
				// schedule event to have next person in queue advance
			{
				int idx = m_waitList.findElement(pLandsideBehavior->GetPersonID());
				if (idx == INT_MAX)
					return;
				
				PaxLandsideBehavior *pNextBehavior = m_waitList.getItem(idx+1);

				if (!pNextBehavior)
					return;
				
				LandsidePaxQueueProcess* pNextQueueProc = pLandsideQueueSys->FindPaxQueueProcess(pNextBehavior);
				if (pNextQueueProc == NULL)
					return;
				
				if (pNextQueueProc->GetStuck())
				{
					pNextQueueProc->SetStuck(false);
					pNextQueueProc->SetQueueState(LandsidePaxQueueProcess::sp_startMove);
					pNextQueueProc->GenerateQueueEvent(time + ElapsedTime(0.1));
				}
			}
		}
		else
		{
			pQueueProcess->SetStuck(true);
		}
	
	}
}
void LandsideQueue::AddToQueue( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	PaxLandsideBehavior* pLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	ASSERT(pLandsideBehavior);
	//AddApproach(pLandsideBehavior);
	LandsidePaxQueueProcess* pNextQueueProc = pLandsideQueueSys->GetNextBehavior(pQueueProcess->GetlandsideBehavior());
	if (pNextQueueProc)
	{
		Person* pPerson = pQueueProcess->GetlandsideBehavior()->getPerson();
		Person* pPrePerson = pNextQueueProc->GetlandsideBehavior()->getPerson();
		pPerson->setSpeed(MIN(pPerson->getSpeed(),pPrePerson->getSpeed()));
	}

	if (pLandsideBehavior)
	{
		ARCVector3 ptDest;
		ptDest = GetNextStep(pQueueProcess,pLandsideQueueSys);
		pLandsideBehavior->setDestination(ptDest);
		ElapsedTime eTime = time + pLandsideBehavior->moveTime();
		pLandsideBehavior->WriteLogEntry(eTime);
		//for adjust direction
		int nCount = cornerCount();
		ARCVector3 vector = corner(nCount - 2) - corner(nCount - 1);
		vector.SetLength(1.0);
		ARCVector3 newPtDest = ptDest + vector;
		pLandsideBehavior->setDestination(newPtDest);

		pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
		pQueueProcess->GenerateQueueEvent(eTime + pLandsideBehavior->moveTime());
	}
}

void LandsideQueue::WaitInQueue( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	PaxLandsideBehavior* pLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	// if next Person has already advanced, continue moving
	if (pLandsideBehavior)
	{
		if (!isHead(pLandsideBehavior))
		{
			/*int nSegment = CalculateSegment(pQueueProcess,pLandsideQueueSys);
			pQueueProcess->SetSegment(nSegment);*/
			StartQueueMove(pQueueProcess,pLandsideQueueSys,time);
		}
		else if (pQueueProcess->GetSegment() == 1)
		{
			pLandsideBehavior->setDestination(corner(0));
			pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_advanceQueue);
			pQueueProcess->GenerateQueueEvent(time+pLandsideBehavior->moveTime());
		}
		else
		{
		//	pLandsideBehavior->setDestination(corner(0));
			pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_movetoQueue);
			pQueueProcess->GenerateQueueEvent(time/*+pLandsideBehavior->moveTime()*/);
		}
	}
	
}

void LandsideQueue::MoveToQueue( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
 	ARCVector3 pt; 
	if (pQueueProcess->GetSegment() == QUEUE_OVERFLOW)
	{
		RemoveApproach(pQueueProcess->GetlandsideBehavior());
		// update all approaching Persons
		UpdateApproach(time,pLandsideQueueSys);
		int nCorner = cornerCount();
		pt = corner(nCorner - 1);
		pQueueProcess->SetSegment(nCorner - 1);
		LandsidePaxQueueProcess* pNextQueueProc = pLandsideQueueSys->GetNextBehavior(pQueueProcess->GetlandsideBehavior());
		if (pNextQueueProc)
		{
			Person* pPerson = pQueueProcess->GetlandsideBehavior()->getPerson();
			Person* pPrePerson = pNextQueueProc->GetlandsideBehavior()->getPerson();
			pPerson->setSpeed(MIN(pPerson->getSpeed(),pPrePerson->getSpeed()));
		}
		pQueueProcess->SetDes(false);
	}
 	else if (pLandsideQueueSys->OnTailSegment(pQueueProcess))
 	{
 		pt = GetNextStep(pQueueProcess,pLandsideQueueSys);
 		pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
		pQueueProcess->SetDes(false);
 	}
 	else
 	{
		int nSegment = pQueueProcess->GetSegment() - 1;
		pt = corner(nSegment);
		pQueueProcess->SetDes(true);
	//	pQueueProcess->SetSegment(nSegment);
 	}
 
 	PaxLandsideBehavior* pLandsideBehavior = pQueueProcess->GetlandsideBehavior();
 	if (pLandsideBehavior)
 	{
 		pLandsideBehavior->setDestination(pt);
 		pQueueProcess->GenerateQueueEvent(time+pLandsideBehavior->moveTime());
 	}
}

void LandsideQueue::AdvanceQueue( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	PaxLandsideBehavior* pLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	int idx = m_waitList.findElement(pLandsideBehavior->GetPersonID());
	if (pLandsideBehavior&&!isHead(pLandsideBehavior))
	 	StartQueueMove(pQueueProcess,pLandsideQueueSys,time);
	if (isHead(pLandsideBehavior))
	{
		pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_leaveQueue);
		pQueueProcess->GenerateQueueEvent(time/* + ElapsedTime(0.6)*/);
		
	}   
}

bool LandsideQueue::isTail( PaxLandsideBehavior* pLandsideBehavior )
{
	if (m_waitList.IsEmpty())
		return true;

	return m_waitList.back() == pLandsideBehavior ? true : false;
}

void LandsideQueue::UpdateApproach( const ElapsedTime& time,LandsideQueueSystemProcess* pLandsideQueueSys )
{
	//if (m_approachList.IsEmpty())
	//	return;
	//
	//PaxLandsideBehavior* pLandsideBehavior = m_approachList.getItem(0);
	//if (pLandsideBehavior == NULL)
	//	return;

	//LandsidePaxQueueProcess* pQueueProc = pLandsideQueueSys->FindPaxQueueProcess(pLandsideBehavior);
	//if (pQueueProc == NULL)
	//	return;

	//if (pQueueProc->GetStuck())
	//{
	//	//if (pLandsideQueueSys->OnTailSegment(pQueueProc))
	//	//{
	//		pQueueProc->SetStuck(false);
	//		pQueueProc->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
	//		pQueueProc->GenerateQueueEvent(time /*+ ElapsedTime(0.1)*/);
	//	//}
	//	//else if (pQueueProc->GetQueueState() == LandsidePaxQueueProcess::sp_waitinQueue)
	//	//{
	//	//	pQueueProc->SetStuck(false);
	//	//	pQueueProc->SetQueueState(LandsidePaxQueueProcess::sp_advanceQueue);
	//	//	MoveToQueue (pQueueProc,pLandsideQueueSys, time);
	//	//}
	//}

	ElapsedTime eTime = time;
 	for (int i = 0; i < m_approachList.getCount(); i++)
 	{
 		PaxLandsideBehavior* pLandsideBehavior = m_approachList.getItem(i);
 		if (pLandsideBehavior == NULL)
 			continue;
 		
 		LandsidePaxQueueProcess* pQueueProc = pLandsideQueueSys->FindPaxQueueProcess(pLandsideBehavior);
 		if (pQueueProc == NULL)
 			continue;
 		
 		if (pQueueProc->GetStuck())
 		{
 		//	if (pLandsideQueueSys->OnTailSegment(pQueueProc))
 			{
				eTime = eTime + ElapsedTime(0.1);
				pQueueProc->SetStuck(false);
 				pQueueProc->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
 				pQueueProc->GenerateQueueEvent(eTime);
 			}
//  			else if (pQueueProc->GetQueueState() == LandsidePaxQueueProcess::sp_waitinQueue)
//  			{
//  				pQueueProc->SetQueueState(LandsidePaxQueueProcess::sp_advanceQueue);
//  				MoveToQueue (pQueueProc,pLandsideQueueSys, time);
//  			}
 		}
 	}
}

void LandsideQueue::AddApproach( PaxLandsideBehavior* pLandsideBehavior )
{
	ASSERT(pLandsideBehavior);
	m_approachList.addItem(pLandsideBehavior);
}
int LandsideQueue::FindWait(PaxLandsideBehavior* pLandsideBehavior)
{
	return m_waitList.findElement(pLandsideBehavior->GetPersonID());
}

int LandsideQueue::FindApproach(PaxLandsideBehavior* pLandsideBehavior)
{
	return m_approachList.findElement(pLandsideBehavior->GetPersonID());
}

PaxLandsideBehavior* LandsideQueue::PeekWait( int idx )
{
	if(idx >= 0 && idx < m_waitList.getCount())
	{
		return m_waitList.getItem(idx);
	}

	return NULL;
}

PaxLandsideBehavior* LandsideQueue::PeekApproach(int idx)
{
	if(idx >= 0 && idx < m_approachList.getCount())
	{
		return m_approachList.getItem(idx);
	}

	return NULL;
}

LandsideQueueEvent::LandsideQueueEvent( LandsidePaxQueueProcess *pQueueProc, ElapsedTime eTime )
:m_pQueueProcess(pQueueProc)
{
	setTime(eTime);
}

LandsideQueueEvent::~LandsideQueueEvent( void )
{

}

int LandsideQueueEvent::process( CARCportEngine* pEngine )
{
	if (m_pQueueProcess)
	{
		m_pQueueProcess->ProcessMove(time);
	}

	return 1;
}

int LandsideQueueEvent::kill( void )
{
	return 1;
}

const char * LandsideQueueEvent::getTypeName( void ) const
{
	return _T("QueueProcessEvent");
}

int LandsideQueueEvent::getEventType( void ) const
{
	return 1;
}

void LandsideQueueEvent::SetPerson( PaxLandsideBehavior* pPerson )
{
	m_pPerson = pPerson;
}

void LandsideQueueEvent::SetState( LandsidePaxQueueProcess::QueueState emState )
{
	m_emState = emState;
}


LandsidePaxQueueProcess::LandsidePaxQueueProcess(PaxLandsideBehavior* pLandsideBehavior)
:m_queueState(sp_movetoQueue)
,m_nSegment(QUEUE_OVERFLOW)
,m_bStuck(false)
,m_pLandsideBehavior(pLandsideBehavior)
,m_bDes(false)
{

}

LandsidePaxQueueProcess::~LandsidePaxQueueProcess()
{

}

void LandsidePaxQueueProcess::ProcessMove(ElapsedTime p_time )
{
	LandsideQueue* pQueue = m_pLandsideQueueSys->GetlandsideQueue();
	ASSERT(pQueue);

	switch (m_queueState)
	{
	case sp_movetoQueue:
		{
			if (m_bDes)
			{
				m_nSegment--;
			}
			m_pLandsideBehavior->WriteLogEntry(p_time);
			pQueue->MoveToQueue(this,m_pLandsideQueueSys,p_time);
		}
		break;
	case sp_addtoQueue:
		{
			m_pLandsideBehavior->WriteLogEntry(p_time);
			pQueue->AddToQueue(this,m_pLandsideQueueSys,p_time);
		}
		break;
	case sp_waitinQueue:
		{
			m_pLandsideBehavior->WriteLogEntry(p_time);
			pQueue->WaitInQueue(this,m_pLandsideQueueSys,p_time);
		}
		break;
	case sp_advanceQueue:
		{
			m_pLandsideBehavior->WriteLogEntry(p_time);
			pQueue->AdvanceQueue(this,m_pLandsideQueueSys,p_time); 
		}
		break;
	case sp_leaveQueue:
		{
			m_pLandsideBehavior->WriteLogEntry(p_time);
			SetStuck(true);
			m_pLandsideQueueSys->LeaveQueueProcess(m_pLandsideBehavior,m_endState,p_time);
		}
		break;
	case sp_startMove:
		{
			m_pLandsideBehavior->WriteLogEntry(p_time);
			pQueue->StartQueueMove(this,m_pLandsideQueueSys,p_time);
		}
		break;
	default:
		break;
	}
	
	//schedule next move event
}

void LandsidePaxQueueProcess::Initialize(int state,LandsideQueueSystemProcess* pQueueProcess)
{
	m_endState = state;
	m_pLandsideQueueSys = pQueueProcess;
}


PaxLandsideBehavior* LandsidePaxQueueProcess::GetlandsideBehavior()
{
	return m_pLandsideBehavior;
}

int LandsidePaxQueueProcess::GetSegment() const
{
	return m_nSegment;
}

void LandsidePaxQueueProcess::StartMove( ElapsedTime p_time )
{
	LandsideQueue* pQueue = m_pLandsideQueueSys->GetlandsideQueue();
	m_pLandsideBehavior->setDestination(m_pLandsideBehavior->getPoint());
	pQueue->AddApproach(m_pLandsideBehavior);
	if (m_pLandsideQueueSys->OnTailSegment(this))
	{
		m_queueState = sp_addtoQueue;
	}
	else
	{
		m_pLandsideBehavior->WriteLogEntry(p_time);
		m_queueState = sp_movetoQueue;
		m_pLandsideBehavior->setDestination(m_pLandsideQueueSys->GetTailPoint());
		p_time += m_pLandsideBehavior->moveTime();
	}
	
	GenerateQueueEvent(p_time);
}

void LandsidePaxQueueProcess::GenerateQueueEvent( const ElapsedTime& time )
{
	LandsideQueueEvent* pQueueEvnet = new LandsideQueueEvent(this,time);
	pQueueEvnet->SetPerson(m_pLandsideBehavior);
	pQueueEvnet->SetState(m_queueState);
	pQueueEvnet->addEvent();
}

LandsideQueueSystemProcess::LandsideQueueSystemProcess(LandsidePaxSericeInSim* pPaxServiceInsim)
{
	m_pQueue = new LandsideQueue(pPaxServiceInsim->GetQueue());
	m_pPaxServiceInSim = pPaxServiceInsim;
}

LandsideQueueSystemProcess::~LandsideQueueSystemProcess( void )
{
	Clear();
}

void LandsideQueueSystemProcess::EnterQueueProcess( PaxLandsideBehavior* pLandsideBehavior,int state,const ElapsedTime& p_time )
{
	LandsidePaxQueueProcess* pPaxQueue = new LandsidePaxQueueProcess(pLandsideBehavior);
	pPaxQueue->Initialize(state,this);
	m_vPaxQueueProc.push_back(pPaxQueue);
	pPaxQueue->StartMove(p_time);
}

void LandsideQueueSystemProcess::LeaveQueueProcess( PaxLandsideBehavior* pLandsideBehavior,int state,const ElapsedTime& p_time )
{
 	pLandsideBehavior->setState(state);
 	pLandsideBehavior->GenerateEvent(p_time);
}

LandsidePaxQueueProcess* LandsideQueueSystemProcess::FindPaxQueueProcess( PaxLandsideBehavior* pLandsideBehavior )
{
	for (size_t i = 0; i < m_vPaxQueueProc.size(); i++)
	{
		LandsidePaxQueueProcess* pQueueProc = m_vPaxQueueProc.at(i);
		if (pLandsideBehavior == pQueueProc->GetlandsideBehavior())
		{
			return pQueueProc;
		}
	}
	return NULL;
}

LandsidePaxQueueProcess* LandsideQueueSystemProcess::GetNextBehavior( PaxLandsideBehavior* pLandsideBehavior )
{
	int idx = m_pQueue->FindWait(pLandsideBehavior);
	if (idx == INT_MAX)
	{
		//check approach head
		int appIdx = m_pQueue->FindApproach(pLandsideBehavior);
		if (appIdx == 0)
		{
			PaxLandsideBehavior* pNextBehavior = m_pQueue->GetTailBehavior();
			return FindPaxQueueProcess(pNextBehavior);
		}
		
		PaxLandsideBehavior* pNextBehavior = m_pQueue->PeekApproach(appIdx - 1);
		return FindPaxQueueProcess(pNextBehavior);
	}
	if (idx == 0)
		return NULL;
	
	PaxLandsideBehavior* pNextBehavior = m_pQueue->PeekWait(idx - 1);
	return FindPaxQueueProcess(pNextBehavior);
}

LandsidePaxQueueProcess* LandsideQueueSystemProcess::GetTailBehavior()
{
	PaxLandsideBehavior* pTailBehavior = m_pQueue->GetTailBehavior();
	if (pTailBehavior == NULL)
		return NULL;
	
	return FindPaxQueueProcess(pTailBehavior);
}


bool LandsideQueueSystemProcess::LastSegment( int nSegment ) const
{
	if (nSegment == 1)
		return true;
	
	return false;
}

bool LandsideQueueSystemProcess::OnTailSegment( LandsidePaxQueueProcess* pPaxQueueProc )
{
	LandsidePaxQueueProcess* pNextQueueProc = GetNextBehavior(pPaxQueueProc->GetlandsideBehavior());

	// if there are no Persons waiting, its based purely on segment
	if (pNextQueueProc == NULL)
	{
		bool bTail = (pPaxQueueProc->GetSegment() == 1) || (m_pQueue->cornerCount() == 1 && pPaxQueueProc->GetSegment() == QUEUE_OVERFLOW)? true : false;
		return bTail;
	}
	
	if (pPaxQueueProc->GetSegment() == QUEUE_OVERFLOW && pNextQueueProc->GetSegment() == m_pQueue->cornerCount() - 1)
		return true;
	
	if (pNextQueueProc->GetSegment() >= pPaxQueueProc->GetSegment())
		return true;
	
	PaxLandsideBehavior* pNextBehavior = pNextQueueProc->GetlandsideBehavior();
	ASSERT(pNextBehavior);

	PaxLandsideBehavior* pPaxBehavior = pPaxQueueProc->GetlandsideBehavior();
	ASSERT(pPaxBehavior);
	Person* pPerson = pPaxBehavior->getPerson();
	ASSERT(pPerson);
	DistanceUnit inStep = pPerson->getInStep();
	
	if (pNextQueueProc->GetSegment() + 1 == pPaxQueueProc->GetSegment())
	{
// 		if (pNextQueueProc->GetSegment() == 1)
// 			return false;
		ARCVector3 locationPt = pNextBehavior->getDest();
		if (locationPt.DistanceTo(m_pQueue->corner(pNextQueueProc->GetSegment())) <= 1.0 * inStep)
		{
			return true;
		}
	}
	
	return false;
}

ARCVector3 LandsideQueueSystemProcess::GetTailPoint() const
{
	int nCount = m_pQueue->cornerCount();
	if(nCount > 0)
		return m_pQueue->corner(nCount - 1);

	return ARCVector3();
}


LandsideQueue* LandsideQueueSystemProcess::GetlandsideQueue() const
{
	return m_pQueue;
}

void LandsideQueueSystemProcess::Release( PaxLandsideBehavior* pLandsideBehavior )
{
	LandsidePaxQueueProcess* pQueueProc = FindPaxQueueProcess(pLandsideBehavior);
	if (pQueueProc)
	{
		std::vector<LandsidePaxQueueProcess*>::iterator iter = std::find(m_vPaxQueueProc.begin(),m_vPaxQueueProc.end(),pQueueProc);
		if (iter != m_vPaxQueueProc.end())
		{
			m_vPaxQueueProc.erase(iter);
			delete pQueueProc;
		}
	}
}

void LandsideQueueSystemProcess::Clear()
{
	if (m_pQueue)
	{
		delete m_pQueue;
		m_pQueue = NULL;
	}

	for (UINT i = 0; i < m_vPaxQueueProc.size(); i++)
	{
		delete m_vPaxQueueProc[i];
	}
	m_vPaxQueueProc.clear();
}


void LandsideQueueSystemProcess::InvokeWaitQueue( const ElapsedTime& time )
{
	PaxLandsideBehavior* pLandsideBehavior = m_pQueue->GetHeadBehavior();
	if (pLandsideBehavior)
	{
		LandsidePaxQueueProcess* pPaxQueueProc = FindPaxQueueProcess(pLandsideBehavior);
		if (pPaxQueueProc)
		{
			if(pPaxQueueProc->GetStuck())
			{
				pPaxQueueProc->SetStuck(false);
				pPaxQueueProc->SetQueueState(LandsidePaxQueueProcess::sp_startMove);
				pPaxQueueProc->GenerateQueueEvent(time /*+ ElapsedTime(0.6)*/);
			}
		}
	}
	
}

void LandsideQueueSystemProcess::LeaveQueue( const ElapsedTime& p_time )
{
	PaxLandsideBehavior* pLandsideBehavior = m_pQueue->GetHeadBehavior();
	if (pLandsideBehavior)
	{
		LandsidePaxQueueProcess* pPaxQueueProc = FindPaxQueueProcess(pLandsideBehavior);
		if (pPaxQueueProc)
		{
			pPaxQueueProc->SetStuck(false);
			m_pQueue->RemoveWait(pLandsideBehavior);
			Release(pLandsideBehavior);
			InvokeWaitQueue(p_time);
		}
	}
}



LandsideQueueWakeUpEvent::LandsideQueueWakeUpEvent( LandsideQueueSystemProcess* pQueueProcessSys,ElapsedTime eTime )
:m_queueProcessSys(pQueueProcessSys)
{
	setTime(eTime);
}

LandsideQueueWakeUpEvent::~LandsideQueueWakeUpEvent()
{

}

int LandsideQueueWakeUpEvent::process( CARCportEngine* pEngine )
{
	if (m_queueProcessSys)
	{
		m_queueProcessSys->InvokeWaitQueue(time);
	}
	return 1;
}

int LandsideQueueWakeUpEvent::kill( void )
{
	return 1;
}

const char * LandsideQueueWakeUpEvent::getTypeName( void ) const
{
	return _T("QueueWakeupEvent");
}

int LandsideQueueWakeUpEvent::getEventType( void ) const
{
	return 2;
}
