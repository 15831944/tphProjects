#include "StdAfx.h"
#include ".\billboardexposureincidencereport.h"
#include "Reports.h"
#include "..\common\termfile.h"
#include "..\common\states.h"
#include "engine\terminal.h"

CBillboardExposureIncidenceReport::CBillboardExposureIncidenceReport(Terminal* _pTerm, const CString& _csProjPath)
:CBaseReport(_pTerm, _csProjPath)
{
}

CBillboardExposureIncidenceReport::~CBillboardExposureIncidenceReport(void)
{
}
void CBillboardExposureIncidenceReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval = 1;
}

void CBillboardExposureIncidenceReport::GenerateSummary( ArctermFile& p_file )
{
	p_file.writeField ("Processor,Group Size,Total Pax,Avg Pax,Total / Hour,Avg / Hour");
	p_file.writeLine();

	const ProcessorID *procID;

	CProgressBar bar( "Generating Billboard Exposure Incidence Report", 100, m_procIDlist.getCount(), TRUE );

	for (int i = 0; i < m_procIDlist.getCount(); i++)
	{
		bar.StepIt();
		clear();
		procID = m_procIDlist.getID(i);
		getAverageThroughput (procID);
		writeAverage (procID, p_file);
	}
}


void CBillboardExposureIncidenceReport::GenerateDetailed( ArctermFile& p_file )
{
	p_file.writeField ("Processor,Start Time,End Time,Pax count Served");
	p_file.writeLine();

	int nProcCount = m_procIDlist.getCount();

	if( nProcCount > 0 )
	{
		CProgressBar bar( "Generating Billboard Exposure Incidence Report", 100, nProcCount, TRUE );

		for (int i = 0; i < nProcCount; i++)
		{
			bar.StepIt();
			ProcessorID* pProcID = m_procIDlist.getItem( i );
			calculateThroughput( pProcID, p_file );		
		}
		return;
	}


	// all processor
	char str[MAX_PROC_ID_LEN];
	ProcessorID id;
	ProcLogEntry logEntry;
	id.SetStrDict( m_pTerm->inStrDict );
	logEntry.SetOutputTerminal( m_pTerm );
	nProcCount = m_pTerm->procLog->getCount();
	CProgressBar bar( "Generating Billboard Exposure Incidence Report", 100, nProcCount, TRUE );
	for (int i = 0; i < nProcCount; i++)
	{
		bar.StepIt();
		m_pTerm->procLog->getItem (logEntry, i);
		logEntry.getID (str);
		id.setID (str);
		calculateThroughput( &id, p_file );		
	}
}

void CBillboardExposureIncidenceReport::getAverageThroughput (const ProcessorID *p_id)
{
	char str[MAX_PROC_ID_LEN];
	ProcessorID id;
	ProcLogEntry logEntry;
	id.SetStrDict( m_pTerm->inStrDict );
	logEntry.SetOutputTerminal( m_pTerm );
	int procCount = m_pTerm->procLog->getCount();
	for (int i = 0; i < procCount; i++)
	{
		m_pTerm->procLog->getItem (logEntry, i);
		logEntry.getID (str);
		id.setID (str);
		if (p_id->idFits (id))
			calculateThroughput (logEntry);
	}
}

void CBillboardExposureIncidenceReport::writeAverage (const ProcessorID *id,
									  ArctermFile& p_file) const
{
	float hours = (float)(m_endTime - m_startTime).asHours();

	char str[MAX_PROC_ID_LEN];
	id->printID (str);

	p_file.writeField (str);
	//p_file.writeInt (m_nCount);
	//p_file.writeInt (m_lPassengersServed);
	//p_file.writeFloat ((float)(m_lPassengersServed / m_nCount), 2);
	p_file.writeFloat (m_lPassengersServed / hours, 2);
//	p_file.writeFloat (m_lPassengersServed / m_nCount / hours, 2);

//	p_file.writeInt (m_lTotalPaxServed);
//	p_file.writeFloat ((float)(m_lTotalPaxServed / m_nCount), 2);
	p_file.writeFloat (m_lTotalPaxServed / hours, 2);
//	p_file.writeFloat (m_lTotalPaxServed / m_nCount / hours, 2);



	p_file.writeLine();
}

