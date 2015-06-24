// ServiceReport.cpp: implementation of the CServiceReport class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Reports.h"
#include "ServiceReport.h"
#include "common\termfile.h"
#include "SERVELEM.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceReport::CServiceReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}

CServiceReport::~CServiceReport()
{

}
void CServiceReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	ServiceElement::initReport (this);
}
void CServiceReport::GenerateSummary(ArctermFile &p_file)
{
	p_file.writeField ("Passenger Type,Minimum,Average,Maximum,Count");
    p_file.writeLine();

    int paxTypeCount = m_multiConst.getCount();

    char str[MAX_PAX_TYPE_LEN];
    const CMobileElemConstraint *aConst;

	CProgressBar bar( "Generating Service Time Report", 100,m_multiConst.getCount(), TRUE );

    for (int i = 0; i < m_multiConst.getCount(); i++)
    {
		bar.StepIt();
        aConst = m_multiConst.getConstraint(i);

		CStatisticalTools< ElapsedTime > tempTools;
        getAverageServiceTime (*aConst, tempTools);
		tempTools.SortData();

        aConst->screenPrint (str, 0, 37);
        p_file.writeField (str);
        p_file.writeTime (minimum, TRUE);
        p_file.writeTime (average, TRUE);
        p_file.writeTime (maximum, TRUE);
        p_file.writeInt (count);
		
		p_file.writeTime( tempTools.GetPercentile( 25 ) , TRUE );//Q1
		p_file.writeTime( tempTools.GetPercentile( 50 ), TRUE );//Q2
		p_file.writeTime( tempTools.GetPercentile( 75 ), TRUE );//Q3
		
		p_file.writeTime( tempTools.GetPercentile( 1 ), TRUE );//P1
		p_file.writeTime( tempTools.GetPercentile( 5 ), TRUE );//P5
		p_file.writeTime( tempTools.GetPercentile( 10 ), TRUE );//P10
		
		p_file.writeTime( tempTools.GetPercentile( 90 ), TRUE );//P90
		p_file.writeTime( tempTools.GetPercentile( 95 ), TRUE );//P95
		p_file.writeTime( tempTools.GetPercentile( 99 ), TRUE );//P99
		
		p_file.writeTime( ElapsedTime( tempTools.GetSigma() ), TRUE );//sigma
		
        p_file.writeLine();
    }
}

void CServiceReport::GenerateDetailed(ArctermFile &p_file)
{
	p_file.writeField ("Passenger #,Group Service,Single Service,Processors,Passenger Type");
    p_file.writeLine();


    ServiceElement element;
	element.initReport(this);
	element.SetOutputTerminal(m_pTerm);
	element.SetEventLog(m_pTerm->m_pMobEventLog);
    long paxCount = m_pTerm->paxLog->getCount();

	CProgressBar bar( "Generating Service Time Report", 100, paxCount, TRUE );

    for (long i = 0; i < paxCount; i++)
    {
        bar.StepIt();
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
        if (element.alive (m_startTime, m_endTime))
            if (element.fits (m_multiConst))
            {
                element.calculateServiceTime (m_startTime, m_endTime);
                element.writeEntry (p_file);
            }
    }
}

void CServiceReport::getAverageServiceTime(const CMobileElemConstraint &aConst, CStatisticalTools< ElapsedTime >& _tools)
{
	long double totalDuration = 0l;
	minimum = maximum = average = 0l;
    count = 0;

	char title[MAX_PAX_TYPE_LEN] = "Calculating Average Service Time for Pax Type ";
    aConst.screenPrint (title + strlen (title), 0, 128);
   // int progressBar = initBar (title, outTerm.paxLog->getCount());

    ServiceElement element;
	element.initReport(this);
	element.SetOutputTerminal(m_pTerm);
	element.SetEventLog(m_pTerm->m_pMobEventLog);
    long paxCount = m_pTerm->paxLog->getCount();
    for (long i = 0; i < paxCount; i++)
    {
        //updateBar (progressBar, i);
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
        if (element.alive (m_startTime, m_endTime))
            if (element.fits (aConst))
            {
                element.calculateServiceTime (m_startTime, m_endTime);
                if (!element.getProcCount())
                    continue;
				
				_tools.AddNewData( element.getTotalTime() );
                if (element.getTotalTime() < minimum || minimum == 0l)
                    minimum = element.getTotalTime();

                if (element.getTotalTime() > maximum)
                    maximum = element.getTotalTime();

                totalDuration += (element.getTotalTime()).asSeconds();
                count++;
            }
    }
   // completeBar (progressBar);

    if (count)
        average = (ElapsedTime) ((long)(totalDuration / count));
}
