// PaxDestDiagnoseLog.cpp: implementation of the CPaxDestDiagnoseLog class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PaxDestDiagnoseLog.h"
#include "common\exeption.h"
#include "common\fileman.h"
#include "common\simandreportmanager.h"
#include "engine\terminal.h"
#include "assert.h"
#include <fstream>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxDestDiagnoseLog::CPaxDestDiagnoseLog() : m_infoFile( NULL )
{

}

CPaxDestDiagnoseLog::~CPaxDestDiagnoseLog()
{
	closeDiagnoseInfoFile();
}

// clear data
void CPaxDestDiagnoseLog::clearLog( void )
{
	m_sDiagnoseLog.clear();
}

// load data if need 
void CPaxDestDiagnoseLog::loadDataIfNeed( const CString& _strLogFilePath )
{
	if( m_sDiagnoseLog.size() > 0)
		return;

	std::ifstream log_file( _strLogFilePath, std::ios::in | std::ios::binary );
	if( log_file.bad() )
		throw new FileOpenError( _strLogFilePath );
	
	int _iCount;
	log_file.read( (char*)&_iCount, sizeof( int) );
	DiagnoseLogEntry entry;
	for( int i=0; i<_iCount; i++ )
	{
		log_file.read((char*)&entry, sizeof( DiagnoseLogEntry ) );
		m_sDiagnoseLog.insert( entry );
	}

	log_file.close();
}

//save Log
void CPaxDestDiagnoseLog::saveLog( const CString& _strLogFilePath ) const
{
	FileManager fileMan;
	if( fileMan.IsFile (_strLogFilePath) )
		fileMan.DeleteFile( _strLogFilePath );

	std::ofstream log_file( _strLogFilePath, std::ios::binary | std::ios::out  );
	if( log_file.bad() )
		throw new FileOpenError( _strLogFilePath );

	int _iCount = m_sDiagnoseLog.size();
	log_file.write( (const char*)& _iCount, sizeof(int) );

	DIAGNOSE_LOG_SET::const_iterator iter;
	for( iter = m_sDiagnoseLog.begin(); iter != m_sDiagnoseLog.end(); ++iter )
	{
		const DiagnoseLogEntry& _entry = *iter;
		log_file.write( (const char*) &_entry, sizeof( DiagnoseLogEntry) );
		//log_file << *iter;
	}

	log_file.close();
}

//add a DiagnoseLogEntry
bool CPaxDestDiagnoseLog::insertDiagnoseEntry( const CPaxDestDiagnoseInfo* _pDiagnoseInfo )
{
	assert( m_infoFile );

	DiagnoseLogEntry entry;
	entry.m_lPaxID		= _pDiagnoseInfo->getDiagnosePaxID();
	
	// first check the pax's id if already exist in the set
	DIAGNOSE_LOG_SET::iterator  iter = m_sDiagnoseLog.find( entry );
	if( iter != m_sDiagnoseLog.end() )
		return false;
	
	// save diagnose info to file
	entry.m_lBeginePos	= static_cast<long>(m_infoFile->tellg());
	*m_infoFile << _pDiagnoseInfo;
	entry.m_lEndPos		= static_cast<long>(m_infoFile->tellg());

	// add the entry to the set
	m_sDiagnoseLog.insert( entry );
	return true;
}


//create DiagnoseInfoFile
void CPaxDestDiagnoseLog::createDiagnoseInfoFile( const CString& _strFileName )
{
	closeDiagnoseInfoFile();
	FileManager fileMan;
	if( fileMan.IsFile( _strFileName ))
		fileMan.DeleteFile( _strFileName );

	m_infoFile = new std::fstream( _strFileName, std::ios::out );
	if( !m_infoFile || m_infoFile->bad() )
		throw new FileOpenError( _strFileName );
}

// save DiagnoseInfoFile
void CPaxDestDiagnoseLog::closeDiagnoseInfoFile( void )
{
	if( m_infoFile )
	{
		delete m_infoFile;
		m_infoFile  = NULL;
	}
}

// open DiagnoseInfoFile
void CPaxDestDiagnoseLog::openDiagnoseInfoFile( const CString& _strFileName )
{
	closeDiagnoseInfoFile();
	
	m_infoFile = new std::fstream( _strFileName, std::ios::in | std::ios::out );
	if( !m_infoFile || m_infoFile->bad() )
		throw new FileOpenError( _strFileName );
}


//get diagnoseInfo by pax ID
bool CPaxDestDiagnoseLog::getDiagnoseInfo( Terminal* _pTerm, const CString& _strPath, long _lPaxID, CPaxDestDiagnoseInfo* _info )
{
	// load diagnose log if need
	CString sFile = _pTerm->GetSimReportManager()->GetCurrentLogsFileName( DiagnoseLogFile,_strPath );
	loadDataIfNeed( sFile );

	// check the pax id if exist in the set
	DiagnoseLogEntry entry;
	entry.m_lPaxID = _lPaxID;

	DIAGNOSE_LOG_SET::iterator iter =  m_sDiagnoseLog.find( entry );
	if( iter == m_sDiagnoseLog.end() ) // no exist in the set
		return false;
	
	// load diagnose info if need
	if( m_infoFile == NULL )
	{
		sFile = _pTerm->GetSimReportManager()->GetCurrentLogsFileName( DiagnoseInfoFile, _strPath );
		openDiagnoseInfoFile( sFile );
		assert( m_infoFile );
	}
	
	_info->clear();
#ifdef DEBUG
	long _lBeginePos = iter->m_lBeginePos;
#endif //DEBUG
	m_infoFile->seekg( iter->m_lBeginePos );

	*m_infoFile>>_info;

	return !m_infoFile->bad();
}
