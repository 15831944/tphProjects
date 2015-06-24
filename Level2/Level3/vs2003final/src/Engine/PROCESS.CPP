#include "stdafx.h"
#include "process.h"
#include "FIXEDQ.h"
#include "NFIXEDQ.h"
#include <Results/baselog.h>
#include <Results/ProcLog.h>
#include <Common/ProbabilityDistribution.h>
#include <inputs\prochrch.h>
#include <inputs\srvctime.h>
#include <inputs\paxflow.h>
#include <inputs\assigndb.h>
#include <inputs\miscproc.h>	
#include <inputs\ProcessorDistributionWithPipe.h>	
#include <inputs\MobileElemConstraintDatabase.h>	
#include <inputs\SubFlow.h>
#include <inputs\SubFlowList.h>
#include "monitor.h"
#include "anserveproclist.h"
#include <inputs\allcarschedule.h>
#include <InputAirside\ALTObject.h>
#include <InputAirside\InputAirside.h>
#include "BridgeConnector.h"
#include "DependentProcessorList.h"
#include "terminal.h"
#include "inputs\pipegraph.h"
#include "inputs\in_term.h"
#include "inputs\pipe.h"
#include "inputs\PipeDataSet.h"
#include <math.h>
#include <algorithm>
#include <limits>
#include "TerminalMobElementBehavior.h"
#include "Pax.h"
#include "RetailProcessor.h"
#include "ARCportEngine.h"


Processor::Processor ()
{
    index = -1;
	available = isOpen = TRUE;
    isClosing = FALSE;
    
    assignmentSchedule = NULL;
    service = NULL;
	m_pTransition = m_pTransitionGateDetail = m_pStationTransition = NULL;

	m_lQueueCapacity=0;

	m_lTerminateTime = -1;

	m_nCyclicFreq = 10;

    m_pQueue = NULL;
	gateList = NULL;
    initMiscData (false, TRUE, 0, NULL);
	
	m_pTerm = NULL; 
	m_pEngine = NULL;
	
	m_bIsTLOSInterestProc = false;
	m_currentServicePaxType.makeNone();

	m_bEmergentFlag = false;	

	m_lReason = -1;
	
	m_bDynamicCreatedProc = false;

	m_fltServiceTimeModificationFactor=0.0f;// >=0.0 and <=1.0  [4/28/2004]
	
	m_nGroupSerTimeModSum = 100;
	m_nGroupSerTimeModIndex = 0;
	m_bWaitInQueueToOpen =true;

	m_bBackup = false;
	m_bNewCreatedTLOSProcFlag = false;

	m_pDependentProcList = new DependentProcessorList(this);
	m_RecycleFreq.clear();
	m_vGateFlowPairProcs.clear();
	m_vReclaimFlowPairProcs.clear();
}

Processor::Processor( const Processor& _anotherPorc )
{
	
		m_pQueue = NULL;

		if ( _anotherPorc.m_pQueue )
		{
			if( _anotherPorc.m_pQueue->isFixed() == 'Y' )
				m_pQueue = new FixedQueue( *(_anotherPorc.m_pQueue->corner()) );
			else
				m_pQueue = new NonFixedQueue(*( _anotherPorc.m_pQueue->corner()) );
			
			*m_pQueue= *(_anotherPorc.m_pQueue);
		}


		gateList = NULL;		

		if( _anotherPorc.gateList )
		{
			gateList = new GroupIndex();
			*gateList = * ( _anotherPorc.gateList );
		}

		
		m_location=_anotherPorc.m_location;
		In_Constr=_anotherPorc.In_Constr;
		index=_anotherPorc.index;
		name=_anotherPorc.name;
		Out_Constr=_anotherPorc.Out_Constr;
		visitorFlag=_anotherPorc.visitorFlag;
		available=_anotherPorc.available;
		isOpen=_anotherPorc.isOpen;
		isClosing=_anotherPorc.isClosing;
		assignmentSchedule=_anotherPorc.assignmentSchedule;
		service=_anotherPorc.service;
		m_pTransition=_anotherPorc.m_pTransition;
		m_pTransitionGateDetail = _anotherPorc.m_pTransitionGateDetail;
		m_pStationTransition=_anotherPorc.m_pStationTransition;
		gateCount=_anotherPorc.gateCount;//error.
		m_pTerm=_anotherPorc.m_pTerm;
		m_pEngine = m_pEngine;
		logEntry=_anotherPorc.logEntry;
		m_bDisallowGroup=_anotherPorc.m_bDisallowGroup;
		m_lQueueCapacity=_anotherPorc.m_lQueueCapacity;
		m_nCyclicFreq=_anotherPorc.m_nCyclicFreq;
		m_bIsTLOSInterestProc = _anotherPorc.m_bIsTLOSInterestProc;
		m_currentServicePaxType = _anotherPorc.m_currentServicePaxType;	
		m_bEmergentFlag = _anotherPorc.m_bEmergentFlag;
		m_vSourceProcs = _anotherPorc.m_vSourceProcs;
		m_lReason = _anotherPorc.m_lReason;
		m_direction = _anotherPorc.m_direction;
		m_bDynamicCreatedProc =_anotherPorc.m_bDynamicCreatedProc;
		
		//copy linkage destination processor.
		m_vLinkedDestProcs= _anotherPorc.m_vLinkedDestProcs;
		
		m_ProcessorCapacity = _anotherPorc.m_ProcessorCapacity;
		m_ProcessorCapacityInSim = _anotherPorc.m_ProcessorCapacityInSim;
		m_RecycleFreq = _anotherPorc.m_RecycleFreq;
		m_vGateFlowPairProcs = _anotherPorc.m_vGateFlowPairProcs;
		m_vReclaimFlowPairProcs = _anotherPorc.m_vReclaimFlowPairProcs;

		if( _anotherPorc.m_pDependentProcList )
		{
			m_pDependentProcList = new DependentProcessorList(this);
			*m_pDependentProcList = *(_anotherPorc.m_pDependentProcList);
		}
}


Processor& Processor::operator = ( const Processor& _anotherPorc)
{
	if( this != &_anotherPorc )
	{
		
		if (m_pQueue)
			delete m_pQueue;
		m_pQueue = NULL;

		if ( _anotherPorc.m_pQueue )
		{
			if( _anotherPorc.m_pQueue->isFixed() == 'Y' )
				m_pQueue = new FixedQueue( *(_anotherPorc.m_pQueue->corner()) );
			else
				m_pQueue = new NonFixedQueue(*( _anotherPorc.m_pQueue->corner()) );
			
			*m_pQueue= *(_anotherPorc.m_pQueue);
		}

		if (gateList)
			delete gateList;
		gateList = NULL;		

		if( _anotherPorc.gateList )
		{
			gateList = new GroupIndex();
			*gateList = * ( _anotherPorc.gateList );
		}

		
		m_location=_anotherPorc.m_location;
		In_Constr=_anotherPorc.In_Constr;
		index=_anotherPorc.index;
		name=_anotherPorc.name;
		Out_Constr=_anotherPorc.Out_Constr;
		visitorFlag=_anotherPorc.visitorFlag;
		available=_anotherPorc.available;
		isOpen=_anotherPorc.isOpen;
		isClosing=_anotherPorc.isClosing;
		assignmentSchedule=_anotherPorc.assignmentSchedule;
		service=_anotherPorc.service;
		m_pTransition=_anotherPorc.m_pTransition;
		m_pTransitionGateDetail = _anotherPorc.m_pTransitionGateDetail;
		m_pStationTransition=_anotherPorc.m_pStationTransition;		
		gateCount=_anotherPorc.gateCount;//error, the count is not equal to gateList's count.
		m_pTerm=_anotherPorc.m_pTerm;
		m_pEngine = _anotherPorc.m_pEngine;
		logEntry=_anotherPorc.logEntry;
		m_bDisallowGroup=_anotherPorc.m_bDisallowGroup;
		m_lQueueCapacity=_anotherPorc.m_lQueueCapacity;
		m_nCyclicFreq=_anotherPorc.m_nCyclicFreq;
		m_bIsTLOSInterestProc = _anotherPorc.m_bIsTLOSInterestProc;
		m_currentServicePaxType = _anotherPorc.m_currentServicePaxType;	
		m_bEmergentFlag = _anotherPorc.m_bEmergentFlag;
		m_vSourceProcs = _anotherPorc.m_vSourceProcs;
		m_lReason = _anotherPorc.m_lReason;		
		m_direction = _anotherPorc.m_direction;	
		m_bDynamicCreatedProc =_anotherPorc.m_bDynamicCreatedProc;

		//copy linkage destination processor.
		m_vLinkedDestProcs= _anotherPorc.m_vLinkedDestProcs;
		
		m_ProcessorCapacity = _anotherPorc.m_ProcessorCapacity;
		m_ProcessorCapacityInSim = _anotherPorc.m_ProcessorCapacityInSim;

		m_vGateFlowPairProcs = _anotherPorc.m_vGateFlowPairProcs;
		m_vReclaimFlowPairProcs = _anotherPorc.m_vReclaimFlowPairProcs;

		if(m_pDependentProcList)
			delete m_pDependentProcList;
		m_pDependentProcList = new DependentProcessorList(this);

		if( _anotherPorc.m_pDependentProcList )
		{
			m_pDependentProcList = new DependentProcessorList(this);
			*m_pDependentProcList = *(_anotherPorc.m_pDependentProcList);
		}
		m_RecycleFreq = _anotherPorc.m_RecycleFreq;

	}
	return *this;
}
Processor::~Processor()
{
    if (m_pQueue)
        delete m_pQueue;
    m_pQueue = NULL;

    if (gateList)
        delete[] gateList;
    gateList = NULL;
	DeleteMyTransition( &m_pTransition);
	//clear linked destination processors.
	clearLinkedDestProcs();

	if(m_pDependentProcList)
		delete m_pDependentProcList;
	m_pDependentProcList = NULL;
	m_RecycleFreq.clear();
	m_vGateFlowPairProcs.clear();
	m_vReclaimFlowPairProcs.clear();
}

 
void Processor::init (const ProcessorID& Name)
{
    name = Name;
    logEntry.setIDs (Name.getID());
}



void Processor::initServiceLocation (int pathCount, const Point *pointList)
{
    if (pathCount != 1)
        throw new StringError ("Processor must have 1 service m_location");

    m_location.init (pathCount, pointList);
	UpdateMinMax();

	synFlag();
}
void Processor::initEntryPoint(std::vector<int>& points) 
   {
     if(m_pQueue != NULL )
	 {
	   try{
           FixedQueue* queue = static_cast<FixedQueue*>(m_pQueue) ;
		   for(int i = 0 ;i <(int)points.size() ; i++)
		   {
			   queue->AddEntryPoint(points[i]) ;
		   }
	   }catch(std::bad_cast)
	   {
		   return ;
	   }  
	 }
   }
void Processor::ResetEntryPoint()
	{
         if(this->m_pQueue != NULL && this->queueIsFixed())
		 {
			 FixedQueue* queue = NULL ;
			 try{
				 queue = static_cast<FixedQueue*>(this->m_pQueue) ;
				 queue->ResetEntryState() ;
			 }catch(std::bad_cast)
			 {
				 return ;
			 }  

		 }
	}
//Queue is cleaned  before initializing.
void Processor::initQueue (int fixed, int pathCount, const Point *pointList)
{
  	logEntry.setQueue (pathCount > 0);
	if (m_pQueue)
		delete m_pQueue;
	m_pQueue = NULL;

    if( pointList== NULL || pathCount <= 0 )
		return;

    // for sequential queues only, test which end of m_pQueue is the head
    Path tempPath;
    tempPath.init (pathCount, pointList);
    Point pnt (m_location.getPoint(0));
    if (pnt.distance (pointList[0]) > pnt.distance (pointList[pathCount-1]))
        tempPath.invertList();


    if (fixed)
        m_pQueue = new FixedQueue (tempPath);
    else
        m_pQueue = new NonFixedQueue (tempPath);
}


//Initializes visitorFlag if the processor will serve a visitor. Gate information is assigned.
void Processor::initMiscData ( bool _bDisallowGroup, int visitors, int count, const GroupIndex *gates)
{
	m_bDisallowGroup = _bDisallowGroup;
    visitorFlag = visitors;
    gateCount = count;
	m_RecycleFreq.clear();

	if( gateList )
	{
		delete[] gateList;
		gateList = NULL;
	}

    if (gateCount)
    {
        gateList = new GroupIndex[gateCount];
        memcpy (gateList, gates, (gateCount * sizeof (GroupIndex)));
    }
    else
	{
		if( gateList )
			delete []gateList;
		gateList = NULL;
	}

	//user clear current processor's capacity prop define in behavior, need clear data
//	m_ProcessorCapacity.Clear();
}

