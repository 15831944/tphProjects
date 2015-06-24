// Monitor.cpp: implementation of the CMonitor class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Monitor.h"
#include "reports\SimLevelOfService.h"
#include "inputs\SimParameter.h"
#include "engine\proclist.h"
#include "engine\procq.h"
#include "TLOSEvent.h"
#include "engine\sequencq.h"
#include "engine\autoroster.h"
#include "../Common/ARCTracker.h"
const long lTLOSEventInterval = 120;
const CString TLOSLogString = "TLOSLOG";

//#include "reports\SimLevelOfService.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonitor::CMonitor( InputTerminal* _pTerm, CSimParameter* _pSimSetting )
:m_pTerm( _pTerm ), m_pSimSetting( _pSimSetting )
{

}

CMonitor::~CMonitor()
{

}

// set processor's TLOS according to the sim setting parameter
void CMonitor::InitTLOSData()
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT( m_pSimSetting->GetSimPara()->GetClassType() == "LEVEOFSERVICE" );
	CSimLevelOfService* pServicePara= (CSimLevelOfService*)m_pSimSetting->GetSimPara();

	int iCount = pServicePara->getSettingCout();
	//char sPaxType[256]; matt
	char sPaxType[2560];
	ProcessorID procGroup;
	procGroup.SetStrDict( m_pTerm->inStrDict );
	Processor* pProc = NULL;
	for( int i=0; i<iCount; ++i )
	{
		CHistoryRecord tempRecord;
		CSimLevelOfServiceItem& tempItem = pServicePara->getSetting( i );
		tempRecord.m_csProc = tempItem.GetProcessorName();

		// set TLOS flag of interesting proc
		procGroup.setID( tempRecord.m_csProc );
		GroupIndex groupIdx = m_pTerm->procList->getGroupIndex( procGroup );
		ASSERT( groupIdx.start >= 0 );
		for( int j=groupIdx.start; j<= groupIdx.end; ++j )
		{
			pProc = m_pTerm->procList->getProcessor( j );
			pProc->SetTLOSFlag( true );
			pProc->setNewCreatedTLOSProcFlag(false);
		}

		
		// init queue length, queue time history map
		const std::vector<Mob_ServicTime>& vService = tempItem.getSpecificPaxTypeService();
		int iItemCount = vService.size();

		for( j=0; j<iItemCount; ++j )
		{
			vService[j].first.WriteSyntaxStringWithVersion( sPaxType );			
			tempRecord.m_mapQLenHistory[ sPaxType ].push_back( 'A' );
			tempRecord.m_mapQTimeHistory[ sPaxType ].push_back( 'A' );	
			tempRecord.m_mapNeedToGenTLOSEvent[ sPaxType ] = true;
		}
		m_vAllProcHistory.push_back( tempRecord );
		/*
		int iSize = tempRecord.m_mapQLenHistory.size();
		iSize = tempRecord.m_mapQTimeHistory.size();
		iSize = m_vAllProcHistory[0].m_mapQLenHistory.size();
		iSize = m_vAllProcHistory[0].m_mapQTimeHistory.size();
		*/
	}
}
void CMonitor::DoTLOSCheck( const ProcessorID* _pWhichProc , const ElapsedTime& _currentTime , const CString& _csPaxType )
{

	CSimLevelOfService* pServicePara= (CSimLevelOfService*)m_pSimSetting->GetSimPara();
	ASSERT( pServicePara );

	const CSimLevelOfServiceItem* pServiceItem = pServicePara->getTheNearestProc( *_pWhichProc );
	
	// get history recode according to the processor name
	int iHistorySize = m_vAllProcHistory.size();
	CHistoryRecord* pHistory =	NULL;
	for( int j=0; j<iHistorySize; ++j )
	{
		if( m_vAllProcHistory[j].m_csProc == pServiceItem->GetProcessorName() )
		{
			pHistory = &m_vAllProcHistory[ j ];
			break;
		}
	}
	
	
	ProcessorID procGroup;
	procGroup.SetStrDict( m_pTerm->inStrDict );
	procGroup.setID( pServiceItem->GetProcessorName() );

	GroupIndex groupIdx =  m_pTerm->procList->getGroupIndex( procGroup );
	ASSERT( groupIdx.start >= 0 );

	

	//std::vector<int>vProcs;	
	std::vector<Processor*>vProcs;	
	Processor* pProc = NULL;
	for( int i= groupIdx.start ; i<= groupIdx.end; ++i )
	{
		pProc = m_pTerm->procList->getProcessor( i );

		if( pProc->isOpened() )
		{
///			// TRACE ("\n%s\n",pProc->getID()->GetIDString());
			//char tempChar[256]; matt
			char tempChar[2560];

			pProc->GetCurrentServicePaxType().WriteSyntaxStringWithVersion( tempChar );
			if( _csPaxType == tempChar )
			{
				//vProcs.push_back( i );
				vProcs.push_back( pProc );
			}
			
		}
	}

	int nMinQLen = INT_MAX;
	if( m_pTerm->procList->GetDynamicCreatedProcs() )			
	{
		groupIdx =  m_pTerm->procList->GetDynamicCreatedProcs()->getGroupIndex( procGroup );
		if( groupIdx.start >= 0 )
		{
			for( i= groupIdx.start ; i<= groupIdx.end; ++i )
			{
				pProc = m_pTerm->procList->GetDynamicCreatedProcs()->getProcessor( i );
				
				if( pProc->isOpened() )
				{
					//// TRACE ("\n%s\n",pProc->getID()->GetIDString());
					//char tempChar[256]; matt
					char tempChar[2560];
					pProc->GetCurrentServicePaxType().WriteSyntaxStringWithVersion( tempChar );
					if( _csPaxType == tempChar )
					{
						vProcs.push_back( pProc );
						if(pProc->getQueueLength() < nMinQLen )
							nMinQLen = pProc->getQueueLength();
					}
					
				}
			}
		}
	}

	ASSERT( pHistory );
	pHistory->m_iMaxQueueLength = 0;
	CMobileElemConstraint tempPaxType(m_pTerm);
	tempPaxType.SetInputTerminal( m_pTerm );
	char szPaxType[1024];
	strcpy( szPaxType, _csPaxType );
	tempPaxType.setConstraintWithVersion( szPaxType );
	bool bReallyOpenProc = false;
	int iQLengthAction = CheckQueueLength( vProcs, pServiceItem , tempPaxType , *pHistory );
	if( iQLengthAction == 1 )// want to open processor
	{

		if( OpenProcessor( *_pWhichProc, _currentTime, tempPaxType ) )
		{
			bReallyOpenProc = true;
		}

		// Log 

	}

	int iQTimeAction = CheckQueueTime( vProcs, pServiceItem, tempPaxType, *pHistory, _currentTime );
	if( iQTimeAction == 1 )// want to open processor
	{
		if( OpenProcessor( *_pWhichProc, _currentTime, tempPaxType ) )
		{
			bReallyOpenProc = true;
		}

	}

	// really want to open processor, but no processor can be opened. 
	// so dynamic created proc if necessary
	int nUpperQueueLength = pServiceItem->getTheNearestPax(tempPaxType)->m_iUpperQueueLength;
	if( (iQLengthAction == 1 || iQTimeAction == 1 ) && !bReallyOpenProc /*&& nMinQLen >= nUpperQueueLength*/)
	{
		
		Processor* pDynamicCreatedProc = m_pTerm->procList->DynamicCreateProcByTLOS( _pWhichProc, m_pTerm );
		if( pDynamicCreatedProc )
		{
			pDynamicCreatedProc->setNewCreatedTLOSProcFlag(true);
			OpenProcessor( pDynamicCreatedProc, _currentTime, tempPaxType );
			pDynamicCreatedProc->setAvail();
			// to make sure that proc log has the pos for new created proc
			pDynamicCreatedProc->writeLog();
		}

		// Log 


	}
	
	if( iQTimeAction == 2 && iQLengthAction == 2  )// they all want to close some processor
	{
		CloseProcessor( vProcs, _currentTime, tempPaxType );			

		// Log 

	}

	if( pHistory->m_iMaxQueueLength <= 0 )// 
	{
		m_pTerm->m_pAutoRoster->closeAllProcssor( _currentTime, vProcs, tempPaxType, m_pTerm );
		pHistory->m_mapNeedToGenTLOSEvent[ _csPaxType ] = true;
	}	
	else
	{
		pHistory->m_mapNeedToGenTLOSEvent[ _csPaxType ] = false;
		GenerateNewTLOSEvent( _pWhichProc, _currentTime , _csPaxType );		
	}			
	// Log 

}

