#include "StdAfx.h"
#include ".\vehiclepoolsanddeployment.h"
#include "..\common\elaptime.h"
#include "..\common\FLT_CNST.H"
#include "..\Common\AirportDatabase.h"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\common\ProbabilityDistribution.h"
#include "..\inputs\probab.h"
#include "..\common\ProbDistManager.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "FollowMeCarServiceMeetingPoints.h"

//--------------------CVehicleServiceTimeRange--------------------
CVehicleServiceTimeRange::CVehicleServiceTimeRange()
{
	m_nID = -1;
	m_endTime.set(60*60*24-1);
	m_enumRegimenType = ServiceCloset;
	m_tLeaveTime.set(5*60);
	m_enumLeaveType = AfterAircraftArrival;
	m_nAircraftServiceNum = 10;
}

CVehicleServiceTimeRange::~CVehicleServiceTimeRange()
{

}

void CVehicleServiceTimeRange::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);

	long lStartTime = 0L;
	adoRecordset.GetFieldValue(_T("STARTTIME"),lStartTime);
	m_startTime.set(lStartTime);

	long lEndTime = 0L;
	adoRecordset.GetFieldValue(_T("ENDTIME"),lEndTime);
	m_endTime.set(lEndTime);

	int nRegimenType;
	adoRecordset.GetFieldValue(_T("REGIMENTYPE"),nRegimenType);
	m_enumRegimenType = (VehicleRegimenType)nRegimenType;

	long lLeaveTime = 0L;
	adoRecordset.GetFieldValue(_T("LEAVETIME"),lLeaveTime);
	m_tLeaveTime.set(lLeaveTime);

	int nLeaveType;
	adoRecordset.GetFieldValue(_T("LEAVETYPE"),nLeaveType);
	m_enumLeaveType = (VehicleLeaveType)nLeaveType;

	adoRecordset.GetFieldValue(_T("AIRCRAFTSERVICENUM"),m_nAircraftServiceNum);

	int distributionID;
	adoRecordset.GetFieldValue(_T("DISTRIBUTIONID"), distributionID);
	if(distributionID != 0)
	{
		m_proDistribution.ReadData(distributionID);
	}
	else
	{
		m_proDistribution.SaveData();
	}
}

void CVehicleServiceTimeRange::SaveData(int nFltTypeID)
{
	m_proDistribution.SaveData();
	CString strSQL, strTmp;
	strSQL = _T("INSERT INTO IN_VEHICLEPOOLS_TIMERANGE \
(FLIGHTTYPEID,STARTTIME,ENDTIME,REGIMENTYPE,LEAVETIME,LEAVETYPE,AIRCRAFTSERVICENUM,DISTRIBUTIONID) VALUES ");
	strTmp.Format("(%d,%d,%d,%d,%d,%d,%d,%d)", 
					nFltTypeID,
					m_startTime.asSeconds(),
					m_endTime.asSeconds(),
					(int)m_enumRegimenType,
					m_tLeaveTime.asSeconds(),
					(int)m_enumLeaveType,
					m_nAircraftServiceNum,
					m_proDistribution.getID());
	strSQL += strTmp;
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CVehicleServiceTimeRange::UpdateData()
{

	CString strSQL, strTmp;
	strSQL = _T("UPDATE IN_VEHICLEPOOLS_TIMERANGE SET ");
	strTmp.Format(_T("STARTTIME =%d, "), m_startTime.asSeconds());
	strSQL += strTmp;
	strTmp.Format(_T("ENDTIME =%d, "), m_endTime.asSeconds());
	strSQL += strTmp;
	strTmp.Format(_T("REGIMENTYPE =%d, "), (int)m_enumRegimenType);
	strSQL += strTmp;
	strTmp.Format(_T("LEAVETIME =%d, "), m_tLeaveTime.asSeconds());
	strSQL += strTmp;
	strTmp.Format(_T("LEAVETYPE =%d, "), (int)m_enumLeaveType);
	strSQL += strTmp;
	strTmp.Format(_T("AIRCRAFTSERVICENUM =%d, "), m_nAircraftServiceNum);
	strSQL += strTmp;
	strTmp.Format(_T("DISTRIBUTIONID=%d "), m_proDistribution.getID());
	strSQL += strTmp;
	strTmp.Format(_T("WHERE (ID=%d)"), m_nID);
	strSQL += strTmp;

	m_proDistribution.UpdateData();
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CVehicleServiceTimeRange::DeleteData() 
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_VEHICLEPOOLS_TIMERANGE\
					 WHERE (ID = %d)"),m_nID);

	m_proDistribution.RemoveData();
	CADODatabase::ExecuteSQLStatement(strSQL);
}


void CVehicleServiceTimeRange::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_startTime.asSeconds());
	exportFile.getFile().writeInt(m_endTime.asSeconds());
	exportFile.getFile().writeInt((int)m_enumRegimenType);
	exportFile.getFile().writeInt(m_tLeaveTime.asSeconds());
	exportFile.getFile().writeInt((int)m_enumLeaveType);
	exportFile.getFile().writeInt(m_nAircraftServiceNum);

	exportFile.getFile().writeLine();
}

