// DistanceReport.cpp: implementation of the CDistanceReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "DistanceReport.h"
#include "DistElem.h"
#include "results\out_term.h"
#include "results\paxlog.h"
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

CDistanceReport::CDistanceReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}

CDistanceReport::~CDistanceReport()
{

}
void CDistanceReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	_pPara->GetFromToProcs( m_fromToProcs );
}

void CDistanceReport::GenerateSummary( ArctermFile& p_file )
{
	p_file.writeField ("Passenger Type,Minimum,Average,Maximum,Count,Q1,Q2,Q3,P1,P5,P10,P90,P95,P99");
	p_file.writeLine();

	char str[MAX_PAX_TYPE_LEN];
	const CMobileElemConstraint* pConst;

	CProgressBar bar( "Generating Passenger Distance Report", 100, m_multiConst.getCount(), TRUE );
	
	for (int i = 0; i < m_multiConst.getCount(); i++)
    {
		bar.StepIt();
		pConst = m_multiConst.getConstraint( i );

		CStatisticalTools<double> tempTools;
		GetAverageDistance( *pConst , tempTools );
		tempTools.SortData();


		pConst->screenPrint( str, 0, 40 );
		p_file.writeField( str );
		p_file.writeDouble( m_ldMinimum, 2 );
		p_file.writeDouble( m_ldAverage, 2 );
		p_file.writeDouble( m_ldMaximum, 2 );
		p_file.writeInt( m_nCount );	
		
		p_file.writeDouble( tempTools.GetPercentile( 25 ) , 2 );//Q1
		p_file.writeDouble( tempTools.GetPercentile( 50 ), 2 );//Q2
		p_file.writeDouble( tempTools.GetPercentile( 75 ), 2 );//Q3
		
		p_file.writeDouble( tempTools.GetPercentile( 1 ), 2 );//P1
		p_file.writeDouble( tempTools.GetPercentile( 5 ), 2 );//P5
		p_file.writeDouble( tempTools.GetPercentile( 10 ), 2 );//P10

		p_file.writeDouble( tempTools.GetPercentile( 90 ), 2 );//P90
		p_file.writeDouble( tempTools.GetPercentile( 95 ), 2 );//P95
		p_file.writeDouble( tempTools.GetPercentile( 99 ), 2 );//P99

		p_file.writeDouble( tempTools.GetSigma() , 2 );//sigma
		
		
		p_file.writeLine();

	}
//    if( paxTypeCount > 1)
//        completeBar (progressBar);
}

void CDistanceReport::InitTools()
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
							m_fromToProcsTools.InitData( jj,ss );
						}				
					}
				}
			}	
			else
			{
				for( int jj=groupIdx.start; jj<=groupIdx.end; ++jj )
				{									
					m_fromToProcsTools.InitData( jj,END_PROCESSOR_INDEX );					
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
					m_fromToProcsTools.InitData( START_PROCESSOR_INDEX,ss );
				}				
			}
		}
		else
		{
			m_fromToProcsTools.InitData( START_PROCESSOR_INDEX,END_PROCESSOR_INDEX );
		}
	}
	
}
void CDistanceReport::GenerateDetailed( ArctermFile& p_file )
{	
	InitTools();
	p_file.writeField ("Passenger #,Distance (m),Passenger Type");
	p_file.writeLine();

	DistanceElement element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	long paxCount = m_pTerm->paxLog->getCount();

	CProgressBar bar( "Generating Passenger Distance Report", 100, paxCount, TRUE );

	for( long i = 0; i < paxCount; i++ )
	{
		bar.StepIt();
		m_pTerm->paxLog->getItem( element, i );
		element.clearLog();
		if( element.alive( m_startTime, m_endTime ) )
			if( element.fits( m_multiConst ) )
			{
				element.calculateDistance( m_fromToProcsTools );
				element.writeEntry( p_file,m_fromToProcsTools,m_pTerm );
				m_fromToProcsTools.ResetAll();
			}
	}
//	completeBar (progressBar);
}


void CDistanceReport::GetAverageDistance(const CMobileElemConstraint &_aConst,CStatisticalTools<double>& _tools)
{
	InitTools();
	long double totalDistance = 0.0;
	m_ldMinimum = m_ldMaximum = m_ldAverage = 0.0;
	m_nCount = 0;

//	char title[80] = "Calculating Average Distances for Pax Type ";
//	aConst.screenPrint (title + strlen (title), MAX_PAX_TYPE_LEN);
//	int progressBar = initBar (title, (long) outTerm.paxLog->getCount());

	DistanceElement element;
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	long paxCount = m_pTerm->paxLog->getCount();
	for (long i = 0; i < paxCount; i++)
	{
//		updateBar (progressBar, i);
		m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
		element.SetOutputTerminal( m_pTerm );
		if (element.alive (m_startTime, m_endTime) && element.fits (_aConst))
		{
			element.calculateDistance( m_fromToProcsTools );

			//_tools.AddNewData( element.getDistance()/SCALE_FACTOR );
			double dTotalDistance = m_fromToProcsTools.GetTotalValue();
			if( dTotalDistance <= 0.0 )
			{
				m_fromToProcsTools.ResetAll();
				continue;
			}
			_tools.AddNewData( dTotalDistance / SCALE_FACTOR );
			if (dTotalDistance < m_ldMinimum )
				m_ldMinimum =dTotalDistance;

			if (dTotalDistance > m_ldMaximum)
				m_ldMaximum = dTotalDistance;

			totalDistance += dTotalDistance;
			m_nCount++;
			m_fromToProcsTools.ResetAll();

		}		
	}
//	completeBar (progressBar);

	if (m_nCount)
	{
		m_ldMinimum /= 100.0;
		m_ldMaximum /= 100.0;
		totalDistance /= 100.0;
		m_ldAverage = totalDistance / m_nCount;
	}	

}