// test queue length to see if should open or close some processor 
int CMonitor::CheckQueueLength( const std::vector<Processor*>& _vProcs ,const CSimLevelOfServiceItem* _pServiceItem, const CMobileElemConstraint& _paxType  ,CHistoryRecord& _history ) 
{
	int iMaxQLen = GetCurrentMaxQueueLen( _vProcs );

	if( iMaxQLen > _history.m_iMaxQueueLength )
	{
		_history.m_iMaxQueueLength = iMaxQLen;
	}
	const SERVICE_ITEMS* pService = _pServiceItem->getTheNearestPax( _paxType );
	ASSERT( pService );
	char chType;
	if( iMaxQLen < pService->m_iLowerQueueLength )
	{
		chType ='A';
	}
	else if( iMaxQLen > pService->m_iUpperQueueLength )
	{
		chType = 'C';
	}
	else
	{
		chType = 'B';
	}	

	// char sPaxType[256]; matt
	char sPaxType[2560];
//	_pServiceItem->getTheNearestPaxType( )
	_paxType.WriteSyntaxStringWithVersion( sPaxType );
	//int iSize = _history.m_mapQLenHistory.size();

	// Log 

	
	return OpenOrClose( chType,  _history.m_mapQLenHistory[ sPaxType ]  );
}

