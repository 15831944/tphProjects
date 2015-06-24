#include "StdAfx.h"
#include "pax.h"
#include "Inputs\TrayHost.h"
#include "PaxTerminalBehavior.h"
#include "ARCportEngine.h"
#include "GroupLeaderInfo.h"
#include "terminal.h"
#include "VisitorTerminalBehavior.h"
#include "inputs\AllGreetingProcessors.h"
#include "SimFlowSync.h"

PaxTerminalBehavior::PaxTerminalBehavior(Passenger* pPerson)
:TerminalMobElementBehavior(pPerson)
,hasBusServer(TRUE) 
,m_lWantToAttatchedVisID(-1)
,m_pSimFlowSync(NULL)
{

}

PaxTerminalBehavior::~PaxTerminalBehavior()
{
	if (m_pSimFlowSync)
	{
		delete m_pSimFlowSync;
		m_pSimFlowSync = NULL;
	}
}

//m_pPerson function processes different m_type pax's service when they arrive at HoldArea.
//If is arrival, set greeter destination; if departure, set flight and last boarding call.
//It will be overloaded to process greeting events.
void PaxTerminalBehavior::processServerArrival (ElapsedTime p_time)
{
	//// TRACE("\n%s\n",m_pProcessor->getID()->GetIDString() );

	//#ifdef _DEBUG
	//if( m_bGerentGreetEventOrWait )
	//{
	//	ASSERT( m_pProcessor->getProcessorType() == HoldAreaProc  );
	//}
	//#endif





	if (m_pProcessor->getProcessorType() == HoldAreaProc)
	{
		if (m_nStranded && m_pPerson->getType().isDeparture())
		{
			// check to see if missed last boarding call
			int flightIndex = m_pPerson->getLogEntry().getDepFlight();
			assert( m_pTerm );
			Flight *aFlight = getEngine()->m_simFlightSchedule.getItem (flightIndex);
			if (aFlight->getLastCall() < p_time)
				m_nStranded = FALSE;
		}

		//matt
		//@@@ find the covergerpair,send 
		//if(m_bGerentGreetEventOrWait)
		//	{

		for(std::vector<ConvergePair*>::iterator it=GetPax()->m_vpConvergePair.begin();it!=GetPax()->m_vpConvergePair.end();it++)
		{
			//	if((*it)->AmNowConvergeProcessor(id,m_pProcessor,m_pPerson,false,p_time))//true means want to convert,waiting
			if((*it)->getWaitTag(m_pPerson->getID()))
			{
				(*it)->ReportArrival(m_pPerson->getID(), p_time);
				m_bGerentGreetEventOrWait = false;
				return;
			}
		}

		//	}

		//@@@

		/*
		if( m_bGerentGreetEventOrWait )
		{
		//PaxVisitor *greeter = (PaxVisitor *)m_pWantToGreetPerson;
		//ASSERT( m_lWantToAttatchedVisID >=0 );
		PaxVisitor* greeter = (PaxVisitor*)m_pProcessor->findElement( m_lWantToAttatchedVisID );
		if( greeter && greeter->getState() == WaitInHoldingArea)
		{
		ASSERT( greeter->getType().GetTypeIndex() > 0 );
		destination = greeter->getPoint();
		GreetEvent *event = new GreetEvent (m_pPerson, greeter, p_time + moveTime());
		event->addEvent();

		m_bGerentGreetEventOrWait = false;
		m_pWantToGreetPerson = NULL;
		return;
		}
		else
		{
		m_iPreState = state;
		state = WaitInHoldingArea;
		return;
		}
		}*/

	}
	else if( m_pProcessor->getProcessorType() == GateProc )
	{
		++m_iNumberOfPassedGateProc;
	}

	m_pPerson->setState(LeaveServer);
	m_pProcessor->beginService (m_pPerson, p_time);
}


