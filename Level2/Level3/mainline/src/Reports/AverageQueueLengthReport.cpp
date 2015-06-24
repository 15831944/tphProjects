// AverageQueueLengthReport.cpp: implementation of the CAverageQueueLengthReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "AverageQueueLengthReport.h"
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

CAverageQueueLengthReport::CAverageQueueLengthReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}

CAverageQueueLengthReport::~CAverageQueueLengthReport()
{

}

void CAverageQueueLengthReport::GenerateDetailed (ArctermFile& p_file)
{
    p_file.writeField ("Processor,Max Queue,Max Queue Time");
    p_file.writeLine();

    char str[MAX_PROC_ID_LEN];
    ProcLogEntry logEntry;
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	int procCount = m_pTerm->procLog->getCount();

	CProgressBar bar( "Generating Average Queue Length Report", 100, procCount, TRUE );

    for (int i = 0; i < procCount; i++)
    {
		bar.StepIt();
        m_pTerm->procLog->getItem (logEntry, i);
        if (ProcFitsSpecs (logEntry) && logEntry.getQueue())
        {
            clear();
            calculateAverageQueueLength (logEntry);
            logEntry.getID (str);
            writeEntry (str, p_file);
        }
    }
}


void CAverageQueueLengthReport::GenerateSummary (ArctermFile& p_file)
{
    p_file.writeField ("Processor Group,Group Size,Max Queue,Max Queue Time");
    p_file.writeLine();

    const ProcessorID *procID;

	CProgressBar bar( "Generating Average Queue Length Report", 100, m_procIDlist.getCount(), TRUE );

	if(m_procIDlist.getCount() == 0)
	{
		AfxMessageBox(_T("Please select at least one specific processor or processor group before run the report."));
	}
    for (int i = 0; i < m_procIDlist.getCount(); i++)
    {
		bar.StepIt();
		clear();
        procID = m_procIDlist.getID (i);
        getAverageQueueLength (procID);
        writeAverage (procID, p_file);
    }
}


void CAverageQueueLengthReport::clear () 
{ 
	//m_fAverageQueueLength = 0.0; 
	m_nMaxQueueLength = 0;
	m_eMaxQueueTime = m_startTime; 
	m_nCount = 0; 
}


void CAverageQueueLengthReport::calculateAverageQueueLength
	(ProcLogEntry& logEntry)
{
    ElapsedTime previousTime (m_startTime), curTime, aTime;
	//float timeSlice;
	int queueLength = 0;
	m_nCount++;

	char title[1024] = "Calculating Average Lengths for Proc ";
	logEntry.SetOutputTerminal( m_pTerm );
    logEntry.getID (title + strlen (title));

    ProcEventStruct event;
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	logEntry.setToFirst();
    long eventCount = logEntry.getCount();
	for (long i = 0; i < eventCount; i++)
	{
		event = logEntry.getNextEvent();
        aTime.setPrecisely (event.time);
        if (aTime < m_startTime)
		{
			previousTime = m_startTime;
            if (event.type == IncreaseQueue)
				queueLength++;
            else if (event.type == DecreaseQueue)
				queueLength--;
			continue;
		}

//         if (aTime > m_endTime)
// 		{
// 			timeSlice = (m_endTime - previousTime) / (m_endTime - m_startTime);
// 			m_fAverageQueueLength += timeSlice * queueLength;
// 			return;
// 		}

        curTime = aTime;
        if (event.type == IncreaseQueue || event.type == DecreaseQueue)
		{
//             timeSlice = (curTime - previousTime) / (m_endTime - m_startTime);
//             m_fAverageQueueLength += timeSlice * queueLength;
            previousTime = curTime;

            (event.type == IncreaseQueue)?
                queueLength++: queueLength--;

            if (queueLength > m_nMaxQueueLength)
            {
                m_nMaxQueueLength = queueLength;
                m_eMaxQueueTime = curTime;
            }
        }
    }
}

void CAverageQueueLengthReport::writeEntry (const char *p_id,
    ArctermFile& p_file) const
{
    p_file.writeField (p_id);
 //   p_file.writeFloat (m_fAverageQueueLength, 2);
    p_file.writeInt (m_nMaxQueueLength);
    p_file.writeTime (m_eMaxQueueTime);
    p_file.writeLine();
}


void CAverageQueueLengthReport::getAverageQueueLength (const ProcessorID *p_id)
{
    ProcLogEntry logEntry;
    char str[MAX_PROC_ID_LEN];
    ProcessorID id;
	id.SetStrDict( m_pTerm->inStrDict );
	logEntry.SetOutputTerminal( m_pTerm );
    int procCount = m_pTerm->procLog->getCount();
    for (int i = 0; i < procCount; i++)
    {
        m_pTerm->procLog->getItem (logEntry, i);
        logEntry.getID (str);
        id.setID (str);
        if (p_id->idFits (id) && logEntry.getQueue())
            calculateAverageQueueLength (logEntry);
    }
}

void CAverageQueueLengthReport::writeAverage (const ProcessorID *p_id,
    ArctermFile& p_file) const
{
	char str[1024]={0};
    p_id->printID (str);

    p_file.writeField (str);
    p_file.writeInt (m_nCount);
// 	if(m_nCount==0)
// 	{
// 		p_file.writeFloat (0, 2);
// 	}
// 	else
// 	{
// 		p_file.writeFloat (m_fAverageQueueLength / m_nCount, 2);
// 	}
    p_file.writeInt (m_nMaxQueueLength);
    p_file.writeTime (m_eMaxQueueTime);
    p_file.writeLine();
}


