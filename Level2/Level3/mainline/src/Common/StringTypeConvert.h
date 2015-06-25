#include <iostream>
#include <sstream>

template<class T>
bool toType(const CString& s,T&d)
{
	std::stringstream str(s.GetString());
	str>>d;
	return !str.fail();
}

template<class T> 
CString toString(const T& d)
{
	std::string str;
	std::stringstream stream;
	stream<<d;
	stream>>str;
	return str.c_str();
}