#include "StdAfx.h"
#include "VisitorTerminalBehavior.h"
#include "PaxTerminalBehavior.h"
#include "pax.h"
#include "Inputs\TrayHost.h"
#include "terminal.h"
#include "inputs\AllGreetingProcessors.h"
#include "VisitorTerminalBehavior.h"
#include "Inputs/procdist.h"
#include "Inputs/ProcessorDistributionWithPipe.h"
#include "SimFlowSync.h"

VisitorTerminalBehavior::VisitorTerminalBehavior(PaxVisitor* pVisitor)
:TerminalMobElementBehavior(pVisitor)
,m_bEventInvalid(false)
,m_nFreeMoving(-1)	
,m_nFreeMovingIndex(-1)
,m_bIsGreeting(false)
,m_bInTray(false)
,m_bWithOwner(false)
{

}


bool VisitorTerminalBehavior::IfHaveOwnFlow(ElapsedTime _time)
{
	int nNextState = GetVisitor()->GetOwner()->getState();


	//bool bFreeMovingANDHasDest = nNextState == FreeMoving && m_iPreState != WalkOnPipe&& m_pProcessor->HasDestination( m_type, m_nInGateDetailMode );
	bool bFreeMovingANDHasDest = nNextState == LeaveServer && m_pPerson->getPreState()!= WalkOnPipe&& m_pProcessor->HasDestination( m_pPerson->getType(), m_nInGateDetailMode );

	//if the group in the processor,cann't be apart
	bFreeMovingANDHasDest = bFreeMovingANDHasDest &&GetVisitor()->GetOwner()!= NULL;
	//// only to sender, if visitor time  less than _time, freemoving (but ,if greet before ,want to next processor depart)
	bool bFreeMovingANDTypeIndex = nNextState == FreeMoving && m_pPerson->getPreState() != WalkOnPipe&& m_pPerson->getType().GetTypeIndex()== 1 &&  m_pPerson->getType().isDeparture() && (GetVisitor()->getVisiteTime()>=ElapsedTime(0l)) && ( GetVisitor()->getVisiteTime() < _time)&&(!GetGreetingTag());

	return (bFreeMovingANDHasDest || bFreeMovingANDTypeIndex);
}

//method called by Passenger to update visitor's location, state.
int VisitorTerminalBehavior::update( ElapsedTime _time, Point _OwnerPreLocation, bool _bBackup )
{
	int nNextState = GetOwner()->getState();
	PaxTerminalBehavior* spTerminalBehavior = (PaxTerminalBehavior*)GetOwner()->getBehavior(MobElementBehavior::TerminalBehavior);
	if (spTerminalBehavior == NULL)
	{
		return 1;
	}

	if (IfHaveOwnFlow(_time))
	{
		//////create ConvergePair,and add to GreetPool.            6.21.2004
		//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

		TrayHostList* pTrayHostList = m_pTerm->GetTrayHostList();

		if (pTrayHostList->IfNonPaxExsit(m_pPerson->getType().GetTypeIndex()) && !m_bInTray)
		{
			spTerminalBehavior->CopyVisitListToTrayHost(_time);
		}

		if (pTrayHostList->FindHostItem(m_pPerson->getType().GetTypeIndex()) || !pTrayHostList->IfNonPaxExsit(m_pPerson->getType().GetTypeIndex())\
			|| (pTrayHostList->IfNonPaxExsit(m_pPerson->getType().GetTypeIndex()) && !m_bInTray))
		{
			if (!spTerminalBehavior->StickForDestProcsOverload(_time) && !StickNonPaxDestProcsOverload(_time))
			{
				GetVisitor()->NewConvergePair(GetOwnerId(),true);
				if(GetOwner()) GetOwner()->NewConvergePair(m_pPerson->getID(),m_pPerson->getType().GetTypeIndex());

				m_pPerson->setState(FreeMoving);
				m_nFreeMoving = 1;
				m_nFreeMovingIndex = m_pPerson->getLogEntry().getCurrentCount();
				//	m_bWithOwner = false;
				m_pPerson->m_pGroupInfo->SetFollowerWithOwner(false);
				setDestination(_OwnerPreLocation);
				ElapsedTime eTime = _time + moveTime();
				generateEvent(eTime,false);

				if( getNextProcessor( eTime ) )
					generateEvent( eTime+moveTime(),false );//

				return 0;
			}
			return 1;
		}
		return 0;
	}

	m_pPerson->setState(nNextState);
	m_nQueuePathSegmentIndex = spTerminalBehavior->getCurrentIndex();
	m_pProcessor = spTerminalBehavior->getProcessor();

	CString sstr1= m_pProcessor->getID()->GetIDString();
	// write log entry
	//writeLog( _time );
	//CString _strOwnerProcessor = m_pOwnerPax->getProcessor()->getID()->GetIDString();
	GetVisitor()->writeVisitorLog( _time,_OwnerPreLocation, _bBackup, false);
	return 1;
}


