#pragma once

class ARCVector2;
class ARCVector3;
class Path;
class CPath2008;
#include <common/point.h>
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_float.hpp>
class ARCStringConvert
{
public:
	static CString toString(const ARCVector3& );
	static ARCVector3 parseVector3(const CString& vec3str);

	static CString toString(const ARCVector2& );
	static ARCVector2 parseVector2(const CString& vec2str);

	static CString toString(const int& d);
	static CString toString(const double& d);

	static bool isNumber(const CString& val, double& fout);

	static bool isAllNumeric(const char* buf);

	static void NextAlphaNumeric(char* buf);

	static CString toString(const Path& );
	static Path parsePath(CString);

	static CString toDBString(const CPath2008& path);
	static bool parsePathFromDBString(CPath2008& path, CString strPath);

	static Point paresePoint(CString );
	static CString toString(const std::vector<bool>& blist);
	static std::vector<bool> parseBoolList(CString );

	template<class ContainerType>
	static int toString( char* buffer, ContainerType& values, int maxLen);

	template<class ContainerType>
	static int toString( CString& cbuffer, ContainerType& values, int maxLen);

	template<class ContainerType>
	static bool toValues(ContainerType& list, char* buffer);
};


/***********************************************************************
buffer[out]: "xxxx,xxxx,xxxx," ;end with '\0'.
values[in]: input values.
maxLen[in]: max length limit of buffer, default 2048 bytes.
return value: if success, return length of buffer, else return -1
***********************************************************************/
template<class ContainerType>
int ARCStringConvert::toString(char* buffer, ContainerType& values, int maxLen = 2048)
{
	using namespace boost;
	typename typedef ContainerType::value_type	value_type;
	typename typedef ContainerType::iterator	value_iter;

	if (!buffer) return -1;

	int len = 0, outLen = 0;
	char str[16] = { 0 };

	//wrong syntax 
	if (is_arithmetic<value_type>::value != true)
		return false;

	size_t count = values.size();
	for (size_t k = 0; k < count; k++)
	{
		//memset(str, 0x0, 16);
		len = sprintf_s(str, "%.2f", values[k]);
		memcpy(buffer+outLen, str, len);
		outLen += len;
		buffer[outLen] = ',';
		outLen += 1;
	}
	buffer[outLen] = '\0';
	outLen += 1;

	ASSERT(outLen <= maxLen);
	return outLen;
}

/***********************************************************************
cbuffer[out]: "xxxx,xxxx,xxxx," ;end with '\0'.
values[in]: input values.
maxLen[in]: max length limit of buffer, default 1024 bytes.
return value: if success, return length of buffer, else return -1
***********************************************************************/
template<class ContainerType>
int ARCStringConvert::toString(CString& cbuffer, ContainerType& values, int maxLen = 2048)
{
	using namespace boost;
	typename typedef ContainerType::value_type	value_type;
	typename typedef ContainerType::iterator	value_iter;

	char* buffer = cbuffer.GetBuffer(maxLen);
	if (!buffer) return -1;

	int outLen = toString(buffer, values, maxLen);

	cbuffer.ReleaseBuffer();
	return outLen;
}

/***********************************************************************
values[out]: input values
buffer[in]: "xxxx,xxxx,xxxx," or "xxxx-xxxx-xxxx-"
***********************************************************************/
template<class ContainerType>
bool ARCStringConvert::toValues(ContainerType& list, char* buffer)
{
	using namespace boost;
	typename typedef ContainerType::value_type	value_type;
	typename typedef ContainerType::iterator	value_iter;

	if(!buffer) return false;

	list.clear();
	char seps[] = ",-\0";

	char* token = NULL;

	char *next_token = NULL;
	if(is_integral<value_type>::value == true)
	{
		token = strtok_s(buffer, seps, &next_token);
		while(token != NULL)
		{
			list.push_back( (value_type)atoi(token) );
			token = strtok_s(NULL, seps, &next_token);
		}
	}
	else if (is_float<value_type>::value == true)
	{
		token = strtok_s(buffer, seps, &next_token);
		while(token != NULL)
		{
			list.push_back( (value_type)atof(token) );
			token = strtok_s(NULL, seps, &next_token);
		}
	}
	else
		return false;

	return true;
}

#define TOSTRING(x) ARCStringConvert::toString(x).GetString()
#define TOVECTOR3(x) ARCStringConvert::parseVector3(x)
#define TOVECTOR2(x) ARCStringConvert::parseVector2(x)