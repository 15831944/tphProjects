#include "StdAfx.h"
#include "TemporaryParkingCriteria.h"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"


//////////////////////////////////////////////////////////////////////////
// class CTemporaryParkingCriteriaData

CTemporaryParkingCriteriaData::CTemporaryParkingCriteriaData()
: m_nID(-1)
 ,m_nStartID(-1)
 ,m_nEndID(-1)
 ,m_nEntryID(-1)
{
}
CTemporaryParkingCriteriaData::~CTemporaryParkingCriteriaData()
{
}

void CTemporaryParkingCriteriaData::SetStartID(int nStartID)
{ 
	m_nStartID = nStartID;
}
int	 CTemporaryParkingCriteriaData::GetStartID()
{
	return m_nStartID;
}

void CTemporaryParkingCriteriaData::SetEndID(int nEndID)
{ 
	m_nEndID = nEndID; 
}
int	 CTemporaryParkingCriteriaData::GetEndID()
{	
	return m_nEndID;
}

void CTemporaryParkingCriteriaData::SetEntryID(int nEntryID)
{
	m_nEntryID = nEntryID;
}
int CTemporaryParkingCriteriaData::GetEntryID()
{
	return m_nEntryID;
}

void CTemporaryParkingCriteriaData::SetOrientationType(OrientationType emType)
{
	m_emType = emType;
}
OrientationType	CTemporaryParkingCriteriaData::GetOrientationType()
{
	return m_emType;

}
//ID, STARTNODEID, ENDNODEID, ENTRYNODEID, ORIENTTYPE
void CTemporaryParkingCriteriaData::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("STARTNODEID"),m_nStartID);
	adoRecordset.GetFieldValue(_T("ENDNODEID"),m_nEndID);
	adoRecordset.GetFieldValue(_T("ENTRYNODEID"),m_nEntryID);

	int nOrientType = 0;
	adoRecordset.GetFieldValue(_T("ORIENTTYPE"),nOrientType);

	if (nOrientType> Orientation_unknown && nOrientType < Orientation_Count)
	{
		m_emType = (OrientationType)nOrientType;
	}

}
void CTemporaryParkingCriteriaData::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TEMPPARKINGCRITERIA_TAXIWAY_DATA\
		SET STARTNODEID =%d, ENDNODEID =%d, ENTRYNODEID =%d, \
		ORIENTTYPE =%d\
		WHERE (ID = %d)"),m_nStartID,m_nEndID,m_nEntryID,(int)m_emType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void CTemporaryParkingCriteriaData::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_TEMPPARKINGCRITERIA_TAXIWAY_DATA\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void CTemporaryParkingCriteriaData::SaveData(int nTempParkID)
{
	if (m_nID >=0)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TEMPPARKINGCRITERIA_TAXIWAY_DATA\
		(TEMPPARKID, STARTNODEID, ENDNODEID, ENTRYNODEID, ORIENTTYPE)\
		VALUES (%d,%d,%d,%d,%d)"),nTempParkID,m_nStartID,m_nEndID,m_nEntryID,(int)m_emType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void CTemporaryParkingCriteriaData::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nStartID);
	exportFile.getFile().writeInt(m_nEndID);
	exportFile.getFile().writeInt(m_nEntryID);
	exportFile.getFile().writeInt((int)m_emType);
	exportFile.getFile().writeLine();
}
void CTemporaryParkingCriteriaData::ImportData(CAirsideImportFile& importFile,int nTempParkID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nOldStartID  = -1;
	importFile.getFile().getInteger(nOldStartID);
	m_nStartID = importFile.GetIntersectionIndexMap(nOldStartID);

	int nOldEndID = -1;
	importFile.getFile().getInteger(nOldEndID);
	m_nEndID = importFile.GetIntersectionIndexMap(nOldEndID);

	int nOldEntryID = -1;
	importFile.getFile().getInteger(nOldEntryID);
	m_nEntryID = importFile.GetIntersectionIndexMap(nOldEntryID);

	int nEnumType = 0;
	importFile.getFile().getInteger(nEnumType);

	if (nEnumType> Orientation_unknown && nEnumType < Orientation_Count)
	{
		m_emType = (OrientationType)nEnumType;
	}

	importFile.getFile().getLine();

	SaveData(nTempParkID);
}
//////////////////////////////////////////////////////////////////////////
// class CTemporaryParkingCriteria

