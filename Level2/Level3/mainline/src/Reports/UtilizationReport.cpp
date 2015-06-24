// UtilizationReport.cpp: implementation of the CUtilizationReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "UtilizationReport.h"
#include "common\termfile.h"
#include "results\out_term.h"
#include "common\states.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtilizationReport::CUtilizationReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}


CUtilizationReport::~CUtilizationReport()
{

}


void CUtilizationReport::GenerateSummary( ArctermFile& p_file )
{
	p_file.writeField ("Processor,Group Size,Schedule,Overtime,Actual,Service,Idle,Utilization");
	p_file.writeLine();



	const ProcessorID *procID;

	for (int i = 0; i < m_procIDlist.getCount(); i++)
	{
		Clear();
		procID = m_procIDlist.getID(i);
		GetAverageUtilization (procID);
		WriteAverages (procID, p_file);
	}
}


void CUtilizationReport::GenerateDetailed( ArctermFile& p_file )
{
	p_file.writeField ("Processor,Schedule,Overtime,Actual,Service,Idle,Utilization");
	p_file.writeLine();

	char str[MAX_PROC_ID_LEN];
	ProcLogEntry logEntry;
	logEntry.SetOutputTerminal( m_pTerm );
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	int procCount = m_pTerm->procLog->getCount();

	CProgressBar bar( "Generating Detailed Report", 100, procCount, TRUE );

	for (int i = 0; i < procCount; i++)
	{
		bar.StepIt();
		m_pTerm->procLog->getItem (logEntry, i);
		if (ProcFitsSpecs (logEntry) && logEntry.getProcType() != HoldAreaProc)
		{
			Clear();
			CalculateUtilization (logEntry);
			logEntry.getID (str);
			WriteEntries (str, p_file);
		}
	}
}


void CUtilizationReport::Clear()
{
	m_dScheduledTime = 0.0;
	m_dOverTime = 0.0;
	m_dActualTime = 0.0;
	m_dServiceTime = 0.0;
	m_count = 0; 
}


enum ProcessorStates { Open, Overtime, Closed };

void CUtilizationReport::CalculateUtilization( ProcLogEntry& logEntry )
{
	int state = Open;
	int paxCount = 0;
	ElapsedTime startService, curTime, overTimeStart, openTime (m_startTime);

	m_count++;


	// calculate time available
	ProcEventStruct event;
	logEntry.setToFirst();
	long eventCount = logEntry.getCount();
	
	char title[80] = "Calculating Utilization for Proc ";
	logEntry.getID (title + strlen (title));
	CProgressBar bar( title, 100, eventCount, TRUE, 1 );

	long temp = -1;
	for (long i = 0; i < eventCount; i++)
	{
		bar.StepIt();
		event = logEntry.getNextEvent();
		if (event.time > (long)m_startTime)
			curTime.setPrecisely (event.time);
		else
			continue;
			//curTime = m_startTime;

		if (curTime > m_endTime)
			break;

		if (event.type == OpenForService && state == Closed)
		{
			state = Open;
			openTime = curTime;
		}
		else if (event.type == StartToClose)
		{
			state = Overtime;
			overTimeStart = curTime;
			//m_dScheduledTime += ( overTimeStart - openTime ).asSeconds();
		}
		else if (event.type == CloseForService && state != Closed)
		{
			//m_dActualTime += ( curTime - openTime ).asSeconds();
			if (state == Overtime)
				m_dOverTime += ( curTime - overTimeStart ).asSeconds();
			//else
				//m_dScheduledTime += ( curTime - openTime ).asSeconds();

			state = Closed;
		}
		else if (event.type == BeginService)
		{
			if (!paxCount && event.element >= 0)
			{
				startService = curTime;
			}
			if(event.element >= 0)
				paxCount++;

		}
		else if (event.type == CompleteService)
		{
			if(paxCount > 0)
			{
				paxCount--;

				//if (!paxCount)
				if (paxCount == 0)
					m_dServiceTime += ( curTime - startService ).asSeconds();
			}

		}
	}	
	m_dActualTime += ( m_endTime - openTime ).asSeconds();
	if (state == Overtime)
	{
		m_dOverTime += ( m_endTime - overTimeStart ).asSeconds();
		m_dScheduledTime += ( m_endTime - openTime ).asSeconds();
	}
	else
		m_dScheduledTime += ( m_endTime - openTime ).asSeconds();

	if (paxCount > 0)
		m_dServiceTime += ( m_endTime - startService ).asSeconds();


}


void CUtilizationReport::WriteEntries( const char *p_id, ArctermFile& p_file ) const
{
	p_file.writeField (p_id);
	p_file.writeTime ( ElapsedTime( m_dScheduledTime ), TRUE);
	p_file.writeTime ( ElapsedTime( m_dOverTime ), TRUE);
	p_file.writeTime ( ElapsedTime( m_dActualTime ), TRUE);
	p_file.writeTime ( ElapsedTime( m_dServiceTime ), TRUE);
	p_file.writeTime ( ElapsedTime( m_dActualTime - m_dServiceTime ), TRUE);

	char str[64];
	double utilization = m_dServiceTime / m_dActualTime;
	sprintf( str, "% 5.2f", utilization * 100.0 );
	p_file.writeField (str);

	p_file.writeLine();

}


void CUtilizationReport::GetAverageUtilization (const ProcessorID *p_id)
{
	char str[MAX_PROC_ID_LEN];
	ProcessorID id;
	ProcLogEntry logEntry;
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	int procCount = m_pTerm->procLog->getCount();
	id.SetStrDict( m_pTerm->inStrDict );

	CProgressBar bar( "Generating Utilization Summary Report", 100, procCount, TRUE );

	for (int i = 0; i < procCount; i++)
	{
		m_pTerm->procLog->getItem (logEntry, i);
		logEntry.SetOutputTerminal( m_pTerm );
		logEntry.getID (str);
		id.setID (str);
		if (p_id->idFits (id))
			CalculateUtilization (logEntry);

		bar.StepIt();
	}

}


void CUtilizationReport::WriteAverages (const ProcessorID *id, ArctermFile& p_file) const
{
	char str[MAX_PROC_ID_LEN];
	id->printID (str);

	p_file.writeField (str);
	p_file.writeInt (m_count);
	p_file.writeTime ( ElapsedTime( m_dScheduledTime / m_count ), TRUE);
	p_file.writeTime ( ElapsedTime( m_dOverTime / m_count ), TRUE);
	p_file.writeTime ( ElapsedTime( m_dActualTime / m_count ), TRUE);
	p_file.writeTime ( ElapsedTime( m_dServiceTime / m_count ), TRUE);
	p_file.writeTime ( ElapsedTime( (m_dActualTime - m_dServiceTime) / m_count ), TRUE);

	sprintf( str, "% 5.2f", (m_dServiceTime / m_dActualTime) * 100.0 );
	p_file.writeField (str);

	p_file.writeLine();

}
