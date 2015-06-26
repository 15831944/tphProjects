#include "StdAfx.h"
#include ".\flighttyperestriction.h"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"

CFlightTypeRestriction::CFlightTypeRestriction(void)
{
	m_nTaxiwayID = 0;
	m_nFromPos = 0;
	m_nToPos = 0;
	m_pAirportDatabase = NULL;
	//m_strFltType = _T("");
}

CFlightTypeRestriction::~CFlightTypeRestriction(void)
{
	
}

void CFlightTypeRestriction::setProjID(int nProjID)
{
	m_nProjID = nProjID;
}

int CFlightTypeRestriction::GetProjID() const
{
	return m_nProjID;
}
void CFlightTypeRestriction::SetFltType(const FlightConstraint& FltType)
{
	m_fltType = FltType;
}

void CFlightTypeRestriction::SetTaxiwayID(int nTaxiwayID)
{
	m_nTaxiwayID = nTaxiwayID;
}


int CFlightTypeRestriction::GetTaxiwayID() const
{
	return m_nTaxiwayID;
}

void CFlightTypeRestriction::SetFromPos(int nFromPos)
{
	m_nFromPos = nFromPos; 
}

int CFlightTypeRestriction::GetFromPos() const
{
	return m_nFromPos;
}

void CFlightTypeRestriction::SetToPos(int nToPos)
{
	m_nToPos = nToPos;
}

int  CFlightTypeRestriction::GetToPos() const
{
	return m_nToPos;
}

void CFlightTypeRestriction::SetAirportDatabase(CAirportDatabase* pAirportDB)
{
	m_pAirportDatabase = pAirportDB;
}

void CFlightTypeRestriction::InitFromDBRecordset(CADORecordset& recordset)
{
	CString strFltType;
	recordset.GetFieldValue(_T("ID"),m_nID);
//	recordset.GetFieldValue(_T("PROJID"), m_nProjID);
	recordset.GetFieldValue(_T("FLIGHTTYPE"),strFltType);
	recordset.GetFieldValue(_T("TAXIWAYID"),m_nTaxiwayID);
	recordset.GetFieldValue(_T("FROMID"),m_nFromPos);
	recordset.GetFieldValue(_T("TOID"),m_nToPos);

	ASSERT(m_pAirportDatabase);
	if (m_pAirportDatabase)
	{
		m_fltType.SetAirportDB(m_pAirportDatabase);
		m_fltType.setConstraintWithVersion(strFltType);
	}
}



void CFlightTypeRestriction::GetInsertSQL(int nParentID,CString& strSQL) const
{
	CString strFltType;
	m_fltType.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));

	strSQL.Format(_T("INSERT INTO IN_TAXIWAYFLIGHTTYPERESTRICTONS\
					 (PROJID, FLIGHTTYPE, TAXIWAYID, FROMID, TOID)\
					 VALUES (%d, '%s', %d, %d, %d)"),
					 nParentID,strFltType, m_nTaxiwayID, m_nFromPos, m_nToPos );
	
}
void  CFlightTypeRestriction::GetUpdateSQL(CString& strSQL) const
{
	CString strFltType;
	m_fltType.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));

	strSQL.Format(_T("UPDATE IN_TAXIWAYFLIGHTTYPERESTRICTONS\
					 SET FLIGHTTYPE = '%s', TAXIWAYID = %d, FROMID = %d, TOID = %d\
					 WHERE (ID = %d)"),strFltType,m_nTaxiwayID,m_nFromPos, m_nToPos, m_nID);

}
void CFlightTypeRestriction::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_TAXIWAYFLIGHTTYPERESTRICTONS\
					 WHERE (ID = %d)"),m_nID);
}
void CFlightTypeRestriction::GetSelectSQL(int nID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, FLIGHTTYPE, TAXIWAYID, FROMID, TOID\
					 FROM IN_TAXIWAYFLIGHTTYPERESTRICTONS\
					 WHERE (ID = %d)"), nID);

}
void CFlightTypeRestriction::ExportData(CAirsideExportFile& exportFile)
{
	CString strFltType;
	m_fltType.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));

	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	exportFile.getFile().writeField(strFltType);
	exportFile.getFile().writeInt(m_nTaxiwayID);
	exportFile.getFile().writeInt(m_nFromPos);
	exportFile.getFile().writeInt(m_nToPos);
	exportFile.getFile().writeLine();

}
void CFlightTypeRestriction::ImportData(CAirsideImportFile& importFile)
{

	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);

	CString strFltType = _T("");
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);
	setFlightType(strFltType);

	int nTaxiwayID = -1;
	importFile.getFile().getInteger(nTaxiwayID);
	m_nTaxiwayID = importFile.GetObjectNewID(nTaxiwayID);

	int nOldFromID = -1;
	importFile.getFile().getInteger(nOldFromID);
	m_nFromPos = importFile.GetIntersectionIndexMap(nOldFromID);

	int nOldToID = -1;
	importFile.getFile().getInteger(nOldToID);
	m_nToPos = importFile.GetIntersectionIndexMap(nOldToID);

	importFile.getFile().getLine();
	SaveData(m_nProjID);	
}


void CFlightTypeRestriction::setFlightType(const CString& strFltType)

{
	//Cm_strFltType = strFltType;
	ASSERT(m_pAirportDatabase);
	if (m_pAirportDatabase)
	{
		m_fltType.SetAirportDB(m_pAirportDatabase);
		m_fltType.setConstraintWithVersion(strFltType);
	}


}
