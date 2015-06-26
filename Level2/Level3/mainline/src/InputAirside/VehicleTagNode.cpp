#include "stdafx.h"
#include "VehicleTagNode.h"
#include "..\Database\DBElementCollection_Impl.h"


CVehicleTagNode::CVehicleTagNode(int nProjID):m_nProjID(nProjID)
{
	m_nID = -1;
	m_strVehicleTagNodeName = _T("Default");
	m_nCurSelItem = -1;
}

CVehicleTagNode::~CVehicleTagNode(void)
{
	Clear();
}

void CVehicleTagNode::Clear(void)
{
	for (std::vector<CVehicleTagItem *>::iterator itrTagItem = m_vrTagItem.begin();\
		itrTagItem != m_vrTagItem.end();++ itrTagItem) {
			if(*itrTagItem)
			{
				delete (*itrTagItem);
			}
	}
	m_vrTagItem.clear();
	m_nCurSelItem = -1;
}

void CVehicleTagNode::ReadData(int nID)
{
	if(nID == -1)return;
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM VEHICLETAGNODE WHERE (ID = %d);"),nID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	Clear();
	if (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("NODENAME"),m_strVehicleTagNodeName);
		std::vector<int> vrTagItem;
		CVehicleTagItem::GetVehicleTagItemList(m_nID,vrTagItem);
		for (std::vector<int>::const_iterator citrID = vrTagItem.begin();\
			citrID != vrTagItem.end();++ citrID) {
				CVehicleTagItem * pTagItem = new CVehicleTagItem(m_nProjID);
				if(!pTagItem){
					Clear();
					return;
				}
				pTagItem->ReadData(*citrID);
				m_vrTagItem.push_back(pTagItem);
		}
		m_nCurSelItem = 0;
	}
}

void CVehicleTagNode::GetVehicleTagNodeList(int nProjID,std::vector<int> & vrVehicleTagNodeID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM VEHICLETAGNODE WHERE (PROJID = %d);"),nProjID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset); 
	vrVehicleTagNodeID.clear();
	while (!adoRecordset.IsEOF())
	{
		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID); 
		vrVehicleTagNodeID.push_back(nID);
		adoRecordset.MoveNextData();
	}
}

void CVehicleTagNode::SaveData(void)
{
	CString strSQL = _T("");
	if(m_nID  == -1)
	{
		strSQL.Format(_T("INSERT INTO VEHICLETAGNODE (PROJID,NODENAME) VALUES (%d,\'%s\');"),m_nProjID,m_strVehicleTagNodeName);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

		
	}
	else
	{
		strSQL.Format(_T("UPDATE VEHICLETAGNODE SET PROJID = %d,NODENAME = \'%s\' WHERE (ID = %d);"),m_nProjID,m_strVehicleTagNodeName, m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
	
	for (std::vector<CVehicleTagItem *>::iterator itrTagItem = m_vrTagItem.begin();\
		itrTagItem != m_vrTagItem.end();++ itrTagItem) {
			if(*itrTagItem)
				(*itrTagItem)->SaveData(m_nID);
		}
}

void CVehicleTagNode::DeleteData(void)
{
	if(m_nID == -1)return;
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM VEHICLETAGNODE WHERE (ID = %d);"), m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);	

	for (std::vector<CVehicleTagItem *>::iterator itrTagItem = m_vrTagItem.begin();\
		itrTagItem != m_vrTagItem.end();++ itrTagItem) {
			if(*itrTagItem)
				(*itrTagItem)->DeleteData();
		}
	Clear();
	m_nID = -1;
	m_nProjID = -1;
}

void CVehicleTagNode::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_strVehicleTagNodeName.GetLength());
	exportFile.getFile().writeField(m_strVehicleTagNodeName);

	exportFile.getFile().writeInt((int)m_vrTagItem.size());
	for (std::vector<CVehicleTagItem *>::iterator itrTagItem = m_vrTagItem.begin();\
		itrTagItem != m_vrTagItem.end();++ itrTagItem) {
			if(*itrTagItem)
			{
				(*itrTagItem)->ExportData(exportFile);
			}
		}

	exportFile.getFile().writeLine();
}

void CVehicleTagNode::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nID); 
	m_nID = -1;
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	int iValue = -1;
	importFile.getFile().getInteger(iValue);
	char * pszVehicleTagNodeName = 0;
	pszVehicleTagNodeName = new char[iValue+1];
	importFile.getFile().getField(pszVehicleTagNodeName,iValue);
	*(pszVehicleTagNodeName + iValue) = 0;
	m_strVehicleTagNodeName = pszVehicleTagNodeName;
	delete []pszVehicleTagNodeName;

	importFile.getFile().getInteger(iValue);
	Clear();
	for (int i = 0;i < iValue;i++) {
		CVehicleTagItem * pTagItem = new CVehicleTagItem(m_nProjID);
		if(!pTagItem){
			Clear();
			return;
		}
		pTagItem->ImportData(importFile);
		m_vrTagItem.push_back(pTagItem);
	}
	m_nCurSelItem = 0;
	importFile.getFile().getLine();
}

void CVehicleTagNode::AddItem(CVehicleTagItem * pVehicleTagItemAdding)
{
	if(!pVehicleTagItemAdding)return;
	m_vrTagItem.push_back(pVehicleTagItemAdding);
}

CVehicleTagItem * CVehicleTagNode::DeleteItem(int nIndex)
{
	CVehicleTagItem * pVehicleTagItemDeleted = 0;
	if(nIndex < 0 || nIndex >= (int)m_vrTagItem.size())return (pVehicleTagItemDeleted);
	std::vector<CVehicleTagItem *>::iterator itrTagItemDeleted = m_vrTagItem.begin() + nIndex;
	pVehicleTagItemDeleted = (*itrTagItemDeleted);
	
	m_vrTagItem.erase(itrTagItemDeleted);
	if(m_nCurSelItem >= nIndex)
	{
		if(m_nCurSelItem - 1 >= 0)
			m_nCurSelItem --;
		else
			m_nCurSelItem = -1;
	}
	return (pVehicleTagItemDeleted);
}

CVehicleTagItem * CVehicleTagNode::GetItem(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_vrTagItem.size())return (0);
	std::vector<CVehicleTagItem *>::iterator itrTagItem = m_vrTagItem.begin() + nIndex;
	return (*itrTagItem);
}

int CVehicleTagNode::GetItemCount(void)
{
	return (int(m_vrTagItem.size()));
}

bool CVehicleTagNode::SetCurSelItemIndex(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_vrTagItem.size())return (false);
	m_nCurSelItem = nIndex;
	return (true);
} 

int CVehicleTagNode::GetCurSelItemIndex(void)
{
	return (m_nCurSelItem);
}

int CVehicleTagNode::GetID(void)
{
	return (m_nID);
}