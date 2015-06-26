#include "stdafx.h"
#include "common\exeption.h"
#include "common\elaptime.h"
#include "engine\pax.h"
#include "results\paxlog.h"
#include "results\out_term.h"
#include "inputs\in_term.h"
#include "Inputs\TrayHost.h"
#include "inputs\flight.h"
#include "engine\process.h"
#include "inputs\schedule.h"
#include "engine\greet.h"
#include "term_err.h"
#include "engine\movevent.h"
#include "common\CodeTimeTest.h"
#include "engine\turnaroundvisitor.h"
#include "Engine\GroupLeaderInfo.h"
#include "engine\terminal.h"
#include "Inputs\SimParameter.h"
#include "SimFlowSync.h"
#include "hold.h"
#include "Engine/ARCportEngine.h"
#include "PaxTerminalBehavior.h"
#include "AirsideMobElementBehavior.h"
#include "TurnaroundPaxTerminalBehavior.h"
#include "Engine/RetailProcessor.h"
#include "inputs\MobileElemConstraintDatabase.h"
#include "inputs\paxdata.h"
#include "Common\ProbabilityDistribution.h"
#include "Engine/MobileElementExitSystemEvent.h"
#include "Airside\AirsidePaxBusInSim.h"


extern std::vector<ConvergePair *> GreetPool;
//Functions as initialization.
Passenger::Passenger (MobLogEntry& p_entry, CARCportEngine* _pEngine ):
Person (p_entry, _pEngine )
{
	if (p_entry.isTurnaround())
	{
		MobileElement::setBehavior( new TurnaroundPaxTerminalBehavior( (TurnaroundPassenger*)this ));
	}
	else
	{
		MobileElement::setBehavior( new PaxTerminalBehavior( this ));
	}
	assert( m_pTerm );
	m_type.SetInputTerminal( _pEngine->getTerminal() );
	
	m_pTerm->paxLog->GetAttachments( m_vVisIndex, id ,p_entry.getIndex() );
	m_iBagsCount = p_entry.getBagCount();// add by tutu 2003-3-4

	if( p_entry.getGroupMember() == 0 )
	{
		PaxTerminalBehavior* spOwnerTerminalBehavior =(PaxTerminalBehavior*)getTerminalBehavior();
		if (spOwnerTerminalBehavior)
		{
			spOwnerTerminalBehavior->CreateSyncFlow();
		}
		
		int nVisCount = m_vVisIndex.size();
		int iPreType = -1;
		for( int i=0; i<nVisCount; i++ )
		{
			MobLogEntry vis;
			m_pTerm->paxLog->getItem (vis, m_vVisIndex[i] );
			
			if( !vis.getOwnStart() )
			{
				if( iPreType != vis.GetMobileType() )
				{
					PaxVisitor* pVisitors = NULL;
					if(p_entry.isTurnaround())
						pVisitors = new TurnaroundVisitor(vis,getEngine());
					else
					    pVisitors = new PaxVisitor( vis, getEngine()); //don't want to creat pair in visitor
					getEngine()->m_simBobileelemList.Register( pVisitors );
					pVisitors->initOwnerPassenger( this );
					m_pVisitorList.push_back( pVisitors );
					iPreType = vis.GetMobileType();

					pVisitors->newGroupFollower();
					pVisitors->m_pGroupInfo->SetFollowerWithOwner(true);
				}
				else
				{
					m_pVisitorList.push_back( NULL );
				}
			}
			else
			{
				int nOwnerID = vis.getOwnerID();
				int nVisID = vis.getID();
				int nVisiteType = vis.GetMobileType();
				NewConvergePair(nVisID,nVisiteType);
				m_pVisitorList.push_back( NULL );
			}		
		}

	}
	m_pRetailPaxShopping = new RetailPaxShoppingCondition();

	//initialize land side 




}

