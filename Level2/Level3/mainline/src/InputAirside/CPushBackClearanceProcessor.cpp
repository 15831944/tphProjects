#include "stdafx.h"
#include "CPushBackClearanceProcessor.h"
#include "../InputAirside/InputAirside.h"

CPushBackClearanceProcessor::CPushBackClearanceProcessor(CAirportDatabase *  _pAirportDatabase):m_pAirportDatabase(_pAirportDatabase)
{
	m_nProjID = -1;
	m_dDistancNoPushBackWithin = 3048.0;
	m_lTimeNoLeadWithin = 300L;
	m_lMaxOutboundFltToAssignedRunway = 100L;
	m_lMaxOutboundFltToAllRunway = 1000L;
}

CPushBackClearanceProcessor::CPushBackClearanceProcessor(int nProjID,CAirportDatabase *  _pAirportDatabase):
m_pAirportDatabase(_pAirportDatabase),
m_nProjID(nProjID),
m_dDistancNoPushBackWithin(3048.0),
m_lTimeNoLeadWithin(300L),
m_takeoffQueue(nProjID),
m_deicingQueue(nProjID),
m_slotTime(nProjID),
m_inboundFltNumLimiter(nProjID),
m_lMaxOutboundFltToAssignedRunway(100L),
m_lMaxOutboundFltToAllRunway(1000L)
{
}

CPushBackClearanceProcessor::~CPushBackClearanceProcessor(void)
{
}

BOOL CPushBackClearanceProcessor::ReadData(int nProjID)
{
	CString strSQL = _T("");
	long nCount = 0L;
	CADORecordset adoRecordset;

	strSQL = GetSelectScript(nProjID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adoRecordset);
	
	while (!adoRecordset.IsEOF()) 
	{
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("NOBACKTOSTWINDIS"),m_dDistancNoPushBackWithin);
		adoRecordset.GetFieldValue(_T("NOINTSCLEADINTIME"),m_lTimeNoLeadWithin);
		adoRecordset.GetFieldValue(_T("MAXOUTFLTTOASSIRW"),m_lMaxOutboundFltToAssignedRunway);
		adoRecordset.GetFieldValue(_T("MAXOUTFLTTOALLRW"),m_lMaxOutboundFltToAllRunway);

		adoRecordset.MoveNextData();
	}

	m_takeoffQueue.ReadData(nProjID);
	m_deicingQueue.ReadData(nProjID);
	m_slotTime.ReadData(nProjID);
	m_inboundFltNumLimiter.ReadData(nProjID);

	return (TRUE);
}

BOOL CPushBackClearanceProcessor::SaveData(void)
{
	CString strSQL = _T("");
	strSQL = GetDeleteScript();

	CADODatabase::ExecuteSQLStatement(strSQL);

	strSQL = GetInsertScript();
	CADODatabase::ExecuteSQLStatement(strSQL);
	
	m_takeoffQueue.SaveData();
	m_deicingQueue.SaveData();
	m_slotTime.SaveData();
	m_inboundFltNumLimiter.SaveData();

	return (TRUE);
}

void CPushBackClearanceProcessor::ImportData(CAirsideImportFile& importFile)
{
	m_nProjID = importFile.getNewProjectID();
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);
	importFile.getFile().getFloat(m_dDistancNoPushBackWithin);
	importFile.getFile().getInteger(m_lTimeNoLeadWithin);
	importFile.getFile().getInteger(m_lMaxOutboundFltToAssignedRunway);
	importFile.getFile().getInteger(m_lMaxOutboundFltToAllRunway);
	m_takeoffQueue.ImportData(importFile);
	m_deicingQueue.ImportData(importFile);
	m_slotTime.ImportData(importFile);
	m_inboundFltNumLimiter.ImportData(importFile);
	importFile.getFile().getLine();
}

void CPushBackClearanceProcessor::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeDouble(m_dDistancNoPushBackWithin);
	exportFile.getFile().writeInt(m_lTimeNoLeadWithin);
	exportFile.getFile().writeInt(m_lMaxOutboundFltToAssignedRunway);
	exportFile.getFile().writeInt(m_lMaxOutboundFltToAllRunway);
	m_takeoffQueue.ExportData(exportFile);
	m_deicingQueue.ExportData(exportFile);
	m_slotTime.ExportData(exportFile);
	m_inboundFltNumLimiter.ExportData(exportFile);
	exportFile.getFile().writeLine();
}