//Stores miscellanous data. The initMiscData() and initSpecificMisc() are called.
void Processor::initMisc (const MiscData *miscData)
{
    if (!miscData)
        return;

	//TRACE("\n%s", name.GetIDString());

	m_lQueueCapacity=miscData->GetQueueCapacity();//TODO
	CapacityAttributes* pAttributes = miscData->GetCapacityAttributes();
	m_ProcessorCapacity.Initialization(pAttributes);

	m_lTerminateTime = miscData->GetTerminateTime();

	m_nCyclicFreq = miscData->GetCyclicFreq();
	
	m_nGroupSerTimeModIndex = miscData->GetGroupSerTimeModIndex();

	m_nGroupSerTimeModSum = miscData->GetGroupSerTimeModSum();

	m_bDisallowGroup = miscData->GetDisallowGroupFlag();
    
	visitorFlag = miscData->allowsVisitors();

	m_bWaitInQueueToOpen = miscData->GetWaitInQueue();

    ProcessorIDList *idList = miscData->getGateList();

	if( MiscProcessorIDList* pMiscIDList = (MiscProcessorIDList*)idList  )
	{
		GroupIndex gates[32];
		
		assert( GetTerminal() );
		
		// put all the possible index into gate[]
		for (int i = 0; i < idList->getCount(); i++)
		{
			const MiscProcessorIDWithOne2OneFlag* pIDWithFlag = (const MiscProcessorIDWithOne2OneFlag*) pMiscIDList->getID( i ) ;
			if( pIDWithFlag )
			{
				
				if( pIDWithFlag->getOne2OneFlag() )//is one to one 
				{
					ProcessorArray tempArray;
					GroupIndex groupIdx = GetTerminal()->procList->getGroupIndex( *idList->getID( i ) );
					///					// TRACE("\n%s\n", idList->getID( i )->GetIDString() );
					ASSERT( groupIdx.start >=0 );
					for( int j=groupIdx.start; j<=groupIdx.end; ++j )
					{
						//						// TRACE("\n%s\n", GetTerminal()->procList->getProcessor( j )->getID()->GetIDString() );
						tempArray.addItem( GetTerminal()->procList->getProcessor( j ) );
					}
					//					// TRACE("\n%s\n", name.GetIDString() );
					//get 1:to:1
					Processor* pProcOneToOne = CFlowChannel::GetOutChannelProc( *this, tempArray, idList->getID( i )->idLength());
					if( pProcOneToOne )
					{
						//						// TRACE("\n%s\n", pProcOneToOne->getID()->GetIDString() );
						gates[i] = GetTerminal()->procList->getGroupIndex ( *pProcOneToOne->getID());
					}
				}
				else
				{
					gates[i] = GetTerminal()->procList->getGroupIndex (*(idList->getID (i)));
				}
				initMiscData ( m_bDisallowGroup, visitorFlag, idList->getCount(), gates);	
				//				int nLinkCount=miscData->getLinkedDestList()->getCount();
				//				for(int nLink=0;nLink<nLinkCount;nLink++)
				//				{
				//					pProcsID=miscData->getLinkedDestList()->getItem(nLink);
				//					GroupIndex group=
				//					m_vLinkedDestProcs.push_back(group);
			}
		}
		
	}
	//////////////////////////////////////////////////////////////////////////

	appendLinkedDestProcs(miscData->getLinkedDestList());

	initDependentProcList(miscData->getUseDependentProcFlag(), miscData->getDependentProcList());

	//for bridge all standid init
	if (getProcessorType() == BridgeConnectorProc)
	{
		std::vector<int> vAirports;
		InputAirside::GetAirportList(GetTerminal()->m_nProjID,vAirports);
		int airportID = -1;
		if(vAirports.size()>0) 
			airportID = vAirports.at(0);

		if (vAirports.size() == 0)
		{
			return;
		}

		ALTObjectIDList tempList;
		ALTObject::GetObjectNameList(ALT_STAND,airportID,tempList);
		((BridgeConnector*)this)->SetAllStandList(tempList);
	}

    initSpecificMisc (miscData);
}

void Processor::appendLinkedDestProcs(const MiscProcessorIDList* _pLinkedDestIdList)
{
	if (_pLinkedDestIdList==NULL) return;

	const MiscProcessorIDList* pMiscLinkedDestIDList = _pLinkedDestIdList;

	GroupIndex pLinkedDestGroupIndex;
	
	assert( GetTerminal() );		
	// put all the possible index into gate[]
	for (int i = 0; i < pMiscLinkedDestIDList->getCount(); i++)
	{
		const MiscProcessorIDWithOne2OneFlag* pIDWithFlag = (const MiscProcessorIDWithOne2OneFlag*) pMiscLinkedDestIDList->getID( i ) ;
		if( pIDWithFlag )
		{
			
			if( pIDWithFlag->getOne2OneFlag() )//is one to one 
			{
				ProcessorArray tempArray;
				GroupIndex groupIdx = GetTerminal()->procList->getGroupIndex( *pMiscLinkedDestIDList->getID( i ) );
				///					// TRACE("\n%s\n", idList->getID( i )->GetIDString() );
				ASSERT( groupIdx.start >=0 );
				for( int j=groupIdx.start; j<=groupIdx.end; ++j )
				{
					//						// TRACE("\n%s\n", GetTerminal()->procList->getProcessor( j )->getID()->GetIDString() );
					tempArray.addItem( GetTerminal()->procList->getProcessor( j ) );
				}
				//					// TRACE("\n%s\n", name.GetIDString() );
				//get 1:to:1
				Processor* pProcOneToOne = CFlowChannel::GetOutChannelProc( *this, tempArray, pMiscLinkedDestIDList->getID( i )->idLength());
				if( pProcOneToOne )
				{
					//						// TRACE("\n%s\n", pProcOneToOne->getID()->GetIDString() );
					//pLinkedDestGroupIndex=new GroupIndex;
					pLinkedDestGroupIndex = GetTerminal()->procList->getGroupIndex ( *pProcOneToOne->getID());
					m_vLinkedDestProcs.push_back(pLinkedDestGroupIndex);
				}
			}
			else
			{
				//pLinkedDestGroupIndex=new GroupIndex;
				pLinkedDestGroupIndex = GetTerminal()->procList->getGroupIndex (*(pMiscLinkedDestIDList->getID (i)));
				m_vLinkedDestProcs.push_back(pLinkedDestGroupIndex);
			}
		}
		
	}	
}
void Processor::setServiceTimeLink (const ServiceTimeDatabase& database)
{
    service = (ProcessorEntry *)database.getEntryPoint (name);
}

bool Processor::CheckIfOnTheSameFloor(int _iCurFloor)
{
	return _iCurFloor == getServicePoint( 0 ).getZ() / SCALE_FACTOR;
}
void Processor::setTransitionLink (const PassengerFlowDatabase& database, const PassengerFlowDatabase& _gateDetailFlowDB, const PassengerFlowDatabase& _stationPaxFlowDB,const CSubFlowList& pSubFlowList)
{
    //m_pTransition = (ProcessorEntry *)database.getEntryPoint (name);
	std::vector<ProcessorDataElement*> vEntryData;

	database.getEntryPoint(name, vEntryData);

	InitTransition( &m_pTransition, vEntryData,database,pSubFlowList);
	m_pTransitionGateDetail = (ProcessorEntry*)_gateDetailFlowDB.getEntryPoint( name );
    m_pStationTransition = (ProcessorEntry *)_stationPaxFlowDB.getEntryPoint (name);
}
void Processor::DeleteMyTransition(ProcessorEntry** _ppTransition)
{
	if(_ppTransition==NULL || (*_ppTransition)==NULL || m_bDynamicCreatedProc==true)return;
	ProcessorEntry* pTransition=* _ppTransition;
	CMobileElemConstraintDatabase* pTransitDb=pTransition->getDatabase();
	for(int i=0;i<pTransitDb->getCount();i++)
	{
		pTransitDb->getItem(i)->DeleteValue();
	}
	delete pTransition;
	*_ppTransition=NULL;
}
void Processor::InitTransition(ProcessorEntry** _ppTransition, std::vector<ProcessorDataElement*>& _vData,const PassengerFlowDatabase& database,const CSubFlowList& pSubFlowList)
{
	DeleteMyTransition(_ppTransition);
	if(_vData.size()>0)
	{
		* _ppTransition=new ProcessorEntry(name,"");
		CMobileElemConstraintDatabase* pTransitDb = (* _ppTransition)->getDatabase();
		CMobileElemConstraintDatabase* pIterDB=NULL;
		std::vector<ProcessorDataElement*>::reverse_iterator iter;
		ConstraintEntry* pNewConstraintEntry=NULL;
		CProcessorDistributionWithPipe* pProcDistribution=NULL;
		for(iter=_vData.rbegin();iter!=_vData.rend();iter++)
		{
			pIterDB=static_cast<ProcessorEntry*>(*iter)->getDatabase();
			for(int i=0;i<pIterDB->getCount();i++)
			{
				pNewConstraintEntry=new ConstraintEntry;
				pProcDistribution=new CProcessorDistributionWithPipe(*(const CProcessorDistributionWithPipe*)pIterDB->getItem(i)->getValue());

				//pProcDistribution->UntieSubFlows(database,pSubFlowList);
				pProcDistribution->InitRules();

				pNewConstraintEntry->initialize( new CMobileElemConstraint( *(const CMobileElemConstraint*)(pIterDB->getItem(i)->getConstraint()) ), pProcDistribution);
				pTransitDb->addEntry( pNewConstraintEntry,true);
			}
		}
	}
}


void Processor::setAssignmentLink (const ProcAssignDatabase& database)
{
	CString str=name.GetIDString();
    ProcAssignEntry *entry = (ProcAssignEntry *)database.getEntryPoint (name);
    assignmentSchedule = (entry)? entry->getSchedule(): NULL;


//	int nIdx = database.findEntry(*getID());
//	if (nIdx != INT_MAX) 
//	{
//		assignmentSchedule = database.getDatabase( nIdx );
//	}
//	else
//	{
//		assignmentSchedule = NULL;
//	}
//	assert( nIdx != INT_MAX );
	
	//////////////////////////////////////////////////////////////////////////
//#ifdef _DEBUG
//	const CMultiMobConstraint *multiConst, *tConst;
//    ProcAssignment *origAssign, *newAssign;
//    int constCount, count = assignmentSchedule->getCount();
//	
//    for (int i = 0; i < count; i++)
//    {
//        origAssign = assignmentSchedule->getItem (i);
//        multiConst = origAssign->getAssignment();
//        constCount = multiConst->getCount();
//        for (int j = 0; j < constCount; j++)
//        {             
//			ofsstream open_close("d:\\setAssignment.log", stdios::app);
//			CString strProc = getID()->GetIDString();
//			//char szBuf[512]; matt
//			char szBuf[2560];
//			(*(multiConst->getConstraint (j))).printConstraint( szBuf );
//			char szTime[64]={"00"};
//			//	curTime.printTime( szTime );
//			//// TRACE(" open processor: %s for Pax Type: %s at time: %s\r\n", strProc, szBuf, szTime );
//			CString strMsg;
//			strMsg.Format(" open processor: %s for Pax Type: %s at time: %s\r\n", strProc, szBuf, szTime  );
//			strcpy( szBuf, strMsg );
//			open_close<< szBuf;	          
//        }
//    }
//#endif
	//////////////////////////////////////////////////////////////////////////	
}

void Processor::addPerson (Person *aPerson)
{
//	TRACE("\naddPerson: Processor:%s, PersonID: %d",this->getID()->GetIDString(), 
//		aPerson->getID());
    occupants.addItem (aPerson);
}
 
//If the person is in m_pQueue, remove him from the m_pQueue.
void Processor::removePerson (const Person *aPerson)
{
//    int index = occupants.findElement (aPerson->getID());
    int index = occupants.findItem (aPerson);

//	// TRACE( "REMOVE PERSON %d\n", index );
    if (index != INT_MAX)
        occupants.removeItem (index);

    if (m_pQueue)
        m_pQueue->removePerson (aPerson);
}

// Remove all Persons from occupants list;
int Processor::clearPerson( const ElapsedTime& _time )
{
	//occupants.clear();
	Person *aPerson;
    int stranded = occupants.getCount();
	
    while (occupants.getCount() != 0)
    {
        aPerson = (Person *)occupants.getItem(0);
        occupants.removeItem (0);
		
		if(!getEngine()->m_simBobileelemList.IsAlive(aPerson))
			continue;

//#define DEBUG		
#ifdef DEBUG
		char str[128];
		name.printID (str);
		//char str1[128];
		CString sBuf;
		
		aPerson->getType().screenPrint( sBuf,0,128 );
		stdcerr << "\nStranded: " << sBuf.GetBuffer(0) << " at " << str;
#endif //DEBUG
		if (aPerson)
			aPerson->flushLog (_time);
//        delete aPerson;
    }

    return stranded;
}


ProcessorDistribution *Processor::getNextDestinations(const CMobileElemConstraint &p_type, int _nInGateDetailMode )
{

	if( _nInGateDetailMode == NORMAL_MODE )
	{
		if( !m_pTransition )
			return NULL;
		return (ProcessorDistribution *)m_pTransition->hierarchyLookup (p_type);
	}

    if( !m_pTransitionGateDetail )
		return NULL;

    return (ProcessorDistribution *)m_pTransitionGateDetail->hierarchyLookup (p_type);
}



// check if there is any destination start from this processor
bool Processor::HasDestination( const CMobileElemConstraint& _type, int _nInGateDetailMode ) const
{
	if( _nInGateDetailMode == NORMAL_MODE )
	{
		if( !m_pTransition )
			return false;
//		// TRACE( "%s\n", this->getID()->GetIDString() );
		if( m_pTransition->hierarchyLookup( _type ) )
			return true;
		else
			return false;
	}

    if( !m_pTransitionGateDetail )
		return false;

	if( m_pTransitionGateDetail->hierarchyLookup( _type ) )
		return true;
	return false;
}



ProcessorDistribution *Processor::getNextStation(const CMobileElemConstraint& _type)
{
    if( !m_pStationTransition )
        return NULL;
    return (ProcessorDistribution *)m_pStationTransition->hierarchyLookup( _type );
}
 
//It scans gateList for person's gate.
// Returns true if gate index found in gate list, false otherwise.
int Processor::leadsToGate (int index) const
{
    if (index == -1)
        return TRUE;

    if (!gateList)
        return TRUE;

    for (int i = 0; i < gateCount; i++)
        if (index >= gateList[i].start && index <= gateList[i].end)
            return TRUE;

    return FALSE;
}