void PaxTerminalBehavior::greet( PaxVisitor* _pVisitor, ElapsedTime _time )
{
	int VisitorId = -1;
	int iBeforVisiType =-1;
	int nCount = GetPax()->m_pVisitorList.size();
	VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior*)_pVisitor->getBehavior(MobElementBehavior::TerminalBehavior);

	if (spTerminalBehavior&&spTerminalBehavior->IfInTrayHost())
	{
		PaxVisitor* pFollower = ((CGroupLeaderInfo*)_pVisitor->m_pGroupInfo)->GetFollowerInTrayHost();
		while (pFollower)
		{
			((CGroupLeaderInfo*)_pVisitor->m_pGroupInfo)->RemoveFollower(pFollower);
			VisitorTerminalBehavior* spFollowerBehavior = (VisitorTerminalBehavior*)_pVisitor->getBehavior(MobElementBehavior::TerminalBehavior);
			for( int i=0; i<nCount; i++ )
			{
				MobLogEntry vis;
				m_pTerm->paxLog->getItem( vis,GetPax()-> m_vVisIndex[i] );
				VisitorId = vis.getID();
				if( vis.getID() == pFollower->getID() )
				{
					pFollower->m_pGroupInfo->SetFollowerInTray(false);
					pFollower->m_pGroupInfo->SetFollowerWithOwner(true);
					GetPax()->m_pVisitorList[i] = pFollower;
					spFollowerBehavior->SetGerentGreetEventOrWaitFlag( false );
					GetPax()->m_pVisitorList[i]->initOwnerPassenger( (Passenger*)m_pPerson );
					spFollowerBehavior->SetGreetingTag(true);
					break;
				}
			}
			pFollower = ((CGroupLeaderInfo*)_pVisitor->m_pGroupInfo)->GetFollowerInTrayHost();
		}
	}
	//	ASSERT( nCount == m_vVisIndex.size() );
	for( int i=0; i<nCount; i++ )
	{
		MobLogEntry vis;
		m_pTerm->paxLog->getItem( vis, GetPax()->m_vVisIndex[i] );
		VisitorId = vis.getID();

		//	ProcessorDistribution *pFlowList = m_pProcessor->getNextDestinations(_pVisitor->getType(),m_nInGateDetailMode);
		if( vis.getID() == _pVisitor->getID() )
		{
			_pVisitor->m_pGroupInfo->SetFollowerWithOwner(true);
			GetPax()->m_pVisitorList[i] = _pVisitor;
			spTerminalBehavior->SetGerentGreetEventOrWaitFlag( false );
			GetPax()->m_pVisitorList[i]->initOwnerPassenger( (Passenger*)m_pPerson );
			spTerminalBehavior->getProcessor()->removePerson(_pVisitor);

			//
			spTerminalBehavior->SetGreetingTag(true);
			if(vis.GetMobileType() != iBeforVisiType)
				GetPax()->DeleteConvergePair(VisitorId,true);
			iBeforVisiType = vis.GetMobileType();

			break;
		}
	}
	//// TRACE(m_pProcessor->getID()->GetIDString());
	m_pProcessor->removePerson( _pVisitor );
	m_nStranded = FALSE;
	//state = LeaveServer;
	m_bGerentGreetEventOrWait = false; 

	// caution: only greet the first one visitor , because only the first one visitor can gernerate event
	// the other visitor just copy his logs

	m_pPerson->setState(Greeting);
	generateEvent( _time ,false);
}