CTemporaryParkingCriteria::CTemporaryParkingCriteria()
: m_nID(-1)
 ,m_nTaxiwayID(-1)
{
}
CTemporaryParkingCriteria::~CTemporaryParkingCriteria()
{
	std::vector<CTemporaryParkingCriteriaData *>::iterator iter = m_vParkingData.begin();
	for (; iter!= m_vParkingData.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vParkingDataNeedDel.begin();
	for (; iter != m_vParkingDataNeedDel.end(); ++iter)
	{
		delete *iter;
	}
}

void CTemporaryParkingCriteria::SetTaxiwayID(int nTaxiwayID)
{
	m_nTaxiwayID = nTaxiwayID;
}
int	 CTemporaryParkingCriteria::GetTaxiwayID()
{ 
	return m_nTaxiwayID; 
}

void CTemporaryParkingCriteria::AddItem(CTemporaryParkingCriteriaData* pNewItem)
{
	m_vParkingData.push_back(pNewItem);
}
void CTemporaryParkingCriteria::DelItem(CTemporaryParkingCriteriaData* pDelItem)
{
	std::vector<CTemporaryParkingCriteriaData *>::iterator iter = std::find(m_vParkingData.begin(), m_vParkingData.end(), pDelItem);
	if(iter == m_vParkingData.end())
		return;

	m_vParkingDataNeedDel.push_back(pDelItem);
	m_vParkingData.erase(iter);
}

CTemporaryParkingCriteriaData* CTemporaryParkingCriteria::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vParkingData.size());
	return m_vParkingData.at(nIndex);
}
int CTemporaryParkingCriteria::GetItemCount()
{
	return static_cast<int>( m_vParkingData.size() ); 
}
void CTemporaryParkingCriteria::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("TAXIWAYID"),m_nTaxiwayID);

	ReadData();
}
void CTemporaryParkingCriteria::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, STARTNODEID, ENDNODEID, ENTRYNODEID, ORIENTTYPE\
		FROM IN_TEMPPARKINGCRITERIA_TAXIWAY_DATA\
		WHERE (TEMPPARKID = %d)"),m_nID);
	
	long nItemEffect = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemEffect,adoRecordset);
	
	while (!adoRecordset.IsEOF())
	{	
		CTemporaryParkingCriteriaData * pTempParkData = new CTemporaryParkingCriteriaData();

		pTempParkData->ReadData(adoRecordset);
		m_vParkingData.push_back(pTempParkData);
		adoRecordset.MoveNextData();
	}


}
void CTemporaryParkingCriteria::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_TEMPPARKINGCRITERIA_TAXIWAY\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CTemporaryParkingCriteriaData *>::iterator iter = m_vParkingData.begin();

	for (;iter !=m_vParkingData.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vParkingData.clear();
	for (iter = m_vParkingDataNeedDel.begin(); iter !=m_vParkingDataNeedDel.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vParkingDataNeedDel.clear();
}
void CTemporaryParkingCriteria::UpdateData()
{
	SaveParkingData();
}
void CTemporaryParkingCriteria::SaveData(int nprojID)
{

	if (m_nID >=0)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TEMPPARKINGCRITERIA_TAXIWAY\
		(PROJID, TAXIWAYID)\
		VALUES (%d,%d)"),nprojID,m_nTaxiwayID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	SaveParkingData();
}

void CTemporaryParkingCriteria::SaveParkingData()
{
	std::vector<CTemporaryParkingCriteriaData *>::iterator iter = m_vParkingData.begin();
	for (;iter != m_vParkingData.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	iter = m_vParkingDataNeedDel.begin();
	for (;iter != m_vParkingDataNeedDel.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vParkingDataNeedDel.clear();
}
void CTemporaryParkingCriteria::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nTaxiwayID);
	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vParkingData.size()));

	//export data in vector
	std::vector<CTemporaryParkingCriteriaData *>::iterator iter = m_vParkingData.begin();
	for (;iter != m_vParkingData.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}

}
void CTemporaryParkingCriteria::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	
	int nOldTaxiwayID = -1;
	importFile.getFile().getInteger(nOldTaxiwayID);
	
	m_nTaxiwayID = importFile.GetObjectNewID(nOldTaxiwayID);
	SaveData(importFile.getNewProjectID());

	importFile.getFile().getLine();
	//vector data
	int nParkingDataCount = 0;
	importFile.getFile().getInteger(nParkingDataCount);

	for (int i =0; i < nParkingDataCount; ++i)
	{
		CTemporaryParkingCriteriaData data;
		data.ImportData(importFile,m_nID);
	}

	importFile.AddTemporaryParkingIndexMap(nOldID, m_nID);
}
//////////////////////////////////////////////////////////////////////////
// class CTaxiwayWeightConstraintList