void CPushBackClearanceProcessor::ExportPushBackClearance(CAirsideExportFile& exportFile,CAirportDatabase *  _pAirportDatabase)
{
	CPushBackClearanceProcessor pushBackClearance(exportFile.GetProjectID(), _pAirportDatabase);
	pushBackClearance.ReadData(exportFile.GetProjectID());
	exportFile.getFile().writeField(_T("PUSHBACKCLEARANCECRITERIA"));
	exportFile.getFile().writeLine();
	pushBackClearance.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void CPushBackClearanceProcessor::ImportPushBackClearance(CAirsideImportFile& importFile)
{
	CPushBackClearanceProcessor pushBackClearance(0);
	while(!importFile.getFile().isBlank())
	{
		pushBackClearance.ImportData(importFile);
	}
	pushBackClearance.SaveData();
}

CString CPushBackClearanceProcessor::GetSelectScript(int nProjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM PUSHBACKCLEARANCECRITERIA WHERE PROJID = %d;"),nProjID);
	return (strSQL);
}

CString CPushBackClearanceProcessor::GetUpdateScript() const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE PUSHBACKCLEARANCECRITERIA SET NOBACKTOSTWINDIS = %f , NOINTSCLEADINTIME = %d ,MAXOUTFLTTOASSIRW = %d,MAXOUTFLTTOALLRW = %d \
				WHERE PROJID = %d ;"),m_dDistancNoPushBackWithin,m_lTimeNoLeadWithin,m_lMaxOutboundFltToAssignedRunway,m_lMaxOutboundFltToAllRunway,m_nProjID );
	return strSQL;
}

CString CPushBackClearanceProcessor::GetInsertScript() const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO PUSHBACKCLEARANCECRITERIA (PROJID ,NOBACKTOSTWINDIS , NOINTSCLEADINTIME ,MAXOUTFLTTOASSIRW ,MAXOUTFLTTOALLRW ) VALUES \
					 (%d , %f, %d, %d, %d);"),m_nProjID,m_dDistancNoPushBackWithin,m_lTimeNoLeadWithin,m_lMaxOutboundFltToAssignedRunway,m_lMaxOutboundFltToAllRunway );
	return strSQL;
}

CString CPushBackClearanceProcessor::GetDeleteScript() const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM PUSHBACKCLEARANCECRITERIA WHERE PROJID = %d;"),m_nProjID);
	return (strSQL);
}

int CPushBackClearanceProcessor::GetTakeoffQueueCount(void)
{
	return (m_takeoffQueue.GetMapCount());
}

int CPushBackClearanceProcessor::GetTakeoffQueueRunwayID(int nIndex)
{
	return (m_takeoffQueue.GetMapAt(nIndex).first);
}

int CPushBackClearanceProcessor::GetTakeoffQueueRunwayMarkingIndex(int nIndex)
{
	return (m_takeoffQueue.GetRunwayMarkingIndex(nIndex));
}

int CPushBackClearanceProcessor::GetTakeoffQueueLengthAtRunway(int nIndex)
{
	return (m_takeoffQueue.GetMapAt(nIndex).second);
}


int CPushBackClearanceProcessor::GetDeicingQueueCount(void)
{
	return (m_deicingQueue.GetMapCount());
}

