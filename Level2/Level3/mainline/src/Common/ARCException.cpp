// ARCException.cpp: implementation of the ARCException class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "elaptime.h"
#include "ARCException.h"
//////////////////////////////////////////////////////////////////////
// ARCException
ARCException::ARCException( const CString& _strErrorMsg, const CString& _strOtherMsg/* = ""*/, const CString& _strTime/* = ""*/ ) 
			: m_strErrorMsg( _strErrorMsg ), m_strOtherMsg( _strOtherMsg ) , m_strErrorTime( _strTime ) , m_strErrorType( "ARC ERROR" )
{

}
	
ARCException::~ARCException()
{

}

// set & get diagnose entry 
void ARCException::setDiagType( DiagnoseType _type )
{
	m_diagEntry.eType = _type;
}

void ARCException::setSimEngineIndex( int _index )
{
	m_diagEntry.iSimResultIndex = _index;
}

void ARCException::setDiagData( long _lData )
{
	m_diagEntry.lData = _lData;
}

const DiagnoseEntry& ARCException::getDiagEntry( void ) const
{
	return m_diagEntry;
}

// virtual function
CString ARCException::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("error: %s", m_strErrorMsg );

	
	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";
	
	return strReturn;
}

FORMATMSG ARCException::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;
	
	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strOtherInfo = m_strErrorMsg + "  " + m_strOtherMsg;
	
	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

//////////////////////////////////////////////////////////////////////////
// ARCSimRuntimeError
ARCSimEngineException::ARCSimEngineException( const CString& _strErrorMsg, const CString& _strOtherMsg /*= ""*/, const CString& _strTime/* = ""*/ ) 
				      :ARCException( _strErrorMsg, _strOtherMsg, _strTime )
{
	m_strErrorType = "SIMENGINE ERROR";

}

ARCSimEngineException::~ARCSimEngineException()
{
	
}

// virtual function
CString ARCSimEngineException::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("runtime error: %s", m_strErrorMsg );

	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";
	
	return strReturn;
}

FORMATMSG ARCSimEngineException::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;

	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strOtherInfo = m_strErrorMsg + "  " + m_strOtherMsg;
	
	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

//////////////////////////////////////////////////////////////////////////
// ARCSystemError
ARCSystemError::ARCSystemError( const CString& _strErrorMsg, const CString& _strOtherMsg /*= ""*/, const CString& _strTime/* = ""*/ )
               :ARCException( _strErrorMsg, _strOtherMsg, _strTime )
{
	m_strErrorType = "SYSTEM ERROR";
}

ARCSystemError::~ARCSystemError()
{

}

//virtual function
CString ARCSystemError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("system error: %s", m_strErrorMsg );
	
	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";
	
	return strReturn;
}

FORMATMSG ARCSystemError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;

	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strOtherInfo = m_strErrorMsg + " " + m_strOtherMsg;

	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

//////////////////////////////////////////////////////////////////////////
// ARCProcessNameInvalideError
ARCProcessNameInvalideError::ARCProcessNameInvalideError( const CString& _strProcName, const CString& _strOtherMsg /*= ""*/, const CString& _strTime/* = ""*/ )
{
	m_strProcName = _strProcName;
	m_strOtherMsg = _strOtherMsg;
	m_strErrorTime = _strTime;

	m_strErrorType = "PROCESSOR NAME INVALID";
}

ARCProcessNameInvalideError::~ARCProcessNameInvalideError()
{

}

// virtual function
CString ARCProcessNameInvalideError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("runtime error: processor name %s is invalid", m_strProcName );
	
	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";

	return strReturn;
}

FORMATMSG ARCProcessNameInvalideError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;

	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strProcessor = m_strProcName;
	formatMsg.strOtherInfo = m_strOtherMsg;

	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}
//////////////////////////////////////////////////////////////////////////
// ARCNoBaggageDeviceAssignError
ARCNoBaggageDeviceAssignError::ARCNoBaggageDeviceAssignError( const CString& _strMobName, const CString& _strOtherMsg /*= ""*/, const CString& _strTime/* = ""*/ )
{
	m_strMobType = _strMobName;
	m_strOtherMsg = _strOtherMsg;
	m_strErrorTime = _strTime;

	m_strErrorType = "NO BAGGAGE DEVICE ASSIGN";
}

