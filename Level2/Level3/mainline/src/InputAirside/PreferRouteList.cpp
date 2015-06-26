// PreferRouteList.cpp : implementation file
//

#include "stdafx.h"
#include "InputAirside.h"
#include "..\Database\ADODatabase.h"
#include ".\Common\CommonDll.h"
#include "PreferRouteList.h"


// CPreferRouteList

CPreferRouteList::CPreferRouteList()
:m_vPerferRouteList(NULL)
,m_vDelPreferRouteList(NULL)
{
}

CPreferRouteList::~CPreferRouteList()
{
	std::vector<CPreferRouteItem*>::iterator iter = m_vPerferRouteList.begin();
	for (; iter != m_vPerferRouteList.end(); ++iter)
	{
		delete *iter;
	}
	iter = m_vDelPreferRouteList.begin();
	for (; iter != m_vDelPreferRouteList.end(); ++iter)
	{
		delete *iter;
	}
	m_vPerferRouteList.clear();
	m_vDelPreferRouteList.clear();
}

void CPreferRouteList::AddItem(CPreferRouteItem* pPreferItem)
{
	ASSERT(pPreferItem);
	m_vPerferRouteList.push_back(pPreferItem);
}

bool CPreferRouteList::DeleteItem(CPreferRouteItem* pPreferItem)
{
	ASSERT(pPreferItem);
	m_vDelPreferRouteList.push_back(pPreferItem);
	std::vector<CPreferRouteItem*>::iterator iter = m_vPerferRouteList.begin();
	for (; iter != m_vPerferRouteList.end(); ++iter)
	{
		if (pPreferItem->getStandID() == (*iter)->getStandID())
		{
			m_vPerferRouteList.erase(iter);
			return true;
		}
	}
	return false;
}

bool CPreferRouteList::DeleteItemData(int nStandID)
{
	std::vector<CPreferRouteItem*>::iterator iter = m_vPerferRouteList.begin();
	for (; iter != m_vPerferRouteList.end(); ++iter)
	{
		if (nStandID == (*iter)->getStandID())
		{
			m_vDelPreferRouteList.push_back(*iter);
			m_vPerferRouteList.erase(iter);
			return true;
		}
	}
	return false;
}
bool CPreferRouteList::DeleteItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vPerferRouteList.size());
	m_vDelPreferRouteList.push_back(m_vPerferRouteList.at(nIndex));
	m_vPerferRouteList.erase(m_vPerferRouteList.begin()+nIndex);
	return true;
}

CPreferRouteItem* CPreferRouteList::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vPerferRouteList.size());
	return m_vPerferRouteList.at(nIndex);
}

CPreferRouteItem* CPreferRouteList::GetItemData(int nStandID)
{
	std::vector<CPreferRouteItem*>::iterator iter = m_vPerferRouteList.begin();
	for (; iter != m_vPerferRouteList.end(); ++iter)
	{
		if (nStandID == (*iter)->getStandID())
		{
			return (*iter);
		}
	}
	return NULL;
}

int CPreferRouteList::getPreferRouteCount()
{
	return (int)m_vPerferRouteList.size();
}

void CPreferRouteList::ReadData(int nProjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM PREFERROUTEINFO WHERE PROJID = %d"),nProjID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	if (adoRecordset.IsEOF())
	{
		m_emRouteType = ShortType;
	}
	else
	{
		m_emRouteType = PreferType;
	}
	while(!adoRecordset.IsEOF())
	{
		CPreferRouteItem* pPerferRouteItem = new CPreferRouteItem();
		pPerferRouteItem->ReadData(adoRecordset);
		m_vPerferRouteList.push_back(pPerferRouteItem);
		adoRecordset.MoveNextData();
	}
}