bool VisitorTerminalBehavior::HandleGreetingLogic( Processor** _pProc ,bool _bStationOnly,ElapsedTime _curTime)
{
	CMobileElemConstraint paxType = m_pPerson->getType();
	paxType.SetTypeIndex( 0 );
	paxType.SetMobileElementType(enum_MobileElementType_PAX);

	std::vector<ProcessorID>vGreetingPlace;
	m_pTerm->m_pGreetingProcessors->GetGreetingPlace( paxType, m_pPerson->getType(), vGreetingPlace );
	if( vGreetingPlace.size() <= 0 )
	{
		return false;
	}

	return false;
}


//It serves greeter who just met his passenger arrival.
//The greeter's state changes to LeaveServer.
void VisitorTerminalBehavior::processServerArrival( ElapsedTime _time )
{
	//	// TRACE("\nid = %s\n",m_pProcessor->getID()->GetIDString() );
	//if( !m_pOwnerPax && m_pProcessor->getProcessorType() == HoldAreaProc)
	SetGreetingTag(false);

	if( m_pProcessor->getProcessorType() == HoldAreaProc)
	{
		if(m_pPerson->m_vpConvergePair.size() > 0)
		{
			for(std::vector<ConvergePair*>::iterator it=m_pPerson->m_vpConvergePair.begin();it!=m_pPerson->m_vpConvergePair.end();it++)
			{
				//	if((*it)->AmNowConvergeProcessor(id,m_pProcessor,this,false,p_time))//true means want to convert,waiting
				if((*it)->getWaitTag(m_pPerson->getID()))
				{
					(*it)->ReportArrival(m_pPerson->getID(), _time);
					m_bGerentGreetEventOrWait = false;
					return;
				}
			}
		}


	}
	else if (m_pProcessor->getProcessorType() == BaggageProc)
	{
		++m_iNumberOfPassedReclaimProc;
	}


	m_pPerson->setState(LeaveServer);
	m_pProcessor->beginService( m_pPerson, _time ); 
}

PaxVisitor* VisitorTerminalBehavior::GetVisitor()
{
	return (PaxVisitor*)m_pPerson;
}

Passenger* VisitorTerminalBehavior::GetOwner()
{
	return ((PaxVisitor*)m_pPerson)->GetOwner();
}

int VisitorTerminalBehavior::GetOwnerId()
{
	return ((PaxVisitor*)m_pPerson)->GetOwnerId();
}