void CVehicleServiceTimeRange::ImportData(CAirsideImportFile& importFile,int nFltTypeID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	long lstartTime = 0L;
	importFile.getFile().getInteger(lstartTime);
	m_startTime.set(lstartTime);

	long lendtime = 0L;
	importFile.getFile().getInteger(lendtime);
	m_endTime.set(lendtime);

	int nRegimenType = 0;
	importFile.getFile().getInteger(nRegimenType);
	m_enumRegimenType = (VehicleRegimenType)nRegimenType;
	
	long lleavetime = 0L;
	importFile.getFile().getInteger(lleavetime);
	m_tLeaveTime.set(lleavetime);
	
	int nLeaveType = 0;
	importFile.getFile().getInteger(nLeaveType);
	m_enumLeaveType = (VehicleLeaveType)nLeaveType;

	importFile.getFile().getInteger(m_nAircraftServiceNum);

	SaveData(nFltTypeID);

	importFile.getFile().getLine();
}

//---------------------------------------CVehicleServiceFlightType-----------------------------------------------------
CVehicleServiceFlightType::CVehicleServiceFlightType()
{
	m_nID = -1;
}

CVehicleServiceFlightType::~CVehicleServiceFlightType()
{
	RemoveAll();
}

void CVehicleServiceFlightType::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	CString strFltType;
	adoRecordset.GetFieldValue(_T("FLIGHTTYPE"),strFltType);
	if(strFltType != "")
	{
		//char szFltType[1024] = {0};
		//strcpy_s(szFltType,strFltType.GetLength(),strFltType);
		m_fltType.SetAirportDB(m_pAirportDB);
		m_fltType.setConstraintWithVersion(strFltType.GetBuffer());
	}
	ReadServiceTimeRangeData();
}

void CVehicleServiceFlightType::ReadServiceTimeRangeData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_VEHICLEPOOLS_TIMERANGE\
					 WHERE (FLIGHTTYPEID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CVehicleServiceTimeRange* pNewItem = new CVehicleServiceTimeRange;
		pNewItem->ReadData(adoRecordset);

		m_vTimeRangeList.push_back(pNewItem);
		adoRecordset.MoveNextData();
	}
}

void CVehicleServiceFlightType::SaveData(int nStandID)
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_VEHICLEPOOLS_FLIGHTTYPE\
					 (STANDID, FLIGHTTYPE)\
					 VALUES (%d,'%s')"),nStandID,szFltType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	ServiceTimeRangeIter iter = m_vTimeRangeList.begin();
	for (; iter != m_vTimeRangeList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelTimeRangeList.begin(); iter != m_vNeedDelTimeRangeList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelTimeRangeList.clear();
}

void CVehicleServiceFlightType::UpdateData()
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_VEHICLEPOOLS_FLIGHTTYPE\
					 SET FLIGHTTYPE ='%s'\
					 WHERE (ID = %d)"),szFltType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	ServiceTimeRangeIter iter = m_vTimeRangeList.begin();
	for (; iter != m_vTimeRangeList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelTimeRangeList.begin(); iter != m_vNeedDelTimeRangeList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelTimeRangeList.clear();
}

void CVehicleServiceFlightType::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_VEHICLEPOOLS_FLIGHTTYPE\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	ServiceTimeRangeIter iter = m_vTimeRangeList.begin();
	for (; iter != m_vTimeRangeList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vTimeRangeList.clear();
	for (iter = m_vNeedDelTimeRangeList.begin(); iter != m_vNeedDelTimeRangeList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelTimeRangeList.clear();
}

int CVehicleServiceFlightType::GetServiceTimeRangeCount()
{
	return (int)m_vTimeRangeList.size();
}

bool CVehicleServiceFlightType::FitTimeRange(const ElapsedTime& tTime)
{
	for ( int j =0; j < GetServiceTimeRangeCount(); j++)
	{
		CVehicleServiceTimeRange* pTimeRange = GetServiceTimeRangeItem(j);
		if ( pTimeRange->GetStartTime() <= tTime && tTime <=pTimeRange->GetEndTime() )
		{
			return true;
		}
	}
	return false;
}

CVehicleServiceTimeRange* CVehicleServiceFlightType::GetServiceTimeRangeItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vTimeRangeList.size());
	return m_vTimeRangeList[nIndex];
}

void CVehicleServiceFlightType::AddServiceTimeRangeItem(CVehicleServiceTimeRange* pItem)
{
	m_vTimeRangeList.push_back(pItem);
}

void CVehicleServiceFlightType::DelServiceTimeRangeItem(CVehicleServiceTimeRange* pItem)
{
	ServiceTimeRangeIter iter = 
		std::find(m_vTimeRangeList.begin(),m_vTimeRangeList.end(),pItem);
	if (iter == m_vTimeRangeList.end())
		return;
	m_vNeedDelTimeRangeList.push_back(pItem);
	m_vTimeRangeList.erase(iter);
}

void CVehicleServiceFlightType::RemoveAll()
{
	ServiceTimeRangeIter iter = m_vTimeRangeList.begin();
	for (; iter != m_vTimeRangeList.end(); ++iter)
		delete *iter;

	for (iter = m_vNeedDelTimeRangeList.begin(); iter != m_vNeedDelTimeRangeList.end(); ++iter)
		delete *iter;
}

