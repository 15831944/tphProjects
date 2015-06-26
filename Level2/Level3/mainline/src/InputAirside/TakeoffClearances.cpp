#include "StdAfx.h"
#include "TakeoffClearances.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"

TakeoffClearances::TakeoffClearances(int nProjID)
:m_nProjID(nProjID)
{
	m_pTBTSameRunway = new 
		TakeoffBehindTakeoffSameRunwayOrCloseParalled(nProjID, 
		TakeoffBehindTakeoffSameRW);
	m_pTBTIntersect = new 
		TakeoffBehindTakeoffIntersectingConvergngRunway(nProjID,
		TakeoffBehindTakeoffIntersectRW);
	m_pTBLSameRunway = new
		TakeoffBehindLandingSameRunway(nProjID, TakeoffBehindLandingSameRW);
	m_pTBLIntersect = new
		TakeoffBehindLandingIntersectingConvergingRunway(nProjID,
		TakeoffBehindLandingIntersectRW);

}

TakeoffClearances::TakeoffClearances(void)
:m_nProjID(-1)
{
	m_pTBTSameRunway = new 
		TakeoffBehindTakeoffSameRunwayOrCloseParalled(m_nProjID, 
		TakeoffBehindTakeoffSameRW);
	m_pTBTIntersect = new 
		TakeoffBehindTakeoffIntersectingConvergngRunway(m_nProjID,
		TakeoffBehindTakeoffIntersectRW);
	m_pTBLSameRunway = new
		TakeoffBehindLandingSameRunway(m_nProjID, TakeoffBehindLandingSameRW);
	m_pTBLIntersect = new
		TakeoffBehindLandingIntersectingConvergingRunway(m_nProjID,
		TakeoffBehindLandingIntersectRW);

}

TakeoffClearances::~TakeoffClearances(void)
{
	delete m_pTBTSameRunway;	m_pTBTSameRunway = NULL;
	delete m_pTBTIntersect;		m_pTBTIntersect = NULL;
	delete m_pTBLSameRunway;	m_pTBLSameRunway = NULL;
	delete m_pTBLIntersect;		m_pTBLIntersect = NULL;
}

void TakeoffClearances::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM IN_TAKEOFFCLEARANCE\
		WHERE (PROJID = %d)"),m_nProjID);
	
	
	long nRecordAffect = 0;
	CADORecordset adoRecordset;
	
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAffect,adoRecordset);
	if(adoRecordset.IsEOF())
	{
		m_pTBTSameRunway->InsertDefaultValue();
//		m_pTBTSameRunway->SaveData();
		m_pTBTIntersect->InsertDefaultValue();
//		m_pTBTIntersect->SaveData();
		m_pTBLSameRunway->InsertDefaultValue();
//		m_pTBLSameRunway->SaveData();
		m_pTBLIntersect->InsertDefaultValue();
//		m_pTBLIntersect->SaveData();
		
		return;
	}
	else
	{
	
		while (!adoRecordset.IsEOF())
		{
			int nType =0;
			adoRecordset.GetFieldValue(_T("TYPE"),nType);

			if (nType == TakeoffBehindTakeoffSameRW)
			{
				m_pTBTSameRunway->ReadData(	adoRecordset);
			}
			else if (nType == TakeoffBehindTakeoffIntersectRW)
			{
				m_pTBTIntersect->ReadData(adoRecordset);
			}
			else if (nType == TakeoffBehindLandingSameRW)
			{
				m_pTBLSameRunway->ReadData(adoRecordset);
			}
			else if (nType == TakeoffBehindLandingIntersectRW)
			{
				m_pTBLIntersect->ReadData(adoRecordset);
			}

			adoRecordset.MoveNextData();
		}
	}

	//m_pTBTSameRunway->ReadData();
	//m_pTBTIntersect->ReadData();
	//m_pTBLSameRunway->ReadData();
	//m_pTBLIntersect->ReadData();
}

void TakeoffClearances::SaveData()
{
	m_pTBTSameRunway->SaveData();
	m_pTBTIntersect->SaveData();
	m_pTBLSameRunway->SaveData();
	m_pTBLIntersect->SaveData();
}

TakeoffClearanceItem* TakeoffClearances::GetTakeoffClearanceItem(TakeoffClearanceType emType)
{
	switch(emType) {
	case TakeoffBehindTakeoffSameRW:
		return m_pTBTSameRunway;
	case TakeoffBehindTakeoffIntersectRW:
		return m_pTBTIntersect;
	case TakeoffBehindLandingSameRW:
		return m_pTBLSameRunway;
	case TakeoffBehindLandingIntersectRW:
		return m_pTBLIntersect;
	default:
		ASSERT(FALSE);
	}
	return NULL;
}

void TakeoffClearances::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	if(m_pTBTSameRunway) delete m_pTBTSameRunway;
	m_pTBTSameRunway = new 
		TakeoffBehindTakeoffSameRunwayOrCloseParalled(m_nProjID, 
		TakeoffBehindTakeoffSameRW);
	if(m_pTBTIntersect)delete m_pTBTIntersect;
	m_pTBTIntersect = new 
		TakeoffBehindTakeoffIntersectingConvergngRunway(m_nProjID,
		TakeoffBehindTakeoffIntersectRW);
	if(m_pTBLSameRunway)delete m_pTBLSameRunway;
	m_pTBLSameRunway = new
		TakeoffBehindLandingSameRunway(m_nProjID, TakeoffBehindLandingSameRW);
	if(m_pTBLIntersect)delete m_pTBLIntersect;
	m_pTBLIntersect = new
		TakeoffBehindLandingIntersectingConvergingRunway(m_nProjID,
		TakeoffBehindLandingIntersectRW);

	m_pTBTSameRunway->ImportData(importFile);
	m_pTBTIntersect->ImportData(importFile);
	m_pTBLSameRunway->ImportData(importFile);
	m_pTBLIntersect->ImportData(importFile);
}

void TakeoffClearances::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("TakeoffClearances"));
	exportFile.getFile().writeLine();
	exportFile.getFile().writeInt(m_nProjID);
	if(m_pTBTSameRunway)m_pTBTSameRunway->ExportData(exportFile);
	if(m_pTBTIntersect)m_pTBTIntersect->ExportData(exportFile);
	if(m_pTBLSameRunway)m_pTBLSameRunway->ExportData(exportFile);
	if(m_pTBLIntersect)m_pTBLIntersect->ExportData(exportFile);
}

void TakeoffClearances::ImportTakeoffClearances(CAirsideImportFile& importFile)
{
	TakeoffClearances importTakeoffClearances;
	importTakeoffClearances.ImportData(importFile);
	importTakeoffClearances.SaveData();
}

void TakeoffClearances::ExportTakeoffClearances(CAirsideExportFile& exportFile)
{
	TakeoffClearances exportTakeoffClearances(exportFile.GetProjectID());
	exportTakeoffClearances.ReadData();
	exportTakeoffClearances.ExportData(exportFile);
	exportFile.getFile().endFile();
}
