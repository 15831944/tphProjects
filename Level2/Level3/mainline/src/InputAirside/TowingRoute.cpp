#include "StdAfx.h"
#include "TowingRoute.h"
#include "AirsideImportExportManager.h"
#include "ALTObjectGroup.h"
#include "ALTObject.h"

//--------------------CTaxiwayNode--------------------


CTaxiwayNode::CTaxiwayNode()
:m_nID(-1)
,m_nUniqueID(-1)
,m_nParentID(-1)
,m_nTaxiwayID(-1)
,m_idx(-1)
{
}

CTaxiwayNode::~CTaxiwayNode()
{
}
void CTaxiwayNode::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);
	adoRecordset.GetFieldValue(_T("TAXIWAYID"),m_nTaxiwayID);
	adoRecordset.GetFieldValue(_T("UNIQUEID"),m_nUniqueID);
	adoRecordset.GetFieldValue(_T("IDX"),m_idx);

}
void CTaxiwayNode::DeleteData() 
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_TOWINGROUTE_TAXIWAYNODEDATA\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void CTaxiwayNode::SaveData(int nTowingRouteID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TOWINGROUTE_TAXIWAYNODEDATA\
					 (TOWINGROUTEID, PARENTID,TAXIWAYID, UNIQUEID, IDX)\
					 VALUES (%d,%d,%d,%d,%d)"),nTowingRouteID,m_nParentID,m_nTaxiwayID, m_nUniqueID, m_idx);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void CTaxiwayNode::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TOWINGROUTE_TAXIWAYNODEDATA\
					 SET PARENTID =%d, TAXIWAYID =%d, UNIQUEID =%d, IDX =%d\
					 WHERE (ID = %d)"),m_nParentID,m_nTaxiwayID, m_nUniqueID, m_idx, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void CTaxiwayNode::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nParentID);
	exportFile.getFile().writeInt(m_nTaxiwayID);
	exportFile.getFile().writeInt(m_nUniqueID);
	exportFile.getFile().writeInt(m_idx);

	exportFile.getFile().writeLine();
}

void CTaxiwayNode::ImportData(CAirsideImportFile& importFile,int nTowingRouteID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	importFile.getFile().getInteger(m_nParentID);

	int nOldTaixayID = -1;
	importFile.getFile().getInteger(nOldTaixayID);
	m_nTaxiwayID = importFile.GetObjectNewID(nOldTaixayID);

	importFile.getFile().getInteger(m_nUniqueID);
	importFile.getFile().getInteger(m_idx);

	SaveData(nTowingRouteID);

	importFile.getFile().getLine();
}

//--------------------CTowingRoute----------------------
CTowingRoute::CTowingRoute()
:m_nID(-1)
,m_nMaxUniID(0)
,m_strName(_T("NewRoute"))
{
	m_vTaxiwayNode.clear();
	m_vNeedDelTaxiwayNode.clear();
}

CTowingRoute::~CTowingRoute()
{
	RemoveAll();
}
void CTowingRoute::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("ROUTENAME"),m_strName);
	//adoRecordset.GetFieldValue(_T("FROMTYPE"),m_nFromType);
	//adoRecordset.GetFieldValue(_T("FROMID"),m_nFromID);
	//adoRecordset.GetFieldValue(_T("TOTYPE"),m_nToType);
	//adoRecordset.GetFieldValue(_T("TOID"),m_nToID);
	ReadTaxiwayNodeData();
	TaxiwayNodeIter iter = m_vTaxiwayNode.begin();
	for (; iter != m_vTaxiwayNode.end(); ++iter)
	{
		int nUniqueID = (*iter)->GetUniqueID();
		if(nUniqueID > m_nMaxUniID)
			m_nMaxUniID = nUniqueID;
	}
}
void CTowingRoute::ReadTaxiwayNodeData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_TOWINGROUTE_TAXIWAYNODEDATA\
					 WHERE (TOWINGROUTEID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CTaxiwayNode* pTaxiwayNode = new CTaxiwayNode;
		pTaxiwayNode->ReadData(adoRecordset);

		m_vTaxiwayNode.push_back(pTaxiwayNode);
		adoRecordset.MoveNextData();
	}
}
void CTowingRoute::SaveData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TOWINGROUTE (ROUTENAME)\
					 VALUES ('%s')"),m_strName);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	TaxiwayNodeIter iter = m_vTaxiwayNode.begin();
	for (; iter !=m_vTaxiwayNode.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}
	for (iter = m_vNeedDelTaxiwayNode.begin(); iter != m_vNeedDelTaxiwayNode.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelTaxiwayNode.clear();
}
void CTowingRoute::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TOWINGROUTE SET ROUTENAME ='%s'\
					 WHERE (ID = %d)"), m_strName, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	TaxiwayNodeIter iter = m_vTaxiwayNode.begin();
	for (;iter !=m_vTaxiwayNode.end();++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}
	for (iter = m_vNeedDelTaxiwayNode.begin(); iter !=m_vNeedDelTaxiwayNode.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelTaxiwayNode.clear();
}
int CTowingRoute::GetTaxiwayNodeCount()
{
	return (int)m_vTaxiwayNode.size();
}

