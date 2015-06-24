// ReportParaWithArea.cpp: implementation of the CReportParaWithArea class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include "ReportParaWithArea.h"
#include "common\termfile.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportParaWithArea::CReportParaWithArea(CReportParameter* _pDecorator )
:CReportParameter( _pDecorator )
{

}

CReportParaWithArea::~CReportParaWithArea()
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
bool CReportParaWithArea::SetAreas( const std::vector<CString>& _vAreas )
{
	m_vAreas = _vAreas;
	return true;
}
bool CReportParaWithArea::GetAreas( std::vector<CString>& _vAreas )const
{
	_vAreas = m_vAreas;
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
bool CReportParaWithArea::GetStartTime( ElapsedTime& _startTime )const
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

bool CReportParaWithArea::GetEndTime( ElapsedTime& _endTime )const
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

bool CReportParaWithArea::GetInterval ( long& _intervalTime )const
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


bool CReportParaWithArea::SetStartTime( const ElapsedTime& _startTime )
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
bool CReportParaWithArea::SetEndTime( const ElapsedTime& _endTime )
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
bool CReportParaWithArea::SetInterval ( const long _intervalSecond )
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
bool CReportParaWithArea::GetReportType ( int& _iReportType )const
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
bool CReportParaWithArea::SetReportType ( int _iReportType  )
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
//Threshold
bool CReportParaWithArea::GetThreshold ( long& _lThreshold )const
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
bool CReportParaWithArea::SetThreshold ( long _lThreshold )
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
bool CReportParaWithArea::GetPaxType( std::vector<CMobileElemConstraint>& _vPaxType )const
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

bool CReportParaWithArea::SetPaxType( const std::vector<CMobileElemConstraint>& _vProcGroup )
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
bool CReportParaWithArea::GetProcessorGroup( std::vector<ProcessorID>& _vProcGroup )const
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
bool CReportParaWithArea::SetProcessorGroup( const std::vector<ProcessorID>& _vProcGroup )
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
//portals
bool CReportParaWithArea::SetPortals( const std::vector<CString>& _vPortals )
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
bool CReportParaWithArea::GetPortals(  std::vector<CString>& _vPortals )const
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
bool CReportParaWithArea::GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const
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
bool CReportParaWithArea::SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs )
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
void CReportParaWithArea::WriteClassType( ArctermFile& _file )
{
	_file.writeInt( Areas_Para );
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassType( _file );
	}
}
void CReportParaWithArea::WriteClassData( ArctermFile& _file )
{
	int iSize = m_vAreas.size();
	_file.writeInt( iSize);

	for( int i=0; i<iSize; ++i )
	{
		_file.writeField( m_vAreas[i] );
	}
	_file.writeLine();
	
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassData( _file );
	}
}
void CReportParaWithArea::ReadClassData( ArctermFile& _file, InputTerminal* _pTerm )
{
	_file.getLine();

	int iSize;
	_file.getInteger( iSize );
	char tempStr[256];
	for( int i=0; i<iSize; ++i )
	{
		_file.getField( tempStr, 256 );
		m_vAreas.push_back( CString( tempStr) );
	}	

	if( m_pDecorator )
	{
		m_pDecorator->ReadClassData( _file , _pTerm);
	}
}

CReportParameter* CReportParaWithArea::Clone()
{
	CReportParameter* pSelf = new CReportParaWithArea( NULL );	
	pSelf->SetAreas( m_vAreas );
	
	pSelf->SetName( m_strName );
	pSelf->SetReportCategory( m_enReportCategory );
	if( m_pDecorator )
	{
		pSelf->SetDecorator( m_pDecorator->Clone() );
	}
	return pSelf;
}

bool CReportParaWithArea::IsEqual( const CReportParameter& _anotherInstance )
{
	std::vector<CString> vAreas;
	if( _anotherInstance.GetAreas( vAreas ) )
	{
		if( vAreas == m_vAreas )
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