int CMonitor::GetCurrentMaxQueueLen( const std::vector<Processor*>& _vProcs )const
{
	Processor* pProc = NULL;
	int iQLength = 0;
	for( UINT i= 0 ; i< _vProcs.size(); ++i )
	{
		//pProc = m_pTerm->procList->getProcessor( _vProcs[i] );
		pProc = _vProcs[i];
		int iCurrentQueueLen = pProc->GetProcessorQueue()->getQueueLength();
		if( iCurrentQueueLen > iQLength )
		{
			iQLength = iCurrentQueueLen;
		}
	}
	return iQLength;
}

// test waiting in queue time to see if should open or close some processor 
int CMonitor::CheckQueueTime( const std::vector<Processor*>& _vProcs ,const CSimLevelOfServiceItem* _pServiceItem, const CMobileElemConstraint& _paxType  ,CHistoryRecord& _history ,const ElapsedTime& _currentTime) 
{
	int iMaxQTime = GetCurrentMaxQueueTime( _vProcs, _currentTime );

	const SERVICE_ITEMS* pService = _pServiceItem->getTheNearestPax( _paxType );
	ASSERT( pService );
	char chType;
	if( iMaxQTime < pService->m_lLowerQueueWaitingSeconds )
	{
		chType ='A';
	}
	else if( iMaxQTime > pService->m_lUpperQueueWaitingSeconds )
	{
		chType = 'C';
	}
	else
	{
		chType = 'B';
	}	

//	char sPaxType[256]; matt
	char sPaxType[2560];
	_paxType.WriteSyntaxStringWithVersion( sPaxType );
	
	return OpenOrClose( chType,  _history.m_mapQTimeHistory [ sPaxType ]  );
}
int CMonitor::GetCurrentMaxQueueTime(const std::vector<Processor*>& _vProcs ,const ElapsedTime& _currentTime) const
{
	Processor* pProc = NULL;
	int iMaxWaitTime = -1;
	for( UINT i= 0 ; i< _vProcs.size(); ++i )
	{
		pProc =  _vProcs[i] ;
		int iTime = _currentTime.asMinutes() - ((SequentialQueue*)pProc->GetProcessorQueue())->GetMaxWaitTime().asMinutes();
		if( iTime > iMaxWaitTime )
		{
			iMaxWaitTime = iTime;
		}
	}
	return iMaxWaitTime;
	
}