void Passenger::NewConvergePair(long _iVisitorId,long _iVisitorType)
{
	bool exitTag = false;
	std::vector<ConvergePair*>::iterator ite,iterat;
	
	for(ite= GreetPool.begin();ite!=GreetPool.end();ite++)
	{
		if((*ite)->IsMyPair(id,_iVisitorId))
		{
			bool localExit = false;
			for(iterat = m_vpConvergePair.begin();iterat!=m_vpConvergePair.end();iterat++)
			{
				if((*iterat)->IsMyPair(id,_iVisitorId)) localExit = true;
			}
			if(!localExit) m_vpConvergePair.push_back((*ite));
			exitTag = true;
			break;
		}
	}
	
	if(exitTag) return;

	ConvergePair *pPair = new ConvergePair(_iVisitorId,id); //defaulse visitor is leader
	pPair->SetPoint(id,this);
	pPair->SetTerminal(m_pTerm);
	pPair->setVisitorType(_iVisitorType);
	GreetPool.push_back(pPair);
    m_vpConvergePair.push_back(pPair);

	//// TRACE("\r\nmmmmm__Create Pair in passenger!!");
}
void Passenger::DeleteConvergePair(long _iVisitorId,bool _IsMyWantDele) //delete local pair.
{
	
	if(m_vpConvergePair.size()<=0) return;
	std::vector<ConvergePair*>::iterator ite = m_vpConvergePair.begin();
	for(ite;ite!=m_vpConvergePair.end();ite++)
	{
		
		if((*ite)->IsMyPair(id,_iVisitorId))
		{
			PaxVisitor * pVisit = NULL;
			if(_IsMyWantDele)
			{
				if((*ite)->IsLeader(id) && (*ite)->GetFollowerPoint())
				{
					pVisit =(PaxVisitor*)((*ite)->GetFollowerPoint());
					if(pVisit->getID()==_iVisitorId) pVisit->DeleteConvergePair(id,false);
				}
				else if((!(*ite)->IsLeader(id))&&(*ite)->GetLeaderPoint())
				{
					pVisit =(PaxVisitor*)((*ite)->GetLeaderPoint());
					if(pVisit->getID()==_iVisitorId) pVisit->DeleteConvergePair(id,false);
				}
				
				ite =m_vpConvergePair.erase(ite);
				DeleteGreetPool(id,_iVisitorId);
				return;
			}
			else 
			{ 
				ite =m_vpConvergePair.erase(ite);return;
				DeleteGreetPool(id,_iVisitorId);
			}
		}
		
		
	}
}
void Passenger::DeleteGreetPool(long _iFirId,long _iSecoId)  //_iFirId is PassengerId
{
	if(GreetPool.size()<=0) return;
	std::vector<ConvergePair*>::iterator ite = GreetPool.begin();
	for(ite;ite!=GreetPool.end();ite++)
	{
		if((*ite)->IsMyPair(_iFirId,_iSecoId))
		{
			if((*ite)->IsLeader(_iFirId))
			{
				(*ite)->ClearLeader();
			}
			else
			{
			   (*ite)->ClearFollower();
			}
			return;
		}
	}
}

void Passenger::setTerminalDestination (const Point& p)
{
	Person::setTerminalDestination(p);
	int nCount = m_pVisitorList.size();
	for( int i=0; i<nCount; i++ )
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		if( pVis )
		{
			pVis->setTerminalDestination(p);
		}
	}
}

// test if the person is my visitor
bool Passenger::IsMyVisitor( Person* _pPerson )const
{
	int nCount = m_pVisitorList.size();
	for( int i=0; i<nCount; i++ )
	{
		MobLogEntry vis;
		m_pTerm->paxLog->getItem( vis, m_vVisIndex[i] );
		if( vis.getID() == _pPerson->getID() )
		{
			return true;
		}
	}

	return false;
}

