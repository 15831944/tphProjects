// ConveyorWaitLenghtReport.cpp: implementation of the CConveyorWaitLenghtReport class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ConveyorWaitLenghtReport.h"
#include "engine\proclist.h"
#include "common\states.h"
#include "IntervalStat.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConveyorWaitLenghtReport::CConveyorWaitLenghtReport(Terminal* _pTerm, const CString& _csProjPath)
:CBaseReport(_pTerm, _csProjPath)
{
	m_nIntervalCount =0;
	m_pLengthList = NULL;
}

CConveyorWaitLenghtReport::~CConveyorWaitLenghtReport()
{

}
void CConveyorWaitLenghtReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval = 1;	
	initIntervalStats();
}
void CConveyorWaitLenghtReport::initIntervalStats (void)
{
    m_nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
	if( m_pLengthList )
	{
		delete[] m_pLengthList;
	}
    m_pLengthList = new int[m_nIntervalCount];    
}
void CConveyorWaitLenghtReport::calculateQueueLengths (const ProcessorID* _pProcID )
{	
	ProcLogEntry logEntry;
	logEntry.SetOutputTerminal( m_pTerm );

	ProcEventStruct event;
	logEntry.SetOutputTerminal( m_pTerm );
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	memset ((void *)m_pLengthList, 0, m_nIntervalCount * sizeof (int));
	int procCount = m_pTerm->procLog->getCount();	
	//CProgressBar bar( "Generating Queue Length Report", 100, procCount, TRUE );
    for (int i = 0; i < procCount; i++)
    {
		int timeIndex = 0;
		ElapsedTime nextTime = m_startTime;
		//bar.StepIt();		
        m_pTerm->procLog->getItem (logEntry, i); 		
		logEntry.setToFirst();
		long eventCount = logEntry.getCount();
		for (long j = 0; timeIndex < m_nIntervalCount && j < eventCount; j++)
		{
			event  = logEntry.getEvent( j );
			if(event.time < (long)m_startTime)//trim the event not in the time range( start time and end time)
				continue;

			//event = logEntry.getNextEvent();			
			if(event.time >= (long)nextTime &&event.time< (long)(nextTime + m_interval) && timeIndex < m_nIntervalCount)
			{
				if( event.lReason >=0 && event.type == ProcessorStateStop)
				{
					Processor* pProc = m_pTerm->procList->getProcessor( event.lReason );
					ASSERT( pProc );
					if( _pProcID->idFits( *pProc->getID() ) )
					{
						m_pLengthList[timeIndex] += event.lLoad;
					}
				}
				continue;
			}
			//move to next range
			timeIndex++;
			nextTime += m_interval;
		}		
    } 
}
bool CConveyorWaitLenghtReport::isInterestedEvent(const ProcEventStruct& _event )const
{
	if( _event.lReason < 0 )
	{
		return false;
	}

	if (!m_procIDlist.getCount() || m_procIDlist.areAllBlank())
        return true;

    Processor* pReasonProc = m_pTerm->procList->getProcessor( _event.lReason );
	ASSERT( pReasonProc );
    for (int i = 0; i < m_procIDlist.getCount(); i++)
    {
        if ((m_procIDlist.getID(i))->idFits ( *pReasonProc->getID() ))
            return true;
    }
    return false;
}



void CConveyorWaitLenghtReport::GenerateSummary( ArctermFile& p_file )
{
	p_file.writeField ("Processor Group,Group Size,Time,Min,Avg,Max,Total");
	p_file.writeLine();
	
	const ProcessorID *procID;
	m_pIntervalStats = new CIntervalStat[m_nIntervalCount];
	
	CProgressBar bar( "Generating Conveyor Wait Length Report", 100, m_procIDlist.getCount(), TRUE );
	int iCount = m_procIDlist.getCount();	
	if( iCount >0 )
	{
		for (int i = 0; i < m_procIDlist.getCount(); i++)
		{
			bar.StepIt();
			procID = m_procIDlist.getID(i);
			calculateQueueLengths (procID);
			updateIntervalStats();
			writeAverages (procID, p_file);
		}
	}
	else
	{
		ProcessorID idDefaut;
		idDefaut.SetStrDict( m_pTerm->inStrDict );
		calculateQueueLengths( &idDefaut );
		updateIntervalStats();
		writeAverages (&idDefaut, p_file);		
	}    
}
void CConveyorWaitLenghtReport::GenerateDetailed( ArctermFile& p_file )
{
    p_file.writeField ("Processor,Time,Queue Length");
    p_file.writeLine();
	int iCount = m_procIDlist.getCount();	
	if( iCount >0 )
	{
		CProgressBar bar( "Generating Conveyor Wait Length Report", 100,iCount, TRUE );
		for (int i = 0; i < m_procIDlist.getCount(); i++)
		{
			bar.StepIt();
			const ProcessorID* pProcID = m_procIDlist.getID(i);
			calculateQueueLengths( pProcID );
			writeEntries (pProcID->GetIDString(), p_file);
		}
		
	}
	else
	{
		ProcessorID idDefaut;
		idDefaut.SetStrDict( m_pTerm->inStrDict );
		calculateQueueLengths( &idDefaut );
		writeEntries (idDefaut.GetIDString(), p_file);		
	}   
}
void CConveyorWaitLenghtReport::writeEntries (const char *p_id, ArctermFile& p_file) const
{
    ElapsedTime currentTime = m_startTime;
    for (int i = 0; i < m_nIntervalCount; i++)
    {
        p_file.writeField (p_id);
        p_file.writeTime (currentTime);
        p_file.writeInt (m_pLengthList[i]);
        p_file.writeLine();
		
        currentTime += m_interval;
    }
}

void CConveyorWaitLenghtReport::updateIntervalStats (void)
{
	for (int i = 0; i < m_nIntervalCount; i++)
        m_pIntervalStats[i].clear();

    for (int i = 0; i < m_nIntervalCount; i++)
        m_pIntervalStats[i].update (m_pLengthList[i]);
}

void CConveyorWaitLenghtReport::writeAverages (const ProcessorID *id,
										ArctermFile& p_file) const
{
    char str[MAX_PROC_ID_LEN];
    id->printID (str);
    ElapsedTime currentTime = m_startTime;
	
    for (int i = 0; i < m_nIntervalCount; i++)
    {
        p_file.writeField (str);
        p_file.writeInt (m_pIntervalStats[i].getCount());
        p_file.writeTime (currentTime);
		
        p_file.writeInt (m_pIntervalStats[i].getMin());
        p_file.writeFloat (m_pIntervalStats[i].getAvg(), 2);
        p_file.writeInt (m_pIntervalStats[i].getMax());
        p_file.writeInt (m_pIntervalStats[i].getTotal());
		
        p_file.writeLine();
		
        currentTime += m_interval;
    }
}
