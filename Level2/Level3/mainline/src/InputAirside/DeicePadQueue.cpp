#include "stdafx.h"
#include "InputAirside.h"
#include "../Database/ADODatabase.h"
#include "DeicePadQueue.h"


//////////////////////////////////////////////////////////////////////////////
//////////////DeicePadList///////////////////////////////////////////////////
DeicePadQueueList::DeicePadQueueList()
:m_vDeiceQueueItem(NULL)
,m_vDelDeiceQueueItem(NULL)
{

}

DeicePadQueueList::~DeicePadQueueList()
{
	std::vector<DeicePadQueueItem*>::iterator iter = m_vDeiceQueueItem.begin();
	for (; iter != m_vDeiceQueueItem.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vDelDeiceQueueItem.begin();
	for (; iter != m_vDelDeiceQueueItem.end(); ++iter)
	{
		delete *iter;
	}
	m_vDeiceQueueItem.clear();
	m_vDelDeiceQueueItem.clear();
}

void DeicePadQueueList::AddItem(DeicePadQueueItem* pPadQueueItem)
{
	ASSERT(pPadQueueItem);
	m_vDeiceQueueItem.push_back(pPadQueueItem);
}

bool DeicePadQueueList::DeleteItem(DeicePadQueueItem* pPadQueueItem)
{
	ASSERT(pPadQueueItem);
	m_vDelDeiceQueueItem.push_back(pPadQueueItem);
	std::vector<DeicePadQueueItem*>::iterator iter = m_vDeiceQueueItem.begin();
	for (; iter != m_vDeiceQueueItem.end(); ++iter)
	{
		if (pPadQueueItem->getDeicePadID() == (*iter)->getDeicePadID())
		{
			m_vDeiceQueueItem.erase(iter);
			return true;
		}
	}
	return false;
}

bool DeicePadQueueList::DeleteItemData(int nDeicePadID)
{
	std::vector<DeicePadQueueItem*>::iterator iter = m_vDeiceQueueItem.begin();
	for (; iter != m_vDeiceQueueItem.end(); ++iter)
	{
		if (nDeicePadID == (*iter)->getDeicePadID())
		{
			m_vDelDeiceQueueItem.push_back(*iter);
			m_vDeiceQueueItem.erase(iter);
			return true;
		}
	}
	return false;
}

bool DeicePadQueueList::DeleteItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vDeiceQueueItem.size());
	m_vDelDeiceQueueItem.push_back(m_vDeiceQueueItem[nIndex]);
	m_vDeiceQueueItem.erase(m_vDeiceQueueItem.begin() + nIndex);
	return true;
}

DeicePadQueueItem* DeicePadQueueList::GetItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vDeiceQueueItem.size());
	return m_vDeiceQueueItem[nIndex];
}

void DeicePadQueueList::ReadData(int nProjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM DEICEPADLISTINFO WHERE PROJID = %d"),nProjID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		DeicePadQueueItem* pDataItem = new DeicePadQueueItem();
		pDataItem->ReadData(adoRecordset);
		m_vDeiceQueueItem.push_back(pDataItem);
		adoRecordset.MoveNextData();
	}
}

