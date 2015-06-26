#include "stdafx.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "ARCUnitManager.h"

//CARCUnit
CARCUnitBaseManager::CARCUnitBaseManager(void)
{ 
	m_nProjID = -1;
	m_lUnitUserID = -1;
	m_nID = -1;
	m_emCurSelLengthUnit = ARCUnit_Length(-1);
	m_emCurSelWeightUnit = ARCUnit_Weight(-1);
	m_emCurSelTimeUnit = ARCUnit_Time(-1);
	m_emCurSelVelocityUnit = ARCUnit_Velocity(-1);
	m_emCurSelAccelerationUnit = ARCUnit_Acceleration(-1);
	m_lFlagUnitInUse = (long)ARCUnit_ALL_InUse;
}

CARCUnitBaseManager::~CARCUnitBaseManager(void)
{
}

ARCUnit_Length CARCUnitBaseManager::GetCurSelLengthUnit(void) const
{
	return (m_emCurSelLengthUnit);
}

bool CARCUnitBaseManager::SetCurSelLengthUnit(ARCUnit_Length aulCurSelLength)
{
	if((int)aulCurSelLength < 0 || aulCurSelLength >= AU_LENGTH_TOTAL_COUNT)return (false);
	m_emCurSelLengthUnit = aulCurSelLength;
	return (true);
}

ARCUnit_Weight CARCUnitBaseManager::GetCurSelWeightUnit(void) const
{
	return (m_emCurSelWeightUnit);
}

bool CARCUnitBaseManager::SetCurSelWeightUnit(ARCUnit_Weight auwCurSelWeight)
{
	if((int)auwCurSelWeight < 0 || auwCurSelWeight >= AU_WEIGHT_TOTAL_COUNT)return (false);
	m_emCurSelWeightUnit = auwCurSelWeight;
	return (true);
}

ARCUnit_Time CARCUnitBaseManager::GetCurSelTimeUnit(void) const
{
	return (m_emCurSelTimeUnit);
}

bool CARCUnitBaseManager::SetCurSelTimeUnit(ARCUnit_Time autCurSelTime)
{
	if((int)autCurSelTime < 0 || autCurSelTime >= AU_TIME_TOTAL_COUNT)return (false);
	m_emCurSelTimeUnit = autCurSelTime;
	return (true);
}

ARCUnit_Velocity CARCUnitBaseManager::GetCurSelVelocityUnit(void) const
{
	return (m_emCurSelVelocityUnit);
}

bool CARCUnitBaseManager::SetCurSelVelocityUnit(ARCUnit_Velocity auvCurSelVelocity)
{
	if((int)auvCurSelVelocity < 0 || auvCurSelVelocity >= AU_VELOCITY_TOTAL_COUNT)return (false);
	m_emCurSelVelocityUnit = auvCurSelVelocity;
	return (true);
}

ARCUnit_Acceleration CARCUnitBaseManager::GetCurSelAccelerationUnit(void)const
{
	return (m_emCurSelAccelerationUnit);
}

bool CARCUnitBaseManager::SetCurSelAccelerationUnit(ARCUnit_Acceleration auaCurSelAcceleration)
{
	if((int)auaCurSelAcceleration < 0 || auaCurSelAcceleration >= AU_ACCELERATION_TOTAL_COUNT)return (false);
	m_emCurSelAccelerationUnit = auaCurSelAcceleration;
	return (true);
}