void CVehicleServiceFlightType::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);

	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strFltType;
	strFltType = szFltType;
	exportFile.getFile().writeField(strFltType);

	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vTimeRangeList.size()));
	ServiceTimeRangeIter iter = m_vTimeRangeList.begin();
	for (; iter != m_vTimeRangeList.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}

void CVehicleServiceFlightType::ImportData(CAirsideImportFile& importFile,int nStandID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	CString strFltType;
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

	ImportSaveData(nStandID,strFltType);

	importFile.getFile().getLine();

	int nServiceTimeRangeCount = 0;
	importFile.getFile().getInteger(nServiceTimeRangeCount);
	for (int i =0; i < nServiceTimeRangeCount; ++i)
	{
		CVehicleServiceTimeRange data;
		data.ImportData(importFile,m_nID);
	}
}
void CVehicleServiceFlightType::ImportSaveData(int nStandID,CString strFltType)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_VEHICLEPOOLS_FLIGHTTYPE\
					 (STANDID, FLIGHTTYPE)\
					 VALUES (%d,'%s')"),nStandID,strFltType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

//---------------------------------------CVehicleServiceStandFamily-----------------------------------------------------
CVehicleServiceStandFamily::CVehicleServiceStandFamily()
{
	m_nID = -1;
	m_nStandfamilyID = -1;
	m_tLeavePoolTime = 300L;
}

CVehicleServiceStandFamily::~CVehicleServiceStandFamily()
{
	RemoveAll();
}

bool FlightTypeCompare(CVehicleServiceFlightType* fItem,CVehicleServiceFlightType* sItem)
{
	if (fItem->GetFltType().fits(sItem->GetFltType()))
		return false;
	else if(sItem->GetFltType().fits(fItem->GetFltType()))
		return true;

	return false;
}

void CVehicleServiceStandFamily::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("STANDFAMILYID"),m_nStandfamilyID);
	long lLeaveTime = 0L;
	adoRecordset.GetFieldValue(_T("LEAVEPOOLTIME"),lLeaveTime);
	m_tLeavePoolTime.set(lLeaveTime);

	ALTObjectGroup altObjGroup;
	altObjGroup.ReadData(m_nStandfamilyID);	
	m_objName = altObjGroup.getName();
	ReadServiceFlightTypeData();
	std::sort(m_vServiceFlightTypeList.begin(), m_vServiceFlightTypeList.end(),FlightTypeCompare);
}

void CVehicleServiceStandFamily::ReadServiceFlightTypeData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_VEHICLEPOOLS_FLIGHTTYPE\
					 WHERE (STANDID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CVehicleServiceFlightType* pNewItem = new CVehicleServiceFlightType;
		pNewItem->SetAirportDB(m_pAirportDB);
		pNewItem->ReadData(adoRecordset);

		m_vServiceFlightTypeList.push_back(pNewItem);
		adoRecordset.MoveNextData();
	}
}

void CVehicleServiceStandFamily::SaveData(int nVehiclePoolID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_VEHICLEPOOLS_STANDFAMILY\
					 (VEHICLEPOOLID, STANDFAMILYID,LEAVEPOOLTIME)\
					 VALUES (%d,%d,%d)"),nVehiclePoolID,m_nStandfamilyID,m_tLeavePoolTime.asSeconds());

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	ServiceFlightTypeIter iter = m_vServiceFlightTypeList.begin();
	for (; iter != m_vServiceFlightTypeList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelServiceFlightTypeList.begin(); iter != m_vNeedDelServiceFlightTypeList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelServiceFlightTypeList.clear();
}

void CVehicleServiceStandFamily::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_VEHICLEPOOLS_STANDFAMILY\
					 SET STANDFAMILYID =%d, LEAVEPOOLTIME = %d\
					 WHERE (ID = %d)"),m_nStandfamilyID,m_tLeavePoolTime.asSeconds(), m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	ServiceFlightTypeIter iter = m_vServiceFlightTypeList.begin();
	for (; iter != m_vServiceFlightTypeList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelServiceFlightTypeList.begin(); iter != m_vNeedDelServiceFlightTypeList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelServiceFlightTypeList.clear();
}

void CVehicleServiceStandFamily::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_VEHICLEPOOLS_STANDFAMILY\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	ServiceFlightTypeIter iter = m_vServiceFlightTypeList.begin();
	for (; iter != m_vServiceFlightTypeList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vServiceFlightTypeList.clear();
	for (iter = m_vNeedDelServiceFlightTypeList.begin(); iter != m_vNeedDelServiceFlightTypeList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelServiceFlightTypeList.clear();
}

int CVehicleServiceStandFamily::GetServiceFlightTypeCount()
{
	return (int)m_vServiceFlightTypeList.size();
}

CVehicleServiceFlightType* CVehicleServiceStandFamily::GetServiceFlightTypeItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vServiceFlightTypeList.size());
	return m_vServiceFlightTypeList[nIndex];
}

void CVehicleServiceStandFamily::AddServiceFlightTypeItem(CVehicleServiceFlightType* pItem)
{
	m_vServiceFlightTypeList.push_back(pItem);
}

void CVehicleServiceStandFamily::DelServiceFlightTypeItem(CVehicleServiceFlightType* pItem)
{
	ServiceFlightTypeIter iter = 
		std::find(m_vServiceFlightTypeList.begin(),m_vServiceFlightTypeList.end(),pItem);
	if (iter == m_vServiceFlightTypeList.end())
		return;
	m_vNeedDelServiceFlightTypeList.push_back(pItem);
	m_vServiceFlightTypeList.erase(iter);
}

