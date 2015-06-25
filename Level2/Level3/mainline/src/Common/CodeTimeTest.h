// CodeTimeTest.h: interface for the CCodeTimeTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODETIMETEST_H__E2FC9029_8D45_4763_BC14_5D62223CDBE8__INCLUDED_)
#define AFX_CODETIMETEST_H__E2FC9029_8D45_4763_BC14_5D62223CDBE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/************************************************************************/
/* the purpose of class CCodeTimeTest is to record the elapsed time whihc some  specific code be excuted */

/*
 *How  to use:
 * 1.put #include "CodeTimeTest.h" in .cpp if necessary
 * 2. put marco START_CODE_TIME_TEST to the start point of function( or any scope of the code ) which you want 
 *  to caculate the time elapsed

  * Note:
    if want to use this class , do not forget to define CODE_PERFOMANCE_DEBUGE_FLAG in file common\template.h
 */
/************************************************************************/
#include <map>
class CCodeTimeTest  
{
	CString m_sKey;
	DWORD m_tStartTime;
	static std::map<CString,DWORD>m_mapFunPerformanceRecord;
public:
	CCodeTimeTest(const CString& sFileName, long lLineNumber );
	virtual ~CCodeTimeTest();
	static void InitTools();
	static void TraceOutResultToFile();

};
#ifdef CODE_PERFOMANCE_DEBUGE_FLAG
	#ifndef START_CODE_TIME_TEST
		#define START_CODE_TIME_TEST CCodeTimeTest cCodeTimeTestTools(__FILE__ , __LINE__);
	#endif
#else
#ifndef START_CODE_TIME_TEST
	#define START_CODE_TIME_TEST ;
#endif
#endif
#endif // !defined(AFX_CODETIMETEST_H__E2FC9029_8D45_4763_BC14_5D62223CDBE8__INCLUDED_)