ALTObjectID CPushBackClearanceProcessor::GetDeicingQueueDeicePad(int nIndex)
{
	CString strName = m_deicingQueue.GetMapAt(nIndex).first ;
	ALTObjectID objName(strName.GetBuffer());
	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{ 
	//	std::vector<ALTObject> vObject;
	//	ALTObject::GetObjectList(ALT_DEICEBAY, *iterAirportID, vObject);

	//	int nObjectCount = (int) vObject.size();
	//	for (int i = 0; i < nObjectCount; ++i)
	//	{
	//		ALTObject& altObject = vObject.at(i);			
	//		altObject.getObjName(objName);
	//		if(m_deicingQueue.GetMapAt(nIndex).first == objName.GetIDString())
	//			return (objName);
	//	}
	//} 

	return (objName);
}

int CPushBackClearanceProcessor::GetDeicingQueueLengthAtDeicePad(int nIndex)
{
	return (m_deicingQueue.GetMapAt(nIndex).second);
}


int CPushBackClearanceProcessor::GetSlotTimeCount(void)
{
	return (m_slotTime.GetMapCount());
}

FlightConstraint CPushBackClearanceProcessor::GetSlotTimeFltType(int nIndex)
{
	FlightConstraint flightType;
	if(!m_pAirportDatabase)		 
		return(flightType);

	flightType.SetAirportDB(m_pAirportDatabase);
	flightType.setConstraintWithVersion(m_slotTime.GetMapAt(nIndex).first);
	return (flightType);
}

int CPushBackClearanceProcessor::GetFltTypeMaxSlotTime(int nIndex)
{
	return (m_slotTime.GetMapAt(nIndex).second);
}


int CPushBackClearanceProcessor::GetCountOfStandForInboundFlt(void)
{
	return (m_inboundFltNumLimiter.GetMapCount());
}

ALTObjectID CPushBackClearanceProcessor::GetStandForInboundFlt(int nIndex)
{
	CString strName = m_inboundFltNumLimiter.GetMapAt(nIndex).first;
	ALTObjectID objName(strName.GetBuffer());
	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{ 
	//	std::vector<ALTObject> vObject;
	//	ALTObject::GetObjectList(ALT_STAND, *iterAirportID, vObject);

	//	int nObjectCount = (int) vObject.size();
	//	for (int i = 0; i < nObjectCount; ++i)
	//	{
	//		ALTObject& altObject = vObject.at(i);			
	//		altObject.getObjName(objName);
	//		if(m_inboundFltNumLimiter.GetMapAt(nIndex).first == objName.GetIDString())
	//			return (objName);
	//	}
	//}

	return (objName);
}

int CPushBackClearanceProcessor::GetInboundFltNumberAtStand(int nIndex)
{
	return (m_inboundFltNumLimiter.GetMapAt(nIndex).second);
}


double CPushBackClearanceProcessor::GetDistancNoPushBackWithin(void)const
{
	return (m_dDistancNoPushBackWithin);
}

void CPushBackClearanceProcessor::SetDistancNoPushBackWithin(double dDistancNoPushBackWithin)
{
	m_dDistancNoPushBackWithin = dDistancNoPushBackWithin;
}


long CPushBackClearanceProcessor::GetTimeNoLeadWithin(void)const
{
	return (m_lTimeNoLeadWithin);
}

void CPushBackClearanceProcessor::SetTimeNoLeadWithin(long lTimeNoLeadWithin)
{
	m_lTimeNoLeadWithin = lTimeNoLeadWithin;
}


long CPushBackClearanceProcessor::GetMaxOutboundFltToAssignedRunway(void)const
{
	return (m_lMaxOutboundFltToAssignedRunway);
}

void CPushBackClearanceProcessor::SetMaxOutboundFltToAssignedRunway(long lMaxOutboundFltToAssignedRunway)
{
	m_lMaxOutboundFltToAssignedRunway = lMaxOutboundFltToAssignedRunway;
}


long CPushBackClearanceProcessor::GetMaxOutboundFltToAllRunway(void)const
{
	return (m_lMaxOutboundFltToAllRunway);
}

void CPushBackClearanceProcessor::SetMaxOutboundFltToAllRunway(long lMaxOutboundFltToAllRunway)
{
	m_lMaxOutboundFltToAllRunway = lMaxOutboundFltToAllRunway;
}

void CPushBackClearanceProcessor::AddTakeoffQueueRunway(int nRunwayID,int nRunwayMarkingIndex)
{
	m_takeoffQueue.InsertMapKey(nRunwayID,nRunwayMarkingIndex);
}

void CPushBackClearanceProcessor::SetTakeoffQueueRunwayOfQueueLength(int iIndex,int iQueueLen)
{
	m_takeoffQueue.SetMapValue(iIndex,iQueueLen);
}

void CPushBackClearanceProcessor::AddDeicingQueueDeicePadName(CString strDeicePadName)
{
	m_deicingQueue.InsertMapKey(strDeicePadName);
}

void CPushBackClearanceProcessor::SetDeicingQueueDeicePadOfQueueLength(int iIndex,int iQueueLen)
{
	m_deicingQueue.SetMapValue(iIndex,iQueueLen);
}

void CPushBackClearanceProcessor::AddSlotTimeFltTypeName(CString strFltType)
{
	m_slotTime.InsertMapKey(strFltType);
}

void CPushBackClearanceProcessor::SetMaxSlotTimeOfFltType(int iIndex,int iMaxTime)
{
	m_slotTime.SetMapValue(iIndex,iMaxTime);
}

void CPushBackClearanceProcessor::AddStandForFltInbound(CString strStandName)
{
	m_inboundFltNumLimiter.InsertMapKey(strStandName);
}

void CPushBackClearanceProcessor::SetFltInboundNumberAtStand(int iIndex,int iFltNumber)
{
	m_inboundFltNumLimiter.SetMapValue(iIndex,iFltNumber);
}

void CPushBackClearanceProcessor::DeleteTakeoffQueueRunway(int iIndex)
{
	m_takeoffQueue.DeleteMap(iIndex);
}

void CPushBackClearanceProcessor::DeleteDeicingQueueDeicePad(int iIndex)
{
	m_deicingQueue.DeleteMap(iIndex);
}

void CPushBackClearanceProcessor::DeleteSlotTimeFltType(int iIndex)
{
	m_slotTime.DeleteMap(iIndex);
}

void CPushBackClearanceProcessor::DeleteStandForFltInbound(int iIndex)
{
	m_inboundFltNumLimiter.DeleteMap(iIndex);
}

////////////////////////////////////////////////////////////////////////////

CString CPushBackClearanceProcessor::GetDeicingQueueDeicePadName(int iIndex)
{
	return (m_deicingQueue.GetMapAt(iIndex).first);
}

CString CPushBackClearanceProcessor::GetSlotTimeFltTypeName(int iIndex)
{
	return (m_slotTime.GetMapAt(iIndex).first);
}

CString CPushBackClearanceProcessor::GetNameOfStandForFltInbound(int iIndex)
{
	return (m_inboundFltNumLimiter.GetMapAt(iIndex).first);
}

//////////////////////////////////////////////////////////////////////////

CTakeoffQueueProcessor::CTakeoffQueueProcessor(void)
{
	m_nProjID = -1;
}

CTakeoffQueueProcessor::CTakeoffQueueProcessor(int nProjID):m_nProjID(nProjID)
{
}

CTakeoffQueueProcessor::~CTakeoffQueueProcessor(void)
{
	m_takeoffQueue.clear();
	m_mapRunwayID2MarkingIndex.clear();
}


std::pair<int,int> CTakeoffQueueProcessor::GetMapAt(int iIndex)
{//if(iIndex < 0 || iIndex >= m_takeoffQueue.count()),i don't thank this necessary,return value is all right.
	std::pair<int,int> pairMap(-1,-1);
	int i = 0;
	for (std::map <int,int>::const_iterator itrMap = m_takeoffQueue.begin();\
		itrMap != m_takeoffQueue.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			pairMap.first = itrMap->first;
			pairMap.second = itrMap->second;
			break;
		}
	}
	return pairMap;
}
int CTakeoffQueueProcessor::GetRunwayMarkingIndex(int iIndex)
{ 
	int i = 0;
	for (std::map <int,int>::const_iterator itrMap = m_mapRunwayID2MarkingIndex.begin();\
		itrMap != m_mapRunwayID2MarkingIndex.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			return (itrMap->second);
		}
	}
	return (-1);
}