bool CVehicleServiceStandFamily::Fit(const AirsideFlightDescStruct& _fltDesc, char mode, const ElapsedTime& tTime)
{
	int nFltCount = GetServiceFlightTypeCount();
	for (int i =0; i < nFltCount; i++)
	{
		CVehicleServiceFlightType* pServiceFlightType = GetServiceFlightTypeItem(i);
		if (pServiceFlightType)
		{
			FlightConstraint fltCnst = pServiceFlightType->GetFltType();
			if(	fltCnst.fits(_fltDesc,mode))
			{
				if (pServiceFlightType->FitTimeRange(tTime))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void CVehicleServiceStandFamily::RemoveAll()
{
	ServiceFlightTypeIter iter = m_vServiceFlightTypeList.begin();
	for (; iter != m_vServiceFlightTypeList.end(); ++iter)
		delete *iter;

	for (iter = m_vNeedDelServiceFlightTypeList.begin(); iter != m_vNeedDelServiceFlightTypeList.end(); ++iter)
		delete *iter;
}

void CVehicleServiceStandFamily::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);

	ALTObjectGroup altObjGroup;
	altObjGroup.ReadData(m_nStandfamilyID);	
	ALTObjectID objName = altObjGroup.getName();
	objName.writeALTObjectID(exportFile.getFile());

	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vServiceFlightTypeList.size()));
	ServiceFlightTypeIter iter = m_vServiceFlightTypeList.begin();
	for (; iter != m_vServiceFlightTypeList.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}

void CVehicleServiceStandFamily::ImportData(CAirsideImportFile& importFile,int nVehiclePoolID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	ALTObjectGroup altObjGroup;	
	altObjGroup.setType(ALT_STAND);
	ALTObjectID objName;
	objName.readALTObjectID(importFile.getFile());
	altObjGroup.setName(objName);

	try
	{
		CADODatabase::BeginTransaction();
		m_nStandfamilyID = altObjGroup.InsertData(importFile.getNewProjectID());
		CADODatabase::CommitTransaction();
	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	SaveData(nVehiclePoolID);

	importFile.getFile().getLine();

	int nServiceFlightTypeCount = 0;
	importFile.getFile().getInteger(nServiceFlightTypeCount);
	for (int i =0; i < nServiceFlightTypeCount; ++i)
	{
		CVehicleServiceFlightType data;
		data.ImportData(importFile,m_nID);
	}
}

//---------------------------------------CVehiclePool-----------------------------------------------------

CVehiclePool::CVehiclePool(bool bFollowMeCar)
:m_bFollowMeCar(bFollowMeCar)
{
	m_nID = -1;
	m_nVehicleQuantity = 20;
	m_nParkingLotID = -1;

	m_strDistName = _T("");
	m_strDistScreenPrint = _T("Constant: [0.00]");
	m_enumProbType = CONSTANT;
	m_strprintDist = _T("Constant:0.00");
	m_pDistribution = NULL;

	if (bFollowMeCar)
		m_pFollowMeCarServiceMeetingPoints = new FollowMeCarServiceMeetingPoints;
	else
		m_pFollowMeCarServiceMeetingPoints = NULL;
}

CVehiclePool::~CVehiclePool()
{
	RemoveAll();
	delete m_pDistribution;
	m_pDistribution = NULL;

	if (m_pFollowMeCarServiceMeetingPoints)
	{
		delete m_pFollowMeCarServiceMeetingPoints;
		m_pFollowMeCarServiceMeetingPoints = NULL;
	}
}

void CVehiclePool::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("POOLNAME"),m_strPoolName);
	adoRecordset.GetFieldValue(_T("QUANTITY"),m_nVehicleQuantity);
	adoRecordset.GetFieldValue(_T("PARKINGLOTID"),m_nParkingLotID);

	adoRecordset.GetFieldValue(_T("DISTNAME"),m_strDistName);
	adoRecordset.GetFieldValue(_T("DISTSCREENPRINT"),m_strDistScreenPrint);

	int nProbType = 0;
	adoRecordset.GetFieldValue(_T("PROBTYPE"),nProbType);
	m_enumProbType = (ProbTypes)nProbType;
	adoRecordset.GetFieldValue(_T("PRINTDIST"),m_strprintDist);

	if (!m_bFollowMeCar)
		ReadServiceStandFamilyData();
	else
	{
		m_pFollowMeCarServiceMeetingPoints->ReadData(m_nID);
	}
}

void CVehiclePool::ReadServiceStandFamilyData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_VEHICLEPOOLS_STANDFAMILY\
					 WHERE (VEHICLEPOOLID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CVehicleServiceStandFamily* pNewItem = new CVehicleServiceStandFamily;
		pNewItem->SetAirportDB(m_pAirportDB);
		pNewItem->ReadData(adoRecordset);

		m_vServiceStandFamilyList.push_back(pNewItem);
		adoRecordset.MoveNextData();
	}
}

