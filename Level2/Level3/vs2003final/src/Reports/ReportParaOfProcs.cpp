// ReportParaOfProcs.cpp: implementation of the CReportParaOfProcs class.
//
//////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "ReportParaOfProcs.h"
#include "common\termfile.h"
#include <Inputs/in_term.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportParaOfProcs::CReportParaOfProcs(CReportParameter* _pDecorator)
:CReportParameter( _pDecorator )
{

}

CReportParaOfProcs::~CReportParaOfProcs()
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
bool CReportParaOfProcs::GetProcessorGroup( std::vector<ProcessorID>& _vProcGroup )const
{
	_vProcGroup = m_vProcGroups;
	return true;
}
bool CReportParaOfProcs::SetProcessorGroup( const std::vector<ProcessorID>& _vProcGroup )
{
	m_vProcGroups = _vProcGroup;
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
bool CReportParaOfProcs::GetStartTime( ElapsedTime& _startTime )const
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

bool CReportParaOfProcs::GetEndTime( ElapsedTime& _endTime )const
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

bool CReportParaOfProcs::GetInterval ( long& _intervalTime )const
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


bool CReportParaOfProcs::SetStartTime( const ElapsedTime& _startTime )
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
bool CReportParaOfProcs::SetEndTime( const ElapsedTime& _endTime )
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
bool CReportParaOfProcs::SetInterval ( const long _intervalSecond )
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
bool CReportParaOfProcs::GetThreshold ( long& _lThreshold )const
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
bool CReportParaOfProcs::SetThreshold ( long _lThreshold )
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
bool CReportParaOfProcs::GetReportType ( int& _iReportType )const
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
bool CReportParaOfProcs::SetReportType ( int _iReportType  )
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
//pax type, 
//**********
bool CReportParaOfProcs::GetPaxType( std::vector<CMobileElemConstraint>& _vPaxType )const
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

bool CReportParaOfProcs::SetPaxType( const std::vector<CMobileElemConstraint>& _vProcGroup )
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
//areas
bool CReportParaOfProcs::SetAreas( const std::vector<CString>& _vAreas )
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
bool CReportParaOfProcs::GetAreas(  std::vector<CString>& _vAreas )const
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
bool CReportParaOfProcs::SetPortals( const std::vector<CString>& _vPortals )
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
bool CReportParaOfProcs::GetPortals(  std::vector<CString>& _vPortals )const
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
bool CReportParaOfProcs::GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const
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
bool CReportParaOfProcs::SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs )
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
void CReportParaOfProcs::WriteClassType( ArctermFile& _file )
{
	_file.writeInt( ProcGroup_Para );
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassType( _file );
	}
}
void CReportParaOfProcs::WriteClassData( ArctermFile& _file )
{
	int iSize = m_vProcGroups.size();
	_file.writeInt( iSize );
	for( int i=0; i<iSize; ++i )
	{
		m_vProcGroups[ i ].writeProcessorID( _file );
	}
	_file.writeLine();
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassData( _file );
	}
}
void CReportParaOfProcs::ReadClassData( ArctermFile& _file, InputTerminal* _pTerm )
{
	int iSize;
	_file.getLine();
	_file.getInteger( iSize );

	ProcessorID temp;
	temp.SetStrDict( _pTerm->inStrDict );
	for( int i=0; i<iSize; ++i )
	{
		temp.readProcessorID( _file );
		m_vProcGroups.push_back( temp );
	}

	if( m_pDecorator )
	{
	m_pDecorator->ReadClassData( _file , _pTerm);
	}
}


CReportParameter* CReportParaOfProcs::Clone()
{
	CReportParameter* pSelf = new CReportParaOfProcs( NULL );	
	pSelf->SetProcessorGroup( m_vProcGroups );
	pSelf->SetName( m_strName );
	pSelf->SetReportCategory( m_enReportCategory );
	if( m_pDecorator )
	{
		pSelf->SetDecorator( m_pDecorator->Clone() );
	}

	return pSelf;
}

bool CReportParaOfProcs::IsEqual( const CReportParameter& _anotherInstance )
{
	std::vector<ProcessorID>vProcGroups;
	if( _anotherInstance.GetProcessorGroup( vProcGroups ) )
	{
		if( m_vProcGroups == vProcGroups )
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