// test if the person is my visitor
bool Passenger::IsMyVisitor( long _lPerson )const
{
	int nCount = m_pVisitorList.size();
	for( int i=0; i<nCount; i++ )
	{
		MobLogEntry vis;
		m_pTerm->paxLog->getItem( vis, m_vVisIndex[i] );
		if( vis.getID() == _lPerson )
		{
			return true;
		}
	}

	return false;
}

 
// in:
// _bBackup: if the log is backwards.
void Passenger::writeLogEntry (ElapsedTime p_time, bool _bBackup, bool bOffset /*= true*/ )
{
	//fisrt write passenger log
	TerminalMobElementBehavior* pPaxTerminalBehavior = getTerminalBehavior();
	Point old_location = pPaxTerminalBehavior->location;
	writeLog (p_time, _bBackup, bOffset);
	
	// write log for all the follower if they are active grouped.
	m_pGroupInfo->WriteLogForFollower( p_time, _bBackup, bOffset );

	int nCount = m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		
		if( pVis )
		{
			VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior*)pVis->getTerminalBehavior();
			if (spTerminalBehavior)
			{
				try
				{
					if( !spTerminalBehavior->update( p_time, old_location, _bBackup) )
					{
						m_pVisitorList[i] = NULL;
					}
				}
				// all recoverable errors that affect 1 passenger are trapped here
				catch (ARCSimEngineException* sim_exception)
				{
					getEngine()->SendSimFormatMessage( sim_exception->getFormatErrorMsg() );
					delete sim_exception;
					m_pVisitorList[i] = NULL;
				}
				//	if(m_iPreState == Greeting) 
				//	pVis->DeleteConvergePair(id);
				}
			
		}
	}

}

void Passenger::MakePassengerSyncAvailable(const ElapsedTime& curTime)
{
	//make sync non passenger available
	if (getCurrentBehavior()->getBehaviorType() == MobElementBehavior::TerminalBehavior)
	{
		PaxTerminalBehavior* spTerminalBehavior = (PaxTerminalBehavior*)getTerminalBehavior();
		if (spTerminalBehavior)
		{
			spTerminalBehavior->MakePassengerSyncAvailable(curTime);
		}
	}
}

void Passenger::flushLog (ElapsedTime p_time, bool bmissflight)
{
	MakePassengerSyncAvailable(p_time);
	// bmissflight -- miss flight flag.
	// flash its visitor log
	int nCount = m_pVisitorList.size();
	for( int i=0; i<nCount; i++ )
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		if( pVis )
		{
			pVis->flushLog( p_time );
//			delete pVis;
			m_pVisitorList[i] = NULL;
		}
	}

	ElapsedTime terminalTime;
	if (!(m_logEntry.getTerminalEntryTime() == terminalTime) && m_logEntry.getTerminalEndTime() == terminalTime)//terminal end time did not set
	{
		m_logEntry.setTerminalEndTime(p_time);
		writeTerminalEndLogEntry(p_time);
	}

	m_iPreState = state;

	state = Death;
	if (getCurrentBehavior()->getBehaviorType() == MobElementBehavior::TerminalBehavior)
	{
		writeLogEntry (p_time, false);
	}
  
    m_logEntry.setExitTime (p_time);
    long trackCount = m_logEntry.getCurrentCount();

//     MobEventStruct *log = NULL;
//     m_logEntry.getLog (log);
// //	ajdustTimeforDisallowGroupProc( log, trackCount, 0 );
// 
//     // write first member of group
//     m_logEntry.setEventList (log, trackCount);	
// 	delete [] log;
// 	log = NULL;
	m_logEntry.saveEventsToLog();

	assert( m_pTerm );
    m_pTerm->paxLog->updateItem (m_logEntry, m_logEntry.getIndex());
	
	m_logEntry.SetMissFlightFlg(bmissflight);

	if( !bmissflight )
	{
		testForLate (p_time, id);

//		int flightIndex = m_logEntry.getDepFlight();
//		if (flightIndex != -1)
//		{ // for miss flight report used
//			assert( m_pTerm );
//			Flight *flight = m_pTerm->m_simFlightSchedule.getItem (flightIndex);
//			if (flight->isDeparting() && (p_time > flight->getDepTime()))
//				m_pEntry->SetMissFlightFlg(true);
//		}

	}

	m_pGroupInfo->FlushLogforFollower( p_time );