CTemporaryParkingCriteriaList::CTemporaryParkingCriteriaList()
: m_nProjID(-1)
{
}
CTemporaryParkingCriteriaList::~CTemporaryParkingCriteriaList()
{
	std::vector<CTemporaryParkingCriteria *>::iterator iter = m_vParking.begin();
	for (; iter!= m_vParking.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vParkingNeedDel.begin();
	for (; iter != m_vParkingNeedDel.end(); ++iter)
	{
		delete *iter;
	}
}


void CTemporaryParkingCriteriaList::AddItem(CTemporaryParkingCriteria* pNewItem)
{
	m_vParking.push_back(pNewItem);
}

void CTemporaryParkingCriteriaList::DelItem(CTemporaryParkingCriteria* pDelItem)
{
	std::vector<CTemporaryParkingCriteria *>::iterator iter = std::find(m_vParking.begin(), m_vParking.end(), pDelItem);
	if(iter == m_vParking.end())
		return;

	m_vParkingNeedDel.push_back(pDelItem);
	m_vParking.erase(iter);
}

void CTemporaryParkingCriteriaList::DelItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vParking.size());

	m_vParkingNeedDel.push_back(m_vParking[nIndex]);
	m_vParking.erase(m_vParking.begin() + nIndex);
}

CTemporaryParkingCriteria* CTemporaryParkingCriteriaList::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vParking.size());
	return m_vParking.at(nIndex);
}

CTemporaryParkingCriteria* CTemporaryParkingCriteriaList::GetItemByID(int nID)
{
	std::vector<CTemporaryParkingCriteria *>::iterator iter;
	for (iter=m_vParking.begin(); iter!=m_vParking.end(); iter++)
	{
		CTemporaryParkingCriteria* pTempParkingCriteria = *iter;

		if (nID == pTempParkingCriteria->getID())
		{
			return pTempParkingCriteria;
		}
	}

	return NULL;
}

int  CTemporaryParkingCriteriaList::GetItemCount()
{
	return static_cast<int>( m_vParking.size() );
}

void CTemporaryParkingCriteriaList::ReadData(int nprojID)
{

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, TAXIWAYID\
		FROM IN_TEMPPARKINGCRITERIA_TAXIWAY\
		WHERE (PROJID = %d)"),nprojID);

	long nItemEffect = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemEffect,adoRecordset);
	
	while (!adoRecordset.IsEOF())
	{	
		CTemporaryParkingCriteria * pTempPark = new CTemporaryParkingCriteria();

		pTempPark->ReadData(adoRecordset);
		m_vParking.push_back(pTempPark);
		adoRecordset.MoveNextData();
	}
	

}

void CTemporaryParkingCriteriaList::SaveData(int nProjID)
{
	std::vector<CTemporaryParkingCriteria *>::iterator iter = m_vParking.begin();
	for (;iter!=m_vParking.end(); ++iter)
	{
		(*iter)->SaveData(nProjID);
	}

	iter = m_vParkingNeedDel.begin();
	for (;iter != m_vParkingNeedDel.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}

	m_vParkingNeedDel.clear();

}
void CTemporaryParkingCriteriaList::ExportTemporaryParkingCriterias(CAirsideExportFile& exportFile)
{
	CTemporaryParkingCriteriaList tempParkList;
	tempParkList.ReadData(exportFile.GetProjectID());
	tempParkList.ExportTemporaryParkingCriteria(exportFile);
	exportFile.getFile().endFile();
}
void CTemporaryParkingCriteriaList::ExportTemporaryParkingCriteria(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("TemporaryParkingCriteria");
	exportFile.getFile().writeLine();

	std::vector<CTemporaryParkingCriteria *>::iterator iter = m_vParking.begin();
	for (;iter!=m_vParking.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}
void CTemporaryParkingCriteriaList::ImportTemporaryParkingCriterias(CAirsideImportFile& importFile)
{
	while(!importFile.getFile().isBlank())
	{
		CTemporaryParkingCriteria tempPark;
		tempPark.ImportData(importFile);
	}

}
void CTemporaryParkingCriteriaList::ImportTemporaryParkingCriteria(CAirsideImportFile& importFile)
{

}