int CTakeoffQueueProcessor::GetMapCount(void)
{
	return ((int)m_takeoffQueue.size());
}

void CTakeoffQueueProcessor::InsertMapKey(int nRunwayID,int nRunwayMarkingIndex)
{
	m_takeoffQueue.insert(std::pair<int,int>(std::make_pair(nRunwayID,1000)));
	m_mapRunwayID2MarkingIndex.insert(std::pair<int,int>(std::make_pair(nRunwayID,nRunwayMarkingIndex)));
}

void CTakeoffQueueProcessor::DeleteMap(int iIndex)
{//if(iIndex < 0 || iIndex >= m_takeoffQueue.count()),i don't thank this necessary,return value is all right.
	int i = 0;
	std::map <int,int>::iterator itrMap;
	for (itrMap = m_takeoffQueue.begin();\
		itrMap != m_takeoffQueue.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			m_takeoffQueue.erase(itrMap);
			break;
		}
	}

	i = 0;
	for (itrMap = m_mapRunwayID2MarkingIndex.begin();\
		itrMap != m_mapRunwayID2MarkingIndex.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			m_mapRunwayID2MarkingIndex.erase(itrMap);
			break;
		}
	}
	return;
}

void CTakeoffQueueProcessor::SetMapValue(int iIndex,int iQueueLen)
{//if(iIndex < 0 || iIndex >= m_takeoffQueue.count()),i don't thank this necessary,return value is all right.
	int i = 0;
	for (std::map <int,int>::iterator itrMap = m_takeoffQueue.begin();\
		itrMap != m_takeoffQueue.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			itrMap->second = iQueueLen;
			break;
		}
	}
	return;
}

BOOL CTakeoffQueueProcessor::ReadData(int nProjID)
{ 
	CString strSQL = _T("");
	long nCount = 0L;
	CADORecordset adoRecordset;

	strSQL = GetSelectScript(nProjID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adoRecordset);
	int nRunwayID = -1;
	int nRunwayMarkingIndex = -1;
	int iMaxValue = 0;
	std::pair<int,int> pairMap;

	m_takeoffQueue.clear();
	m_mapRunwayID2MarkingIndex.clear();
	while (!adoRecordset.IsEOF()) 
	{
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("RUNWAYID"),nRunwayID);
		adoRecordset.GetFieldValue(_T("RUNWAYMARKINGINDEX"),nRunwayMarkingIndex);
		adoRecordset.GetFieldValue(_T("FLTQUEUEMAXLEN"),iMaxValue);
		pairMap = std::make_pair(nRunwayID,iMaxValue);
		m_takeoffQueue.insert(pairMap);
		pairMap = std::make_pair(nRunwayID,nRunwayMarkingIndex);
		m_mapRunwayID2MarkingIndex.insert(pairMap);

		adoRecordset.MoveNextData();
	}

	return (TRUE);
}

