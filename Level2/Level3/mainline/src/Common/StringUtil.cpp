#include "StdAfx.h"
#include ".\stringutil.h"
#include <iosfwd>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
bool CStringUtil::ToInteger( const CString& val,int& ret )
{
	std::istringstream str(val.GetString());	
	str >> ret;
	return !str.fail() && str.eof();
}

bool CStringUtil::ToReal( const CString& val, double& ret )
{
	std::istringstream str(val.GetString());	
	str >> ret;
	return !str.fail() && str.eof();
}

CString CStringUtil::FromInteger( int fint )
{
	CString str;
	str.Format("%d",fint);
	return str;
}
