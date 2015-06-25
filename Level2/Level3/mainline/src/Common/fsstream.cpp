#include "stdafx.h"
#include <fcntl.h>      //O_XXXXXX
#include <sys\stat.h>   //S_IXXXXX, stat()
#include <share.h>      //SH_XXXXX
#include <io.h>         //sopen(), close(), read(), write()
//#include <errno.h>
#include "exeption.h"
#include "fileman.h"
#include "fsstream.h"

using namespace std ;

void fsstream::open (const char *szName, ios_base::openmode  nMode, int nShare)
{	
	basic_fstream<char>::open(szName, nMode);
}

fsstream::fsstream (const char *szName, ios_base::openmode  nMode, int nShare):
	basic_fstream<char> (szName, nMode)	{ ; }

ifsstream::ifsstream (const char *szName, ios_base::openmode  nMode, int nShare)
	: ifstream(szName, nMode){ ;}

void ifsstream::open (const char *szName, ios_base::openmode  nMode, int nShare)
{ 
	ifstream::open (szName, nMode); 
}

ofsstream::ofsstream (const char *szName, ios_base::openmode  nMode , int nShare) 
	:ofstream(szName, nMode)
{ ; }

void ofsstream::open (const char *szName, ios_base::openmode  nMode , int nShare)
{ ofstream::open (szName, nMode); }