void CVehiclePool::SaveData(int nVehicleTypePoolsID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_VEHICLEPOOLS_POOL\
					 (VEHICLETYPEPOOLSID, POOLNAME, QUANTITY, PARKINGLOTID,DISTNAME,DISTSCREENPRINT,PROBTYPE,PRINTDIST)\
					 VALUES (%d,'%s',%d,%d,'%s','%s',%d,'%s')"),nVehicleTypePoolsID,m_strPoolName,m_nVehicleQuantity,m_nParkingLotID,m_strDistName,m_strDistScreenPrint,(int)m_enumProbType,m_strprintDist);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	if (m_bFollowMeCar && m_pFollowMeCarServiceMeetingPoints != NULL)
	{
		m_pFollowMeCarServiceMeetingPoints->SaveData(m_nID);
		return;
	}

	ServiceStandFamilyIter iter = m_vServiceStandFamilyList.begin();
	for (; iter != m_vServiceStandFamilyList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelServiceStandFamilyList.begin(); iter != m_vNeedDelServiceStandFamilyList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelServiceStandFamilyList.clear();
}

void CVehiclePool::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_VEHICLEPOOLS_POOL\
					 SET POOLNAME ='%s', QUANTITY =%d, PARKINGLOTID =%d, DISTNAME ='%s', DISTSCREENPRINT ='%s', PROBTYPE =%d, PRINTDIST ='%s'\
					 WHERE (ID = %d)"),m_strPoolName,m_nVehicleQuantity,m_nParkingLotID,m_strDistName,m_strDistScreenPrint,(int)m_enumProbType,m_strprintDist,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	if (m_bFollowMeCar && m_pFollowMeCarServiceMeetingPoints != NULL)
	{
		m_pFollowMeCarServiceMeetingPoints->SaveData(m_nID);
		return;
	}

	ServiceStandFamilyIter iter = m_vServiceStandFamilyList.begin();
	for (; iter != m_vServiceStandFamilyList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelServiceStandFamilyList.begin(); iter != m_vNeedDelServiceStandFamilyList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelServiceStandFamilyList.clear();
}

void CVehiclePool::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_VEHICLEPOOLS_POOL\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	if (m_bFollowMeCar && m_pFollowMeCarServiceMeetingPoints != NULL)
	{
		m_pFollowMeCarServiceMeetingPoints->DeleteData();
		return;
	}

	ServiceStandFamilyIter iter = m_vServiceStandFamilyList.begin();
	for (; iter != m_vServiceStandFamilyList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vServiceStandFamilyList.clear();
	for (iter = m_vNeedDelServiceStandFamilyList.begin(); iter != m_vNeedDelServiceStandFamilyList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelServiceStandFamilyList.clear();
}

int CVehiclePool::GetServiceStandFamilyCount()
{
	return (int)m_vServiceStandFamilyList.size();
}

CVehicleServiceStandFamily* CVehiclePool::GetServiceStandFamilyItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vServiceStandFamilyList.size());
	return m_vServiceStandFamilyList[nIndex];
}

bool CVehiclePool::GetServiceFitStand(const ALTObjectID& standID, std::vector<CVehicleServiceStandFamily*>& standVector)
{
	for (int i = 0; i < GetServiceStandFamilyCount(); i++)
	{
		CVehicleServiceStandFamily* pStandFamily = GetServiceStandFamilyItem(i);
		if (standID.idFits(pStandFamily->GetStandALTObjectID()))
		{
			standVector.push_back(pStandFamily);
		}
	}
	return true;
}

void CVehiclePool::AddServiceStandFamilyItem(CVehicleServiceStandFamily* pItem)
{
	m_vServiceStandFamilyList.push_back(pItem);
}

void CVehiclePool::DelServiceStandFamilyItem(CVehicleServiceStandFamily* pItem)
{
	ServiceStandFamilyIter iter = 
		std::find(m_vServiceStandFamilyList.begin(),m_vServiceStandFamilyList.end(),pItem);
	if (iter == m_vServiceStandFamilyList.end())
		return;
	m_vNeedDelServiceStandFamilyList.push_back(pItem);
	m_vServiceStandFamilyList.erase(iter);
}

void CVehiclePool::RemoveAll()
{
	ServiceStandFamilyIter iter = m_vServiceStandFamilyList.begin();
	for (; iter != m_vServiceStandFamilyList.end(); ++iter)
		delete *iter;

	for (iter = m_vNeedDelServiceStandFamilyList.begin();iter != m_vNeedDelServiceStandFamilyList.end(); ++iter)
		delete *iter;
}

void CVehiclePool::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeField(m_strPoolName);
	exportFile.getFile().writeInt(m_nVehicleQuantity);
	exportFile.getFile().writeInt(m_nParkingLotID);

	exportFile.getFile().writeField(m_strDistName);
	//	exportFile.getFile().writeField(m_strDistScreenPrint);
	exportFile.getFile().writeInt((int)m_enumProbType);
	//	exportFile.getFile().writeField(m_strprintDist);

	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pAirportDB->getProbDistMan();
	CProbDistEntry* pPDEntry = NULL;
	int nCount = static_cast<int>(pProbDistMan->getCount());
	
	int i=0; 
	for(; i<nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if( strcmp( pPDEntry->m_csName, m_strDistName ) == 0 )
			break;
	}
	if(i>=nCount)
	{
		pProbDist = new ConstantDistribution;
		char szprintDist[1024] = {0};
		const char *endOfName = strstr (m_strprintDist, ":");
		strcpy_s(szprintDist,endOfName + 1);
		pProbDist->setDistribution(szprintDist);
		pProbDist->writeDistribution(exportFile.getFile());
		delete pProbDist;
	}
	else
	{
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		pProbDist->writeDistribution(exportFile.getFile());
	}

	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vServiceStandFamilyList.size()));
	ServiceStandFamilyIter iter = m_vServiceStandFamilyList.begin();
	for (; iter != m_vServiceStandFamilyList.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}

