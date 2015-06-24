// ReportParaOfPaxType.cpp: implementation of the CReportParaOfPaxType class.
//
//////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "ReportParaOfPaxType.h"
#include "common\termfile.h"

#include "../Inputs/IN_TERM.H"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportParaOfPaxType::CReportParaOfPaxType( CReportParameter* _pDecorator )
:CReportParameter( _pDecorator )
{

}

CReportParaOfPaxType::~CReportParaOfPaxType()
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
bool CReportParaOfPaxType ::GetPaxType( std::vector<CMobileElemConstraint>& _vPaxType )const
{
	_vPaxType = m_vPaxType;
	return true;
}

bool CReportParaOfPaxType ::SetPaxType( const std::vector<CMobileElemConstraint>& _vProcGroup )
{
	m_vPaxType =_vProcGroup ;
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
bool CReportParaOfPaxType ::GetStartTime( ElapsedTime& _startTime )const
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

bool CReportParaOfPaxType ::GetEndTime( ElapsedTime& _endTime )const
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

bool CReportParaOfPaxType ::GetInterval ( long& _intervalTime )const
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


bool CReportParaOfPaxType ::SetStartTime( const ElapsedTime& _startTime )
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
bool CReportParaOfPaxType ::SetEndTime( const ElapsedTime& _endTime )
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
bool CReportParaOfPaxType ::SetInterval ( const long _intervalSecond )
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
//**********
//Threshold
bool CReportParaOfPaxType ::GetThreshold ( long& _lThreshold )const
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
bool CReportParaOfPaxType ::SetThreshold ( long _lThreshold )
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
//report type
bool CReportParaOfPaxType ::GetReportType ( int& _iReportType )const
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
bool CReportParaOfPaxType ::SetReportType ( int _iReportType  )
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
//processor group
//**********

bool CReportParaOfPaxType ::GetProcessorGroup( std::vector<ProcessorID>& _vProcGroup )const
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
bool CReportParaOfPaxType ::SetProcessorGroup( const std::vector<ProcessorID>& _vProcGroup )
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
bool CReportParaOfPaxType::SetAreas( const std::vector<CString>& _vAreas )
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
bool CReportParaOfPaxType::GetAreas(  std::vector<CString>& _vAreas )const
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
bool CReportParaOfPaxType::SetPortals( const std::vector<CString>& _vPortals )
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
bool CReportParaOfPaxType::GetPortals(  std::vector<CString>& _vPortals )const
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
bool CReportParaOfPaxType::GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const
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
bool CReportParaOfPaxType::SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs )
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
void CReportParaOfPaxType::WriteClassType( ArctermFile& _file )
{
	_file.writeInt( PaxType_Para );
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassType( _file );
	}
}
void CReportParaOfPaxType::WriteClassData( ArctermFile& _file )
{
	int iSize = m_vPaxType.size();
	_file.writeInt( iSize );
	for( int i=0; i<iSize; ++i )
	{
		m_vPaxType[ i ].writeConstraint( _file );
	}
	_file.writeLine();
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassData( _file );
	}
}
void CReportParaOfPaxType::ReadClassData(  ArctermFile& _file, InputTerminal* _pTerm )
{
	int iSize;
	_file.getLine();
	_file.getInteger( iSize );
	for( int i=0; i<iSize; ++i )
	{
		CMobileElemConstraint temp(_pTerm);
		//temp.SetInputTerminal( _pTerm );
		temp.readConstraint( _file );
		m_vPaxType.push_back( temp );
	}

	if( m_pDecorator )
	{
		m_pDecorator->ReadClassData( _file , _pTerm);
	}
}

CReportParameter* CReportParaOfPaxType::Clone()
{
	CReportParameter* pSelf = new CReportParaOfPaxType( NULL );	
	pSelf->SetPaxType( m_vPaxType );
	pSelf->SetName( m_strName );
	pSelf->SetReportCategory( m_enReportCategory );
	if( m_pDecorator )
	{
		pSelf->SetDecorator( m_pDecorator->Clone() );
	}

	return pSelf;
}

bool CReportParaOfPaxType::IsEqual( const CReportParameter& _anotherInstance )
{
	std::vector<CMobileElemConstraint>vPaxType;
	if( _anotherInstance.GetPaxType( vPaxType ) )
	{
		/*
		if( m_vPaxType.size() == vPaxType.size() )
		{
			
		}
		else
		{
			return false;
		}
		*/
		if ( vPaxType == m_vPaxType )
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