/* 20050607	
	// write other member of group
	for( i=0; i< m_pGroupFollowerList.getCount(); i++ )
	{
		Person* _pFollower = (Person*) m_pGroupFollowerList.getItem(i);
		assert( _pFollower->getGroupMemberMoveState() == move_follow_leader );
		
		// copy part event list 
		float dir = (i> (MAX_GROUP-1)) ? 0.0 : rotation[i+1];
		_pFollower->copyPassengerEventLog( _pFollower->getGroupAttachLogIndex(), m_logEntry.getCurrentCount(), m_logEntry );
		_pFollower->adjustCoordinates( _pFollower->getLogEntry().getEventList(), _pFollower->getGroupAttachLogIndex(), _pFollower->getLogEntry().getCurrentCount(),
										  dir, GROUP_OFFSET );
		
		// write log entry
		_pFollower->flushGroupFollowerLog( p_time, dir, i+1 );
		testForLate( p_time, _pFollower->getID() );
	}
*/
    m_logEntry.clearLog();	

	// when the person death, decrease the count of area
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	m_pTerm->m_pAreas->handlePersonDeath( this, spTerminalBehavior->location, p_time );

	// when the person death, notice the bus if it has one.
	CAirsidePaxBusInSim* pBus = spTerminalBehavior->GetAirsideBus();
	if(pBus != NULL)
		pBus->handlePersonDeath(this);
	//person death,delete convergePair



	int nPairCount = m_vpConvergePair.size();
	if( nPairCount<0) return;
	
	int  pIdGroup[256];
	int h=0;
	for(h=0;h<256;h++) 
		pIdGroup[h] = -1;
	

    int iIndex =0;
	std::vector<ConvergePair*>::iterator ite = m_vpConvergePair.begin();
	for(ite;ite!=m_vpConvergePair.end();ite++)  //call part delete pair
	{
		if((*ite)->IsMyPair(id))
		{
			pIdGroup[iIndex] = -2;
			
			if((*ite)->IsLeader(id))
			{
				pIdGroup[iIndex] = (*ite)->GetFollowerIndex();
				//if((*ite)->GetFollowerPoint())
				//{
				//	PaxVisitor * pVis = (PaxVisitor *)((*ite)->GetFollowerPoint());
				 //   pVis->DeleteConvergePair(id,false);
			//	}
				
			}
			else
			{
			//	if((*ite)->GetLeaderPoint())
			//	{
			//		PaxVisitor * pVis = (PaxVisitor *)((*ite)->GetLeaderPoint());
			//		pVis->DeleteConvergePair(id,false);
			//}
				pIdGroup[iIndex] = (*ite)->GetLeaderIndex();
			}
			iIndex++;
		}
	}

	int i_Count=0;
	for(i_Count=0;i_Count<nPairCount;i_Count++) 
	{
		if(pIdGroup[i_Count]!=-1)
		{
		   DeleteConvergePair(pIdGroup[i_Count],true);
		}
	}



	//m_vpConvergePair.clear(); //delete local


