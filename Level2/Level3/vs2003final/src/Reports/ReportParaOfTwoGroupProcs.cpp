// ReportParaOfTwoGroupProcs.cpp: implementation of the CReportParaOfTwoGroupProcs class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include "common\termfile.h"
#include "ReportParaOfTwoGroupProcs.h"
#include <CommonData/procid.h>
#include <Inputs/IN_TERM.H>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportParaOfTwoGroupProcs::CReportParaOfTwoGroupProcs(CReportParameter* _pDecorator )
:CReportParameter( _pDecorator )
{

}

CReportParaOfTwoGroupProcs::~CReportParaOfTwoGroupProcs()
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

//from---to processors
bool CReportParaOfTwoGroupProcs::GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const
{
	_fromToProcs = m_FromToProcs;
	return true;
}
bool CReportParaOfTwoGroupProcs::SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs )
{
	m_FromToProcs= _fromToProcs;
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
bool CReportParaOfTwoGroupProcs::GetStartTime( ElapsedTime& _startTime )const
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

bool CReportParaOfTwoGroupProcs::GetEndTime( ElapsedTime& _endTime )const
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

bool CReportParaOfTwoGroupProcs::GetInterval ( long& _intervalTime )const
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


bool CReportParaOfTwoGroupProcs::SetStartTime( const ElapsedTime& _startTime )
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
bool CReportParaOfTwoGroupProcs::SetEndTime( const ElapsedTime& _endTime )
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
bool CReportParaOfTwoGroupProcs::SetInterval ( const long _intervalSecond )
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
bool CReportParaOfTwoGroupProcs::GetReportType ( int& _iReportType )const
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
bool CReportParaOfTwoGroupProcs::SetReportType ( int _iReportType  )
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
bool CReportParaOfTwoGroupProcs::GetPaxType( std::vector<CMobileElemConstraint>& _vPaxType )const
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

bool CReportParaOfTwoGroupProcs::SetPaxType( const std::vector<CMobileElemConstraint>& _vProcGroup )
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
bool CReportParaOfTwoGroupProcs::GetProcessorGroup( std::vector<ProcessorID>& _vProcGroup )const
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
bool CReportParaOfTwoGroupProcs::SetProcessorGroup( const std::vector<ProcessorID>& _vProcGroup )
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
bool CReportParaOfTwoGroupProcs::SetAreas( const std::vector<CString>& _vAreas )
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
bool CReportParaOfTwoGroupProcs::GetAreas(  std::vector<CString>& _vAreas )const
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
bool CReportParaOfTwoGroupProcs::SetPortals( const std::vector<CString>& _vPortals )
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
bool CReportParaOfTwoGroupProcs::GetPortals(  std::vector<CString>& _vPortals )const
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
// thresh hold
bool CReportParaOfTwoGroupProcs::GetThreshold ( long& _lThreshold )const
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
bool CReportParaOfTwoGroupProcs::SetThreshold ( long _lThreshold )
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

//read ,write
void CReportParaOfTwoGroupProcs::WriteClassType( ArctermFile& _file )
{
	_file.writeInt( FromTo_Para );
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassType( _file );
	}
}
void CReportParaOfTwoGroupProcs::WriteClassData( ArctermFile& _file )
{	
	int iSize = m_FromToProcs.m_vFromProcs.size();
	_file.writeInt( iSize );
	for( int i=0; i<iSize; ++i )
	{
		m_FromToProcs.m_vFromProcs[i].writeProcessorID( _file );
	}
	_file.writeLine();

	iSize = m_FromToProcs.m_vToProcs.size();
	_file.writeInt( iSize );
	for( i=0; i<iSize; ++i )
	{
		m_FromToProcs.m_vToProcs[i].writeProcessorID( _file );
	}
	_file.writeLine();
	
	if( m_pDecorator )
	{
		m_pDecorator->WriteClassData( _file );
	}
}
void CReportParaOfTwoGroupProcs::ReadClassData( ArctermFile& _file, InputTerminal* _pTerm )
{
	_file.getLine();
	int iSize =0;
	_file.getInteger( iSize );
	ProcessorID temp;
	temp.SetStrDict( _pTerm->inStrDict );
	for( int i=0; i<iSize; ++i )
	{		
		temp.readProcessorID( _file );
		m_FromToProcs.m_vFromProcs.push_back( temp );
	}
	
	_file.getLine();
	iSize = 0;
	_file.getInteger( iSize );	
	for( i=0; i<iSize; ++i )
	{		
		temp.readProcessorID( _file );
		m_FromToProcs.m_vToProcs.push_back( temp );
	}
	

	if( m_pDecorator )
	{
		m_pDecorator->ReadClassData( _file , _pTerm);
	}
}

CReportParameter* CReportParaOfTwoGroupProcs::Clone()
{
	CReportParameter* pSelf = new CReportParaOfTwoGroupProcs( NULL );
	pSelf->SetFromToProcs( m_FromToProcs );	
	pSelf->SetName( m_strName );
	pSelf->SetReportCategory( m_enReportCategory );
	if( m_pDecorator )
	{
		pSelf->SetDecorator( m_pDecorator->Clone() );
	}

	return pSelf;
}

bool CReportParaOfTwoGroupProcs::IsEqual( const CReportParameter& _anotherInstance )
{
	CReportParaOfTwoGroupProcs::FROM_TO_PROCS tempProcs;
	if( _anotherInstance.GetFromToProcs( tempProcs ) )
	{
		if( tempProcs.m_vFromProcs.size() != m_FromToProcs.m_vFromProcs.size() )
		{
			return false;
		}

		if( tempProcs.m_vToProcs.size() != m_FromToProcs.m_vToProcs.size() )
		{
			return false;
		}

		for( unsigned i=0; i<m_FromToProcs.m_vFromProcs.size(); ++i )
		{
			bool bEqual = false;
			for( unsigned j=0; j<tempProcs.m_vFromProcs.size(); ++j )
			{
				if( m_FromToProcs.m_vFromProcs[i] == tempProcs.m_vFromProcs[ j ] )
				{
					bEqual = true;
					break;
				}
			}			

			if( !bEqual )
			{
				return false;
			}
		}


		for( i=0; i<m_FromToProcs.m_vToProcs.size(); ++i )
		{
			bool bEqual = false;
			for(  unsigned j=0; j<tempProcs.m_vToProcs.size(); ++j )
			{
				if( m_FromToProcs.m_vToProcs[i] == tempProcs.m_vToProcs[ j ] )
				{
					bEqual = true;
					break;
				}
			}						
			if( !bEqual )
			{
				return false;
			}
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
	else
	{
		return false;
	}
		
}