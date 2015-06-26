#include "StdAfx.h"

#include "AirsideFlightLog.h"

#define  LOGVERSION 701

AirsideFlightLog::AirsideFlightLog(void):BaseLog<AirsideFlightDescStruct, AirsideFlightEventStruct, AirsideFlightLogEntry>(LOGVERSION)
{
}

AirsideFlightLog::AirsideFlightLog( EventLog<AirsideFlightEventStruct>* pAirsideEventLog ) : BaseLog<AirsideFlightDescStruct, AirsideFlightEventStruct, AirsideFlightLogEntry>(LOGVERSION)
{
	eventLog = pAirsideEventLog;
}
AirsideFlightLog::~AirsideFlightLog(void)
{
}


long AirsideFlightLog::getRangeCount (ElapsedTime pstart, ElapsedTime pend)
{
	long nCount = 0;
/*	AirsideFlightLogEntry entry;
	int count = getCount();
	for (long i = 0; i < count; i++)
	{
		getItem (entry, i);
		if (entry.alive (pstart, pend) && entry.getEntryTime() < entry.getExitTime())
			nCount++;
	}*/
	return nCount;
}

ElapsedTime AirsideFlightLog::getMinTime (void)
{
	AirsideFlightLogEntry entry;
	getItem (entry, 0);
	return entry.getEntryTime();
}

ElapsedTime AirsideFlightLog::getMaxTime (void)
{
	ElapsedTime maxTime (0l);
	AirsideFlightLogEntry entry;
	int count = getCount();
	for (long i = 0; i < count; i++)
	{
		getItem (entry, i);
		if (entry.getExitTime() > maxTime)
			maxTime = entry.getExitTime();
	}
	return maxTime;
}

bool AirsideFlightLog::GetItemByID( AirsideFlightLogEntry& _entry , long _lID )
{
	int count = getCount();	
	for (long i = 0; i < count; i++)
	{
		getItem (_entry, i);
		if( _entry.GetAirsideDesc().id == _lID )
		{
			return true;
		}			
	}

	return false;
}

// Need to sort the event file
// Event log was store to the file by order of dead time.
// need to sort the event log by birth time, which is the index of pax desc.
void AirsideFlightLog::SaveEventToFile(const char *pfilename)
{
	// source file.
	if( !eventLog )
		return;

	eventLog->closeEventFile();
	int nCount = getCount();
	/*if( nCount < 400000 )*/
		return;

	fsstream* pSrcEventFile = new fsstream (pfilename, stdios::binary | stdios::in | stdios::out);
	if( !pSrcEventFile || pSrcEventFile->bad() )
		return;	

	// dest file.
	CString csTempFileName = "SortEventLog.bin";		
	fsstream* pDestEventFile = new fsstream( csTempFileName, stdios::out|stdios::binary );
	if( !pDestEventFile  || pDestEventFile->bad() )
		return;

	CProgressBar bar( "Sorting Logs" );
	for( int i = 0; i<  nCount; i++ )
	{
		/*if( i % ( nCount/ 100 ) == 0 )
			bar.StepIt();*/
		AirsideFlightLogEntry entry;
		getItem( entry, i );
		AirsideFlightDescStruct item = entry.GetAirsideDescStruct();
		if(item.endPos< item.startPos) continue;
		// read source file
		int nEventSize = ( item.endPos - item.startPos ) / sizeof(AirsideFlightEventStruct) + 1;
		pSrcEventFile->seekg( item.startPos, stdios::beg);
		AirsideFlightEventStruct* pEventList = new AirsideFlightEventStruct[nEventSize];
		pSrcEventFile->read( (char *)pEventList, nEventSize * sizeof (AirsideFlightEventStruct));

		item.startPos = static_cast<long>(pDestEventFile->tellg());
		pDestEventFile->write( (char *)pEventList, nEventSize * sizeof (AirsideFlightEventStruct) );
		if( pDestEventFile->bad() )
		{
			return;
		}
		item.endPos = static_cast<int>(pDestEventFile->tellg());
		item.endPos -= sizeof( AirsideFlightEventStruct );
		delete [] pEventList;

		// update the start / end pos
		updateItem( item, i );
	}

	delete pDestEventFile;
	delete pSrcEventFile;

	// copy the temp file to the eventLog
	FileManager fileMan;	
	fileMan.CopyFile( csTempFileName, pfilename );
	fileMan.DeleteFile( csTempFileName );
}


