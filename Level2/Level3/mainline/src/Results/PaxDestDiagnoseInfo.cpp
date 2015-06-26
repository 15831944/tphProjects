// PaxDestDiagnoseInfo.cpp: implementation of the CPaxDestDiagnoseInfo class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PaxDestDiagnoseInfo.h"
#include "paxDestDiagnoseLog.h"
#include "engine\terminal.h"
#include <string>
#include "assert.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxDestDiagnoseInfo::CPaxDestDiagnoseInfo( Terminal* _pTerm, long _lID) : m_pTerm( _pTerm ), m_lDiagnosePaxID( _lID )
{
	clear();
}

CPaxDestDiagnoseInfo::~CPaxDestDiagnoseInfo()
{
	clear();
}

// clear data
void CPaxDestDiagnoseInfo::clear()
{
	//m_lDiagnosePaxID	= -1l;
	m_lDiagnoseTime		= -1l;
	m_strSrcProcessorName = "";
	m_vDiagnoseInfoList.clear();
}

// set & get
void CPaxDestDiagnoseInfo::setDiagnosePaxID( long _lID )
{
	m_lDiagnosePaxID = _lID;
}

void CPaxDestDiagnoseInfo::setDiagnoseTime( long _lTime )
{
	m_lDiagnoseTime = _lTime;
}

void CPaxDestDiagnoseInfo::setSrcProcessor( CString _strProc )
{
	_strProc.Remove('\r');
	_strProc.Remove('\n');
	_strProc += "\r";
	m_strSrcProcessorName = _strProc;
}

long CPaxDestDiagnoseInfo::getDiagnosePaxID( void ) const
{
	return m_lDiagnosePaxID;
}

long CPaxDestDiagnoseInfo::getDiagnoseTime( void ) const
{
	return m_lDiagnoseTime;
}

const CString& CPaxDestDiagnoseInfo::getSrcProcessor( void ) const
{
	return m_strSrcProcessorName;
}

// operate diagnose info list
int CPaxDestDiagnoseInfo::getDiagnoseListSize( void ) const
{
	return m_vDiagnoseInfoList.size();
}

const DEST_REASON_PAIR& CPaxDestDiagnoseInfo::getDiagnoseInfoByIndex( int _index ) const
{
	assert( _index >=0 && (unsigned)_index < m_vDiagnoseInfoList.size() );
	return m_vDiagnoseInfoList[_index];
}

void CPaxDestDiagnoseInfo::insertDiagnoseInfo( CString _strDestProc, CString _strReason )
{
	_strDestProc.Remove('\r');
	_strDestProc.Remove('\n');
	_strDestProc += "\r";
	_strReason.Remove('\r');
	_strReason.Remove('\n');
	_strReason += "\r";

	// check the dest processor if exist in the list, avoid repeat
	for( unsigned i=0; i< m_vDiagnoseInfoList.size(); i++ )
	{
		if( m_vDiagnoseInfoList[i].first == _strDestProc )
			return;
	}
	
	m_vDiagnoseInfoList.push_back( std::make_pair( _strDestProc, _strReason ) );
}

// save the pax's diagnose info to file
void CPaxDestDiagnoseInfo::flushDiagnoseInfo( void )
{
	m_pTerm->m_pDiagnosLog->insertDiagnoseEntry( this );
}


//////////////////////////////////////////////////////////////////////////
std::ostream& operator << ( std::ostream& out,const CPaxDestDiagnoseInfo* _pinfo )
{
	int _iCount = _pinfo->m_vDiagnoseInfoList.size();
	out<< _iCount<< '\t' <<_pinfo->m_lDiagnosePaxID <<'\t'<< _pinfo->m_lDiagnoseTime <<'\t'<< (LPCTSTR) _pinfo->m_strSrcProcessorName;
	
	DIAGNOSE_INFO_LIST::const_iterator iter;
	for( iter = _pinfo->m_vDiagnoseInfoList.begin(); iter != _pinfo->m_vDiagnoseInfoList.end(); ++iter )
	{
		out<<(LPCTSTR) iter->first << (LPCTSTR) iter->second;
	}

	return out;
}

std::istream& operator>>( std::istream& in, CPaxDestDiagnoseInfo* _pinfo)
{
	std::string tmp_string;
	int _iCount;

	in>> _iCount>> _pinfo->m_lDiagnosePaxID>> _pinfo->m_lDiagnoseTime >> tmp_string;
	_pinfo->m_strSrcProcessorName = tmp_string.c_str();
	in.ignore();
	
	for( int i=0; i< _iCount && in; i++)
	{
		std::string strFirst, strSecond;
		std::getline( in, strFirst,'\r' );
		std::getline( in, strSecond,'\r' );
		//in >> strFirst >> strSecond;
		if( in )
			_pinfo->m_vDiagnoseInfoList.push_back( std::make_pair( strFirst.c_str(), strSecond.c_str()) );
	}
	
	in.clear();
	return in;
}
