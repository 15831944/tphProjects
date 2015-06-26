#include "StdAfx.h"
#include ".\airsidevehiclelog.h"

AirsideVehicleLog::AirsideVehicleLog(void)
{
}

AirsideVehicleLog::~AirsideVehicleLog(void)
{
}

long AirsideVehicleLog::getRangeCount (ElapsedTime pstart, ElapsedTime pend)
{
	long nCount = 0;

	return nCount;
}

ElapsedTime AirsideVehicleLog::getMinTime (void)
{
	AirsideVehicleLogEntry entry;
	getItem (entry, 0);
	return entry.getEntryTime();
}

ElapsedTime AirsideVehicleLog::getMaxTime (void)
{
	ElapsedTime maxTime (0l);
	AirsideVehicleLogEntry entry;
	int count = getCount();
	for (long i = 0; i < count; i++)
	{
		getItem (entry, i);
		if (entry.getExitTime() > maxTime)
			maxTime = entry.getExitTime();
	}
	return maxTime;
}

bool AirsideVehicleLog::GetItemByID( AirsideVehicleLogEntry& _entry , long _lID )
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
void AirsideVehicleLog::SaveEventToFile(const char *pfilename)
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
	CString csTempFileName = "SortVehicleEventLog.bin";		
	fsstream* pDestEventFile = new fsstream( csTempFileName, stdios::out|stdios::binary );
	if( !pDestEventFile  || pDestEventFile->bad() )
		return;

	CProgressBar bar( "Sorting Logs" );
	for( int i = 0; i<  nCount; i++ )
	{
		/*if( i % ( nCount/ 100 ) == 0 )
		bar.StepIt();*/
		AirsideVehicleLogEntry entry;
		getItem( entry, i );
		AirsideVehicleDescStruct item = entry.GetAirsideDesc();
		if(item.endPos< item.startPos) continue;
		// read source file
		int nEventSize = ( item.endPos - item.startPos ) / sizeof(AirsideVehicleEventStruct) + 1;
		pSrcEventFile->seekg( item.startPos, stdios::beg);
		AirsideVehicleEventStruct* pEventList = new AirsideVehicleEventStruct[nEventSize];
		pSrcEventFile->read( (char *)pEventList, nEventSize * sizeof (AirsideVehicleEventStruct));

		item.startPos = static_cast<long>(pDestEventFile->tellg());
		pDestEventFile->write( (char *)pEventList, nEventSize * sizeof (AirsideVehicleEventStruct) );
		if( pDestEventFile->bad() )
		{
			return;
		}
		item.endPos = static_cast<long>(pDestEventFile->tellg());
		item.endPos -= sizeof( AirsideVehicleEventStruct );
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