// ReportParaOfTime.cpp: implementation of the CReportParaOfTime class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ReportParaOfTime.h"
#include "common\termfile.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportParaOfTime::CReportParaOfTime( CReportParameter* _pDecorator )
:CReportParameter( _pDecorator ),m_lInterval( 60l )
{
   m_endTime.setHours(24) ;
}

CReportParaOfTime::~CReportParaOfTime()
{
	if( m_pDecorator )
	{
		delete m_pDecorator;
		m_pDecorator = NULL;
	}
}
//**************************
// I have this kind of info ,so get mine and return true
//**************************
bool CReportParaOfTime::GetStartTime( ElapsedTime& _startTime )const
{
	_startTime = m_startTime;
	return true;
}

bool CReportParaOfTime::GetEndTime( ElapsedTime& _endTime )const
{
	_endTime = m_endTime;
	return true;
}

bool CReportParaOfTime::GetInterval ( long& _intervalTime )const
{
	_intervalTime = m_lInterval;
	return true;
}

bool CReportParaOfTime::SetStartTime( const ElapsedTime& _startTime )
{
	m_startTime = _startTime;
	return true;
}
bool CReportParaOfTime::SetEndTime( const ElapsedTime& _endTime )
{
	m_endTime = _endTime;
	return true;
}
bool CReportParaOfTime::SetInterval ( const long _intervalSecond )
{
	m_lInterval = _intervalSecond;
	return true;
}
//**************************
// end of my data
//**************************


//**************************
// I donnt have this kind of info ,so ask my decorator if he has such kind of info
//**************************
bool CReportParaOfTime::GetReportType ( int& _iReportType )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetReportType( _iReportType );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfTime::SetReportType ( int _iReportType  )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetReportType( _iReportType );
	}
	else
	{
		return false;
	}
}

//**********
//Threshold
bool CReportParaOfTime::GetThreshold ( long& _lThreshold )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetThreshold( _lThreshold );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfTime::SetThreshold ( long _lThreshold )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetThreshold( _lThreshold );
	}
	else
	{
		return false;
	}
}

//**********
//pax type, processor group
//**********
bool CReportParaOfTime::GetPaxType( std::vector<CMobileElemConstraint>& _vPaxType )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetPaxType( _vPaxType );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfTime::SetPaxType( const std::vector<CMobileElemConstraint>& _vProcGroup )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetPaxType( _vProcGroup );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfTime::GetProcessorGroup( std::vector<ProcessorID>& _vProcGroup )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetProcessorGroup( _vProcGroup );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfTime::SetProcessorGroup( const std::vector<ProcessorID>& _vProcGroup )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetProcessorGroup( _vProcGroup );
	}
	else
	{
		return false;
	}
}
//areas
bool CReportParaOfTime::SetAreas( const std::vector<CString>& _vAreas )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetAreas( _vAreas );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfTime::GetAreas(  std::vector<CString>& _vAreas )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetAreas( _vAreas );
	}
	else
	{
		return false;
	}
}

//portals
bool CReportParaOfTime::SetPortals( const std::vector<CString>& _vPortals )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetPortals( _vPortals );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfTime::GetPortals(  std::vector<CString>& _vPortals )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetPortals( _vPortals );
	}
	else
	{
		return false;
	}
}
//from---to processors
bool CReportParaOfTime::GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetFromToProcs( _fromToProcs );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfTime::SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetFromToProcs( _fromToProcs );
	}
	else
	{
		return false;
	}
}
//read ,write
void CReportParaOfTime::WriteClassType( ArctermFile& _file )
{
	_file.writeInt( Time_Para );
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassType( _file );
	}
}
void CReportParaOfTime::WriteClassData( ArctermFile& _file )
{

	_file.writeTime( m_startTime,true );
	_file.writeTime( m_endTime,true );
	_file.writeInt( m_lInterval );
	_file.writeLine();
	
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassData( _file );
	}
}
void CReportParaOfTime::ReadClassData( ArctermFile& _file, InputTerminal* _pTerm )
{
	_file.getLine();
	_file.getTime( m_startTime, true );
	_file.getTime( m_endTime, true );
	_file.getInteger( m_lInterval );

	if( m_pDecorator )
	{
		m_pDecorator->ReadClassData( _file , _pTerm);
	}
}

CReportParameter* CReportParaOfTime::Clone()
{
	CReportParameter* pSelf = new CReportParaOfTime( NULL );	
	pSelf->SetStartTime( m_startTime );
	pSelf->SetEndTime( m_endTime );
	pSelf->SetInterval( m_lInterval );
	pSelf->SetName( m_strName );
	pSelf->SetReportCategory( m_enReportCategory );
	if( m_pDecorator )
	{
		pSelf->SetDecorator( m_pDecorator->Clone() );
	}
	return pSelf;
}

bool CReportParaOfTime::IsEqual( const CReportParameter& _anotherInstance )
{
	ElapsedTime startTime;
	ElapsedTime endTime;
	long lInterval;
	if( !_anotherInstance.GetStartTime( startTime ) )
	{
		return false;
	}

	if( !_anotherInstance.GetEndTime( endTime ) )
	{
		return false;
	}

	if( !_anotherInstance.GetInterval( lInterval ) )
	{
		return false;
	}

	if( !( m_startTime == startTime ) )
	{
		return false;
	}

	if( !( endTime == endTime ) )
	{
		return false;
	}

	if( m_lInterval != lInterval )
	{
		return false;
	}

	if( m_pDecorator )
	{
		return m_pDecorator->IsEqual( _anotherInstance );
	}
	else
	{
		return true;
	}
}