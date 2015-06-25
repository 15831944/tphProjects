// ExceptionMsgDB.h: interface for the ExceptionMsgDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCEPTIONMSGDB_H__53931D67_61D0_446F_9249_E5E7C3D86B5A__INCLUDED_)
#define AFX_EXCEPTIONMSGDB_H__53931D67_61D0_446F_9249_E5E7C3D86B5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ExceptionMsgDB  
{
private:
	static ExceptionMsgDB* m_pInstance;
private:
	ExceptionMsgDB();
	virtual ~ExceptionMsgDB();
public:
	static ExceptionMsgDB* GetExceptionMsgDB();
	CString GetErrorMessage( enum ErrorCode _iErrorCode );

};

#endif // !defined(AFX_EXCEPTIONMSGDB_H__53931D67_61D0_446F_9249_E5E7C3D86B5A__INCLUDED_)
