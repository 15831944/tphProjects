#pragma once
/**
	Class to provide SEH exception handle, avoid crash when open project and run simulation.
*/

//warning C4535: calling _set_se_translator() requires /EHa
#pragma warning(disable: 4535)

#include <stdexcept>
class seh_exception_base : public std::exception
{
public:
	seh_exception_base (const PEXCEPTION_POINTERS pExp, std::string what );;
	~seh_exception_base() throw(){} ;

	virtual const char* what() const throw(){
		return m_what.c_str();
	}
	virtual DWORD exception_code() const throw(){
		return m_ExceptionRecord.ExceptionCode;
	}
	virtual const EXCEPTION_RECORD& get_exception_record() const throw(){
		return m_ExceptionRecord;
	}
	virtual const CONTEXT& get_context() const throw(){
		return m_ContextRecord;
	}
	void DumpToStream(std::ostream& os);

	static void setALTOExceptionHandler();

	static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp );
	static void purecall_func();

protected:

	std::string m_what;
	EXCEPTION_RECORD m_ExceptionRecord;
	CONTEXT m_ContextRecord;

};


class ALTOException : public seh_exception_base
{
public:

	ALTOException (const PEXCEPTION_POINTERS pExp, std::string what)
		:seh_exception_base(pExp, what) {};
	~ALTOException() throw(){};
};



