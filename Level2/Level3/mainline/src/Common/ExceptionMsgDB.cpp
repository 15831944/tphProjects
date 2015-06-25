// ExceptionMsgDB.cpp: implementation of the ExceptionMsgDB class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ExceptionMsgDB.h"

#include "template.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ExceptionMsgDB* ExceptionMsgDB::m_pInstance = NULL;

ExceptionMsgDB::ExceptionMsgDB()
{

}

ExceptionMsgDB::~ExceptionMsgDB()
{

}

ExceptionMsgDB* ExceptionMsgDB::GetExceptionMsgDB()
{
	if( !m_pInstance )
	{
		m_pInstance = new ExceptionMsgDB;
	}
	return m_pInstance;
}

CString ExceptionMsgDB::GetErrorMessage( enum ErrorCode _iErrorCode )
{
	return ErrorMessageDB[ _iErrorCode ];
}