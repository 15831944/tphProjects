#include "StdAfx.h"
#include "..\Database\DBElementCollection_Impl.h"
#include ".\taxiroute.h"
#include "AirsideImportExportManager.h"
#include "ALTObjectGroup.h"
#include "common\ALTObjectID.h"
#include "HoldShortLines.h"
#include "RunwayExit.h"

void RecaptureAndConditionData::SaveDataToDB()
{
	CString strSQL;

	//not exist
	if (m_nID < 0)
	{
		strSQL.Format(_T("INSERT INTO IN_OUTBOUNDROUTE_PRIORITY\
						 (OUTBOUNDROUTEID, ACONROUTECOUNT,ACONROUTETIME,PRECDING,RECAPTURE,PRIORITYINDEX,NAME) \
						 VALUES (%d,%d,%d,%d,%d,%d,'%s')"),\
						 m_nRouteID,m_nAcOnRouteCount,m_nAcOnRouteStopTime,m_nPrecdingAcSlowerPercent,m_bRecapture?1:0,m_nPriorityIndex,m_sName);

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		strSQL.Format(_T("UPDATE IN_OUTBOUNDROUTE_PRIORITY\
						 SET OUTBOUNDROUTEID = %d, ACONROUTECOUNT = %d, \
						 ACONROUTETIME = %d,PRECDING = %d,RECAPTURE = %d,PRIORITYINDEX = %d,NAME = '%s'\
						 WHERE (ID =%d)"),
						 m_nRouteID,
						 m_nAcOnRouteCount,
						 m_nAcOnRouteStopTime,
						 m_nPrecdingAcSlowerPercent,
						 m_bRecapture?1:0,
						 m_nPriorityIndex,
						 m_sName,
						 m_nID);

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void RecaptureAndConditionData::DeleteDataFromDB()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM IN_OUTBOUNDROUTE_PRIORITY\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void RecaptureAndConditionData::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("ACONROUTECOUNT"), m_nAcOnRouteCount);
	recordset.GetFieldValue(_T("ACONROUTETIME"), m_nAcOnRouteStopTime);
	recordset.GetFieldValue(_T("PRECDING"), m_nPrecdingAcSlowerPercent);
	int nRecapture = -1;
	recordset.GetFieldValue(_T("RECAPTURE"),nRecapture);
	m_bRecapture = nRecapture?true:false;
	recordset.GetFieldValue(_T("OUTBOUNDROUTEID"), m_nRouteID);
	recordset.GetFieldValue(_T("PRIORITYINDEX"),m_nPriorityIndex);
	recordset.GetFieldValue(_T("NAME"),m_sName);
}
//////////////////////////////////////////////////////////////////////////////////////////////
CRoutePriority::CRoutePriority()
:m_nRoutID(-1)
,m_bHasChangeCondition(true)
{
 
}
 
  
CRoutePriority::~CRoutePriority()
{
 
}
 
void CRoutePriority::DeleteItem(const CRouteItem* dbElement)
{
	std::vector<CRouteItem *>::iterator iter = m_dataList.begin();
	for (;iter != m_dataList.end(); ++iter)
	{		
		if ((*iter) == dbElement)
		{
		 	m_deleteddataList.push_back(*iter);
			m_dataList.erase(iter);
		 	break;
		}
	}
}

void CRoutePriority::SaveDataToDB()
{
	m_RepConData.SaveDataToDB();

	std::vector<CRouteItem *>::iterator iter = m_dataList.begin();
	for (;iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetParentID(-1);
		(*iter)->SetRouteID(m_RepConData.m_nRouteID);
		(*iter)->SetPrioirtyID(m_RepConData.m_nID);
		(*iter)->SaveDataToDB();
	}

	for (std::vector<CRouteItem *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}
	m_deleteddataList.clear();
}
 
void CRoutePriority::DeleteDataFromDB()
{
	std::vector<CRouteItem *>::iterator iter = m_dataList.begin();
	for (;iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}
	m_RepConData.DeleteDataFromDB();
}
 
void CRoutePriority::DeleteData()
{
	//delete content
	for (std::vector<CRouteItem*>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteData();
		m_deleteddataList.push_back(*iter);
	}

	m_dataList.clear();
}

void CRoutePriority::InitFromDBRecordset(CADORecordset& recordset)
{
	if (!recordset.IsEOF())
	{
		std::vector<CRouteItem *>::iterator iter = m_dataList.begin();
		for (;iter != m_dataList.end(); ++iter)
		{
			(*iter)->InitFromDBRecordset(recordset);
		}
	}
}

