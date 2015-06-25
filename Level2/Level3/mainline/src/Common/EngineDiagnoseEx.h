#pragma once
#include "elaptime.h"
//------------------------------------------------------------------------------------------
//ARCDiagnose
class ARCDiagnose
{
public:
	typedef enum{
		em_TerminalDiagnose,
		em_AirsideDiagnose,
		em_LandsideDiagnose,
		em_OnBoardDiagnose

	}DiagnoseType;
	
public:
	ARCDiagnose();
	ARCDiagnose(const CString& strErrorMessage);
	ARCDiagnose(const ElapsedTime& _time);
	ARCDiagnose(const ElapsedTime& _time, const CString& strErrorMessage);
	ARCDiagnose( int nMobileElementID,const CString& strFltID ,const ElapsedTime& _time, const CString& strErrorMessage);

	virtual ~ARCDiagnose(void){};

	CString& GetTime()			{	return m_strTime; }
	CString& GetErrorType()		{	return m_strErrorType; }
	void SetErrorType(const CString& strErrorType);
	int GetMobElementID(){return m_nMobileID;}
	void SetMobElementID(int id){ m_nMobileID = id; }

	//add details(multi line on tooltip window)
	virtual void AddDetailsItem(const CString& strItem);
	//add details(single line on tooltip window)
	virtual void SetDetails(const CString& strDetails);
	CString& GetDetails()		{	return m_strDetails; }
	CString GetFltID() const { return m_strFltID; }
	void SetFltID(const CString& fltID) { m_strFltID = fltID; }

protected:

	CString m_strTime;
	CString m_strErrorType;
	CString m_strDetails;
	int m_nMobileID;
	CString m_strFltID;

};

//TODO:	maybe we could modify terminal's diagnose structure someday.
//------------------------------------------------------------------------------------------
//TerminalDiagnose 
class TerminalDiagnose : public ARCDiagnose
{
public:
	TerminalDiagnose();
	~TerminalDiagnose();
};

//------------------------------------------------------------------------------------------
//AirsideDiagnose
class AirsideDiagnose : public ARCDiagnose
{
public:
	AirsideDiagnose(const ElapsedTime& _time, const CString& strMobileElemID);
	AirsideDiagnose(const CString& strTime, const CString& strMobileElemID);
	~AirsideDiagnose(void){};

public:

	CString& GetMobElementID()	{	return m_strMobileElemID; };
protected:

	CString m_strMobileElemID;
};

//TODO: 
//------------------------------------------------------------------------------------------
//LandsideDiagnose
class LandsideDiagnose : public ARCDiagnose
{
public:
	LandsideDiagnose(const ElapsedTime& _time);
};