void DeicePadQueueList::SaveData(int nProjID)
{
	std::vector<DeicePadQueueItem*>::iterator iter = m_vDeiceQueueItem.begin();
	for (; iter != m_vDeiceQueueItem.end(); ++iter)
	{
		(*iter)->SaveData(nProjID);
	}
	iter = m_vDelDeiceQueueItem.begin();
	for (; iter != m_vDelDeiceQueueItem.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vDelDeiceQueueItem.clear();
}
//////////////////////////////////////////////////////////////////////////////
//////////////DeicePadItem////////////////////////////////////////////////////

DeicePadQueueItem::DeicePadQueueItem()
:m_vDeiceQueueRoute(NULL)
,m_vDelDeiceQueueroute(NULL)
,m_nID(-1)
{

}

DeicePadQueueItem::~DeicePadQueueItem()
{
	std::vector<DeicePadQueueRoute*>::iterator iter = m_vDeiceQueueRoute.begin();
	for (; iter != m_vDeiceQueueRoute.end(); ++iter)
	{
		delete *iter;
	}
	iter = m_vDelDeiceQueueroute.begin();
	for (; iter != m_vDelDeiceQueueroute.begin(); ++iter)
	{
		delete *iter;
	}
	m_vDeiceQueueRoute.clear();
	m_vDelDeiceQueueroute.clear();
}

void DeicePadQueueItem::AddItem(DeicePadQueueRoute* pPadQueueItem)
{
	ASSERT(pPadQueueItem);
	m_vDeiceQueueRoute.push_back(pPadQueueItem);
}

bool DeicePadQueueItem::DeleteItem(DeicePadQueueRoute* pPadQueueItem)
{
	ASSERT(pPadQueueItem);
	m_vDelDeiceQueueroute.push_back(pPadQueueItem);
	std::vector<DeicePadQueueRoute*>::iterator iter = m_vDeiceQueueRoute.begin();
	for (; iter != m_vDeiceQueueRoute.end(); ++iter)
	{
		if (pPadQueueItem->getFitPadID() == (*iter)->getFitPadID())
		{
			m_vDeiceQueueRoute.erase(iter);
			return true;
		}
	}
	return false;
}

DeicePadQueueRoute* DeicePadQueueItem::GetItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vDeiceQueueRoute.size());
	return m_vDeiceQueueRoute[nIndex];
}
bool DeicePadQueueItem::DeleteItemData(int nFitID)
{
	std::vector<DeicePadQueueRoute*>::iterator iter = m_vDeiceQueueRoute.begin();
	for (; iter != m_vDeiceQueueRoute.end(); ++iter)
	{
		if (nFitID == (*iter)->getFitPadID())
		{
			m_vDeiceQueueRoute.erase(iter);
			return true;
		}
	}
	return false;
}

bool DeicePadQueueItem::DeleteItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vDeiceQueueRoute.size());
	m_vDelDeiceQueueroute.push_back(m_vDeiceQueueRoute.at(nIndex));
	m_vDeiceQueueRoute.erase(m_vDeiceQueueRoute.begin()+nIndex);
	return true;
}

void DeicePadQueueItem::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("DEICEPADID"),m_nDeicePadID);

	ReadData();
}

void DeicePadQueueItem::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM DEICEPADPATH WHERE PAD_ID = %d"),m_nID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		DeicePadQueueRoute* pPath = new DeicePadQueueRoute();
		pPath->ReadData(adoRecordset);
		m_vDeiceQueueRoute.push_back(pPath);
		adoRecordset.MoveNextData();
	}
}

