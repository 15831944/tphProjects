#include "stdafx.h"
#include "VehicleDispProps.h"
#include "..\Database\DBElementCollection_Impl.h"


CVehicleDispProps::CVehicleDispProps(void)
{
	m_nProjID = -1;
	m_nCurSelVehicle = -1; 
}

CVehicleDispProps::~CVehicleDispProps(void)
{
	Clear();
}

void CVehicleDispProps::Clear(void)
{
	for (std::vector<CVehicleDispPropNode *>::iterator itrVehicle = m_vrVehicleDispProp.begin();\
		itrVehicle != m_vrVehicleDispProp.end();++ itrVehicle) {
			if(*itrVehicle)
				delete (*itrVehicle);
	}
	m_vrVehicleDispProp.clear();
	m_nCurSelVehicle = -1;
}

void CVehicleDispProps::SaveData(void)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		for (std::vector<CVehicleDispPropNode *>::iterator itrVehicle = m_vrVehicleDispProp.begin();\
			itrVehicle != m_vrVehicleDispProp.end();++ itrVehicle) {
				if(*itrVehicle)
					(*itrVehicle)->SaveData(0);//parameter not use
			}
			CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}

void CVehicleDispProps::ReadData(int nProjID)
{
	if(nProjID < 0)return;
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM VEHICLEDISPLAYPROPERTYNODE WHERE (PROJID = %d);"),nProjID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	m_nProjID = nProjID;
	int nIDValue = -1;
	Clear();
	m_nCurSelVehicle = 0;
	if(adoRecordset.IsEOF())
	{
		AddVehicleDispProp(_T("Default"));
		return;
	}
	for (;!adoRecordset.IsEOF();adoRecordset.MoveNextData()) {
		adoRecordset.GetFieldValue(_T("ID"),nIDValue);
		CVehicleDispPropNode * pVehicleDispProp = new CVehicleDispPropNode(m_nProjID);
		if(!pVehicleDispProp)continue;
		pVehicleDispProp->ReadData(nIDValue);
		m_vrVehicleDispProp.push_back(pVehicleDispProp);
	}
}

void CVehicleDispProps::DeleteData(void)
{
	for (std::vector<CVehicleDispPropNode *>::iterator itrVehicle = m_vrVehicleDispProp.begin();\
		itrVehicle != m_vrVehicleDispProp.end();++ itrVehicle) {
			if(*itrVehicle)
				(*itrVehicle)->DeleteData();
		}
}

void CVehicleDispProps::UpdateData(void)
{
	for (std::vector<CVehicleDispPropNode *>::iterator itrVehicle = m_vrVehicleDispProp.begin();\
		itrVehicle != m_vrVehicleDispProp.end();++ itrVehicle) {
			if(*itrVehicle)
				(*itrVehicle)->UpdateData();
		}
}

int CVehicleDispProps::GetVehicleCount(void)
{
	return (int(m_vrVehicleDispProp.size()));
}

CVehicleDispPropNode * CVehicleDispProps::GetVehicleDispProp(int nIndex)
{
	if(nIndex < 0)return (0);
	int i = 0;
	for (std::vector<CVehicleDispPropNode *>::iterator itrVehicle = m_vrVehicleDispProp.begin();\
		itrVehicle != m_vrVehicleDispProp.end();++ itrVehicle) {
			if(nIndex == i++)
				return (*itrVehicle);
		}
	return (0);
}

int CVehicleDispProps::GetCurSelVehicle(void)
{
	return (m_nCurSelVehicle);
}

void CVehicleDispProps::SetCurSelVehicle(int nCurSelVehicle)
{
	m_nCurSelVehicle = nCurSelVehicle;
}

void CVehicleDispProps::SetProjID(int nProjID)
{
	m_nProjID = nProjID;
}