ARCNoBaggageDeviceAssignError::~ARCNoBaggageDeviceAssignError()
{

}

//virtual function
CString ARCNoBaggageDeviceAssignError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("runtime error: not assign baggage device for pax type %s", m_strMobType );

	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";

	return strReturn;
}

FORMATMSG ARCNoBaggageDeviceAssignError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;

	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strMobType = m_strMobType;
	formatMsg.strOtherInfo = m_strOtherMsg;

	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}


//////////////////////////////////////////////////////////////////////////
// ARCPipeNotIntersectError
ARCPipeNotIntersectError::ARCPipeNotIntersectError( const CString& _strPipe1, const CString& _strPipe2, const CString& _strOtherMsg /*= ""*/, const CString& _strTime/* = ""*/ )
{
	m_strPipe1 = _strPipe1;
	m_strPipe2 = _strPipe2;
	m_strOtherMsg = _strOtherMsg;
	m_strErrorTime = _strTime;

	m_strErrorType = "PIPE NOT INTERSECT";
}

ARCPipeNotIntersectError::~ARCPipeNotIntersectError()
{

}

//virtual function
CString ARCPipeNotIntersectError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("runtime error: pipe %s and pipe %s not intersect", m_strPipe1, m_strPipe2 );

	if( !m_strOtherMsg.IsEmpty() )
		 strReturn = strReturn + "(" + m_strOtherMsg + ")";

	return strReturn;
}

FORMATMSG ARCPipeNotIntersectError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;
	
	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strOtherInfo.Format("pipe %s and pipe %s not intersect", m_strPipe1, m_strPipe2 );
	formatMsg.strOtherInfo += m_strOtherMsg;

	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

//////////////////////////////////////////////////////////////////////////
//ARCFlowError
ARCFlowError::ARCFlowError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg /*= ""*/, const CString& _strTime/* = ""*/ )
{
	m_strMobType = _strMobType;
	m_strProcName = _strProcName;
	m_strOtherMsg = _strOtherMsg;
	m_strErrorTime  = _strTime;

	m_strErrorType = "FLOW ERROR";
}

ARCFlowError::~ARCFlowError()
{

}

//virtual function
CString ARCFlowError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("runtime error: flow error");

	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";
	
	return strReturn;
}

FORMATMSG ARCFlowError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;
	
	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strMobType = m_strMobType;
	formatMsg.strProcessor = m_strProcName;
	formatMsg.strOtherInfo = m_strOtherMsg;

	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

//////////////////////////////////////////////////////////////////////////
// ARCFlowUndefinedError
ARCFlowUndefinedError::ARCFlowUndefinedError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg /*= ""*/, const CString& _strTime/* = ""*/ )
					  :ARCFlowError( _strMobType, _strProcName, _strOtherMsg, _strTime )
{
	m_strErrorType = "FLOW UNDEFINED";
}

ARCFlowUndefinedError::~ARCFlowUndefinedError()
{

}

//virtual function
CString ARCFlowUndefinedError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("runtime error: not define flow for pax type %s after processor %s ", m_strMobType, m_strProcName );

	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";
	
	return strReturn;
}

FORMATMSG ARCFlowUndefinedError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;
	
	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strMobType = m_strMobType;
	formatMsg.strProcessor = m_strProcName;
	formatMsg.strOtherInfo = m_strOtherMsg;
	
	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

//////////////////////////////////////////////////////////////////////////
// ARCDestProcessorUnavailableError
ARCDestProcessorUnavailableError::ARCDestProcessorUnavailableError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg/* = ""*/, const CString& _strTime/* = ""*/ )
								: ARCFlowError( _strMobType, _strProcName, _strOtherMsg, _strTime )
{
	m_strErrorType = "DESTINATION PROCESSOR UNAVAILABLE";
}

