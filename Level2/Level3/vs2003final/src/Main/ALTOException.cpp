#include "StdAfx.h"
#include "ALTOException.h"
#include <strstream>
#include <Common/ARCTracker.h>
/**
	SEH exception table:( copy from winnt.h)

	#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)    
	#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)    
	#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)    
	#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
	#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)    
	#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)    
	#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)    
	#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)    
	#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)    
	#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)    
	#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)    
	#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)    
	#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)    
	#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)    
	#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)    
	#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)    
	#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)    
	#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)    
	#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)    
	#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)    
	#define STATUS_FLOAT_MULTIPLE_FAULTS     ((DWORD   )0xC00002B4L)    
	#define STATUS_FLOAT_MULTIPLE_TRAPS      ((DWORD   )0xC00002B5L)    
	#define STATUS_REG_NAT_CONSUMPTION       ((DWORD   )0xC00002C9L)    
	#if defined(STATUS_SUCCESS) || (_WIN32_WINNT > 0x0500) || (_WIN32_FUSION >= 0x0100) 
	#define STATUS_SXS_EARLY_DEACTIVATION    ((DWORD   )0xC015000FL)    
	#define STATUS_SXS_INVALID_DEACTIVATION  ((DWORD   )0xC0150010L)    
	#endif 
*/

seh_exception_base::seh_exception_base( const PEXCEPTION_POINTERS pExp, std::string what )
{
	if (pExp)
	{
		m_ExceptionRecord = *pExp->ExceptionRecord;
		m_ContextRecord = *pExp->ContextRecord;
	}
	else
	{
		memset(&m_ExceptionRecord, 0, sizeof(m_ExceptionRecord));
		memset(&m_ContextRecord, 0, sizeof(m_ContextRecord));
	}
	m_what = what;
}

void seh_exception_base::setALTOExceptionHandler()
{
	// comment out the line below if want enable unexpected error handing in ALTO System.
	// return; 
	//

#ifndef _DEBUG// disable under debug mode.

	_set_se_translator( trans_func );

	//_set_purecall_handler( purecall_func );

	//<TODO::> set_unexpected();

	//<TODO::> set_terminate();

#endif

}

//Exception: ALTOException.
void seh_exception_base::trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string strErrorCode = "DEFAULT";
	if(pExp->ExceptionRecord->ExceptionCode)
	{
		CString strBuf;
		strBuf.Format(_T("%x"), pExp->ExceptionRecord->ExceptionCode);
		strErrorCode = strBuf;
	}
	ARCStringTracker::BuildTrackerImage();
	throw ALTOException(pExp, strErrorCode.c_str());

}

void seh_exception_base::purecall_func()
{
	//<TODO::> 
}

#define ARRAY_EXCEPTION_CODE          20

static const DWORD g_dwArrayExceptionCode[ARRAY_EXCEPTION_CODE] =
{
	EXCEPTION_ACCESS_VIOLATION,
	EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
	EXCEPTION_BREAKPOINT,
	EXCEPTION_DATATYPE_MISALIGNMENT,
	EXCEPTION_FLT_DENORMAL_OPERAND,
	EXCEPTION_FLT_DIVIDE_BY_ZERO,
	EXCEPTION_FLT_INEXACT_RESULT,
	EXCEPTION_FLT_INVALID_OPERATION,
	EXCEPTION_FLT_OVERFLOW,
	EXCEPTION_FLT_STACK_CHECK,
	EXCEPTION_FLT_UNDERFLOW,
	EXCEPTION_ILLEGAL_INSTRUCTION,
	EXCEPTION_IN_PAGE_ERROR,
	EXCEPTION_INT_DIVIDE_BY_ZERO,
	EXCEPTION_INT_OVERFLOW,
	EXCEPTION_INVALID_DISPOSITION,
	EXCEPTION_NONCONTINUABLE_EXCEPTION,
	EXCEPTION_PRIV_INSTRUCTION,
	EXCEPTION_SINGLE_STEP,
	EXCEPTION_STACK_OVERFLOW,
};

