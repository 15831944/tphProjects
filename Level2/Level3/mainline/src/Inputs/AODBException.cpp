#include "StdAfx.h"
#include ".\aodbexception.h"



const CString AODBException::TypeName[] = 
{
	_T("Warning"),
		_T("Error"),
		_T("Fatel Error")
};



AODBException::AODBException(ExceptionType enumType,const CString& strDescription)
{
	m_enumType = enumType;
	m_strDescription = strDescription;
}

AODBException::~AODBException(void)
{
}

CString AODBException::getErrorType()
{
	if(m_enumType>= ET_WARNING && m_enumType < ET_COUNT)
	{
		return TypeName[m_enumType];
	}
	return "";
}

CString AODBException::getDescription() const
{
	return m_strDescription;
}