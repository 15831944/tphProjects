// PaxCountReport.cpp: implementation of the CPaxCountReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "PaxCountReport.h"

#include "DistElem.h"
#include "results\out_term.h"
#include "results\paxlog.h"
#include "common\CodeTimeTest.h"
#include "engine\terminal.h"
#include "../Common/ARCTracker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CPaxCountReport* PaxCount::m_pReport = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int PaxCount::process (CARCportEngine *)
{
	PLACE_METHOD_TRACK_STRING();
	START_CODE_TIME_TEST
		assert( m_pReport );
	m_pReport->updateCount (incType, time);
	if (incType == IncPaxCount)
	{
		time = exitTime;
		incType = DecPaxCount;
		addEvent();
	}
	return 0;
}

CPaxCountReport::CPaxCountReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{
    intervalCount = 0;
    passengerCountList = NULL;
    m_eventList = NULL;
    clear();
}

CPaxCountReport::~CPaxCountReport()
{
	if (intervalCount)
        delete [] passengerCountList;
    intervalCount = 0;
    passengerCountList = NULL;
    //delete [] m_eventList;
}

void CPaxCountReport::GenerateDetailed(ArctermFile &p_file)
{
	p_file.writeField ("Passenger Type,Time,Count");
    p_file.writeLine();

    int paxTypeCount = m_multiConst.getCount();

    const CMobileElemConstraint *paxType;

	CProgressBar bar( "Generating Passenger Count Report", 100, paxTypeCount, TRUE );

    for (int i = 0; i < paxTypeCount; i++)
    {
        paxType = m_multiConst.getConstraint(i);
        calculatePassengerCount (*paxType);
        writeEntries (*paxType, p_file);
		bar.StepIt();
    }
}

void CPaxCountReport::GenerateSummary(ArctermFile &p_file)
{
	p_file.writeField ("Passenger Type,Min,Min Time,Average,"
        "Max,Max Time,Total,First Entry,Last Exit");
    p_file.writeLine();

    int paxTypeCount = m_multiConst.getCount();

    const CMobileElemConstraint *paxType;

	CProgressBar bar( "Generating Passenger Count Report", 100, m_multiConst.getCount(), TRUE );

    for (int i = 0; i < m_multiConst.getCount(); i++)
    {
        paxType = m_multiConst.getConstraint(i);
        calculatePassengerCount (*paxType);
        writeSummary (*paxType, p_file);
		bar.StepIt();
    }
}

void CPaxCountReport::calculatePassengerCount(const CMobileElemConstraint &paxType)
{
	clear();
    long i;

    m_nPaxOfType = 0;
	m_pProgressBar = new CProgressBar( "Loading passenger activities", 100, m_pTerm->paxLog->getCount(), TRUE, 1 );
    EventList eventLoop;
    ReportPaxEntry element;

		

	m_eventList->initEvents( &eventLoop );
	m_eventList->setReport( this );
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	long paxCount = m_pTerm->paxLog->getCount();
    //outTerm.paxLog->setToFirst();	
    //long paxCount = outTerm.paxLog->getCount();

    for (i = 0; i < paxCount; i++)
    {
        //updateBar (loadBar, i);
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();

        if (element.getEntryTime() > m_endTime)
            break;

        if (element.fits (paxType))
        {
            if (element.alive (m_startTime, m_endTime))
                totalCount++;
            m_eventList[i].init (IncPaxCount, element.getEntryTime(),
                element.getExitTime());
            m_eventList[i].addEvent();
            m_nPaxOfType += 2;
        }
		m_pProgressBar->StepIt();
    }
	delete m_pProgressBar;
	m_pProgressBar = NULL;

	m_pProgressBar = new CProgressBar( "Calculating Passenger Type Count", 100, m_nPaxOfType, TRUE, 1 );

    m_nPaxOfType = 0;
	bool bCancel = false;
    eventLoop.start( NULL, &bCancel, false );
	delete m_pProgressBar;
	m_pProgressBar = NULL;

}

void CPaxCountReport::clear()
{
	if (passengerCountList && intervalCount)
	{
		memset ((void *)passengerCountList, '\0', intervalCount * sizeof (long));
	}
        

    totalCount = minCount = maxCount = paxCount = timeIndex = 0;
    timeOfMaxCount = timeOfMinCount = lastExit = m_startTime;
    previousTime = curTime = m_startTime;
	firstEntry = 86400l;
    averageCount = 0.0;
}

void CPaxCountReport::writeEntries(const CMobileElemConstraint &paxType, ArctermFile &p_file)
{
	//char str[64];
	CString str;
    paxType.screenPrint (str, 0, MAX_PAX_TYPE_LEN);
    ElapsedTime currentTime = m_startTime;

    for (int i = 0; i < intervalCount; i++)
    {
        p_file.writeField (str.GetBuffer(0));
        p_file.writeTime (currentTime);
        p_file.writeInt (passengerCountList[i]);
        p_file.writeLine();

        currentTime += m_interval;
    }
}

void CPaxCountReport::writeSummary(const CMobileElemConstraint &paxType, ArctermFile &p_file)
{
	//char str[MAX_PAX_TYPE_LEN];
    CString str;
	paxType.screenPrint (str, 0, MAX_PAX_TYPE_LEN);

    p_file.writeField (str.GetBuffer(0));
    p_file.writeInt (minCount);
    p_file.writeTime (timeOfMinCount, TRUE);

    p_file.writeFloat ((float)averageCount, 2);

    p_file.writeInt (maxCount);
    p_file.writeTime (timeOfMaxCount, TRUE);

    p_file.writeInt (totalCount);
    p_file.writeTime (firstEntry, TRUE);
    p_file.writeTime (lastExit, TRUE);

    p_file.writeLine();
}

void CPaxCountReport::initIntervalStats()
{
	intervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
    passengerCountList = new long[intervalCount];

	ReportPaxEntry element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
    m_eventList = new PaxCount[m_pTerm->paxLog->getCount()];
}
void CPaxCountReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval = 1;
	initIntervalStats();
	PaxCount::setReport( this );
}
void CPaxCountReport::updateCount(int incType, ElapsedTime time)
{
    m_pProgressBar->StepIt();
	if (timeIndex >= intervalCount)
        return;

    // update interval stats
    while (curTime < time && timeIndex < intervalCount)
    {
		//less than next interval should break
		if (time <= (curTime + m_interval))
			break;

		if (passengerCountList)
            passengerCountList[timeIndex++] = paxCount;
		curTime += m_interval;
	}

    // update count
    if (incType == IncPaxCount)
		paxCount++;
	else
		paxCount--;

	// test time range
	if (time < m_startTime)
		firstEntry = m_startTime;
	else if (time < firstEntry)
		firstEntry = time;

	if (time > m_endTime)
		lastExit = m_endTime;
    else if (time > lastExit)
		lastExit = time;

    // both min and max are set to state at start time of report
    if (time < m_startTime)
    {
        minCount = maxCount = paxCount;
        timeOfMinCount = timeOfMaxCount = m_startTime;
        previousTime = m_startTime;
    }
    else
    {
        // test for max count
        if (paxCount > maxCount && time <= m_endTime)
        {
            maxCount = paxCount;
            timeOfMaxCount = time;
        }

        // test for min count
        if (paxCount < minCount && time <= m_endTime)
        {
            minCount = paxCount;
            timeOfMinCount = time;
        }

        // update weighted average
        time = (time < m_endTime)? time: m_endTime;
        double timeSlice;
        timeSlice = (time - previousTime) / (m_endTime - m_startTime);
        averageCount += timeSlice * paxCount;
        previousTime = time;
    }
}