/*
		
		|
		|
		|
		|
		|						Area C
		|
		|
		|____________________________________________________________________________uper line
		|
		|
		|
		|						Area B
		|
		|
		|_____________________________________________________________________________ lower line
		|
		|
		|			
		|						Area A
		|
		|
		|
		|
--------|--------------------------------------------------------------------------------------->
		|O
		|
		|
		|


*/
// decide opening processor or closing processor according to history record
int CMonitor::OpenOrClose(char _chNewState,HISTORY& _history )
{

	int iSize = _history.size();
	if( iSize == 0 )
	{
		return 0;
	}
	char chLast = _history[ iSize -1 ];
	if( chLast == _chNewState && _chNewState == 'B' )
	{
		return 0;//no thing to do
	}

	if( chLast == 'C' && _chNewState == 'C' )
	{
		return 1;
	}

	if( chLast == 'A' && _chNewState == 'A' )
	{
		return 2;
	}


	if( std::find( _history.begin(), _history.end(), _chNewState ) != _history.end() )
	{
		_history.clear();
		_history.push_back( _chNewState );
		return 0;//no thing to do
	}
	else
	{
		_history.push_back( _chNewState );
	}

	
	if( _chNewState == 'C' )
	{
		_history.clear();
		_history.push_back( _chNewState );
		return 1;//open
	}
	else if( _chNewState == 'A' )
	{
		_history.clear();
		_history.push_back( _chNewState );
		return 2;//close
	}

	return 0;//noting to do
}

// when person arrival at processor's queue, call this function to notify the monitor
void CMonitor::PersonArrivalAtProcQueue( const Processor* _pWhichProc , const ElapsedTime& _currentTime , const CMobileElemConstraint& _paxType )
{
	CSimLevelOfService* pServicePara= (CSimLevelOfService*)m_pSimSetting->GetSimPara();
//	// TRACE("\n%s\n", _pWhichProc->getID()->GetIDString() );
	const CSimLevelOfServiceItem* pServiceItem = pServicePara->getTheNearestProc( *_pWhichProc->getID() );
	
	ASSERT( pServiceItem );
	CString csProcName = pServiceItem->GetProcessorName();
	const CMobileElemConstraint* pPaxType = pServiceItem->getTheNearestPaxType( _paxType );
	if( pPaxType )
	{
		//char sPaxType[256];  matt
		char sPaxType[2560];
		pPaxType->WriteSyntaxStringWithVersion( sPaxType );
		int iSize = m_vAllProcHistory.size();
		for( int i=0; i<iSize; ++i )
		{
			if( csProcName == m_vAllProcHistory[i].m_csProc )
			{
				if( m_vAllProcHistory[i].m_mapNeedToGenTLOSEvent[ sPaxType] )
				{
					GenerateNewTLOSEvent( _pWhichProc->getID(), _currentTime , sPaxType );
					m_vAllProcHistory[i].m_mapNeedToGenTLOSEvent[ sPaxType] = false;
				}

				break;
			}
		}
	}	
}

