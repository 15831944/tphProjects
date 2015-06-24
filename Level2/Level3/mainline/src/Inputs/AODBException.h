#pragma once

class AODBException
{
public:
	enum ExceptionType
	{
		ET_WARNING = 0,
		ET_ERROR,
		ET_FATEL,



		ET_COUNT

	};
	const static CString TypeName[];

public:
	AODBException(ExceptionType enumType,const CString& strDescription);
	virtual ~AODBException(void);


public:
	CString getErrorType();
	CString getDescription() const;
	//void setErrorDescription(const CString& strDesc) { m_strErrorDescription = strDesc; }

protected:
	CString m_strDescription;
	ExceptionType m_enumType;


};
