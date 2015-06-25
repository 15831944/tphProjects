#include "StdAfx.h"
#include "OnBoardException.h"

//--------------------------------------------------------------------
//OnBoardException

OnBoardException::OnBoardException(OnBoardDiagnose* pDiagnose)
:ARCExceptionEx(pDiagnose)
{
	m_strErrorType = _T("ONBOARD ENGINE ERROR");
	m_pDiagnose->SetErrorType(m_strErrorType);
}

OnBoardSimEngineConfigException::OnBoardSimEngineConfigException(OnBoardDiagnose* pDiagnose)
:OnBoardException(pDiagnose)
{
	m_strErrorType = _T("ONBOARD ENGINE CONFIG ERROR");
	m_pDiagnose->SetErrorType(m_strErrorType);
}

OnBoardSimEngineException::OnBoardSimEngineException(OnBoardDiagnose* pDiagnose)
:OnBoardException(pDiagnose)
{
	m_strErrorType = _T("ONBOARD ENGINE SIMULATION ERROR");
	m_pDiagnose->SetErrorType(m_strErrorType);
}

OnBoardSimEngineFatalError::OnBoardSimEngineFatalError(OnBoardDiagnose* pDiagnose)
:OnBoardException(pDiagnose)
{
	m_strErrorType = _T("ONBOARD ENGINE FATAL ERROR");
	m_pDiagnose->SetErrorType(m_strErrorType);
}