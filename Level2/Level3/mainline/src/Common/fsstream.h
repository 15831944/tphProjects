//fsstream.h
//Anthony Malizia
//July 4, 1995

#ifndef FSSTREAM_H
#define FSSTREAM_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning(disable:4275)

#include <fstream>
#include <share.h>
#include "replace.h"
#include "commondll.h"


class COMMON_TRANSFER fsstream : public stdfstream
{
public:
//    enum { sh_compat = 0, sh_none = 1, sh_read = 2, sh_write = 4};
	fsstream()  {};
	void open (const char *szName, stdios::openmode  nMode, int nShare);
	fsstream (const char *szName, int nMode, int nShare = _SH_DENYRW);
	virtual ~fsstream() {};
};

class COMMON_TRANSFER ifsstream : public stdifstream
{

public:
	ifsstream(){}
	ifsstream (const char *szName, stdios::openmode nMode = stdios::in, int nShare = SH_DENYNONE);
	virtual void open (const char *szName, stdios::openmode nMode = stdios::in, int nShare = SH_DENYNONE);
	virtual ~ifsstream() { ;}
};

class COMMON_TRANSFER ofsstream : public stdofstream
{
public:
	ofsstream() {}
	ofsstream (const char *szName, stdios::openmode nMode = stdios::out, int nShare = SH_DENYRW) ;
	virtual void open (const char *szName, stdios::openmode nMode = stdios::out, int nShare = SH_DENYRW);
	virtual ~ofsstream() {;}
};



#endif
