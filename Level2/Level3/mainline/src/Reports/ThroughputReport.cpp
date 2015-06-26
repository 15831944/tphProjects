// ThroughtputReport.cpp: implementation of the CThroughputReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "rep_pax.h"
#include "ThroughputReport.h"
#include "..\common\termfile.h"
#include "..\common\states.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThroughputReport::CThroughputReport(Terminal* _pTerm, const CString& _csProjPath)
:CBaseReport(_pTerm, _csProjPath)
{

}

CThroughputReport::~CThroughputReport()
{

}

void CThroughputReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	//m_multiConst.freeConstraints();
	//int iSize = _pPara->Thrognput_pax.size();
	//for( int i=0; i<iSize; ++i )
	//{
	//	m_multiConst.addConstraint( _pPara->Thrognput_pax[i] );
	//}
	m_usesInterval = 1;
}



void CThroughputReport::GenerateSummary( ArctermFile& p_file )
{
	p_file.writeField ("Processor,Group Size,Total Pax,Avg Pax,Total / Hour,Avg / Hour");
	p_file.writeLine();

	int nProcCount = m_procIDlist.getCount();
	if(nProcCount>0)
	{
		
		CProgressBar bar( "Generating Throughput Report", 100, m_procIDlist.getCount(), TRUE );
		for (int i = 0; i < m_procIDlist.getCount(); i++)
		{
			bar.StepIt();
			clear();
			const ProcessorID *procID = m_procIDlist.getID(i);
			getAverageThroughput (procID);
			writeAverage (procID, p_file);
		}
		return;
	}
	//generate all processors
	CProgressBar bar( "Generating Throughput Report", 100, 1, TRUE );
	clear();
	ProcessorID defaultAll;
	defaultAll.SetStrDict(m_pTerm->inStrDict);
	getAverageThroughput (&defaultAll);
	writeAverage (&defaultAll, p_file);

	
}