CTaxiwayNode* CTowingRoute::GetTaxiwayNodeItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vTaxiwayNode.size());
	return m_vTaxiwayNode[nIndex];
}

CTaxiwayNode* CTowingRoute::GetTaxiwayNodeByParentID(int nParentID)
{
	TaxiwayNodeIter iter = m_vTaxiwayNode.begin();
	for (; iter !=m_vTaxiwayNode.end(); ++iter)
	{
		if((*iter)->GetParentID() == nParentID)
		{
			return *iter;
		}
	}

	return NULL;
}

void CTowingRoute::AddTaxiwayNodeItem(CTaxiwayNode* pItem)
{
	m_vTaxiwayNode.push_back(pItem);
}
void CTowingRoute::DelTaxiwayNodeItem(CTaxiwayNode* pItem)
{
	TaxiwayNodeIter iter = 
		std::find(m_vTaxiwayNode.begin(),m_vTaxiwayNode.end(),pItem);
	if (iter == m_vTaxiwayNode.end())
		return;
	m_vNeedDelTaxiwayNode.push_back(pItem);
	m_vTaxiwayNode.erase(iter);
}
void CTowingRoute::RemoveAll()
{
	TaxiwayNodeIter iter = m_vTaxiwayNode.begin();
	for (; iter != m_vTaxiwayNode.end(); iter++)
		delete *iter;
	for (iter = m_vNeedDelTaxiwayNode.begin(); iter != m_vNeedDelTaxiwayNode.end(); ++iter)
		delete *iter;
}
void CTowingRoute::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_TOWINGROUTE WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	TaxiwayNodeIter iter = m_vTaxiwayNode.begin();
	for (; iter !=m_vTaxiwayNode.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vTaxiwayNode.clear();
	for (iter = m_vNeedDelTaxiwayNode.begin(); iter != m_vNeedDelTaxiwayNode.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelTaxiwayNode.clear();
}
void CTowingRoute::DelFromvTaxiwayNode(CTaxiwayNode* pTaxiwayNode)
{
	DelChildren(pTaxiwayNode->GetUniqueID());
	TaxiwayNodeIter iter = std::find(m_vTaxiwayNode.begin(), m_vTaxiwayNode.end(), pTaxiwayNode);
	if(iter == m_vTaxiwayNode.end())
		return;
	m_vNeedDelTaxiwayNode.push_back(pTaxiwayNode);
	m_vTaxiwayNode.erase(iter);
}
void CTowingRoute::DelChildren(int nID)
{
	TaxiwayNodeIter iterchild = m_vTaxiwayNode.begin();
	for (; iterchild != m_vTaxiwayNode.end(); ++iterchild)
	{
		if((*iterchild)->GetParentID() == nID)
		{
			DelFromvTaxiwayNode((*iterchild));		
			iterchild = m_vTaxiwayNode.begin();
		}
	}
}
void CTowingRoute::UpdatevTaxiwayNode(CTaxiwayNode* pTaxiwayNode)
{
	DelChildren(pTaxiwayNode->GetUniqueID());
}

void CTowingRoute::ExportData(CAirsideExportFile& exportFile)
{

}

void CTowingRoute::ImportData(CAirsideImportFile& importFile)
{

}

std::vector<CTaxiwayNode*> CTowingRoute::GetFirstItems()
{
	std::vector<CTaxiwayNode *> vTaxiwayNode;
	TaxiwayNodeIter iter = m_vTaxiwayNode.begin();
	for (; iter !=m_vTaxiwayNode.end(); ++iter)
	{
		if((*iter)->GetParentID() == -1)
		{
			vTaxiwayNode.push_back(*iter);
		}
	}
	
	return vTaxiwayNode;
}

std::vector<CTaxiwayNode*> CTowingRoute::GetExitItems()
{
	std::vector<CTaxiwayNode *> vTaxiwayNode;
	TaxiwayNodeIter iterExit = m_vTaxiwayNode.begin();
	for (; iterExit !=m_vTaxiwayNode.end(); ++iterExit)
	{
		TaxiwayNodeIter iterNode = m_vTaxiwayNode.begin();
		if((*iterNode)->GetParentID() == (*iterExit)->GetID())
		{
			continue;
		}

		vTaxiwayNode.push_back(*iterExit);
	}

	return vTaxiwayNode;


}
//------------CTowingRouteList-----------------
CTowingRouteList::CTowingRouteList()
{
}

CTowingRouteList::~CTowingRouteList()
{
	RemoveAll();
}

void CTowingRouteList::ReadData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM IN_TOWINGROUTE"));

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CTowingRoute* pTowingRoute = new CTowingRoute;
		pTowingRoute->ReadData(adoRecordset);

		m_vTowingRoute.push_back(pTowingRoute);
		adoRecordset.MoveNextData();
	}
}