//It scans gateList for person's gate.
// Returns true if gate index found in gate list, false otherwise.
bool Processor::AttachToGate (int index) const
{
    if (index == -1)
        return false;

    if (!gateList)
        return false;

    for (int i = 0; i < gateCount; i++)
        if (index >= gateList[i].start && index <= gateList[i].end)
            return true;

    return false;
}
long Processor::getServiceTime(const CMobileElemConstraint &type,const ElapsedTime& curTime )const
{
	long srvTime = 0l;
	if (!service)
		return 0l;
	
	const ProbabilityDistribution* prob = service->hierarchyLookupEx (type, curTime );
	if (!prob)
		return 0l;
	srvTime = (long)prob->getRandomValue();
	//return (srvTime < 0l)? 0l: srvTime;
	if(srvTime<0l) srvTime = 0l;
	return srvTime;
}
long Processor::getServiceTime  (Person* person, const ElapsedTime& curTime ) const
{
//	const CMobileElemConstraint &type = person->getType();
    long srvTime = 0l;
    long serviceTime = 0l;
     
	std::vector< long > vlServiceTime ;
	float fTotalTime=0.0;
	person->regetAttatchedNopax();
	int nGroupSize=person->GetActiveGroupSize();	
   // vlServiceTime.resize(nGroupSize);
	for(int i=0 ; i<nGroupSize; i++)
	{
		//long srvTime = 0l;
		if (!service)
			srvTime= 0l;
		else
		{
			const ProbabilityDistribution* prob = service->hierarchyLookupEx (person->getType(), curTime );
			if (!prob)
				srvTime= 0l;
			else
				srvTime = (long)prob->getRandomValue();
		}
		if(srvTime<0l) srvTime = 0l;
		
		//srvTime = getServiceTime(person->getType(),curTime);
		vlServiceTime.push_back(srvTime);
	}
	ASSERT(nGroupSize);
	std::sort(vlServiceTime.begin(),vlServiceTime.end());

	if(m_nGroupSerTimeModIndex < 0 ||m_nGroupSerTimeModIndex>4)
		(int)m_nGroupSerTimeModIndex = 0;
	if(m_nGroupSerTimeModSum < 0 || m_nGroupSerTimeModSum >100 )
		(int)m_nGroupSerTimeModSum = 100;

	int n;
	switch ( m_nGroupSerTimeModIndex ) 
	{
	case 0://Sum
		for( n=0; n<nGroupSize; n++)
		{
			fTotalTime = fTotalTime + vlServiceTime[n];
		}
		fTotalTime = (float)(fTotalTime * (m_nGroupSerTimeModSum /100.0)) ;
		break;
	case 1://Average
		for( n=0; n<nGroupSize; n++)
		{
			fTotalTime = fTotalTime + vlServiceTime[n];
		}
		fTotalTime =fTotalTime/ nGroupSize;
		break;
	case 2: //Minimum
		fTotalTime =  (float)vlServiceTime[0];
		break;
	case 3://Maximum
		fTotalTime = (float)vlServiceTime[nGroupSize-1];
		break;
	case 4://Random for group
		fTotalTime = (float)getServiceTime(person->getType(),curTime);
		break;
	default:
		for( n=0; n<nGroupSize; n++)
		{
			fTotalTime = fTotalTime + vlServiceTime[n];
		}
		fTotalTime = fTotalTime * (float)( m_nGroupSerTimeModSum / 100);
		break;		
	}
	
   serviceTime = (long)(fTotalTime+0.5);
   return serviceTime;

}


void Processor::getNextState (int& state,Person* _pPerson) const
{
	assert( _pPerson );

    if (state == FreeMoving && In_Constr.getCount())
        state = MoveAlongInConstraint;
    else if (state == LeaveServer && Out_Constr.getCount())
        state = MoveAlongOutConstraint;
    else if (state == LeaveServer)
        state = FreeMoving;
    else if (m_pQueue != NULL)
		state = MoveToQueue;
    else
        state = ArriveAtServer;
}

//It sets Person's next destination based on state & processor data
void Processor::getNextLocation (Person *aPerson)
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

    switch (aPerson->getState())
    {
        case MoveAlongInConstraint:
            aPerson->setTerminalDestination(In_Constr.getPoint(0));
			break;
        case MoveToQueue:
            m_pQueue->getTailPoint (aPerson);
            break;
        case ArriveAtServer:
            aPerson->setTerminalDestination ( AcquireServiceLocation( aPerson ) );
			if(spTerminalBehavior->getProcessor()->getProcessorType()==HoldAreaProc || 
				spTerminalBehavior->getProcessor()->getProcessorType() == LineProc || 
				spTerminalBehavior->getProcessor()->getProcessorType() == BaggageProc )//matt
			{
				if(spTerminalBehavior->getNextHoldAreaPoTag())
				{
					spTerminalBehavior->setDestination (spTerminalBehavior->getNextHoldAreaPoint());
					spTerminalBehavior->setNextHoldAreaPoTag(false);
				}
			}
            break;
    }
}

//It calculates service time and records start of service to log.
// determination of service time is based on the Person's type.
void Processor::beginService (Person *aPerson, ElapsedTime curTime)
{
	aPerson->regetAttatchedNopax();
    // determine service time, based on the Person's type
    ElapsedTime serviceTime;

	/*
	std::vector< long > vlServiceTime ;
		float fTotalTime=0.0;
		int nGroupSize=aPerson->getGroupSize();	
	   // vlServiceTime.resize(nGroupSize);
		for(int i=0 ; i<nGroupSize; i++)
		{
		/ *
			long srvTime = 0l;
				if (!service)
					srvTime= 0l;
				else{
				const ProbabilityDistribution* prob = service->hierarchyLookupEx ( aPerson->getType(), curTime );
				if (!prob)
				   srvTime= 0l;
				else
					srvTime = prob->getRandomValue();
				}
			    long iTemp =(srvTime < 0l)? 0l: srvTime;* /
		
	
			//long iTemp = getServiceTime(aPerson->getType(), curTime);
			vlServiceTime.push_back(getServiceTime(aPerson->getType(),curTime));
		}	
		ASSERT(nGroupSize);
		std::sort(vlServiceTime.begin(),vlServiceTime.end());
		//check m_nGroupSerTimeModIndex and m_nGroupSerTimeModSum , set them default data when they is invalidation
		if( m_nGroupSerTimeModIndex<0 || m_nGroupSerTimeModIndex >4 ) m_nGroupSerTimeModIndex =0;
		if( m_nGroupSerTimeModSum < 0 || m_nGroupSerTimeModSum > 100) m_nGroupSerTimeModSum = 100;
		int n;
		switch ( m_nGroupSerTimeModIndex ) 
		{
		case 0://Sum
			for( n=0; n<nGroupSize; n++)
			{
				fTotalTime = fTotalTime + vlServiceTime[n];
			}
			fTotalTime = fTotalTime * ( m_nGroupSerTimeModSum / 100.0);
			break;
		case 1://Average
			for( n=0; n<nGroupSize; n++)
			{
				fTotalTime = fTotalTime + vlServiceTime[n];
			}
			fTotalTime =fTotalTime/ nGroupSize;
			break;
		case 2: //Minimum
			fTotalTime =  vlServiceTime[0];
			break;
		case 3://Maximum
			fTotalTime = vlServiceTime[nGroupSize-1];
			break;
		case 4://Random for group
			fTotalTime = getServiceTime(aPerson->getType(),curTime);
			break;
		default:
			for( n=0; n<nGroupSize; n++)
			{
				fTotalTime = fTotalTime + vlServiceTime[n];
			}
			fTotalTime = fTotalTime * ( m_nGroupSerTimeModSum / 100.0);
			break;		
		}
		
	   serviceTime = (long)(fTotalTime+0.5);*/
	serviceTime = getServiceTime(aPerson,curTime);
   aPerson->generateEvent (curTime+serviceTime,false);
   writeLogEvent (aPerson, curTime, BeginService);
}

 
void Processor::makeAvailable (const Person *aPerson, ElapsedTime availTime, bool _bNoDelay )
{
   writeLogEvent ( aPerson,availTime, CompleteService);
   available = TRUE;
    if (!isOpen && !isClosing)
        return;

    // accept the next waiting Person, if any
    if (m_pQueue)
    {
        if (m_pQueue->hasWait())
        {
			if( _bNoDelay )
	            m_pQueue->releaseNext ( availTime );
			else
				m_pQueue->releaseNext ( availTime + ElapsedTime(DELAY) );
            if (m_pQueue->getGroupCount() == 1)
                balanceQueues (availTime);
        }
        else
		{
			balanceQueues (availTime);
			if( isClosing && m_pQueue->getTotalQueueLength() <= 0 && occupants.IsEmpty())
			{
				if (isClosing)
		        {
		            isClosing = FALSE;
					isOpen = FALSE;
		            writeLogEvent (NULL, availTime, CloseForService);
		        }
			}
			
		}		
    }

    else // close processor if isClosing == TRUE && m_pQueue empty
        if (isClosing && occupants.IsEmpty())
        {
            isClosing = FALSE;
			isOpen = FALSE;
            writeLogEvent (NULL, availTime, CloseForService);
        }

	WakeupWaitingPaxInPrevProc( availTime );
	WakeupWaitingPaxInDependentProc( availTime );

	noticeSourceConveyor( availTime );

}

// wake up the passenger in previous processor who was stuck because the exceed the capacity in this processor
void Processor::WakeupWaitingPaxInPrevProc(  const ElapsedTime& _time )
{
	//wake up the pax who wait for me.
	Processor* pStayProcs=NULL;
	Person* pPerson=NULL;

	std::vector<WaitingPair>::iterator iter=m_vWaitingPax.begin();
	while(iter!=m_vWaitingPax.end())	
	{
		pStayProcs=GetTerminal()->procList->getProcessor( iter->second);
		pPerson = (Person*)pStayProcs->findElement( iter->first);
		if(pPerson==NULL)
		{
			//the person have left waited processor.
			iter=m_vWaitingPax.erase( iter );
			continue;
		}
		else if( this->isVacant(pPerson) )
		{
			pPerson->generateEvent( _time ,false);
			iter=m_vWaitingPax.erase( iter );
			continue;
		}
		iter++;
	}
}

void Processor::WakeupWaitingPaxInDependentProc( const ElapsedTime& _time )
{
	//wake up the pax who wait for me.
	Processor* pStayProcs=NULL;
	Person* pPerson=NULL;

	std::vector<WaitingPair>::iterator iter=m_vWaitingPaxInDependentProc.begin();
	while(iter!=m_vWaitingPaxInDependentProc.end())	
	{
		pStayProcs=GetTerminal()->procList->getProcessor( iter->second);
		pPerson = (Person*)pStayProcs->findElement( iter->first);
		if(pPerson==NULL)
		{
			//the person have left waited processor.
			iter=m_vWaitingPaxInDependentProc.erase( iter );
			continue;
		}
		else if( this->isVacant(pPerson) )
		{
			pPerson->generateEvent( _time ,false);
			iter=m_vWaitingPaxInDependentProc.erase( iter );
			continue;
		}
		iter++;
	}
}



// If the processor is currently open, the processor is simply changing assignment.
// If the processor is closed, open the processor for service and set the correct flags.
void Processor::open ( ProcessorRoster* _pAss /*= NULL*/)
{
	ElapsedTime curTime;
	if( _pAss!= NULL )	// call by auto roster 
	{
		assert( m_bIsTLOSInterestProc );
		curTime = _pAss->getOpenTime();
		m_currentServicePaxType = *(_pAss->getAssignment()->getConstraint(0) );
	}
	
	else
	{
		curTime = assignmentSchedule->getCurrentTime();
		CString str = getID()->GetIDString();
//////////////////////////////////////////////////////////////////////////
//#ifdef _DEBUG
//
//		const CMultiMobConstraint *multiConst, *tConst;
//		ProcAssignment *origAssign, *newAssign;
//		int constCount, count = assignmentSchedule->getCount();
//		
//		for (int i = 0; i < count; i++)
//		{
//			origAssign = assignmentSchedule->getItem (i);
//			multiConst = origAssign->getAssignment();
//			constCount = multiConst->getCount();
//			for (int j = 0; j < constCount; j++)
//			{             
//				ofsstream open_close("d:\\open.log", stdios::app);
//				CString strProc = getID()->GetIDString();
//				//char szBuf[512]; matt
//				char szBuf[2560];
//				(*(multiConst->getConstraint (j))).printConstraint( szBuf );
//				char szTime[64]={"00"};
//				//	curTime.printTime( szTime );
//				//// TRACE(" open processor: %s for Pax Type: %s at time: %s\r\n", strProc, szBuf, szTime );
//				CString strMsg;
//				strMsg.Format(" open processor: %s for Pax Type: %s at time: %s\r\n", strProc, szBuf, szTime  );
//				strcpy( szBuf, strMsg );
//				open_close<< szBuf;	          
//			}
//		}
//#endif		
//////////////////////////////////////////////////////////////////////////

		ProcessorRoster* pAss = assignmentSchedule->getProcRoster();

		// add events' assign to m_***PassType
		switch( pAss->getAssRelation() )
		{
		case Inclusive:
			addConstraintToPaxTypeList( pAss->getAssignment(), m_InclusivePassType );
			break;

		case Exclusive:
			addConstraintToPaxTypeList( pAss->getAssignment(), m_ExclusivePassType );
			break;

		case Not:
			addConstraintToPaxTypeList( pAss->getAssignment(), m_NotPassType );
			break;

		default:
			break;
		}	
	}
	//if(this->index == 77)
	//{
		TRACE("\r\n Processor%s    ,Open: %d, %s \r\n",
			getID()->GetIDString(),
			curTime.getPrecisely(),
			curTime.printTime());
	//}

    // if the processor is currently open, the
    // processor is simply changing assignment
    if (isOpen)
    {
		isClosing = FALSE;

        if (m_pQueue != NULL)
            balanceQueues (curTime);

		//add by bird 2003/9/30
		if (available)
            makeAvailable (NULL, curTime, false);
    }

    // if the processor is closed, open the processor
    // for service and set the correct flags
    else
    {
        writeLogEvent (NULL, curTime, OpenForService);

        isOpen = TRUE;
		isClosing = FALSE;

        if (m_pQueue != NULL)
            balanceQueues (curTime);

        // do not make the processor available if there are passengers
        // still in service from the previous open state
        if (available)
            makeAvailable (NULL, curTime, false);
	}
}

