// FireEvacutionReport.cpp: implementation of the CFireEvacutionReport class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FireEvacutionReport.h"
#include "Reports.h"
#include "FireEvacuationReportElement.h"
#include "results\out_term.h"
#include "results\paxlog.h"
#include "engine\terminal.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFireEvacutionReport::CFireEvacutionReport( Terminal* _pTerm, const CString& _strProjPath)
					: CBaseReport( _pTerm, _strProjPath), m_ldMinimum(0.0), m_ldMaximum(0.0), m_ldAverage(0.0), m_nCount(0)
{

}

CFireEvacutionReport::~CFireEvacutionReport()
{

}

// generate detail report
void CFireEvacutionReport::GenerateDetailed( ArctermFile& p_file )
{
	p_file.writeField("Passenger ID, Passenger Type, Exit Processor Index, Exit Time, Exit Duration Time");
	p_file.writeLine( );

	FireEvacuationReportElement element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );

	long paxCount = m_pTerm->paxLog->getCount();
	
	CProgressBar bar( "Generating Fire Evacuation Report(Detail)", 100, paxCount, TRUE );

	for( long i = 0l; i < paxCount; i++ )
	{
		bar.StepIt();
		m_pTerm->paxLog->getItem( element, i );
		element.clearLog();
		if( element.alive( m_startTime, m_endTime) && element.fits( m_multiConst) )
		{
			if( !element.getDataFromPaxLog() )
				continue;
			element.writeEntry( p_file, m_pTerm );
		}
	}
}

// generate summary report
void CFireEvacutionReport::GenerateSummary( ArctermFile& p_file )
{
	char _szBuf[ MAX_PAX_TYPE_LEN ];
	const CMobileElemConstraint* _pConst;

	p_file.writeField("Passenger Type,Minimum,Average,Maximum,Count,Q1,Q2,Q3,P1,P5,P10,P90,P95,P99");
	p_file.writeLine();
	
	CProgressBar _bar("Generating Fire Evacuation Report(Summary)", 100, m_multiConst.getCount(), TRUE );
	for( int i=0; i< m_multiConst.getCount(); i++ )
	{
		_bar.StepIt();
		_pConst = m_multiConst.getConstraint( i );
		
		CStatisticalTools<double> _tempTools;
		if( !GetAverageValue( *_pConst, _tempTools ) )
			continue;

		_tempTools.SortData();

		_pConst->screenPrint( _szBuf, 0, 40 );
		p_file.writeField( _szBuf );
		p_file.writeTime( ElapsedTime((double)m_ldMinimum/TimePrecision),TRUE );
		p_file.writeTime( ElapsedTime((double)m_ldAverage/TimePrecision),TRUE );
		p_file.writeTime( ElapsedTime((double)m_ldMaximum/TimePrecision),TRUE );
		p_file.writeInt( m_nCount );

		p_file.writeTime( ElapsedTime(_tempTools.GetPercentile( 25 )/TimePrecision ),TRUE );//Q1
		p_file.writeTime( ElapsedTime(_tempTools.GetPercentile( 50 )/TimePrecision ),TRUE );//Q2
		p_file.writeTime( ElapsedTime(_tempTools.GetPercentile( 75 )/TimePrecision ),TRUE );//Q3
		
		p_file.writeTime( ElapsedTime(_tempTools.GetPercentile( 1 )/TimePrecision ),TRUE );//P1
		p_file.writeTime( ElapsedTime(_tempTools.GetPercentile( 5 )/TimePrecision ),TRUE );//P5
		p_file.writeTime( ElapsedTime(_tempTools.GetPercentile( 10 )/TimePrecision ),TRUE );//P10
		
		p_file.writeTime( ElapsedTime(_tempTools.GetPercentile( 90 )/TimePrecision ),TRUE );//P90
		p_file.writeTime( ElapsedTime(_tempTools.GetPercentile( 95 )/TimePrecision ),TRUE );//P95
		p_file.writeTime( ElapsedTime(_tempTools.GetPercentile( 99 )/TimePrecision ),TRUE );//P99
		
		p_file.writeTime( ElapsedTime(_tempTools.GetSigma()/TimePrecision ),TRUE );//sigma
		
		p_file.writeLine();
		
	}
}

bool CFireEvacutionReport::GetAverageValue( const CMobileElemConstraint& _aConst ,CStatisticalTools<double>& _tools )
{
	long double _ldTottalExitTime = 0.0;
	m_ldMinimum	= 0.0;
	m_ldMaximum	= 0.0;
	m_ldAverage	= 0.0;
	m_nCount	= 0;

	FireEvacuationReportElement element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );

	long paxCount = m_pTerm->paxLog->getCount();
	for( long i=0; i< paxCount; i++ )
	{
		m_pTerm->paxLog->getItem( element, i );
		element.clearLog();
		if( element.alive( m_startTime, m_endTime ) && element.fits( _aConst) )
		{
			if( !element.getDataFromPaxLog() )
				continue;

			_tools.AddNewData( element.getExitDuration() );
			m_ldMinimum = min( element.getExitDuration(), m_ldMinimum );
			m_ldMaximum	= max( element.getExitDuration(), m_ldMaximum );

			_ldTottalExitTime += element.getExitDuration();
			m_nCount ++ ;
		}
	}
	
	if( m_nCount<=0 )
		return false;
	
	m_ldAverage = _ldTottalExitTime / m_nCount;
	return true;
}