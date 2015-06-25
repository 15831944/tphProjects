#include "stdafx.h"
#include "results\paxlog.h"
#include "common\containr.h"
#include "../Common/ARCTracker.h"

void PaxLog::echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm )
{

    ofsstream echoFile (p_filename);
    echoFile << p_filename << '\n';
    echoFile << "Log Echo File\n";
    echoFile << echoTitles() << '\n';
    MobLogEntry logEntry;
	logEntry.SetOutputTerminal( _pOutTerm );
	int nCount = getCount();
	for( int i=0; i<nCount; i++ )
    {
        getItem (logEntry, i);
        logEntry.echo (echoFile, _csProjPath);
    }
    echoFile.close();
}

void PaxLog::sortPassengers (void)
{
	int count = getCount();
    if (!count)
        return;

    SortedContainer<MobLogEntry> elementArray (count, 0, 512);
    elementArray.ownsElements(1);

	m_vPaxIndexTable.clear();
	m_vPaxIndexTable.reserve( count );
	m_mapNewIdxToOldIdx.clear();
	

    MobLogEntry *entry;
    for (long i = 0; i < count; i++)
    {		
        entry = new MobLogEntry;
        getItem (*entry, i);
		
		PAXINDEXTABLE tempIndex;
		tempIndex.lID = entry->getID();
		tempIndex.lOwnerId = entry->getOwnerID();
		tempIndex.iGroupMember = entry->getGroupMember();
		tempIndex.iIndexInLog = i;
		tempIndex.iGroupSize = entry->getGroupSize();
		m_vPaxIndexTable.push_back( tempIndex );
		
		entry->setIndex( i );
        elementArray.addItem (entry);
		
    }

    MobDescStruct tStruct;
    for (long i = 0; i < count; i++)
    {
        entry = elementArray.getItem(i);
        entry->initStruct (tStruct);
		m_mapNewIdxToOldIdx[ i ] = tStruct.indexNum;
		m_vPaxIndexTable[ tStruct.indexNum ].iIndexInLog = i;
        tStruct.indexNum = i;
		updateItem( tStruct, i );
    }
    elementArray.clear();
}

void PaxLog::sortPassengers(CProgressBar& bar)
{
	PLACE_METHOD_TRACK_STRING();
	int count = getCount();
    if (!count)
        return;
	bar.SetText("Sort Passengers");
	bar.SetPos(0);
	SortedContainer<MobLogEntry> elementArray (count, 0, 512);
    elementArray.ownsElements(1);
	
	m_vPaxIndexTable.clear();
	m_vPaxIndexTable.reserve( count );
	m_mapNewIdxToOldIdx.clear();
	bar.SetRange(0,count*2);
    MobLogEntry *entry;
    for (long i = 0; i < count; i++)
    {		
        entry = new MobLogEntry;
        getItem (*entry, i);
		PAXINDEXTABLE tempIndex;
		tempIndex.lID = entry->getID();
		tempIndex.lOwnerId = entry->getOwnerID();
		tempIndex.iGroupMember = entry->getGroupMember();
		tempIndex.iIndexInLog = i;
		tempIndex.iGroupSize = entry->getGroupSize();
		m_vPaxIndexTable.push_back( tempIndex );		
		entry->setIndex( i );
        elementArray.addItem (entry);
		bar.StepIt();
    }
	
    MobDescStruct tStruct;
    for (long i = 0; i < count; i++)
    {
        entry = elementArray.getItem(i);
        entry->initStruct (tStruct);
		m_mapNewIdxToOldIdx[ i ] = tStruct.indexNum;
		m_vPaxIndexTable[ tStruct.indexNum ].iIndexInLog = i;
        tStruct.indexNum = i;
		updateItem( tStruct, i );
		bar.StepIt();				
    }
    elementArray.clear();	
}


long PaxLog::getRangeCount (ElapsedTime p_start, ElapsedTime p_end)
{
    long paxCount = 0;
    MobLogEntry entry;
	int count = getCount();
    for (long i = 0; i < count; i++)
    {
        getItem (entry, i);
        if (entry.alive (p_start, p_end) && entry.getEntryTime() <
                entry.getExitTime())
            paxCount++;
    }
    return paxCount;
}

ElapsedTime PaxLog::getMinTime (void)
{
    MobLogEntry entry;
    getItem (entry, 0);
    return entry.getEntryTime();
}

ElapsedTime PaxLog::getMaxTime (void)
{
    ElapsedTime maxTime (0l);
    MobLogEntry entry;
	int count = getCount();
    for (long i = 0; i < count; i++)
    {
        getItem (entry, i);
        if (entry.getExitTime() > maxTime)
            maxTime = entry.getExitTime();
    }
    return maxTime;
}
// get all pax's attachment 's index in pax log
void PaxLog::GetAttachments( std::vector<long>& _vVisIndex, long _ownerID ,long _lPaxIndex )
{
	int count = m_vPaxIndexTable.size();	
    ASSERT( _lPaxIndex >=0 && _lPaxIndex < count );
	int iOldIndex = m_mapNewIdxToOldIdx[ _lPaxIndex ];
	ASSERT( iOldIndex >=0 && iOldIndex < count );	
	
    for (long i = iOldIndex+m_vPaxIndexTable[iOldIndex].iGroupSize ; i < count; i++)
    {				
        if( m_vPaxIndexTable[i].lOwnerId != _ownerID )
		{
			return;
		}

		if( m_vPaxIndexTable[i].iGroupMember == 0 )
		{
			_vVisIndex.push_back( m_vPaxIndexTable[i].iIndexInLog );
		}	
		
    }
}
bool PaxLog::GetItemByID( MobLogEntry& _entry , long _lID )
{
	int count = getCount();	
    for (long i = 0; i < count; i++)
    {
        getItem (_entry, i);
		if( _entry.getID() == _lID )
		{
			return true;
		}			
    }

	return false;
}


