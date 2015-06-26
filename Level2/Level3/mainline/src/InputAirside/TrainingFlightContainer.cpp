#include "StdAfx.h"
#include ".\trainingflightcontainer.h"
#include "Database\DBElementCollection_Impl.h"

template DBElementCollection<CTrainingFlightData>;

CTrainingFlightData::CTrainingFlightData()
:m_nNumFlight(0)
,m_bStartStand(true)
,m_nStopNum(0)
,m_bEndStand(true)
,m_nForceNum(0)
{
	m_timeRange.SetStartTime(0l);
	m_timeRange.SetEndTime(24*60*60l - 1l);
}

CTrainingFlightData::~CTrainingFlightData()
{

}

void CTrainingFlightData::SetName( const CString& sName )
{
	m_sName = sName;
}

const CString& CTrainingFlightData::GetName() const
{	
	return m_sName;
}

void CTrainingFlightData::SetCarrierCode( const CString& sCode )
{
	m_sCarrierCode = sCode;
}

const CString& CTrainingFlightData::GetCarrierCode() const
{
	return m_sCarrierCode;
}

void CTrainingFlightData::SetAcType( const CString& sAcType )
{
	m_sAcType = sAcType;
}

const CString& CTrainingFlightData::GetAcType() const
{
	return m_sAcType;
}

void CTrainingFlightData::SetNumFlight( int nNumFlight )
{
	m_nNumFlight = nNumFlight;
}

int CTrainingFlightData::GetNumFlight() const
{
	return m_nNumFlight;
}	

void CTrainingFlightData::SetStartPoint( const ALTObjectID& objID )
{
	m_startPointID = objID;
}

const ALTObjectID& CTrainingFlightData::GetStartPoint() const
{
	return m_startPointID;
}

void CTrainingFlightData::SetDistribution( CProbDistEntry* pPDEntry )
{
	m_distribution.SetProDistrubution(pPDEntry);
}

const CProDistrubution& CTrainingFlightData::GetDistribution() const
{
	return m_distribution;
}

void CTrainingFlightData::SetTimeRange( const ElapsedTime& startTime, const ElapsedTime& endTime )
{
	m_timeRange.SetStartTime(startTime);
	m_timeRange.SetEndTime(endTime);
}

const TimeRange& CTrainingFlightData::GetTimeRange() const
{
	return m_timeRange;
}

void CTrainingFlightData::SetEndPoint( const ALTObjectID& endPoint )
{
	m_endPointID = endPoint;
}

const ALTObjectID& CTrainingFlightData::GetEndPoint() const
{
	return m_endPointID;
}

void CTrainingFlightData::SetLowFlight( int nNumFlight )
{
	m_lowInterntion.m_nNum = nNumFlight;
}

int CTrainingFlightData::GetLowFlight() const
{
	return m_lowInterntion.m_nNum;
}

void CTrainingFlightData::SetLowValue( double dValue )
{
	m_lowInterntion.m_dValue = dValue;
}

double CTrainingFlightData::GetLowValue() const
{
	return m_lowInterntion.m_dValue;
}

void CTrainingFlightData::SetTouchFlight( int nNumFlight )
{
	m_touchInterntion.m_nNum = nNumFlight;
}

int CTrainingFlightData::GetTouchFlight() const
{
	return m_touchInterntion.m_nNum;
}

void CTrainingFlightData::SetTouchValue( double dValue )
{
	m_touchInterntion.m_dValue = dValue;
}

double CTrainingFlightData::GetTouchValue() const
{
	return m_touchInterntion.m_dValue;
}

void CTrainingFlightData::SetStopFlight( int nNumFlight )
{
	m_nStopNum = nNumFlight;
}

int CTrainingFlightData::GetStopFlight() const
{
	return m_nStopNum;
}

void CTrainingFlightData::SetStopValue( CProbDistEntry* pPDEntry )
{
	m_stopDist.SetProDistrubution(pPDEntry);
}

const CProDistrubution& CTrainingFlightData::GetStopValue() const
{
	return m_stopDist;
}