//	for(int n=0;n<iIndex;n++) // last to delete Grobal GreetPool
	//	if(iVisitorIndex[n] != -1) DeleteGreetPool(id,iVisitorIndex[n]);

	if(getEngine()->m_simBobileelemList.IsAlive(this))
	{
		MobileElementExitSystemEvent *pEvent = new MobileElementExitSystemEvent(this, p_time);
		pEvent->addEvent();
	}
	else
	{
		ASSERT(0);
	}


}

 
void Passenger::testForLate (ElapsedTime curTime, long _lPaxID )
{
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

    int flightIndex = m_logEntry.getDepFlight();
    if (flightIndex == -1)
        return;

	assert( m_pTerm );
    Flight *flight = getEngine()->m_simFlightSchedule.getItem (flightIndex);

	if(!flight->isDeparting())//arrival flight. donot need test.
		return;

	if (getEngine()->IsAirsideSel() && !flight->IsAirsideFlightDepartured())//flight doesn't leave won't delay
		return;

	if (getEngine()->IsAirsideSel() && !flight->RetrieveAirsideDepartureTime())
		return;
	

	//-------------------------------------------------------------------------
	// late check with flight delay time.
	bool bOnlyRunTerminal = getEngine()->IsTerminal() && !getEngine()->IsAirsideSel();
	ElapsedTime actureDepTimeWithDelay = flight->GetRealDepTime();// not delay.
	if(bOnlyRunTerminal && flight->getDepDelay() > ElapsedTime(0L))// late
	{
		actureDepTimeWithDelay = flight->getDepTime() + flight->getDepDelay();//delayed
	}

	//-----------------------------------------------------------------------

	if (flight->isDeparting() && (curTime > actureDepTimeWithDelay))
    {
		char szBuffer[128] = {0};
		CString sPrint;
		FORMATMSGEX msgEx;

		CStartDate sDate = m_pTerm->m_pSimParam->GetStartDate();
		bool bAbsDate;
		COleDateTime date;
		int nDtIdx;
		COleDateTime time;
		CString sDateTimeStr;
		sDate.GetDateTime( curTime, bAbsDate, date, nDtIdx, time );
		if( bAbsDate )
		{
			sDateTimeStr = date.Format(_T("%Y-%m-%d"));
		}
		else
		{
			sDateTimeStr.Format("Day %d", nDtIdx + 1 );
		}
		sDateTimeStr += "  ";
		ElapsedTime tmptime(curTime.asSeconds() % WholeDay);
		sDateTimeStr += tmptime.printTime();

		msgEx.strTime			= sDateTimeStr;
		msgEx.strMsgType		= "MISSED FLIGHT";
		msgEx.strProcessorID	= spTerminalBehavior->getProcessor() ? spTerminalBehavior->getProcessor()->getID()->GetIDString() : "";
		msgEx.strPaxID.Format("%ld", _lPaxID );
		flight->getFullID( szBuffer, 'D' );
		//szBuffer[7] = NULL;
		msgEx.strFlightID.Format(_T("%s"),szBuffer) ;		
		m_type.screenPrint( sPrint, 0, 128 ); 
		msgEx.strMobileType		= sPrint;//CString( szBuffer );
		if(m_pGroupInfo->IsFollower())
		{
			long lID = m_pGroupInfo->GetGroupLeader()->getID();
			msgEx.strOther.Format("missed its flight by leader %d in group", lID);
		}
		else
		{
			msgEx.strOther.Format( "missed its flight by %s" , (curTime - actureDepTimeWithDelay ).printTime() );
		}
		m_logEntry.SetRealDepartureTime(actureDepTimeWithDelay);
		m_logEntry.SetMissFlightTime(curTime);
		msgEx.diag_entry.eType	= MISS_FLIGHT;
		msgEx.diag_entry.lData	= m_logEntry.getIndex();
		
		getEngine()->SendSimFormatExMessage( msgEx );
   }
}

//Returns sum of group size of passenger and of visitor.
//Default input value is zero.
int Passenger::GetActiveTotalSize() const
{
	int nRes = GetActiveGroupSize();
	int nCount = m_pVisitorList.size();
	for( int i=0; i<nCount; i++ )
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		
		if( pVis && pVis->getType().GetTypeIndex() == 1 )
		{
			nRes += pVis->GetActiveGroupSize();
		}
	}
	return nRes;
};

/*
int Passenger::getAttatchedNopaxCount() const
{
	int nRes = 0;
	int nCount = m_pVisitorList.size();
	for( int i=0; i<nCount; i++ )
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		if( pVis )
		{
			nRes += pVis->getGroupSize();
		}
	}
	return nRes;
}
*/
// get the number of each type of nopax which is attatch with its owner
void Passenger::regetAttatchedNopax()
{
	m_type.getNonpaxCount().init();

	int nCount = m_pVisitorList.size();
	for( int i=0; i<nCount; i++ )
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		if( pVis )
		{
			int iNonpaxType = pVis->getType().GetTypeIndex();
			m_type.getNonpaxCount().increaseNonpaxCount( iNonpaxType, pVis->GetActiveGroupSize() );
		}
	}
}


// new person for group follower
void Passenger::newGroupFollower( void )
{
	MobLogEntry _followerEntry = m_logEntry;
	PaxTerminalBehavior* spTerminalBehavior = (PaxTerminalBehavior*)getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	for( int i=1; i< GetAdminGroupSize(); i++ )
	{
		_followerEntry.incPax();
		_followerEntry.ReleaseCarryon();

		Person* _pFollower = new Passenger( _followerEntry, getEngine()  );
		PaxTerminalBehavior* spFollowerTerminalBehavior = (PaxTerminalBehavior*)_pFollower->getTerminalBehavior();
		if (spFollowerTerminalBehavior)
		{
			getEngine()->m_simBobileelemList.Register( _pFollower );
			((CGroupLeaderInfo*)m_pGroupInfo)->AddFollower( _pFollower, this );	
		}
			
	}

	spTerminalBehavior->LoadSyncData(m_pTerm->m_pInputFlowSync);
}


