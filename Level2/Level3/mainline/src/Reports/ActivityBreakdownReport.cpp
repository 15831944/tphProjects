// ActivityBreakdownReport.cpp: implementation of the CActivityBreakdownReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "ActivityBreakdownReport.h"
#include "common\termfile.h"
#include "ActivityElements.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActivityBreakdownReport::CActivityBreakdownReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}

CActivityBreakdownReport::~CActivityBreakdownReport()
{

}


void CActivityBreakdownReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	CActivityElements::initReport(this);
}
void CActivityBreakdownReport::GenerateSummary( ArctermFile& p_file )
{
    p_file.writeField ("Passenger Type,Avg Moving,Avg Queuing,"
        "Avg Bag Wait,Avg Hold Area,Avg Service,Count");
    p_file.writeLine();

	CString str;
    //char str[MAX_PAX_TYPE_LEN];
    const CMobileElemConstraint *aConst;

	CProgressBar bar( "Generating Activity Breakdown Report", 100, m_multiConst.getCount(), TRUE );
	try
	{
		for (int i = 0; i < m_multiConst.getCount(); i++)
		{
			bar.StepIt();
			aConst = m_multiConst.getConstraint(i);
			getAverageActivityTimes (*aConst);

			aConst->screenPrint (str, 0, MAX_PAX_TYPE_LEN);
			p_file.writeField (str.GetBuffer(0));
			p_file.writeTime (avgFreeMovingTime, TRUE);
			p_file.writeTime (avgQueueWaitTime, TRUE);
			p_file.writeTime (avgBagWaitTime, TRUE);
			p_file.writeTime (avgHoldAreaWaitTime, TRUE);
			p_file.writeTime (avgServiceTime, TRUE);
			p_file.writeInt (count);

			p_file.writeLine();
		}
	}
	catch(ARCInvalidTimeError* timeError)
    {
		char msg[256];
		timeError->getMessage(msg);
		CString strMsg(msg);
		AfxMessageBox(msg);
        delete timeError;
    }
	catch(...)
	{
		throw;
	}
}


void CActivityBreakdownReport::GenerateDetailed( ArctermFile& p_file )
{
    p_file.writeField ("ID,Moving Time,Queuing Time,Bag Wait Time,Hold Area Time,Service Time,Passenger Type");
    p_file.writeLine();

    CActivityElements element;
    long paxCount = m_pTerm->paxLog->getCount();

	CProgressBar bar( "Generating Activity Breakdown Report", 100, paxCount, TRUE );
	try
	{
		for (long i = 0; i < paxCount; i++)
		{
			bar.StepIt();
			m_pTerm->paxLog->getItem (element, i);
			element.clearLog();
			element.SetOutputTerminal( m_pTerm );
			element.SetEventLog( m_pTerm->m_pMobEventLog );
			if (element.alive (m_startTime, m_endTime))
				if (element.fits (m_multiConst))
				{
					element.calculateActivities();
					element.writeEntry (p_file);
				}
		}
	}
	catch(ARCInvalidTimeError* timeError)
    {
		char msg[256];
		timeError->getMessage(msg);
		CString strMsg(msg);
		AfxMessageBox(msg);
        delete timeError;
    }
	catch(...)
	{
		throw;
	}
}


void CActivityBreakdownReport::getAverageActivityTimes (const CMobileElemConstraint& aConst)
{
    count = 0;
    avgQueueWaitTime = avgBagWaitTime = avgHoldAreaWaitTime =
        avgServiceTime = 0l;

    long double totFreeMovingTime = 0L, totQueueWaitTime = 0L,
        totBagWaitTime = 0L, totHoldAreaWaitTime = 0L,
        totServiceTime = 0L;

    //char title[80]
	CString title = "Activity Breakdown for Pax Type ";
	CString pTemp;
    //aConst.screenPrint (title + strlen (title), 0, MAX_PAX_TYPE_LEN);
      aConst.screenPrint(pTemp,0,MAX_PAX_TYPE_LEN);
    CActivityElements element;
    long paxCount = m_pTerm->paxLog->getCount();
    for (long i = 0; i < paxCount; i++)
    {
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
		element.SetOutputTerminal( m_pTerm );
		element.SetEventLog( m_pTerm->m_pMobEventLog );
        if (element.alive (m_startTime, m_endTime))
            if (element.fits (aConst))
            {
                element.calculateActivities();

                totFreeMovingTime += (element.getFreeMovingTime()).asSeconds();
                totQueueWaitTime += (element.getQueueWaitTime()).asSeconds();
                totBagWaitTime += (element.getBagWaitTime()).asSeconds();
                totHoldAreaWaitTime += (element.getHoldAreaWaitTime()).asSeconds();
                totServiceTime += (element.getServiceTime()).asSeconds();

                count++;
            }
    }

    if (count)
    {
        avgFreeMovingTime = (ElapsedTime) ((long)(totFreeMovingTime / count));
        avgQueueWaitTime = (ElapsedTime) ((long)(totQueueWaitTime / count));
        avgBagWaitTime = (ElapsedTime) ((long)(totBagWaitTime / count));
        avgHoldAreaWaitTime = (ElapsedTime) ((long)(totHoldAreaWaitTime  / count));
        avgServiceTime = (ElapsedTime) ((long)(totServiceTime  / count));
    }
}