//check priority has change condition
bool CRoutePriority::IfHasChangeCondition()const
{
	return m_bHasChangeCondition;
}

//check the priority whether be head
bool CRoutePriority::IfHasRepcaputreCondition()const
{
	return (m_RepConData.m_nPriorityIndex == 0)? true: false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
CIn_OutBoundRoute::CIn_OutBoundRoute(void)
	: m_nRouteAssignmentID(-1)
	, m_eRouteChoice(RouteChoice_ShortestPath)
	, m_bAllRunway(false)
	, m_bAllStand(false)
	, m_bAllDeice(false)
	, m_bAllStartPosition(false)
	, m_bAllTaxiInterruptLine(false)
{
// 	m_vRunwayExitIDList.clear();
// 	m_vStandGroupList.clear();
}

CIn_OutBoundRoute::~CIn_OutBoundRoute(void)
{
}

BOUNDROUTETYPE CIn_OutBoundRoute::GetRouteType()const
{	
	return BOUND;
}

bool CIn_OutBoundRoute::operator ==(const CIn_OutBoundRoute& route)
{
	return (m_nID == route.m_nID);
}

void CIn_OutBoundRoute::SetRouteAssignmentID(int nRouteAssignmentID)
{
	m_nRouteAssignmentID = nRouteAssignmentID;
}

int CIn_OutBoundRoute::GetRouteAssignmentID()
{
	return m_nRouteAssignmentID;
}

void CIn_OutBoundRoute::SetAllRunway(BOOL bAllRunway)
{
	m_bAllRunway = bAllRunway;
	m_vRunwayExitIDList.clear();
}

BOOL CIn_OutBoundRoute::IsAllRunway()const
{
	return m_bAllRunway;
}

void CIn_OutBoundRoute::SetAllStand(BOOL bAllStand)
{
	m_bAllStand = bAllStand;
	m_vStandGroupList.clear();
}

BOOL CIn_OutBoundRoute::IsAllStand()const
{
	return m_bAllStand;
}
void CIn_OutBoundRoute::SetAllDeice(BOOL bAllDeice){
	m_bAllDeice = bAllDeice;
	m_vDeiceGroupList.clear();
}
BOOL CIn_OutBoundRoute::IsAllDeice()const
{
	return m_bAllDeice;
}

void CIn_OutBoundRoute::SetAllStartPosition(BOOL bAllStartPosition)
{
	m_bAllStartPosition = bAllStartPosition;
	m_vStartPositionGroupList.clear();
}

BOOL CIn_OutBoundRoute::IsAllStartPosition()const
{
	return m_bAllStartPosition;
}

void CIn_OutBoundRoute::SetAllTaxiInterruptLine(BOOL bAllTaxiInterruptLine)
{
	m_bAllTaxiInterruptLine = bAllTaxiInterruptLine;
	m_vTaxiInterruptLineList.clear();
}

BOOL CIn_OutBoundRoute::IsAllTaxiInterruptLine()const
{
	return m_bAllTaxiInterruptLine;
}

void CIn_OutBoundRoute::ExchangePriorityItem(CRoutePriority* pPriorityPre,CRoutePriority* pPriorityNext)
{
	int nPriorityIndex = pPriorityPre->m_RepConData.m_nPriorityIndex;
	pPriorityPre->m_RepConData.m_nPriorityIndex = pPriorityNext->m_RepConData.m_nPriorityIndex;
	pPriorityNext->m_RepConData.m_nPriorityIndex = nPriorityIndex;
}

CRoutePriority* CIn_OutBoundRoute::GetPriorityItem(int nPriorityIndex)
{
	for (int i = 0; i < (int)GetElementCount(); i++)
	{
		if (nPriorityIndex == GetItem(i)->m_RepConData.m_nPriorityIndex)
		{
			return GetItem(i);
		}
	}
	return NULL;
}

void CIn_OutBoundRoute::ResetPriorityIndex(CRoutePriority* pPriority)
{
	for (size_t i = 0; i < GetElementCount(); i++)
	{
		CRoutePriority* pItem = GetItem(i);
		if (pPriority->m_RepConData.m_nPriorityIndex < pItem->m_RepConData.m_nPriorityIndex)
		{
			pItem->m_RepConData.m_nPriorityIndex--;
		}
	}
}


void CIn_OutBoundRoute::SortAssignPriorityIndex()
{
	// sort the priorities and re-assign the index
	std::sort(m_dataList.begin(), m_dataList.end(), PrioritySorter());
	std::for_each(m_dataList.begin(), m_dataList.end(), PriorityIndexAssigner());
}

int CIn_OutBoundRoute::GetMinPriorityIndex()
{
	int nMin = INT_MAX;
	for (size_t i = 0; i < GetElementCount(); i++)
	{
		CRoutePriority* pItem = GetItem(i);
		if (pItem->m_RepConData.m_nPriorityIndex < nMin)
		{
			nMin = pItem->m_RepConData.m_nPriorityIndex;
		}
	}
	return nMin;
}

int CIn_OutBoundRoute::GetMaxPriorityIndex()
{
	int nMax = 0;
	for (size_t i = 0; i < GetElementCount(); i++)
	{
		CRoutePriority* pItem = GetItem(i);
		if (pItem->m_RepConData.m_nPriorityIndex > nMax)
		{
			nMax = pItem->m_RepConData.m_nPriorityIndex;
		}
	}
	return nMax;
}

void CIn_OutBoundRoute::AssignPriorityChangeCondition()
{
	int nMin = GetMinPriorityIndex();
	int nMax = GetMaxPriorityIndex();
	size_t nCount = GetElementCount();

	for (size_t i = 0; i < nCount; i++)
	{
		CRoutePriority* pItem = GetItem(i);
		if (pItem->m_RepConData.m_nPriorityIndex >= nMin &&  pItem->m_RepConData.m_nPriorityIndex < nMax)
		{
			pItem->m_bHasChangeCondition = true;
		}
		else
		{
			pItem->m_bHasChangeCondition = false;
		}
	}
}

void CIn_OutBoundRoute::SetStandGroupIDList(const ObjIDList& vStandGroupIDList)
{
	m_vStandGroupList.clear();
	m_vStandGroupList.assign(vStandGroupIDList.begin(), vStandGroupIDList.end());
}

void CIn_OutBoundRoute::GetStandGroupIDList(ObjIDList& vStandGroupIDList)const
{
	vStandGroupIDList.assign(m_vStandGroupList.begin(), m_vStandGroupList.end());
}

void CIn_OutBoundRoute::SetRunwayExitIDList(const ObjIDList& vRunwayExitIDList)
{
	m_vRunwayExitIDList.clear();
	m_vRunwayExitIDList.assign(vRunwayExitIDList.begin(), vRunwayExitIDList.end());
}

void CIn_OutBoundRoute::GetRunwayExitIDList(ObjIDList& vRunwayExitIDList)const
{
	vRunwayExitIDList.assign(m_vRunwayExitIDList.begin(), m_vRunwayExitIDList.end());
}
void CIn_OutBoundRoute::SetDeiceGroupIDList(const ObjIDList& vDeiceGroupIDList)
{
	m_vDeiceGroupList.clear();
	m_vDeiceGroupList.assign(vDeiceGroupIDList.begin(),vDeiceGroupIDList.end());
}
void CIn_OutBoundRoute::GetDeiceGroupIDList(ObjIDList& vDeiceGroupIDList)const
{
	vDeiceGroupIDList.assign(m_vDeiceGroupList.begin(),m_vDeiceGroupList.end());
}

void CIn_OutBoundRoute::SetStartPositionGroupIDList(const ObjIDList& vStartPositionGroupList)
{
	m_vStartPositionGroupList = vStartPositionGroupList;
}

void CIn_OutBoundRoute::GetStartPositionGroupIDList(std::vector<int>& vStartPositionGroupList)const
{
	vStartPositionGroupList = m_vStartPositionGroupList;
}

void CIn_OutBoundRoute::SetTaxiInterruptLineIDList(const ObjIDList& vTaxiInterruptLineList)
{
	m_vTaxiInterruptLineList = vTaxiInterruptLineList;
}

void CIn_OutBoundRoute::GetTaxiInterruptLineIDList(ObjIDList& vTaxiInterruptLineList)const
{
	vTaxiInterruptLineList = m_vTaxiInterruptLineList;
}

//DBElementCollection
void CIn_OutBoundRoute::SaveData(int nID)
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(strSQL);

		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	SaveOriginAndDestination();

	for (std::vector<CRoutePriority*>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
	
		(*iter)->m_nRoutID = m_nID;
		(*iter)->m_RepConData.m_nRouteID = m_nID;
		
		(*iter)->SaveDataToDB();
	}

	for (std::vector<CRoutePriority*>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CIn_OutBoundRoute::DeleteItem(const CRoutePriority* dbElement)
{
	std::vector<CRoutePriority *>::iterator iter = std::find(m_dataList.begin(),m_dataList.end(),dbElement);
	if (iter != m_dataList.end())
	{		
		m_deleteddataList.push_back(*iter);
		m_dataList.erase(iter);

	}
}


void CIn_OutBoundRoute::DeleteData()
{
	if (m_nID < 0)
		return;
	//delete content
	for (std::vector<CRoutePriority*>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteData();
		m_deleteddataList.push_back(*iter);
	}

	m_dataList.clear();
}

//DBElement
void CIn_OutBoundRoute::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("ROUTECHOICE"), (int&)m_eRouteChoice);
	recordset.GetFieldValue(_T("BEALLRUNWAY"), m_bAllRunway);
	recordset.GetFieldValue(_T("BEALLSTAND"), m_bAllStand);
	recordset.GetFieldValue(_T("BEALLDEICE"), m_bAllDeice);
	recordset.GetFieldValue(_T("BEALLSTARTPOSITION"), m_bAllStartPosition);
	recordset.GetFieldValue(_T("BEALLTAXIINTERRUPTLINE"), m_bAllTaxiInterruptLine);

	ReadOriginAndDestination();

	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT * \
						   FROM IN_OUTBOUNDROUTE_PRIORITY \
						   WHERE (OUTBOUNDROUTEID = %d)"),
						   m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	if (adoRecordset.IsEOF())
	{
		CString strSelectSQL;
		strSelectSQL.Format(_T("SELECT * \
							   FROM IN_OUTBOUNDROUTE_DATA \
							   WHERE (OUTBOUNDROUTEID = %d AND PARENTID = -1)"),
							   m_nID);

		long nRecordCount = -1;
		CADORecordset adoRecordset;

		CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

		CRoutePriority *pOutboundRouteItem = new CRoutePriority();

		while (!adoRecordset.IsEOF())
		{
			CRouteItem* pRouteItem = new CRouteItem();
			pRouteItem->InitFromDBRecordset(adoRecordset);
			adoRecordset.MoveNextData();
			pOutboundRouteItem->AddNewItem(pRouteItem);
		}
		pOutboundRouteItem->m_bHasChangeCondition = false;
		AddNewItem(pOutboundRouteItem);
	}
	else
	{
		while (!adoRecordset.IsEOF())
		{
			CRoutePriority *pOutboundRouteItem = new CRoutePriority();
			pOutboundRouteItem->m_RepConData.InitFromDBRecordset(adoRecordset);

			CADORecordset adoRecordset1;
			strSelectSQL.Format(_T("SELECT * \
								   FROM IN_OUTBOUNDROUTE_DATA \
								   WHERE (OUTBOUNDROUTEID = %d AND PRIORITYID = %d AND PARENTID = -1)"),
								   m_nID,pOutboundRouteItem->m_RepConData.m_nID);
			CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset1);
			while (!adoRecordset1.IsEOF())
			{
				CRouteItem* pRouteItem = new CRouteItem();
				pRouteItem->InitFromDBRecordset(adoRecordset1);
				adoRecordset1.MoveNextData();
				pOutboundRouteItem->AddNewItem(pRouteItem);
			}
			adoRecordset.MoveNextData();
			AddNewItem(pOutboundRouteItem);
		}
		AssignPriorityChangeCondition();
	}

	SortAssignPriorityIndex();
}