//The isOpen sets to TRUE; the isClosing sets to FALSE.
//It calls makeAvailable() if the processor is available.
void Processor::openStation (ElapsedTime p_time)
{
    writeLogEvent (NULL, p_time, OpenForService);
    isOpen = TRUE;
    isClosing = FALSE;

    // do not make the processor available if there are passengers
    // still in service from the previous open state
    if (available)
        makeAvailable (NULL, p_time, false );
}


void Processor::close ( ProcessorRoster* _pAss /*= NULL*/ )
{
	ElapsedTime curTime;


	if( _pAss!= NULL )	// call by auto roster 
	{
		assert( m_bIsTLOSInterestProc );
		assert( m_currentServicePaxType == *(_pAss->getAssignment()->getConstraint(0)) || m_currentServicePaxType.isNone() );
		curTime = _pAss->getCloseTime();
		m_currentServicePaxType.makeNone();
	}
	else
	{
		curTime = assignmentSchedule->getCurrentTime();
		ProcessorRoster* pAss = assignmentSchedule->getProcRoster();

		// remove events' assign from m_***PassType
		switch( pAss->getAssRelation() ) 
		{
		case Inclusive:
			removeConstraintFromPaxTypeList( pAss->getAssignment(), m_InclusivePassType );
			break;

		case Exclusive:
			removeConstraintFromPaxTypeList( pAss->getAssignment(), m_ExclusivePassType );
			break;

		case Not:
			removeConstraintFromPaxTypeList( pAss->getAssignment(), m_NotPassType );
			break;

		default:
			break;
		}
	}
	//if(this->index == 77)
	//{
		TRACE("\r\n Processor%s    ,Close: %d, %s \r\n",
			getID()->GetIDString(),
			curTime.getPrecisely(),
			curTime.printTime());
	//}
	//// TRACE("TIME:%s\tProcessor%s\r\n",curTime.printTime(),  );
	ProcessorRoster* _pCloseAss = _pAss == NULL ? assignmentSchedule->getProcRoster() : _pAss;
	setPersonFlagWhenClose( _pCloseAss );
	
	if( (_pAss == NULL && m_InclusivePassType.getCount() == 0 && m_ExclusivePassType.getCount() == 0 && m_NotPassType.getCount() == 0)
	  ||(_pAss != NULL) ) 								// no pax can pass, close the processor
	{
		// close the processor and set the flags to stop new passengers
	    isOpen = FALSE;

	    // if people are being served or waiting for service, service them
	    // before closing
	    // otherwise, allow passengers to line up for next opening
		//if (getTotalQueueLength() || !available)
		CMobileElemConstraint closeType = *_pCloseAss->getAssignment()->getConstraint(0);
		if(IsWantSettingClosing(closeType))
	    {
	        isClosing = TRUE;
//			// TRACE("\n%s\n", name.GetIDString() );
			char timestr[32];
			curTime.printTime( timestr );
//			// TRACE("\n%s\n", timestr );
	        writeLogEvent (NULL, curTime, StartToClose);
	    }
	    else
	    {
	        notifyAssignmentChange (curTime);
	        writeLogEvent (NULL, curTime, CloseForService);
	    }
	}
	else if(_pAss == NULL && m_InclusivePassType.getCount() > 0 )
	{
		if(_pCloseAss)
		{
			CMobileElemConstraint closeType = *_pCloseAss->getAssignment()->getConstraint(0);
		    if(IsWantSettingClosing(closeType))
			{
				isClosing = TRUE;
			    //			// TRACE("\n%s\n", name.GetIDString() );
			    char timestr[32];
			    curTime.printTime( timestr );
			    //			// TRACE("\n%s\n", timestr );
			    writeLogEvent (NULL, curTime, StartToClose);
			}
		}
	}
}


//It closes the processor and sets the flags to stop new passengers
//The isOpen sets to FALSE; the isClosing sets to FALSE.
void Processor::closeStation (ElapsedTime p_time)
{
    if (!isOpen && !isClosing)
        return;

    // close the processor and set the flags to stop new passengers
    isOpen = FALSE;
    isClosing = FALSE;
    writeLogEvent (NULL, p_time, CloseForService);
}

 
int Processor::canServe (const CMobileElemConstraint& paxType, CanServeProcList* _canServeProcList) const
{
	if( m_bIsTLOSInterestProc )		// auto roster
	{
		if( m_currentServicePaxType.fits( paxType ) )
		{
			if( _canServeProcList )
				_canServeProcList->addCanServeProcToList(  const_cast<Processor*>( this ), Inclusive, NULL );
			return true;
		}
		else
			return false;
	}
	else							
	{
		if (!assignmentSchedule)
			return TRUE;
		
		return doMatch( paxType, _canServeProcList, true );
	}
}

// This method indicates to other members of receiver's group that this processor is available.
void Processor::balanceQueues (ElapsedTime curTime)
{
    GroupIndex group;
    ProcessorID groupName;
	Processor *aProc;

    if (m_pQueue && m_pQueue->isFixed() == 'Y')
        return;

    groupName = name;
    if (groupName.getSuperGroup())
    {
		assert( GetTerminal() );
        group = GetTerminal()->procList->getGroupIndex (groupName);

        for (int i = group.start; i <= group.end; i++)
        {
            if (i == index)
                continue;

            aProc = GetTerminal()->procList->getProcessor(i);
            if (aProc->m_pQueue && aProc->m_pQueue->isFixed() == 'N' &&
                aProc->getFloor() == getFloor())
                    aProc->m_pQueue->balanceQueue (this, curTime);
        }
		
		if( GetTerminal()->procList->GetDynamicCreatedProcs() )
		{
			group = GetTerminal()->procList->GetDynamicCreatedProcs()->getGroupIndex (groupName);
			if( group.start >=0 )
			{
				for (int i = group.start; i <= group.end; i++)
				{
					if (i == index)
						continue;
					
					aProc = GetTerminal()->procList->GetDynamicCreatedProcs()->getProcessor(i);
					if (aProc->m_pQueue && aProc->m_pQueue->isFixed() == 'N' &&
						aProc->getFloor() == getFloor())
						aProc->m_pQueue->balanceQueue (this, curTime);
				}
			}
		}
	}
}

void Processor::notifyAssignmentChange (ElapsedTime time) const
{
}


MobileElement *Processor::findElement (long p_ndx) const
{
    MobileElement temp (p_ndx, 0);
	temp.M_DELIDX = 1;
    int index = occupants.findItem (&temp);
    if (index != INT_MAX)
        return occupants.getItem (index);
    else
        return fineVisitor(p_ndx);
}


MobileElement *Processor::fineVisitor(long p_ndx) const
{
	for (int i = 0; i < occupants.getCount(); i++)
	{
		Person* pPerson = (Person*)occupants.getItem(i);
		if (pPerson->getType().GetTypeIndex() == 0)
		{
			Passenger* pPax = (Passenger*)pPerson;
			if (pPax->IsMyVisitor(p_ndx))
			{
				return pPerson;
			}
		}
	}
	return NULL;
}

//The "available" is set FALSE.
void Processor::leaveQueue (const Person *aPerson, ElapsedTime p_time, bool _bNoDelay )
{
    available = FALSE;
    writeLogEvent (aPerson, p_time, DecreaseQueue);
	if( _bNoDelay )
		m_pQueue->leaveQueue ( aPerson, p_time );
	else
		m_pQueue->leaveQueue ( aPerson, p_time + ElapsedTime( DELAY ) );
}

//When a person arrives at a wait point in the m_pQueue, it updates info and check to see if further movement is possible.
void Processor::notifyQueueAdvance (Person *aPerson, ElapsedTime p_time, bool _bStation )
{
    m_pQueue->arriveAtWaitPoint (aPerson, p_time);
    processPerson (aPerson, p_time, _bStation );
}

// It is called by passenger arriving at m_pQueue.
// If the person is already in logical m_pQueue, call process Person to test if the passenger can advance and be served.
void Processor::arriveAtQueue (Person *aPerson, ElapsedTime p_time, bool _bStation)
{
	//////////////////////////////////////////////////////////////////////////
	// when pax arrival at processor's queue,  
	// ask monitor if he should generate TLOSEvent and then generate or not.  
	if( m_bIsTLOSInterestProc )
	{
		GetTerminal()->m_pTLOSMonitor->PersonArrivalAtProcQueue( this, p_time , aPerson->getType() );
	}
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	spTerminalBehavior->setEntryQueueTime( p_time );

	
    int prevState = aPerson->getState();
	//TRACE("\n %s \n", this->getID()->GetIDString() );
    m_pQueue->arriveAtQueue (aPerson, p_time);
	//// TRACE ("\n%s\n",p_time.printTime() );
    if (prevState == WaitInQueue)
    {
       writeLogEvent (aPerson, p_time, IncreaseQueue);
       processPerson (aPerson, p_time, _bStation );
    }
	
	
}

void Processor::moveThroughQueue (Person *p_person, ElapsedTime p_time)
{
    ((SequentialQueue *)m_pQueue)->relayAdvance (p_person, p_time);
}

void Processor::processPerson (Person *aPerson, ElapsedTime p_time, bool _bStation )
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

    if (spTerminalBehavior->getCurrentIndex() == HEAD && isAvailable() && (isOpen || isClosing))
    {
		if( !_bStation && IsDestProcExceedCapacity(aPerson, p_time) )
		{
			aPerson->setState( WaitInQueue );
			aPerson->generateEvent( p_time + ElapsedTime(2l),true );
			return;
		}

        aPerson->setState (LeaveQueue);
		if( _bStation )
		    aPerson->generateEvent ( p_time,false );
		else
	        aPerson->generateEvent ( p_time + ElapsedTime(DELAY) ,false);
    }    
}

void Processor::addToQueue (Person *aPerson, ElapsedTime)
{
    m_pQueue->addToQueue (aPerson);
}

int Processor::getQueueLength (void) const
{
    if (m_pQueue)
        return !available + m_pQueue->getQueueLength();
    else
        return !available + occupants.getCount();
}

int Processor::getTotalQueueLength (void) const
{
    if (m_pQueue)
        return !available + m_pQueue->getTotalQueueLength();
    else
        return !available + occupants.getCount();
}

int Processor::getTotalGroupCount (void) const
{
    if (m_pQueue)
        return !available + m_pQueue->getGroupCount();
    else
        return !available + occupants.getCount();
}

int Processor::clearQueue (ElapsedTime curTime)
{
    if (!m_pQueue)
        return 0;

    //int count = m_pQueue->wipe (curTime);
	int count = m_pQueue->GetWaitList()->getCount();
	m_pQueue->GetWaitList()->clear();

    return count;
}

//Prints person approaching to processor to screen.
//Always returns true.
int Processor::echoApproach (void) const
{
//    if (m_pQueue != NULL)
//        m_pQueue->printApproaching();
    return TRUE;
}

//Writes info about processor to processor file.
int Processor::writeProcessor (ArctermFile& procFile) const
{
    char str[256];
    int i;

	
    name.printID (str);
    procFile.writeField (str);
    procFile.writeField (getProcessorName());
	procFile.writeInt((int)m_bBackup);

    int serviceFlag = serviceLocationLength();
    int inConstFlag = inConstraintLength();
    int queueFlag = (m_pQueue != NULL)?m_pQueue->cornerCount():FALSE;
    int outConstFlag = outConstraintLength();
	FixedQueue* queue = NULL ;
	int entrypoint  = 0 ;
	if(m_pQueue != NULL && m_pQueue->isFixed() == 'Y')
	{
		try{
			queue = static_cast<FixedQueue*>(m_pQueue) ;
			entrypoint= (m_pQueue != NULL)?queue->GetEntryPoints()->size():FALSE;
		}catch(std::bad_cast)
		{

		}
	}
    for (i = 0; serviceFlag || inConstFlag || queueFlag || outConstFlag; i++)
    {
        if (i) procFile.writeBlankField();

        if (serviceFlag)
        {
            procFile.writePoint (getServicePoint(i));
            serviceFlag = i+1 < serviceLocationLength();
        }
        else
            procFile.writeBlankFields (3);

        if (inConstFlag)
        {
            procFile.writePoint (inConstraint(i));
            inConstFlag = i+1 < inConstraintLength();
        }
        else
            procFile.writeBlankFields (3);

        if (queueFlag)
        {
            procFile.writePoint (m_pQueue->corner(m_pQueue->cornerCount()-i-1));
            queueFlag = i+1 < m_pQueue->cornerCount();
        }
        else
            procFile.writeBlankFields (3);

        if (outConstFlag)
        {
            procFile.writePoint (outConstraint(i));
            outConstFlag = i+1 < outConstraintLength();
        }
        else
            procFile.writeBlankFields (3);
        
        if (i == 0 && m_pQueue != NULL)
            procFile.writeChar (m_pQueue->isFixed());

		if( i == 0 )
			writeSpecialField(procFile );
         if(i != 0 && entrypoint)
		 {
            if( i <=entrypoint && m_pQueue->isFixed() == 'Y')
			{
                 procFile.writeInt(queue->GetEntryPoints()->at(i-1));
				
			}else
                procFile.writeBlankFields (3);
		 }
        procFile.writeLine();
    }
	
	//write integrated processor info
//	writeSpecialProcessor(procFile);
    return TRUE;
}

//Write miscellanous data to file.
int Processor::writeMiscData (ArctermFile& miscDataFile) const
{
    char str[256];

    name.printID (str);
    miscDataFile.writeField (str);

    miscDataFile.writeChar ((visitorFlag)?'Y':'N');

    if (gateCount)
    {
		assert( GetTerminal() );
        GetTerminal()->procList->getProcessorID (gateList[0], str);
        for (int i = 1; i < gateCount; i++)
        {
            strcat (str, ";");
            GetTerminal()->procList->getProcessorID (gateList[i], str + strlen(str));
        }
    }
    else
        str[0] = '\0';

    miscDataFile.writeField (str);
    writeExtraFields (miscDataFile);
    miscDataFile.writeLine();
    return TRUE;
}

