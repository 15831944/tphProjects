// BagCountReport.cpp: implementation of the CBagCountReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "BagCountReport.h"
#include "common\termfile.h"
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

CBagCountReport::CBagCountReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath),m_pBagCountList( NULL)
{

}

CBagCountReport::~CBagCountReport()
{
	if( m_pBagCountList )
		delete []m_pBagCountList;
}

void CBagCountReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval=1;
	initIntervalStats();
	
}
void CBagCountReport::GenerateSummary( ArctermFile& p_file )
{
    p_file.writeField ("Processor Group,Group Size,Time,Min,Avg,Max,Total");
    p_file.writeLine();

    const ProcessorID *procID;
    m_pIntervalStats = new CIntervalStat[m_nIntervalCount];

	CProgressBar bar( "Generating Bag Count Report", 100, m_procIDlist.getCount(), TRUE );

    for (int i = 0; i < m_procIDlist.getCount(); i++)
    {
		bar.StepIt();
        procID = m_procIDlist.getID (i);
        getAverageBagCount (procID);
        writeAverages (procID, p_file);
    }
    delete [] m_pIntervalStats;
}


void CBagCountReport::GenerateDetailed( ArctermFile& p_file )
{
    p_file.writeField ("Processor,Time,Bag Count");
    p_file.writeLine();

    char str[MAX_PROC_ID_LEN];
    ProcLogEntry logEntry;
	logEntry.SetOutputTerminal( m_pTerm );
	logEntry.SetEventLog( m_pTerm->m_pProcEventLog );
    int procCount = m_pTerm->procLog->getCount();

	CProgressBar bar( "Generating Bag Count Report", 100, procCount, TRUE );
	
    for (int i = 0; i < procCount; i++)
    {
		bar.StepIt();
        m_pTerm->procLog->getItem (logEntry, i);
        if (ProcFitsSpecs (logEntry) && logEntry.getProcType() == BaggageProc)
        {
            calculateBagCounts (logEntry);
            logEntry.getID (str);
            writeEntries (str, p_file);
        }
    }
}



void CBagCountReport::getAverageBagCount (const ProcessorID *id)
{
    int i;
    for (i = 0; i < m_nIntervalCount; i++)
        m_pIntervalStats[i].clear();
    memset ((void *)m_pBagCountList, '\0', m_nIntervalCount * sizeof (int));

    ProcLogEntry logEntry;
    ProcessorID procID;
    char str[MAX_PROC_ID_LEN];
	logEntry.SetOutputTerminal( m_pTerm );
	logEntry.SetEventLog( m_pTerm->m_pProcEventLog );
	procID.SetStrDict( m_pTerm->inStrDict );
    int procCount = m_pTerm->procLog->getCount();
    for (i = 0; i < procCount; i++)
    {
        m_pTerm->procLog->getItem (logEntry, i);
        logEntry.getID (str);
        procID.setID (str);
        if (id->idFits (procID) && logEntry.getProcType() == BaggageProc)
        {
            calculateBagCounts (logEntry);
            updateIntervalStats();
        }
    }
}

void CBagCountReport::writeAverages (const ProcessorID *id,
    ArctermFile& p_file) const
{
    char str[64];
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

void CBagCountReport::calculateBagCounts (ProcLogEntry& logEntry)
{
    int timeIndex = 0;
    ElapsedTime nextTime = m_startTime;
    int bagCount = 0;

    memset ((void *)m_pBagCountList, '\0', m_nIntervalCount * sizeof (int));

    ProcEventStruct event;
	
    logEntry.setToFirst();
	long lTime =-2;
    long eventCount = logEntry.getCount();
    for (long i = 0; timeIndex < m_nIntervalCount && i < eventCount; i++)
    {
        event = logEntry.getNextEvent();
		//// TRACE("\n id = %d\n",event.element );
		//// TRACE("\n id = %d\n",event.time);
		//// TRACE("\n id = %d\n",event.type );
		ASSERT( event.time >= lTime );
		lTime = event.time;
	
        while (event.time > (long)nextTime && timeIndex<m_nIntervalCount )
        {
            m_pBagCountList[timeIndex++] = bagCount;
            nextTime += m_interval;
        }

        if (event.type == BagArriveAtBaggageProc)
		{
			bagCount++;
		}
            
        else if (event.type == BagLeaveBaggageProc)
		{
			bagCount--;
		}
            
    }
}

void CBagCountReport::updateIntervalStats (void)
{
    for (int i = 0; i < m_nIntervalCount; i++)
        m_pIntervalStats[i].update (m_pBagCountList[i]);
}


void CBagCountReport::writeEntries (const char *id, ArctermFile& p_file) const
{
    ElapsedTime currentTime = m_startTime;

    for (int i = 0; i < m_nIntervalCount; i++)
    {
        p_file.writeField (id);
        p_file.writeTime (currentTime);
        p_file.writeInt (m_pBagCountList[i]);
        p_file.writeLine();

        currentTime += m_interval;
    }
}

void CBagCountReport::initIntervalStats (void)
{
    m_nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
	if( m_pBagCountList )
	{
		delete []m_pBagCountList;
	}

    m_pBagCountList = new int[m_nIntervalCount];
}