void CTrainingFlightData::InitFromDBRecordset( CADORecordset& recordset )
{
	if (recordset.IsEOF())
		return;
	
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("NAME"),m_sName);
	recordset.GetFieldValue(_T("CARRIERCODE"),m_sCarrierCode);
	recordset.GetFieldValue(_T("ACTYPE"),m_sAcType);
	recordset.GetFieldValue(_T("NUMFLIGHT"),m_nNumFlight);
	recordset.GetFieldValue(_T("FORCENUM"),m_nForceNum);
	int nStartStand = 0;
	recordset.GetFieldValue(_T("STARTSTAND"),nStartStand);
	m_bStartStand = nStartStand ? true : false;

	CString strStartPoint;
	recordset.GetFieldValue(_T("STARTPOINT"),strStartPoint);
	m_startPointID.FromString(strStartPoint);

	int nDisID = -1;
	recordset.GetFieldValue(_T("DISID"),nDisID);
	if (nDisID > -1)
	{
		m_distribution.ReadData(nDisID);
	}

	CString strLowInterntion;
	recordset.GetFieldValue(_T("LOW"),strLowInterntion);
	m_lowInterntion.ParseString(strLowInterntion);

	CString strTouchInternation;
	recordset.GetFieldValue(_T("TOUCH"),strTouchInternation);
	m_touchInterntion.ParseString(strTouchInternation);

	CString strStopInternation;
	recordset.GetFieldValue(_T("STOPNUM"),m_nStopNum);
	int nStopDist = -1;
	recordset.GetFieldValue(_T("STOPDISTID"),nStopDist);
	m_stopDist.ReadData(nStopDist);

	CString strTimeRange;
	recordset.GetFieldValue(_T("TIMERANGE"),strTimeRange);
	m_timeRange.ParseDatabaseString(strTimeRange);

	CString strEndStandPoint;
	recordset.GetFieldValue(_T("ENDPOINT"),strEndStandPoint);
	m_endPointID.FromString(strEndStandPoint);

	int nEndStand = 0;
	recordset.GetFieldValue(_T("ENDSTAND"),nEndStand);
	m_bEndStand = nEndStand ? true : false;
}

void CTrainingFlightData::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO IN_AIRSIDE_TRAINFLIGHT (NAME,CARRIERCODE,ACTYPE,NUMFLIGHT,STARTPOINT,FORCENUM,DISID,LOW,TOUCH,STOPNUM,STOPDISTID,TIMERANGE,STARTSTAND,ENDSTAND,ENDPOINT) VALUES \
					('%s','%s','%s',%d,'%s',%d,%d,'%s','%s',%d,%d,'%s',%d,%d,'%s')"),m_sName,m_sCarrierCode,m_sAcType,m_nNumFlight,m_startPointID.GetIDString(),m_nForceNum,m_distribution.getID(),\
					m_lowInterntion.getString(),m_touchInterntion.getString(),m_nStopNum,m_stopDist.getID(),m_timeRange.MakeDatabaseString(),m_bStartStand?true:false,m_bEndStand?true:false,m_endPointID.GetIDString());
}

void CTrainingFlightData::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE IN_AIRSIDE_TRAINFLIGHT SET NAME = '%s',CARRIERCODE = '%s',ACTYPE = '%s',NUMFLIGHT = %d,STARTPOINT = '%s',FORCENUM = %d, \
					 DISID = %d,LOW = '%s',TOUCH = '%s',STOPNUM = %d,STOPDISTID = %d,TIMERANGE = '%s',STARTSTAND = %d,ENDSTAND = %d,ENDPOINT = '%s' \
					 WHERE ID = %d"),m_sName,m_sCarrierCode,m_sAcType,m_nNumFlight,m_startPointID.GetIDString(),m_nForceNum,m_distribution.getID(),\
					 m_lowInterntion.getString(),m_touchInterntion.getString(),m_nStopNum,m_stopDist.getID(),m_timeRange.MakeDatabaseString(),\
					 m_bStartStand?true:false,m_bEndStand?true:false,m_endPointID.GetIDString(),m_nID);
}

void CTrainingFlightData::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM IN_AIRSIDE_TRAINFLIGHT WHERE ID = %d"),m_nID);
}

void CTrainingFlightData::GetSelectSQL( int nID,CString& strSQL ) const
{
	ASSERT(0);
}

void CTrainingFlightData::SetStartStand( bool bStand )
{
	m_bStartStand = bStand;
}

bool CTrainingFlightData::GetStartType() const
{
	return m_bStartStand;
}

void CTrainingFlightData::SetEndStand( bool bStand )
{
	m_bEndStand = bStand;
}

bool CTrainingFlightData::GetEndType() const
{
	return m_bEndStand;
}

void CTrainingFlightData::SaveData( int nParentID )
{
	CString strSQL;

	m_distribution.SaveData();
	m_stopDist.SaveData();

	if (m_nID < 0)
	{
		GetInsertSQL(nParentID,strSQL);

		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CTrainingFlightData::SetForceGo( int nNum )
{
	m_nForceNum = nNum;
}

int CTrainingFlightData::GetForceGo() const
{
	return m_nForceNum;
}

CString CTrainingFlightData::GetTimeRaneString() const
{
	CString strText;
	m_timeRange.GetString(strText);
	return strText;
}

CTrainingFlightContainer::CTrainingFlightContainer(void)
{

}

CTrainingFlightContainer::~CTrainingFlightContainer(void)
{

}

void CTrainingFlightContainer::GetSelectElementSQL( CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM IN_AIRSIDE_TRAINFLIGHT"));
}