#include "StdAfx.h"
#include "SideStepSpecification.h"
#include <algorithm>
#include "Runway.h"
#include "ALTAirport.h"///
#include "AirsideImportExportManager.h"
#include "..\Common\AirportDatabase.h"


//--------------------CSideStepToRunway--------------------

CSideStepToRunway::CSideStepToRunway()
{
	m_nID = -1;
}

CSideStepToRunway::~CSideStepToRunway()
{

}
void CSideStepToRunway::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("RUNWAYID"),m_nRunwayID);
	adoRecordset.GetFieldValue(_T("MARKINDEX"),m_nMarkIndex);
	adoRecordset.GetFieldValue(_T("MINFINALLEG"),m_nMinFinalLeg);
	adoRecordset.GetFieldValue(_T("MAXTURNANGLE"),m_nMaxTurnAngle);
}
void CSideStepToRunway::DeleteData() 
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_SIDESTEP_TORUNWAY\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void CSideStepToRunway::SaveData(int nApproachRunwayID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_SIDESTEP_TORUNWAY\
					 (SISTAPRWID, RUNWAYID, MARKINDEX,MINFINALLEG, MAXTURNANGLE)\
					 VALUES (%d,%d,%d,%d,%d)"),nApproachRunwayID,m_nRunwayID,m_nMarkIndex, m_nMinFinalLeg, m_nMaxTurnAngle);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void CSideStepToRunway::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_SIDESTEP_TORUNWAY\
					 SET RUNWAYID =%d, MARKINDEX =%d, MINFINALLEG=%d, MAXTURNANGLE=%d\
					 WHERE (ID = %d)"),m_nRunwayID,m_nMarkIndex, m_nMinFinalLeg, m_nMaxTurnAngle, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void CSideStepToRunway::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nRunwayID);
	exportFile.getFile().writeInt(m_nMarkIndex);
	exportFile.getFile().writeInt(m_nMinFinalLeg);
	exportFile.getFile().writeInt(m_nMaxTurnAngle);
	exportFile.getFile().writeLine();
}

void CSideStepToRunway::ImportData(CAirsideImportFile& importFile,int nApproachRunwayID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nOldRunwayID = -1;
	importFile.getFile().getInteger(nOldRunwayID);
	m_nRunwayID = importFile.GetObjectNewID(nOldRunwayID);

	importFile.getFile().getInteger(m_nMarkIndex);
	importFile.getFile().getInteger(m_nMinFinalLeg);
	importFile.getFile().getInteger(m_nMaxTurnAngle);

	SaveData(nApproachRunwayID);
	importFile.getFile().getLine();
}

//--------------------CSideStepApproachRunway--------------------

CSideStepApproachRunway::CSideStepApproachRunway()
{
	m_nID = -1;
}

CSideStepApproachRunway::~CSideStepApproachRunway()
{
	RemoveAll();
}

void CSideStepApproachRunway::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("RUNWAYID"),m_nRunwayID);
	adoRecordset.GetFieldValue(_T("MARKINDEX"),m_nMarkIndex);
	ReadToRunwayData();
}
void CSideStepApproachRunway::ReadToRunwayData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_SIDESTEP_TORUNWAY\
					 WHERE (SISTAPRWID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CSideStepToRunway* pToRunway = new CSideStepToRunway;
		pToRunway->ReadData(adoRecordset);

		m_vToRunway.push_back(pToRunway);
		adoRecordset.MoveNextData();
	}
}
void CSideStepApproachRunway::SaveData(int nFlightTypeID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_SIDESTEP_APPROACHRUNWAY\
					 (SISTFLTID, RUNWAYID,MARKINDEX)\
					 VALUES (%d,%d,%d)"),nFlightTypeID,m_nRunwayID,m_nMarkIndex);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	SideStepToRunwayIter iter = m_vToRunway.begin();
	for (;iter !=m_vToRunway.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vInvalidToRunway.begin(); iter !=m_vInvalidToRunway.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vInvalidToRunway.clear();
}
void CSideStepApproachRunway::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_SIDESTEP_APPROACHRUNWAY\
					 SET RUNWAYID =%d, MARKINDEX =%d\
					 WHERE (ID = %d)"),m_nRunwayID,m_nMarkIndex,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	SideStepToRunwayIter iter = m_vToRunway.begin();

	for (;iter !=m_vToRunway.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vInvalidToRunway.begin(); iter !=m_vInvalidToRunway.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vInvalidToRunway.clear();
}
int CSideStepApproachRunway::GetToRunwayCount()
{
	return (int)m_vToRunway.size();
}

CSideStepToRunway* CSideStepApproachRunway::GetToRunwayItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vToRunway.size());
	return m_vToRunway[nIndex];
}

