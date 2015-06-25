#include "stdafx.h"
#include "LaneRange.h"

CLaneRange::CLaneRange()
{
}


CLaneRange::CLaneRange(int nStart, int nEnd)
:m_nStartLane(nStart), m_nEndLane(nEnd)
{
}

CLaneRange::~CLaneRange(void)
{
}

void CLaneRange::ParseDatabaseString(const CString& strDatabaseString)
{
	if (strDatabaseString.CompareNoCase("All") == 0)
	{
		m_nStartLane = 0;
		m_nEndLane = 0;
	}
	else
	{
		int nPos = strDatabaseString.Find(',');
		if (nPos != -1)
		{
			CString strLeft;
			CString strRight;
			strLeft = strDatabaseString.Left(nPos);
			m_nStartLane = atoi(strLeft);
			int nLengh = strDatabaseString.GetLength();
			strRight = strDatabaseString.Right(nLengh - nPos - 1);
			m_nEndLane = atoi(strRight);
		}
	}
}

CString CLaneRange::GetLaneRange() const
{
	CString strRet;
	if (m_nStartLane == 0 || m_nEndLane == 0)
	{
		strRet.Format("All");
	} 
	else
	{
		strRet.Format("Lane:%d - Lane:%d",m_nStartLane,m_nEndLane);
	}	
	return strRet;
}

CString CLaneRange::MakeDatabaseString() const
{
	CString strData;
	strData.Format(_T("%d,%d"),m_nStartLane,m_nEndLane);
	return strData;
}