void CPreferRouteList::SaveData(int nProjID,RouteType emRouteType)
{
	std::vector<CPreferRouteItem*>::iterator iter = m_vPerferRouteList.begin();
	switch(emRouteType)
	{
	case PreferType:
		{
			for (; iter != m_vPerferRouteList.end(); ++iter)
			{
				(*iter)->SaveData(nProjID);
			}
			iter = m_vDelPreferRouteList.begin();
			for (; iter != m_vDelPreferRouteList.end(); ++iter)
			{
				(*iter)->DeleteData();
				delete (*iter);
			}
			m_vDelPreferRouteList.clear();
		}
		break;
	case ShortType:
		{
			for (; iter != m_vPerferRouteList.end(); ++iter)
			{
				(*iter)->DeleteData();
				delete *iter;
			}
			m_vPerferRouteList.clear();
		}
		break;
	default:
		break;
	}

}

//////////////////////////////////////////////////////////////////////////////
//CPrferRouteItem

CPreferRouteItem::CPreferRouteItem()
:m_nID(-1)
,m_vDeicePadList(NULL)
,m_vDelDeicePadList(NULL)
{

}

CPreferRouteItem::~CPreferRouteItem()
{
	std::vector<CDeicePadItem*>::iterator iter = m_vDeicePadList.begin();
	for (; iter != m_vDeicePadList.end(); ++iter)
	{
		delete *iter;
	}
	iter = m_vDelDeicePadList.begin();
	for (; iter != m_vDelDeicePadList.begin(); ++iter)
	{
		delete *iter;
	}
	m_vDeicePadList.clear();
	m_vDelDeicePadList.clear();
}

void CPreferRouteItem::AddItem(CDeicePadItem* pDeicePadItem)
{
	ASSERT(pDeicePadItem);
	m_vDeicePadList.push_back(pDeicePadItem);
}

int CPreferRouteItem::getDeicePadCount()
{
	return (int)m_vDeicePadList.size();
}
bool CPreferRouteItem::DeleteItem(CDeicePadItem* pDeicePadItem)
{
	ASSERT(pDeicePadItem);
	m_vDelDeicePadList.push_back(pDeicePadItem);
	std::vector<CDeicePadItem*>::iterator iter = m_vDeicePadList.begin();
	for (; iter != m_vDeicePadList.end(); ++iter)
	{
		if (pDeicePadItem->getDeicePadID() == (*iter)->getDeicePadID())
		{
			m_vDeicePadList.erase(iter);
			return true;
		}
	}
	return false;
}

bool CPreferRouteItem::DeleteItemData(int nDeicePadID)
{
	std::vector<CDeicePadItem*>::iterator iter = m_vDeicePadList.begin();
	for (; iter != m_vDeicePadList.end(); ++iter)
	{
		if (nDeicePadID == (*iter)->getDeicePadID())
		{
			m_vDelDeicePadList.push_back(*iter);
			m_vDeicePadList.erase(iter);
			return true;
		}
	}
	return false;
}
bool CPreferRouteItem::DeleteItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vDeicePadList.size());
	m_vDelDeicePadList.push_back(m_vDeicePadList.at(nIndex));
	m_vDeicePadList.erase(m_vDeicePadList.begin()+nIndex);
	return true;
}

CDeicePadItem* CPreferRouteItem::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vDeicePadList.size());
	return m_vDeicePadList.at(nIndex);
}

CDeicePadItem* CPreferRouteItem::GetItemData(int nDeicePadID)
{
	std::vector<CDeicePadItem*>::iterator iter = m_vDeicePadList.begin();
	for (; iter != m_vDeicePadList.end(); ++iter)
	{
		if (nDeicePadID == (*iter)->getDeicePadID())
		{
			return *iter;
		}
	}
	return NULL;
}
void CPreferRouteItem::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("STAND"),m_nStandID);

	ReadData();
}

