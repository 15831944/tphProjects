#include "StdAfx.h"
#include ".\rangenumbers.h"


void RangeNumbers::AddNumber( int n )
{
	m_vInNumbers.insert(n);
}

void RangeNumbers::AddRange( int nMin, int nMax )
{
	ASSERT(nMin<=nMax);
	for(int i= nMin; i<=nMax; i++)
	{
		m_vInNumbers.insert(i);
	}
}

CString RangeNumbers::PrintString() const
{
	if(m_vInNumbers.size() == 0) 
		return "";
	
	std::set<int>::const_iterator itr = m_vInNumbers.begin();
	int nLastNum = *m_vInNumbers.rbegin();

	int nMin = *itr;
	int nMax = nMin;
	itr++;

	CString strret;
	for(;itr!=m_vInNumbers.end();itr++)
	{
		int nNext = *itr;
		if(nMax+1 != nNext)
		{
			CString strNum;
			if(nMin = nMax)
			{
				strNum.Format("%d",nMax);
			}	
			else
			{
				strNum.Format("%d-%d", nMin, nMax);
			}
			
			if(nLastNum == nNext)
			{
				strret = strret+strNum+",";
			}
			else
			{
				strret = strret + strNum;
			}
			
			nMin = nNext;
			nMax = nNext;
		}
		else
		{
			nMax++;
		}
	}
	return strret;
}