void CSideStepApproachRunway::AddToRunwayItem(CSideStepToRunway* pItem)
{
	m_vToRunway.push_back(pItem);
}
void CSideStepApproachRunway::DeleteToRunwayItem(CSideStepToRunway* pItem)
{
	SideStepToRunwayIter iter = 
		std::find(m_vToRunway.begin(),m_vToRunway.end(), pItem);
	if (iter == m_vToRunway.end())
		return;
	m_vInvalidToRunway.push_back(pItem);
	m_vToRunway.erase(iter);
}
void CSideStepApproachRunway::RemoveAll()
{
	SideStepToRunwayIter iter = m_vToRunway.begin();
	for (; iter != m_vToRunway.end(); iter++)
		delete (*iter);
	for (iter = m_vInvalidToRunway.begin(); iter != m_vInvalidToRunway.end(); iter++)
		delete (*iter);
}
void CSideStepApproachRunway::CopyData(CSideStepApproachRunway* pApproachRunway)///
{
	*this = *pApproachRunway;
}
void CSideStepApproachRunway::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_SIDESTEP_APPROACHRUNWAY\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	SideStepToRunwayIter iter = m_vToRunway.begin();
	for (;iter !=m_vToRunway.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vToRunway.clear();
	for (iter = m_vInvalidToRunway.begin(); iter !=m_vInvalidToRunway.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vInvalidToRunway.clear();
}
void CSideStepApproachRunway::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nRunwayID);
	exportFile.getFile().writeInt(m_nMarkIndex);
	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vToRunway.size()));
	SideStepToRunwayIter iter = m_vToRunway.begin();
	for (; iter!=m_vToRunway.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}

void CSideStepApproachRunway::ImportData(CAirsideImportFile& importFile,int nFlightTypeID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nOldRunwayID = -1;
	importFile.getFile().getInteger(nOldRunwayID);
	m_nRunwayID = importFile.GetObjectNewID(nOldRunwayID);

	importFile.getFile().getInteger(m_nMarkIndex);

	SaveData(nFlightTypeID);

	importFile.getFile().getLine();

	int nToRunwayCount = 0;
	importFile.getFile().getInteger(nToRunwayCount);
	for (int i =0; i < nToRunwayCount; ++i)
	{
		CSideStepToRunway data;
		data.ImportData(importFile,m_nID);
	}
}

//--------------------CSideStepFlightType----------------------

CSideStepFlightType::CSideStepFlightType()
{
	m_nID = -1;
}

CSideStepFlightType::~CSideStepFlightType()
{
	RemoveAll();
}

void CSideStepFlightType::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	//adoRecordset.GetFieldValue(_T("FLTTYPE"),m_strFlightType);
	CString strFltType;
	adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);
	m_fltType.SetAirportDB(m_pAirportDB);
	m_fltType.setConstraintWithVersion(strFltType);
	ReadApproachRunwayData();
}
void CSideStepFlightType::ReadApproachRunwayData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_SIDESTEP_APPROACHRUNWAY\
					 WHERE (SISTFLTID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CSideStepApproachRunway* pApproachRunway = new CSideStepApproachRunway;
		pApproachRunway->ReadData(adoRecordset);

		m_vApproachRunway.push_back(pApproachRunway);
		adoRecordset.MoveNextData();
	}
}
void CSideStepFlightType::SaveData(int nProjID)
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_SIDESTEP_FLIGHTTYPE\
					 (PROJID, FLTTYPE)\
					 VALUES (%d,'%s')"),nProjID,szFltType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	SideStepApproachRunwayIter iter = m_vApproachRunway.begin();
	for (;iter !=m_vApproachRunway.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vInvalidApproachRunway.begin(); iter !=m_vInvalidApproachRunway.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vInvalidApproachRunway.clear();
}
void CSideStepFlightType::UpdateData()
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_SIDESTEP_FLIGHTTYPE\
					 SET FLTTYPE ='%s'\
					 WHERE (ID = %d)"),szFltType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	SideStepApproachRunwayIter iter = m_vApproachRunway.begin();

	for (;iter !=m_vApproachRunway.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vInvalidApproachRunway.begin(); iter !=m_vInvalidApproachRunway.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vInvalidApproachRunway.clear();
}
int CSideStepFlightType::GetApproachRunwayCount()
{
	return (int)m_vApproachRunway.size();
}

CSideStepApproachRunway* CSideStepFlightType::GetApproachRunwayItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vApproachRunway.size());
	return m_vApproachRunway[nIndex];
}