// Need to sort the event file
// Event log was store to the file by order of dead time.
// need to sort the event log by birth time, which is the index of pax desc.
void PaxLog::SaveEventToFile( const char *p_filename )
{
/*
#ifdef _DEBUG
	// debug procdesc.log file.
	char printstr[6][255];

	
//	typedef struct
//	{
//		....
//	} MobDescStruct;
//
	static int flg = 0;
	if(strstr(p_filename,"paxevent.log")!=NULL)
		//if(!flg)
		{
			flg ++;
			int co = m_vLogList.size();
			FILE *fw = fopen("c:\\debugtime.txt", "a+");	
			fprintf(fw, "\n\n#############################\n%d\n#############################\n\n",
				flg);

			for( int i = 0; i < co; i++ )
			{
				fprintf(fw, "\n\n-----[[[[%d]]]]-------", i);

				ElapsedTime sttime(m_vLogList[i].startTime/100l);
				ElapsedTime endtime(m_vLogList[i].endTime/100l);
				sprintf(printstr[0], "\nid=%d,starttime=%s, endtime=%s,\n startpos=%d, endpos=%d, mobileType=%c, intrinsic=%c",
					m_vLogList[i].id, sttime.printTime(), endtime.printTime(),
					m_vLogList[i].startPos, m_vLogList[i].endPos, 
					m_vLogList[i].mobileType, m_vLogList[i].intrinsic);
				sprintf(printstr[1], "\npaxType[0]=%d, paxType[1]=%d", 
					m_vLogList[i].paxType[0], m_vLogList[i].paxType[1]);
				sprintf(printstr[2], "\n arrFlight=%d, depFlight=%d, groupSize=%d, groupMember=%d", 
					m_vLogList[i].arrFlight, m_vLogList[i].depFlight, m_vLogList[i].groupSize, m_vLogList[i].groupMember);
				sprintf(printstr[3], "\nbagcount=%d, cartcount=%d, ownerID=%d, indexNum=%d",
					m_vLogList[i].bagCount, m_vLogList[i].cartCount, m_vLogList[i].ownerID, m_vLogList[i].indexNum);
				sprintf(printstr[4], "\n ownStart=%d, eachPaxOwnNoPax=%d, bMissFlightDeath=%d",
					m_vLogList[i].ownStart, m_vLogList[i].eachPaxOwnNoPax, m_vLogList[i].bMissFlightDeath );
				for(int j = 0; j <= 4; j++)
				{
					fprintf(fw, "%s", printstr[j]);
				}				
			}

			fclose(fw);
		}
#endif
*/
	// source file.
	if( !eventLog )
		return;

	eventLog->closeEventFile();
	int nPaxCount = getCount();
	//if( nPaxCount < 400000 )
		return;
	
	fsstream* pSrcEventFile = new fsstream (p_filename, stdios::binary | stdios::in | stdios::out);
	if( !pSrcEventFile || pSrcEventFile->bad() )
		return;	

	// dest file.
	CString csTempFileName = "SortEventLog.bin";		
	fsstream* pDestEventFile = new fsstream( csTempFileName, stdios::out|stdios::binary );
	if( !pDestEventFile  || pDestEventFile->bad() )
		return;
	
	CProgressBar bar( "Sorting Logs" );
	for( int i=0; i<nPaxCount; i++ )
	{
		if( i % ( nPaxCount/ 100 ) == 0 )
			bar.StepIt();
		MobLogEntry entry;
		getItem( entry, i );
		MobDescStruct item = entry.GetMobDesc();
		
		// read source file
		int nEventSize = ( item.endPos - item.startPos ) / sizeof(MobEventStruct) + 1;
		pSrcEventFile->seekg( item.startPos, stdios::beg);
        MobEventStruct* pEventList = new MobEventStruct[nEventSize];
		pSrcEventFile->read( (char *)pEventList, nEventSize * sizeof (MobEventStruct));

		item.startPos = static_cast<long>(pDestEventFile->tellg());
		pDestEventFile->write( (char *)pEventList, nEventSize * sizeof (MobEventStruct) );
		if( pDestEventFile->bad() )
		{
			return;
		}
		item.endPos = static_cast<long>(pDestEventFile->tellg());
		item.endPos -= sizeof( MobEventStruct );
		delete [] pEventList;

		// update the start / end pos
		updateItem( item, i );
	}
	
	delete pDestEventFile;
	delete pSrcEventFile;

	// copy the temp file to the eventLog
	FileManager fileMan;	
	fileMan.CopyFile( csTempFileName, p_filename );
	fileMan.DeleteFile( csTempFileName );
}

PaxLog::PaxLog( EventLog<MobEventStruct>* _pPaxEventLog, TempMobileEventLog* pTempLog  ) : 
BaseLog<MobDescStruct,MobEventStruct,MobLogEntry>(208)
{
	eventLog = _pPaxEventLog;
	m_pTempLog = pTempLog;
}
