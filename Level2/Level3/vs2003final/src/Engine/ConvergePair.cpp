// onvergePair.cpp: implementation of the ConvergePair class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ConvergePair.h"
#include "inputs\mobileelemconstraint.h"
#include "inputs\allgreetingprocessors.h"
#include "inputs\ProcessorDistributionWithPipe.h"
#include "inputs\MobileElemConstraintDatabase.h"
#include "inputs\paxdata.h"
#include "engine\greet.h"
#include "common\elaptime.h"
#include "hold.h"
#include "engine\terminal.h"
#include "inputs\subflow.h"
#include "Engine\TerminalMobElementBehavior.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
std::vector<ConvergePair *> GreetPool;  //Global Pool 

ConvergePair::ConvergePair(int _iLeadIndex,int _iFollowIndex)//,int _iCreaterIndex)
{
	ASSERT(m_iLeadIndex == m_iFollowIndex);
	m_iLeadIndex = _iLeadIndex;
	m_iFollowIndex = _iFollowIndex;
//	m_iCreatePairId = _iCreaterIndex;
	m_iVisitorType = -1;
    
    m_bLeadArrival = false;
	m_bFollowArrival = false;
	m_bFollowInHolding = false;

	m_bDelete = false;

    m_bLeaderWaiting = false;
    m_bFollowWaiting = false;
	m_pLeader = NULL;
	m_pFollow = NULL;
	m_pTerm = NULL;
	m_BeforeLinkWaitProc = NULL;
}

ConvergePair::~ConvergePair()
{
}

