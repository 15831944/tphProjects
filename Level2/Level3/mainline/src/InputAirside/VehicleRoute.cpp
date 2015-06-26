#include "StdAfx.h"
#include "VehicleRoute.h"
#include "AirsideImportExportManager.h"


//--------------------CStretchNode--------------------


CStretchNode::CStretchNode()
{
	m_nID = -1;
	m_nUniqueID = -1;
}

CStretchNode::~CStretchNode()
{
}
void CStretchNode::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);
	adoRecordset.GetFieldValue(_T("STRETCHID"),m_nStretchID);
	adoRecordset.GetFieldValue(_T("UNIQUEID"),m_nUniqueID);
}
void CStretchNode::DeleteData() 
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_VEHICLEROUTE_STRETCHNODEDATA\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void CStretchNode::SaveData(int nVehicleRouteID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_VEHICLEROUTE_STRETCHNODEDATA\
					 (VEHICLEROUTEID, PARENTID,STRETCHID, UNIQUEID)\
					 VALUES (%d,%d,%d,%d)"),nVehicleRouteID,m_nParentID,m_nStretchID, m_nUniqueID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void CStretchNode::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_VEHICLEROUTE_STRETCHNODEDATA\
					 SET PARENTID =%d, STRETCHID =%d, UNIQUEID =%d\
					 WHERE (ID = %d)"),m_nParentID,m_nStretchID, m_nUniqueID,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void CStretchNode::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nParentID);
	exportFile.getFile().writeInt(m_nStretchID);
	exportFile.getFile().writeInt(m_nUniqueID);

	exportFile.getFile().writeLine();
}

void CStretchNode::ImportData(CAirsideImportFile& importFile,int nVehicleRouteID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	importFile.getFile().getInteger(m_nParentID);

	int nOldStretchID = -1;
	importFile.getFile().getInteger(nOldStretchID);
	m_nStretchID = importFile.GetObjectNewID(nOldStretchID);

	importFile.getFile().getInteger(m_nUniqueID);

	SaveData(nVehicleRouteID);

	importFile.getFile().getLine();
}

//--------------------CVehicleRoute----------------------
CVehicleRoute::CVehicleRoute()
{
	m_nID = -1;	
}

CVehicleRoute::~CVehicleRoute()
{
	RemoveAll();
}
void CVehicleRoute::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("ROUTENAME"),m_strRouteName);

	ReadStretchNodeData();

	
}
void CVehicleRoute::ReadStretchNodeData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_VEHICLEROUTE_STRETCHNODEDATA\
					 WHERE (VEHICLEROUTEID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CStretchNode* pStretchNode = new CStretchNode;
		pStretchNode->ReadData(adoRecordset);

		m_vStretchNode.push_back(pStretchNode);
		adoRecordset.MoveNextData();
	}
}
void CVehicleRoute::SaveData(int nVehicleRouteListID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_VEHICLEROUTE\
					 (PROJID, ROUTENAME)\
					 VALUES (%d,'%s')"),nVehicleRouteListID,m_strRouteName);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	StretchNodeIter iter = m_vStretchNode.begin();
	for (; iter !=m_vStretchNode.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}
	for (iter = m_vNeedDelStretchNode.begin(); iter != m_vNeedDelStretchNode.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelStretchNode.clear();
}
void CVehicleRoute::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_VEHICLEROUTE\
					 SET ROUTENAME ='%s'\
					 WHERE (ID =%d)"),m_strRouteName,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	StretchNodeIter iter = m_vStretchNode.begin();
	for (;iter !=m_vStretchNode.end();++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}
	for (iter = m_vNeedDelStretchNode.begin(); iter !=m_vNeedDelStretchNode.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelStretchNode.clear();
}
int CVehicleRoute::GetStretchNodeCount()
{
	return (int)m_vStretchNode.size();
}

CStretchNode* CVehicleRoute::GetStretchNodeItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vStretchNode.size());
	return m_vStretchNode[nIndex];
}

void CVehicleRoute::AddStretchNodeItem(CStretchNode* pItem)
{
	m_vStretchNode.push_back(pItem);
}
void CVehicleRoute::DelStretchNodeItem(CStretchNode* pItem)
{
	StretchNodeIter iter = 
		std::find(m_vStretchNode.begin(),m_vStretchNode.end(),pItem);
	if (iter == m_vStretchNode.end())
		return;
	m_vNeedDelStretchNode.push_back(pItem);
	m_vStretchNode.erase(iter);
}
void CVehicleRoute::RemoveAll()
{
	StretchNodeIter iter = m_vStretchNode.begin();
	for (; iter != m_vStretchNode.end(); iter++)
		delete *iter;
	for (iter = m_vNeedDelStretchNode.begin(); iter != m_vNeedDelStretchNode.end(); ++iter)
		delete *iter;
}
void CVehicleRoute::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_VEHICLEROUTE\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	StretchNodeIter iter = m_vStretchNode.begin();
	for (; iter != m_vStretchNode.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vStretchNode.clear();
	for (iter = m_vNeedDelStretchNode.begin(); iter != m_vNeedDelStretchNode.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelStretchNode.clear();
}
void CVehicleRoute::DelFromvStretchNode(CStretchNode* pStretchNode)
{
	DelChildren(pStretchNode->GetUniqueID());
	StretchNodeIter iter = std::find(m_vStretchNode.begin(), m_vStretchNode.end(), pStretchNode);
	if(iter == m_vStretchNode.end())
		return;
	m_vNeedDelStretchNode.push_back(pStretchNode);
	m_vStretchNode.erase(iter);
}
void CVehicleRoute::DelChildren(int nID)
{
	StretchNodeIter iterchild = m_vStretchNode.begin();
	for (; iterchild != m_vStretchNode.end(); ++iterchild)
	{
		if((*iterchild)->GetParentID() == nID)
		{
			DelFromvStretchNode((*iterchild));		
			iterchild = m_vStretchNode.begin();
		}
	}
}
void CVehicleRoute::UpdatevStretchNode(CStretchNode* pStretchNode)
{
	DelChildren(pStretchNode->GetUniqueID());
}

void CVehicleRoute::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeField(m_strRouteName);

	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vStretchNode.size()));
	StretchNodeIter iter = m_vStretchNode.begin();
	for (; iter != m_vStretchNode.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}