bool VisitorTerminalBehavior::StickNonPaxDestProcsOverload( const ElapsedTime& _curTime )
{
	m_pPerson->regetAttatchedNopax();
	ProcessorDistribution* pNextProcDistribution = m_pProcessor->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
	if(pNextProcDistribution ==NULL ) return false;
	CSinglePaxTypeFlow* pProcessSingleFlow=NULL;
	ProcessorID* pProcessCurID=NULL;

	pNextProcDistribution->getDestinationGroup( RANDOM);


	ASSERT( pNextProcDistribution!=NULL);

	CFlowChannel* pInChannel=NULL;
	ProcessorArray aryOverloadDestProcs;
	ProcessorArray aryDestProcs;

	//defined for function FindDestProcessors,
	//if can not find the destination processor for 1:1 reason
	//or 1*1 reason, remember it.
	//throw exception after test every processor in distribution.
	bool bOneToOneReason = false;
	bool bOneXOneReason = false;

	do
	{
		pInChannel=NULL;
		if( static_cast<CProcessorDistributionWithPipe*>(pNextProcDistribution)->GetOneXOneState()==ChannelEnd)
		{
			pInChannel=m_FlowChannelList.PopChannel();
			m_FlowChannelList.PushChannel(pInChannel);//restore channel info for next time to use.
		}
		m_pProcessor->FindDestProcessors( m_pPerson, pNextProcDistribution, 
			pInChannel, _curTime, pProcessSingleFlow, pProcessCurID,
			aryDestProcs, bOneToOneReason, bOneXOneReason);
		Processor::PruneInvalidDestProcs(getTerminal(), m_pPerson, &aryDestProcs, NULL, &aryOverloadDestProcs );

		if(aryDestProcs.getCount()>0 && noExceptions (&aryDestProcs) && IfBaggageProcAllowed( &aryDestProcs, pNextProcDistribution ))
			return false;


	}while( pNextProcDistribution->getDestinationGroup( SEQUENTIAL) ) ;

	//check zeropercent dest group.
	if(aryDestProcs.getCount()==0)
	{
		while(pNextProcDistribution->getZeropercentDestGroup())
		{
			pInChannel=NULL;
			if( static_cast<CProcessorDistributionWithPipe*>(pNextProcDistribution)->GetOneXOneState()==ChannelEnd)
			{	
				pInChannel=m_FlowChannelList.PopChannel();
				m_FlowChannelList.PushChannel(pInChannel);//restore channel info for next time to use.
			}
			m_pProcessor->FindDestProcessors( m_pPerson, pNextProcDistribution, 
				pInChannel, _curTime, pProcessSingleFlow, pProcessCurID, 
				aryDestProcs, bOneToOneReason, bOneXOneReason);
			ASSERT( aryDestProcs.getCount()>0);

			Processor::PruneInvalidDestProcs(getTerminal(), m_pPerson, &aryDestProcs, NULL, &aryOverloadDestProcs );

			if(aryDestProcs.getCount()>0 && noExceptions (&aryDestProcs) && IfBaggageProcAllowed( &aryDestProcs, pNextProcDistribution ))
				return false;

		}
	}

	if(bOneToOneReason)
	{
		CString str = "Can't find processor to correspond to "+ m_pProcessor->getID()->GetIDString()+ " by 1:1";
		throw new ARCDestProcessorUnavailableError( getPersonErrorMsg(), 
			m_pProcessor->getID()->GetIDString(), str, _curTime.PrintDateTime());
	}
	if(bOneXOneReason)
	{
		CString str = "Cannot find processor to correspond to "+ pInChannel->GetInChannelProc()->getID()->GetIDString()+ " by 1:x:1";
		throw new ARCDestProcessorUnavailableError( getPersonErrorMsg(),
			m_pProcessor->getID()->GetIDString(), str, _curTime.PrintDateTime());
	}

	//if(aryDestProcs.getCount()>0) return false;//not need to stick.
	if(aryOverloadDestProcs.getCount()>0)
	{
		Processor* pAnOverloadProc=NULL;
		WaitingPair waitPair;
		Passenger* pOwner = GetOwner();
		if (pOwner)
		{
			waitPair.first= pOwner->getID();
			waitPair.second =m_pProcessor->getIndex();

			for( int i=0; i<aryOverloadDestProcs.getCount(); i++)
			{
				pAnOverloadProc= aryOverloadDestProcs.getItem( i );
				pAnOverloadProc->AddWaitingPair( waitPair );	// add pax id and processor idx to the dest processor.
			}
			return true;
		}
		
	}
	return false;

}

bool VisitorTerminalBehavior::ProcessPassengerSync( const ElapsedTime& _curTime )
{
	Passenger* pOwner = GetOwner();
	
	if (pOwner == NULL)
		return true;
	
	PaxTerminalBehavior* spPaxBehavior = (PaxTerminalBehavior*)pOwner->getTerminalBehavior();
	if (spPaxBehavior == NULL)
		return true;

	return spPaxBehavior->ProcessVisitorSync(m_pPerson,*m_pProcessor->getID(),_curTime);
}