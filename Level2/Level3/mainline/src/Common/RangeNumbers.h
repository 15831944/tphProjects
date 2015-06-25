#pragma once
#include <set>

class RangeNumbers
{
public:
	void AddNumber(int n);
	void AddRange(int nMin, int nMax);
	
	CString PrintString()const;
protected:
	std::set<int> m_vInNumbers;
};
