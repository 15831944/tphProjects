// DurationReport.cpp: implementation of the CDurationReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "DurationReport.h"
#include "common\termfile.h"
#include "Rep_PAX.h"
#include "engine\proclist.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDurationReport::CDurationReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{
	

}

CDurationReport::~CDurationReport()
{

}
void CDurationReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	_pPara->GetFromToProcs( m_fromToProcs );
}
void CDurationReport::GenerateDetailed(ArctermFile &p_file)
{
	InitTools();
	p_file.writeField ("Passenger #,Duration,Passenger Type");
    p_file.writeLine();

	

    char str[MAX_PAX_TYPE_LEN];
	ReportPaxEntry element;
	element.SetOutputTerminal(m_pTerm);
	element.SetEventLog(m_pTerm->m_pMobEventLog);
    
    long paxCount = m_pTerm->paxLog->getCount();

	CProgressBar bar( "Generating Passenger Duration Report", 100, paxCount, TRUE );

    for (long i = 0; i < paxCount; i++)
    {
        bar.StepIt();
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
        if (element.alive (m_startTime, m_endTime))
            if (element.fits (m_multiConst))
            {
				element.printFullType (str);
				CaculateEveryPax( element );
				std::vector<CFromToProcCaculation::FROM_TO_PROC>& values = m_fromToProcsTools.GetValues();
				for( unsigned j=0;j<values.size(); ++j )
				{			
					if( values[j].m_enFlag == CFromToProcCaculation::END_COMPUTE || values[j].m_iToProc == END_PROCESSOR_INDEX )
					{
						p_file.writeInt (element.getID());
						p_file.writeTime (ElapsedTime(values[j].m_dValue), TRUE);					
						p_file.writeField (str);

						Processor* pProc = m_pTerm->procList->getProcessor( values[j].m_iFromProc );
						p_file.writeField( pProc->getID()->GetIDString() );
						
						pProc = m_pTerm->procList->getProcessor( values[j].m_iToProc );
						p_file.writeField( pProc->getID()->GetIDString() );	
						p_file.writeLine();
					}
				}
				
				m_fromToProcsTools.ResetAll();
            }
    }
    //completeBar (progressBar);
}
void CDurationReport::CaculateEveryPax(ReportPaxEntry& _events )
{		
    _events.loadEvents();
    PaxEvent track1, track2;
    track1.init (_events.getEvent(0));
    track2.init (_events.getEvent(1));
    
    int trackCount = _events.getCount();
	
	m_fromToProcsTools.AddValue( track1.getProc(),track1.getState(), 0.0 );	
	int nSaftTime=0;

    for (int i = 2; i < trackCount; i++)
    {
        track1 = track2;
        track2.init (_events.getEvent(i));
		nSaftTime=track2.getTime().asSeconds()-track1.getTime().asSeconds();
//		if(nSaftTime<0)
//			nSaftTime+=WholeDay;

		m_fromToProcsTools.AddValue( track1.getProc(),track1.getState() , nSaftTime);		
    }
    _events.clearLog();
}
void CDurationReport::GenerateSummary(ArctermFile &p_file)
{
	InitTools();
	p_file.writeField ("Passenger Type,Minimum,Average,Maximum,Count");
    p_file.writeLine();

    int paxTypeCount = m_multiConst.getCount();
//    if (paxTypeCount > 1)
//        progressBar = initBar ("Generating Passenger Distances",
//            paxTypeCount);

    char str[MAX_PAX_TYPE_LEN];
    const CMobileElemConstraint *aConst;

	CProgressBar bar( "Generating Passenger Duration Report", 100, m_multiConst.getCount(), TRUE );

    for (int i = 0; i < m_multiConst.getCount(); i++)
    {
		bar.StepIt();
        aConst = m_multiConst.getConstraint(i);
		
		CStatisticalTools< ElapsedTime > tempTools;
        getAverageDuration (*aConst, tempTools );
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
//    if (paxTypeCount > 1)
//        completeBar (progressBar);
}

void CDurationReport::getAverageDuration(const CMobileElemConstraint &aConst,CStatisticalTools< ElapsedTime >& _tools)
{
	long double totalDuration = 0.0;
    minimum = maximum = average = 0l;
    count = 0;

	char title[1024] = "Calculating Average Durations for Pax Type ";
    aConst.screenPrint (title + strlen (title), 0, 800);
    //int progressBar = initBar (title, outTerm.paxLog->getCount());

    ReportPaxEntry element;
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
				CaculateEveryPax( element );
				ElapsedTime tValue (m_fromToProcsTools.GetTotalValue());
				if( tValue == 0l )
				{
					m_fromToProcsTools.ResetAll();
					continue;
				}
				_tools.AddNewData( tValue );
                if (tValue < minimum || minimum == 0l)
                    minimum = tValue;

                if (tValue > maximum)
                    maximum = tValue;

                totalDuration += tValue.asSeconds();
                count++;

				m_fromToProcsTools.ResetAll();				
            }
    }   

    if (count)
        average = (ElapsedTime) ((long)(totalDuration / count));
}
void CDurationReport::InitTools()
{
	m_fromToProcsTools.ClearAll();
	int iFromSize = m_fromToProcs.m_vFromProcs.size();
	int iToSize = m_fromToProcs.m_vToProcs.size();
	if( iFromSize >  0 )
	{
		for( int j=0; j<iFromSize; ++j )
		{
			GroupIndex groupIdx = m_pTerm->procList->getGroupIndex( m_fromToProcs.m_vFromProcs[j] );
			ASSERT( groupIdx.start >=0 );	
			if( iToSize > 0 )
			{
				for( int s=0; s<iToSize ; ++s )
				{
					GroupIndex toProcGroupIdx = m_pTerm->procList->getGroupIndex( m_fromToProcs.m_vToProcs[s] );
					ASSERT( toProcGroupIdx.start>=0 );
					for( int jj=groupIdx.start; jj<=groupIdx.end; ++jj )
					{
						for( int ss=toProcGroupIdx.start; ss<=toProcGroupIdx.end; ++ss )
						{
							m_fromToProcsTools.InitData( jj,ArriveAtServer,ss,LeaveServer );
						}				
					}
				}
			}	
			else
			{
				for( int jj=groupIdx.start; jj<=groupIdx.end; ++jj )
				{									
					m_fromToProcsTools.InitData( jj,ArriveAtServer,END_PROCESSOR_INDEX,LeaveServer  );					
				}
			}
		}
	}	
	else
	{
		if( iToSize > 0 )
		{
			for( int s=0; s<iToSize ; ++s )
			{
				GroupIndex toProcGroupIdx = m_pTerm->procList->getGroupIndex( m_fromToProcs.m_vToProcs[s] );
				ASSERT( toProcGroupIdx.start>=0 );
				for( int ss=toProcGroupIdx.start; ss<=toProcGroupIdx.end; ++ss )
				{
					m_fromToProcsTools.InitData( START_PROCESSOR_INDEX,ArriveAtServer,ss,LeaveServer );
				}				
			}
		}
		else
		{
			m_fromToProcsTools.InitData( START_PROCESSOR_INDEX,ArriveAtServer,END_PROCESSOR_INDEX ,LeaveServer );
		}
	}
	
}