void CSideStepFlightType::AddApproachRunwayItem(CSideStepApproachRunway* pItem)
{
	m_vApproachRunway.push_back(pItem);
}
void CSideStepFlightType::DeleteApproachRunwayItem(CSideStepApproachRunway* pItem)
{
	SideStepApproachRunwayIter iter = 
		std::find(m_vApproachRunway.begin(),m_vApproachRunway.end(), pItem);
	if (iter == m_vApproachRunway.end())
		return;
	m_vInvalidApproachRunway.push_back(pItem);
	m_vApproachRunway.erase(iter);
}
void CSideStepFlightType::RemoveAll()
{
	SideStepApproachRunwayIter iter = m_vApproachRunway.begin();
	for (; iter != m_vApproachRunway.end(); iter++)
		delete (*iter);
	for (iter = m_vInvalidApproachRunway.begin(); iter != m_vInvalidApproachRunway.end(); iter++)
		delete (*iter);
}
void CSideStepFlightType::CopyData(CSideStepFlightType* pFlightType)///
{
	*this = *pFlightType;
}
void CSideStepFlightType::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_SIDESTEP_FLIGHTTYPE\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	SideStepApproachRunwayIter iter = m_vApproachRunway.begin();
	for (;iter !=m_vApproachRunway.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vApproachRunway.clear();
	for (iter = m_vInvalidApproachRunway.begin(); iter !=m_vInvalidApproachRunway.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vInvalidApproachRunway.clear();
}
void CSideStepFlightType::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	exportFile.getFile().writeField(szFltType);
	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vApproachRunway.size()));
	SideStepApproachRunwayIter iter = m_vApproachRunway.begin();
	for (; iter!=m_vApproachRunway.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}

void CSideStepFlightType::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	//importFile.getFile().getField(m_strFlightType.GetBuffer(1024),1024);
	CString strFltType;
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

	ImportSaveData(importFile.getNewProjectID(),strFltType);

	importFile.getFile().getLine();

	int nApproachRunwayCount = 0;
	importFile.getFile().getInteger(nApproachRunwayCount);
	for (int i =0; i < nApproachRunwayCount; ++i)
	{
		CSideStepApproachRunway data;
		data.ImportData(importFile,m_nID);
	}
}
void CSideStepFlightType::ImportSaveData(int nProjID,CString strFltType)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_SIDESTEP_FLIGHTTYPE\
					 (PROJID, FLTTYPE)\
					 VALUES (%d,'%s')"),nProjID,strFltType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

//------------CSideStepSpecification-----------------
CSideStepSpecification::CSideStepSpecification(int nProjID)
:m_nProjID(nProjID)
{
}

CSideStepSpecification::~CSideStepSpecification(void)
{
	RemoveAll();
}

void CSideStepSpecification::ReadData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT ID, FLTTYPE\
					 FROM IN_SIDESTEP_FLIGHTTYPE\
					 WHERE (PROJID = %d)"),m_nProjID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CSideStepFlightType* pFlightType = new CSideStepFlightType;
		pFlightType->SetAirportDB(m_pAirportDB);
		pFlightType->ReadData(adoRecordset);

		m_vFlightType.push_back(pFlightType);
		adoRecordset.MoveNextData();
	}
}

void CSideStepSpecification::SaveData()
{
	SideStepFlightTypeIter iter  = m_vFlightType.begin();
	SideStepFlightTypeIter iterEnd  = m_vFlightType.end();

	for (;iter != iterEnd;++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nProjID);
		else
			(*iter)->UpdateData();
	}
	iter = m_vInvalidFlightType.begin();
	for (;iter != m_vInvalidFlightType.end();++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vInvalidFlightType.clear();
}

int CSideStepSpecification::GetFlightTypeCount()
{
	return (int)m_vFlightType.size();
}

CSideStepFlightType* CSideStepSpecification::GetFlightTypeItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vFlightType.size());
	return m_vFlightType[nIndex];
}

void CSideStepSpecification::AddFlightTypeItem(CSideStepFlightType* pItem)
{
	m_vFlightType.push_back(pItem);
}
void CSideStepSpecification::DeleteFlightTypeItem(CSideStepFlightType* pItem)
{
	SideStepFlightTypeIter iter = 
		std::find(m_vFlightType.begin(),m_vFlightType.end(), pItem);
	if (iter == m_vFlightType.end())
		return;
	m_vInvalidFlightType.push_back(pItem);
	m_vFlightType.erase(iter);
}
void CSideStepSpecification::RemoveAll()
{
	SideStepFlightTypeIter iter = m_vFlightType.begin();
	for (; iter != m_vFlightType.end(); iter++)
		delete (*iter);
	for (iter = m_vInvalidFlightType.begin(); iter != m_vInvalidFlightType.end(); ++iter)
		delete *iter;
}
void CSideStepSpecification::ExportSideStepSpecification(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	CSideStepSpecification sideStepSpecification(exportFile.GetProjectID());
	sideStepSpecification.SetAirportDB(pAirportDB);
	sideStepSpecification.ReadData();
	sideStepSpecification.ExportData(exportFile);
	exportFile.getFile().endFile();
}
void CSideStepSpecification::ExportData(CAirsideExportFile& exportFile)
{		
	exportFile.getFile().writeField("SideStepSpecification");
	exportFile.getFile().writeLine();
	SideStepFlightTypeIter iter = m_vFlightType.begin();
	for (; iter!=m_vFlightType.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}
void CSideStepSpecification::ImportSideStepSpecification(CAirsideImportFile& importFile)
{
	while(!importFile.getFile().isBlank())
	{
		CSideStepFlightType sideStepFlightType;
		sideStepFlightType.ImportData(importFile);
	}
}
