// BagWaitReport.cpp: implementation of the CBagWaitReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "BagWaitReport.h"
#include "BagWaitElement.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBagWaitReport::CBagWaitReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}

CBagWaitReport::~CBagWaitReport()
{

}

void CBagWaitReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	CBagWaitElement::initReport( this );			
}
void CBagWaitReport::GenerateDetailed (ArctermFile& p_file)
{
    p_file.writeField ("Pax #,Bag Count,Wait Time,Passenger Type");
    p_file.writeLine();

    CBagWaitElement element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	
	CProgressBar bar( "Generating Bag Wait Report", 100, m_pTerm->paxLog->getCount(), TRUE );

    for (long i = 0; i < m_pTerm->paxLog->getCount(); i++)
    {
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
		if( element.GetMobileType() == 0 )
		{
			if (element.alive (m_startTime, m_endTime))
            if (element.fits (m_multiConst))
                if (element.isArriving() && element.getBagCount())
                {
                    if(element.calculateBagWaitTime (m_startTime, m_endTime, m_pTerm ,&m_procIDlist ))
						element.writeEntry (p_file);
                }
		}        
		bar.StepIt();
    }
}

void CBagWaitReport::GenerateSummary (ArctermFile& p_file)
{
    p_file.writeField ("Passenger Type,Minimum,Average,Maximum,Count");
    p_file.writeLine();

    char str[MAX_PAX_TYPE_LEN];
    const CMobileElemConstraint *aConst;


	CProgressBar bar( "Generating Bag Wait Report", 100, m_pTerm->paxLog->getCount(), TRUE );
    for (int i = 0; i < m_multiConst.getCount(); i++)
    {
        aConst = m_multiConst.getConstraint(i);

		CStatisticalTools< ElapsedTime > tempTools;
        getAverageBagWaitTime (*aConst, tempTools );
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
		bar.StepIt();
    }
}

void CBagWaitReport::getAverageBagWaitTime (const CMobileElemConstraint& aConst, CStatisticalTools< ElapsedTime >& _tools)
{
    long double totalDuration = 0l;
    minimum = 86400l;
    maximum = average = 0l;
    count = 0;

	CProgressBar bar( "Calculating Average Baggage Wait Time", 100, m_pTerm->paxLog->getCount(), TRUE, 1 );
    ElapsedTime waitTime;
    CBagWaitElement element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
    for (long i = 0; i < m_pTerm->paxLog->getCount(); i++)
    {
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
		if( element.GetMobileType() == 0 )
		{
	        if (element.alive (m_startTime, m_endTime))
	            if (element.fits (aConst) && element.getBagCount())
	            {
	                if(element.calculateBagWaitTime (m_startTime, m_endTime, m_pTerm ,&m_procIDlist ))
					{
						waitTime = element.getTotalTime();
						_tools.AddNewData( waitTime );

						if (waitTime < minimum)
							minimum = waitTime;

						if (waitTime > maximum)
							maximum = waitTime;

						totalDuration += waitTime.asSeconds();
						count++;
					}

	            }
		}
		bar.StepIt();
    }
    if (count)
        average = (ElapsedTime) ((long)(totalDuration / count));
}