PaxVisitor* Passenger::GetVisitorItemByType(int nTypeIndex)
{
	for (int i = 0; i < (int)m_pVisitorList.size(); i++)
	{
		MobLogEntry vis;
		m_pTerm->paxLog->getItem( vis, m_vVisIndex[i] );
		if( vis.GetMobileType() == nTypeIndex )
		{
			return m_pVisitorList[i];
		}
	}
	return NULL;
}

void Passenger::SetFollowerArrivalDoor(bool bArrivalDoor)
{
	Person::SetFollowerArrivalDoor(bArrivalDoor);

	for (int i = 0; i < (int)m_pVisitorList.size(); i++)
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		if(pVis)
		{
			pVis->m_pGroupInfo->setFollowerArrivalDoor(bArrivalDoor);
		}
	}
}

void Passenger::SetEntryTime(const ElapsedTime& tEntryTime)
{
	Person::SetEntryTime(tEntryTime);

	for (int i = 0; i < (int)m_pVisitorList.size(); i++)
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		if(pVis)
		{
			pVis->m_pGroupInfo->setEntryTime(tEntryTime);
		}
	}
}
Passenger::~Passenger()
{
	//delete m_pSimFlowSync;
	//m_pSimFlowSync = NULL;

	if (m_pRetailPaxShopping)
	{
		delete m_pRetailPaxShopping;
		m_pRetailPaxShopping = NULL;
	}
}

void Passenger::AttachCarryon( PaxVisitor *pCarryon )
{
	int nCount = m_pVisitorList.size();
	for( int i=0; i<nCount; i++ )
	{
		MobLogEntry vis;
		m_pTerm->paxLog->getItem( vis, m_vVisIndex[i] );
		if( vis.getID() == pCarryon->getID())
		{
			m_pVisitorList[i] = pCarryon;
			m_pVisitorList[i]->initOwnerPassenger( this );
			break;
		}
	}

}

void Passenger::AddEntry( RetailProcessor* pRetailProc,int nProductID,int nCount )
{
	if (m_pRetailPaxShopping)
	{
		m_pRetailPaxShopping->AddEntry(pRetailProc,nProductID,nCount);
	}
}


void Passenger::ClearEntry()
{
	if (m_pRetailPaxShopping)
	{
		m_pRetailPaxShopping->ClearEntry();
	}
}

void Passenger::CheckOutShopping( RetailProcessor* pRetailProc,const ElapsedTime& curTime )
{
	for (int i = 0; i < m_pRetailPaxShopping->GetCount(); i++)
	{
		PaxShoppingCondition* pPaxCon = m_pRetailPaxShopping->FindEntry(i);
		if (pPaxCon == NULL)
			continue;

		if (pPaxCon->m_productMap.empty())
			continue;

		if (pPaxCon->m_pRetailProc == NULL)
			continue;

		std::map<int,int>::iterator iter = pPaxCon->m_productMap.begin();
		for (; iter != pPaxCon->m_productMap.end(); ++iter)
		{
			std::pair<int,int> mapPair = *iter;
			pPaxCon->m_pRetailProc->WriteLogCheckOutShopping(this,mapPair.first,mapPair.second,pRetailProc,curTime);
		}
	}
	m_pRetailPaxShopping->ClearEntry();
}
void Passenger::CancelShopping( RetailProcessor* pRetailProc, const ElapsedTime& curTime )
{
	//check retail processor valid
	for (int i = 0; i < m_pRetailPaxShopping->GetCount(); i++)
	{
		PaxShoppingCondition* pPaxCon = m_pRetailPaxShopping->FindEntry(i);
		if (pPaxCon == NULL)
			continue;
	
		if (pPaxCon->m_productMap.empty())
			continue;
		
		if (pPaxCon->m_pRetailProc == NULL)
			continue;
		
		std::map<int,int>::iterator iter = pPaxCon->m_productMap.begin();
		for (; iter != pPaxCon->m_productMap.end(); ++iter)
		{
			std::pair<int,int> mapPair = *iter;
			pPaxCon->m_pRetailProc->WriteLogCancelShopping(this,mapPair.first,mapPair.second,pRetailProc,curTime);
		}
	}
	m_pRetailPaxShopping->ClearEntry();
}

