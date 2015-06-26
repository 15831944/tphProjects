#include "StdAfx.h"
#include ".\databasefatalerror.h"

CDatabaseFatalError::CDatabaseFatalError(const CString& strError)
:m_strErrorMessage(strError)
{

}

CDatabaseFatalError::~CDatabaseFatalError(void)
{
}

CString CDatabaseFatalError::GetErrorMessage()
{
	return m_strErrorMessage;
}