void CVehicleRoute::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	importFile.getFile().getField(m_strRouteName.GetBuffer(1024),1024);

	SaveData(importFile.getNewProjectID());

	importFile.getFile().getLine();

	int nStretchNodeCount = 0;
	importFile.getFile().getInteger(nStretchNodeCount);
	for (int i = 0; i < nStretchNodeCount; ++i)
	{
		CStretchNode data;
		data.ImportData(importFile,m_nID);
	}
}

int CVehicleRoute::GetMaxUniID()const
{
	int nMaxUniID = -1;
	StretchNodeConstIter iter = m_vStretchNode.begin();
	for (; iter != m_vStretchNode.end(); ++iter)
	{
		int nUniqueID = (*iter)->GetUniqueID();
		if(nUniqueID > nMaxUniID)
			nMaxUniID = nUniqueID;
	}
	return nMaxUniID;
}

CVehicleRoute::StretchNodeVector CVehicleRoute::GetChildNodes( int nParentNodeID )
{
	StretchNodeVector ret;
	for(int i=0;i<GetStretchNodeCount();++i)
	{
		CStretchNode* pNode = GetStretchNodeItem(i);
		if(pNode&&pNode->GetParentID()== nParentNodeID)
			ret.push_back(pNode);
	}
	return ret;
}

CStretchNode* CVehicleRoute::GetStretchNodeItemByID( int nNodeID )
{
	for(int i=0;i<GetStretchNodeCount();++i)
	{
		CStretchNode * pStretchNode = GetStretchNodeItem(i);
		if(pStretchNode->GetID() == nNodeID) 
			return pStretchNode;
	}
	return NULL;
}
//------------CVehicleRouteList-----------------
CVehicleRouteList::CVehicleRouteList()
{
}

CVehicleRouteList::~CVehicleRouteList()
{
	RemoveAll();
}

void CVehicleRouteList::ReadData(int nProjID)
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_VEHICLEROUTE\
					 WHERE (PROJID = %d)"),nProjID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CVehicleRoute* pVehicleRoute = new CVehicleRoute;
		pVehicleRoute->ReadData(adoRecordset);

		m_vVehicleRoute.push_back(pVehicleRoute);
		adoRecordset.MoveNextData();
	}
}

void CVehicleRouteList::SaveData(int nProjID)
{
	VehicleRouteIter iter  = m_vVehicleRoute.begin();
	for (; iter != m_vVehicleRoute.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(nProjID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelVehicleRoute.begin(); iter != m_vNeedDelVehicleRoute.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelVehicleRoute.clear();
}

int CVehicleRouteList::GetVehicleRouteCount()
{
	return (int)m_vVehicleRoute.size();
}

CVehicleRoute* CVehicleRouteList::GetVehicleRouteItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vVehicleRoute.size());
	return m_vVehicleRoute[nIndex];
}

void CVehicleRouteList::AddVehicleRouteItem(CVehicleRoute* pItem)
{
	m_vVehicleRoute.push_back(pItem);
}
void CVehicleRouteList::DelVehicleRouteItem(CVehicleRoute* pItem)
{
	VehicleRouteIter iter = 
		std::find(m_vVehicleRoute.begin(),m_vVehicleRoute.end(),pItem);
	if (iter == m_vVehicleRoute.end())
		return;
	m_vNeedDelVehicleRoute.push_back(pItem);
	m_vVehicleRoute.erase(iter);
}
void CVehicleRouteList::RemoveAll()
{
	VehicleRouteIter iter = m_vVehicleRoute.begin();
	for (; iter != m_vVehicleRoute.end(); ++iter)
		delete *iter;

	for (iter = m_vNeedDelVehicleRoute.begin(); iter != m_vNeedDelVehicleRoute.end(); ++iter)
		delete *iter;
}

void CVehicleRouteList::ExportVehicleRoutes(CAirsideExportFile& exportFile)
{
	CVehicleRouteList VehicleRouteList;
	VehicleRouteList.ReadData(exportFile.GetProjectID());
	VehicleRouteList.ExportData(exportFile);
	exportFile.getFile().endFile();
}
void CVehicleRouteList::ExportData(CAirsideExportFile& exportFile)
{		
	exportFile.getFile().writeField("VehicleRoute");
	exportFile.getFile().writeLine();
	VehicleRouteIter iter = m_vVehicleRoute.begin();
	for (; iter!=m_vVehicleRoute.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}
void CVehicleRouteList::ImportVehicleRoutes(CAirsideImportFile& importFile)
{
	while(!importFile.getFile().isBlank())
	{
		CVehicleRoute VehicleRoute;
		VehicleRoute.ImportData(importFile);
	}
}