void CPreferRouteItem::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM DEICEPADINFO WHERE NID = %d"),m_nID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CDeicePadItem* pDeiceData = new CDeicePadItem();
		pDeiceData->ReadData(adoRecordset);
		m_vDeicePadList.push_back(pDeiceData);
		adoRecordset.MoveNextData();
	}
}
void CPreferRouteItem::SaveData(int nProjID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO PREFERROUTEINFO (PROJID,STAND)\
					 VALUES (%d,%d)"),nProjID,m_nStandID);
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	
	std::vector<CDeicePadItem*>::iterator iter = m_vDeicePadList.begin();
	for (; iter != m_vDeicePadList.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}
	iter = m_vDelDeicePadList.begin();
	for (; iter != m_vDelDeicePadList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vDelDeicePadList.clear();
}

void CPreferRouteItem::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE PREFERROUTEINFO SET STAND = %d WHERE (ID = %d)"),m_nStandID,m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CDeicePadItem*>::iterator iter = m_vDeicePadList.begin();
	for (; iter != m_vDeicePadList.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}
	iter = m_vDelDeicePadList.begin();
	for (; iter != m_vDelDeicePadList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vDelDeicePadList.clear();
}

void CPreferRouteItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM PREFERROUTEINFO WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CDeicePadItem*>::iterator iter = m_vDeicePadList.begin();
	for (; iter != m_vDeicePadList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vDeicePadList.clear();
}
/////////////////////////////////////////////////////////////////////////////
//CDeicePadItem

CDeicePadItem::CDeicePadItem()
:m_nID(-1)
{

}

CDeicePadItem::~CDeicePadItem()
{
	std::vector<CRoutePath*>::iterator iter = m_vRoutePath.begin();
	for (; iter != m_vRoutePath.end(); ++iter)
	{
		delete *iter;
	}
	iter = m_vDelRoutePath.begin();
	for (; iter != m_vDelRoutePath.end(); ++iter)
	{
		delete *iter;
	}
	m_vRoutePath.clear();
	m_vDelRoutePath.clear();
}

void CDeicePadItem::AddItem(CRoutePath* pRoute)
{
	ASSERT(pRoute);
	m_vRoutePath.push_back(pRoute);
}

bool CDeicePadItem::DeleteItem(CRoutePath* pRoute)
{
	ASSERT(pRoute);
	m_vDelRoutePath.push_back(pRoute);
	std::vector<CRoutePath*>::iterator iter = m_vRoutePath.begin();
	for (; iter != m_vRoutePath.end(); ++iter)
	{
		if (pRoute->getFitPadID() == (*iter)->getFitPadID())
		{
			m_vRoutePath.erase(iter);
			return true;
		}
	}
	return false;
}

bool CDeicePadItem::DeleteItemData(int nFitID)
{
	std::vector<CRoutePath*>::iterator iter = m_vDelRoutePath.begin();
	for (; iter != m_vDelRoutePath.end(); ++iter)
	{
		if (nFitID == (*iter)->getFitPadID())
		{
			m_vDelRoutePath.push_back(*iter);
			m_vRoutePath.erase(iter);
			return true;
		}
	}
	return false;
}
bool CDeicePadItem::DeleteItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vRoutePath.size());
	m_vDelRoutePath.push_back(m_vRoutePath.at(nIndex));
	m_vRoutePath.erase(m_vRoutePath.begin()+nIndex);
	return true;
}

CRoutePath* CDeicePadItem::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vRoutePath.size());
	return m_vRoutePath.at(nIndex);
}

CRoutePath* CDeicePadItem::GetItemData(int nFitPadID)
{
	std::vector<CRoutePath*>::iterator iter = m_vDelRoutePath.begin();
	for (; iter != m_vDelRoutePath.end(); ++iter)
	{
		if (nFitPadID == (*iter)->getFitPadID())
		{
			return *iter;
		}
	}
	return NULL;
}
int CDeicePadItem::GetCount()
{
	return (int)m_vRoutePath.size();
}
void CDeicePadItem::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("NID"),m_nFitStandID);
	adoRecordset.GetFieldValue(_T("DEICEPADID"),m_nDeicePadID);
	
	ReadData();
}