static const char* const g_sArrayExceptionCode[ARRAY_EXCEPTION_CODE + 1] =
{
	"EXCEPTION_ACCESS_VIOLATION",
	"EXCEPTION_ARRAY_BOUNDS_EXCEEDED",
	"EXCEPTION_BREAKPOINT",
	"EXCEPTION_DATATYPE_MISALIGNMENT",
	"EXCEPTION_FLT_DENORMAL_OPERAND",
	"EXCEPTION_FLT_DIVIDE_BY_ZERO",
	"EXCEPTION_FLT_INEXACT_RESULT",
	"EXCEPTION_FLT_INVALID_OPERATION",
	"EXCEPTION_FLT_OVERFLOW",
	"EXCEPTION_FLT_STACK_CHECK",
	"EXCEPTION_FLT_UNDERFLOW",
	"EXCEPTION_ILLEGAL_INSTRUCTION",
	"EXCEPTION_IN_PAGE_ERROR",
	"EXCEPTION_INT_DIVIDE_BY_ZERO",
	"EXCEPTION_INT_OVERFLOW",
	"EXCEPTION_INVALID_DISPOSITION",
	"EXCEPTION_NONCONTINUABLE_EXCEPTION",
	"EXCEPTION_PRIV_INSTRUCTION",
	"EXCEPTION_SINGLE_STEP",
	"EXCEPTION_STACK_OVERFLOW",

	"UNKNOW",
};


#include <algorithm>

void seh_exception_base::DumpToStream( std::ostream& os )
{
	os << "Dumping exception record..." << std::endl;

	os.setf(std::ios::hex);
	const DWORD* pCodeFound = std::find(g_dwArrayExceptionCode, g_dwArrayExceptionCode + ARRAY_EXCEPTION_CODE, m_ExceptionRecord.ExceptionCode);
	os << "\tExceptionCode   : 0x" << m_ExceptionRecord.ExceptionCode    << "(" << g_sArrayExceptionCode[pCodeFound - g_dwArrayExceptionCode] << ")" << std::endl;
	os << "\tExceptionFlags  : 0x" << m_ExceptionRecord.ExceptionFlags   << (m_ExceptionRecord.ExceptionFlags ? "(EXCEPTION_NONCONTINUABLE)" : "(EXCEPTION_CONTINUABLE)") << std::endl;
	os << "\tExceptionAddress: 0x" << m_ExceptionRecord.ExceptionAddress << std::endl;
	os << "\tExceptionInformation:(Number of Parameters - " << m_ExceptionRecord.NumberParameters << ")" << std::endl;
	for(DWORD dw=0;dw<m_ExceptionRecord.NumberParameters;dw++)
		os << "\t\t" << m_ExceptionRecord.ExceptionInformation[dw] << std::endl;

	os << "Dumping context.." << std::endl;
	os << "\tContextFlags : 0x" << m_ContextRecord.ContextFlags << std::endl;
	os << "\tDr0          : 0x" << m_ContextRecord.Dr0          << std::endl;
	os << "\tDr1          : 0x" << m_ContextRecord.Dr1          << std::endl;
	os << "\tDr2          : 0x" << m_ContextRecord.Dr2          << std::endl;
	os << "\tDr3          : 0x" << m_ContextRecord.Dr3          << std::endl;
	os << "\tDr6          : 0x" << m_ContextRecord.Dr6          << std::endl;
	os << "\tDr7          : 0x" << m_ContextRecord.Dr7          << std::endl;
	os << "\tSegGs        : 0x" << m_ContextRecord.SegGs        << std::endl;
	os << "\tSegFs        : 0x" << m_ContextRecord.SegFs        << std::endl;
	os << "\tSegEs        : 0x" << m_ContextRecord.SegEs        << std::endl;
	os << "\tSegDs        : 0x" << m_ContextRecord.SegDs        << std::endl;
	os << "\tEdi          : 0x" << m_ContextRecord.Edi          << std::endl;
	os << "\tEsi          : 0x" << m_ContextRecord.Esi          << std::endl;
	os << "\tEbx          : 0x" << m_ContextRecord.Ebx          << std::endl;
	os << "\tEdx          : 0x" << m_ContextRecord.Edx          << std::endl;
	os << "\tEcx          : 0x" << m_ContextRecord.Ecx          << std::endl;
	os << "\tEax          : 0x" << m_ContextRecord.Eax          << std::endl;
	os << "\tEbp          : 0x" << m_ContextRecord.Ebp          << std::endl;
	os << "\tEip          : 0x" << m_ContextRecord.Eip          << std::endl;
	os << "\tSegCs        : 0x" << m_ContextRecord.SegCs        << std::endl;
	os << "\tEFlags       : 0x" << m_ContextRecord.EFlags       << std::endl;
	os << "\tEsp          : 0x" << m_ContextRecord.Esp          << std::endl;
	os << "\tSegSs        : 0x" << m_ContextRecord.SegSs        << std::endl;
}




