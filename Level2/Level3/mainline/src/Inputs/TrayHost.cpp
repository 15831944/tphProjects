#include "stdafx.h"
#include "TrayHost.h"
#include <algorithm>
////////////////////////////////////////////////////////////////////////////////////////////////////
TrayHost::TrayHost()
{
	m_sHost = _T("");
	m_nHostIndex = 1;
	m_nRadius = 0;
}

TrayHost::~TrayHost()
{

}

void TrayHost::GetDataFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("NAME"),m_sHost);
	recordset.GetFieldValue(_T("HOSTINDEX"),m_nHostIndex);
	recordset.GetFieldValue(_T("RADIUS"),m_nRadius);
	CString sNonPaxList(_T(""));
	recordset.GetFieldValue(_T("NONPAXLIST"),sNonPaxList);
	ConveyToNonPaxList(sNonPaxList);
}

CString TrayHost::GetTableName()const
{
	return CString(_T("TRAYHOST"));
}

bool TrayHost::GetInsertSQL(int nParentID,CString& strSQL)const
{
	strSQL.Format(_T("INSERT INTO TRAYHOST\
					 (PROJID,NAME, HOSTINDEX, RADIUS, NONPAXLIST)\
					 VALUES (%d,'%s',%d,%d,'%s')"),nParentID,m_sHost,m_nHostIndex,m_nRadius,FormatNonPaxList(m_vNonPaxIndex));
	return true;
}

bool TrayHost::GetUpdateSQL(CString& strSQL)const
{
	strSQL.Format(_T("UPDATE TRAYHOST\
					 SET NAME ='%s',HOSTINDEX = %d, RADIUS =%d, NONPAXLIST ='%s'\
					 WHERE (ID = %d)"),m_sHost,m_nHostIndex,m_nRadius,FormatNonPaxList(m_vNonPaxIndex),m_nID);

	return true;
}

void TrayHost::GetDeleteSQL(CString& strSQL)const
{
	strSQL.Format(_T("DELETE FROM TRAYHOST\
					 WHERE (ID = %d)"),m_nID);
}

void TrayHost::GetSelectSQL(int nID,CString& strSQL)const
{

}

bool TrayHost::RemoveItem(int nNonPaxIndex)
{
	std::vector<int>::iterator iter = std::find(m_vNonPaxIndex.begin(),m_vNonPaxIndex.end(),nNonPaxIndex);
	if (iter != m_vNonPaxIndex.end())
	{
		m_vNonPaxIndex.erase(iter);
		return true;
	}
	return false;
}

int TrayHost::GetItem(int nIndex)
{
	ASSERT(nIndex > -1 && nIndex < GetCount());
	return m_vNonPaxIndex[nIndex];
}

int TrayHost::FindItem(int nNnPaxIndex)
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (m_vNonPaxIndex[i] == nNnPaxIndex)
		{
			return i;
		}
	}
	return -1;
}

void TrayHost::SetItem(int nIndex, int nNonPaxIndex)
{
	ASSERT(nIndex > -1 && nIndex < GetCount());
	m_vNonPaxIndex[nIndex] = nNonPaxIndex;
}

bool TrayHost::IsEqual(const std::vector<int>& vNonPaxIndex)
{
	if (m_vNonPaxIndex.size() != vNonPaxIndex.size())
	{
		return false;
	}

	for (int i = 0; i < GetCount(); i++)
	{
		if (std::find(vNonPaxIndex.begin(),vNonPaxIndex.end(),m_vNonPaxIndex[i]) == vNonPaxIndex.end())
		{
			return false;
		}
	}
	return true;
}

CString TrayHost::FormatNonPaxList(const std::vector<int>&vData)const
{
	CString strData = _T("");

	for (int i = 0; i < (int)vData.size(); i++)
	{
		CString strTemp = _T("");
		strTemp.Format(_T("%d"),vData.at(i));
		if (strData.IsEmpty())
		{
			strData = strTemp;
		}
		else
		{
			strData += "," + strTemp;
		}
	}

	return strData;
}

void TrayHost::ConveyToNonPaxList(CString& strNonPaxList)
{
	if (strNonPaxList.IsEmpty())
	{
		return;
	}
	int nPos = strNonPaxList.Find(',');
	while (nPos != -1)
	{
		m_vNonPaxIndex.push_back(atoi(strNonPaxList.Left(nPos)));

		strNonPaxList = strNonPaxList.Right(strNonPaxList.GetLength()-nPos-1);
		nPos = strNonPaxList.Find(',');
	}
	// last item
	m_vNonPaxIndex.push_back( atoi(strNonPaxList));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
TrayHostList::TrayHostList()
{

}

TrayHostList::~TrayHostList()
{

}

void TrayHostList::GetSelectElementSQL(int nParentID,CString& strSQL)const
{
	strSQL.Format(_T("SELECT *\
					 FROM TRAYHOST\
					 WHERE (PROJID = %d)"), nParentID);
}

void TrayHostList::ReadData(int nParentID)
{
	CleanData();

	CString strSelectSQL;
	GetSelectElementSQL(nParentID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		TrayHost* element = new TrayHost;
		element->GetDataFromDBRecordset(adoRecordset);
		m_vDatalist.push_back(element);
		adoRecordset.MoveNextData();
	}
}

int TrayHostList::FindItem(const CString& sHostName)
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		TrayHost* pHost = GetItem(i);
		if (pHost->GetHostName() == sHostName)
		{
			return i;
		}
	}
	return -1;
}

bool TrayHostList::FindHostItem(int nNonPaxIndex)
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		TrayHost* pHost = GetItem(i);
		if (pHost->GetHostIndex() == nNonPaxIndex)
		{
			return true;
		}
	}
	return false;
}

bool TrayHostList::IfNonHostPaxExsit(int nNonPaxIndex)
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		TrayHost* pHost = GetItem(i);
		if(pHost->FindItem(nNonPaxIndex) != -1)
			return true;
	}	
	return false;
}

bool TrayHostList::IfNonPaxExsit(int nNonPaxIndex)
{
	return (FindHostItem(nNonPaxIndex)||IfNonHostPaxExsit(nNonPaxIndex));
}

int TrayHostList::FindItem(int nNonPaxHostIndex,std::vector<int>vNonPaxIndex)
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		TrayHost* pHost = GetItem(i);
		if (pHost->GetHostIndex() == nNonPaxHostIndex &&\
			pHost->IsEqual(vNonPaxIndex))
		{
			return i;
		}
	}
	return -1;
}