// attach my check bags
void PaxTerminalBehavior::AttachBags( std::vector<Person*>&  _vBags, const ElapsedTime& _time )
{
	int iBagCount = _vBags.size();
	std::vector<PaxVisitor*> PaxVisitorList; 
	int nCount = GetPax()->m_pVisitorList.size();
	BOOL bFind = FALSE;
	for( int j=0; j<iBagCount; ++j )	
	{
		//		_vBags[j]->setDestination( location );
		_vBags[j]->writeLog( _time, false );
		for( int i=0; i<nCount; i++ )
		{
			MobLogEntry vis;
			m_pTerm->paxLog->getItem( vis, GetPax()->m_vVisIndex[i] );
			if( vis.getID() == _vBags[j]->getID() && !bFind)
			{
				bFind = TRUE;
				GetPax()->m_pVisitorList[i] =(PaxVisitor *) _vBags[j];
				GetPax()->m_pVisitorList[i]->initOwnerPassenger( (Passenger*)m_pPerson );;
				MobileElementMovementEvent::RemoveAllMobileElementEvent( GetPax()->m_pVisitorList[i] );
				for (int k = 0; k < iBagCount; ++k)
				{
					VisitorTerminalBehavior* pVisitorBehavior = (VisitorTerminalBehavior*)m_pPerson->getBehavior(MobElementBehavior::TerminalBehavior);
					if (pVisitorBehavior)
					{
						pVisitorBehavior->SetWithOwner(true);
						if (k != iBagCount -1)
							((CGroupLeaderInfo*)GetPax()->m_pVisitorList[i]->m_pGroupInfo)->AddFollower(_vBags[k+1],_vBags[0]);	
					}
				} 
				//			MobileElementMovementEvent::RemoveAllMobileElementEvent( _vBags[j] );
				break;
			}
		}
		m_pProcessor->removePerson( _vBags[j] );
	}

	//	m_pVisitorList.clear();
	// 	if (iBagCount > 0)
	// 	{
	// 		m_pVisitorList.push_back((PaxVisitor *)_vBags[0]);
	// 	}
	//for (j = 0; j < iBagCount; ++j)
	//{
	//	if (j != iBagCount -1)
	//		((CGroupLeaderInfo*)m_pVisitorList[0]->m_pGroupInfo)->AddFollower(_vBags[j+1],_vBags[0]);	
	//}
	//	m_pProcessor->removePerson( _pVisitor );
	m_nStranded = FALSE;

	m_pPerson->setState(LeaveServer);
}

bool PaxTerminalBehavior::StickVisitorListForDestProcesOverload(const ElapsedTime& _curTime)
{
	int nCount = GetPax()->m_pVisitorList.size();
	for( int i=0; i<nCount; i++ )
	{
		PaxVisitor* pVisitor = GetPax()->m_pVisitorList[i];
		TerminalMobElementBehavior* spTerminalBehavior = NULL;
		if (pVisitor)
		{
			spTerminalBehavior = pVisitor->getTerminalBehavior();
		}

		if (spTerminalBehavior && spTerminalBehavior->StickNonPaxDestProcsOverload(_curTime))
		{
			return true;
		}
	}
	return false;
}

// output: _pProc
bool PaxTerminalBehavior::HandleGreetingLogic( Processor** _pProc,bool _bStationOnly, ElapsedTime _curTime )
{
	int nCount = GetPax()->m_pVisitorList.size();
	m_lWantToAttatchedVisID = -1;
	for( int i=0; i<nCount; i++ )
	{
		PaxVisitor* pVis = GetPax()->m_pVisitorList[i];
		if( !pVis )
		{
			long lVisitorIdx = GetPax()->m_vVisIndex[ i ];
			MobLogEntry vis;
			//m_pTerm->paxLog->GetItemByID( vis,lVisitorID );
			m_pTerm->paxLog->getItem (vis, GetPax()->m_vVisIndex[i] );
			int iVisitorType = vis.GetMobileType();
			ASSERT( iVisitorType > 0 );
			CMobileElemConstraint visitorType = m_pPerson->getType();
			visitorType.SetTypeIndex( iVisitorType );
			visitorType.SetMobileElementType(enum_MobileElementType_NONPAX);

			std::vector<ProcessorID>vGreetingPlace;
			m_pTerm->m_pGreetingProcessors->GetGreetingPlace( GetPax()->getType(), visitorType, vGreetingPlace );  // possible greeting processor group
			if( vGreetingPlace.size() <= 0 )
				continue;
		}
	}

	return false;
}