//Creates events of person and his group member.
//Adds all events to processor log entry.
void Processor::writeLogEvent (const Person* aPerson, ElapsedTime time, int eventType , long _lLoad /*= -1 */)
{
    static int testID = -1;
    if (testID == index)
        testID += 0;

    ProcEventStruct event;

    event.time = (long)time;
    event.type = (unsigned char) eventType;
    event.element = (aPerson)? aPerson->getID(): -1;
	event.lLoad = _lLoad;
	event.lReason = m_lReason;
    logEntry.addEvent (event);
/*
#ifdef _DEBUG
	FILE *fw = fopen("c:\\debugtime.txt", "a+");	
	fprintf(fw, "\n this=0x%x		personid=%d		time=%s		type=%d\n", 
		this, event.element, time.printTime(), event.type);
	for (int td = 1; aPerson != NULL && td < aPerson->GetActiveGroupSize(); td++)
	{
		fprintf(fw, "\n this=0x%x		personid=%d		time=%s		type=%d	--added.\n", 
			this, event.element + td , time.printTime(), event.type);
	}
	fclose(fw);
#endif
*/
    for (int i = 1; aPerson != NULL && i < aPerson->GetActiveGroupSize(); i++)
    {
        event.element++;
        logEntry.addEvent (event);
    }
}

//Writes out processor log entry for animation.
void Processor::writeLog (void)
{
    logEntry.setProcType (getProcessorType());
	logEntry.SetDisallowGroupFlag( m_bDisallowGroup );
	assert( GetTerminal() );
    GetTerminal()->procLog->addItem (logEntry);
}

void Processor::sortLog( void )
{	
	logEntry.sortProcEventLog();
}

//Updates the output data before clearing entry log.  
void Processor::flushLog (void)
{
    ProcEventStruct *log = NULL;
	assert( GetTerminal() );
	logEntry.SetEventLog( GetTerminal()->m_pProcEventLog );
    if (logEntry.getCurrentCount())
    {
        logEntry.getLog (log);
        logEntry.setEventList (log, logEntry.getCurrentCount());
		
		delete [] log;
		log = NULL;
    }
    GetTerminal()->procLog->updateItem (logEntry, index);
    logEntry.clearLog();
}

int Processor::queueLength(void) const
{
	if(m_pQueue != NULL)
		return m_pQueue->cornerCount();
	else
		return 0;
}

Point Processor::queuePoint(int index) const
{
	return m_pQueue->corner(index);
}

BOOL Processor::queueIsFixed(void) const
{
	if(m_pQueue != NULL)
		return (m_pQueue->isFixed() == 'Y');
	else
		return FALSE;
}


Path* Processor::queuePath()
{
	if(m_pQueue != NULL)
		return m_pQueue->corner();
	else
		return NULL;
}

//just change conveyor state now
void Processor::synFlag()
{

}

// check the destination processor which the person want to go is exceed capacity or not
bool Processor::IsDestProcExceedCapacity(Person* pPerson ,const ElapsedTime& _curTime)
{
	if( getProcessorType() == IntegratedStationProc )
		return false;
	
	bool bAllExceedCapacity=true;
	TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return false;
	}

	//// TRACE ("\n%s\n",name.GetIDString() );
	// now is moving in flow
	{
		ProcessorDistribution* pDestList = getNextDestinations( pPerson->getType(), spTerminalBehavior->GetInGateDetailMode() );
		// handle case where Processor has no ProcessorDistribution
		if (pDestList == NULL )
		{
			if(  NORMAL_MODE == spTerminalBehavior->GetInGateDetailMode() )
			{
				//throw new NoFlowError (pPerson);
				CString szMobType = pPerson->getPersonErrorMsg();
				CString strProcName = spTerminalBehavior->getProcessor() ? spTerminalBehavior->getProcessor()->getID()->GetIDString() : "";
				pPerson->kill(_curTime);
				MobileElementMovementEvent::RemoveAllMobileElementEvent( pPerson );
//				delete pPerson;
				
				throw new ARCFlowUndefinedError( szMobType, strProcName, "", _curTime.printTime() );				
			}

			else
				return false;
		} 
        

		int nCount = pDestList->GetDestCount();
		
		//// TRACE( "pDestList->getCount()= %d\n",nCount);
		assert(GetTerminal());

		for( int i=0;i<nCount && bAllExceedCapacity; ++i )
		{
			ProcessorID* pProcID = pDestList->getGroup( i );
			CString strProcID = pProcID->GetIDString();
			if( GetTerminal()->m_pSubFlowList->IfProcessUnitExist( strProcID) )	// is a process
			{
				bAllExceedCapacity=false;
				continue;
			}
			std::vector<CFlowItemEx>& flowItem = ((CProcessorDistributionWithPipe*)pDestList)->GetPipeVector();
			long lAvoidMaxQueue = -1;
			long lMaxWaitMins = -1;
			if( !flowItem.empty() )
			{
				lAvoidMaxQueue = flowItem[ i ].GetMaxQueueLength();
				lMaxWaitMins = flowItem[ i ].GetMaxWaitMins();

			}
			 
			GroupIndex destGroupIndex=GetTerminal()->procList->getGroupIndex( *pProcID );
			if(destGroupIndex.start>=0)
			{		
				for(int j=destGroupIndex.start;j<= destGroupIndex.end && bAllExceedCapacity; ++j)
				{
					Processor* pProc = GetTerminal()->procList->getProcessor(j);
	
					if( pProc->isVacant(pPerson))
					{
							
						//// TRACE ("\nprocessor id %s\n", GetTerminal()->procList->getProcessor(j)->getID()->GetIDString() );
						//// TRACE ("\nid = %s\n",this->getID()->GetIDString() );
						if( lAvoidMaxQueue > 0 || lMaxWaitMins > 0 )
						{	
							if( lAvoidMaxQueue > 0 )
							{
								if( pProc->GetOccupants()->getCount() <  lAvoidMaxQueue )// check max queue length
								{
									bAllExceedCapacity=false;
								}
							}	
							
							if( lMaxWaitMins > 0 )
							{
								if( ProcessorQueue* pProcQueue = pProc->GetProcessorQueue() )
								{
									if( pProcQueue->hasWait() )// check max wait time
									{
										MobileElementList* pWaitList = pProcQueue->GetWaitList();
										Person* pHeadPerson = (Person*)pWaitList->getItem( 0 );
										TerminalMobElementBehavior* spHeadTerminalBehavior = pHeadPerson->getTerminalBehavior();
										ElapsedTime timeEntryQueue = spHeadTerminalBehavior->getEntryQueueTime();
										if( (_curTime - timeEntryQueue ) >= ElapsedTime( lMaxWaitMins*60l ) )
										{
											bAllExceedCapacity=true;
										}
										else
										{
											bAllExceedCapacity=false;
										}
									}
									else
									{
										if( pProc->GetOccupants()->getCount() > 0  )
										{
											bAllExceedCapacity=true;
										}
										else
										{
											bAllExceedCapacity=false;
										}
										
									}
								}
								else
								{
									bAllExceedCapacity=false;
								}
							}
						}
						else
						{
							bAllExceedCapacity=false;
						}
						

					}
				}
			}		
			
		}
	}


	return bAllExceedCapacity;
}
Processor*  Processor::GetRandomDestProc( Person* _pPerson )
{

	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return NULL;
	}
	ProcessorDistribution* pDestList = getNextDestinations( _pPerson->getType(), spTerminalBehavior->GetInGateDetailMode() );
	// handle case where Processor has no ProcessorDistribution
	if (pDestList == NULL )
	{
		return NULL;
	} 

	int nCount = pDestList->getCount();	
	int iDest = random( nCount );
	ProcessorID* pProcID = pDestList->getGroup( iDest );
	GroupIndex destGroupIndex=GetTerminal()->procList->getGroupIndex( *pProcID );
	ASSERT( destGroupIndex.start >=0 );
	int iDestCount = destGroupIndex.end - destGroupIndex.start;
	if( iDestCount >0 )
	{
		return GetTerminal()->procList->getProcessor( destGroupIndex.start + random( iDestCount ) );
	}
	else
	{
		return GetTerminal()->procList->getProcessor( destGroupIndex.start );
	}
	

				
}

// check if processor can serve current person according to 
// processor's capacity attributes and current occupancy MobileElements
bool Processor::isVacant( Person* pPerson ) const
{
	ASSERT(pPerson != NULL);
	TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return false;
	}

	if(m_ProcessorCapacity.GetCount() < 1)
		return true;

	ProcessorOccupantsState procOccupied;

	//Get all  Occupant item info, passenger, follower, visitor, ...
	for (int i = 0; i < occupants.getCount(); i++)
	{
		Person* pCurrentPerson = ((Person*)occupants.getItem( i ));	
		TerminalMobElementBehavior* spCurrentTerminalBehavior = pCurrentPerson->getTerminalBehavior();
		if (spCurrentTerminalBehavior)
		{
			spCurrentTerminalBehavior->getFollowersAndVisitorsOccupancyInfo(procOccupied);
		}
	}

	ProcessorOccupantsState procWillOccupied;
	spTerminalBehavior->getFollowersAndVisitorsOccupancyInfo(procWillOccupied);

	return m_ProcessorCapacityInSim.IsVacant(pPerson, procOccupied, procWillOccupied);
}


int Processor::ReadProcessor(ArctermFile& procFile)
{
    ProcessorID procID;
//CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)AfxGetApp()->GetMainWndm_hGetDocument();	
	assert( GetTerminal() );
	procID.SetStrDict( GetTerminal()->inStrDict );
    if (!procID.readProcessorID (procFile))
        throw new StringError ("Invalid processor IDs, ProcessorList::getProcessor");

    char string[SMALL];
    procFile.getField (string, SMALL);
	
    this->init (procID);

	int nBackup;
	procFile.getInteger(nBackup);
	if (nBackup > 0)
		SetBackup(true);
	else
		SetBackup(false);
	
	Point aPoint;
    Point inConst[16], outConst[16];
	int index = 0 ;
	//m_pServicePoints = new 
	Point m_pServicePoints[MAX_POINTS];
	Point m_pQueue[MAX_POINTS];
   // m_pQueue = new Point[MAX_POINTS];
	int svcCount = 0, inCount = 0, queueCount = 0, outCount = 0;
	std::vector<int> entrypoint ;
    if (!procFile.getPoint (aPoint))
        throw new StringError ("All processors must have a service m_location");

	m_pServicePoints[svcCount++] = aPoint;

    if (procFile.getPoint (aPoint))
        inConst[inCount++] = aPoint;

    if (procFile.getPoint (aPoint))
        m_pQueue[queueCount++] = aPoint;

    if (procFile.getPoint (aPoint))
        outConst[outCount++] = aPoint;

    char fixedQueue = 0;
    if (queueCount)
        procFile.getChar (fixedQueue);
    // make sure point limits are not exceeded
    for (int i = 1; procFile.getLine() && !procFile.isNewEntryLine(); i++)
    {
        if (i + 1 >= MAX_POINTS)
        {
            procID.printID (string);
            throw new TwoStringError ("maximum point count exceeded by ", string);
        }

        // skip ID and Processor Type fields
        procFile.skipField (2);

        if (procFile.getPoint (aPoint))
            m_pServicePoints[svcCount++] = aPoint;

        if (procFile.getPoint (aPoint))
            inConst[inCount++] = aPoint;

        if (procFile.getPoint (aPoint))
            m_pQueue[queueCount++] = aPoint;

        if (procFile.getPoint (aPoint))
            outConst[outCount++] = aPoint;
		if(procFile.getVersion() >= 2.5 && fixedQueue=='Y')///// version changede  ,
		{
			if(procFile.getInteger(index))
            entrypoint.push_back(index) ;
		}
    }

    this->initInConstraint (inCount, inConst);
    this->initServiceLocation (svcCount, m_pServicePoints);
    this->initOutConstraint (outCount, outConst);
    this->initQueue (fixedQueue == 'Y', queueCount, m_pQueue);
	if(fixedQueue=='Y')
      this->initEntryPoint(entrypoint) ;
	return TRUE;
}


int Processor::GetCyclicFreq() const
{
	return m_nCyclicFreq;
}

void Processor::GetQueueTailPoint( Person* _pPerson )
{ 
	m_pQueue->getTailPoint( _pPerson ); 
}


// Clean all sim engine related data, for the new sim process
void Processor::CleanData()
{

	assert( occupants.getCount() == 0 );
	if( m_pQueue )
		assert( m_pQueue->getQueueLength() == 0 );

    available = TRUE;
	isOpen = TRUE;

	isClosing = FALSE;

	// clear **canPassType
	m_NotPassType.freeConstraints();
	m_ExclusivePassType.freeConstraints();
	m_InclusivePassType.freeConstraints();
}


// Get Entry Point of the processor,
// might be inconstraint, or service m_location.
Point Processor::GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime)
{
	if(getServicePoint( 0 ).getZ()/ SCALE_FACTOR !=_iCurFloor) 
	{
		throw new ARCFlowError (_strMobType, this->getID()->GetIDString(),"not the same Floor Problem! ",_strTime);
	    return getServicePoint( 0 );
	}
	//AfxMessageBox("the Same Floor Problem !");
	if( In_Constr.getCount() )
		return In_Constr.getPoint( 0 );

	if( m_pQueue != NULL )
		return	m_pQueue->corner( m_pQueue->cornerCount() - 1 );

	return getServicePoint( 0 );
}

// Get Exit Point of the processor,
// might be outconstraint, or service m_location.
Point Processor::GetExitPoint()
{
	if( Out_Constr.getCount() )
		return Out_Constr.getPoint( Out_Constr.getCount()-1 );
	
	return getServicePoint( 0 );
}

void Processor::SetTerminal( Terminal *_pInTerm )
{
	m_pTerm = _pInTerm;
	name.SetStrDict( _pInTerm->inStrDict ); 
}