void CThroughputReport::GenerateDetailed( ArctermFile& p_file )
{
	p_file.writeField ("Processor,Start Time,End Time,Pax Served");
	p_file.writeLine();

	int nProcCount = m_procIDlist.getCount();

	if( nProcCount > 0 )
	{
		CProgressBar bar( "Generating Throughput  Report", 100, nProcCount, TRUE );

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
	CProgressBar bar( "Generating Throughput  Report", 100, nProcCount, TRUE );
	if(nProcCount >= 500)//it will crash while drawing graph
	{
		MessageBox(NULL, _T("The number of processors selected cannot exceed 500. Please, reduce the amount of selected processors.\r\nNote that a family can have many processors."), _T("Throughput Report"), MB_OK);
		return;
	}
	for (int i = 0; i < nProcCount; i++)
	{
		bar.StepIt();
		m_pTerm->procLog->getItem (logEntry, i);
		logEntry.getID (str);
		id.setID (str);
		calculateThroughput( &id, p_file );		
	}
}


void CThroughputReport::getAverageThroughput (const ProcessorID *p_id)
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

void CThroughputReport::writeAverage (const ProcessorID *id,
									  ArctermFile& p_file) const
{
	float hours = (float)(m_endTime - m_startTime).asHours();

	char str[MAX_PROC_ID_LEN];
	id->printID (str);

	p_file.writeField (str);
	p_file.writeInt (m_nCount);
	p_file.writeInt (m_lPassengersServed);
	p_file.writeFloat ((float)(m_lPassengersServed / m_nCount), 2);
	p_file.writeFloat (m_lPassengersServed / hours, 2);
	p_file.writeFloat (m_lPassengersServed / m_nCount / hours, 2);
	p_file.writeLine();
}

void CThroughputReport::writeEntry (const char *p_id, ArctermFile& p_file) const
{
	float hours = (float)(m_endTime - m_startTime).asHours();
	ElapsedTime timePeriod (m_endTime - m_startTime);

	p_file.writeField (p_id);
	p_file.writeInt (m_lPassengersServed);
	p_file.writeFloat (m_lPassengersServed / hours, 2);
	p_file.writeTime (timePeriod / m_lPassengersServed, 1);
	p_file.writeLine();
}



// given processor/group id, write the result to file.
void CThroughputReport::calculateThroughput( ProcessorID* _pProcID, ArctermFile& p_file )
{
	CSpaceThroughtputResult tempResult;
	int nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
	tempResult.InitItem( m_startTime , m_interval , nIntervalCount );

	GetResult( tempResult,  _pProcID );

	int nResultItemCount = tempResult.GetResultItemCount();
	for( int i=0 ; i< nResultItemCount ; ++i )
	{
		char szID[512] = {0};
		_pProcID->printID( szID );
		p_file.writeField( szID );
		THROUGHTOUTITEM tempItem = tempResult[ i ];
		p_file.writeTime (tempItem.m_startTime);
		p_file.writeTime(tempItem.m_endTime);
		p_file.writeInt (tempItem.m_lThroughtout);
		p_file.writeLine();				
	}	
}

// given processorid and get all the resuld into _result.
void CThroughputReport::GetResult( CSpaceThroughtputResult& _result,  ProcessorID* _pProcID )
{
	int nProcCount = m_pTerm->procLog->getCount();
	ProcLogEntry logEntry;
	logEntry.SetOutputTerminal( m_pTerm );
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	CProgressBar bar( "Calculating Processor Throughtput :", 100, nProcCount, TRUE, 1 );

	//get paxlog ,and then find all its events ,match the event that pass the processor,
	//count it.
	ReportPaxEntry element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	long lPaxCount = m_pTerm->paxLog->getCount();
	for (long lPax = 0; lPax < lPaxCount; lPax++)
	{
		m_pTerm->paxLog->getItem (element, lPax);//pax element
		element.clearLog();

		if(!element.alive(m_startTime,m_endTime))//not alive between time range.
			continue;
        if(element.fits(m_multiConst) == 0) 
			continue ;
		MobEventStruct event;
		element.setToFirst();
		long lEventCount = element.getCount();//pax element's all events
		for (long lEvent = 0; lEvent < lEventCount; lEvent++)
		{
			event = element.getNextEvent();
			if(!(event.time > (long)m_startTime && event.time < (long)m_endTime && event.state == ArriveAtServer))
				continue;
			for( int nProc = 0; nProc < nProcCount; nProc++ )
			{
				m_pTerm->procLog->getItem( logEntry, nProc );
				if (logEntry.getIndex() != event.procNumber) //not match
					continue;
				
				int id[MAX_PROC_IDS];
				logEntry.getIDs( id );
				ProcessorID procID;
				procID.copyIDlist( id );
				if( _pProcID->idFits( procID ))
				{//satisfy condition,and count
					bar.StepIt();
					_result.IncreaseCount( (ElapsedTime)( event.time / TimePrecision ));
				}
			}
		}
	}	
}

void CThroughputReport::calculateThroughput (ProcLogEntry& logEntry)
{
	m_nCount++;
	logEntry.SetOutputTerminal( m_pTerm );
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);

	ReportPaxEntry element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	long lPaxCount = m_pTerm->paxLog->getCount();
	for (long lPax = 0; lPax < lPaxCount; lPax++)
	{
		m_pTerm->paxLog->getItem (element, lPax);
		element.clearLog();

		if(!element.alive(m_startTime,m_endTime))//not alive between time range.
			continue;

		if(element.fits(m_multiConst) == 0) 
			continue ;

		MobEventStruct event;
		element.setToFirst();
		long lEventCount = element.getCount();
		for (long lEvent = 0; lEvent < lEventCount; lEvent++)
		{
			event = element.getNextEvent();
			if(!(event.time > (long)m_startTime && event.time < (long)m_endTime && event.state == ArriveAtServer))
				continue;
			if (logEntry.getIndex() == event.procNumber)
			{
				m_lPassengersServed++;
			}
		}
	}	
}