DistanceUnit Passenger::getInStep( void ) const
{
	double dImpact = 0.0;
	CMobileElemConstraintDatabase *pImpactInStepDB = GetTerminal()->paxDataList->getImpactInStep();
	if (pImpactInStepDB)
	{
		for (unsigned i = 0; i < m_pVisitorList.size(); i++)
		{
			PaxVisitor* pVisitor = m_pVisitorList[i];
			if (pVisitor == NULL)
				continue;
			int nTypeIndex = pVisitor->getType().GetTypeIndex();
			CMobileElemConstraintDatabase* pMobConDatabase = m_pTerm->paxDataList->getNopaxData(nTypeIndex);
			if (pMobConDatabase == NULL)
				continue;
			const ConstraintEntry* pEntry = pMobConDatabase->FindFitEntry(m_type);
			if (pEntry == NULL)
				continue;

			CMobileElemConstraint* p_const = (CMobileElemConstraint*)pEntry->getConstraint();
			const ProbabilityDistribution* pDistribution = pImpactInStepDB->FindEqual(*p_const);
			if (pDistribution == NULL)
				continue;

			dImpact += pDistribution->getMean()/100.0;
		}
	}
	
	  return (m_in*(1-dImpact)*SCALE_FACTOR + ((GetActiveGroupSize() < 4)? 0: ((GetActiveGroupSize() < 6)? 20: 40))); 
}

void Passenger::SetWalkOnBridge( bool b )
{
	Person::SetWalkOnBridge( b);
	for (int i = 0; i < (int)m_pVisitorList.size(); i++)
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		if(pVis)
		{
			pVis->SetWalkOnBridge(b);
		}
	}
}
void Passenger::ChangeZ(double dz)
{
	Person::ChangeZ( dz);
	for (int i = 0; i < (int)m_pVisitorList.size(); i++)
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		if(pVis)
		{
			pVis->ChangeZ(dz);
		}
	}
}
void Passenger::SetBridgeState(ArrDepBridgeState state)
{
	Person::SetBridgeState( state);

	for (int i = 0; i < (int)m_pVisitorList.size(); i++)
	{
		PaxVisitor* pVis = m_pVisitorList[i];
		if(pVis)
		{
			pVis->SetBridgeState(state);
		}
	}
}



RetailPaxShoppingCondition::~RetailPaxShoppingCondition()
{
	ClearEntry();
}

void RetailPaxShoppingCondition::AddEntry( RetailProcessor* pRetailProc,int nProductID,int nCount )
{
	for (size_t i = 0; i < m_vRetailShoppingCondition.size(); i++)
	{
		PaxShoppingCondition* pItem = m_vRetailShoppingCondition[i];
		if (pItem->m_pRetailProc == pRetailProc)
		{
			std::map<int,int>::iterator iter = pItem->m_productMap.find(nProductID);
			if (iter != pItem->m_productMap.end())
			{
				iter->second += nCount;
				return;
			}
		}
	}

	PaxShoppingCondition* pNewItem = new PaxShoppingCondition();
	pNewItem->m_pRetailProc = pRetailProc;
	pNewItem->m_productMap.insert(std::make_pair(nProductID,nCount));
	m_vRetailShoppingCondition.push_back(pNewItem);
}

int RetailPaxShoppingCondition::GetCount()const
{
	return (int)m_vRetailShoppingCondition.size();
}

PaxShoppingCondition* RetailPaxShoppingCondition::FindEntry( int ndx ) const
{
	if (ndx >= 0 && ndx < GetCount())
	{
		return m_vRetailShoppingCondition.at(ndx);
	}
	return NULL;
}

void RetailPaxShoppingCondition::DeleteEntry( RetailProcessor* pRetailProc )
{
	for (size_t i = 0; i < m_vRetailShoppingCondition.size(); i++)
	{
		PaxShoppingCondition* pItem = m_vRetailShoppingCondition[i];
		if (pItem->m_pRetailProc == pRetailProc)
		{
			delete pItem;
			m_vRetailShoppingCondition.erase(m_vRetailShoppingCondition.begin() + i);
			break;
		}
	}
}


void RetailPaxShoppingCondition::ClearEntry()
{
	for (size_t i = 0; i < m_vRetailShoppingCondition.size(); i++)
	{
		delete m_vRetailShoppingCondition[i];
	}
	m_vRetailShoppingCondition.clear();
}