ARCDestProcessorUnavailableError::~ARCDestProcessorUnavailableError()
{

}

//virtual function
CString ARCDestProcessorUnavailableError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("runtime error: pax type %s have not available destination at processor %s ", m_strMobType, m_strProcName );

	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";
	
	return strReturn;
}

FORMATMSG ARCDestProcessorUnavailableError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;
	
	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strMobType = m_strMobType;
	formatMsg.strProcessor = m_strProcName;
	formatMsg.strOtherInfo = m_strOtherMsg;
	
	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}
//////////////////////////////////////////////////////////////////////////
// ARCLackOfNecessaryInfo
ARCLackOfNecessaryInfo::ARCLackOfNecessaryInfo( const CString& _strMsg, const CString& _strOtherMsg /*= ""*/,const CString& _strTime/* = ""*/ )
					   :ARCSimEngineException( _strMsg, _strOtherMsg, _strTime )
{
	m_strErrorType = "LACK OF NECESSARY INFO";
}

ARCLackOfNecessaryInfo::~ARCLackOfNecessaryInfo()
{

}

//virtual function
CString ARCLackOfNecessaryInfo::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("%s",m_strErrorMsg);

	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";
	
	return strReturn;
}

FORMATMSG ARCLackOfNecessaryInfo::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;

	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strOtherInfo = m_strErrorMsg + "  " + m_strOtherMsg;

	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

//////////////////////////////////////////////////////////////////////////
// 	ARCNoEmergeExitError
ARCNoEmergeExitError::ARCNoEmergeExitError( const CString& _strMsg, const CString& _strOtherMsg /*= ""*/, const CString& _strTime /*= ""*/)
					:ARCSimEngineException( _strMsg, _strOtherMsg, _strTime )
{
	m_strErrorType = " NO EMERGE EXIT "; 
}

ARCNoEmergeExitError:: ~ARCNoEmergeExitError( )
{

}

//virtual function
CString ARCNoEmergeExitError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("%s",m_strErrorMsg);
	
	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";
	
	return strReturn;
}
	
FORMATMSG ARCNoEmergeExitError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;
	
	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strOtherInfo = m_strErrorMsg + "  " + m_strOtherMsg;
	
	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

//////////////////////////////////////////////////////////////////////////
// 	ARCConveyorSimultaneousServiceError
ARCConveyorSimultaneousServiceError::ARCConveyorSimultaneousServiceError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg /*= ""*/, const CString& _strTime/* = ""*/ )
{
	m_strMobType = _strMobType;
	m_strProcName = _strProcName;
	m_strOtherMsg = _strOtherMsg;
	m_strErrorTime  = _strTime;

	m_strErrorType = " CONVEYOR SYSTEM: SIMULTANEOUS SERVICE ";
}

ARCConveyorSimultaneousServiceError::~ARCConveyorSimultaneousServiceError()
{

}

//virtual function
CString ARCConveyorSimultaneousServiceError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("conveyor system simultaneous service error");

	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";

	return strReturn;
}

FORMATMSG ARCConveyorSimultaneousServiceError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;

	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strMobType = m_strMobType;
	formatMsg.strProcessor = m_strProcName;
	formatMsg.strOtherInfo = m_strOtherMsg;

	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

//////////////////////////////////////////////////////////////////////////
// ARCBarrierSystemError
ARCBarrierSystemError::ARCBarrierSystemError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg/* = ""*/, const CString& _strTime/* = ""*/ )
	: ARCFlowError( _strMobType, _strProcName, _strOtherMsg, _strTime )
{
	m_strErrorType = "BARRIER SYSTEM";
}

ARCBarrierSystemError::~ARCBarrierSystemError()
{

}

//virtual function
CString ARCBarrierSystemError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format(" barrier system error: pax type %s processor %s ", m_strMobType, m_strProcName );

	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";

	return strReturn;
}

FORMATMSG ARCBarrierSystemError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;

	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strMobType = m_strMobType;
	formatMsg.strProcessor = m_strProcName;
	formatMsg.strOtherInfo = m_strOtherMsg;

	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}