void Processor::SetEngine( CARCportEngine* _pEngine )
{
	m_pEngine = _pEngine;  
	//name.SetStrDict( m_pEngine->getTerminal()->inStrDict ); 
	if (getProcessorType() == RetailProc)
	{
		RetailProcessor* pRetailProc = (RetailProcessor*)this;
		pRetailProc->InitShoppingRule();
		pRetailProc->InitShopStocking();
	}
}

CARCportEngine* Processor::getEngine()
{
	ASSERT(m_pEngine != NULL);
	return m_pEngine;
}

Terminal* Processor::GetTerminal()
{
	return m_pTerm;

	//ASSERT(m_pEngine != NULL);
	//return m_pEngine->getTerminal();
}

Terminal* Processor::GetTerminal() const
{
	return m_pTerm;
}
// base on the "occupants" which is count of group, return number of passengers.
// int Processor::GetTotalNumberPassengerInThisProcessor() const
// {
// 	int nGroupCount = occupants.getCount();
// 	int nPaxCount = 0;
// 	for( int i=0; i<nGroupCount; i++ )
// 	{
// 		Person* pPerson = (Person*)occupants.getItem( i );
// 		if( pPerson->IsHuman() )
// 		{
// 			int nGroupSize = pPerson->GetActiveGroupSize();
// 			nPaxCount += nGroupSize;		
// 		}
// 	}
// 	return nPaxCount;
// }

bool Processor::CanLeadTo( const CMobileElemConstraint& _paxType,  const ProcessorID& _destProcID)const
{
	return m_direction.CanLeadTo( _paxType, _destProcID ,GetTerminal());
}

bool Processor::CanLeadToReclaim(const CMobileElemConstraint& _paxType, const ProcessorID& _destProcID)const
{
	return m_reclaimDirection.CanLeadTo( _paxType, _destProcID ,GetTerminal());
}

int Processor::readSpecialField(ArctermFile& procFile) 
{ 
	if( getProcessorType() == PointProc )
	{
		char chFlag;
		procFile.getChar( chFlag );
		m_bEmergentFlag = (chFlag == 'Y' ? TRUE : FALSE) ;
	}

	return TRUE;
}

int Processor::writeSpecialField(ArctermFile& procFile) const 
{
	if( getProcessorType() == PointProc )
	{
		char chFlag = m_bEmergentFlag ? 'Y' : 'N';
		procFile.writeChar( chFlag );	
	}
	
	return TRUE;
}
void Processor::AddSourceProcs( std::vector<Processor*>& _AllNewSources )
{
	std::vector<Processor*>::iterator iter = _AllNewSources.begin();
	std::vector<Processor*>::iterator iterEnd = _AllNewSources.end();
	for(; iter!=iterEnd; ++iter )
	{
		if( std::find( m_vSourceProcs.begin(), m_vSourceProcs.end(), *iter ) == m_vSourceProcs.end() )
		{
			m_vSourceProcs.push_back( *iter );
		}
	}
}
/*
void Processor::AddDestProcs( std::vector<Processor*>& _AllNewDests )
{
	std::vector<Processor*>::iterator iter = _AllNewDests.begin();
	std::vector<Processor*>::iterator iterEnd = _AllNewDests.end();
	for(; iter!=iterEnd; ++iter )
	{
		if( std::find( m_vDestProcs.begin(), m_vDestProcs.end(), *iter ) == m_vDestProcs.end() )
		{
			m_vDestProcs.push_back( *iter );
		}
	}
}
*/
// when make available, wake up pre conveyor processor if necessary
bool Processor::noticeSourceConveyor( const ElapsedTime& time )
{
//	// TRACE("processor name: %s\r\n", this->getID()->GetIDString() );
	if( m_vSourceProcs.size() <= 0 )      
		return false;

	int iRandom = rand()%2 ;
	if( iRandom == 0 )
	{
		if( releasePusher(time) )
			return true;
		else
			return releaseSimpleConveyor( time );
	}
	else
	{ 
		if( releaseSimpleConveyor( time ) )
			return true;
		else
			return releasePusher( time );
	}
}

bool Processor::releasePusher( const ElapsedTime& time )
{
	static int iNoticeIndex = -1;
	iNoticeIndex = (iNoticeIndex + 1)%m_vSourceProcs.size();	
	for( UINT i=0; i< m_vSourceProcs.size(); i++ )
	{
		if( m_vSourceProcs[iNoticeIndex] && m_vSourceProcs[iNoticeIndex]->getProcessorType() == ConveyorProc )
		{
			Conveyor* pConveyor = (Conveyor*) m_vSourceProcs[iNoticeIndex];
			if( pConveyor->GetSubConveyorType() == PUSHER )
				if( pConveyor->destProcAvailable( time ) )
					return true;
		}
		
		iNoticeIndex = (iNoticeIndex+1) % m_vSourceProcs.size();
	}
	return false;
}


bool Processor::releaseSimpleConveyor( const ElapsedTime& time )
{
	int iNoticeIndex = -1;
	ElapsedTime earlyTime( 3*24*60*60l );
	for( UINT i=0; i< m_vSourceProcs.size(); i++ )
	{
		if( m_vSourceProcs[i] && m_vSourceProcs[i]->getProcessorType() == ConveyorProc )
		{
			Conveyor* pConveyor = (Conveyor*) m_vSourceProcs[i];
			if( pConveyor->GetSubConveyorType() != PUSHER )
			{
				if( pConveyor->ifNeedToNotice() )
				{
					ElapsedTime releasePersonTime = pConveyor->getReleasePersonTime();

						earlyTime = releasePersonTime;
						iNoticeIndex = i;
				}
			}		
		}
	}
	
	if( iNoticeIndex == -1 )	// no processor need to release
		return false;
	
	// release the most prior ( by time ) conveyor
	Conveyor* pReleaseConveyor = (Conveyor*) m_vSourceProcs[iNoticeIndex];
	assert( pReleaseConveyor );
	pReleaseConveyor->destProcAvailable( time );
	return true;
}

//init reource pool data
void Processor::initResourcePool( void )
{
	m_ResourcePoolMap.initMapData( GetTerminal(), *getID() );
}

// get best pool by pax type
CResourcePool* Processor::getBestResourcePoolByType( const CMobileElemConstraint& _mob,long& _lServiceTime  )
{
	return m_ResourcePoolMap.getBestMatch( _mob, _lServiceTime );
}

//check recycle frequency
bool Processor::CheckRecycleFrequency(Person* pPerson)
{
	std::map<int,int>::iterator iter = m_RecycleFreq.begin();
	iter = m_RecycleFreq.find(pPerson->getID());
	if(iter != m_RecycleFreq.end())
	{
		return (*iter).second >= m_nCyclicFreq;
	}
	return false;
}

//add recycle frequency
void Processor::AddRecycleFrequency(Person* pPerson)
{
	std::map<int,int>::iterator iter = m_RecycleFreq.begin();
	iter = m_RecycleFreq.find(pPerson->getID());
	if (iter != m_RecycleFreq.end())
	{
		(*iter).second++;
	}
	else
	{
		m_RecycleFreq.insert(std::make_pair(pPerson->getID(),1));
	}
}

//clear recycle frequency
void Processor::ClearRecycleFrequency()
{
	m_RecycleFreq.clear();
}

// notice processor release pax
bool Processor::noticeReleasePax( MobileElement* _mobElem, const ElapsedTime& _time )
{
	int _idx = occupants.Find( _mobElem );
	if( _idx == INT_MAX )		// the pax not in the processor
		return false;

	// let the pax leave the processor
	_mobElem->writeLogEntry( _time, false );
	_mobElem->setState(  LeaveServer );
	_mobElem->generateEvent( _time,false );
	return true;
}

void Processor::setPersonFlagWhenClose( ProcessorRoster* _pCloseAss )
{
	assert( _pCloseAss );
	if( m_pQueue == NULL )
		return;

	MobileElementList* _pWaitList = m_pQueue->GetWaitList();

	for( int i=0; i< _pWaitList->getCount(); i++ )
	{
		Person* _personInQueue = ( Person*) _pWaitList->getItem( i );
		TerminalMobElementBehavior* spTerminalBehavior = _personInQueue->getTerminalBehavior();
	
		for( int j=0; j < _pCloseAss->getAssignment()->getCount(); j++ )
		{
			const CMobileElemConstraint* p_CloseType = _pCloseAss->getAssignment()->getConstraint(j);
			if( p_CloseType->fits( _personInQueue->getType() )&& spTerminalBehavior)
				spTerminalBehavior->m_bCanServeWhenClosing = true;
		}
	}
}

void Processor::addConstraintToPaxTypeList( CMultiMobConstraint* _pSrcTypeList, CMultiMobConstraint& _destTypeList )
{
	
	for( int i=0; i< _pSrcTypeList->getCount(); i++ )
	{
		_destTypeList.addConstraint( *(_pSrcTypeList->getConstraint(i)) );
//#ifdef _DEBUG
//		//////////////////////////////////////////////////////////////////////////
//		ElapsedTime curTime = assignmentSchedule->getCurrentTime();
//		ofsstream open_close("d:\\open_close.log", stdios::app);
//		CString strProc = getID()->GetIDString();
//		//char szBuf[512]; matt
//		char szBuf[2560];
//		(*(_pSrcTypeList->getConstraint(i))).printConstraint( szBuf );
//		char szTime[64];
//		curTime.printTime( szTime );
//		//// TRACE(" open processor: %s for Pax Type: %s at time: %s\r\n", strProc, szBuf, szTime );
//		CString strMsg;
//		strMsg.Format(" open processor: %s for Pax Type: %s at time: %s\r\n", strProc, szBuf, szTime  );
//		strcpy( szBuf, strMsg );
//		open_close<< szBuf;
//		//////////////////////////////////////////////////////////////////////////		
//#endif
	}
}

void Processor::removeConstraintFromPaxTypeList( CMultiMobConstraint* _pSrcTypeList, CMultiMobConstraint& _destTypeList )
{
	for( int i=0; i< _pSrcTypeList->getCount(); i++ )
	{
		const CMobileElemConstraint* _Type = _pSrcTypeList->getConstraint(i);
		for( int j=0; j<_destTypeList.getCount(); j++ )
		{
			if( _Type->isEqual(_destTypeList.getConstraint(j)) )
			{	
				_destTypeList.deleteConst(j);
				break;
			}
		}
	}
}

// do match in m_NotPassType, m_ExclusivePassType, m_InclusivePassType, 
// and updata _canServeProcList if need
int Processor::doMatch( const CMobileElemConstraint& paxType, CanServeProcList* _canServeProcList, bool _bIgnoreStar ) const
{
	int i;
	// first match not_pass_type
	for( i =0; i< m_NotPassType.getCount(); i++ )
	{
		if( m_NotPassType.getConstraint(i)->fitex( paxType, _bIgnoreStar ) )
			return FALSE;
	}
	
	// then match exclusive_pax_type
	for( i=0; i<m_ExclusivePassType.getCount(); i++ )
	{
		if( m_ExclusivePassType.getConstraint(i)->fitex( paxType, _bIgnoreStar ))
		{
			if( _canServeProcList )
				_canServeProcList->addCanServeProcToList( const_cast<Processor*>( this ), Exclusive, m_ExclusivePassType.getConstraint(i) );

			return TRUE;
		}
	}

	// the match inclusive pax type
	
	if( m_InclusivePassType.isMatched( paxType, _bIgnoreStar ) )
	{
		if( _canServeProcList )
			_canServeProcList->addCanServeProcToList(  const_cast<Processor*>( this ), Inclusive, NULL );
		return TRUE;
	}
	
	//// then 
 //	if( m_NotPassType.getCount() >0 )
 //	{
 //		if( _canServeProcList )
 //			_canServeProcList->addCanServeProcToList(  const_cast<Processor*>( this ), Not, NULL );
 //		return TRUE;
 //	}

	return FALSE;
}

int Processor::FindLinkedDestinationProcs(const ProcessorID& _groupID, ProcessorArray& _destProcs, bool& _bLevel0Match) const
{
	if(m_vLinkedDestProcs.size()==0)return -1;

	const ProcessorID* pMyProcsID=getID();
	const ProcessorID* pLinkedProcsID=NULL;
	int nLinkCount=m_vLinkedDestProcs.size();
	int nOrigSize=_destProcs.getCount();

	int i=0;
	GroupIndex pLinkedIndex;
	std::vector<GroupIndex>::const_iterator iter;
	CString sLinkedID;

	TCHAR csCheckIDLevel0[128]="";
	TCHAR csLinkIDLevel0[128]="";
	_groupID.getNameAtLevel( csCheckIDLevel0, 0);

	for(iter=m_vLinkedDestProcs.begin();iter!=m_vLinkedDestProcs.end();iter++)
	{
		pLinkedIndex=(*iter);
		//ASSERT(pLinkedIndex!=NULL);
		for(i=pLinkedIndex.start;i<=pLinkedIndex.end;i++)
		{
			pLinkedProcsID=GetTerminal()->procList->getProcessor(i)->getID();
			sLinkedID=pLinkedProcsID->GetIDString();
			ASSERT(pLinkedProcsID!=NULL);
			if ( _groupID==*pLinkedProcsID)
			{
				_destProcs.addItem(GetTerminal()->procList->getProcessor(i));
			}
			else if(_bLevel0Match==false)
			{
				pLinkedProcsID->getNameAtLevel(csLinkIDLevel0, 0);
				if(_tcscmp(csLinkIDLevel0, csCheckIDLevel0)==0)
				{
					_bLevel0Match=true;
				}
			}
		}
	}
	return _destProcs.getCount()-nOrigSize;

}
void Processor::clearLinkedDestProcs()
{
	if(m_vLinkedDestProcs.size()>0)
	{
		std::vector<GroupIndex>::iterator iter;
		for(iter=m_vLinkedDestProcs.begin();iter!=m_vLinkedDestProcs.end();iter++)
		{
		//	delete (*iter);
		}
		m_vLinkedDestProcs.clear();

	}
	
}

