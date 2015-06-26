#pragma once
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CDatabaseFatalError
{
public:
	CDatabaseFatalError( const CString& strError);
	virtual ~CDatabaseFatalError(void);

	CString GetErrorMessage();
	
protected:
	CString m_strErrorMessage;
};