void CARCUnitBaseManager::ReadData(int nProjID,long lUnitUserID,long lDefaultGlobalUnitUserID/* = UM_ID_GLOBAL_DEFAULT*/)
{
	m_nProjID = nProjID;
	m_lUnitUserID = lUnitUserID;

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM UNITMANAGERSETTING WHERE (PROJID = %d AND OWNERID = %d);"),m_nProjID,m_lUnitUserID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("OWNERID"),m_lUnitUserID); 
		int nValue = -1;
		adoRecordset.GetFieldValue(_T("LENGTHUNIT"),nValue);
		m_emCurSelLengthUnit = (ARCUnit_Length)nValue;
		adoRecordset.GetFieldValue(_T("WEIGHTUNIT"),nValue);
		m_emCurSelWeightUnit = (ARCUnit_Weight)nValue;
		adoRecordset.GetFieldValue(_T("TIMEUNIT"),nValue);
		m_emCurSelTimeUnit = (ARCUnit_Time)nValue;
		adoRecordset.GetFieldValue(_T("VELOCITYUNIT"),nValue);
		m_emCurSelVelocityUnit = (ARCUnit_Velocity)nValue;
		adoRecordset.GetFieldValue(_T("ACCELERATIONUNIT"),nValue);
		m_emCurSelAccelerationUnit = (ARCUnit_Acceleration)nValue;
	}
	else
	{
		//no unit manager ,so use the global unit as default.
		strSQL.Format(_T("SELECT * FROM UNITMANAGERSETTING WHERE (PROJID = %d AND OWNERID = %d);"),m_nProjID,lDefaultGlobalUnitUserID);
		CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
		if (!adoRecordset.IsEOF())
		{
			m_nID = -1;
			adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
			int nValue = -1;
			adoRecordset.GetFieldValue(_T("LENGTHUNIT"),nValue);
			m_emCurSelLengthUnit = (ARCUnit_Length)nValue;
			adoRecordset.GetFieldValue(_T("WEIGHTUNIT"),nValue);
			m_emCurSelWeightUnit = (ARCUnit_Weight)nValue;
			adoRecordset.GetFieldValue(_T("TIMEUNIT"),nValue);
			m_emCurSelTimeUnit = (ARCUnit_Time)nValue;
			adoRecordset.GetFieldValue(_T("VELOCITYUNIT"),nValue);
			m_emCurSelVelocityUnit = (ARCUnit_Velocity)nValue;
			adoRecordset.GetFieldValue(_T("ACCELERATIONUNIT"),nValue);
			m_emCurSelAccelerationUnit = (ARCUnit_Acceleration)nValue;
		}
		else
		{
			m_nID = -1;
			m_emCurSelLengthUnit   = AU_LENGTH_METER; //default:centimeter
			m_emCurSelWeightUnit   = AU_WEIGHT_KILOGRAM; //DEFAULT_DATABASE_WEIGHT_UNIT; //default:kilogram
			m_emCurSelTimeUnit     = DEFAULT_DATABASE_TIME_UNIT;//default:second
			m_emCurSelVelocityUnit = DEFAULT_DATABASE_VELOCITY_UNIT;//default:m/s
			m_emCurSelAccelerationUnit = DEFAULT_DATABASE_ACCELERATION_UNIT;//default:m/s^2
		}
	}
}

