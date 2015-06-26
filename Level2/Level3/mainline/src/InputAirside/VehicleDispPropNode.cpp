#include "stdafx.h"
#include "VehicleDispPropNode.h"
#include "..\Database\DBElementCollection_Impl.h"


CVehicleDispPropNode::CVehicleDispPropNode(int nProjID):DBElement()
{
	m_nProjID = nProjID;
	m_strVehicleNodeName = _T(""); 
	m_nCulSelItem = -1;
}

 CVehicleDispPropNode::~CVehicleDispPropNode(void)
 {
	Clear();
 }

CString CVehicleDispPropNode::GetVehicleNodeName(void)
{
	return (m_strVehicleNodeName);
}

void CVehicleDispPropNode::SetVehicleNodeName(CString strName)
{
	m_strVehicleNodeName = strName;
}

CVehicleDispPropItem * CVehicleDispPropNode::GetVehicleDispProp(int nIndex)
{
	if(nIndex < 0)return (0);
	int i = 0;
	for(std::vector<CVehicleDispPropItem *>::iterator itrVehicleDispPropItem = m_vrVehicleDispProp.begin();\
		itrVehicleDispPropItem != m_vrVehicleDispProp.end();++ itrVehicleDispPropItem)
	{
		if(i++ == nIndex)
			return (*itrVehicleDispPropItem);
	}
	return (0);
}

int CVehicleDispPropNode::GetCulSelItem(void)
{
	return (m_nCulSelItem);
}

void CVehicleDispPropNode::SetCulSelItem(int nCulSel)
{
	if(nCulSel > -1 && nCulSel < (int)m_vrVehicleDispProp.size())
		m_nCulSelItem = nCulSel;
}

void CVehicleDispPropNode::AddVehicleDispPropItem(CVehicleDispPropItem * pVehicleDispPropItem)
{
	if(!pVehicleDispPropItem)return;
	pVehicleDispPropItem->SetParentID(m_nID);
	for(std::vector<CVehicleDispPropItem *>::iterator itrVehicleDispPropItem = m_vrVehicleDispProp.begin();\
		itrVehicleDispPropItem != m_vrVehicleDispProp.end();++ itrVehicleDispPropItem)
	{
		if(pVehicleDispPropItem == *itrVehicleDispPropItem)	
		{
			delete *itrVehicleDispPropItem;
			m_vrVehicleDispProp.erase(itrVehicleDispPropItem);
			m_vrVehicleDispProp.insert(itrVehicleDispPropItem,pVehicleDispPropItem);
			return;
		}
	}
	m_vrVehicleDispProp.push_back(pVehicleDispPropItem);
}

CVehicleDispPropItem * CVehicleDispPropNode::DeleteVehicleDispPropItem(int nIndex)
{
	if(nIndex < 0)return (0);
	int i = 0;
	for(std::vector<CVehicleDispPropItem *>::iterator itrVehicleDispPropItem = m_vrVehicleDispProp.begin();\
		itrVehicleDispPropItem != m_vrVehicleDispProp.end();++ itrVehicleDispPropItem)
	{
		if(nIndex == i++)	
		{
			/*( *itrVehicleDispPropItem)->DBElement::DeleteData();*/
			CVehicleDispPropItem *pDeletedItem = *itrVehicleDispPropItem;
			m_vrVehicleDispProp.erase(itrVehicleDispPropItem);
			if(m_nCulSelItem == nIndex)
			{
				if(m_nCulSelItem - 1 < 0)
					m_nCulSelItem = -1;
				else
					m_nCulSelItem--;
			}
			return (pDeletedItem);
		}
	}
	return (0);
}

int CVehicleDispPropNode::GetVehicleDispPropCount(void)
{
	return (int(m_vrVehicleDispProp.size()));
}

void CVehicleDispPropNode::InitFromDBRecordset(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
	adoRecordset.GetFieldValue(_T("VEHICLENAME"),m_strVehicleNodeName);	
}

void CVehicleDispPropNode::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO VEHICLEDISPLAYPROPERTYNODE \
					 (PROJID,VEHICLENAME) \
					 VALUES (%d,'%s');"),m_nProjID,m_strVehicleNodeName);
}

void CVehicleDispPropNode::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE VEHICLEDISPLAYPROPERTYNODE SET \
					 PROJID = %d,VEHICLENAME = '%s'\
					 WHERE (ID = %d);"),m_nProjID,m_strVehicleNodeName,m_nID);
}

void CVehicleDispPropNode::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM VEHICLEDISPLAYPROPERTYNODE WHERE (ID = %d);"),m_nID);
}