void DeicePadQueueItem::SaveData(int nProjID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO DEICEPADLISTINFO(DEICEPADID,PROJID)\
					 VALUES (%d,%d)"),m_nDeicePadID,nProjID);
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	std::vector<DeicePadQueueRoute*>::iterator iter = m_vDeiceQueueRoute.begin();
	for (; iter != m_vDeiceQueueRoute.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}
	iter = m_vDelDeiceQueueroute.begin();
	for (; iter != m_vDelDeiceQueueroute.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vDelDeiceQueueroute.clear();
}

void DeicePadQueueItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM DEICEPADLISTINFO WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<DeicePadQueueRoute*>::iterator iter = m_vDeiceQueueRoute.begin();
	for (; iter != m_vDeiceQueueRoute.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vDeiceQueueRoute.clear();
}

void DeicePadQueueItem::UpdateData()
{
	CString strSQL  = _T("");
	strSQL.Format(_T("UPDATE DEICEPADLISTINFO SET DEICEPADID = %d\
					 WHERE (ID = %d)"),m_nDeicePadID,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<DeicePadQueueRoute*>::iterator iter = m_vDeiceQueueRoute.begin();
	for (; iter != m_vDeiceQueueRoute.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}
	iter = m_vDelDeiceQueueroute.begin();
	for (; iter != m_vDelDeiceQueueroute.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vDelDeiceQueueroute.clear();
}
///////////////////////////////////////////////////////////////////////////////
/////////////////////DeicePadRoute/////////////////////////////////////////////

DeicePadQueueRoute::DeicePadQueueRoute()
:m_nID(-1)
{

}

DeicePadQueueRoute::~DeicePadQueueRoute()
{

}

void DeicePadQueueRoute::Additem(int nDeiceRouteID)
{
	m_vDeiceRouteIDList.push_back(nDeiceRouteID);
}

bool DeicePadQueueRoute::DeleteItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vDeiceRouteIDList.size());
	m_vDeiceRouteIDList.erase(m_vDeiceRouteIDList.begin() + nIndex);
	return true;
}
int DeicePadQueueRoute::getRouteID(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vDeiceRouteIDList.size());
	return m_vDeiceRouteIDList[nIndex];
}
void DeicePadQueueRoute::UpdataItem(int nIndex,int RouteID)
{	
	ASSERT(nIndex < (int)m_vDeiceRouteIDList.size());
	m_vDeiceRouteIDList[nIndex] = RouteID;
}
int DeicePadQueueRoute::getDeiceRouteIDListCount()
{
	return (int)m_vDeiceRouteIDList.size();
}

void DeicePadQueueRoute::setVDeiceRouteID(CString& strDeiceRouteList)
{
	int nFirst = -1;
	int nLast =strDeiceRouteList.Find(',');
	CString strTemp;
	if (nLast == -1)
	{
		m_vDeiceRouteIDList.push_back(atoi(strDeiceRouteList));
	}
	while (nLast != -1)
	{
		nLast = strDeiceRouteList.Find(',',nFirst + 1);
		if (nLast != -1)
		{
			strTemp = strDeiceRouteList.Mid(nFirst + 1,nLast);
			m_vDeiceRouteIDList.push_back(atoi(strDeiceRouteList.Mid(nFirst + 1,nLast)));
			nFirst = nLast;
		}
		else
		{
			strTemp = strDeiceRouteList.Right(strDeiceRouteList.GetLength() - nFirst - 1);
			m_vDeiceRouteIDList.push_back(atoi(strDeiceRouteList.Right(strDeiceRouteList.GetLength() - nFirst - 1)));
		}
	}
}

void DeicePadQueueRoute::setDeiceRouteListString(std::vector<int>vDeiceRouteIDList)
{
	m_strDeiceRouteList.Empty();
	int nCount = (int)vDeiceRouteIDList.size();
	CString strTemp = _T("");
	for (int i = 0; i < nCount; i++)
	{
		strTemp.Format("%d",vDeiceRouteIDList.at(i));
		if (i == nCount-1)
		{
			m_strDeiceRouteList += strTemp;
		}
		else
		{
			m_strDeiceRouteList += strTemp + _T(",");
		}
	}
}

CString& DeicePadQueueRoute::getDeiceRouteListString()
{
	return m_strDeiceRouteList;
}

void DeicePadQueueRoute::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PAD_ID"),m_nFitPadID);
	adoRecordset.GetFieldValue(_T("VPATHID"),m_strDeiceRouteList);
	setVDeiceRouteID(m_strDeiceRouteList);
}

void DeicePadQueueRoute::SaveData(int nID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	m_nFitPadID = nID;
	setDeiceRouteListString(m_vDeiceRouteIDList);
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO DEICEPADPATH (PAD_ID,VPATHID) VALUES (%d,'%s')"),m_nFitPadID,m_strDeiceRouteList);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void DeicePadQueueRoute::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM DEICEPADPATH WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void DeicePadQueueRoute::UpdateData()
{
	setDeiceRouteListString(m_vDeiceRouteIDList);
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE DEICEPADPATH SET PAD_ID = %d,VPATHID = '%s'\
					 WHERE (ID = %d)"),m_nFitPadID,m_strDeiceRouteList,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}