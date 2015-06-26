// PaxDestDiagnoseLog.h: interface for the CPaxDestDiagnoseLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXDESTDIAGNOSELOG_H__CEAB480E_8AD4_4102_8135_6A9F2D062099__INCLUDED_)
#define AFX_PAXDESTDIAGNOSELOG_H__CEAB480E_8AD4_4102_8135_6A9F2D062099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include "paxdestdiagnoseinfo.h"
#include <set>

// declare
class Terminal;

/************************************************************************/
/*                     struct DiagnoseLogEntry                          */
/************************************************************************/
struct DiagnoseLogEntry
{
	long	m_lPaxID;		
	long	m_lBeginePos;
	long	m_lEndPos;
	
	// operator
	bool operator<( const DiagnoseLogEntry& _entry ) const
	{
		return m_lPaxID < _entry.m_lPaxID;
	}

	bool operator==( const DiagnoseLogEntry& _entry ) const
	{
		return m_lPaxID == _entry.m_lPaxID;
	}
	
	friend std::istream& operator>>( std::istream& in, DiagnoseLogEntry _entry )
	{
		char ch;
		in>>_entry.m_lPaxID>>ch >>_entry.m_lBeginePos>>ch >> _entry.m_lEndPos>>ch;
		return in;
	}

	friend std::ostream& operator<<( std::ostream& out, const DiagnoseLogEntry& _entry )
	{
		
		out << _entry.m_lPaxID <<'\t' << _entry.m_lBeginePos <<'\t' << _entry.m_lEndPos<<"\r";
		return out;
	}
};

// declare
typedef std::set< DiagnoseLogEntry > DIAGNOSE_LOG_SET;

/************************************************************************/
/*                        CPaxDestDiagnoseLog                           */
/************************************************************************/
class CPaxDestDiagnoseLog  
{
public:
	CPaxDestDiagnoseLog();
	virtual ~CPaxDestDiagnoseLog();

private:
	DIAGNOSE_LOG_SET m_sDiagnoseLog;
	std::fstream* m_infoFile;
	
public:
	// clear data
	void clearLog( void );

	// load data if need 
	void loadDataIfNeed( const CString& _strLogFilePath );

	//save Log
	void saveLog( const CString& _strLogFilePath ) const;

	//add a DiagnoseLogEntry
	bool insertDiagnoseEntry( const CPaxDestDiagnoseInfo* _pDiagnoseInfo );

	//create DiagnoseInfoFile
	void createDiagnoseInfoFile( const CString& _strFileName );

	// save DiagnoseInfoFile
	void closeDiagnoseInfoFile( void );

	// open DiagnoseInfoFile
	void openDiagnoseInfoFile( const CString& _strFileName );

	//get diagnoseInfo by pax ID
	bool getDiagnoseInfo( Terminal* _pTerm, const CString& _strPath, long _lPaxID, CPaxDestDiagnoseInfo* _info );
};

#endif // !defined(AFX_PAXDESTDIAGNOSELOG_H__CEAB480E_8AD4_4102_8135_6A9F2D062099__INCLUDED_)
