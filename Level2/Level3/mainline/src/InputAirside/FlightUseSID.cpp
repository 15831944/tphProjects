#include "StdAfx.h"
#include ".\flightusesid.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "Common/AirportDatabase.h"

CFlightUseSID::CFlightUseSID()
: DBElement()
, m_strFlightType("")
, m_SidID(0)
, m_pAirportDatabase(NULL)
{
}

CFlightUseSID::~CFlightUseSID(void)
{
}

void CFlightUseSID::SetFlightType(FlightConstraint& flightType)
{
	m_FlightType = flightType;
	flightType.WriteSyntaxStringWithVersion(m_strFlightType.GetBuffer(1024));
}

FlightConstraint CFlightUseSID::GetFlightType(void)
{

	return m_FlightType;
}

void CFlightUseSID::SetSidID(int nSidID)
{
	m_SidID = nSidID;
}

int CFlightUseSID::GetSidID(void)
{
	return m_SidID;
}

void CFlightUseSID::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("FLTTYPE"),m_strFlightType);
	recordset.GetFieldValue(_T("SIDID"),m_SidID);
	if (m_pAirportDatabase)
	{
		m_FlightType.SetAirportDB(m_pAirportDatabase);
		m_FlightType.setConstraintWithVersion(m_strFlightType);
	}


}

void CFlightUseSID::GetInsertSQL(int nProjID, int nRunwayID, int nMarking, ElapsedTime estFromTime, ElapsedTime estToTime, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_SIDASSIGNMENT\
		(PROJID, RUNWAYID, RUNWAYMARKING, FROMTIME, TOTIME, FLTTYPE, SIDID)\
		VALUES (%d, %d, %d, %d, %d, '%s', %d)"),\
											  nProjID,
											  nRunwayID,
											  nMarking,
											  estFromTime.asSeconds(),
											  estToTime.asSeconds(),
											  m_strFlightType,
											  m_SidID);
}

void CFlightUseSID::GetUpdateSQL(ElapsedTime estFromTime, ElapsedTime estToTime, CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_SIDASSIGNMENT\
		SET FROMTIME = %d, TOTIME = %d,\
		FLTTYPE ='%s', SIDID = %d\
		WHERE (ID =%d)"),\
					  estFromTime.asSeconds(),
					  estToTime.asSeconds(),
					  m_strFlightType,
					  m_SidID,
					  m_nID);
}

void CFlightUseSID::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_SIDASSIGNMENT\
		WHERE (ID = %d)"),m_nID);
}

void CFlightUseSID::ExportDataEx(CAirsideExportFile& exportFile, int nRunwayID, int nMarkingID, int nFromTime, int nToTime)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());

	exportFile.getFile().writeInt(nRunwayID);
	exportFile.getFile().writeInt(nMarkingID);
	exportFile.getFile().writeInt(nFromTime);
	exportFile.getFile().writeInt(nToTime);
	exportFile.getFile().writeField(m_strFlightType);
	exportFile.getFile().writeInt(m_SidID);

	exportFile.getFile().writeLine();
}

void CFlightUseSID::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldID);
	importFile.getFile().getInteger(nOldProjID);

	int nRunwayID;
	importFile.getFile().getInteger(nRunwayID);
	nRunwayID = importFile.GetObjectNewID(nRunwayID);

	int nMarkingID;
	importFile.getFile().getInteger(nMarkingID);

	int nFromTime;
	importFile.getFile().getInteger(nFromTime);
	int nToTime;
	importFile.getFile().getInteger(nToTime);

	importFile.getFile().getField(m_strFlightType.GetBuffer(1024),1024);
	importFile.getFile().getInteger(m_SidID);
	m_SidID = importFile.GetAirRouteNewIndex(m_SidID);

	importFile.getFile().getLine();

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_SIDASSIGNMENT\
					 (PROJID, RUNWAYID, RUNWAYMARKING, FROMTIME, TOTIME, FLTTYPE, SIDID)\
					 VALUES (%d, %d, %d, %d, %d, '%s', %d)"),\
					 importFile.getNewProjectID(),
					 nRunwayID,
					 nMarkingID,
					 nFromTime,
					 nToTime,
					 m_strFlightType,
					 m_SidID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CFlightUseSID::Save(int nParentID, int nRunwayID, int nMarking, ElapsedTime estFromTime, ElapsedTime estToTime)
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(nParentID, nRunwayID , nMarking, estFromTime , estToTime, strSQL);

		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(estFromTime, estToTime, strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}