// generate a new TLOS event and add it into event list
void CMonitor::GenerateNewTLOSEvent( const ProcessorID* _pWhichProc , const ElapsedTime& _currentTime, const CString& _csPaxType )
{
	TLOSEvent* pEvent = new TLOSEvent();
	pEvent->SetRelatedProcID( _pWhichProc );
	pEvent->SetPaxTypeString( _csPaxType );
	pEvent->setTime( _currentTime + lTLOSEventInterval );
	pEvent->addEvent();    
}

Processor* CMonitor::OpenProcessor(  std::vector<Processor*>& _procs , const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const
{
	CSimLevelOfService* pServicePara= (CSimLevelOfService*)m_pSimSetting->GetSimPara();
	ASSERT( pServicePara );
	
	const CSimLevelOfServiceItem* pServiceItem = pServicePara->getTheNearestProc( *_procs.at( 0 )->getID() );
	
	const CMobileElemConstraint*  pPaxType = pServiceItem->getTheNearestPaxType( _paxType );
	if( pPaxType )
	{
		return m_pTerm->m_pAutoRoster->openProcessor( _currentTime, _procs, *pPaxType , m_pTerm );
	}
	else 
	{
		return NULL;
	}
}
// call roster to open one of processors in proc group  for such pax type at such time
Processor* CMonitor::OpenProcessor( const ProcessorID& _procGroupID , const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const
{
	CSimLevelOfService* pServicePara= (CSimLevelOfService*)m_pSimSetting->GetSimPara();
	ASSERT( pServicePara );
	
	const CSimLevelOfServiceItem* pServiceItem = pServicePara->getTheNearestProc( _procGroupID );

	ASSERT( pServiceItem );
	
	ProcessorID tempId;
	tempId.SetStrDict( m_pTerm->inStrDict );
	tempId.setID( pServiceItem->GetProcessorName() );

	const CMobileElemConstraint*  pPaxType = pServiceItem->getTheNearestPaxType( _paxType );
	if( pPaxType )
	{
		return m_pTerm->m_pAutoRoster->openProcessor( _currentTime, tempId , *pPaxType , m_pTerm );
	}
	else 
	{
		return NULL;
	}
	
}
void CMonitor::OpenProcessor( Processor* _pProcToOpen , const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const
{
	CSimLevelOfService* pServicePara= (CSimLevelOfService*)m_pSimSetting->GetSimPara();
	ASSERT( pServicePara );
	
	const CSimLevelOfServiceItem* pServiceItem = pServicePara->getTheNearestProc( *_pProcToOpen->getID() );
	ASSERT( pServiceItem );
	const CMobileElemConstraint*  pPaxType = pServiceItem->getTheNearestPaxType( _paxType );
	if( pPaxType )
	{
		m_pTerm->m_pAutoRoster->openProcessor( _currentTime, _pProcToOpen, *pPaxType );
	}
}
// call roster to close one of processors in proc group for such pax type at such time
Processor* CMonitor::CloseProcessor( const ProcessorID& _procGroupID , const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const
{
	CSimLevelOfService* pServicePara= (CSimLevelOfService*)m_pSimSetting->GetSimPara();
	ASSERT( pServicePara );
	
	const CSimLevelOfServiceItem* pServiceItem = pServicePara->getTheNearestProc( _procGroupID );

	ASSERT( pServiceItem );
	
	ProcessorID tempId;
	tempId.SetStrDict( m_pTerm->inStrDict );
	tempId.setID( pServiceItem->GetProcessorName() );
	

	const CMobileElemConstraint*  pPaxType = pServiceItem->getTheNearestPaxType( _paxType );
	if( pPaxType )
	{	
		return m_pTerm->m_pAutoRoster->closeProcssor( _currentTime, tempId , *pPaxType , m_pTerm );
	}
	else
	{
		return NULL;
	}
}
Processor* CMonitor::CloseProcessor(  std::vector<Processor*>_vProcs, const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const
{

	return m_pTerm->m_pAutoRoster->closeProcssor( _currentTime, _vProcs, _paxType , m_pTerm );
}
