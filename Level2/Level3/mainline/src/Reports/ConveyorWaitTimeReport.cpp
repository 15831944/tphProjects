// ConveyorWaitTimeReport.cpp: implementation of the CConveyorWaitTimeReport class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ConveyorWaitTimeReport.h"
#include "common\termfile.h"
#include "QueueElement.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConveyorWaitTimeReport::CConveyorWaitTimeReport(Terminal* _pTerm, const CString& _csProjPath)
:CBaseReport(_pTerm, _csProjPath)
{

}

CConveyorWaitTimeReport::~CConveyorWaitTimeReport()
{

}
void CConveyorWaitTimeReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	QueueElement::initReport ( this );	
}
void CConveyorWaitTimeReport::GenerateSummary( ArctermFile& p_file )
{
	p_file.writeField ("Passenger Type,Minimum,Average,Maximum,Count");
    p_file.writeLine();
	
	QueueElement element;
	element.initReport(this);
    int  paxTypeCount = m_multiConst.getCount();
	
    char str[MAX_PAX_TYPE_LEN];
    const CMobileElemConstraint *aConst;
	
	CProgressBar bar( "Generating Passenger Wait In Conveyor Report", 100, m_multiConst.getCount(), TRUE );
	
    for (int i = 0; i < m_multiConst.getCount(); i++)
    {
		bar.StepIt();
        aConst = m_multiConst.getConstraint(i);
		CStatisticalTools<ElapsedTime> tempTools;
        getAverageQueueTime (*aConst, tempTools);
		
		tempTools.SortData();
		
        aConst->screenPrint (str, 0, 37);
        p_file.writeField (str);
        p_file.writeTime (minimum, TRUE);
        p_file.writeTime (average, TRUE);
        p_file.writeTime (maximum, TRUE);
        p_file.writeInt (count);
		if( count <=0 )
		{
			p_file.writeTime( 0l,  TRUE );//Q1
			p_file.writeTime( 0l, TRUE );//Q2
			p_file.writeTime( 0l, TRUE );//Q3
			
			p_file.writeTime( 0l, TRUE );//P1
			p_file.writeTime( 0l, TRUE );//P5
			p_file.writeTime( 0l, TRUE );//P10
			
			p_file.writeTime( 0l, TRUE );//P90
			p_file.writeTime( 0l, TRUE );//P95
			p_file.writeTime( 0l, TRUE );//P99
			
			p_file.writeTime( 0l, TRUE );//Sigma
		}
		else
		{
			p_file.writeTime( tempTools.GetPercentile( 25 ),  TRUE );//Q1
			p_file.writeTime( tempTools.GetPercentile( 50 ), TRUE );//Q2
			p_file.writeTime( tempTools.GetPercentile( 75 ), TRUE );//Q3
			
			p_file.writeTime( tempTools.GetPercentile( 1 ), TRUE );//P1
			p_file.writeTime( tempTools.GetPercentile( 5 ), TRUE );//P5
			p_file.writeTime( tempTools.GetPercentile( 10 ), TRUE );//P10
			
			p_file.writeTime( tempTools.GetPercentile( 90 ), TRUE );//P90
			p_file.writeTime( tempTools.GetPercentile( 95 ), TRUE );//P95
			p_file.writeTime( tempTools.GetPercentile( 99 ), TRUE );//P99
			
			p_file.writeTime( ElapsedTime(tempTools.GetSigma()), TRUE );//Sigma
		}
		
		
        p_file.writeLine();
    }
}
void CConveyorWaitTimeReport::getAverageQueueTime(const CMobileElemConstraint &aConst,CStatisticalTools<ElapsedTime>& _tools)
{
	long double totalDuration = 0l;
    minimum = maximum = average = 0l;
    count = 0;
	
    char title[1024] = "Calculating Passenger Wait In Conveyor Times for Pax Type ";
    aConst.screenPrint (title + strlen (title), 0, MAX_PAX_TYPE_LEN);    
	
    QueueElement element;	
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	
    long paxCount = m_pTerm->paxLog->getCount();		
    for (long i = 0; i < paxCount; i++)
    {        
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
        if (element.alive (m_startTime, m_endTime))
            if (element.fits (aConst))
            {
                element.calculateConveyorWaitQueueTime (m_startTime, m_endTime);
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
    if (count)
        average = (ElapsedTime) ((long)(totalDuration / count));
}
void CConveyorWaitTimeReport::GenerateDetailed( ArctermFile& p_file )
{
	p_file.writeField ("Passenger #,Total Time,Average Time,Processors,Passenger Type");
    p_file.writeLine();
	
    QueueElement element;
	element.initReport(this);
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
    long paxCount = m_pTerm->paxLog->getCount();
	
	CProgressBar bar( "Generating Passenger Wait In Conveyor Report", 100, paxCount, TRUE );
	
    for (long i = 0; i < paxCount; i++)
    {
		bar.StepIt();
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
        if (element.alive (m_startTime, m_endTime))
            if (element.fits (m_multiConst))
            {
                element.calculateConveyorWaitQueueTime (m_startTime, m_endTime);
                element.writeEntry (p_file);
            }
    }
}