bool PaxTerminalBehavior::CopyVisitListToTrayHost(ElapsedTime _time)
{
	TrayHostList* pTrayHostList = m_pTerm->GetTrayHostList();
	//copy TrayHost NonPax visitor
	for (int i =0; i < pTrayHostList->GetItemCount(); i++)
	{
		TrayHost* pHost = pTrayHostList->GetItem(i);
		PaxVisitor* pHostVistor = GetPax()->GetVisitorItemByType(pHost->GetHostIndex());
		if (!pHostVistor)
		{
			continue;
		}

		VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior*)pHostVistor->getBehavior(MobElementBehavior::TerminalBehavior);
		CMobileElemConstraint mobileHostcon(pHostVistor->getType());
		mobileHostcon.SetTypeIndex(ALLPAX_COUNT + i);
		if (!m_pProcessor->HasDestination(mobileHostcon,m_nInGateDetailMode))
		{
			continue;
		}

		if (spTerminalBehavior && spTerminalBehavior->IfHaveOwnFlow(_time)&& CheckHostHaveOwnFlow(pHost,_time))
		{
			for (int j = 0; j < pHost->GetCount(); j++)
			{
				int nNonPaxIndex = pHost->GetItem(j);
				PaxVisitor* pFollower = GetPax()->GetVisitorItemByType(nNonPaxIndex);
				if (pFollower)
				{
					pFollower->m_pGroupInfo->SetFollowerInTray(true);
					((CGroupLeaderInfo*)pHostVistor->m_pGroupInfo)->AddFollower(pFollower, pHostVistor);	
				}
			}
			GetPax()->m_pTrayVisitorList.push_back(pHostVistor);
			pHostVistor->m_pGroupInfo->SetFollowerInTray(true);
		}
	}
	return true;
}

bool PaxTerminalBehavior::CheckHostHaveOwnFlow(TrayHost* pHost,ElapsedTime _time)
{
	for (int j = 0; j < pHost->GetCount(); j++)
	{
		int nNonPaxIndex = pHost->GetItem(j);
		PaxVisitor* pFollower = GetPax()->GetVisitorItemByType(nNonPaxIndex);
		if (pFollower)
		{
			VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior*)pFollower->getBehavior(MobElementBehavior::TerminalBehavior);
			if (spTerminalBehavior && !spTerminalBehavior->IfHaveOwnFlow(_time))
			{
				return false;	
			}
		}
		else
			return false;
	}
	return true;
}

Passenger* PaxTerminalBehavior::GetPax()
{
	return (Passenger*)m_pPerson;
}

bool PaxTerminalBehavior::ProcessPassengerSync( const ElapsedTime& _curTime )
{
	if (m_pSimFlowSync == NULL)
		return true;
	
	return m_pSimFlowSync->ProcessPassengerSync(*m_pProcessor->getID(),_curTime);
}

bool PaxTerminalBehavior::ProcessVisitorSync( Person* pVisitor,const ProcessorID& procID,const ElapsedTime& _curTime )
{
	if (m_pSimFlowSync == NULL)
		return true;
	
	return m_pSimFlowSync->ProcessVisitorSync(pVisitor,procID,_curTime);
}

void PaxTerminalBehavior::CreateSyncFlow()
{
	m_pSimFlowSync = new CSimFlowSync();
}

CSimFlowSync* PaxTerminalBehavior::GetSyncFlow() const
{
	return m_pSimFlowSync;
}

void PaxTerminalBehavior::MakePassengerSyncAvailable( const ElapsedTime& curTime )
{
	if (m_pSimFlowSync == NULL)
		return;
	
	m_pSimFlowSync->MakePassengerSyncAvailable(curTime);
}

void PaxTerminalBehavior::LoadSyncData( CInputFlowSync* pInputFlowSync )
{
	if (m_pSimFlowSync == NULL)
		return;
	
	m_pSimFlowSync->LoadData(GetPax(),pInputFlowSync);
}

void PaxTerminalBehavior::MakeVisitorSyncAvailable( Person* pVisitor, const ElapsedTime& curTime )
{
	if (m_pSimFlowSync == NULL)
		return;

	m_pSimFlowSync->MakeVisitorSyncAvailable(pVisitor,curTime);
}