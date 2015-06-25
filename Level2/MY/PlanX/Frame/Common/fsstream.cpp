#include <fcntl.h>      //O_XXXXXX
#include <sys\stat.h>   //S_IXXXXX, stat()
#include <io.h>         //sopen(), close(), read(), write()
//#include <errno.h>
#include "exception.h"
#include "fileman.h"
#include "fsstream.h"

using namespace std ;

void fsstream::open (const char *szName, ios_base::openmode  nMode)
{	
	basic_fstream<char>::open(szName, nMode);
}

fsstream::fsstream (const char *szName, ios_base::openmode  nMode):
	basic_fstream<char> (szName, nMode)	{ ; }

ifsstream::ifsstream (const char *szName, ios_base::openmode  nMode)
	: ifstream(szName, nMode){ ;}

void ifsstream::open (const char *szName, ios_base::openmode  nMode)
{ 
	ifstream::open (szName, nMode); 
}

ofsstream::ofsstream (const char *szName, ios_base::openmode  nMode) 
	:ofstream(szName, nMode)
{ ; }

void ofsstream::open (const char *szName, ios_base::openmode  nMode)
{ ofstream::open (szName, nMode); }
