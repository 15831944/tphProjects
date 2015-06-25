#pragma once
#include "EngineDiagnoseEx.h"

class ARCExceptionEx
{
public:
	ARCExceptionEx(ARCDiagnose* pDiagnose);
	virtual ~ARCExceptionEx(void);

	ARCDiagnose* GetDiagnose() const;

protected:
	CString			m_strErrorType;
	ARCDiagnose		*m_pDiagnose;
};


class ARCSimEngineExceptionEx : public ARCExceptionEx
{
public:
	ARCSimEngineExceptionEx(ARCDiagnose* pDiagnose);
	~ARCSimEngineExceptionEx();
};

class AirsideException : public ARCSimEngineExceptionEx
{
public:
	AirsideException(ARCDiagnose* pDiagnose);
	~AirsideException();
};

class AirsideHeadingException: public ARCExceptionEx
{
public:
	AirsideHeadingException(ARCDiagnose* pDiagnose);
	~AirsideHeadingException();
};

class AirsideSystemError : public AirsideException
{
public:
	AirsideSystemError(ARCDiagnose* pDiagnose);
	~AirsideSystemError();
};

class AirsideACTypeMatchError: public AirsideException
{
public:
	AirsideACTypeMatchError(ARCDiagnose* pDiagnose);
	~AirsideACTypeMatchError();
};

class TerminalException : public ARCSimEngineExceptionEx
{

};

class LandsideException : public ARCSimEngineExceptionEx
{

};