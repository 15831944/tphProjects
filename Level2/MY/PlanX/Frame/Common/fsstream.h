#ifndef FSSTREAM_H
#define FSSTREAM_H

#pragma warning(disable:4275)

#include <fstream>
#include <share.h>
#include "commondll.h"


class COMMON_TRANSFER fsstream : public std::fstream
{
public:
	fsstream()  {};
	void open (const char *szName, std::ios::openmode nMode);
	fsstream (const char *szName, int nMode);
	virtual ~fsstream() {};
};

class COMMON_TRANSFER ifsstream : public std::ifstream
{

public:
	ifsstream(){}
	ifsstream (const char *szName, std::ios::openmode nMode = std::ios::in);
	virtual void open (const char *szName, std::ios::openmode nMode = std::ios::in);
	virtual ~ifsstream(){}
};

class COMMON_TRANSFER ofsstream : public std::ofstream
{
public:
	ofsstream(){}
	ofsstream (const char *szName, std::ios::openmode nMode = std::ios::out);
	virtual void open (const char *szName, std::ios::openmode nMode = std::ios::out);
	virtual ~ofsstream() {}
};



#endif //FSSTREAM_H