BOOL CTakeoffQueueProcessor::SaveData(void)
{
	CString strSQL = _T("");
	strSQL = GetDeleteScript();
	CADODatabase::ExecuteSQLStatement(strSQL);

	std::map<int,int>::const_iterator itrMarkingMap = m_mapRunwayID2MarkingIndex.begin();
	for (std::map<int ,int>::const_iterator itrMap = m_takeoffQueue.begin();itrMap != m_takeoffQueue.end();++itrMap,++itrMarkingMap)
	{
		strSQL = GetInsertScript(itrMap,itrMarkingMap);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	return (TRUE);
}

void CTakeoffQueueProcessor::ImportData(CAirsideImportFile& importFile)
{
	m_nProjID = importFile.getNewProjectID();
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);
	int nCount = 0;
	std::pair<int,int> pairMap;
	int nRunwayID = -1;
	int nRunwayMarkingIndex = -1;
	int iMaxValue = 0;
	importFile.getFile().getInteger(nCount);
	m_takeoffQueue.clear();
	m_mapRunwayID2MarkingIndex.clear();
	for (int i = 0; i < nCount;i++) 
	{	
		int nOldRunwayID = -1;
		importFile.getFile().getInteger(nOldRunwayID); 
		nRunwayID = importFile.GetObjectNewID(nOldRunwayID);
		importFile.getFile().getInteger(iMaxValue);
		importFile.getFile().getInteger(nRunwayMarkingIndex);
		pairMap = std::make_pair(nRunwayID,iMaxValue);
		m_takeoffQueue.insert(pairMap);
		pairMap = std::make_pair(nRunwayID,nRunwayMarkingIndex);
		m_mapRunwayID2MarkingIndex.insert(pairMap);
	}
	importFile.getFile().getLine();
}

void CTakeoffQueueProcessor::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	int nCount = (int) m_takeoffQueue.size();
	exportFile.getFile().writeInt(nCount);
	std::map<int,int>::const_iterator itrMarkingMap = m_mapRunwayID2MarkingIndex.begin();
	for (std::map<int ,int>::const_iterator itrMap = m_takeoffQueue.begin();itrMap != m_takeoffQueue.end();++itrMap,++itrMarkingMap)
	{
		exportFile.getFile().writeInt(itrMap->first);
		exportFile.getFile().writeInt(itrMap->second);
		exportFile.getFile().writeInt(itrMarkingMap->second);
	}
	exportFile.getFile().writeLine();
}

CString CTakeoffQueueProcessor::GetSelectScript(int nProjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TAKEOFFQUEUE WHERE PROJID = %d;"),nProjID);
	return (strSQL);
}

CString CTakeoffQueueProcessor::GetInsertScript(std::map<int,int>::const_iterator citrMapFirst,std::map<int,int>::const_iterator citrMapSecond) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO TAKEOFFQUEUE (PROJID ,RUNWAYID ,RUNWAYMARKINGINDEX, FLTQUEUEMAXLEN ) VALUES \
					 (%d, %d,%d, %d);"),m_nProjID,citrMapFirst->first,citrMapSecond->second,citrMapFirst->second);
	return strSQL;
}

CString CTakeoffQueueProcessor::GetDeleteScript() const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TAKEOFFQUEUE WHERE PROJID = %d;"),m_nProjID);
	return (strSQL);
}



CDeicingQueueProcessor::CDeicingQueueProcessor(void)
{
	m_nProjID = -1;
}

CDeicingQueueProcessor::CDeicingQueueProcessor(int nProjID):m_nProjID(nProjID)
{
}

CDeicingQueueProcessor::~CDeicingQueueProcessor(void)
{
	m_deicingQueue.clear();
}

BOOL CDeicingQueueProcessor::ReadData(int nProjID)
{ 
	CString strSQL = _T("");
	long nCount = 0L;
	CADORecordset adoRecordset;

	strSQL = GetSelectScript(nProjID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adoRecordset);	
	CString strName = _T("");
	int iMaxValue = 0;
	std::pair<CString,int> pairMap;

	m_deicingQueue.clear();
	while (!adoRecordset.IsEOF()) 
	{
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("DEICEPADNAME"),strName);
		adoRecordset.GetFieldValue(_T("DEICEPADQUEUEMAXLEN"),iMaxValue);
		pairMap = std::make_pair(strName,iMaxValue);
		m_deicingQueue.insert(pairMap);

		adoRecordset.MoveNextData();
	}

	return (TRUE);
}

BOOL CDeicingQueueProcessor::SaveData(void)
{
	CString strSQL = _T("");
	strSQL = GetDeleteScript();
	CADODatabase::ExecuteSQLStatement(strSQL);

	for (std::map<CString ,int>::const_iterator itrMap = m_deicingQueue.begin();itrMap != m_deicingQueue.end();++itrMap)
	{
		strSQL = GetInsertScript(itrMap->first,itrMap->second);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	return (TRUE);
}

void CDeicingQueueProcessor::ImportData(CAirsideImportFile& importFile)
{
	m_nProjID = importFile.getNewProjectID();
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);
	int nCount = 0;
	std::pair<CString,int> pairMap;
	char szRunwayName[1024] = {0};
	CString strName = _T("");
	int iMaxValue = 0;
	importFile.getFile().getInteger(nCount);
	m_deicingQueue.clear();
	for (int i = 0; i < nCount;i++) 
	{	
		importFile.getFile().getInteger(iMaxValue);
		importFile.getFile().getField(szRunwayName,iMaxValue);
		szRunwayName[iMaxValue+1] = 0;
		strName = szRunwayName;
		importFile.getFile().getInteger(iMaxValue);
		pairMap = std::make_pair(strName,iMaxValue);
		m_deicingQueue.insert(pairMap);
	}
	importFile.getFile().getLine();
}

