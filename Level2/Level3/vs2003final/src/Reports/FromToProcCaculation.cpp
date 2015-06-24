// FromToProcCaculation.cpp: implementation of the CFromToProcCaculation class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FromToProcCaculation.h"
#include "engine\proclist.h"
#include "Common\States.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFromToProcCaculation::CFromToProcCaculation()
{

}

CFromToProcCaculation::~CFromToProcCaculation()
{

}
void CFromToProcCaculation::InitData( int _iFrom, int _iTo )
{
	FROM_TO_PROC temp;
	temp.m_iFromProc = _iFrom;
	temp.m_iToProc = _iTo;
	temp.m_enFlag = INVALID;
	temp.m_dValue = 0.0;
	m_vValues.push_back( temp );
}
void CFromToProcCaculation::AddValue( int _iProc , double _dValue , int enumState )
{
	for( unsigned i=0; i<m_vValues.size(); ++i )
	{
//		// TRACE( "%d --> %d  %d  --  %.2f\n", m_vValues[i].m_iFromProc, m_vValues[i].m_iToProc, _iProc, _dValue );
		if( m_vValues[i].m_iFromProc == _iProc && m_vValues[i].m_enFlag == INVALID &&enumState == ArriveAtServer )
		{
			m_vValues[i].m_enFlag = START_COMPUTE;
			m_vValues[i].m_dValue= _dValue;
		}
		else if ( m_vValues[i].m_iToProc == _iProc && m_vValues[i].m_enFlag == START_COMPUTE &&enumState == ArriveAtServer)
		{
			m_vValues[i].m_enFlag = END_COMPUTE;			
		}
		else if ( m_vValues[i].m_enFlag == START_COMPUTE )
		{
			m_vValues[i].m_dValue += _dValue;
		}
		else if( m_vValues[i].m_iFromProc == START_PROCESSOR_INDEX )
		{
			m_vValues[i].m_dValue += _dValue;
		}
	}
}
void CFromToProcCaculation::ResetAll()
{
	for( unsigned i=0; i<m_vValues.size(); ++i )
	{						
		m_vValues[i].m_dValue = 0.0;		
		m_vValues[i].m_enFlag = INVALID;		
	}
}
double CFromToProcCaculation::GetTotalValue()const
{
	double dReturnValue = 0.0;
	for( unsigned i=0; i<m_vValues.size(); ++i )
	{
		if( m_vValues[i].m_enFlag == END_COMPUTE || m_vValues[i].m_iToProc == END_PROCESSOR_INDEX )
		{		
			dReturnValue += m_vValues[i].m_dValue;
		}		
	}

	return dReturnValue;
}