void CVehicleDispProps::AddVehicleDispProp(CString strVehicleDispPropNodeName)
{
	if(strVehicleDispPropNodeName.IsEmpty())return;
	/*for (std::vector<CVehicleDispPropNode *>::iterator itrVehicle = m_vrVehicleDispProp.begin();\
		itrVehicle != m_vrVehicleDispProp.end();++ itrVehicle) {
			if((*itrVehicle)->GetVehicleNodeName() == strVehicleDispPropNodeName )
				return;
		}*/
	CVehicleDispPropNode * pVehicleDispProp = new CVehicleDispPropNode(m_nProjID);
	if(!pVehicleDispProp)return;
	pVehicleDispProp->SetVehicleNodeName(strVehicleDispPropNodeName);
	CVehicleDispPropItem * pItemDefault = new CVehicleDispPropItem(m_nProjID);
	if(!pItemDefault)
	{
		delete pVehicleDispProp;
		return;
	} 
	pVehicleDispProp->AddVehicleDispPropItem(pItemDefault);
	m_vrVehicleDispProp.push_back(pVehicleDispProp);
}

CVehicleDispPropNode * CVehicleDispProps::DeleteVehicleDispProp(int nIndex)
{
	CVehicleDispPropNode * pDeletedNode = 0;
	if(nIndex < 0)return (0);
	int i = 0;
	for (std::vector<CVehicleDispPropNode *>::iterator itrVehicle = m_vrVehicleDispProp.begin();\
		itrVehicle != m_vrVehicleDispProp.end();++ itrVehicle) {
			if(nIndex == i++ )
			{
				/*(*itrVehicle)->DeleteData();*/
				pDeletedNode = (*itrVehicle);
				m_vrVehicleDispProp.erase(itrVehicle);
				break;
			}
		}
		if(m_nCurSelVehicle == nIndex)
		{
			if(nIndex - 1 < 0)
				m_nCurSelVehicle = 0;
			else
				m_nCurSelVehicle --;
		}
		return (pDeletedNode); 
}

void CVehicleDispProps::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	int nCount = -1;
	importFile.getFile().getInteger(nCount);
	for (int i = 0; i < nCount; i++) {
		CVehicleDispPropNode * pVehicleDispPropNode = new CVehicleDispPropNode(m_nProjID);
		if(!pVehicleDispPropNode)return;
		pVehicleDispPropNode->ImportData(importFile);
		m_vrVehicleDispProp.push_back(pVehicleDispPropNode);
	}
	m_nCurSelVehicle = 0;
	importFile.getFile().getLine();
}

void CVehicleDispProps::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("VehicleDisplayProperty"));
	exportFile.getFile().writeLine();
	exportFile.getFile().writeInt(m_nProjID);
	int nSize = (int)m_vrVehicleDispProp.size();
	exportFile.getFile().writeInt(nSize);
	for (std::vector<CVehicleDispPropNode *>::iterator itrVehicle = m_vrVehicleDispProp.begin();\
		itrVehicle != m_vrVehicleDispProp.end();++ itrVehicle) {
			if(*itrVehicle)
				(*itrVehicle)->ExportData(exportFile);
		}
	exportFile.getFile().writeLine();
}

void CVehicleDispProps::ImportVehicleDispProps(CAirsideImportFile& importFile)
{
	CVehicleDispProps vehicleDispProp;
	vehicleDispProp.ImportData(importFile);
	vehicleDispProp.SaveData();
}

void CVehicleDispProps::ExportDataVehicleDispProps(CAirsideExportFile& exportFile)
{
	CVehicleDispProps vehicleDispProp;
	vehicleDispProp.ReadData(exportFile.GetProjectID());
	vehicleDispProp.ExportData(exportFile);
	exportFile.getFile().endFile();
}

CVehicleDispPropItem* CVehicleDispProps::FindBestMatch(int VehicleTypeID)
{
	int nVehicleTypeid = VehicleTypeID;
	CVehicleDispPropNode * pDisplayNode = GetVehicleDispProp(GetCurSelVehicle());
	if(pDisplayNode)
	{
		for(int i=0;i< pDisplayNode->GetVehicleDispPropCount();i++)
		{
			CVehicleDispPropItem* dispPropItem = pDisplayNode->GetVehicleDispProp(i);
			if(dispPropItem && dispPropItem->GetVehicleType()->GetID() == nVehicleTypeid )
				return dispPropItem;
		}
	}
	return NULL;		
}