void CDeicePadItem::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM ROUTEPATHINFO WHERE FITPADID = %d"),m_nID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CRoutePath* pRoute = new CRoutePath();
		pRoute->ReadData(adoRecordset);
		m_vRoutePath.push_back(pRoute);
		adoRecordset.MoveNextData();
	}
}
void CDeicePadItem::SaveData(int nID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	m_nFitStandID = nID;
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO DEICEPADINFO(NID,DEICEPADID)\
					 VALUES (%d,%d)"),m_nFitStandID,m_nDeicePadID);
    m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	std::vector<CRoutePath*>::iterator iter = m_vRoutePath.begin();
	for (; iter != m_vRoutePath.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}
	iter = m_vDelRoutePath.begin();
	for (; iter != m_vDelRoutePath.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vDelRoutePath.clear();
}

void CDeicePadItem::UpdateData()
{
	CString strSQL  = _T("");
	strSQL.Format(_T("UPDATE DEICEPADINFO SET NID = %d,DEICEPADID = %d\
					 WHERE (ID = %d)"),m_nFitStandID,m_nDeicePadID,m_nID);
	
	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CRoutePath*>::iterator iter = m_vRoutePath.begin();
	for (; iter != m_vRoutePath.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}
	iter = m_vDelRoutePath.begin();
	for (; iter != m_vDelRoutePath.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vDelRoutePath.clear();
}

void CDeicePadItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM DEICEPADINFO WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CRoutePath*>::iterator iter = m_vRoutePath.begin();
	for (; iter != m_vRoutePath.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vRoutePath.clear();
}

/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////CRoutePath
CRoutePath::CRoutePath()
:m_strRouteList(_T(""))
,m_nID(-1)
{

}

CRoutePath::~CRoutePath()
{

}

void CRoutePath::Additem(int RouteID)
{
	m_vRouteIDList.push_back(RouteID);
}

bool CRoutePath::DeleteItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vRouteIDList.size());
	m_vRouteIDList.erase(m_vRouteIDList.begin() + nIndex);
	return true;
}

void CRoutePath::UpdataItem(int nIndex,int RouteID)
{
	ASSERT(nIndex < (int)m_vRouteIDList.size());
	m_vRouteIDList[nIndex] = RouteID;
}
void CRoutePath::setRouteListString(std::vector<int>vRouteIDList)
{
	m_strRouteList.Empty();
	int nCount = (int)vRouteIDList.size();
	CString strTemp = _T("");
	for (int i = 0; i < nCount; i++)
	{
		strTemp.Format("%d",vRouteIDList.at(i));
		if (i == nCount-1)
		{
			m_strRouteList += strTemp;
		}
		else
		{
			m_strRouteList += strTemp + _T(",");
		}
	}
}

CString& CRoutePath::getRouteListString()
{
	return m_strRouteList;
}

void CRoutePath::setVRouteID(CString& strRouteList)
{
	int nFirst = -1;
	int nLast =strRouteList.Find(',');
	CString strTemp;
	if (nLast == -1)
	{
		m_vRouteIDList.push_back(atoi(strRouteList));
	}
	while (nLast != -1)
	{
		nLast = strRouteList.Find(',',nFirst + 1);
		if (nLast != -1)
		{
			strTemp = strRouteList.Mid(nFirst + 1,nLast);
			m_vRouteIDList.push_back(atoi(strRouteList.Mid(nFirst + 1,nLast)));
			nFirst = nLast;
		}
		else
		{
			strTemp = strRouteList.Right(strRouteList.GetLength() - nFirst - 1);
			m_vRouteIDList.push_back(atoi(strRouteList.Right(strRouteList.GetLength() - nFirst - 1)));
		}
	}
}
int CRoutePath::getRouteIDListCount()
{
	return (int)m_vRouteIDList.size();
}

void CRoutePath::SaveData(int nID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	m_nFitPadID = nID;
	setRouteListString(m_vRouteIDList);
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO ROUTEPATHINFO (FITPADID,VROUTEID) VALUES (%d,'%s')"),m_nFitPadID,m_strRouteList);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CRoutePath::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("FITPADID"),m_nFitPadID);
	adoRecordset.GetFieldValue(_T("VROUTEID"),m_strRouteList);
	setVRouteID(m_strRouteList);
}

void CRoutePath::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ROUTEPATHINFO WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CRoutePath::UpdateData()
{
	setRouteListString(m_vRouteIDList);
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE ROUTEPATHINFO SET FITPADID = %d,VROUTEID = '%s'\
					 WHERE (ID = %d)"),m_nFitPadID,m_strRouteList,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
