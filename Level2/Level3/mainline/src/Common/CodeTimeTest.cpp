// CodeTimeTest.cpp: implementation of the CCodeTimeTest class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CodeTimeTest.h"
#include "fsstream.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 std::map<CString,DWORD>CCodeTimeTest::m_mapFunPerformanceRecord;
CCodeTimeTest::CCodeTimeTest(const CString& sFileName, long lLineNumber )
{		
	CString sLineNumber;
	sLineNumber.Format(" %d",lLineNumber );
	m_sKey = sFileName + sLineNumber;
	m_tStartTime = GetTickCount();
	//AfxMessageBox( m_sKey );
	m_mapFunPerformanceRecord.insert( std::map<CString,DWORD>::value_type( m_sKey ,0));
	//AfxMessageBox( m_sKey );
}

CCodeTimeTest::~CCodeTimeTest()
{
//	time_t tElapsedTime = CTime::GetCurrentTime().GetTime();
//	tElapsedTime -= m_tStartTime;
	m_mapFunPerformanceRecord[ m_sKey ] +=  GetTickCount() - m_tStartTime ;;
}
void CCodeTimeTest::InitTools()
{
	m_mapFunPerformanceRecord.clear();
}
void CCodeTimeTest::TraceOutResultToFile()
{
	ofsstream echoFile ("c:\\TimeDebug.log", stdios::app);	
	for( std::map<CString,DWORD>::iterator iter =m_mapFunPerformanceRecord.begin(); iter!= m_mapFunPerformanceRecord.end(); ++iter )
	{	
		CString sFileInfo( iter->first );
		echoFile <<" File info: " << sFileInfo.GetBuffer(sFileInfo.GetLength() ) <<"  Time Spend : " << iter->second <<" \n ";
	}
	echoFile.close();
}