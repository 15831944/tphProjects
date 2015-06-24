// AutoRoster.cpp: implementation of the CAutoRoster class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AutoRoster.h"
#include "engine\process.h"
#include "inputs\MobileElemConstraint.h"
#include "inputs\in_term.h"
#include "inputs\assigndb.h"
#include "engine\proclist.h"
#include "reports\SimLevelOfService.h"
#include "../Common/ARCTracker.h"

//#define  _DEBUG 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CAutoRosterItem::CAutoRosterItem()
{

}

CAutoRosterItem::~CAutoRosterItem()
{
	clearData();
}

void CAutoRosterItem::clearData( void )
{
	m_mapAutoProcAss.clear();
}

void CAutoRosterItem::addProcAss( const ProcessorID& _id,const ProcessorRoster& _procAss )
{
	m_mapAutoProcAss[_id].push_back( _procAss );
}

std::map< ProcessorID, ProcAssList>& CAutoRosterItem::getProcAssMap( void )
{
	return m_mapAutoProcAss;
}

void CAutoRosterItem::SortVector(ProcAssList& pa_list)
{
	std::sort( pa_list.begin(), pa_list.end() );
}

void CAutoRosterItem::addAutoRostorToDateSet( InputTerminal* _inTerm)
{
	std::vector<ProcessorRoster*> m_vectAddItem;
	std::vector<int> vDeleteIndex;
	ProcessorRoster* newProcAss = NULL;
	
	std::map< ProcessorID, ProcAssList>::iterator iter = m_mapAutoProcAss.begin();
	for( iter = m_mapAutoProcAss.begin(); iter!= m_mapAutoProcAss.end(); ++iter )
	{
		m_vectAddItem.clear();
		
//		// TRACE(" ProcessorID: %s\r\n",iter->first.GetIDString() );

		// first sort the vector
		ProcAssList pa_List = iter->second;
		SortVector( pa_List );
		//////////////////////////////////////////////////////////////////////////
		CMobileElemConstraint mob(_inTerm);
		bool bFindClose = false;
		ProcessorRoster procAss,nextprocAss;
		
		while(1)
		{
			bFindClose = false;
			vDeleteIndex.clear();
			
			for( UINT i= 0; i< pa_List.size(); i++ )
			{
				nextprocAss = pa_List[i];

			#ifdef _DEBUG
				//char szBuf[256]; matt
				char szBuf[2560];
				nextprocAss.getAbsOpenTime().printTime( szBuf );
//				// TRACE( " Time:%s\r\n",szBuf );
				nextprocAss.getAssignment()->getConstraint(0)->WriteSyntaxStringWithVersion( szBuf );
//				// TRACE( "PaxType:%s\r\n",szBuf );
			#endif // _DEBUG

				if( i == 0 )
				{
					procAss = nextprocAss;
					
					assert( procAss.isOpen()!= 0 ); // not close
					mob = *(procAss.getAssignment()->getConstraint(0));
					
					bFindClose = true;
					vDeleteIndex.push_back(i);
				}

				else
				{
					if( *(nextprocAss.getAssignment()->getConstraint(0)) == mob )
					{
						if( bFindClose && nextprocAss.isOpen()== 0 )
						{
							newProcAss = new ProcessorRoster( _inTerm );
							newProcAss->setFlag(1);
							newProcAss->setOpenTime( procAss.getOpenTime() );
							newProcAss->setAbsOpenTime( procAss.getAbsOpenTime() );
							newProcAss->setCloseTime( nextprocAss.getCloseTime() );
							newProcAss->setAbsCloseTime( nextprocAss.getAbsCloseTime() );
							newProcAss->setAssignment( *(procAss.getAssignment()) );
							m_vectAddItem.push_back( newProcAss );		
							bFindClose = false;
						}
						else if( !bFindClose && nextprocAss.isOpen()!= 0 )
						{
							procAss = nextprocAss;
							bFindClose = true;
						}
						vDeleteIndex.push_back(i);
					}
				}
			}

			if( bFindClose )
			{
				ElapsedTime endTime;
				endTime.set(23,59,59);

				newProcAss = new ProcessorRoster( _inTerm );
				newProcAss->setFlag(1);
				newProcAss->setOpenTime( procAss.getOpenTime() );
				newProcAss->setAbsOpenTime( procAss.getAbsOpenTime() );
				newProcAss->setCloseTime( endTime );
				newProcAss->setAbsCloseTime( endTime );
				newProcAss->setAssignment( *(procAss.getAssignment()) );
				m_vectAddItem.push_back( newProcAss );		
				bFindClose = false;
			}
			//////////////////////////////////////////////////////////////////////////
		
			//delete item
			for( int j = vDeleteIndex.size()-1; j>=0; j-- )
			{
				pa_List.erase( pa_List.begin() + vDeleteIndex[j] );
			}
			
			if( pa_List.size()<=0 )
				break;
		}
		
		addProcAssToDataSet( iter->first, _inTerm, m_vectAddItem );
	}			
}

