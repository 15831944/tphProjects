// ARCException.h: interface for the ARCException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCEXCEPTION_H__0C3A2EC9_B362_4895_A8C3_10AD60EC24C5__INCLUDED_)
#define AFX_ARCEXCEPTION_H__0C3A2EC9_B362_4895_A8C3_10AD60EC24C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "enginediagnose.h"
//////////////////////////////////////////////////////////////////////////
//
class ElapsedTime;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCException
class ARCException  
{
protected:
	CString m_strErrorMsg;
	CString m_strOtherMsg;
	
	CString m_strErrorType;
	CString m_strErrorTime;

	DiagnoseEntry m_diagEntry;
public:
	ARCException() : m_strErrorType( "ARC ERROR" )
	{};
	ARCException( const CString& _strErrorMsg, const CString& _strOtherMsg = "", const CString& _strTime = "");
	virtual ~ARCException();
	
	// set & get diagnose entry 
	void setDiagType( DiagnoseType _type );
	void setSimEngineIndex( int _index );
	void setDiagData( long _lData );
	const DiagnoseEntry& getDiagEntry( void ) const;
	
	// virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCSimRuntimeError
class ARCSimEngineException : public ARCException
{
public:
	ARCSimEngineException()
	{ m_strErrorType = "SIMENGINE ERROR";};
	ARCSimEngineException( const CString& _strErrorMsg, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCSimEngineException();

	// virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCSystemError
class ARCSystemError : public ARCException
{
public:
	ARCSystemError() 
	{ m_strErrorType = "SYSTEM ERROR";}

	ARCSystemError( const CString& _strErrorMsg, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCSystemError();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCProcessNameInvalideError
class ARCProcessNameInvalideError : public ARCSimEngineException
{
private:
	CString m_strProcName;

public:
	ARCProcessNameInvalideError( const CString& _strProcName, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCProcessNameInvalideError();

	// virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCNoBaggageDeviceAssignError
class ARCNoBaggageDeviceAssignError : public ARCSimEngineException
{
private:
	CString m_strMobType;

public:
	ARCNoBaggageDeviceAssignError( const CString& _strMobName, const CString& _strOtherMsg = "", const CString& _strTime = "");
	virtual ~ARCNoBaggageDeviceAssignError();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCPipeNotIntersectError
class ARCPipeNotIntersectError : public ARCSimEngineException
{
private:
	CString m_strPipe1;
	CString m_strPipe2;

public:
	ARCPipeNotIntersectError( const CString& _strPipe1, const CString& _strPipe2, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCPipeNotIntersectError();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCFlowError
class ARCFlowError : public ARCSimEngineException
{
protected:
	CString m_strMobType;
	CString m_strProcName;

public:
	ARCFlowError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCFlowError();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCFlowUndefinedError
class ARCFlowUndefinedError : public ARCFlowError
{
public:
	ARCFlowUndefinedError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg = "",const CString& _strTime = "" );
	virtual ~ARCFlowUndefinedError();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCDestProcessorUnavailableError
class ARCDestProcessorUnavailableError : public ARCFlowError
{
public:
	ARCDestProcessorUnavailableError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCDestProcessorUnavailableError();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCLackOfNecessaryInfo
class ARCLackOfNecessaryInfo: public ARCSimEngineException
{
public:
	ARCLackOfNecessaryInfo() 
	{ m_strErrorType = "LACK OF NECESSARY INFO";}
	ARCLackOfNecessaryInfo( const CString& _strMsg, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCLackOfNecessaryInfo();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCNoEmergeExitError
class ARCNoEmergeExitError : public ARCSimEngineException 
{
public:
	ARCNoEmergeExitError()
	{ m_strErrorType = " NO EMERGE EXIT "; }
	ARCNoEmergeExitError( const CString& _strMsg, const CString& _strOtherMsg = "", const CString& _strTime = "");
	virtual ~ARCNoEmergeExitError( );

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

class ARCConveyorSimultaneousServiceError : public ARCSimEngineException
{
protected:
	CString m_strMobType;
	CString m_strProcName;

public:
	ARCConveyorSimultaneousServiceError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCConveyorSimultaneousServiceError();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ARCBarrierSystemError
class ARCBarrierSystemError : public ARCFlowError
{
public:
	ARCBarrierSystemError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg = "", const CString& _strTime = "" );
	virtual ~ARCBarrierSystemError();

	//virtual function
	virtual CString getErrorMsg();
	virtual FORMATMSG getFormatErrorMsg( void );
};


#endif // !defined(AFX_ARCEXCEPTION_H__0C3A2EC9_B362_4895_A8C3_10AD60EC24C5__INCLUDED_)
