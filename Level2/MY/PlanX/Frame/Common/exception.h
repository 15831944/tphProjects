#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

//#include <math.h>
#include "fsstream.h"

class Exception
{
protected:
    const char *errorType;
    const char *errorName;
    const char *message;
public:
    Exception (const char *p_msg) { message = p_msg; }
    void raise (void) { throw *this; }
    virtual void getMessage (char *string) const;

    friend stdostream& operator << (stdostream& s, const Exception& p_error);
};


/*****
*
*   Basic ARCError Types
*
*****/

class FatalError : public Exception
{
public:
    FatalError (const char *p_msg) : Exception (p_msg)
        { errorType = "Fatal Error"; }
};

class ARCError : public Exception
{
public:
    ARCError (const char *p_msg) : Exception (p_msg) { errorType = "Error"; }
};

class Warning : public Exception
{
public:
    Warning (const char *p_msg) : Exception (p_msg) { errorType = "Warning"; }
};


class StringError : public ARCError
{
public:
    StringError (const char *p_msg) : ARCError (p_msg) { errorName = ":"; }
};

class ARCInvalidTimeError : public ARCError  
{
public:
	ARCInvalidTimeError(const char *p_msg):ARCError (p_msg){errorName = ":";};
	virtual ~ARCInvalidTimeError(){};
	
};

class TwoStringError : public ARCError
{
protected:
    char *tempString;
public:
    TwoStringError (const char *p_msg1, const char *p_msg2);
    virtual ~TwoStringError () { delete [] tempString; }
};

class OutOfRangeError : public FatalError
{
public:
    OutOfRangeError (const char *p_msg) : FatalError (p_msg)
        { errorName = ", index out of range"; }
};

class ShutDownError : public ARCError
{
public:
    ShutDownError () : ARCError (NULL)
        { errorName = ", Shutting down event system"; }
};

/*****
*
*   File Errors
*
*****/

class FileNotFoundError : public FatalError
{
public:
    FileNotFoundError (const char *p_filename) : FatalError (p_filename)
        { errorName = ", cannot find file"; }
};

class FileLockedError : public ARCError
{
public:
    FileLockedError (const char *p_filename) : ARCError (p_filename)
        { errorName = ", file is locked"; }
};

class TooManyFilesError : public FatalError
{
public:
    TooManyFilesError (const char *p_filename) : FatalError (p_filename)
        { errorName = ", maximum open files exceeded"; }
};

class FileCreateError : public FatalError
{
public:
    FileCreateError (const char *p_filename) : FatalError (p_filename)
        { errorName = " creating file"; }
};

class FileOpenError : public FatalError
{
public:
    FileOpenError (const char *p_filename) : FatalError (p_filename)
        { errorName = " opening file"; }
};

class FileReadError : public FatalError
{
public:
    FileReadError (const char *p_filename) : FatalError (p_filename)
        { errorName = " reading file"; }
};

class FileWriteError : public FatalError
{
public:
    FileWriteError (const char *p_filename) : FatalError (p_filename)
        { errorName = " writing file"; }
};

class FileFormatError : public FatalError
{
public:
    FileFormatError (const char *p_filename) : FatalError (p_filename)
        { errorName = ", bad file format"; }
};

class EndOfFileError : public FatalError
{
public:
    EndOfFileError (const char *p_function) : FatalError (p_function)
        { errorName = ", attempt to read beyond end of file"; }
};

class ObsoleteFileError : public FatalError
{
public:
    ObsoleteFileError (const char *p_function) : FatalError (p_function)
        { errorName = ", Obsolete file found"; }
};

class FileVersionTooNewError : public ARCError
{
public:
    FileVersionTooNewError (const char *p_filename) : ARCError (p_filename)
        { errorName = ", file version is too new"; }
};

/*****
*
*   Disk Errors
*
*****/

class DiskAccessError : public FatalError
{
public:
    DiskAccessError (const char *p_drive) : FatalError (p_drive)
        { errorName = ", drive not ready"; }
};

class WriteProtectError : public FatalError
{
public:
    WriteProtectError (const char *p_drive) : FatalError (p_drive)
        { errorName = ", write protected drive"; }
};

class DiskSpaceError : public FatalError
{
public:
    DiskSpaceError (const char *p_drive) : FatalError (p_drive)
        { errorName = ", insufficient space on drive"; }
};

class DirNotFoundError : public FatalError
{
public:
    DirNotFoundError (const char *p_dir) : FatalError (p_dir)
        { errorName = ", directory not found"; }
};


class DirCreateError : public FatalError
{
public:
    DirCreateError (const char *p_dir) : FatalError (p_dir)
        { errorName = ", creating directory"; }
};




/*****
*
*   Math Errors
*
*****/

#ifdef __FLAT__
	int _matherr (struct exception *e);
	int _matherrl (struct _exceptionl *e);
#endif

class DomainError : public ARCError
{
public:
    DomainError (const char *p_function) : ARCError (p_function)
        { errorName = ", mathematic domain exceeded in function"; }
};

class SingularityError : public ARCError
{
public:
    SingularityError (const char *p_function) : ARCError (p_function)
        { errorName = ", mathematic singularity generated in function"; }
};

class OverflowError : public ARCError
{
public:
    OverflowError (const char *p_function) : ARCError (p_function)
        { errorName = ", mathematic overflow in function"; }
};

class UnderflowError : public ARCError
{
public:
    UnderflowError (const char *p_function) : ARCError (p_function)
        { errorName = ", mathematic underflow in function"; }
};

class TotalLossError : public ARCError
{
public:
    TotalLossError (const char *p_function) : ARCError (p_function)
        { errorName = ", total loss of significant digits in math function"; }
};

class PrintError : public ARCError
{
protected:
	int errorCode;
public:
	PrintError (const char *p_msg, int p_errorCode = 0)
        : ARCError (p_msg), errorCode (p_errorCode)
	{
        errorName = "Printing ";
	}
	int GetErrorCode ()
	{
		return errorCode;
	}
};


#endif
