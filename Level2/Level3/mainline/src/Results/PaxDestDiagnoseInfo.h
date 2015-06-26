// PaxDestDiagnoseInfo.h: interface for the CPaxDestDiagnoseInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXDESTDIAGNOSEINFO_H__72247240_471F_402B_8A57_768C5F0B98A1__INCLUDED_)
#define AFX_PAXDESTDIAGNOSEINFO_H__72247240_471F_402B_8A57_768C5F0B98A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include <vector>
#include <iostream>
#include <fstream>

// declare
class Terminal;
typedef std::pair< CString, CString > DEST_REASON_PAIR;
typedef std::vector< DEST_REASON_PAIR > DIAGNOSE_INFO_LIST;

/************************************************************************/
/*                          CPaxDestDiagnoseInfo                        */
/************************************************************************/
class CPaxDestDiagnoseInfo  
{
public:
	CPaxDestDiagnoseInfo( Terminal* _pTerm, long _lID = -1 );
	virtual ~CPaxDestDiagnoseInfo();

private:
	Terminal*			m_pTerm;
	long				m_lDiagnosePaxID;
	long				m_lDiagnoseTime;
	CString				m_strSrcProcessorName;
	DIAGNOSE_INFO_LIST	m_vDiagnoseInfoList;
	
public:
	// clear data
	void clear();

	// set & get
	void setDiagnosePaxID( long _lID );
	void setDiagnoseTime( long _lTime );
	void setSrcProcessor( CString _strProc );
	long getDiagnosePaxID( void ) const;
	long getDiagnoseTime( void ) const;
	const CString& getSrcProcessor( void ) const;

	// operate diagnose info list
	int getDiagnoseListSize( void ) const;
	const DEST_REASON_PAIR& getDiagnoseInfoByIndex( int _index ) const;
	void insertDiagnoseInfo( CString _strDestProc, CString _strReason );
	
	// save the pax's diagnose info to file
	void flushDiagnoseInfo( void );

	//////////////////////////////////////////////////////////////////////////
	friend std::ostream& operator << ( std::ostream& out, const CPaxDestDiagnoseInfo* _pinfo );
	friend std::istream& operator >> ( std::istream& in, CPaxDestDiagnoseInfo* _pinfo);
};

#endif // !defined(AFX_PAXDESTDIAGNOSEINFO_H__72247240_471F_402B_8A57_768C5F0B98A1__INCLUDED_)