void CDeicingQueueProcessor::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	int nCount = (int) m_deicingQueue.size();
	exportFile.getFile().writeInt(nCount);
	for (std::map<CString ,int>::const_iterator itrMap = m_deicingQueue.begin();itrMap != m_deicingQueue.end();++itrMap)
	{
		exportFile.getFile().writeInt((itrMap->first).GetLength());
		exportFile.getFile().writeField(itrMap->first);
		exportFile.getFile().writeInt(itrMap->second);
	}
	exportFile.getFile().writeLine();
}

std::pair<CString,int> CDeicingQueueProcessor::GetMapAt(int iIndex)
{//if(iIndex < 0 || iIndex >= m_deicingQueue.count()),i don't thank this necessary,return value is all right.
	std::pair<CString,int> pairMap(_T(""),-1);
	int i = 0;
	for (std::map <CString,int>::const_iterator itrMap = m_deicingQueue.begin();\
		itrMap != m_deicingQueue.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			pairMap.first = itrMap->first;
			pairMap.second = itrMap->second;
			break;
		}
	}
	return pairMap;
}

int CDeicingQueueProcessor::GetMapCount(void)
{
	return ((int)m_deicingQueue.size());
}

void CDeicingQueueProcessor::InsertMapKey(CString strDeicePad)
{
	m_deicingQueue.insert(std::pair<CString,int>(std::make_pair(strDeicePad,1000)));
}

void CDeicingQueueProcessor::DeleteMap(int iIndex)
{//if(iIndex < 0 || iIndex >= m_deicingQueue.count()),i don't thank this necessary,return value is all right.
	int i = 0;
	for (std::map <CString,int>::iterator itrMap = m_deicingQueue.begin();\
		itrMap != m_deicingQueue.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			m_deicingQueue.erase(itrMap);
			break;
		}
	}
	return;
}

void CDeicingQueueProcessor::SetMapValue(int iIndex,int iQueueLen)
{//if(iIndex < 0 || iIndex >= m_deicingQueue.count()),i don't thank this necessary,return value is all right.
	int i = 0;
	for (std::map <CString,int>::iterator itrMap = m_deicingQueue.begin();\
		itrMap != m_deicingQueue.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			itrMap->second = iQueueLen;
			break;
		}
	}
	return;
}

CString CDeicingQueueProcessor::GetSelectScript(int nProjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM DEICINGQUEUE WHERE PROJID = %d;"),nProjID);
	return (strSQL);
}

CString CDeicingQueueProcessor::GetInsertScript(CString strFirst,int iSecond) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO DEICINGQUEUE (PROJID ,DEICEPADNAME , DEICEPADQUEUEMAXLEN ) VALUES \
					 (%d, '%s', %d);"),m_nProjID,strFirst,iSecond);
	return strSQL;
}

CString CDeicingQueueProcessor::GetDeleteScript() const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM DEICINGQUEUE WHERE PROJID = %d;"),m_nProjID);
	return (strSQL);
}



CSlotTimeProcessor::CSlotTimeProcessor(void)
{
	m_nProjID = -1;
}

CSlotTimeProcessor::CSlotTimeProcessor(int nProjID):m_nProjID(nProjID)
{
}

CSlotTimeProcessor::~CSlotTimeProcessor(void)
{
	m_slotTime.clear();	 
}

BOOL CSlotTimeProcessor::ReadData(int nProjID)
{ 
	CString strSQL = _T("");
	long nCount = 0L;
	CADORecordset adoRecordset;

	strSQL = GetSelectScript(nProjID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adoRecordset);	
	CString strName = _T("");
	int iMaxValue = 0;
	std::pair<CString,int> pairMap; 

	m_slotTime.clear();	 
	while (!adoRecordset.IsEOF()) 
	{
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("FLIGHTTYPE"),strName);
		adoRecordset.GetFieldValue(_T("MAXSLOTTIME"),iMaxValue);  
		pairMap = std::make_pair(strName,iMaxValue);
		m_slotTime.insert(pairMap);

		adoRecordset.MoveNextData();
	}

	return (TRUE);
}

