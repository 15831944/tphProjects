#include "StdAfx.h"
#include ".\fromtoproccaculationwithmode.h"
#include "engine\proclist.h"
CFromToProcCaculationWithMode::CFromToProcCaculationWithMode(void)
{
}

CFromToProcCaculationWithMode::~CFromToProcCaculationWithMode(void)
{
}

void CFromToProcCaculationWithMode::InitData( int _iFromProc,int fromMode,int _iToProc,int toMode )
{

		FROM_TO_PROC temp;
		temp.m_iFromProc = _iFromProc;
		temp.FromProcState = fromMode;
		temp.m_iToProc = _iToProc;
		temp.ToProcState = toMode;
		temp.m_enFlag = INVALID;
		temp.m_dValue = 0.0;
		m_vValues.push_back( temp );

}

void CFromToProcCaculationWithMode::AddValue( int _iProc ,int paxMode ,double _dValue )
{
	for( unsigned i=0; i<m_vValues.size(); ++i )
	{
		//		// TRACE( "%d --> %d  %d  --  %.2f\n", m_vValues[i].m_iFromProc, m_vValues[i].m_iToProc, _iProc, _dValue );
		if( m_vValues[i].m_iFromProc == _iProc && 
			m_vValues[i].m_enFlag == INVALID && 
			m_vValues[i].FromProcState == paxMode)
		{
			m_vValues[i].m_enFlag = START_COMPUTE;
			m_vValues[i].m_dValue= _dValue;
		}
		else if ( m_vValues[i].m_iToProc == _iProc &&
			m_vValues[i].m_enFlag == START_COMPUTE &&
			m_vValues[i].ToProcState == paxMode)
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
			m_vValues[i].m_enFlag = START_COMPUTE;
		}
	}
	
}