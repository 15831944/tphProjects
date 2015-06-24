// QueueLengthReport.cpp: implementation of the CQueueLengthReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "QueueLengthReport.h"
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

CQueueLengthReport::CQueueLengthReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{
	m_pQueueLengthList = NULL; 
	m_nIntervalCount = 0;
}

CQueueLengthReport::~CQueueLengthReport()
{
	if( m_pQueueLengthList )
	{
		delete[] m_pQueueLengthList;
	}
}


void CQueueLengthReport::initIntervalStats (void)
{
    m_nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
	if( m_pQueueLengthList )
	{
		delete[] m_pQueueLengthList;
	}
    m_pQueueLengthList = new int[m_nIntervalCount];
    
}
void CQueueLengthReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval = 1;	
	initIntervalStats();
}

void CQueueLengthReport::GenerateSummary( ArctermFile& p_file )
{
    p_file.writeField ("Processor Group,Group Size,Time,Min,Avg,Max,Total");
    p_file.writeLine();

    const ProcessorID *procID;
    m_pIntervalStats = new CIntervalStat[m_nIntervalCount];
	
	CProgressBar bar( "Generating Queue Length Report", 100, m_procIDlist.getCount(), TRUE );

    for (int i = 0; i < m_procIDlist.getCount(); i++)
    {
		bar.StepIt();
        procID = m_procIDlist.getID(i);
        getAverageQueueLength (procID);
        writeAverages (procID, p_file);
    }
    //delete m_pIntervalStats;
}


void CQueueLengthReport::GenerateDetailed( ArctermFile& p_file )
{
	if(calculateValidProcLog()>1000)	// Limited by chart drawing.
		throw new ARCException("Too many target processors!");

    p_file.writeField ("Processor,Time,Queue Length");
    p_file.writeLine();

    char str[MAX_PROC_ID_LEN];
    ProcLogEntry logEntry;
    int procCount = m_pTerm->procLog->getCount();

	CProgressBar bar( "Generating Queue Length Report", 100, procCount, TRUE );

    for (int i = 0; i < procCount; i++)
    {
		bar.StepIt();
        m_pTerm->procLog->getItem (logEntry, i);
        if (ProcFitsSpecs (logEntry) && logEntry.getQueue())
        {
            calculateQueueLengths (logEntry);
            logEntry.getID (str);
            writeEntries (str, p_file);// TRACE("<%s>\n",str);
        }
    }
}

void CQueueLengthReport::calculateQueueLengths (ProcLogEntry& logEntry)
{
    int timeIndex = 0;
    ElapsedTime nextTime = m_startTime;
    int queueLength = 0;

	char title[256] = "Calculating Average Lengths for Proc ";
	// the max length of the proc id is 128(MAX_PROC_ID_LEN) + 128(other ) ,so the title length set to 640
	logEntry.SetOutputTerminal( m_pTerm );
    logEntry.getID (title + strlen (title));

    memset ((void *)m_pQueueLengthList, '\0', m_nIntervalCount * sizeof (int));

    ProcEventStruct event;
	logEntry.SetOutputTerminal( m_pTerm );
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
    logEntry.setToFirst();
    long eventCount = logEntry.getCount();
    for (long i = 0; timeIndex < m_nIntervalCount && i < eventCount; i++)
    {
        event = logEntry.getNextEvent();
        while (event.time > (long)nextTime && timeIndex < m_nIntervalCount)
        {
            m_pQueueLengthList[timeIndex++] = queueLength;
            nextTime += m_interval;
        }

        if (event.type == IncreaseQueue)
            queueLength++;
        else if (event.type == DecreaseQueue)
            queueLength--;
    }
}

void CQueueLengthReport::writeEntries (const char *p_id, ArctermFile& p_file) const
{
    ElapsedTime currentTime = m_startTime;
    for (int i = 0; i < m_nIntervalCount; i++)
    {
        p_file.writeField (p_id);
        p_file.writeTime (currentTime);
        p_file.writeInt (m_pQueueLengthList[i]);
        p_file.writeLine();

        currentTime += m_interval;
    }
}

void CQueueLengthReport::getAverageQueueLength (const ProcessorID *p_id)
{
    int i;
    for (i = 0; i < m_nIntervalCount; i++)
        m_pIntervalStats[i].clear();
    memset ((void *)m_pQueueLengthList, '\0', m_nIntervalCount * sizeof (int));

    ProcessorID id;
	id.SetStrDict( m_pTerm->inStrDict );
    char str[MAX_PROC_ID_LEN];
    ProcLogEntry logEntry;
	logEntry.SetOutputTerminal( m_pTerm );
    int procCount = m_pTerm->procLog->getCount();
    for (i = 0; i < procCount; i++)
    {
        m_pTerm->procLog->getItem (logEntry, i);
        logEntry.getID (str);
        id.setID (str);
        if (p_id->idFits (id) && logEntry.getQueue())
        {
            calculateQueueLengths (logEntry);
            updateIntervalStats();
        }
    }
}

void CQueueLengthReport::updateIntervalStats (void)
{
    for (int i = 0; i < m_nIntervalCount; i++)
        m_pIntervalStats[i].update (m_pQueueLengthList[i]);
}

void CQueueLengthReport::writeAverages (const ProcessorID *id,
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


int CQueueLengthReport::calculateValidProcLog()
{
	ASSERT(m_pTerm!=NULL && m_pTerm->procLog!=NULL);

	int nCount=0;
    ProcLogEntry logEntry;
    int procCount = m_pTerm->procLog->getCount();

    for (int i = 0; i < procCount; i++)
    {
        m_pTerm->procLog->getItem (logEntry, i);
        if (ProcFitsSpecs (logEntry) && logEntry.getQueue())
        {
			nCount++;
        }
    }
	return nCount;
}
