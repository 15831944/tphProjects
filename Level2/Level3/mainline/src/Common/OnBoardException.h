#pragma once
#include "EngineDiagnoseEx.h"
#include "ARCExceptionEx.h"

//--------------------------------------------------------------------
//OnBoardDiagnose
class OnBoardDiagnose : public ARCDiagnose
{
public:
	OnBoardDiagnose(const CString& strErrorMessage)
		:ARCDiagnose( strErrorMessage){};
	OnBoardDiagnose( int nMobileElementID, const CString& strFltID, const ElapsedTime& _time, const CString& strErrorMessage)
		:ARCDiagnose( nMobileElementID,strFltID,_time,strErrorMessage){}

	~OnBoardDiagnose(){};

};

//--------------------------------------------------------------------
//OnBoardException
class OnBoardException : public ARCExceptionEx
{
public:
	OnBoardException(OnBoardDiagnose* pDiagnose);
};

class OnBoardSimEngineConfigException : public OnBoardException
{
public:
	OnBoardSimEngineConfigException(OnBoardDiagnose* pDiagnose);
};

class OnBoardSimEngineException : public OnBoardException
{
public:
	OnBoardSimEngineException(OnBoardDiagnose* pDiagnose);
};


class OnBoardSimEngineFatalError : public OnBoardException
{
public:
	OnBoardSimEngineFatalError(OnBoardDiagnose* pDiagnose);
};