void CVehiclePool::ImportData(CAirsideImportFile& importFile,int nVehicleTypePoolsID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	importFile.getFile().getField(m_strPoolName.GetBuffer(1024),1024);
	importFile.getFile().getInteger(m_nVehicleQuantity);

	int nParkingLotID = 0;
	importFile.getFile().getInteger(nParkingLotID);
	m_nParkingLotID = importFile.GetObjectNewID(nParkingLotID);

	importFile.getFile().getField(m_strDistName.GetBuffer(1024),1024);

	int nProbType = 0;
	importFile.getFile().getInteger(nProbType);
	m_enumProbType = (ProbTypes)nProbType;

	ProbabilityDistribution* pProbDist;
	switch(m_enumProbType) 
	{
	case BERNOULLI:
		pProbDist = new BernoulliDistribution;
		break;
	case BETA:
		pProbDist = new BetaDistribution;
		break;
	case CONSTANT:
		pProbDist = new ConstantDistribution;
		break;
	case EMPIRICAL:
		pProbDist = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		pProbDist = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		pProbDist = new HistogramDistribution;
		break;
	case NORMAL:
		pProbDist = new NormalDistribution;
		break;
	case UNIFORM:
		pProbDist = new UniformDistribution;
		break;
	case WEIBULL:
		pProbDist = new WeibullDistribution;
		break;
	case GAMMA:
		pProbDist = new GammaDistribution;
		break;
	case ERLANG:
		pProbDist = new ErlangDistribution;
		break;
	case TRIANGLE:
		pProbDist = new TriangleDistribution;
		break;
	default:
		break;
	}
	pProbDist->readDistribution(importFile.getFile());
	char szBuffer[1024] = {0};
	pProbDist->screenPrint(szBuffer);
	m_strDistScreenPrint = szBuffer;

	pProbDist->printDistribution(szBuffer);
	m_strprintDist = szBuffer;
	delete pProbDist;

	SaveData(nVehicleTypePoolsID);

	importFile.getFile().getLine();

	int nServiceStandFamilyCount = 0;
	importFile.getFile().getInteger(nServiceStandFamilyCount);
	for (int i =0; i < nServiceStandFamilyCount; ++i)
	{
		CVehicleServiceStandFamily data;
		data.ImportData(importFile,m_nID);
	}
}
ProbabilityDistribution* CVehiclePool::GetProbDistribution()
{
	if(m_pDistribution)
	{
		delete m_pDistribution;
	}

	switch(m_enumProbType) 
	{
	case BERNOULLI:
		m_pDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}

	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strprintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pDistribution->setDistribution(szprintDist);
	return m_pDistribution;
}

FollowMeCarServiceMeetingPoints* CVehiclePool::GetFollowMeCarServiceMeetingPoints()
{
	return m_pFollowMeCarServiceMeetingPoints;
}
//--------------------------------------CVehicleTypePools------------------------------------------------------
CVehicleTypePools::CVehicleTypePools()
{
	m_nID = -1;
}

CVehicleTypePools::~CVehicleTypePools()
{
	RemoveAll();
}

CString CVehicleTypePools::GetVehicleTypeName()
{
	CString strName;
	CVehicleSpecificationItem* pVehicleSpecificationItem;
	size_t itemCount = m_pvehicleSpecifications->GetElementCount();	
	for (size_t i =0; i < itemCount; ++i)
	{		
		pVehicleSpecificationItem = m_pvehicleSpecifications->GetItem(i);		
		if(m_nVehicleTypeID == pVehicleSpecificationItem->GetID())
		{
			strName = pVehicleSpecificationItem->getName();
			break;
		}
	}
	return strName;
}

void CVehicleTypePools::SetVehicleSpecifications(CVehicleSpecifications* pvehicleSpecifications)
{
	m_pvehicleSpecifications = pvehicleSpecifications;
}

void CVehicleTypePools::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("VEHICLETYPEID"),m_nVehicleTypeID);
	ReadVehiclePoolData();
}

void CVehicleTypePools::ReadVehiclePoolData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_VEHICLEPOOLS_POOL\
					 WHERE (VEHICLETYPEPOOLSID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	bool bFollowMeCar = IsFollowMeCarPoolsData();

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CVehiclePool* pNewItem = new CVehiclePool(bFollowMeCar);
		pNewItem->SetAirportDB(m_pAirportDB);
		pNewItem->ReadData(adoRecordset);

		m_vPoolList.push_back(pNewItem);
		adoRecordset.MoveNextData();
	}
}

