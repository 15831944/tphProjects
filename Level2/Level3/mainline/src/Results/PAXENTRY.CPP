 #include "stdafx.h"
#include <string.h>
#include "results\paxentry.h"
#include "results\out_term.h"
#include "results\fltlog.h"
//#include "Ogre\Ogre.h"
#include "MultiFileEventLog.h"
#include "TempMobileEventLog.h"


MobLogEntry::MobLogEntry() : TerminalLogItem<MobDescStruct,MobEventStruct> ()
{
    item.id = 0l;
    item.startTime = 0l;
    item.endTime = 0l;
	item.terminalStartTime = 0l;
	item.terminalEndTime = 0l;
    item.startPos = -1;
    item.endPos = -1;
	item.fileID = -1;
    item.intrinsic = 0;
    for (int i = 0; i < MAX_PAX_TYPES; i++)
        item.paxType[i] = 0;
    item.arrFlight = -1;
    item.depFlight = -1;
    item.groupSize = 0;
    item.groupMember = 0;
    item.bagCount = 0;
    item.cartCount = 0;
    item.ownerID = -1;
    item.indexNum = -1;
	m_pEventLog = NULL;
	item.ownStart = true;
	item.mobileType = 0;
	item.eachPaxOwnNoPax = false;
	item.bMissFlightDeath = false;
	item.carryonCount = 0;

	item.xAngle = 0.0;
	item.yAngle = 0.0;
	item.zAngle = 0.0;
	item.w = 1.0;
	item.enplanTime = INT_MAX;
	item.nBulkIndex=-1;
	item.nFreqIndex = -1;
	item.bBulkProfileCapacity = BEC_UNKNOWN;

}

void MobLogEntry::echo (ofsstream& p_stream, const CString& _csProjPath) const
{
	assert( m_pOutTerm );
    p_stream << item.id << ',' << getEntryTime() << ',' << getExitTime();
    char str[FULL_ID_LEN];
    if (item.arrFlight == -1)
        strcpy (str, "-1");
    else
        m_pOutTerm->flightLog->getFlightID (str, item.arrFlight, 'A');
    p_stream << ',' << str << ',';
    if (item.depFlight == -1)
        strcpy (str, "-1");
    else
        m_pOutTerm->flightLog->getFlightID (str, item.depFlight, 'D');
    p_stream << str << ',' << (int)item.intrinsic << ',' << getCount() << '\n';
}


ElapsedTime MobLogEntry::getEntryTime (void) const
{
    ElapsedTime t;
    t.setPrecisely (item.startTime);
    return t;
}

ElapsedTime MobLogEntry::getTerminalEntryTime(void)const
{
	ElapsedTime t;
	t.setPrecisely (item.terminalStartTime);
	return t;
}

ElapsedTime MobLogEntry::getTerminalEndTime(void)const
{
	ElapsedTime t;
	t.setPrecisely (item.terminalEndTime);
	return t;
}

ElapsedTime MobLogEntry::getExitTime (void) const
{
    ElapsedTime t;
    t.setPrecisely (item.endTime);
    return t;
}

void MobLogEntry::getPaxType (int *p_type) const
{
    for (int i = 0; i < MAX_PAX_TYPES; i++)
        p_type[i] = (int)item.paxType[i];
}

void MobLogEntry::setPaxType (const int *p_type)
{
    for (int i = 0; i < MAX_PAX_TYPES; i++)
        item.paxType[i] = (short)p_type[i];
}

const RotateAngle MobLogEntry::getAngle()
{
	RotateAngle rotation;
	rotation.x = item.xAngle;
	rotation.y = item.yAngle;
	rotation.z = item.zAngle;
	rotation.w = item.w;

	return rotation;
}

void MobLogEntry::SetAngle(const RotateAngle& rotation)
{
	item.xAngle = (float)rotation.x;
	item.yAngle = (float)rotation.y;
	item.zAngle = (float)rotation.z;
	item.w = (float)rotation.w;
}

ElapsedTime MobLogEntry::getEnplanTime() const
{
	ElapsedTime t;
	t.setPrecisely (item.enplanTime);
	return t;

}

ElapsedTime MobLogEntry::GetMissFlightTime() const
{
	ElapsedTime t;
	t.setPrecisely (item.missFlightTime);
	return t;
}

ElapsedTime MobLogEntry::GetRealDepartureTime() const
{
	ElapsedTime t;
	t.setPrecisely (item.flightRealDepartureTime);
	return t;
}

void MobLogEntry::setEnplanTime( ElapsedTime p_time )
{
	item.enplanTime = (long)p_time;
}

