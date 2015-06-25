#ifndef REPLACE_H
#define REPLACE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include<cfloat>
#include<climits>
#include<cstdlib>


#define SH_DENYNONE _SH_DENYNO

#define huge

#define far

const double M_PI = 3.1415926535897932;

const double M_PI_2 = (3.1415926535897932 / 2.0);

const double MAXDOUBLE = DBL_MAX;

const double MINFLOAT = FLT_MIN;

const float MAXFLOAT = FLT_MAX;

//const int MAXINT = INT_MAX;

const int MAXPATH = _MAX_PATH;

const int MAXDRIVE = _MAX_DRIVE;

const int MAXDIR = _MAX_DIR;

const int MAXFILE = _MAX_FNAME;

const int MAXEXT = _MAX_EXT;

#define fnsplit _splitpath

#define stdvector std::vector

#define stdmap std::map

#define stdostream std::ostream

#define stdistream std::istream

#define stdios_base std::ios

#define stdios std::ios

#define stdifstream std::ifstream

#define stdfstream std::fstream

#define stdofstream std::ofstream

#define stdistrstream std::istrstream

#define stdostrstream std::ostrstream

#define stdendl std::endl

#define stdcerr std::cerr

#define stdsort std::sort

#define stdfind std::find

#define stdsetprecision std::setprecision

#define random(a) ( rand() % (a) )
#endif