//all the valid dest processors are laid in _destProcs.
//these dest processor include closed processor, exceed capacity porcessor, 
//do not include the processor which is forbidden by roster or linkage.
//please set _pInChannel as NULL if there isn't in 1:x:1 channel.
//even if destination processor is in 1:x:1 channel, but it isn't the end of 1:x:1, you should save _pInChannel for next time to use.
bool Processor::FindDestProcessors(const Terminal* _pTerminal, const Person* _person, 
								   const Processor* _curProc, const ProcessorDistribution* _pDistribution ,
								   const CFlowChannel* _pInChannel,const ElapsedTime& _curTime,
								   const CSinglePaxTypeFlow* _pProcessSingleFlow, 
								   const ProcessorID* _pProcessCurID, ProcessorArray& _destProcs, 
								   bool& bOneToOneReason, bool& bOneXOneReason)
{

	CProcessorDestinationList tempPair((InputTerminal*)_pTerminal);
	if(_pTerminal==NULL || _person==NULL || _pDistribution==NULL) return false;
	const ProcessorID* pNextGroupID = _pDistribution->GetCurDestID();

	TerminalMobElementBehavior* spTerminalBehavior = _person->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return false;
	}

	if(_pProcessSingleFlow)
	{
		//within process flow.
		ASSERT( _pProcessCurID!=NULL && _pProcessCurID->isBlank()!=TRUE);
		_pProcessSingleFlow->GetFlowPairAt( *_pProcessCurID, tempPair );
		pNextGroupID = tempPair.GetDestCount()>0? (&(tempPair.GetDestProcAt( 0 ).GetProcID())):NULL;
	}


	if(pNextGroupID==NULL)
	{
		pNextGroupID = _pDistribution->GetCurDestID();
		if (pNextGroupID == NULL)
		{
			return false;
		}

	}

	
	const ProcessorID* pToGate = _pTerminal->procList->getProcessor(TO_GATE_PROCESS_INDEX)->getID();
	const ProcessorID* pFromGate = _pTerminal->procList->getProcessor(FROM_GATE_PROCESS_INDEX)->getID();

	CString& sNextProcessors = pNextGroupID->GetIDString();
	const CMobileElemConstraint& personType=_person->getType();
	std::vector<ProcessorID> vRootProc;//used for process flow
	
	// check for PROCESS
	if( _pTerminal->m_pSubFlowList->IfProcessUnitExist( sNextProcessors) )
	{
		CSubFlow* pProcessUnitFlow = _pTerminal->m_pSubFlowList->GetProcessUnit( sNextProcessors );
		if( pProcessUnitFlow == NULL) 
			return false;

		Person* pPerson = (Person*)_person;
		Processor* pPorc = pProcessUnitFlow->GetNextProcessor(pPerson, _curTime, false);
		if(pPorc != NULL)
		{
			pNextGroupID = pPorc->getID();			// the head of PROCESS flow ,only one path
			sNextProcessors = pNextGroupID->GetIDString();
		}	
		else 
			return false;
	}

	
	bool bNeedCheckLeadToGate = spTerminalBehavior->NeedCheckLeadToGate();	// only check the gate after arrival gate for the turnaround passenger.
	if( *pToGate == *pNextGroupID || *pFromGate == *pNextGroupID || spTerminalBehavior->IfPersonInStationSystem() )
	{
		bNeedCheckLeadToGate = false;
	}
	//build destination array, consider linkage at the same time.
	ProcessorArray arrayNextDest;
	bool bLevel0Matched=false;
	//build destination by linkage.
	GroupIndex pIndex = _pTerminal->procList->getGroupIndex(*pNextGroupID);


	for(int k=pIndex.start;k<=pIndex.end;k++)
	{
	  Processor * pNextProcessor = _pTerminal->procList->getProcessor(k);
      _curProc->FindLinkedDestinationProcs( *(pNextProcessor->getID()), arrayNextDest, bLevel0Matched);
	  if(arrayNextDest.getCount()>0) 
	  {
		  _destProcs.addItem(arrayNextDest.getItem(0));
          arrayNextDest.clear();
	  }
	//_curProc->FindLinkedDestinationProcs( *pNextGroupID, arrayNextDest, bLevel0Matched);//linkage is first.
	}
	if(_destProcs.getCount()>0) 
	{
		arrayNextDest.clear();
		for(int ii=0;ii<_destProcs.getCount();ii++)
		 arrayNextDest.addItem(_destProcs.getItem(ii));
		_destProcs.clear();
	}


	if(arrayNextDest.getCount()==0)
	{
		//build destination if no linkage.
		//if(bLevel0Matched) linkNoTag = true;
		GroupIndex nextGroupRange = _pTerminal->procList->getGroupIndex( *pNextGroupID );
		if (nextGroupRange.start == nextGroupRange.end && 
			(nextGroupRange.start == END_PROCESSOR_INDEX 
			|| nextGroupRange.start == TO_GATE_PROCESS_INDEX
			|| nextGroupRange.start == FROM_GATE_PROCESS_INDEX
			|| nextGroupRange.start == BAGGAGE_DEVICE_PROCEOR_INDEX) )
		{
			_destProcs.addItem (_pTerminal->procList->getProcessor( nextGroupRange.start ));
			return true;
		}
		
		for(int i=nextGroupRange.start;i<=nextGroupRange.end;i++)
			arrayNextDest.addItem( _pTerminal->procList->getProcessor( i ));
	}
	
	//filter invalidate destination by 1:1 , roster
	if(_pDistribution->IsCurOneToOne() )
	{
		//fiter by 1:1
		Processor* pOne2OneProc = CFlowChannel::GetOutChannelProc( *_curProc, arrayNextDest, _pDistribution->GetCurDestIDLength());
		if(pOne2OneProc)
		{
			arrayNextDest.clear();
			arrayNextDest.addItem( pOne2OneProc );
		}
		else
		{
			arrayNextDest.clear();
			bOneToOneReason = true;
			//throw new ARCDestProcessorUnavailableError( _person->getPersonErrorMsg(), _curProc->getID()->GetIDString(), pNextGroupID->GetIDString()+ " can't correspond to "+ _curProc->getID()->GetIDString()+ " by 1:1", _curTime.printTime());
			return false;
		}
	}
	
	if(static_cast<const CProcessorDistributionWithPipe*>(_pDistribution)->GetOneXOneState()==ChannelEnd)
	{
		//filter by 1:x:1
		if(_pInChannel==NULL) return false;
		Processor* pOneXOneProc=_pInChannel->GetOutChannelProc( arrayNextDest, _pDistribution->GetCurDestIDLength());
		if(pOneXOneProc)
		{
			arrayNextDest.clear();
			arrayNextDest.addItem( pOneXOneProc);
		}
		else
		{
			arrayNextDest.clear();
			bOneXOneReason = true;
			//throw new ARCDestProcessorUnavailableError( _person->getPersonErrorMsg(), _curProc->getID()->GetIDString(), pNextGroupID->GetIDString()+ " can't correspond to "+ _curProc->getID()->GetIDString()+ " by 1:x:1", _curTime.printTime());
			return false;
		}
	}
	
	Processor* pTestPrcs=NULL;
	if( spTerminalBehavior->IfPersonInStationSystem() )
	{
		//filter by station system.
		for(int i=arrayNextDest.getCount()-1 ; i>=0;i--)
		{
			pTestPrcs=arrayNextDest.getItem( i );
			if( pTestPrcs->getProcessorType() !=IntegratedStationProc || !_pTerminal->m_pAllCarSchedule->IfHaveScheduleBetweenStations((IntegratedStation*)_curProc ,(IntegratedStation*)pTestPrcs ))
			{
				arrayNextDest.removeItem(i);
			}
		}
	}
	//filter by roster
	CanServeProcList canServerProcs;
	for(int i=0;i<arrayNextDest.getCount();i++)
	{
		pTestPrcs=arrayNextDest.getItem( i );
		pTestPrcs->doMatch( personType, &canServerProcs, true );
	}
	
	canServerProcs.getProcFromList( &_destProcs );

	return true;
}

bool Processor::FindDestProcessors(const Person* _person, 
								   const ProcessorDistribution* _pDistribution , 
								   const CFlowChannel* _pInChannel,
								   const ElapsedTime& _curTime,  
								   const CSinglePaxTypeFlow* _pProcessSingleFlow,
								   const ProcessorID* _pProcessCurID, 
								   ProcessorArray& _destProcs,
								   bool& bOneToOneReason, bool& bOneXOneReason)
{
	return Processor::FindDestProcessors(GetTerminal(), _person, this, _pDistribution,
										_pInChannel, _curTime, _pProcessSingleFlow,	
										_pProcessCurID, _destProcs, bOneToOneReason,
										bOneXOneReason);
}

// remove the processors which can not take more passengers from the dest processor list and add into different processor list for different 
// reasons ( closed, exceedcapacity, not open )
void Processor::PruneInvalidDestProcs(Terminal* pTerm, Person* pPerson,ProcessorArray* _pSourceDestProcs, ProcessorArray* _pClosedDestProcs/*=NULL*/, ProcessorArray* _pExceedCapacityProcs/*=NULL*/, ProcessorArray* _pOtherReasonProcs/*=NULL*/)
{
	ASSERT( _pSourceDestProcs !=NULL);
	//ProcessorArray _pLinkDestProcs;

	if(_pSourceDestProcs==NULL || _pSourceDestProcs->getCount()==0)return;
	
	bool bRemoveFlag=false;
	Processor* pTestProcs=NULL;
	for(int i=_pSourceDestProcs->getCount()-1; i>=0; i--)
	{
		bRemoveFlag=false;
		pTestProcs= _pSourceDestProcs->getItem(i);
		
		//test closed
		if(pTestProcs->isClosed())
		{
			bRemoveFlag=true;

			if(_pClosedDestProcs)
				_pClosedDestProcs->addItem( pTestProcs );
		}

		//test capacity
		if( pTestProcs->isVacant(pPerson)  == false)
		{
			bRemoveFlag = true;
			if( _pExceedCapacityProcs)
				_pExceedCapacityProcs->addItem( pTestProcs );
		}

		//test recycle frequency
		if(pTestProcs->CheckRecycleFrequency(pPerson) == true)
		{
			bRemoveFlag = true;
			if (_pExceedCapacityProcs)
				_pExceedCapacityProcs->addItem(pTestProcs);
		}

		if (pTestProcs->isQueueLengthVacant(pPerson) == false)
		{
			bRemoveFlag = true;
			if (_pExceedCapacityProcs)
				_pExceedCapacityProcs->addItem(pTestProcs);
			
		}
		//test other reason.
		if( pTestProcs->isOpened()==false /*||*/)
		{
			bRemoveFlag =true;
			if( _pOtherReasonProcs)
				_pOtherReasonProcs->addItem( pTestProcs );
		}

		//if destination processors have several Baggage devices.
		//pTestProcs->isVacant() test maybe passed, but should make sure the baggage device
		//is the item which has been assigned to current person.
		if( pTestProcs->getProcessorType() == BaggageProc 
			&& pPerson->getType().GetTypeIndex() == 2/*CHECKED bag*/)
		{
			int nProcIndex = pTestProcs->getIndex();
			ProcessorList *procList = pTerm->procList;
			int nAssignBagIndex = pPerson->getBagCarosel();
			if(nProcIndex != nAssignBagIndex)
			{
				bRemoveFlag=true;
				if( _pExceedCapacityProcs)
					_pExceedCapacityProcs->addItem( pTestProcs );
			}
		}

		//remove the invalid item.
		if(bRemoveFlag)
		{
			_pSourceDestProcs->removeItem( i );
		}
	}

	// dependent processors check(stop serve if all dependent processors are occupied)
	if(_pSourceDestProcs->getCount() > 0)
	{
		
		for(int i=_pSourceDestProcs->getCount()-1; i>=0; i--)
		{
			Processor* pDependentProc = _pSourceDestProcs->getItem(i);
			
			//TRACE( "-dependent processor = %s--\n", pDependentProc->getID()->GetIDString() );

			if(!pDependentProc->isDependentProcUsed())
				continue;

			if(pDependentProc->isAllDependentProcsOccupied( pPerson ))
			{
				Processor* pAnOverloadProc=NULL;
				WaitingPair waitPair;
				waitPair.first= pPerson->getID();
				TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
				if (spTerminalBehavior)
				{
					waitPair.second =spTerminalBehavior->getProcessor()->getIndex();
				}
				

				// register to dependent procs's
				pDependentProc->AddWaitingPairInDependentProcList( waitPair  );

				// add to waiting pair in prev procs
				_pExceedCapacityProcs->addItem( pDependentProc );

				_pSourceDestProcs->removeItem( i );
			}

		}

	}
}

// add pax id and processor idx to the dest processor.
void Processor::AddWaitingPair(const WaitingPair& _wait)
{
	m_vWaitingPax.push_back( _wait );
}

void Processor::AddWaitingPairInDependentProcList(const WaitingPair& _wait)
{
	m_pDependentProcList->AddWaitingPairInDependentProc( _wait );
}

void Processor::AddWaitingPairInDependentProc(const WaitingPair& _wait)
{
	for (std::vector<WaitingPair>::iterator iter = m_vWaitingPaxInDependentProc.begin();iter != m_vWaitingPaxInDependentProc.end();iter ++)
	{
		if((*iter).first == _wait.first  && (*iter).second == _wait.second)
			return;
	}
	m_vWaitingPaxInDependentProc.push_back( _wait );
}

//

void Processor::InitProcessorCapacityInSim()
{
	//clean up waiting mobile elements records. 
	m_vWaitingPax.clear();
	m_vWaitingPaxInDependentProc.clear();

	// init capacity management object.
	m_ProcessorCapacityInSim.Initialization(GetTerminal(), &m_ProcessorCapacity);
}