BOOL CSlotTimeProcessor::SaveData(void)
{
	CString strSQL = _T("");
	strSQL = GetDeleteScript();
	CADODatabase::ExecuteSQLStatement(strSQL);

	for (std::map<CString ,int>::const_iterator itrMap = m_slotTime.begin();itrMap != m_slotTime.end();++itrMap)
	{
		strSQL = GetInsertScript(itrMap->first,itrMap->second);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	return (TRUE);
}

void CSlotTimeProcessor::ImportData(CAirsideImportFile& importFile)
{
	m_nProjID = importFile.getNewProjectID();
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);
	int nCount = 0;
	std::pair<CString,int> pairMap;
	char szRunwayName[1024] = {0};
	CString strName = _T("");
	int iMaxValue = 0;
	importFile.getFile().getInteger(nCount);
	m_slotTime.clear();
	for (int i = 0; i < nCount;i++) 
	{	
		importFile.getFile().getInteger(iMaxValue);
		importFile.getFile().getField(szRunwayName,iMaxValue);
		szRunwayName[iMaxValue+1] = 0;
		strName = szRunwayName;
		importFile.getFile().getInteger(iMaxValue);
		pairMap = std::make_pair(strName,iMaxValue);
		m_slotTime.insert(pairMap);
	}
	importFile.getFile().getLine();
}

void CSlotTimeProcessor::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	int nCount = (int) m_slotTime.size();
	exportFile.getFile().writeInt(nCount);
	for (std::map<CString ,int>::const_iterator itrMap = m_slotTime.begin();itrMap != m_slotTime.end();++itrMap)
	{
		exportFile.getFile().writeInt((itrMap->first).GetLength());
		exportFile.getFile().writeField(itrMap->first);
		exportFile.getFile().writeInt(itrMap->second);
	}
	exportFile.getFile().writeLine();
}

std::pair<CString,int> CSlotTimeProcessor::GetMapAt(int iIndex)
{//if(iIndex < 0 || iIndex >= m_slotTime.count()),i don't thank this necessary,return value is all right.
	std::pair<CString,int> pairMap(_T(""),-1);
	int i = 0;
	for (std::map <CString,int>::const_iterator itrMap = m_slotTime.begin();\
		itrMap != m_slotTime.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			pairMap.first = itrMap->first;
			pairMap.second = itrMap->second;
			break;
		}
	}
	return pairMap;
}

int CSlotTimeProcessor::GetMapCount(void)
{
	return ((int)m_slotTime.size());
}

void CSlotTimeProcessor::InsertMapKey(CString strFltType)
{
	m_slotTime.insert(std::pair<CString,int>(std::make_pair(strFltType,60)));
}

void CSlotTimeProcessor::DeleteMap(int iIndex)
{//if(iIndex < 0 || iIndex >= m_slotTime.count()),i don't thank this necessary,return value is all right.
	int i = 0;
	for (std::map <CString,int>::iterator itrMap = m_slotTime.begin();\
		itrMap != m_slotTime.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			m_slotTime.erase(itrMap);
			break;
		}
	}
	return;
}


void CSlotTimeProcessor::SetMapValue(int iIndex,int iMaxTime)
{//if(iIndex < 0 || iIndex >= m_slotTime.count()),i don't thank this necessary,return value is all right.
	int i = 0;
	for (std::map <CString,int>::iterator itrMap = m_slotTime.begin();\
		itrMap != m_slotTime.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			itrMap->second = iMaxTime;
			break;
		}
	}
	return;
}

CString CSlotTimeProcessor::GetSelectScript(int nProjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM SLOTTIME WHERE PROJID = %d;"),nProjID);
	return (strSQL);
}

CString CSlotTimeProcessor::GetInsertScript(CString strFirst,int iSecond) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO SLOTTIME (PROJID ,FLIGHTTYPE , MAXSLOTTIME ) VALUES \
					 (%d, '%s', %d);"),m_nProjID,strFirst,iSecond);
	return strSQL;
}

CString CSlotTimeProcessor::GetDeleteScript() const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM SLOTTIME WHERE PROJID = %d;"),m_nProjID);
	return (strSQL);
}


CInboundFltNumLimiter::CInboundFltNumLimiter(void)
{
	m_nProjID = -1;
}

CInboundFltNumLimiter::CInboundFltNumLimiter(int nProjID):m_nProjID(nProjID)
{
}

CInboundFltNumLimiter::~CInboundFltNumLimiter(void)
{
	m_inboundFltNumLimit.clear();
}

BOOL CInboundFltNumLimiter::ReadData(int nProjID)
{ 
	CString strSQL = _T("");
	long nCount = 0L;
	CADORecordset adoRecordset;

	strSQL = GetSelectScript(nProjID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adoRecordset);	
	CString strName = _T("");
	int iMaxValue = 0;
	std::pair<CString,int> pairMap;

	m_inboundFltNumLimit.clear();
	while (!adoRecordset.IsEOF()) 
	{
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("STANDNAME"),strName);
		adoRecordset.GetFieldValue(_T("MAXINFLTTOSTAND"),iMaxValue);
		pairMap = std::make_pair(strName,iMaxValue);
		m_inboundFltNumLimit.insert(pairMap);

		adoRecordset.MoveNextData();
	}

	return (TRUE);
}

