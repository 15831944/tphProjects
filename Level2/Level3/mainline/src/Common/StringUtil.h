#pragma once

class CStringUtil
{
public:
	static bool ToInteger(const CString& str, int& ret);
	static bool ToReal(const CString& str, double& ret);
	static CString FromInteger(int fint);
};
