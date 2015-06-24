#include "StdAfx.h"
#include ".\reportparaofruns.h"
#include "common\termfile.h"

CReportParaOfRuns::CReportParaOfRuns( CReportParameter* _pDecorator )
:CReportParameter( _pDecorator )
{

}
CReportParaOfRuns::~CReportParaOfRuns(void)
{
}



bool CReportParaOfRuns::GetReportRuns ( std::vector<int>& vReportRuns )const
{
	vReportRuns = m_vReportRuns;
	return true;
}
bool CReportParaOfRuns::SetReportRuns ( const std::vector<int>& vReportRuns  )
{
	m_vReportRuns = vReportRuns;
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
bool CReportParaOfRuns::GetStartTime( ElapsedTime& _startTime )const
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

bool CReportParaOfRuns::GetEndTime( ElapsedTime& _endTime )const
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

bool CReportParaOfRuns::GetInterval ( long& _intervalTime )const
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


bool CReportParaOfRuns::SetStartTime( const ElapsedTime& _startTime )
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
bool CReportParaOfRuns::SetEndTime( const ElapsedTime& _endTime )
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
bool CReportParaOfRuns::SetInterval ( const long _intervalSecond )
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
bool CReportParaOfRuns::GetThreshold ( long& _lThreshold )const
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
bool CReportParaOfRuns::SetThreshold ( long _lThreshold )
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
bool CReportParaOfRuns::GetPaxType( std::vector<CMobileElemConstraint>& _vPaxType )const
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

bool CReportParaOfRuns::SetPaxType( const std::vector<CMobileElemConstraint>& _vProcGroup )
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
bool CReportParaOfRuns::GetProcessorGroup( std::vector<ProcessorID>& _vProcGroup )const
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
bool CReportParaOfRuns::SetProcessorGroup( const std::vector<ProcessorID>& _vProcGroup )
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
bool CReportParaOfRuns::SetAreas( const std::vector<CString>& _vAreas )
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
bool CReportParaOfRuns::GetAreas(  std::vector<CString>& _vAreas )const
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
bool CReportParaOfRuns::SetPortals( const std::vector<CString>& _vPortals )
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
bool CReportParaOfRuns::GetPortals(  std::vector<CString>& _vPortals )const
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
bool CReportParaOfRuns::GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const
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
bool CReportParaOfRuns::SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs )
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
//**********report type
bool CReportParaOfRuns::GetReportType ( int& _iReportType )const
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
bool CReportParaOfRuns::SetReportType ( int _iReportType  )
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

//read ,write
void CReportParaOfRuns::WriteClassType( ArctermFile& _file )
{
	_file.writeInt( Runs_Para );
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassType( _file );
	}
}
void CReportParaOfRuns::WriteClassData( ArctermFile& _file )
{
	int nRunsCount = static_cast<int>(m_vReportRuns.size());
	_file.writeInt(nRunsCount);
	for (int i =0; i < nRunsCount; ++i)
	{
		_file.writeInt(m_vReportRuns.at(i));
	}
	_file.writeLine();

	if( m_pDecorator )
	{
		m_pDecorator->WriteClassData( _file );
	}
}
void CReportParaOfRuns::ReadClassData( ArctermFile& _file, InputTerminal* _pTerm )
{
	_file.getLine();
	int nRunsCount;
	_file.getInteger(nRunsCount);
	for (int i = 0; i < nRunsCount; ++ i)
	{
		int nRun = 0;
		_file.getInteger(nRun);
		m_vReportRuns.push_back(nRun);
	}
	//_file.getInteger( m_iReportType );

	if( m_pDecorator )
	{
		m_pDecorator->ReadClassData( _file , _pTerm);
	}
}


CReportParameter* CReportParaOfRuns::Clone()
{
	CReportParameter* pSelf = new CReportParaOfRuns( NULL );	
	//pSelf->SetReportType( m_iReportType );
	pSelf->SetReportRuns(m_vReportRuns);
	pSelf->SetName( m_strName );
	pSelf->SetReportCategory( m_enReportCategory );
	if( m_pDecorator )
	{
		pSelf->SetDecorator( m_pDecorator->Clone() );
	}

	return pSelf;
}

bool CReportParaOfRuns::IsEqual( const CReportParameter& _anotherInstance )
{
	std::vector<int> vAnotherInstanceRuns;
	if( _anotherInstance.GetReportRuns( vAnotherInstanceRuns ) )
	{
		if( m_vReportRuns == vAnotherInstanceRuns )
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

bool CReportParaOfRuns::IsMultiRunReport()
{
	if(m_vReportRuns.size() > 1)
		return true;

	return false;
}