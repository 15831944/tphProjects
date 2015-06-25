#include "StdAfx.h"
#include "EngineDiagnoseEx.h"

//------------------------------------------------------------------------------------------
//ARCDiagnose
ARCDiagnose::ARCDiagnose()
:m_strTime(_T(""))
,m_strDetails(_T(""))
,m_nMobileID(-1)
{
}

ARCDiagnose::ARCDiagnose(const CString& strErrorMessage)
:m_strTime(_T(""))
,m_strDetails(strErrorMessage)
,m_nMobileID(-1)
{
}

ARCDiagnose::ARCDiagnose(const ElapsedTime& _time)
:m_nMobileID(-1)
{
	m_strTime = _time.printTime();
}

ARCDiagnose::ARCDiagnose(const ElapsedTime& _time, const CString& strErrorMessage)
:m_nMobileID(-1)
{
	m_strTime = _time.printTime();
	m_strDetails = strErrorMessage;
}

ARCDiagnose::ARCDiagnose( int nMobileElementID, const CString& strFltID ,const ElapsedTime& _time, const CString& strErrorMessage )
:m_nMobileID(nMobileElementID)
{
	m_strTime = _time.printTime();
	m_strDetails = strErrorMessage;
	m_strFltID = strFltID;
}
void ARCDiagnose::SetErrorType(const CString& strErrorType)
{
	m_strErrorType = strErrorType;
}

void ARCDiagnose::AddDetailsItem(const CString& strItem)
{
	m_strDetails += strItem;
	m_strDetails += "\n";//sep on tooltip
}

void ARCDiagnose::SetDetails(const CString& strDetails)
{
	m_strDetails.Empty();
	m_strDetails = strDetails;
}

//------------------------------------------------------------------------------------------
//TerminalDiagnose
TerminalDiagnose::TerminalDiagnose()
{

}

TerminalDiagnose::~TerminalDiagnose(void)
{

}

//------------------------------------------------------------------------------------------
//AirsideDiagnose
AirsideDiagnose::AirsideDiagnose(const ElapsedTime& _time, const CString& strMobileElemID)
:ARCDiagnose(_time)
,m_strMobileElemID(strMobileElemID)
{
}

AirsideDiagnose::AirsideDiagnose(const CString& strTime, const CString& strMobileElemID)
{
	m_strTime = strTime;
	m_strMobileElemID = strMobileElemID;
}


//------------------------------------------------------------------------------------------
//LandsideDiagnose


LandsideDiagnose::LandsideDiagnose( const ElapsedTime& _time )
:ARCDiagnose(_time)
{
}