void CTowingRouteList::SaveData()
{
	TowingRouteIter iter  = m_vTowingRoute.begin();
	for (; iter != m_vTowingRoute.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData();
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelTowingRoute.begin(); iter != m_vNeedDelTowingRoute.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelTowingRoute.clear();
}

int CTowingRouteList::GetTowingRouteCount()
{
	return (int)m_vTowingRoute.size();
}

CTowingRoute* CTowingRouteList::GetTowingRouteItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vTowingRoute.size());
	return m_vTowingRoute[nIndex];
}

CTowingRoute* CTowingRouteList::GetTowingRouteByID(int nID)
{
	int nCount = (int)m_vTowingRoute.size();
	for (int i =0; i <nCount; i++)
	{
		CTowingRoute* pRoute = m_vTowingRoute.at(i);
		if (pRoute->GetID() == nID)
			return pRoute;
	}

	return NULL;
}

void CTowingRouteList::AddTowingRouteItem(CTowingRoute* pItem)
{
	m_vTowingRoute.push_back(pItem);
}
void CTowingRouteList::DelTowingRouteItem(CTowingRoute* pItem)
{
	TowingRouteIter iter = 
		std::find(m_vTowingRoute.begin(),m_vTowingRoute.end(),pItem);
	if (iter == m_vTowingRoute.end())
		return;
	m_vNeedDelTowingRoute.push_back(pItem);
	m_vTowingRoute.erase(iter);
}
void CTowingRouteList::RemoveAll()
{
	TowingRouteIter iter = m_vTowingRoute.begin();
	for (; iter != m_vTowingRoute.end(); ++iter)
		delete *iter;

	for (iter = m_vNeedDelTowingRoute.begin(); iter != m_vNeedDelTowingRoute.end(); ++iter)
		delete *iter;
}

void CTowingRouteList::ExportTowingRoutes(CAirsideExportFile& exportFile)
{
	/*
	CTowingRouteList towingRouteList;
	towingRouteList.ReadData(exportFile.GetProjectID());
	towingRouteList.ExportData(exportFile);
	*/	
	exportFile.getFile().endFile();
}
void CTowingRouteList::ExportData(CAirsideExportFile& exportFile)
{		
	exportFile.getFile().writeField("TowingRoute");
	exportFile.getFile().writeLine();
	TowingRouteIter iter = m_vTowingRoute.begin();
	for (; iter!=m_vTowingRoute.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}
void CTowingRouteList::ImportTowingRoutes(CAirsideImportFile& importFile)
{
	/*
	while(!importFile.getFile().isBlank())
	{
		CTowingRoute towingRoute;
		towingRoute.ImportData(importFile);
	}
	*/
}