void CARCUnitBaseManager::SaveData(void)
{
	CString strSQL = _T("");
	if(m_nID == -1)
		strSQL.Format(_T("INSERT INTO UNITMANAGERSETTING (PROJID,OWNERID,LENGTHUNIT,WEIGHTUNIT,TIMEUNIT,VELOCITYUNIT,ACCELERATIONUNIT) VALUES (%d,%d,%d,%d,%d,%d,%d);"),m_nProjID,m_lUnitUserID,m_emCurSelLengthUnit,m_emCurSelWeightUnit,m_emCurSelTimeUnit,m_emCurSelVelocityUnit,m_emCurSelAccelerationUnit);
	else
		strSQL.Format(_T("UPDATE UNITMANAGERSETTING SET PROJID = %d ,OWNERID = %d ,LENGTHUNIT = %d ,WEIGHTUNIT = %d ,TIMEUNIT = %d , VELOCITYUNIT = %d , ACCELERATIONUNIT = %d WHERE (ID = %d); "),m_nProjID,m_lUnitUserID,m_emCurSelLengthUnit,m_emCurSelWeightUnit,m_emCurSelTimeUnit,m_emCurSelVelocityUnit,m_emCurSelAccelerationUnit,m_nID);
	 CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CARCUnitBaseManager::ExportARCUnit(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("ARC_UnitManager_Setting"));
	exportFile.getFile().writeLine();

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM UNITMANAGERSETTING WHERE (PROJID = %d);"),exportFile.GetProjectID());

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);	
	while (!adoRecordset.IsEOF()) {
		int nProjID = -1;
		long nOwnerID = -1;
		int nLenUnitIndex = -1;
		int nWeightUnitIndex = -1;
		int nTimeUnitIndex = -1;
		int nVelocityUnitIndex = -1;
		int nAccelerationUnitIndex = -1;

		adoRecordset.GetFieldValue(_T("PROJID"),nProjID);
		adoRecordset.GetFieldValue(_T("OWNERID"),nOwnerID);
		adoRecordset.GetFieldValue(_T("LENGTHUNIT"),nLenUnitIndex);
		adoRecordset.GetFieldValue(_T("WEIGHTUNIT"),nWeightUnitIndex);
		adoRecordset.GetFieldValue(_T("TIMEUNIT"),nTimeUnitIndex);
		adoRecordset.GetFieldValue(_T("VELOCITYUNIT"),nVelocityUnitIndex);
		adoRecordset.GetFieldValue(_T("ACCELERATIONUNIT"),nAccelerationUnitIndex);

		exportFile.getFile().writeInt(nProjID);
		exportFile.getFile().writeInt(nOwnerID);
		exportFile.getFile().writeInt(nLenUnitIndex);
		exportFile.getFile().writeInt(nWeightUnitIndex);
		exportFile.getFile().writeInt(nTimeUnitIndex);
		exportFile.getFile().writeInt(nVelocityUnitIndex);
		exportFile.getFile().writeInt(nAccelerationUnitIndex);
		exportFile.getFile().writeLine();

		adoRecordset.MoveNextData();
	}

	exportFile.getFile().endFile();
}

void CARCUnitBaseManager::ImportARCUnit(CAirsideImportFile& importFile)
{	
	int nProjIDNew = importFile.getNewProjectID();
	while(!importFile.getFile().isBlank())
	{
		int nProjID = -1;
		long nOwnerID = -1;
		int nLenUnitIndex = -1;
		int nWeightUnitIndex = -1;
		int nTimeUnitIndex = -1;
		int nVelocityUnitIndex = -1;
		int nAccelerationUnitIndex = -1;

		importFile.getFile().getInteger(nProjID);
		importFile.getFile().getInteger(nOwnerID);
		importFile.getFile().getInteger(nLenUnitIndex);
		importFile.getFile().getInteger(nWeightUnitIndex);
		importFile.getFile().getInteger(nTimeUnitIndex);
		importFile.getFile().getInteger(nVelocityUnitIndex);
		if(importFile.getVersion() > 1027)
			importFile.getFile().getInteger(nAccelerationUnitIndex);
		importFile.getFile().getLine(); 

		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO UNITMANAGERSETTING (PROJID,OWNERID,LENGTHUNIT,WEIGHTUNIT,TIMEUNIT,VELOCITYUNIT,ACCELERATIONUNIT) VALUES (%d,%d,%d,%d,%d,%d,%d);"),nProjIDNew,nOwnerID,nLenUnitIndex,nWeightUnitIndex,nTimeUnitIndex,nVelocityUnitIndex,nAccelerationUnitIndex);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CARCUnitBaseManager::ClearUnitManagerSettingInDB(int nProjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM UNITMANAGERSETTING WHERE (PROJID = %d);"),nProjID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CARCUnitBaseManager::SetUnitInUse(long lFlag)
{
	m_lFlagUnitInUse = lFlag;
}

long CARCUnitBaseManager::GetUnitInUse(void)
{
	return (m_lFlagUnitInUse);
}
CARCUnitManager* CARCUnitManager::m_Instance = NULL;
CARCUnitManager::CARCUnitManager():CARCUnitBaseManager()
{

}

CARCUnitManager& CARCUnitManager::GetInstance()
{
	if(CARCUnitManager::m_Instance == NULL)
		CARCUnitManager::m_Instance = new CARCUnitManager ;
	return *CARCUnitManager::m_Instance ;
}

CARCUnitManager::~CARCUnitManager( void )
{

}