void CVehicleTypePools::SaveData(int nProjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_VEHICLEPOOLS_VEHICLETYPE\
					 (PROJID, VEHICLETYPEID)\
					 VALUES (%d,%d)"),nProjID,m_nVehicleTypeID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	VehiclePoolIter iter = m_vPoolList.begin();
	for (; iter != m_vPoolList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelPoolList.begin(); iter != m_vNeedDelPoolList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelPoolList.clear();
}

void CVehicleTypePools::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_VEHICLEPOOLS_VEHICLETYPE\
					 SET VEHICLETYPEID =%d\
					 WHERE (ID = %d)"),m_nVehicleTypeID,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	VehiclePoolIter iter = m_vPoolList.begin();
	for (; iter != m_vPoolList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelPoolList.begin(); iter != m_vNeedDelPoolList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelPoolList.clear();
}

void CVehicleTypePools::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_VEHICLEPOOLS_VEHICLETYPE\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	VehiclePoolIter iter = m_vPoolList.begin();
	for (; iter != m_vPoolList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vPoolList.clear();
	for (iter = m_vNeedDelPoolList.begin(); iter != m_vNeedDelPoolList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelPoolList.clear();
}

int CVehicleTypePools::GetVehiclePoolCount()
{
	return (int)m_vPoolList.size();
}

CVehiclePool* CVehicleTypePools::GetVehiclePoolItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vPoolList.size());
	return m_vPoolList[nIndex];
}

void CVehicleTypePools::AddVehiclePoolItem(CVehiclePool* pItem)
{
	m_vPoolList.push_back(pItem);
}

void CVehicleTypePools::DelVehiclePoolItem(CVehiclePool* pItem)
{
	VehiclePoolIter iter = 
		std::find(m_vPoolList.begin(),m_vPoolList.end(),pItem);
	if (iter == m_vPoolList.end())
		return;
	m_vNeedDelPoolList.push_back(pItem);
	m_vPoolList.erase(iter);
}

void CVehicleTypePools::RemoveAll()
{
	VehiclePoolIter iter = m_vPoolList.begin();
	for (; iter != m_vPoolList.end(); ++iter)
		delete *iter;

	for (iter = m_vNeedDelPoolList.begin(); iter != m_vNeedDelPoolList.end(); ++iter)
		delete *iter;
}

void CVehicleTypePools::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nVehicleTypeID);

	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vPoolList.size()));
	VehiclePoolIter iter = m_vPoolList.begin();
	for (; iter != m_vPoolList.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}

void CVehicleTypePools::ImportData(CAirsideImportFile& importFile)
{
	//int nOldID = -1;
	//importFile.getFile().getInteger(nOldID);

	//int nVehicleTypeID;
	//importFile.getFile().getInteger(nVehicleTypeID);
	//m_nVehicleTypeID = importFile.GetVehicleDataSpecificationNewID(nVehicleTypeID);

	//SaveData(importFile.getNewProjectID());

	//importFile.getFile().getLine();

	//int nVehiclePoolCount = 0;
	//importFile.getFile().getInteger(nVehiclePoolCount);
	//for (int i =0; i < nVehiclePoolCount; ++i)
	//{
	//	CVehiclePool data;
	//	data.ImportData(importFile,m_nID);
	//}
}

bool CVehicleTypePools::IsFollowMeCarPoolsData()
{
	enumVehicleBaseType eVihicleType = m_pvehicleSpecifications->GetVehicleItemByID(m_nVehicleTypeID)->getBaseType();
	if (eVihicleType == VehicleType_FollowMeCar)
		return true;

	return false;
}

bool CVehicleTypePools::IsTowTruckPoolsData()
{
	enumVehicleBaseType eVihicleType = m_pvehicleSpecifications->GetVehicleItemByID(m_nVehicleTypeID)->getBaseType();
	if (eVihicleType == VehicleType_TowTruck)
		return true;

	return false;
}
//----------------------------------------CVehiclePoolsAndDeployment----------------------------------------------------
CVehiclePoolsAndDeployment::CVehiclePoolsAndDeployment()
{
}

CVehiclePoolsAndDeployment::~CVehiclePoolsAndDeployment()
{
	RemoveAll();
}

void CVehiclePoolsAndDeployment::ReadData(int nProjID)
{
	InitVehicleType(nProjID);
	VehicleTypePoolsVector vRead;
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_VEHICLEPOOLS_VEHICLETYPE\
					 WHERE (PROJID = %d)"),nProjID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		int nID =-1;
		adoRecordset.GetFieldValue(_T("VEHICLETYPEID"),nID);

		VehicleTypePoolsIter iter = m_vVehicleTypePools.begin();
		for (;iter != m_vVehicleTypePools.end(); ++iter)
		{
			if((*iter)->GetVehicleTypeID() == nID)
			{
				(*iter)->SetAirportDB(m_pAirportDB);
				(*iter)->ReadData(adoRecordset);
				break;
			}
		}
	
		adoRecordset.MoveNextData();
	}
}

