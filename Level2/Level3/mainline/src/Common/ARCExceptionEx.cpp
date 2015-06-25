#include "StdAfx.h"
#include "ARCExceptionEx.h"

ARCExceptionEx::ARCExceptionEx(ARCDiagnose* pDiagnose)
:m_pDiagnose(pDiagnose)
{
	m_pDiagnose->SetErrorType(m_strErrorType);
}

ARCExceptionEx::~ARCExceptionEx(void)
{
}

ARCDiagnose* ARCExceptionEx::GetDiagnose() const
{
	return m_pDiagnose;
}

ARCSimEngineExceptionEx::ARCSimEngineExceptionEx(ARCDiagnose* pDiagnose)
:ARCExceptionEx(pDiagnose)
{
	m_strErrorType = _T("ENGINE ERROR");
	m_pDiagnose->SetErrorType(m_strErrorType);
}

ARCSimEngineExceptionEx::~ARCSimEngineExceptionEx()
{

}


AirsideException::AirsideException(ARCDiagnose* pDiagnose)
:ARCSimEngineExceptionEx(pDiagnose)
{
	m_strErrorType = _T("ENGINE ERROR");
	m_pDiagnose->SetErrorType(m_strErrorType);
}


AirsideException::~AirsideException()
{

}

AirsideSystemError::AirsideSystemError(ARCDiagnose* pDiagnose)
:AirsideException(pDiagnose)
{
	m_strErrorType = _T("SYSTEM ERROR");
	m_pDiagnose->SetErrorType(m_strErrorType);

}

AirsideSystemError::~AirsideSystemError()
{

}

AirsideACTypeMatchError::AirsideACTypeMatchError(ARCDiagnose* pDiagnose)
:AirsideException(pDiagnose)
{
	m_strErrorType = _T("ACTYPE MATCH ERROR");
	m_pDiagnose->SetErrorType(m_strErrorType);
}

AirsideACTypeMatchError::~AirsideACTypeMatchError()
{

}

AirsideHeadingException::AirsideHeadingException( ARCDiagnose* pDiagnose )
:ARCExceptionEx(pDiagnose)
{
	m_strErrorType = _T("HEADING ERROR");
	m_pDiagnose->SetErrorType(m_strErrorType);
}

AirsideHeadingException::~AirsideHeadingException()
{
	
}