void CIn_OutBoundRoute::GetInsertSQL(CString& strSQL) const
{

}

void CIn_OutBoundRoute::GetUpdateSQL(CString& strSQL) const
{

}

void CIn_OutBoundRoute::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_OUTBOUNDROUTE\
					 WHERE (ID = %d)"),m_nID);
}

void CIn_OutBoundRoute::DeleteDataFromDB()
{
	for (std::vector<CRoutePriority *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();

	DeleteOriginAndDestination();

	CString strSQL;
	GetDeleteSQL(strSQL);
	if (strSQL.IsEmpty())
		return;

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CIn_OutBoundRoute::SaveStandGroupList()
{
	DeleteStandGroupList();

	CString strSQL;

	size_t nSize = m_vStandGroupList.size();
	for (size_t i =0; i < nSize; i++ )
	{
		strSQL.Format(_T("INSERT INTO IN_OUTBOUNDSTANDFAMILY\
						 (PARENTID, STANDGROUPID) \
						 VALUES (%d, %d)"),\
						 m_nID,
						 m_vStandGroupList.at(i));

		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}

void CIn_OutBoundRoute::SaveRunwayExitList()
{
	DeleteRunwayExitList();

	CString strSQL;

	size_t nSize = m_vRunwayExitIDList.size();
	ASSERT(m_bAllRunway || nSize);
	for (size_t i =0; i < nSize; i++ )
	{
		strSQL.Format(_T("INSERT INTO IN_OUTBOUNDRUNWAYEXIT\
						 (PARENTID, RUNWAYEXITID) \
						 VALUES (%d, %d)"),\
						 m_nID,
						 m_vRunwayExitIDList.at(i));

		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}
void CIn_OutBoundRoute::SaveDeiceGroupList() const
{
	DeleteDeiceGroupList();

	CString strSQL;

	size_t nSize = m_vDeiceGroupList.size();
	for (size_t i =0; i < nSize; i++ )
	{
		strSQL.Format(_T("INSERT INTO IN_OUTBOUND_DEICEFAMILY\
						 (PARENTID, DEICEGROUPID) \
						 VALUES (%d, %d)"),\
						 m_nID,
						 m_vDeiceGroupList.at(i));

		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}

void CIn_OutBoundRoute::SaveStartPositionGroupList()
{
	DeleteStartPositionGroupList();

	CString strSQL;

	size_t nSize = m_vStartPositionGroupList.size();
	for (size_t i =0; i < nSize; i++ )
	{
		strSQL.Format(_T("INSERT INTO IN_OUTBOUND_STARTPOSITION\
						 (PARENTID, STARTPOSITION_GROUPID) \
						 VALUES (%d, %d)"),\
						 m_nID,
						 m_vStartPositionGroupList.at(i));

		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}

void CIn_OutBoundRoute::SaveTaxiInterruptLineList()
{
	DeleteTaxiInterruptLineList();

	CString strSQL;

	size_t nSize = m_vTaxiInterruptLineList.size();
	for (size_t i =0; i < nSize; i++ )
	{
		strSQL.Format(_T("INSERT INTO IN_OUTBOUND_TAXIINTERRUPTLINE\
						 (PARENTID, TAXIINTERRUPTLINEID) \
						 VALUES (%d, %d)"),\
						 m_nID,
						 m_vTaxiInterruptLineList.at(i));

		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}


void CIn_OutBoundRoute::DeleteStandGroupList()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM IN_OUTBOUNDSTANDFAMILY\
					 WHERE (PARENTID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CIn_OutBoundRoute::DeleteRunwayExitList()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM IN_OUTBOUNDRUNWAYEXIT\
					 WHERE (PARENTID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CIn_OutBoundRoute::DeleteDeiceGroupList()const
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM IN_OUTBOUND_DEICEFAMILY\
					 WHERE (PARENTID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CIn_OutBoundRoute::DeleteStartPositionGroupList()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM IN_OUTBOUND_STARTPOSITION\
					 WHERE (PARENTID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CIn_OutBoundRoute::DeleteTaxiInterruptLineList()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM IN_OUTBOUND_TAXIINTERRUPTLINE\
					 WHERE (PARENTID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

bool CIn_OutBoundRoute::IsFitRoute(const ALTObjectID& standID, int nRwyExitID) const
{
	bool bStandFit = m_bAllStand ? true : IsALTGroupInList(standID, m_vStandGroupList);
	if (!bStandFit)
		return false;

	return  m_bAllRunway ? true : 
	m_vRunwayExitIDList.end() != std::find(m_vRunwayExitIDList.begin(), m_vRunwayExitIDList.end(), nRwyExitID);
}

void CIn_OutBoundRoute::ReadRunwayExitList()
{
	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT RUNWAYEXITID \
						   FROM IN_OUTBOUNDRUNWAYEXIT \
						   WHERE (PARENTID = %d)"),
						   m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nExitID;
		adoRecordset.GetFieldValue(_T("RUNWAYEXITID"),nExitID);
		m_vRunwayExitIDList.push_back(nExitID);
		adoRecordset.MoveNextData();
	}
}

void CIn_OutBoundRoute::ReadStandGroupList()
{
	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT STANDGROUPID \
						   FROM IN_OUTBOUNDSTANDFAMILY \
						   WHERE (PARENTID = %d)"),
						   m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nGroupID;
		adoRecordset.GetFieldValue(_T("STANDGROUPID"),nGroupID);
		m_vStandGroupList.push_back(nGroupID);
		adoRecordset.MoveNextData();
	}
}

void CIn_OutBoundRoute::ReadDeiceGroupList()
{
	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT DEICEGROUPID \
						   FROM IN_OUTBOUND_DEICEFAMILY \
						   WHERE (PARENTID = %d)"),
						   m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nGroupID;
		adoRecordset.GetFieldValue(_T("DEICEGROUPID"),nGroupID);
		m_vDeiceGroupList.push_back(nGroupID);
		adoRecordset.MoveNextData();
	}
}

void CIn_OutBoundRoute::ReadStartPositionList()
{
	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT STARTPOSITION_GROUPID \
						   FROM IN_OUTBOUND_STARTPOSITION \
						   WHERE (PARENTID = %d)"),
						   m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nGroupID;
		adoRecordset.GetFieldValue(_T("STARTPOSITION_GROUPID"),nGroupID);
		m_vStartPositionGroupList.push_back(nGroupID);
		adoRecordset.MoveNextData();
	}
}

void CIn_OutBoundRoute::ReadTaxiInterrupLineList()
{
	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT TAXIINTERRUPTLINEID \
						   FROM IN_OUTBOUND_TAXIINTERRUPTLINE \
						   WHERE (PARENTID = %d)"),
						   m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nGroupID;
		adoRecordset.GetFieldValue(_T("TAXIINTERRUPTLINEID"),nGroupID);
		m_vTaxiInterruptLineList.push_back(nGroupID);
		adoRecordset.MoveNextData();
	}
}

void CIn_OutBoundRoute::SaveOriginAndDestination()
{
	SaveRunwayExitList();
	SaveStandGroupList();
	SaveDeiceGroupList();
	SaveStartPositionGroupList();
	SaveTaxiInterruptLineList();
}

void CIn_OutBoundRoute::DeleteOriginAndDestination()
{
	DeleteStandGroupList();
	DeleteRunwayExitList();
	DeleteDeiceGroupList();
	DeleteStartPositionGroupList();
	DeleteTaxiInterruptLineList();
}

void CIn_OutBoundRoute::ReadOriginAndDestination()
{
	if (!m_bAllRunway)
	{
		ReadRunwayExitList();
	}
	if (!m_bAllStand)
	{
		ReadStandGroupList();
	}
	if(!m_bAllDeice)
	{
		ReadDeiceGroupList();
	}
	if(!m_bAllStartPosition)
	{
		ReadStartPositionList();
	}
	if(!m_bAllTaxiInterruptLine)
	{
		ReadTaxiInterrupLineList();
	}

}

BoundRouteChoice CIn_OutBoundRoute::GetBoundRouteChoice() const
{
	return m_eRouteChoice;
}

void CIn_OutBoundRoute::SetBoundRouteChoice( BoundRouteChoice eRouteChoice )
{
	m_eRouteChoice = eRouteChoice;
}

bool CIn_OutBoundRoute::IsALTGroupInList( const ALTObjectID& altID, const ObjIDList& vIDList )
{
	size_t nSize = vIDList.size();
	for (size_t i =0; i < nSize; i++)
	{
		ALTObjectGroup standgroup;
		standgroup.ReadData(vIDList[i]);

		//standName is in standGroupName
		if (altID.idFits(standgroup.getName()))
			return true;
	}
	return false;
}
CString CIn_OutBoundRoute::GetDestinationString() const
{
	CString strRunwayExitName = "";
	if (IsAllRunway())
	{
		strRunwayExitName = "All";
	}
	else
	{
		size_t nSize = m_vRunwayExitIDList.size();
		bool bFirst = true;
		for (size_t i =0; i < nSize; i++)
		{
			RunwayExit rwyExit;
			rwyExit.ReadData(m_vRunwayExitIDList.at(i));
			if (!bFirst)
			{
				strRunwayExitName += ", ";
			}

			strRunwayExitName += rwyExit.GetName();
			bFirst = false;
		}
	}
	return strRunwayExitName;
}

//////////////////////////////////////CIn_BoundRoute//////////////////////////
CIn_BoundRoute::CIn_BoundRoute()
:CIn_OutBoundRoute()
{

}

CIn_BoundRoute::~CIn_BoundRoute()
{

}

//DBElement

void CIn_BoundRoute::GetInsertSQL(CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_OUTBOUNDROUTE\
					 (OUTBOUNDROUTEASSIGNMENTID, ROUTECHOICE, BEALLRUNWAY, BEALLSTAND, ROUTETYPE) \
					 VALUES (%d, %d, %d, %d, %d)"),\
					 m_nRouteAssignmentID,
					 m_eRouteChoice,
					 m_bAllRunway?1:0,
					 m_bAllStand?1:0,					
					 (int)INBOUND);
}

void CIn_BoundRoute::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_OUTBOUNDROUTE\
					 SET OUTBOUNDROUTEASSIGNMENTID = %d, ROUTECHOICE = %d,  BEALLRUNWAY = %d, BEALLSTAND = %d \
					 WHERE (ID =%d)"),
					 m_nRouteAssignmentID,
					 m_eRouteChoice,
					 m_bAllRunway?1:0,
					 m_bAllStand?1:0,
					 m_nID);
}

//////////////////////////////////////COut_BoundRoute//////////////////////////
COut_BoundRoute::COut_BoundRoute()
:CIn_OutBoundRoute()
{

}

COut_BoundRoute::~COut_BoundRoute()
{

}

//DBElement
void COut_BoundRoute::GetInsertSQL(CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_OUTBOUNDROUTE\
					 (OUTBOUNDROUTEASSIGNMENTID,ROUTECHOICE,BEALLRUNWAY,BEALLSTAND,BEALLDEICE,BEALLSTARTPOSITION,BEALLTAXIINTERRUPTLINE,ROUTETYPE) \
					 VALUES (%d, %d, %d, %d, %d, %d, %d, %d)"),\
					 m_nRouteAssignmentID,
					 m_eRouteChoice,
					 m_bAllRunway?1:0,
					 m_bAllStand?1:0,
					 m_bAllDeice?1:0,
					 m_bAllStartPosition?1:0,
					 m_bAllTaxiInterruptLine?1:0,
					 (int)OUTBOUND);
}

void COut_BoundRoute::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_OUTBOUNDROUTE\
					 SET OUTBOUNDROUTEASSIGNMENTID = %d, ROUTECHOICE = %d,\
					 BEALLRUNWAY = %d, BEALLSTAND = %d, BEALLDEICE = %d, BEALLSTARTPOSITION = %d, BEALLTAXIINTERRUPTLINE = %d \
					 WHERE (ID =%d)"),
					 m_nRouteAssignmentID,
					 m_eRouteChoice,
					 m_bAllRunway?1:0,
					 m_bAllStand?1:0,
					 m_bAllDeice?1:0,
					 m_bAllStartPosition?1:0,
					 m_bAllTaxiInterruptLine?1:0,
					 m_nID);
}

bool COut_BoundRoute::IsFitRouteStand( const ALTObjectID& standID, int nRwyExitID ) const
{
	return CIn_OutBoundRoute::IsFitRoute(standID, nRwyExitID);
}

bool COut_BoundRoute::IsFitRouteDeicepad( const ALTObjectID& deicepadID, int nRwyExitID ) const
{
	bool bDeiceFit = m_bAllDeice ? true : IsALTGroupInList(deicepadID, m_vDeiceGroupList);
	if (!bDeiceFit)
		return false;

	return m_bAllRunway ? true : 
	m_vRunwayExitIDList.end() != std::find(m_vRunwayExitIDList.begin(), m_vRunwayExitIDList.end(), nRwyExitID);
}

bool COut_BoundRoute::IsFitRouteStartPosition( const ALTObjectID& startPosID, int nRwyExitID ) const
{
	bool bStartPosFit = m_bAllStartPosition ? true : IsALTGroupInList(startPosID, m_vStartPositionGroupList);
	if (!bStartPosFit)
		return false;

	return m_bAllRunway ? true : 
	m_vRunwayExitIDList.end() != std::find(m_vRunwayExitIDList.begin(), m_vRunwayExitIDList.end(), nRwyExitID);
}

bool COut_BoundRoute::IsFitRouteTaxiInterruptLine( int nTaxiInterruptLineID, int nRwyExitID ) const
{
	bool bTaxiInterruptLineFit = m_bAllStand ? true : 
	m_vTaxiInterruptLineList.end() != std::find(m_vTaxiInterruptLineList.begin(), m_vTaxiInterruptLineList.end(), nTaxiInterruptLineID);
	if (!bTaxiInterruptLineFit)
		return false;

	return m_bAllRunway ? true : 
	m_vRunwayExitIDList.end() != std::find(m_vRunwayExitIDList.begin(), m_vRunwayExitIDList.end(), nRwyExitID);
}

BOOL COut_BoundRoute::RecALTObject( CString& strName, BOOL bHasPrev, BOOL bAll, const ObjIDList& recALTObj, ALTOBJECT_TYPE objType, LPCTSTR orgTypeTag )
{
	if(!bAll)
	{
		BOOL bFirst = TRUE;
		for (ObjIDList::const_iterator ite = recALTObj.begin();ite!=recALTObj.end();ite++)
		{
			if (bFirst)
			{
				if (bHasPrev)
					strName += ";";

				strName += orgTypeTag;
				bHasPrev = TRUE;
				bFirst = FALSE;
			}
			else
				strName += ", ";

			ALTObjectGroup altObjGroup;
			altObjGroup.ReadData(*ite);
			strName += altObjGroup.getName().GetIDString();
		}
	}
	else
	{
		if (bHasPrev)
			strName += _T(";");

		strName += orgTypeTag;
		strName += _T("All");
		bHasPrev = TRUE;
	}
	return bHasPrev;
}

CString COut_BoundRoute::GetOriginString() const
{
	CString strOriginName;
	BOOL bHasPrev = FALSE;
	bHasPrev = RecALTObject(strOriginName, bHasPrev, m_bAllStand, m_vStandGroupList, ALT_STAND, _T(" Stand: "));
	bHasPrev = RecALTObject(strOriginName, bHasPrev, m_bAllDeice, m_vDeiceGroupList, ALT_DEICEBAY, _T(" Deicepad: "));
	bHasPrev = RecALTObject(strOriginName, bHasPrev, m_bAllStartPosition, m_vStartPositionGroupList, ALT_STARTPOSITION, _T(" Start Position: "));

	if(!m_bAllTaxiInterruptLine)
	{
		BOOL bFirst = TRUE;
		for (ObjIDList::const_iterator ite = m_vTaxiInterruptLineList.begin();ite!=m_vTaxiInterruptLineList.end();ite++)
		{
			if (bFirst)
			{
				if (bHasPrev)
					strOriginName += ";";

				strOriginName += _T(" Taxi Interrupt Line: ");
				bHasPrev = TRUE;
				bFirst = FALSE;
			}
			else
				strOriginName += ", ";

			CTaxiInterruptLine taxiInterruptLine;
			taxiInterruptLine.ReadData(*ite);
			strOriginName += taxiInterruptLine.GetName();
		}
	}
	else
	{
		if (bHasPrev)
			strOriginName += _T(";");

		strOriginName += _T(" Taxi Interrupt Line: All");
		bHasPrev = TRUE;
	}
	return strOriginName;
}

//////////////////////////////////////////////////////////////////////////
void CDeice_BoundRoute::GetInsertSQL( CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO IN_OUTBOUNDROUTE\
					 (OUTBOUNDROUTEASSIGNMENTID, BEALLDEICE, BEALLSTAND,ROUTETYPE) \
					 VALUES (%d, %d, %d, %d)"),\
					 m_nRouteAssignmentID,
					 m_bAllDeice?1:0,
					 m_bAllStand?1:0,
					 (int)GetRouteType());
}