/************************************************************************
FUNCTION:check follower or leader is want to converge in next processorGroup,
		if want  return next processor to it
IN		:_index,mobileElement ID
		_pPerson,the current person
		_bStationOnly
		_curTime, current time
OUT		:the pointer point to next processor

RETURN	: true,the person want to converge in next proc,and return NEXT PROC
		  FALSE,the person don't want.                                                              
/************************************************************************/
 bool ConvergePair::AmNowConvergeProcessor(int _index,Processor **_iProc,Person *_pPerson,bool _bStationOnly, ElapsedTime _curTime)
{
	ProcessorArray vNextProcArray;
	bool LeadTag = false;
	int VisitorID = 0;
	
	CMobileElemConstraint visType(m_pTerm) ;
	
    // first contact with Pair, set self's point 
	if(_index == m_iLeadIndex) 
	{
		LeadTag = true;
		if(m_pLeader==NULL) m_pLeader = _pPerson;
	}
	else if(_index == m_iFollowIndex)
	{
		LeadTag = false;
		if(m_pFollow==NULL) m_pFollow = _pPerson;
	}
	
	// find all greetingplace 
	std::vector<ProcessorID>vGreetingPlace;
	if(_pPerson->getType().GetTypeIndex()>0) //_pPerson is visitor
	{
		CMobileElemConstraint paxType = _pPerson->getType();
		paxType.SetTypeIndex(0);	
		paxType.SetMobileElementType(enum_MobileElementType_PAX);		
		m_pTerm->m_pGreetingProcessors->GetGreetingPlace(paxType,_pPerson->getType(),vGreetingPlace );
		
	}
	else //pax
	{
		Passenger * pPax = (Passenger *)_pPerson;
		
		visType = _pPerson->getType();
		visType.SetTypeIndex(m_iVisitorType);
		visType.SetMobileElementType(enum_MobileElementType_PAX);
		m_pTerm->m_pGreetingProcessors->GetGreetingPlace(_pPerson->getType(),visType,vGreetingPlace );	
	}
	
	
	if( vGreetingPlace.size() <= 0 )
		return false;
	
	
	//check if want to greet
	TerminalMobElementBehavior* spIterTerminalBehavior = _pPerson->getTerminalBehavior();
	if(spIterTerminalBehavior &&
		spIterTerminalBehavior->FindGreetingProcFromDestination(vGreetingPlace,vNextProcArray,_index,_bStationOnly,_curTime))
	{

		bool iUserDefineIsNopax = false;
		int iCount = vNextProcArray.getCount();
		Processor * pNextProces = vNextProcArray.getItem(0);
        if(iCount>1)
		{
			for(int d=0;d<iCount;d++)
			{
				Processor *proc =  vNextProcArray.getItem(d);
//				// TRACE("\r\n &&&&==&&&& proc[%d]==%s",d,proc->getID()->GetIDString());
			}
		}
		// before leaderTag is default; next work is read the user define LeaderTag;
		// set user difine tag for Follower or Leader
		
		if(_pPerson->getType().GetTypeIndex()==0) //person is pax
		{
			//CMobileElemConstraintDatabase *pMobileElemConstDB = m_pTerm->paxDataList->getPaxData(GroupSizeDatabase);
			//ProcessorDistribution *m_pList =(ProcessorDistribution *)(pMobileElemConstDB->FindFit (paxType));
			
			ProcessorDistribution *pList = NULL;
			if(LeadTag) 
			{
				if(m_pFollow)
				{
					TerminalMobElementBehavior* spFollowerTerminalBehavior = m_pFollow->getTerminalBehavior();
					if (spFollowerTerminalBehavior)
					{
						pList = spFollowerTerminalBehavior->getDistribution();
					}
					
				}
				if(m_pFollow&&(pList==NULL)) 
				{
					TerminalMobElementBehavior* spFollowerTerminalBehavior = m_pFollow->getTerminalBehavior();
					if (spFollowerTerminalBehavior)
					{
						pList = spFollowerTerminalBehavior->getProcessor()->getNextDestinations(m_pFollow->getType(),spFollowerTerminalBehavior->GetInGateDetailMode());
						if(pList==NULL) spFollowerTerminalBehavior->getProDistribution();
					}
					
//					// TRACE("\r\n@@@m_pFollowProcessor==%s",m_pFollow->getProcessor()->getID()->GetIDString());
				}
			}
			else
			{
				if(m_pLeader) 
				{
					TerminalMobElementBehavior* spLeaderTerminalBehavior = m_pLeader->getTerminalBehavior();
					if (spLeaderTerminalBehavior)
					{
						pList = spLeaderTerminalBehavior->getDistribution();
					}
					
				}
				if(m_pLeader&&(pList==NULL)) 
				{
					TerminalMobElementBehavior* spLeaderTerminalBehavior = m_pLeader->getTerminalBehavior();
					if (spLeaderTerminalBehavior)
					{
						pList = spLeaderTerminalBehavior->getProcessor()->getNextDestinations(m_pLeader->getType(),spLeaderTerminalBehavior->GetInGateDetailMode());
						//// TRACE("\r\n@@@LeaderProcessor==%s",m_pLeader->getProcessor()->getID()->GetIDString());
						if(pList==NULL) spLeaderTerminalBehavior->getProDistribution();
					}
					
				}
			}
			
			if(pList == NULL)
			{
					Processor * pStartProc =  m_pTerm->procList->getProcessor (START_PROCESSOR_INDEX);
					pList = pStartProc->getNextDestinations(visType,spIterTerminalBehavior->GetInGateDetailMode());
			}
			//// TRACE("\r\nProcessor==%s",_pPerson->getProcessor()->getID()->GetIDString());
//			if(pList == NULL)
			//{
			//	if (_pPerson->getID() == 290)
			//	{
			//		FILE *fp = fopen("C:\\FindGreetProc.txt","a+");
			//		fprintf(fp,"\r\nperson_m_type ==%d",_pPerson->getType().GetTypeIndex()); 
			//		fprintf(fp,"\r\nProcessor==%s",_pPerson->getProcessor()->getID()->GetIDString());
			//		char buff[128];
			//		memset((void*)&buff,0,128);
			//		if(m_pLeader)
			//		{
			//			m_pLeader->getType().screenPrint(buff);
			//			fprintf(fp,"\r\n%s",buff);
			//			fprintf(fp,"\r\n m_pLeader processor==%s",m_pLeader->getProcessor()->getID()->GetIDString());
			//		}
			//		if(m_pFollow) 
			//		{
			//			m_pFollow->getType().screenPrint(buff);
			//			fprintf(fp,"\r\n%s",buff);
			//			fprintf(fp,"\r\n m_pFoller processor==%s",m_pFollow->getProcessor()->getID()->GetIDString());
			//		}
			//		fprintf(fp,"\r\n");
			//		fclose(fp);
			//	}
			//}
		
		//	ASSERT(pList);
			if(pList) pList->getDestinationGroup(RANDOM);
			CProcessorDistributionWithPipe * pVisitDitribution= (CProcessorDistributionWithPipe*)pList;
			if(pVisitDitribution)
			{
			
			    if(pVisitDitribution->GetFollowState()) //defalse visitor is leader,return true is means visitor is follow
			  	{	if(IsLeader(_index))   //check pax is leader,visitor is follower, no the defalse,don't want exchange
			  			    iUserDefineIsNopax = false;
			        else iUserDefineIsNopax = true;//checked visitor is leader,is defalse,want to exchanged
			  	}
				else
				{	if(IsLeader(_index))   //check pax is leader,visitor is follower, no the defalse,don't want exchange
			    	    iUserDefineIsNopax = true;
				    else iUserDefineIsNopax = false;//checked visitor is leader,is defalse,want to exchanged
				}
			}
		}
		else  //visitor
		{
			ProcessorDistribution *pList = NULL;
		
			   pList = spIterTerminalBehavior->getDistribution();
			
			if(pList == NULL)
			{
				pList = spIterTerminalBehavior->getProcessor()->getNextDestinations(_pPerson->getType(),spIterTerminalBehavior->GetInGateDetailMode());
				//CMobileElemConstraintDatabase *pMobileElemConstDB = m_pTerm->paxDataList->getPaxData(GroupSizeDatabase);
				//pList =(ProcessorDistribution *)(pMobileElemConstDB->FindFit (visType));
				if(pList==NULL) spIterTerminalBehavior->getProDistribution();
			}
			//// TRACE("\r\n@@@%sProcessor==",_pPerson->getProcessor()->getID()->GetIDString());

////			if(pList == NULL)
//					{
//						if (_pPerson->getID() == 290)
//						{
//							FILE *fp = fopen("C:\\FindGreetProc.txt","a+");
//							fprintf(fp,"\r\nperson_m_type ==%d",_pPerson->getType().GetTypeIndex()); 
//							fprintf(fp,"\r\nProcessor==%s",_pPerson->getProcessor()->getID()->GetIDString());
//							char buff[128];
//							memset((void*)&buff,0,128);
//							if(m_pLeader)
//							{
//								m_pLeader->getType().screenPrint(buff);
//								fprintf(fp,"\r\n%s",buff);
//								fprintf(fp,"\r\n m_pLeader processor==%s",m_pLeader->getProcessor()->getID()->GetIDString());
//							}
//							if(m_pFollow) 
//							{
//								m_pFollow->getType().screenPrint(buff);
//								fprintf(fp,"\r\n%s",buff);
//								fprintf(fp,"\r\n m_pFoller processor==%s",m_pFollow->getProcessor()->getID()->GetIDString());
//							}
//							fprintf(fp,"\r\n");
//							fclose(fp);
//						}
//					}
		
			ASSERT(pList);


			if(pList) pList->getDestinationGroup(RANDOM);
			CProcessorDistributionWithPipe * pVisitDitribution= (CProcessorDistributionWithPipe*)pList;
            
			if(pVisitDitribution)
			{
				if(pVisitDitribution->GetFollowState()) //defalse visitor is leader,return true is means visitor is follow
				{	
					if(IsLeader(_index))   //check visitor is leader, defalse, want exchange
						iUserDefineIsNopax = true;
					else iUserDefineIsNopax = false;//checked visitor is follow,no defalse,don't want to exchanged
				}
				else
				{
					if(IsLeader(_index))   //check visitor is leader, defalse, want exchange
						iUserDefineIsNopax = false;
					else iUserDefineIsNopax = true;//checked visitor is follow,no defalse,don't want to 
				}
			}
			
		}
		
		if(iUserDefineIsNopax &&(!m_bLeaderWaiting)&&(!m_bFollowWaiting))
			exchangeLeadFollow(); // exchange part!
		
		if(iCount==1)
		{
			*_iProc = vNextProcArray.getItem(0);
			setWaitTag(_index,true);
			return true;
		}
		else
		{
			if(_index == m_iFollowIndex)
			{
				if(m_bLeadArrival)
				{
					TerminalMobElementBehavior* spLeaderTerminalBehavior = m_pLeader->getTerminalBehavior();
					if (spLeaderTerminalBehavior)
					{
						*_iProc = spLeaderTerminalBehavior->getProcessor();
					}
					setWaitTag(_index,true); //set tag to arrival before;
					return true;
					
				}
				else
				{
			        if(getWaitTag(_index)) //the second 
					{
						TerminalMobElementBehavior* spLeaderTerminalBehavior = m_pLeader->getTerminalBehavior();
						if (spLeaderTerminalBehavior)
						{
							*_iProc = spLeaderTerminalBehavior->getProcessor();
						}
						return true;
					}
											
				
						
					//if (_pPerson->getID() == 290)
					//{
					//	// TRACE("290");
					//}
					//if (_pPerson->getID() == 289)
					//{
					//	// TRACE("298");
					//}
					//if (this->m_iFollowIndex == 281 || this->m_iLeadIndex == 281)
					//{
					//	// TRACE("281");
					//}
					*_iProc = GetNextConvergeProcessor(_index,vNextProcArray);//getNextProc
					if(*_iProc == NULL) return true;
					m_bFollowInHolding = true;
					TerminalMobElementBehavior* spFollowerTerminalBehavior = m_pFollow->getTerminalBehavior();
					if (spFollowerTerminalBehavior)
					{
						m_BeforeLinkWaitProc = spFollowerTerminalBehavior->getProcessor();
					}
					
					//m_beforeProcessFlow = m_pFollow->m_pSubFlow->GetInternalFlow(); 
					//m_beforeProcessFlowCurID = m_pFollow->m_ProcessFlowCurID;
					
					setWaitTag(_index,true); //set tag to arrival before;
					return true;
				}
			}
			else
			{ //leader select random
				*_iProc = vNextProcArray.getItem( random( iCount) ); 
				setWaitTag(_index,true);//set tag to arrival before;
				return true;
			} 
		}
	} //
	else 
		return false;
}