void CAutoRosterItem::addProcAssToDataSet( const ProcessorID& id,InputTerminal* _inTerm, std::vector<ProcessorRoster*>& _newAssList)
{
	//////////////////////////////////////////////////////////////////////////
	// first clear old data
	int iEntry = _inTerm->procAssignDB->findEntry( id );
	if( iEntry!= INT_MAX )
	{
		_inTerm->procAssignDB->deleteItem( iEntry );
	}
	
	// add new data
	UnmergedAssignments m_unMergedAssgn;
	m_unMergedAssgn.clear();
	for( UINT i=0; i<_newAssList.size(); i++ )
	{
		m_unMergedAssgn.addItem( _newAssList[i] );
	}
	
	if( m_unMergedAssgn.getCount()>0 )
	{
		//first add a new Entry			
		_inTerm->procAssignDB->addEntry( id );
		
		int nIdx = _inTerm->procAssignDB->findEntry( id );
		assert( nIdx != INT_MAX );
		ProcessorRosterSchedule* pSchd = _inTerm->procAssignDB->getDatabase( nIdx );
		
		m_unMergedAssgn.merge( pSchd );
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CAutoRoster::CAutoRoster()
{
	m_pAutoRoster = NULL;
}

CAutoRoster::~CAutoRoster()
{
	clear();
}

void CAutoRoster::setCurRosterIndex( int _index )
{
	m_iCurRosterIndex = _index;
}

int  CAutoRoster::getCurRosterIndex( void )
{
	return m_iCurRosterIndex;
}

void CAutoRoster::clearUserRoster( InputTerminal* _inTerm, const CSimLevelOfService*  _pTLOSSim /*= NULL*/ )
{
	assert( _inTerm );

	if(  _pTLOSSim )
	{
		clearTLOSProcRoster( _pTLOSSim, _inTerm );
		closeAllTLOSProc( _pTLOSSim , _inTerm );
	}
}

void CAutoRoster::init( int _numOfRun)
{
	PLACE_METHOD_TRACK_STRING();
	clear();
	
	m_iRosterItemCount = _numOfRun;
	m_pAutoRoster = new CAutoRosterItem[ m_iRosterItemCount ];
	m_iCurRosterIndex = 0;		
}



void CAutoRoster::clearTLOSProcRoster( const CSimLevelOfService*  _pTLOSSim, InputTerminal* _inTerm, bool _bAddCloseEvent /*= true*/ )
{
	assert( _pTLOSSim );
	assert( _inTerm );

	ProcessorID id;
	id.SetStrDict( _inTerm->inStrDict );
	Processor* pProc = NULL;

	int iItemCount = _pTLOSSim->getSettingCout();
	for( int i=0; i<iItemCount; i++ )
	{
		const CSimLevelOfServiceItem& item = _pTLOSSim->GetSetting( i );
		CString strProcName = item.GetProcessorName();
		id.setID( strProcName );

		GroupIndex group = _inTerm->procList->getGroupIndex( id );
		for( int j = group.start; j<= group.end; j++ )
		{
			ProcessorID ProcID = *(_inTerm->procList->getProcessor(j)->getID());
			// if exist old data , delete it;
			int iEntry = _inTerm->procAssignDB->findEntry( ProcID );
			if( iEntry!= INT_MAX )
			{
				_inTerm->procAssignDB->deleteItem( iEntry );
			}
			
			if( _bAddCloseEvent )
			{
				// add new data
				UnmergedAssignments m_unMergedAssgn;
				m_unMergedAssgn.clear();

				ProcessorRoster* procAss = new ProcessorRoster( _inTerm );
				procAss->setFlag(0); // close
				procAss->setOpenTime(0l);
				procAss->setAbsOpenTime(0l);
				ElapsedTime time;
				time.set(71,59,59);
				procAss->setCloseTime( time );
				procAss->setAbsCloseTime( time );
				CMultiMobConstraint multiMob;
				CMobileElemConstraint mob(_inTerm);
				mob.initDefault();
				multiMob.addConstraint( mob );
				procAss->setAssignment( multiMob );

				m_unMergedAssgn.addItem( procAss );

				//first add a new Entry			
				_inTerm->procAssignDB->addEntry( ProcID );

				int nIdx = _inTerm->procAssignDB->findEntry( ProcID );
				assert( nIdx != INT_MAX );
				ProcessorRosterSchedule* pSchd = _inTerm->procAssignDB->getDatabase( nIdx );

				m_unMergedAssgn.merge( pSchd );
			}
		}
	}
	return;
}

void CAutoRoster::closeAllTLOSProc( const CSimLevelOfService*  _pTLOSSim , InputTerminal* _inTerm)
{
	assert( _pTLOSSim );
	assert( _inTerm );

	ProcessorID id;
	id.SetStrDict( _inTerm->inStrDict );
	Processor* pProc = NULL;

	int iItemCount = _pTLOSSim->getSettingCout();
	for( int i=0; i<iItemCount; i++ )
	{
		const CSimLevelOfServiceItem& item = _pTLOSSim->GetSetting( i );
		CString strProcName = item.GetProcessorName();
		id.setID( strProcName );

		GroupIndex group = _inTerm->procList->getGroupIndex( id );
		for( int j =group.start; j<= group.end; j++ )
		{
			pProc = _inTerm->procList->getProcessor( j );
			pProc->setCloseFlag();
		}
	}
}

void CAutoRoster::clear()
{
	if( m_pAutoRoster )
		delete []m_pAutoRoster;
	m_iRosterItemCount = 0;
	m_iCurRosterIndex = -1;
}

bool CAutoRoster::openProcessor( const ElapsedTime&  _openTime, Processor* _proc, const CMobileElemConstraint& _mob )
{
	assert( m_iCurRosterIndex >=0 && m_iCurRosterIndex< m_iRosterItemCount );
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	ofsstream open_close("d:\\open_close.log", stdios::app);
	CString strProc = _proc->getID()->GetIDString();
	//char szBuf[512]; matt
	char szBuf[2560];
	_mob.WriteSyntaxStringWithVersion( szBuf );
	char szTime[64];
	_openTime.printTime( szTime );
	//// TRACE(" open processor: %s for Pax Type: %s at time: %s\r\n", strProc, szBuf, szTime );
	CString strMsg;
	strMsg.Format("  open processor: %s for Pax Type: %s at time: %s\r\n", strProc, szBuf, szTime );
	strcpy( szBuf, strMsg );
	open_close<< szBuf;
#endif	//_DEBUG
	//////////////////////////////////////////////////////////////////////////
	
	// save the proc_assign
	ProcessorRoster procAss;
	procAss.setFlag( 1 );		// open
	procAss.setOpenTime( _openTime );
	procAss.setAbsOpenTime( _openTime );
	procAss.setCloseTime( _openTime );
	procAss.setAbsCloseTime( _openTime );
	
	CMultiMobConstraint multiMob;
	multiMob.addConstraint( _mob );
	procAss.setAssignment( multiMob );

	ProcessorID id = *(_proc->getID());
	m_pAutoRoster[m_iCurRosterIndex].addProcAss( id, procAss );

	
	// open the processor for _mob
	_proc->open( &procAss );		//??

	return true;
}

bool CAutoRoster::closeProcssor( const ElapsedTime& _closeTime, Processor* _proc, const CMobileElemConstraint& _mob )
{
	assert( m_iCurRosterIndex >=0 && m_iCurRosterIndex< m_iRosterItemCount );
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	ofsstream open_close("d:\\open_close.log", stdios::app);
	CString strProc = _proc->getID()->GetIDString();
	//char szBuf[512]; matt
	char szBuf[2560];
	_mob.WriteSyntaxStringWithVersion( szBuf );
	char szTime[64];
	_closeTime.printTime( szTime );
	//// TRACE(" open processor: %s for Pax Type: %s\r\n", strProc, szBuf );
	CString strMsg;
	strMsg.Format(" close processor: %s for Pax Type: %s at time: %s\r\n", strProc, szBuf, szTime );
	strcpy( szBuf, strMsg );
	open_close<< szBuf;
#endif // _DEBUG
	//////////////////////////////////////////////////////////////////////////
	
	// save the procassign 
	ProcessorRoster procAss;
	procAss.setFlag( 0 );		//close
	procAss.setOpenTime( _closeTime );
	procAss.setAbsOpenTime( _closeTime );
	procAss.setCloseTime( _closeTime );
	procAss.setAbsCloseTime( _closeTime );
	
	CMultiMobConstraint multiMob;
	multiMob.addConstraint( _mob );
	procAss.setAssignment( multiMob );

	ProcessorID id = *(_proc->getID());
	m_pAutoRoster[m_iCurRosterIndex].addProcAss( id,procAss );

	// close the processor for _mob
	_proc->close( &procAss );		//??

	return true;
}

Processor* CAutoRoster::openProcessor( const ElapsedTime&  _openTime, const ProcessorID& _procID, const CMobileElemConstraint& _mob , InputTerminal* _inTerm )
{
	GroupIndex group = _inTerm->procList->getGroupIndex( _procID );
	Processor* pProc = NULL;
	
	// get all closed processor
	std::vector<Processor*> vClosedProcList;
	for( int i=group.start; i<= group.end; i++ )
	{
		pProc = _inTerm->procList->getProcessor( i );
		if( pProc->isClosed() && pProc->GetTLOSFlag() )
			vClosedProcList.push_back( pProc );
	}
	
	if( _inTerm->procList->GetDynamicCreatedProcs() )
	{
		group = _inTerm->procList->GetDynamicCreatedProcs()->getGroupIndex( _procID );
		if( group.start >= 0 )
		{
			for( int i=group.start; i<= group.end; i++ )
			{
				pProc = _inTerm->procList->GetDynamicCreatedProcs()->getProcessor( i );
				if( pProc->isClosed() && pProc->GetTLOSFlag() )
					vClosedProcList.push_back( pProc );
			}
		}
	}	

	if( vClosedProcList.size() >0 )
	{
		int idx = random( vClosedProcList.size() );
		pProc = vClosedProcList[idx];
		openProcessor( _openTime, pProc, _mob );
		return 	pProc;
	}
	return NULL;
}

Processor* CAutoRoster::closeProcssor( const ElapsedTime& _closeTime, const ProcessorID& _procID, const CMobileElemConstraint& _mob , InputTerminal* _inTerm )
{
	GroupIndex group = _inTerm->procList->getGroupIndex( _procID );
	Processor* pProc = NULL;
	
	// get all open processor that is serve for _mob
	std::vector<Processor*> vOpenProcList;
	for( int i=group.start; i<= group.end; i++ )
	{
		pProc = _inTerm->procList->getProcessor( i );
		if( pProc->isOpened() && pProc->GetTLOSFlag() && pProc->GetCurrentServicePaxType() == _mob )
			vOpenProcList.push_back( pProc );
	}

	if( _inTerm->procList->GetDynamicCreatedProcs() )
	{
		group = _inTerm->procList->GetDynamicCreatedProcs()->getGroupIndex( _procID );
		if( group.start >= 0 )
		{
			for( int i=group.start; i<= group.end; i++ )
			{
				pProc = _inTerm->procList->GetDynamicCreatedProcs()->getProcessor( i );
				if( pProc->isOpened() && pProc->GetTLOSFlag()  && pProc->GetCurrentServicePaxType() == _mob )
					vOpenProcList.push_back( pProc );
			}
		}
	}
	
	if( vOpenProcList.size() >0 )
	{
		int idx = random( vOpenProcList.size() );
		pProc = vOpenProcList[idx];
		closeProcssor( _closeTime, pProc, _mob );
		return 	pProc;
	}
	
	return NULL;
}


Processor* CAutoRoster::openProcessor( const ElapsedTime&  _openTime,std::vector<Processor*>& _procList, const CMobileElemConstraint& _mob , InputTerminal* _inTerm )
{
	Processor* pProc = NULL;

	// get all closed processor
	std::vector<Processor*> vClosedProcList;
	for( UINT i=0; i< _procList.size(); i++ )
	{
		pProc = _procList.at( i );
//		// TRACE ( "\n%s\n",pProc->getID()->GetIDString() );
		if( pProc->isClosed() && pProc->GetTLOSFlag() )
		{
//			// TRACE ( "\n%s\n",pProc->getID()->GetIDString() );
			vClosedProcList.push_back( pProc );
		}
			
	}
	
	if( vClosedProcList.size() >0 )
	{
		int idx = random( vClosedProcList.size() );
		pProc = vClosedProcList[idx];
//		// TRACE ( "\n%s\n",pProc->getID()->GetIDString() );
		openProcessor( _openTime, pProc, _mob );
		return 	pProc;
	}

	return NULL;
}


Processor* CAutoRoster::closeProcssor( const ElapsedTime& _closeTime,  std::vector<Processor*>& _procList, const CMobileElemConstraint& _mob , InputTerminal* _inTerm )
{
	Processor* pProc = NULL;
	
	// get all open processor that is serve for _mob
	std::vector<Processor*> vOpenProcList;
	for( UINT i=0; i< _procList.size(); i++ )
	{
		pProc = _procList.at( i );
		if( pProc->isOpened() && pProc->GetTLOSFlag() && pProc->GetCurrentServicePaxType() == _mob )
			vOpenProcList.push_back( pProc );
	}
	
	if( vOpenProcList.size() >0 )
	{
		int idx = random( vOpenProcList.size() );
		pProc = vOpenProcList[idx];
		closeProcssor( _closeTime, pProc, _mob );
		return 	pProc;
	}
	
	return NULL;
}

void CAutoRoster::closeAllProcssor( const ElapsedTime& _closeTime, const std::vector<Processor*>& _procList, const CMobileElemConstraint& _mob , InputTerminal* _inTerm )
{
	Processor* pProc = NULL;
	int iSize = _procList.size();
	for( int i=0; i<iSize; ++i )
	{
		pProc = _procList[i];
		closeProcssor( _closeTime, pProc, _mob );
	}	
}

Processor* CAutoRoster::closeProcssor( const ElapsedTime& _closeTime,  std::vector<int>& _procIndexList, const CMobileElemConstraint& _mob , InputTerminal* _inTerm )
{
	Processor* pProc = NULL;
	
	// get all open processor that is serve for _mob
	std::vector<Processor*> vOpenProcList;
	for( UINT i=0; i< _procIndexList.size(); i++ )
	{
		pProc = _inTerm->procList->getProcessor( _procIndexList[i] );
		if( pProc->isOpened() && pProc->GetTLOSFlag() && pProc->GetCurrentServicePaxType() == _mob )
			vOpenProcList.push_back( pProc );
	}
	
	if( vOpenProcList.size() >0 )
	{
		int idx = random( vOpenProcList.size() );
		pProc = vOpenProcList[idx] ;
//		// TRACE ( "\n%s\n",pProc->getID()->GetIDString() );
		closeProcssor( _closeTime, pProc, _mob );
		return 	pProc;
	}

	return NULL;
}


void CAutoRoster::addAutoRostorToDateSet( InputTerminal* _inTerm )
{
	addAutoRostorToDateSet( m_iCurRosterIndex ,_inTerm);
}

void CAutoRoster::addAutoRostorToDateSet( int _index,InputTerminal* _inTerm )
{	
	assert(_inTerm);
	assert(m_iCurRosterIndex >=0 && m_iCurRosterIndex< m_iRosterItemCount );
	
	m_pAutoRoster[_index].addAutoRostorToDateSet(  _inTerm );
	
}