void CVehicleDispPropNode::GetSelectSQL(int nID,CString& strSQL)const
{
	strSQL.Format(_T("SELECT * FROM VEHICLEDISPLAYPROPERTYNODE WHERE (ID = %d);"),nID);
}

void CVehicleDispPropNode::ExportData(CAirsideExportFile& exportFile)
{	
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	int nLen = m_strVehicleNodeName.GetLength();
	exportFile.getFile().writeInt(nLen);
	exportFile.getFile().writeField(m_strVehicleNodeName.GetBuffer(nLen));
	m_strVehicleNodeName.ReleaseBuffer(nLen);
	
	exportFile.getFile().writeInt((int)m_vrVehicleDispProp.size());
	for(std::vector<CVehicleDispPropItem *>::iterator itrVehicleDispPropItem = m_vrVehicleDispProp.begin();\
		itrVehicleDispPropItem != m_vrVehicleDispProp.end();++ itrVehicleDispPropItem)
	{
		if((*itrVehicleDispPropItem))
			(*itrVehicleDispPropItem)->ExportData(exportFile);
	}
	exportFile.getFile().writeLine();
}

void CVehicleDispPropNode::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nID); 
	m_nID = -1;
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	int nValue = -1;
	importFile.getFile().getInteger(nValue);
	char * pszVehicleNodeName = new char[nValue + 1];
	importFile.getFile().getField(pszVehicleNodeName,nValue);
	m_strVehicleNodeName = pszVehicleNodeName;
	delete []pszVehicleNodeName;
	pszVehicleNodeName = 0;
	
	Clear();
	nValue = -1;
	importFile.getFile().getInteger(nValue);
	for(int i = 0;i < nValue;i++)
	{
		CVehicleDispPropItem * pVehicleDispPropItem = new CVehicleDispPropItem(m_nProjID);
		if(!pVehicleDispPropItem)return;
		pVehicleDispPropItem->ImportData(importFile);
		m_vrVehicleDispProp.push_back(pVehicleDispPropItem);
	}
	importFile.getFile().getLine();	
}

void CVehicleDispPropNode::SaveData(int nParentID)
{
	DBElement::SaveData(nParentID);
	for(std::vector<CVehicleDispPropItem *>::iterator itrVehicleDispPropItem = m_vrVehicleDispProp.begin();\
		itrVehicleDispPropItem != m_vrVehicleDispProp.end();++ itrVehicleDispPropItem)
	{
		if((*itrVehicleDispPropItem))
			(*itrVehicleDispPropItem)->SaveData(m_nID);
	}
}

void CVehicleDispPropNode::DeleteData(void)
{
	for(std::vector<CVehicleDispPropItem *>::iterator itrVehicleDispPropItem = m_vrVehicleDispProp.begin();\
	 itrVehicleDispPropItem != m_vrVehicleDispProp.end();++ itrVehicleDispPropItem)
	{
		if((*itrVehicleDispPropItem))
			(*itrVehicleDispPropItem)->DeleteData();
	}
	DBElement::DeleteData();
	
	m_nCulSelItem = -1;
}

void CVehicleDispPropNode::UpdateData(void)
{
	DBElement::SaveData(-1);
	for(std::vector<CVehicleDispPropItem *>::iterator itrVehicleDispPropItem = m_vrVehicleDispProp.begin();\
		itrVehicleDispPropItem != m_vrVehicleDispProp.end();++ itrVehicleDispPropItem)
	{
		if((*itrVehicleDispPropItem))
			(*itrVehicleDispPropItem)->SaveData(m_nID);
	}
}

void CVehicleDispPropNode::ReadData(int nID)
{
	DBElement::ReadData(nID);
	std::vector<int> vrIDList;
	CVehicleDispPropItem::GetVehicleDispPropItemIDListByParentID(nID,vrIDList);
	Clear();
	for(std::vector<int>::iterator itrID = vrIDList.begin();\
		itrID != vrIDList.end();++ itrID)
	{
		 CVehicleDispPropItem * pVehicleDispPropItem = new CVehicleDispPropItem(m_nProjID);
		 if(!pVehicleDispPropItem)continue;
		 pVehicleDispPropItem->ReadData(*itrID);
		 m_vrVehicleDispProp.push_back(pVehicleDispPropItem);
	}
	m_nCulSelItem = 0;
}

void CVehicleDispPropNode::Clear(void)
{
	for(std::vector<CVehicleDispPropItem *>::iterator itrVehicleDispPropItem = m_vrVehicleDispProp.begin();\
		itrVehicleDispPropItem != m_vrVehicleDispProp.end();++ itrVehicleDispPropItem)
	{
		if((*itrVehicleDispPropItem))
			delete (*itrVehicleDispPropItem);
	}
	m_vrVehicleDispProp.clear();
	m_nCulSelItem = -1;
}