/************************************************************************
FUNCTION:Follower get next processor
IN		:_iIndex,the current person
		:_nextProcGroup, processor array need to be flited
RETURN	:
		If find,return the found processor
		fail,return NULL
/************************************************************************/
Processor *ConvergePair::GetNextConvergeProcessor(int _iIndex,ProcessorArray &_nextProcGroup)
{
	//if(m_bDelete) return NULL;
	if(m_iFollowIndex != _iIndex) return NULL;
	
	int nCount = _nextProcGroup.getCount();
	HoldingArea* _linkHold = (HoldingArea*)( m_pTerm->procList->GetWaitArea(_nextProcGroup[random(nCount)]->getID()));
	if( _linkHold == NULL )	// can not get link hold area
	{
		for(int i=0;i<nCount;i++)
		{
			_linkHold = (HoldingArea*)( m_pTerm->procList->GetWaitArea(_nextProcGroup[i]->getID()));
			if(_linkHold != NULL) break;
		}
	}
	return _linkHold;
	
}

bool ConvergePair::IsMyPair(int _iIndex)
{
	if((m_iLeadIndex == _iIndex) ||(m_iFollowIndex == _iIndex))
		return true;
	else return false;
}

bool ConvergePair::IsMyPair(int _iFirIndex,int _iSecIndex)
{

	if(((m_iLeadIndex == _iFirIndex)&&(m_iFollowIndex == _iSecIndex))
		||((m_iLeadIndex == _iSecIndex)&&(m_iFollowIndex == _iFirIndex)))
		return true;
	else return false;
}
/************************************************************************
FUNCTION:follower or leader report arrival to convergepair ,and serve the follower or leader
IN		:_index,the person that report arrival
		p_time,current time                                                                  
/************************************************************************/
void ConvergePair::ReportArrival(int _index,ElapsedTime p_time)
{
	//if( !m_pFollow )
	//	return;

	 ASSERT(m_iLeadIndex!=m_iFollowIndex);
	
 //   int i=0;
	//if(_index==160) 
 //        i = 160;
	//if(_index == 162)
	//	i=162;
	if(_index == m_iLeadIndex) 
	{
		m_bLeadArrival = true;
        //// TRACE("%s%d","\r\n@@@ LeadIndex_Arrival==",_index);
	}
	if(_index == m_iFollowIndex) 
	{
		//// TRACE("%s%d","\r\n@@@ m_FollowIndex_Arrival==",_index);
		if(!m_bFollowInHolding)
			m_bFollowArrival = true; // 
	}



	if(m_iLeadIndex ==_index)  //leader 
	{
		if( m_bFollowArrival || m_bFollowInHolding ) // follow arrival ago ,call it to there  ||m_bHoding
		{
			TerminalMobElementBehavior* spFollowerTerminalBehavior = m_pFollow->getTerminalBehavior();
			if (spFollowerTerminalBehavior)
			{
				if(m_bFollowInHolding) //
				{
					spFollowerTerminalBehavior->getProcessor()->removePerson(m_pFollow);
	//				m_pFollow->getProcessor()->makeAvailable( m_pFollow, p_time, false );
					spFollowerTerminalBehavior->setProcessor(m_BeforeLinkWaitProc);
					//m_BeforeLinkWaitProc->addPerson( m_pFollow);
					m_pFollow->setState(LeaveServer);
					spFollowerTerminalBehavior->getProcessor()->beginService(m_pFollow,p_time);
					m_bFollowInHolding = false;
				}
				else
				{
					m_pFollow->writeLogEntry( p_time, false );
					TerminalMobElementBehavior* spLeaderBehavior = m_pLeader->getTerminalBehavior();
					Point leaderPt;
					m_pLeader->getTerminalPoint(leaderPt);
					m_pFollow->setTerminalDestination(leaderPt);
	//				m_pFollow->getProcessor()->removePerson(m_pFollow);
	//				m_pFollow->getProcessor()->makeAvailable( m_pFollow, p_time, false );
					//// TRACE("\r\n%s",m_pFollow->getProcessor()->getID()->GetIDString());
					GreetEvent *event =NULL;
					if(m_pFollow->getType().GetTypeIndex()==0)
						event = new GreetEvent((Passenger*)m_pFollow,(PaxVisitor *)m_pLeader, p_time + m_pFollow->moveTime());
					else
						event = new GreetEvent((Passenger *)m_pLeader,(PaxVisitor *)m_pFollow, p_time + m_pFollow->moveTime());
					event->addEvent();
					ClearTag();
				}
			}
				
			
		}
		else
		{
			m_pLeader->setState(WaitInHoldingArea);
		}
		
		
	}
	else                   //follower
	{
		if(m_bLeadArrival)   // leader arrival before ,i want to go there.
		{
			TerminalMobElementBehavior* spFollowerTerminalBehavior = m_pFollow->getTerminalBehavior();
			if (spFollowerTerminalBehavior)
			{
				if(m_bFollowInHolding)
				{
					spFollowerTerminalBehavior->getProcessor()->removePerson(m_pFollow);
	//				m_pFollow->getProcessor()->makeAvailable( m_pFollow, p_time, false );
					spFollowerTerminalBehavior->setProcessor(m_BeforeLinkWaitProc);
					//m_BeforeLinkWaitProc->addPerson( m_pFollow);
					m_pFollow->setState(LeaveServer);			
					spFollowerTerminalBehavior->getProcessor()->beginService(m_pFollow,p_time);
					m_bFollowInHolding = false;
				}
				else
				{
					TerminalMobElementBehavior* spLeaderBehavior = m_pLeader->getTerminalBehavior();
					Point leaderPt;
					m_pLeader->getTerminalPoint(leaderPt);
					m_pFollow->setTerminalDestination(leaderPt);
					////m_pFollow->getProcessor()->removePerson(m_pFollow);
	//				m_pFollow->getProcessor()->makeAvailable( m_pFollow, p_time, false );
					//// TRACE("\r\n%s",m_pFollow->getProcessor()->getID()->GetIDString());
					GreetEvent *event = NULL;
					if(m_pFollow->getType().GetTypeIndex()==0) //follow is  passenger
						event = new GreetEvent((Passenger *)m_pFollow,(PaxVisitor *)m_pLeader, p_time + m_pFollow->moveTime());
					else
						event = new GreetEvent((Passenger *)m_pLeader,(PaxVisitor *)m_pFollow, p_time + m_pFollow->moveTime());
					event->addEvent();
					ClearTag();
				}
			}

		}
		else //leader no arrival 
		{
			m_pFollow->setState(WaitInHoldingArea);
		}
	}
}

