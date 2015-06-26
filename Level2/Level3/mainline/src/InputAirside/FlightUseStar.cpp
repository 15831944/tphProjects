#include "StdAfx.h"
#include ".\flightusestar.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "Common/AirportDatabase.h"
//#include "../Common/FLT_CNST.H"

CFlightUseStar::CFlightUseStar()
 : DBElement()
 , m_strFlightType("")
 , m_StarID(0)
 , m_pAirportDatabase(NULL)
{
}

CFlightUseStar::~CFlightUseStar(void)
{
}

void CFlightUseStar::SetFlightType(FlightConstraint& flightType)
{
	m_FlightType = flightType;
	m_FlightType.WriteSyntaxStringWithVersion(m_strFlightType.GetBuffer(1024));
}

FlightConstraint CFlightUseStar::GetFlightType()
{

	return m_FlightType;
}

void CFlightUseStar::SetStarID(int nStarID)
{
	m_StarID = nStarID;
}

int CFlightUseStar::GetStarID(void)
{
	return m_StarID;
}

void CFlightUseStar::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("FLTTYPE"),m_strFlightType);
	recordset.GetFieldValue(_T("STARID"),m_StarID);

	if (m_pAirportDatabase)
	{
		m_FlightType.SetAirportDB(m_pAirportDatabase);
		m_FlightType.setConstraintWithVersion(m_strFlightType);
	}
}

void CFlightUseStar::GetInsertSQL(int nProjID, int nRunwayID, int nMarking, ElapsedTime estFromTime, ElapsedTime estToTime, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_STARASSIGNMENT\
					 (PROJID, RUNWAYID, RUNWAYMARKING, FROMTIME, TOTIME, FLTTYPE, STARID)\
					 VALUES (%d, %d, %d, %d, %d, '%s', %d)"),\
					 nProjID,
					 nRunwayID,
					 nMarking,
					 estFromTime.asSeconds(),
					 estToTime.asSeconds(),
					 m_strFlightType,
					 m_StarID);
}

void CFlightUseStar::GetUpdateSQL(ElapsedTime estFromTime, ElapsedTime estToTime, CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_STARASSIGNMENT\
					 SET FROMTIME = %d, TOTIME = %d,\
					 FLTTYPE ='%s', STARID = %d\
					 WHERE (ID =%d)"),\
					 estFromTime.asSeconds(),
					 estToTime.asSeconds(),
					 m_strFlightType,
					 m_StarID,
					 m_nID);
}

void CFlightUseStar::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_STARASSIGNMENT\
					 WHERE (ID = %d)"),m_nID);
}

void CFlightUseStar::ExportDataEx(CAirsideExportFile& exportFile, int nRunwayID, int nMarkingID, int nFromTime, int nToTime)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());

	exportFile.getFile().writeInt(nRunwayID);
	exportFile.getFile().writeInt(nMarkingID);
	exportFile.getFile().writeInt(nFromTime);
	exportFile.getFile().writeInt(nToTime);
	exportFile.getFile().writeField(m_strFlightType);
	exportFile.getFile().writeInt(m_StarID);

	exportFile.getFile().writeLine();
}

void CFlightUseStar::ImportData(CAirsideImportFile& importFile)
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

	int nOldStarID; 
	importFile.getFile().getInteger(nOldStarID);
	m_StarID = importFile.GetAirRouteNewIndex(nOldStarID);

	importFile.getFile().getLine();

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_STARASSIGNMENT\
					 (PROJID, RUNWAYID, RUNWAYMARKING, FROMTIME, TOTIME, FLTTYPE, STARID)\
					 VALUES (%d, %d, %d, %d, %d, '%s', %d)"),\
					 importFile.getNewProjectID(),
					 nRunwayID,
					 nMarkingID,
					 nFromTime,
					 nToTime,
					 m_strFlightType,
					 m_StarID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CFlightUseStar::Save(int nParentID, int nRunwayID, int nMarking, ElapsedTime estFromTime, ElapsedTime estToTime)
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