BOOL CInboundFltNumLimiter::SaveData(void)
{
	CString strSQL = _T("");
	strSQL = GetDeleteScript();
	CADODatabase::ExecuteSQLStatement(strSQL);

	for (std::map<CString ,int>::const_iterator itrMap = m_inboundFltNumLimit.begin();itrMap != m_inboundFltNumLimit.end();++itrMap)
	{
		strSQL = GetInsertScript(itrMap->first,itrMap->second);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	return (TRUE);
}

void CInboundFltNumLimiter::ImportData(CAirsideImportFile& importFile)
{
	m_nProjID = importFile.getNewProjectID();
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);
	int nCount = 0;
	std::pair<CString,int> pairMap;
	char szRunwayName[1024] = {0};
	CString strName = _T("");
	int iMaxValue = 0;
	importFile.getFile().getInteger(nCount);
	m_inboundFltNumLimit.clear();
	for (int i = 0; i < nCount;i++) 
	{	
		importFile.getFile().getInteger(iMaxValue);
		importFile.getFile().getField(szRunwayName,iMaxValue);
		szRunwayName[iMaxValue+1] = 0;
		strName = szRunwayName;
		importFile.getFile().getInteger(iMaxValue);
		pairMap = std::make_pair(strName,iMaxValue);
		m_inboundFltNumLimit.insert(pairMap);
	}
	importFile.getFile().getLine();
}

void CInboundFltNumLimiter::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	int nCount = (int) m_inboundFltNumLimit.size();
	exportFile.getFile().writeInt(nCount);
	for (std::map<CString ,int>::const_iterator itrMap = m_inboundFltNumLimit.begin();itrMap != m_inboundFltNumLimit.end();++itrMap)
	{
		exportFile.getFile().writeInt((itrMap->first).GetLength());
		exportFile.getFile().writeField(itrMap->first);
		exportFile.getFile().writeInt(itrMap->second);
	}
	exportFile.getFile().writeLine();
}

std::pair<CString,int> CInboundFltNumLimiter::GetMapAt(int iIndex)
{//if(iIndex < 0 || iIndex >= m_inboundFltNumLimit.count()),i don't thank this necessary,return value is all right.
	std::pair<CString,int> pairMap(_T(""),-1);
	int i = 0;
	for (std::map <CString,int>::const_iterator itrMap = m_inboundFltNumLimit.begin();\
		itrMap != m_inboundFltNumLimit.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			pairMap.first = itrMap->first;
			pairMap.second = itrMap->second;
			break;
		}
	}
	return pairMap;
}

int CInboundFltNumLimiter::GetMapCount(void)
{
	return ((int)m_inboundFltNumLimit.size());
}

CString CInboundFltNumLimiter::GetSelectScript(int nProjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM MAXINBOUNDFLT WHERE PROJID = %d;"),nProjID);
	return (strSQL);
}

CString CInboundFltNumLimiter::GetInsertScript(CString strFirst,int iSecond) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO MAXINBOUNDFLT (PROJID ,STANDNAME , MAXINFLTTOSTAND ) VALUES \
					 (%d, '%s', %d);"),m_nProjID,strFirst,iSecond);
	return strSQL;
}

CString CInboundFltNumLimiter::GetDeleteScript() const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM MAXINBOUNDFLT WHERE PROJID = %d;"),m_nProjID);
	return (strSQL);
}

void CInboundFltNumLimiter::InsertMapKey(CString strStandName)
{
	m_inboundFltNumLimit.insert(std::pair<CString,int>(std::make_pair(strStandName,1000)));
}

void CInboundFltNumLimiter::DeleteMap(int iIndex)
{//if(iIndex < 0 || iIndex >= m_inboundFltNumLimit.count()),i don't thank this necessary,return value is all right.
	int i = 0;
	for (std::map <CString,int>::iterator itrMap = m_inboundFltNumLimit.begin();\
		itrMap != m_inboundFltNumLimit.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			m_inboundFltNumLimit.erase(itrMap);
			break;
		}
	}
	return;
}


void CInboundFltNumLimiter::SetMapValue(int iIndex,int iFltNumber)
{//if(iIndex < 0 || iIndex >= m_inboundFltNumLimit.count()),i don't thank this necessary,return value is all right.
	int i = 0;
	for (std::map <CString,int>::iterator itrMap = m_inboundFltNumLimit.begin();\
		itrMap != m_inboundFltNumLimit.end() ;++ itrMap)
	{
		if(i++ == iIndex)
		{
			itrMap->second = iFltNumber;
			break;
		}
	}
	return;
}