void CBillboardExposureIncidenceReport::writeEntry (const char *p_id, ArctermFile& p_file) const
{
	float hours = (float)(m_endTime - m_startTime).asHours();
	ElapsedTime timePeriod (m_endTime - m_startTime);

	p_file.writeField (p_id);
	//exposure
	p_file.writeInt (m_lPassengersServed);
	p_file.writeFloat (m_lPassengersServed / hours, 2);
	p_file.writeTime (timePeriod / m_lPassengersServed, 1);
	//total
	p_file.writeInt (m_lTotalPaxServed);
	p_file.writeFloat (m_lTotalPaxServed / hours, 2);
	p_file.writeTime (timePeriod / m_lTotalPaxServed, 1);
	p_file.writeLine();
}



// given processor/group id, write the result to file.
void CBillboardExposureIncidenceReport::calculateThroughput( ProcessorID* _pProcID, ArctermFile& p_file )
{
	CBillboardThroughputResult tempResult;
	int nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
	tempResult.InitItem( m_startTime , m_interval , nIntervalCount );

	GetResult( tempResult,  _pProcID );

	int nResultItemCount = tempResult.GetResultItemCount();
	for( int i=0 ; i< nResultItemCount ; ++i )
	{
		char szID[512];
		_pProcID->printID( szID );
		p_file.writeField( szID );
		BillboardThroughputItem tempItem = tempResult[ i ];
		p_file.writeTime (tempItem.m_startTime);
		p_file.writeTime(tempItem.m_endTime);
		p_file.writeInt (tempItem.m_lExposureCount);
		p_file.writeInt(tempItem.m_lTotalThroughput);
		p_file.writeLine();				
	}	
}


// given processorid and get all the resuld into _result.
void CBillboardExposureIncidenceReport::GetResult( CBillboardThroughputResult& _result,  ProcessorID* _pProcID )
{
	int nProcCount = m_pTerm->procLog->getCount();
	ProcLogEntry logEntry;
	logEntry.SetOutputTerminal( m_pTerm );
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	CProgressBar bar( "Calculating Processor Throughtput :", 100, nProcCount, TRUE, 1 );
	for( int i = 0; i < nProcCount; i++ )
	{
		bar.StepIt();
		m_pTerm->procLog->getItem( logEntry, i );
		int id[MAX_PROC_IDS];
		logEntry.getIDs( id );

		ProcessorID procID;
		procID.copyIDlist( id );

		// TRACE("\n   %d - %d - %d - %d", id[0], id[1], id[2], id[3]);

		if( _pProcID->idFits( procID ) )
		{
			ProcEventStruct event;
			logEntry.setToFirst();
			long eventCount = logEntry.getCount();
			for (long i = 0; i < eventCount; i++)
			{
				event = logEntry.getNextEvent();
				if (event.time >= (long)m_startTime && event.time <= (long)m_endTime)
					if (event.type == BeginService )
					{
						_result.IncreaseCount( (ElapsedTime)( event.time / TimePrecision ),1 );
					}
					else if (event.type ==WalkThroughBillboard )
					{
						_result.IncreaseCount( (ElapsedTime)( event.time / TimePrecision ),0);					
					}
			}

		}
	}	
}



void CBillboardExposureIncidenceReport::calculateThroughput (ProcLogEntry& logEntry)
{
	m_nCount++;


	// calculate passenger served
	ProcEventStruct event;
	logEntry.SetOutputTerminal( m_pTerm );
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	logEntry.setToFirst();
	long eventCount = logEntry.getCount();
	for (long i = 0; i < eventCount; i++)
	{
		event = logEntry.getNextEvent();
		if (event.time >= (long)m_startTime && event.time <= (long)m_endTime)
			if (event.type == WalkThroughBillboard)
				m_lPassengersServed++;
			else if (event.type == BeginService)
				m_lTotalPaxServed++;
				
	}
}
