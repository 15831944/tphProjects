// ReportParaOfThreshold.cpp: implementation of the CReportParaOfThreshold class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ReportParaOfThreshold.h"
#include "common\termfile.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportParaOfThreshold::CReportParaOfThreshold(CReportParameter* _pDecorator)
:CReportParameter( _pDecorator ),m_lThreshold( 0 )
{

}

CReportParaOfThreshold::~CReportParaOfThreshold()
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
//Threshold
bool CReportParaOfThreshold::GetThreshold ( long& _lThreshold )const
{
	_lThreshold = m_lThreshold;
	return true;
}
bool CReportParaOfThreshold::SetThreshold ( long _lThreshold )
{
	m_lThreshold = _lThreshold;
	return true;
}


//**************************
// end of my data
//**************************



//**************************
// I donnt have this kind of info ,so ask my decorator if he has such kind of info
//**************************

//********** 
//startime , end time, interval
//**********
bool CReportParaOfThreshold::GetStartTime( ElapsedTime& _startTime )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetStartTime( _startTime );
	}
	else
	{
		return false;
	}
}

bool CReportParaOfThreshold::GetEndTime( ElapsedTime& _endTime )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetEndTime( _endTime );
	}
	else
	{
		return false;
	}
}

bool CReportParaOfThreshold::GetInterval ( long& _intervalTime )const
{
	if( m_pDecorator )
	{
		return m_pDecorator->GetInterval( _intervalTime );
	}
	else
	{
		return false;
	}
}


bool CReportParaOfThreshold::SetStartTime( const ElapsedTime& _startTime )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetStartTime( _startTime );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfThreshold::SetEndTime( const ElapsedTime& _endTime )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetEndTime( _endTime );
	}
	else
	{
		return false;
	}
}
bool CReportParaOfThreshold::SetInterval ( const long _intervalSecond )
{
	if( m_pDecorator )
	{
		return m_pDecorator->SetInterval( _intervalSecond );
	}
	else
	{
		return false;
	}
}
//**********report type
bool CReportParaOfThreshold::GetReportType ( int& _iReportType )const
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
bool CReportParaOfThreshold::SetReportType ( int _iReportType  )
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
//pax type, processor group
//**********
bool CReportParaOfThreshold::GetPaxType( std::vector<CMobileElemConstraint>& _vPaxType )const
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

bool CReportParaOfThreshold::SetPaxType( const std::vector<CMobileElemConstraint>& _vProcGroup )
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
bool CReportParaOfThreshold::GetProcessorGroup( std::vector<ProcessorID>& _vProcGroup )const
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
bool CReportParaOfThreshold::SetProcessorGroup( const std::vector<ProcessorID>& _vProcGroup )
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
bool CReportParaOfThreshold::SetAreas( const std::vector<CString>& _vAreas )
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
bool CReportParaOfThreshold::GetAreas(  std::vector<CString>& _vAreas )const
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
bool CReportParaOfThreshold::SetPortals( const std::vector<CString>& _vPortals )
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
bool CReportParaOfThreshold::GetPortals(  std::vector<CString>& _vPortals )const
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
bool CReportParaOfThreshold::GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const
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
bool CReportParaOfThreshold::SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs )
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
void CReportParaOfThreshold::WriteClassType( ArctermFile& _file )
{
	_file.writeInt( Threshold_Para );
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassType( _file );
	}
}
void CReportParaOfThreshold::WriteClassData( ArctermFile& _file )
{
	
	_file.writeInt( m_lThreshold );
	_file.writeLine();
	
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassData( _file );
	}
}
void CReportParaOfThreshold::ReadClassData( ArctermFile& _file, InputTerminal* _pTerm )
{
	_file.getLine();
	_file.getInteger( m_lThreshold );

	if( m_pDecorator )
	{
		m_pDecorator->ReadClassData( _file , _pTerm);
	}
}

CReportParameter* CReportParaOfThreshold::Clone()
{
	CReportParameter* pSelf = new CReportParaOfThreshold( NULL );	
	pSelf->SetThreshold( m_lThreshold );
	pSelf->SetName( m_strName );
	pSelf->SetReportCategory( m_enReportCategory );
	if( m_pDecorator )
	{
		pSelf->SetDecorator( m_pDecorator->Clone() );
	}

	return pSelf;
}

bool CReportParaOfThreshold::IsEqual( const CReportParameter& _anotherInstance )
{
	long lThreshold;
	if( _anotherInstance.GetThreshold( lThreshold ) )
	{
		if( m_lThreshold == lThreshold )
		{
			if( m_pDecorator )
			{
				return m_pDecorator->IsEqual( _anotherInstance );
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
		
}