Processor* Processor::CreateNewProc()
{
	Processor* pProc = new Processor;
	return pProc;
}



Processor* Processor::CopyProcessor(const ProcessorID& _newPorcID, Terminal* _pTerm, const int _nFloor, const int _nFloorCount, const int _nDestFloorTotal)
{
	Processor* pNewProc = CreateNewProc();
	if (pNewProc == NULL)
		return NULL;

	pNewProc->init(_newPorcID);
	pNewProc->SetTerminal(_pTerm);

	if( !PrepareCopy(pNewProc, _nFloor, _nDestFloorTotal)
		|| !CopyServiceLocation(pNewProc, _nFloor, _nFloorCount)
		|| !CopyInConstraint(pNewProc)
		|| !CopyOutconstraint(pNewProc)
		|| !CopyQueue(pNewProc)
		|| !CopyOtherData(pNewProc) )
	{
		delete pNewProc;
		return NULL;
	}

	return pNewProc;
}

bool Processor::PrepareCopy(Processor* _pDestProc, const int _nFloor, const int _nDestFloorTotal)
{
	return true;
}

bool Processor::CopyServiceLocation(Processor* _pDestProc, const int _nFloor, const int _nFloorCount)
{
	//set properties
	int nFloor = getFloor() / (int)SCALE_FACTOR;
	int nFloorOffset = _nFloor - nFloor;
	int nFloorCount = _nFloorCount;

	int nPSLCount = serviceLocationPath()->getCount();

	if(nPSLCount> 0)
	{
		Point* pPointSL = new Point[nPSLCount];
		memcpy(pPointSL, serviceLocationPath()->getPointList(), sizeof(Point)*nPSLCount);
		for (int i = 0; i < nPSLCount; i++)
		{
			DistanceUnit zPos = pPointSL[i].getZ();
			zPos += nFloorOffset*SCALE_FACTOR;
			if ( zPos >= nFloorCount*SCALE_FACTOR || zPos < 0 )
			{
				MessageBox(NULL, _T("The Destination Model has not enough floors"), _T("Copy Error"), MB_OK | MB_ICONERROR);
				delete[] pPointSL;
				return false;
			}
			pPointSL[i].setZ( zPos );
		}
		_pDestProc->initServiceLocation(nPSLCount,pPointSL);
		delete[] pPointSL;
	}

	return true;
}

bool Processor::CopyInConstraint(Processor* _pDestProc)
{
	int nPSLCount = _pDestProc->serviceLocationPath()->getCount();
	ASSERT(nPSLCount > 0);
	DistanceUnit zPos = _pDestProc->serviceLocationPath()->getPoint(0).getZ();

	int nPICCount = inConstraint()->getCount();
	if( nPICCount> 0)
	{
		Point* pPointIC = new Point[nPICCount];
		memcpy(pPointIC, inConstraint()->getPointList(), sizeof(Point)*nPICCount);
		for(int i = 0; i < nPICCount; i++)
			pPointIC[i].setZ(zPos);

		_pDestProc->initInConstraint( nPICCount, pPointIC);
		delete[] pPointIC;
	}

	return true;
}

bool Processor::CopyOutconstraint(Processor* _pDestProc)
{
	int nPSLCount = _pDestProc->serviceLocationPath()->getCount();
	ASSERT(nPSLCount > 0);
	DistanceUnit zPos = _pDestProc->serviceLocationPath()->getPoint(nPSLCount-1).getZ();

	int nPOCCount = outConstraint()->getCount();
	if(nPOCCount > 0)
	{
		Point* pPointOC = new Point[nPOCCount];
		memcpy(pPointOC, outConstraint()->getPointList(), sizeof(Point)*nPOCCount);
		for(int i = 0; i < nPOCCount; i++)
			pPointOC[i].setZ(zPos);
		_pDestProc->initOutConstraint( nPOCCount, pPointOC);
		delete[] pPointOC;
	}

	return true;
}

bool Processor::CopyQueue(Processor* _pDestProc)
{
	int nPSLCount = _pDestProc->serviceLocationPath()->getCount();
	ASSERT(nPSLCount > 0);
	DistanceUnit zPos = _pDestProc->serviceLocationPath()->getPoint(0).getZ();

	int nPQCount = queueLength();
	if(nPQCount> 0)
	{
		Point* pPointQ=new Point[nPQCount];
		memcpy(pPointQ, queuePath()->getPointList(), sizeof(Point)*nPQCount);
		for(int i = 0; i < nPQCount; i++)
			pPointQ[i].setZ(zPos);
		BOOL bFixed = FALSE;
		if(m_pQueue && (m_pQueue->isFixed() == 'Y'))
			bFixed = TRUE;
		_pDestProc->initQueue(bFixed, nPQCount, pPointQ);
		delete[] pPointQ;
	}

	return true;
}

bool Processor::CopyOtherData(Processor* _pDestProc)
{
	return true;
}


//update the min max, include min max;
void GetMinMax(Point& min, Point& max, const Point& newPt ){
	if(newPt.getX() < min.getX()) min.setX(newPt.getX());
	if(newPt.getY() < min.getY()) min.setY(newPt.getY());
	if(newPt.getZ() < min.getZ()) min.setZ(newPt.getZ());

	if(newPt.getX() > max.getX()) max.setX(newPt.getX());
	if(newPt.getY() > max.getY()) max.setY(newPt.getY());
	if(newPt.getZ() > max.getZ()) max.setZ(newPt.getZ());
}
void GetMinMax(Point& min, Point& max, const Path& newPath ){
	for(int i=0 ;i < newPath.getCount(); ++i ){
		GetMinMax(min,max,newPath.getPoint(i));
	}
}

//
void Processor::UpdateMinMax()
{
	if(m_location.getCount()>0){
		m_min = m_max =  m_location.getPoint(0);
	}
	
	GetMinMax(m_min,m_max,m_location);
	GetMinMax(m_min,m_max,In_Constr);
	GetMinMax(m_min,m_max,Out_Constr);
	if(queuePath()){
		GetMinMax(m_min,m_max,*queuePath());
	}


}


void Processor::initDependentProcList(bool bUseDependentProc, const MiscProcessorIDList* _pDependentProcList)
{
	if(m_pDependentProcList)
	{
		delete m_pDependentProcList;
		m_pDependentProcList = NULL;
	}
	m_pDependentProcList = new DependentProcessorList(this);

	m_pDependentProcList->enableDependentProc( bUseDependentProc );
	if(!bUseDependentProc)
		return;

	GroupIndex destGroupIndex;
	for (int i = 0; i < _pDependentProcList->getCount(); i++)
	{
		destGroupIndex = GetTerminal()->procList->getGroupIndex( *(_pDependentProcList->getID(i)) );

		for (int j = destGroupIndex.start; j <= destGroupIndex.end; j++)
		{
			Processor* pDestProcessor = GetTerminal()->procList->getProcessor( j );
			if(pDestProcessor == NULL)
				continue;

			m_pDependentProcList->addDependentProcessor( pDestProcessor );
		}
	}
}

bool Processor::isDependentProcUsed() const
{
	ASSERT(m_pDependentProcList);

	return m_pDependentProcList->isDependentProcUsed();
}

bool Processor::isAllDependentProcsOccupied(Person* _person) const
{
	ASSERT(m_pDependentProcList);

	return m_pDependentProcList->allProcessorsOccupied(_person);
}

ProcessorQueueBase* Processor::GetQueue() const
{
	return m_pQueue;
}
void Processor::clearGateAndReclaimFlowPair()
{
	m_vGateFlowPairProcs.clear();
	m_vReclaimFlowPairProcs.clear();
}

void Processor::ClearSequentialQueue()
{

	if(m_pQueue)
	{

		m_pQueue->ClearAllTempData();
	}
}
void Processor::notifyGateFlowPair(const CMobileElemConstraint& mobileType)
{
	for (int i = 0; i < (int)m_vGateFlowPairProcs.size(); i++)
	{
		int nProcIndex = m_vGateFlowPairProcs.at(i);

		Processor* pProc = m_pTerm->procList->getProcessor(nProcIndex);
		if (pProc == NULL || pProc == this )
			return;
		pProc->GetDirection().Insert(m_direction,mobileType);
	}
}

void Processor::AddGateFlowPairProcessor(int nIndex)
{
	if (std::find(m_vGateFlowPairProcs.begin(),m_vGateFlowPairProcs.end(),nIndex) == m_vGateFlowPairProcs.end())
	{
		m_vGateFlowPairProcs.push_back(nIndex);
	}
}

void Processor::notifyReclaimFlowPair(const CMobileElemConstraint& mobileType)
{
	for (int i = 0; i < (int)m_vReclaimFlowPairProcs.size(); i++)
	{
		int nProcIndex = m_vReclaimFlowPairProcs.at(i);
		Processor* pProc = m_pTerm->procList->getProcessor(nProcIndex);
		if (pProc == NULL || pProc == this)
		{
			return;
		}
		pProc->GetReclaimDrection().Insert(m_reclaimDirection,mobileType);
	}
}

void Processor::AddReclaimFlowPairProcessor(int nIndex)
{
	if (std::find(m_vReclaimFlowPairProcs.begin(),m_vReclaimFlowPairProcs.end(),nIndex) == m_vReclaimFlowPairProcs.end())
	{
		m_vReclaimFlowPairProcs.push_back(nIndex);
	}
}

void Processor::ClearGateAndReclaim()
{
	m_direction.GetPaxDirection().clear();
	m_reclaimDirection.GetPaxDirection().clear();
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

// From get one processor which has one to one relationship ( beyond the _nDestIDLength ) to the _pSourceProcessor
// 1. assume the id length of the processors in this array <= _nDestIdLength;
// in:
//		_pSourceID:			target ID
//		_nDestIdLength:		target level on processor id
// return:
//		null if can not found
//      processor*
Processor* ProcessorArray::GetOneToOneProcessor( const ProcessorID* _pSourceID, int _nDestIdLength ) const
{
//	// TRACE( "-%s-=\n", _pSourceID->GetIDString());
	ProcessorArray arrayResult;
	ProcessorArray arrayCandidated( *this );
	int nDeltaFromLeaf = 0;

	while( arrayCandidated.getCount() > 0 )
	{
		// move 
		int nCount = arrayCandidated.getCount();
		if( nCount == 1 )
			// A-1  ->  G-1-2 is ok
			return arrayCandidated.getItem( 0 );

		ProcessorArray arrayTemp( arrayCandidated );
		arrayCandidated.clear();
		char szSourceLevelName[128];
		int nSourceIdLength = _pSourceID->idLength();
		nSourceIdLength -= nDeltaFromLeaf + 1;
		if( nSourceIdLength == 0 )
			break;	// stop on the first level.
		_pSourceID->getNameAtLevel( szSourceLevelName, nSourceIdLength );
		nCount = arrayTemp.getCount();
		for( int i=0; i<nCount; i++ )
		{
			Processor* pProc = arrayTemp.getItem( i );
			
			int nThisIdLenght = pProc->getID()->idLength();
			nThisIdLenght -= nDeltaFromLeaf;

			if( nThisIdLenght <= _nDestIdLength )
			{
				// satisfy the checking. add to the result array.
//				// TRACE( "-arrayResult = %s--\n", pProc->getID()->GetIDString() );
				arrayResult.addItem( pProc );
			}
			else
			{
				char szLevelName[128];
				pProc->getID()->getNameAtLevel( szLevelName, nThisIdLenght-1 );
				if( strcmp( szLevelName, szSourceLevelName ) == 0 )
				{
					// put into the candidate list.
//					// TRACE( "-arrayCandidated = %s--\n", pProc->getID()->GetIDString() );
					arrayCandidated.addItem( pProc );
				}
			}
		}

		if( arrayCandidated.getCount() == 0 )
		{
			int nCount = arrayTemp.getCount();
			return arrayTemp.getItem( random( nCount ) );
		}
		nDeltaFromLeaf++;
	}
	int nCount = arrayResult.getCount();
	if( nCount == 0 )
		return NULL;
	return arrayResult.getItem( random(nCount) );
}

ProcessorArray::ProcessorArray( const ProcessorArray& _array ) 
	: SortedContainer<Processor> (8, 0, 8) 
{
	int nCount = _array.getCount();
	for( int i=0; i<nCount; i++ )
	{
		addItem( _array.getItem( i ) );
	}
}

//Point Processor::GetPipeExitPoint(int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob)
//{
//	
//	//AfxMessageBox("the Same Floor Problem !");
//	if( this->inConstraintLength())
//		return this->inConstraint(0);
//
//	ProcessorQueue* procQueue = GetProcessorQueue();
//	if( procQueue != NULL )
//	{	
//		int nQueuePointCount = GetProcessorQueue()->cornerCount();
//		if(procQueue->isFixed() == 'Y')
//		{
//
//			if(nQueuePointCount > 0)
//				return this->GetProcessorQueue()->corner( 0);
//		}
//		else
//		{	
//			if(nQueuePointCount > 0)
//			{
//				Point entryPoint = GetProcessorQueue()->corner( 0 );
//				Point tempPoint ;
//
//				double dTravelLength = (std::numeric_limits<double>::max)();
//				CPipeGraphMgr* pPipeMgr = m_pTerm->m_pPipeDataSet->m_pPipeMgr;
//				for (int nQueuePoint = 0; nQueuePoint < nQueuePointCount; ++ nQueuePoint)
//				{
//					CGraphVertexList shortestPath;
//					tempPoint = GetProcessorQueue()->corner( nQueuePoint );
//					if(pPipeMgr->getShortestPathFromLib(outPoint, tempPoint, shortestPath))
//					{
//						shortestPath.ReCalculateLength() ;	
//						if(shortestPath.GetLength() < dTravelLength)
//							entryPoint = tempPoint;
//					}
//				}
//
//				return entryPoint;
//			}
//
//		}
//
//
//	}
//	return getServicePoint( 0 );
//}