void CVehiclePoolsAndDeployment::SaveData(int nProjID)
{
	VehicleTypePoolsIter iter  = m_vVehicleTypePools.begin();
	for (; iter != m_vVehicleTypePools.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(nProjID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelVehicleTypePools.begin(); iter != m_vNeedDelVehicleTypePools.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelVehicleTypePools.clear();
}

void CVehiclePoolsAndDeployment::InitVehicleType(int nProjID)
{
	m_vehicleSpecifications.ReadData(nProjID);
	CVehicleSpecificationItem* pVehicleSpecificationItem;
	size_t itemCount = m_vehicleSpecifications.GetElementCount();	
	for (size_t i =0; i < itemCount; ++i)
	{		
		pVehicleSpecificationItem = m_vehicleSpecifications.GetItem(i);
		CVehicleTypePools* pNewItem = new CVehicleTypePools;
		pNewItem->SetVehicleTypeID(pVehicleSpecificationItem->GetID());
		pNewItem->SetVehicleSpecifications(&m_vehicleSpecifications);
		m_vVehicleTypePools.push_back(pNewItem);
	}
}

void CVehiclePoolsAndDeployment::InsertDataIntoStructure(VehicleTypePoolsVector& vRead)
{
	VehicleTypePoolsIter readiter = vRead.begin();
	while (readiter != vRead.end())
	{
		VehicleTypePoolsIter iter = m_vVehicleTypePools.begin();
		for (;iter != m_vVehicleTypePools.end(); ++iter)
		{
			if((*iter)->GetVehicleTypeID() == (*readiter)->GetVehicleTypeID())
			{
				delete *iter;
				*iter = *readiter;
				(*iter)->SetVehicleSpecifications(&m_vehicleSpecifications);
				break;
			}
		}
		vRead.erase(readiter);
		iter = vRead.begin();
	}
}

int CVehiclePoolsAndDeployment::GetVehicleTypePoolsCount()
{
	return (int)m_vVehicleTypePools.size();
}

CVehicleTypePools* CVehiclePoolsAndDeployment::GetVehicleTypePoolsItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vVehicleTypePools.size());
	return m_vVehicleTypePools[nIndex];
}

CVehicleTypePools* CVehiclePoolsAndDeployment::GetVehicleTypePools(int nVehicleTypeID)
{
	for (size_t i = 0; i < m_vVehicleTypePools.size(); i++)
	{
		if (nVehicleTypeID == m_vVehicleTypePools[i]->GetVehicleTypeID())
		{
			return m_vVehicleTypePools[i];
		}
	}
	return NULL;
}

void CVehiclePoolsAndDeployment::GetVehicleTypePoolsByName(VehicleTypePoolsVector& vPools,const CString& VehicleName)
{
	for (size_t i = 0; i< m_vVehicleTypePools.size(); ++ i)
	{
		int nVehicleTypeID =  m_vVehicleTypePools[i]->GetVehicleTypeID();

		CVehicleSpecificationItem * pItem = m_vehicleSpecifications.GetVehicleItemByID(nVehicleTypeID);
		if (pItem->getName() == VehicleName)
		{
			vPools.push_back(m_vVehicleTypePools[i]);
		}
	}
}
void CVehiclePoolsAndDeployment::AddVehicleTypePoolsItem(CVehicleTypePools* pItem)
{
	m_vVehicleTypePools.push_back(pItem);
}

void CVehiclePoolsAndDeployment::DelVehicleTypePoolsItem(CVehicleTypePools* pItem)
{
	VehicleTypePoolsIter iter = 
		std::find(m_vVehicleTypePools.begin(),m_vVehicleTypePools.end(),pItem);
	if (iter == m_vVehicleTypePools.end())
		return;
	m_vNeedDelVehicleTypePools.push_back(pItem);
	m_vVehicleTypePools.erase(iter);
}

void CVehiclePoolsAndDeployment::RemoveAll()
{
	VehicleTypePoolsIter iter = m_vVehicleTypePools.begin();
	for (; iter != m_vVehicleTypePools.end(); ++iter)
		delete *iter;

	for (iter = m_vNeedDelVehicleTypePools.begin(); iter != m_vNeedDelVehicleTypePools.end(); ++iter)
		delete *iter;
}

void CVehiclePoolsAndDeployment::ExportVehiclePoolsAndDeployment(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
	CVehiclePoolsAndDeployment vehiclePoolsAndDeployment;
	vehiclePoolsAndDeployment.SetAirportDB(pAirportDB);
	vehiclePoolsAndDeployment.ReadData(exportFile.GetProjectID());
	vehiclePoolsAndDeployment.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void CVehiclePoolsAndDeployment::ExportData(CAirsideExportFile& exportFile)
{		
	exportFile.getFile().writeField("VehiclePoolsAndDeployment");
	exportFile.getFile().writeLine();
	VehicleTypePoolsIter iter = m_vVehicleTypePools.begin();
	for (; iter != m_vVehicleTypePools.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}

void CVehiclePoolsAndDeployment::ImportVehiclePoolsAndDeployment(CAirsideImportFile& importFile)
{
	while(!importFile.getFile().isBlank())
	{
		CVehicleTypePools vehicleTypePools;
		vehicleTypePools.ImportData(importFile);
	}
}

enumVehicleBaseType CVehiclePoolsAndDeployment::GetVehicleTypeByID( int nID )
{
	CVehicleSpecificationItem* pItem = m_vehicleSpecifications.GetVehicleItemByID(nID);
	if (pItem != NULL)
	{
		return pItem->getBaseType();
	}

	return VehicleType_General;
}

const CVehicleSpecifications* CVehiclePoolsAndDeployment::GetVehicleSpecification()
{
	return &m_vehicleSpecifications;
}