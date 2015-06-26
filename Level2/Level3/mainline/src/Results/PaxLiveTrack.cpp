// PaxLiveTrack.cpp: implementation of the CPaxLiveTrack class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PaxLiveTrack.h"
#include "engine\terminal.h"
#include "common\SimAndReportManager.h"
#include <algorithm>
#include <iterator>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxLiveTrack::CPaxLiveTrack()
{
	clear();
}

CPaxLiveTrack::~CPaxLiveTrack()
{
	clear();
}

// get & set 
void CPaxLiveTrack::setPaxID( long _lID )
{
	m_lPaxID = _lID;
}

void CPaxLiveTrack::setLogIndex( long _lIndex )
{
	m_lLogIndex = _lIndex;
}

long CPaxLiveTrack::getPaxID( void ) const
{
	return m_lPaxID;
}

long CPaxLiveTrack::getLogIndex( void ) const
{
	return m_lLogIndex;
}

long CPaxLiveTrack::getEventCount( void ) const
{
	return m_vTrack.size();
}

const MobEventStruct& CPaxLiveTrack::getEvent( long _lIdx ) const
{
	assert( _lIdx >=0 && (unsigned)_lIdx < m_vTrack.size() );
	return m_vTrack[_lIdx];
}

// init
bool CPaxLiveTrack::initByID( long _lID, Terminal* _term, const CString& _strPrjPath )
{
	// clear
	clear();
	
	if( _lID <0 )
		return false;
	
	// get pax des struct
	CString strPaxLogFileName = _term->GetSimReportManager()->GetCurrentLogsFileName( PaxDescFile, _strPrjPath );	
	_term->paxLog->LoadDataIfNecessary( strPaxLogFileName );
	
	MobLogEntry element;
	int i;
	for(i=0; i < _term->paxLog->getCount(); i++ )
	{
		_term->paxLog->getItem(element, i);
		if( element.getID() == _lID )
			break;
	}
	
	if( i == _term->paxLog->getCount() )	// can not get the pax id
		return false;
	
	return initByIndex( element.getIndex(), _term, _strPrjPath );

	
}

bool CPaxLiveTrack::initByIndex( long _lIndex, Terminal* _term, const CString& _strPrjPath )
{
	// clear;
	clear();

	if( _lIndex <0 )
		return false;
	
	// get pax des struct
	CString strPaxLogFileName = _term->GetSimReportManager()->GetCurrentLogsFileName( PaxDescFile, _strPrjPath );	
	_term->paxLog->LoadDataIfNecessary( strPaxLogFileName );
	
	if( _lIndex > _term->paxLog->getCount() )
		return false;

	// get log entry by index
	MobLogEntry element;
	_term->paxLog->getItem( element, _lIndex );
	setPaxID( element.getID() );
	setLogIndex( element.getIndex() );

	// open log file
	CString strPaxEventFileName = _term->GetSimReportManager()->GetCurrentLogsFileName( PaxEventFile , _strPrjPath );
	CString sPaxEventFile = strPaxEventFileName;
	if (element.GetFileIndex())
	{
		CString strLeft;
		long lPos = strPaxEventFileName.Find('.');
		strLeft = strPaxEventFileName.Left(lPos);

		sPaxEventFile.Format(_T("%s%03d.log"),strLeft,element.GetFileIndex());
	}
	

	std::ifstream logfile( sPaxEventFile, std::ios::in | std::ios::binary );
	if( logfile.bad() )
		return false;
	
	// get the pax all track
	long beginPos	= element.GetMobDesc().startPos;
	long endPos		= element.GetMobDesc().endPos + sizeof( MobEventStruct ) ;
	long eventNum   = ( endPos-beginPos )/sizeof( MobEventStruct );
	
	event_list tmpList( eventNum );
	// read all event to memory
	logfile.seekg( beginPos, std::ios::beg );
	logfile.read( (char*)&( tmpList[0] ), endPos - beginPos );
	
	// copy unique struct
	std::unique_copy( tmpList.begin(), tmpList.end(), std::back_inserter<event_list>( m_vTrack ), MobEventStructIsEqual );
	
	return true;
}

void CPaxLiveTrack::clear( void )
{
	m_lPaxID		= -1l;
	m_lLogIndex		= -1l;
	m_vTrack.clear();
}	


bool MobEventStructIsEqual( const MobEventStruct& _struct1, const MobEventStruct& _struct2 )
{
	return _struct1.state == _struct2.state && _struct1.procNumber == _struct2.procNumber;
}
