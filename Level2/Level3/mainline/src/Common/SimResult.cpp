// SimResult.cpp: implementation of the CSimResult class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "SimResult.h"
#include "common\termfile.h"
#include "common\mutilreport.h"
#include "common\fileman.h"
#include "common\exeption.h"
#include "inputs\SimParameter.h"
#include "reports\SimAutoReportPara.h"
#include "reports\SimLevelOfService.h"


extern CString arrayReortName[];
extern LOGTOFILENAME inputfilename[];
#define INPUT_FILE_COUNT 40 // Whenever change input file array, must change this value 

CSimItem::CSimItem()
{
}

CSimItem::~CSimItem()
{
}

CSimItem::CSimItem( const CString& _strPath, const CString& _strSubSimName  ):m_reports( _strPath + "\\" + strReportSetName )
{
	m_strLogDirectoryName = _strPath;
	m_strSubSimName = _strSubSimName;
	initLogsName();
}

// init logs file name by array
void CSimItem::initLogsName()
{
	for( int i= 0; i<INPUT_FILE_COUNT; i++ )
	{
		CString strLogsName = m_strLogDirectoryName + "\\" + strLogPath + "\\" + inputfilename[i]._strFileName;
		m_logs[ (InputFiles)inputfilename[i]._iLogType ] = strLogsName;
	}
	
	return;
}

bool CSimItem::getLogsName( int _iLogsType, CString& _strLogsName )
{
	std::map< InputFiles, CString > ::const_iterator iter;
	iter = m_logs.find( (InputFiles)_iLogsType );
	if( iter != m_logs.end() )	// find the key
	{
		_strLogsName = iter->second;
		return true;
	}
	return false;				// can not find the key
}

void CSimItem::readFromFile( ArctermFile& file)
{
	// need not to read logs
	char szBuf[256];

	file.getLine();
	file.getField( szBuf, 256 );
	m_strLogDirectoryName = CString( szBuf );	// dir

	file.getLine();
	file.getField( szBuf, 256 );
	m_strSubSimName = CString( szBuf );			// sub name

	// read the begin and end time
	file.getLine();
	file.getTime( m_timeBegin, TRUE );
	file.getTime( m_timeEnd, TRUE );
	
	m_reports.readFromFile( file );
	
	// init logs name
	initLogsName();
	return;
}

void CSimItem::writeToFile( ArctermFile& file ) const
{
	// need not to write logs
	file.writeField( m_strLogDirectoryName );	// dir 
	file.writeLine();
	file.writeField( m_strSubSimName );
	file.writeLine();
	// write the begibe and end time
	file.writeTime( m_timeBegin, TRUE );
	file.writeTime( m_timeEnd,TRUE);
	file.writeLine();

	m_reports.writeToFile( file );
	return;
}

bool CSimItem::initSimItem(  const CSimParameter* _pSimPara,InputTerminal* _pInTerm, const CString& _strPath )
{
	// make directory and create File
	// make root
	CString strSimItemPath = _strPath + "\\" + m_strLogDirectoryName;

	FileManager::MakePath( strSimItemPath );
	// make LOGS directory
	FileManager::MakePath( strSimItemPath + "\\" + strLogPath );
	// make REPORT directory
	FileManager::MakePath( strSimItemPath + "\\" + strReportSetName );

	//////////////////////////////////////////////////////////////////////////
	// shall create all report path 
	//if( (const_cast<CSimParameter*>(_pSimPara))->GetSimPara()->GetClassType() == "AUTOREPORT" )
	//{
		//CSimAutoReportPara* pPara = (CSimAutoReportPara*)(const_cast<CSimParameter*>(_pSimPara))->GetSimPara();
		m_reports.setDirectoryName( m_strLogDirectoryName + "\\" + strReportSetName );
		m_reports.initReportSet( _pSimPara, _pInTerm ,_strPath );
	//}
	return true;
}

bool CSimItem::isReportRealExist( int _ReportType ) const
{
	return m_reports.isReportRealExist( _ReportType );
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSimResult::CSimResult()
{
}

CSimResult::~CSimResult()
{
}

CSimResult::CSimResult( const CSimResult& _anthoer )
{
	m_vSimResults = _anthoer.m_vSimResults;	
}

CSimResult& CSimResult::operator = ( const CSimResult& _anthoer )
{
	if( this != &_anthoer )
	{
		m_vSimResults = _anthoer.m_vSimResults;
	}
	return *this;
}

bool CSimResult::initSimResult( const CSimParameter* _pSimPara, InputTerminal* _pInTerm,const CString& _strPath )
{
	assert( _pSimPara );
	// make directory and create file
	// make the root
	CString strSimResultPath = _strPath + "\\" + strSimResult;
	FileManager::MakePath( strSimResultPath );

	// create single run result
	for( int i=0; i< (const_cast<CSimParameter*>(_pSimPara))->GetSimPara()->GetNumberOfRun(); i++ )
	{
		CString strNum;
		strNum.Format("%d", i );

		CString strSubSimReslutPath = strSimResult + "\\" + strSimResult  + strNum;
		CSimItem item( strSubSimReslutPath,  strSimResult  + strNum );
		item.initSimItem( _pSimPara, _pInTerm, _strPath );
		m_vSimResults.push_back( item );
	}

	return true;
}

void CSimResult::readFromFile( ArctermFile& file )
{
	int iSize;

	m_vSimResults.clear();

	file.getLine();
	file.getInteger( iSize );
	for( int i=0; i<iSize; i++ )
	{
		CSimItem item;
		item.readFromFile( file );
		m_vSimResults.push_back( item );
	}
	
	return;
}
	
void CSimResult::writeToFile( ArctermFile& file ) const
{
	file.writeInt( (int)m_vSimResults.size() );
	file.writeLine();
	for( unsigned i=0; i<m_vSimResults.size(); i++ )
	{
		m_vSimResults[i].writeToFile( file );
	}

	return;
}
