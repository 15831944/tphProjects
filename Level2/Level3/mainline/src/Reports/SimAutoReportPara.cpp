// SimAutoReportPara.cpp: implementation of the CSimAutoReportPara class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include"common\template.h"
#include "common\exeption.h"
#include "SimAutoReportPara.h"
#include "common\termfile.h"
#include <algorithm>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimAutoReportPara::CSimAutoReportPara( CStartDate _startDate )
:CSimGeneralPara( _startDate )
{

}

CSimAutoReportPara::~CSimAutoReportPara()
{

}
bool CSimAutoReportPara::IfExist( ENUM_REPORT_TYPE _enReport) 
{
	return std::find( m_vReports.begin(), m_vReports.end(), _enReport ) != m_vReports.end();
}

ENUM_REPORT_TYPE CSimAutoReportPara::operator [] ( int _iIdx )
{
	if( _iIdx<0 || (unsigned)_iIdx >= m_vReports.size() )
	{
		throw new OutOfRangeError("CSimAutoReportPara::operator []" );
	}
	return m_vReports[ _iIdx ];
}

ENUM_REPORT_TYPE CSimAutoReportPara::At ( int _iIdx ) const
{
	if( _iIdx<0 || (unsigned)_iIdx >= m_vReports.size() )
	{
		throw new OutOfRangeError("CSimAutoReportPara::At");
	}
	return m_vReports[ _iIdx ];
}
void CSimAutoReportPara::WriteData( ArctermFile& _file )
{
	CSimGeneralPara::WriteData( _file );
	int iSize = m_vReports.size();
	_file.writeInt(iSize);
	for( int i=0; i<iSize; ++i )
	{
		_file.writeInt( m_vReports[i] );
	}

	_file.writeLine();
}
void CSimAutoReportPara::ReadData( ArctermFile& _file ,InputTerminal* _interm )
{
	CSimGeneralPara::ReadData( _file, _interm );
	int iSize = -1;
	_file.getLine();
	_file.getInteger( iSize );
	int iType =-1;
	for( int i=0; i<iSize ; ++i )
	{
		_file.getInteger( iType );
		m_vReports.push_back( (ENUM_REPORT_TYPE)iType );
	}
}