// void MobLogEntry::setEventList( const MobEventStruct *p_item, int p_count )
// {
// 	assert( m_pEventLog );
// 	long lSize = sizeof(MobEventStruct)*p_count;
// 	long lTotal = lSize + m_pEventLog->getPositionForWrite();
// 
// 	item.startPos = m_pEventLog->getPositionForWrite();
// 	if (m_pEventLog->needCreateFile(lTotal))
// 		item.startPos = 0;
// 	
// 	m_pEventLog->setEventList(p_item, p_count);
// 	item.endPos = m_pEventLog->getPositionForWrite() - sizeof( MobEventStruct );
// 	item.fileID = m_pEventLog->GetLastFileID();
// }
// 
// void MobLogEntry::setEventList( const std::vector<MobEventStruct>& p_item )
// {
// 	assert( m_pEventLog );
// 	long lCount = (long)p_item.size();
// 	long lSize = sizeof(MobEventStruct)*lCount;
// 	long lTotal = lSize + m_pEventLog->getPositionForWrite();
// 
// 	item.startPos = m_pEventLog->getPositionForWrite();
// 	if (m_pEventLog->needCreateFile(lTotal))
// 		item.startPos = 0l;
// 
// 	m_pEventLog->setEventList( p_item );
// 	item.endPos = m_pEventLog->getPositionForWrite() - sizeof( MobEventStruct );
// 	item.fileID = m_pEventLog->GetLastFileID();
// }

void MobLogEntry::saveEventsToLog()
{
	assert(m_pEventLog);

	long lCount = (long)m_vPaxEvenList.size();
	long lSize = sizeof(MobEventStruct)*lCount;
	long lTotal = lSize + m_pEventLog->getPositionForWrite();

	item.startPos = m_pEventLog->getPositionForWrite();
	if (m_pEventLog->needCreateFile(lTotal))
		item.startPos = 0l;
	
	m_pEventLog->setEventList( m_vPaxEvenList );
	item.endPos = m_pEventLog->getPositionForWrite() - sizeof( MobEventStruct );
	item.fileID = m_pEventLog->GetLastFileID();
}

void MobLogEntry::setToFirst( void )
{
	assert( m_pEventLog );
	EventLog<MobEventStruct>* pSingleEventlog = NULL;
	if (item.fileID != -1)
	{
		pSingleEventlog = m_pEventLog->GetEventLog(item.fileID);
	}
	else
	{
		pSingleEventlog = m_pEventLog->GetEventLog();
	}
	
	assert(pSingleEventlog);

	pSingleEventlog->setPosition ((item.startPos != -1)? item.startPos: 0);
}

MobEventStruct MobLogEntry::getNextEvent( void )
{
	assert( m_pEventLog );
	EventLog<MobEventStruct>* pSingleEventlog = NULL;
	if (item.fileID != -1)
	{
		pSingleEventlog = m_pEventLog->GetEventLog(item.fileID);
	}
	else
	{
		pSingleEventlog = m_pEventLog->GetEventLog();
	}

	assert(pSingleEventlog);
	return pSingleEventlog->getNextEvent();
}

MobEventStruct MobLogEntry::getEvent( long p_ndx )
{
	if( m_vPaxEvenList.size() > 0 )
		return *(m_vPaxEvenList[p_ndx]);

	assert( m_pEventLog );
	EventLog<MobEventStruct>* pSingleEventlog = NULL;
	if (item.fileID != -1)
	{
		pSingleEventlog = m_pEventLog->GetEventLog(item.fileID);
	}
	else
	{
		pSingleEventlog = m_pEventLog->GetEventLog();
	}

	assert(pSingleEventlog);
	pSingleEventlog->setPosition (item.startPos + p_ndx * sizeof( MobEventStruct ) );
	return pSingleEventlog->getNextEvent();
}

void MobLogEntry::updateEvent( MobEventStruct& _newEventData, long _lIdx )
{
	if( m_vPaxEvenList.size() > 0 )
	{
		*(m_vPaxEvenList[_lIdx]) = _newEventData;
	}		
	assert( m_pEventLog );
	EventLog<MobEventStruct>* pSingleEventlog = NULL;
	if (item.fileID != -1)
	{
		pSingleEventlog = m_pEventLog->GetEventLog(item.fileID);
	}
	else
	{
		pSingleEventlog = m_pEventLog->GetEventLog();
	}

	assert(pSingleEventlog);
	pSingleEventlog->setPosition (item.startPos + _lIdx * sizeof( MobEventStruct ) );
	pSingleEventlog->updateNextEvent( _newEventData );
}