void CDeice_BoundRoute::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE IN_OUTBOUNDROUTE\
					 SET OUTBOUNDROUTEASSIGNMENTID = %d, BEALLDEICE = %d, BEALLSTAND = %d \
					 WHERE (ID =%d)"),
					 m_nRouteAssignmentID,
					 m_bAllDeice?1:0,
					 m_bAllStand?1:0,
					 m_nID);
}

bool CDeice_BoundRoute::IsFitRoute( const ALTObjectID& standID,const ALTObjectID& deiceID ) const
{
	bool bStandFit = false;
	bool bDeiceFit = false;

	if (m_bAllStand )
		bStandFit = true;
	else
	{
		size_t nSize = m_vStandGroupList.size();
		for (size_t i =0; i < nSize; i++)
		{
			ALTObjectGroup standgroup;
			standgroup.ReadData(m_vStandGroupList[i]);

			//standName is in standGroupName
			if (standID.idFits(standgroup.getName()))
			{
				bStandFit = true;
				break;
			}
		}

	}

	if (m_bAllDeice)
		bDeiceFit = true;
	else
	{
		size_t nSize = m_vDeiceGroupList.size();
		for (size_t i =0; i < nSize; i++)
		{

			ALTObjectGroup deicegroup;
			deicegroup.ReadData( m_vDeiceGroupList[i]);

			if (deiceID.idFits(deicegroup.getName()))
			{
				bDeiceFit = true;
				break;
			}
		}
	}

	return bDeiceFit&&bStandFit ;
}