bool ConvergePair::SetPoint(int _id,Person * _pPerson)
{
	if(_id == m_iLeadIndex)
	{
		m_pLeader = _pPerson;
		return true;
	}
	else if (_id == m_iFollowIndex) 
	{
		m_pFollow = _pPerson;
		return true;
	}
	else return false;
}

void ConvergePair::ClearTag()
{
	m_bLeadArrival = false;
	m_bFollowArrival = false;
	m_bFollowInHolding = false;

	m_bLeaderWaiting = false;
    m_bFollowWaiting = false;
}

bool ConvergePair::getWaitTag(int _index)
{
	if(_index == m_iLeadIndex)
		return m_bLeaderWaiting;
	else if(_index == m_iFollowIndex)
		return m_bFollowWaiting;	
	return 0; //error.
}
/************************************************************************
FUNCTION:Set waiting leader or follower flag
IN		:_index,the person need to set flag
		_value,true,waiting;false,not arrival
RETURN:
                                                                      
/************************************************************************/
void ConvergePair::setWaitTag(int _index,bool _value)
{
	if(_index == m_iLeadIndex)
		m_bLeaderWaiting =_value;
	else if(_index == m_iFollowIndex)
	    m_bFollowWaiting =_value;
}
void ConvergePair::exchangeLeadFollow()
{
    int iTemp;
	bool bTemp;
	Person * pTemp;

	iTemp = m_iLeadIndex;              //index
	m_iLeadIndex = m_iFollowIndex;
	m_iFollowIndex = iTemp;

	pTemp = m_pLeader;                // point
	m_pLeader = m_pFollow;
	m_pFollow = pTemp;

	bTemp = m_bLeadArrival;            //arrival tag
	m_bLeadArrival = m_bFollowArrival;
	m_bFollowArrival = bTemp;

	bTemp = m_bLeaderWaiting;           //waiting tag
	m_bLeaderWaiting = m_bFollowWaiting;
	m_bFollowWaiting = bTemp;
}