void MobLogEntry::loadEvents( void )
{
	//int nSize = m_vEventList.size();
	int nSize = 1+ ( item.endPos - item.startPos )/ sizeof( MobEventStruct );		
	clearLog();
	assert( m_pEventLog );
	EventLog<MobEventStruct>* pSingleEventlog = NULL;
	if (item.fileID != -1)
	{
		pSingleEventlog = m_pEventLog->GetEventLog(item.fileID);
	}
	else
	{
		pSingleEventlog = m_pEventLog->GetEventLog();
	}

	assert(pSingleEventlog);
	pSingleEventlog->setPosition (item.startPos);
	//setToFirst();
	MobEventStruct* pEventList = new MobEventStruct[nSize];
	assert( m_pEventLog );
	pSingleEventlog->getEvents ( pEventList, nSize);
	for( int i=0; i<nSize; i++ )
	{
		MobEventStruct* pNewItem = new MobEventStruct(pEventList[i]);
		m_vPaxEvenList.push_back( pNewItem );
	}
	delete [] pEventList;
}

const std::vector<MobEventStruct*>& MobLogEntry::getMobEventList( void ) const
{
	return m_vPaxEvenList;
}

std::vector<MobEventStruct*>& MobLogEntry::getMobEventList( void )
{
	return m_vPaxEvenList;
}

void MobLogEntry::addEvent( const MobEventStruct &p_item )
{

	m_preEvent = p_item;
	m_preEvent.elementID = item.id;

	MobEventStruct* pNewItem = new MobEventStruct(m_preEvent);

	//if(m_preEvent.elementID == 0 ||
	//	m_preEvent.elementID == 18 )
	//{
	//	int nID = m_preEvent.elementID;
	//	ElapsedTime eTime;
	//	eTime.setPrecisely(m_preEvent.time);
	//	TRACE(_T("\r\n %d , time = %s(%d),X= %f, Y = %f, Z= %f,    %f \r\n"),m_preEvent.elementID, eTime.printTime(1), m_preEvent.time,m_preEvent.x, m_preEvent.y, m_preEvent.z, m_preEvent.m_RealZ);
	//}

	//////if(m_preEvent.time >= 4*60*60*100)
	////{
	////	//if(m_preEvent.elementID == 506 )
	////	{
	////	
	////		int nMobElemID = m_preEvent.elementID;
	////		CString strLogFile;
	////		strLogFile.Format(_T("%sPaxEntry%d.log"), _T("F:\\landsidedebug\\"), 111);

	////		ofsstream echoFile (strLogFile.GetBuffer(0), stdios::app);
	////		echoFile	
	////			<< m_preEvent.time<<", "
	////			<<m_preEvent.elementID<<", "
	////			<<m_preEvent.x <<", "
	////			<<m_preEvent.y <<", "		
	////			<<m_preEvent.z <<", "
	////			<<m_preEvent.m_RealZ <<", "
	////			<<(int)m_preEvent.m_IsRealZ<<", "
	////			<<m_preEvent.state<<", "
	////			<<ElapsedTime(m_preEvent.time/100.0).PrintDateTime()
	////			<<"\r\n";

	////			echoFile.close();
	////	}
	////}


	
	if(m_pMobTempLogFile)
		m_pMobTempLogFile->addEvent(pNewItem);
	//m_vPaxEvenList.push_back( pNewItem );
}

int MobLogEntry::getLog( MobEventStruct* &p_log )
{
	int retVal = 1;
	// no file buffer used, copy memory buffer
	int nSize = (int)m_vPaxEvenList.size();
	if( nSize > 0 )
	{
		if (!p_log)
			p_log = new MobEventStruct[nSize];
		for( int i=0; i<nSize; i++ )
			p_log[i] = *(m_vPaxEvenList[i]);
	}

	return retVal;
}

void MobLogEntry::clearLog( void )
{
	LogItem<MobDescStruct,MobEventStruct>::clearLog();

	for (size_t i = 0; i < m_vPaxEvenList.size(); i++)
	{
		delete m_vPaxEvenList[i];
	}
	m_vPaxEvenList.clear();
}

void MobLogEntry::initLog( void )
{
	clearLog();
}

long MobLogEntry::getCurrentCount( void ) const
{
	return (long)m_vPaxEvenList.size();
}

MobEventStruct* MobLogEntry::getPreEvent()
{
	if(m_preEvent.elementID < 0 || (m_preEvent.x == float(0) && m_preEvent.y == float(0)) )
		return NULL;
	return &m_preEvent;
}


