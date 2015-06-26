#include "stdafx.h"
#include "TakeoffSequencing.h"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include ".\runwaytakeoffposition.h"
#include "InputAirside.h"

TakeoffSequencing::TakeoffSequencing(int nProjID,CAirportDatabase* _AirportDB ):m_AirportDB(_AirportDB)
 , m_nProjID(nProjID)
 , m_nID(-1)
{
	m_nAirportID = InputAirside::GetAirportID(nProjID);
}

 TakeoffSequencing::~TakeoffSequencing(void)
 {
	 Clear();
 }

 void TakeoffSequencing::Clear(void)
 {
	 std::vector<TakeOffSequenceInfo *>::iterator itrItemTakeOffSequenceInfo;
	 std::vector<TakeOffPositionInfo *>::iterator itrItemTakeOffPositionInfo;

	 for (itrItemTakeOffSequenceInfo = m_vrTakeOffSequenceInfo.begin();\
		 itrItemTakeOffSequenceInfo != m_vrTakeOffSequenceInfo.end();++ itrItemTakeOffSequenceInfo) {
			 if(*itrItemTakeOffSequenceInfo)
				 delete (*itrItemTakeOffSequenceInfo);
	 }
	 m_vrTakeOffSequenceInfo.clear();
	
	for (itrItemTakeOffSequenceInfo = m_vrTakeOffSequenceInofDeleted.begin();\
		itrItemTakeOffSequenceInfo != m_vrTakeOffSequenceInofDeleted.end();++ itrItemTakeOffSequenceInfo) {
			if(*itrItemTakeOffSequenceInfo)
				delete (*itrItemTakeOffSequenceInfo);
	}
	m_vrTakeOffSequenceInofDeleted.clear();

	for (itrItemTakeOffPositionInfo = m_vrTakeOffPositionInfo.begin();\
		itrItemTakeOffPositionInfo != m_vrTakeOffPositionInfo.end();++ itrItemTakeOffPositionInfo) {
			if(*itrItemTakeOffPositionInfo)
				delete (*itrItemTakeOffPositionInfo);
		}
	m_vrTakeOffPositionInfo.clear();

	for (itrItemTakeOffPositionInfo = m_vrTakeOffPositionInfoDeleted.begin();\
		itrItemTakeOffPositionInfo != m_vrTakeOffPositionInfoDeleted.end();++ itrItemTakeOffPositionInfo) {
			if(*itrItemTakeOffPositionInfo)
				delete (*itrItemTakeOffPositionInfo);
		}
	m_vrTakeOffPositionInfoDeleted.clear();
 }

 TakeOffSequenceInfo::TakeOffSequenceInfo(int nParentID,CAirportDatabase* _data):m_AirportDB(_data)
 {
	 m_nID = -1;
	 m_nParentID = nParentID;
	 m_startTime.set(0L);
	 m_endTime.set(86399L);
	 InitDefaultPriorityData();
 }

TakeOffSequenceInfo::~TakeOffSequenceInfo(void)
 {
 }
 
void TakeOffSequenceInfo::InitDefaultPriorityData(void)
 {
	 for(int i=0; i< PriorityCount; ++i)
	 {
		 m_priority[i].m_Type =(Priority)i;
		 m_priority[i].m_FltTy.SetAirportDB(m_AirportDB) ;
	 }
 }

 void TakeOffSequenceInfo::ReadPriorityData(void)
 {
	 if (m_nID < 0)
		 return InitDefaultPriorityData();

	 CString strSQL = _T("");
	 strSQL.Format(_T("SELECT * FROM IN_TAKEOFFSEQUENCING_DATA WHERE (TAKEOFFID = %d) ORDER BY PORDER;"),m_nID);

	 long nRecordCount = -1;
	 CADORecordset adoRecordset;
	 CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	 int  i = 0;
	 BOOL val = FALSE ;
	 CString flt ;
	 while (!adoRecordset.IsEOF())
	 {
		 if (i >= PriorityCount)
			 break;

		 adoRecordset.GetFieldValue(_T("ID"),m_priority[i].m_ID);
		 int nPriority = 0;
		 adoRecordset.GetFieldValue(_T("PRIORITY"),nPriority);		
		 m_priority[i].m_Type = (Priority)nPriority;
		
		adoRecordset.GetFieldValue(_T("CAN_CROSSPOINT"),val) ;
		m_priority[i].m_CrossPoint = val ;
		
		adoRecordset.GetFieldValue(_T("CAN_TAKEOFF"),val) ;
		m_priority[i].m_CanTakeOff = val ;

		adoRecordset.GetFieldValue(_T("CAN_ENCROACH"),val) ;
		m_priority[i].m_CanEncroach = val ;

		adoRecordset.GetFieldValue(_T("FLT_TYPE"),flt) ;
		m_priority[i].m_FltTy.SetAirportDB(m_AirportDB) ;
		m_priority[i].m_FltTy.setConstraintWithVersion(flt) ;


		 ++i;
		 adoRecordset.MoveNextData();
	 }
 }
 void TakeOffSequenceInfo::SavePriorityData(void)
 {
	 CADODatabase::BeginTransaction();
	 try{
		for (int i = 0; i < PriorityCount; ++i)
		{	
			CString strSQL = _T("");
			TCHAR TH[1024] = {0} ;
			m_priority[i].m_FltTy.WriteSyntaxStringWithVersion(TH) ;
			if (m_priority[i].m_ID < 0)
			{
				strSQL.Format(_T("INSERT INTO IN_TAKEOFFSEQUENCING_DATA (TAKEOFFID, PORDER, PRIORITY,CAN_CROSSPOINT,CAN_TAKEOFF,CAN_ENCROACH,FLT_TYPE) VALUES (%d,%d,%d,%d,%d,%d,'%s');"),m_nID,i,m_priority[i].m_Type,
					m_priority[i].m_CrossPoint,
					m_priority[i].m_CanTakeOff,
					m_priority[i].m_CanEncroach,
					TH);
				m_priority[i].m_ID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
			}
			else
			{
				strSQL.Format(_T("UPDATE IN_TAKEOFFSEQUENCING_DATA SET PRIORITY = %d, PORDER = %d,CAN_CROSSPOINT = %d ,CAN_TAKEOFF = %d , CAN_ENCROACH = %d ,FLT_TYPE = '%s' WHERE (ID = %d);"),
					m_priority[i].m_Type,
					i,
					m_priority[i].m_CrossPoint,
					m_priority[i].m_CanTakeOff,
					m_priority[i].m_CanEncroach,
					TH,
					m_priority[i].m_ID);
				CADODatabase::ExecuteSQLStatement(strSQL);
			}
		}
		 CADODatabase::CommitTransaction();
	 }
	 catch (CADOException& e) {
		 CADODatabase::RollBackTransation();
		 e.ErrorMessage() ;
		 return;
	 }
	
 } 

 void TakeOffSequenceInfo::DeletePriorityData()
 {
	 CString strSQL = _T("");
	 strSQL.Format(_T("DELETE FROM IN_TAKEOFFSEQUENCING_DATA WHERE (TAKEOFFID = %d);"),m_nID);

	 CADODatabase::ExecuteSQLStatement(strSQL);
	 InitDefaultPriorityData();
 }

 TakeOffSequenceInfo::TakeOffSequenceInfo(CADORecordset &adoRecordset,CAirportDatabase* _data):m_AirportDB(_data)
 {
	 if(!adoRecordset.IsEOF())
	 {
		 adoRecordset.GetFieldValue(_T("ID"),m_nID);
		 adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);

		 int nTimeSeconds = 0;
		 adoRecordset.GetFieldValue(_T("STARTTIME"),nTimeSeconds);
		 m_startTime.set(nTimeSeconds);
		 nTimeSeconds = 0;
		 adoRecordset.GetFieldValue(_T("ENDTIME"),nTimeSeconds);
		 m_endTime.set(nTimeSeconds);

		 ReadPriorityData();
	 }
 }

 void TakeOffSequenceInfo::SaveData(int nParentID)
 {
	 CString strSQL = _T("");
	 if(m_nID < 0)
	 {
		 strSQL.Format(_T("INSERT INTO TAKEOFFSEQINFOLIST (PARENTID,STARTTIME,ENDTIME) VALUES (%d,%d,%d);"),m_nParentID = nParentID,m_startTime.asSeconds(),m_endTime.asSeconds());
		 m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	 }
	 else
	 {
		 strSQL.Format(_T("UPDATE TAKEOFFSEQINFOLIST SET PARENTID = %d ,STARTTIME = %d ,ENDTIME = %d WHERE (ID = %d);"),m_nParentID = nParentID,m_startTime.asSeconds(),m_endTime.asSeconds(),m_nID);
		 CADODatabase::ExecuteSQLStatement(strSQL);
	 }
	 SavePriorityData();
 }

 void TakeOffSequenceInfo::DeleteData(void)
 {
	 DeletePriorityData();

	 CString strSQL = _T("");
	 strSQL.Format(_T("DELETE FROM TAKEOFFSEQINFOLIST WHERE (ID = %d);"),m_nID);
	 CADODatabase::ExecuteSQLStatement(strSQL);

	 m_nID = -1;
	 m_nParentID = -1;
	 m_startTime.set(0L);
	 m_endTime.set(86399L);
 }

 void TakeOffSequenceInfo::ImportData(CAirsideImportFile& importFile)
 {
	 m_nID = -1;
	 m_nParentID = -1;
	 int nValue = -1;
	 importFile.getFile().getInteger(nValue);
	 m_startTime.set(nValue);
	 importFile.getFile().getInteger(nValue);
	 m_endTime.set(nValue);
	 for (int i = 0;i < PriorityCount;++i) {
		// m_priorityID[i] = -1;
		 if(importFile.getVersion() <= 1074)
		 {
			 importFile.getFile().getInteger(nValue); 
			 m_priority[i].m_Type = (Priority)nValue;
		 }else
		 {
			 TCHAR th[1024] = {0} ;
			 importFile.getFile().getInteger(nValue); 
			 m_priority[i].m_Type = (Priority)nValue;

			 importFile.getFile().getInteger(nValue); 
			 m_priority[i].m_CrossPoint = (BOOL)nValue;

			 importFile.getFile().getInteger(nValue); 
			 m_priority[i].m_CanTakeOff = (BOOL)nValue;

			 importFile.getFile().getInteger(nValue); 
			 m_priority[i].m_CanEncroach = (BOOL)nValue;

			 importFile.getFile().getField(th,1024) ;
			 m_priority[i].m_FltTy.setConstraintWithVersion(th) ;
		 }

	 }
	 importFile.getFile().getLine();
 }

 void TakeOffSequenceInfo::ExportData(CAirsideExportFile& exportFile)
 {
	 exportFile.getFile().writeInt(m_startTime.asSeconds());
	 exportFile.getFile().writeInt(m_endTime.asSeconds());
	 for (int i = 0;i < PriorityCount;++i) {
		 TCHAR Th[1024] = {0} ;
		 m_priority[i].m_FltTy.WriteSyntaxStringWithVersion(Th) ;
		 exportFile.getFile().writeInt(m_priority[i].m_Type);
		  exportFile.getFile().writeInt(m_priority[i].m_CrossPoint);
		  exportFile.getFile().writeInt(m_priority[i].m_CanTakeOff);
		  exportFile.getFile().writeInt(m_priority[i].m_CanEncroach) ;
		  exportFile.getFile().writeField(Th) ;
	 }
	 exportFile.getFile().writeLine();
 }

TakeOffPositionInfo::TakeOffPositionInfo(int nParentID)
 {
	 m_nID = -1;
	 m_nParentID = nParentID;
	 m_nTakeOffPositionID = -1;
	 m_strTakeOffPositionName = _T("");
 }

TakeOffPositionInfo::TakeOffPositionInfo(CADORecordset &adoRecordset)
 {
	 if(!adoRecordset.IsEOF())
	 {
		adoRecordset.GetFieldValue(_T("ID"),m_nID); 
		adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID); 
		adoRecordset.GetFieldValue(_T("TAKEOFFPOSID"),m_nTakeOffPositionID);
		adoRecordset.GetFieldValue(_T("RWYID"),m_nRwyID);
		adoRecordset.GetFieldValue(_T("RWYMARK"),m_nRwyMark);
	 }
	 m_strTakeOffPositionName = _T("");
 }
 
TakeOffPositionInfo::~TakeOffPositionInfo(void)
 {
 }

 void TakeOffPositionInfo::SaveData(int nParentID)
 {
	 CString strSQL = _T("");
	 if(m_nID < 0)
	 {
		 strSQL.Format(_T("INSERT INTO TAKEOFFPOSLIST (PARENTID,TAKEOFFPOSID,RWYID,RWYMARK) VALUES (%d,%d,%d,%d);"),m_nParentID = nParentID,m_nTakeOffPositionID,m_nRwyID,m_nRwyMark);
		  m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	 }
	 else
	 {
		 strSQL.Format(_T("UPDATE TAKEOFFPOSLIST SET PARENTID = %d ,TAKEOFFPOSID = %d ,RWYID = %d,RWYMARK = %d WHERE (ID = %d);"),m_nParentID = nParentID,m_nTakeOffPositionID,m_nRwyID,m_nRwyMark, m_nID);
		 CADODatabase::ExecuteSQLStatement(strSQL);
	 }
 }

 void TakeOffPositionInfo::DeleteData(void)
 {
	 CString strSQL = _T("");
	 strSQL.Format(_T("DELETE FROM TAKEOFFPOSLIST WHERE (ID = %d);"),m_nID);
	 CADODatabase::ExecuteSQLStatement(strSQL);

	 m_nID = -1;
	 m_nParentID = -1;
	 m_nTakeOffPositionID = -1;
	 m_nRwyID = -1;
	 m_nRwyMark = -1;
	 m_strTakeOffPositionName = _T("");
 }

 void TakeOffPositionInfo::ImportData(CAirsideImportFile& importFile)
 {
	 m_nID = -1;
	 m_nParentID = -1;
	 m_strTakeOffPositionName = _T("");
	 importFile.getFile().getInteger(m_nTakeOffPositionID);
	 m_nTakeOffPositionID = importFile.GetTakeoffpositionRwmkdataNewID(m_nTakeOffPositionID);
	 importFile.getFile().getInteger(m_nRwyID);
	 m_nRwyID = importFile.GetObjectNewID(m_nRwyID);
	 importFile.getFile().getInteger(m_nRwyMark);
	 importFile.getFile().getLine();
 }

 void TakeOffPositionInfo::ExportData(CAirsideExportFile& exportFile)
 {
	 exportFile.getFile().writeInt(m_nTakeOffPositionID);
	 exportFile.getFile().writeInt(m_nRwyID);
	 exportFile.getFile().writeInt(m_nRwyMark);
	 exportFile.getFile().writeLine();
 }

 int TakeoffSequencing::GetTakeOffSequenceInfoCount(void)
 {
	 return ((int)m_vrTakeOffSequenceInfo.size());
 }

 TakeOffSequenceInfo * TakeoffSequencing::GetTakeOffSequenceInfo(int nIndex)
 {
	 if(nIndex < 0 || nIndex >= (int)m_vrTakeOffSequenceInfo.size())
		 return (0);
	 else
		 return (m_vrTakeOffSequenceInfo.at(nIndex));
 }

 int TakeoffSequencing::GetTakeOffPositionCount(void)
 {
	 return ((int)m_vrTakeOffPositionInfo.size());
 }
TakeOffPositionInfo * TakeoffSequencing::GetTakeOffPositionInfo(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_vrTakeOffPositionInfo.size())
		return (0);
	else
		return (m_vrTakeOffPositionInfo.at(nIndex));
}

TakeoffSequencing::TakeoffSequencing(CADORecordset &adoRecordset,CAirportDatabase* _AirportDB ):m_AirportDB(_AirportDB)
{
	if(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("AIRPORTID"),m_nAirportID);
	}
	Clear();
	ReadTakeOffSequenceInfo();
	ReadTakeOffPositionInfo();
}


void TakeoffSequencing::ReadTakeOffSequenceInfo(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TAKEOFFSEQINFOLIST WHERE (PARENTID = %d);"),m_nID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	TakeOffSequenceInfo * pTOSInfo = 0;
	while (!adoRecordset.IsEOF()) {
		pTOSInfo = new TakeOffSequenceInfo(adoRecordset,m_AirportDB);
		if(!pTOSInfo){
			adoRecordset.MoveNextData();
			continue;
		}
		m_vrTakeOffSequenceInfo.push_back(pTOSInfo);
		pTOSInfo = 0;
		adoRecordset.MoveNextData();
	}
}

void TakeoffSequencing::SaveTakeOffSequenceInfo(void)
{
	std::vector<TakeOffSequenceInfo *>::iterator itrInfo;
	for (itrInfo = m_vrTakeOffSequenceInfo.begin();\
		itrInfo != m_vrTakeOffSequenceInfo.end();++itrInfo) {
			if(*itrInfo)
				(*itrInfo)->SaveData(m_nID);
	}

	for (itrInfo = m_vrTakeOffSequenceInofDeleted.begin();\
		itrInfo != m_vrTakeOffSequenceInofDeleted.end();++itrInfo) {
			if(*itrInfo){
				(*itrInfo)->DeleteData();
				delete (*itrInfo);
			}
	}
	m_vrTakeOffSequenceInofDeleted.clear();
}

void TakeoffSequencing::DeleteTakeOffSequenceInfo(void)
{
	std::vector<TakeOffSequenceInfo *>::iterator itrInfo;
	for (itrInfo = m_vrTakeOffSequenceInfo.begin();\
		itrInfo != m_vrTakeOffSequenceInfo.end();++itrInfo) {
			if(*itrInfo){
				(*itrInfo)->DeleteData();
				delete (*itrInfo);
			}
		}
	m_vrTakeOffSequenceInfo.clear();

	for (itrInfo = m_vrTakeOffSequenceInofDeleted.begin();\
		itrInfo != m_vrTakeOffSequenceInofDeleted.end();++itrInfo) {
			if(*itrInfo){
				(*itrInfo)->DeleteData();
				delete (*itrInfo);
			}
		}
	m_vrTakeOffSequenceInofDeleted.clear();
}

bool TakeoffSequencing::DeleteTakeOffSequenceInfoItem(TakeOffSequenceInfo * pTakeOffSequenceItem)
{
	if(!pTakeOffSequenceItem)return (false);
	std::vector<TakeOffSequenceInfo *>::iterator itrTakeOffSeqItem;
	for (itrTakeOffSeqItem = m_vrTakeOffSequenceInfo.begin();itrTakeOffSeqItem != m_vrTakeOffSequenceInfo.end();++itrTakeOffSeqItem) {
		if(*itrTakeOffSeqItem == pTakeOffSequenceItem)
		{
			m_vrTakeOffSequenceInofDeleted.push_back(*itrTakeOffSeqItem);
			m_vrTakeOffSequenceInfo.erase(itrTakeOffSeqItem);
			return (true);
		}
	}
	return (false);
}

bool  TakeoffSequencing::AddTakeOffSequenceInfo(TakeOffSequenceInfo * pItemAdd)
{
	if(!pItemAdd)return (false);
	for (std::vector<TakeOffSequenceInfo *>::iterator itrItem = m_vrTakeOffSequenceInfo.begin();\
		itrItem != m_vrTakeOffSequenceInfo.end();++itrItem) {
			if(*itrItem && ( pItemAdd->m_startTime < (*itrItem)->m_endTime && pItemAdd->m_endTime > (*itrItem)->m_startTime)){
				return (false);
			}
		}
	m_vrTakeOffSequenceInfo.push_back(pItemAdd);
	return (true);
}

void TakeoffSequencing::ReadTakeOffPositionInfo(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TAKEOFFPOSLIST WHERE (PARENTID = %d);"),m_nID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	
	TakeOffPositionInfo * pTOPosInfo = 0;
	while (!adoRecordset.IsEOF()) 
	{
		pTOPosInfo = new TakeOffPositionInfo(adoRecordset);
		if(!pTOPosInfo)
		{
			adoRecordset.MoveNextData();
			continue;
		}
		bool bIDExist = true;
		CString strName = _T("");
		if (pTOPosInfo->m_nTakeOffPositionID == -1)
			strName = _T("All");
		else
			//strName = FindTakeOffPositionName(pTOPosInfo->m_nTakeOffPositionID,rwTakeOffPosition,bIDExist);
		{
			//Runway altRunway;
			//altRunway.ReadObject(pTOPosInfo->m_nRwyID);

			//RunwayExitList exitlist;
			//altRunway.GetExitList((RUNWAY_MARK)pTOPosInfo->m_nRwyMark,exitlist);

			//int nCount = (int)exitlist.size();
			//for (int i = 0; i < nCount; i++)
			//{
			//	if (exitlist[i].GetID() == pTOPosInfo->m_nTakeOffPositionID )
			//	{
			//		strName = exitlist[i].GetName();
			//		break;
			//	}
			//}
			RunwayExit exit;
			exit.ReadData(pTOPosInfo->m_nTakeOffPositionID);
			strName = exit.GetName();
		}
		//if(bIDExist){
			m_vrTakeOffPositionInfo.push_back(pTOPosInfo);
			pTOPosInfo->m_strTakeOffPositionName = strName;
		//}
		//else
		//	m_vrTakeOffPositionInfoDeleted.push_back(pTOPosInfo);
		pTOPosInfo = 0; 
		adoRecordset.MoveNextData();
	}
}

void TakeoffSequencing::SaveTakeOffPositionInfo(void)
{
	std::vector<TakeOffPositionInfo *>::iterator itrInfo;
	for (itrInfo = m_vrTakeOffPositionInfo.begin();\
		itrInfo != m_vrTakeOffPositionInfo.end();++itrInfo) {
			if(*itrInfo)
				(*itrInfo)->SaveData(m_nID);
		}

	for (itrInfo = m_vrTakeOffPositionInfoDeleted.begin();\
		itrInfo != m_vrTakeOffPositionInfoDeleted.end();++itrInfo) {
			if(*itrInfo){
				(*itrInfo)->DeleteData();
				delete (*itrInfo);
			}
		}
	m_vrTakeOffPositionInfoDeleted.clear();
}

void TakeoffSequencing::DeleteTakeOffPositionInfo(void)
{
	std::vector<TakeOffPositionInfo *>::iterator itrInfo;
	for (itrInfo = m_vrTakeOffPositionInfo.begin();\
		itrInfo != m_vrTakeOffPositionInfo.end();++itrInfo) {
			if(*itrInfo){
				(*itrInfo)->DeleteData();
				delete (*itrInfo);
			}
		}
	m_vrTakeOffPositionInfo.clear();

	for (itrInfo = m_vrTakeOffPositionInfoDeleted.begin();\
		itrInfo != m_vrTakeOffPositionInfoDeleted.end();++itrInfo) {
			if(*itrInfo){
				(*itrInfo)->DeleteData();
				delete (*itrInfo);
			}
		}
	m_vrTakeOffPositionInfoDeleted.clear();
}

bool TakeoffSequencing::DeleteTakeOffPositionItem(TakeOffPositionInfo * pTakeOffPositionItem)
{
	if(!pTakeOffPositionItem)return (false);
	std::vector<TakeOffPositionInfo *>::iterator itrTakeOffPosItem;
	for (itrTakeOffPosItem = m_vrTakeOffPositionInfo.begin();itrTakeOffPosItem != m_vrTakeOffPositionInfo.end();++itrTakeOffPosItem) {
		if((*itrTakeOffPosItem) == pTakeOffPositionItem)
		{
			m_vrTakeOffPositionInfoDeleted.push_back(*itrTakeOffPosItem);
			m_vrTakeOffPositionInfo.erase(itrTakeOffPosItem);
			return (true);
		}
	}
	return (false);
}

void TakeoffSequencing::DeleteAllTakeOffPositionItem(void)
{
	m_vrTakeOffPositionInfoDeleted.insert(m_vrTakeOffPositionInfoDeleted.end(),m_vrTakeOffPositionInfo.begin(),m_vrTakeOffPositionInfo.end());
	m_vrTakeOffPositionInfo.clear() ; 

}

bool TakeoffSequencing::AddTakeOffPositionInfo(TakeOffPositionInfo * pItemAdd)
{
	if(!pItemAdd)return (false);
	for (std::vector<TakeOffPositionInfo *>::iterator itrItem = m_vrTakeOffPositionInfo.begin();\
		itrItem != m_vrTakeOffPositionInfo.end();++itrItem) 
	{
			if(*itrItem && (*itrItem)->m_nRwyID == pItemAdd->m_nRwyID && (*itrItem)->m_nRwyMark == pItemAdd->m_nRwyMark
				&& (*itrItem)->m_nTakeOffPositionID == pItemAdd->m_nTakeOffPositionID)
			{
				return (false);
			}
	}
	m_vrTakeOffPositionInfo.push_back(pItemAdd);
	return (true);
}

//CString TakeoffSequencing::FindTakeOffPositionName(int nTakeOffPositionID,CRunwayTakeOffPosition &rwTakeOffPosition,bool &bIDExist)
//{	
//	bIDExist = true;
//	CString strMarkIntTemp = _T("");
//	if(nTakeOffPositionID < 0)
//	{
//		bIDExist = false;
//		return (strMarkIntTemp);
//	}
//	int nAirportCount = rwTakeOffPosition.GetAirportCount();
//	for (int i = 0; i < nAirportCount; ++i) {
//		CRunwayTakeOffAirportData* pTakeOffAirportData = 0;
//		pTakeOffAirportData = rwTakeOffPosition.GetAirportItem(i);
//		if(!pTakeOffAirportData)continue;
//		int nRunwayCount = pTakeOffAirportData->GetRunwayCount();
//		for (int j = 0; j < nRunwayCount; ++j) {
//			CRunwayTakeOffRunwayData* pRwTakeOffRunwayData = 0;		
//			pRwTakeOffRunwayData = pTakeOffAirportData->GetRunwayItem(j);
//			if(!pRwTakeOffRunwayData)continue;
//
//			CRunwayTakeOffRunwayWithMarkData* pRwTakeOffRunwayMarkData = 0;
//			int nRunwayID = -1;
//			int nRunwayMarkIndex = -1;
//			int nTaxiwayCount = -1;
//			int nTaxiwayID = -1;
//			CRunwayTakeOffRMTaxiwayData* pRwTakeOffTaxiwayData = 0;
//			CRunwayTakeOffRTIntersectionData* pRwTakeOffRTIntersectionData = 0;
//			CString strMarkTemp = _T("");
//			Runway altRunway;
//			Taxiway* taxiway = 0;
//			CString taxiwayName = _T("");
//			strMarkIntTemp = _T("");
//			IntersectionNode intersectionNode;
//			int idx = -1;
//
//			pRwTakeOffRunwayMarkData = pRwTakeOffRunwayData->getRunwayMark1Data();
//			if(pRwTakeOffRunwayMarkData)
//			{
//				altRunway.ReadObject(pRwTakeOffRunwayMarkData->getRunwayID());
//				if(pRwTakeOffRunwayMarkData->getRunwayMarkIndex() == 0)
//					strMarkTemp = altRunway.GetMarking1().c_str();
//				else
//					strMarkTemp = altRunway.GetMarking2().c_str();
//
//				nTaxiwayCount = pRwTakeOffRunwayMarkData->GetTaxiwayCount();
//				for (int k = 0;k < nTaxiwayCount;++k) {
//					pRwTakeOffTaxiwayData = pRwTakeOffRunwayMarkData->GetTaxiwayItem(k);
//					if(!pRwTakeOffTaxiwayData)continue;
//					
//					taxiway = pRwTakeOffTaxiwayData->getTaxiway();
//					taxiwayName = taxiway->GetMarking().c_str();
//
//					int nRwTakeOffRTInetersectionCount = pRwTakeOffTaxiwayData->GetInterCount();
//					for (int u = 0;u < nRwTakeOffRTInetersectionCount; ++u) {
//						pRwTakeOffRTIntersectionData = pRwTakeOffTaxiwayData->GetInterItem(u); 
//						if(!pRwTakeOffRTIntersectionData)continue;
//						if(pRwTakeOffRTIntersectionData->getID() == nTakeOffPositionID){	
//							if(nRwTakeOffRTInetersectionCount == 1)
//								strMarkIntTemp.Format("%s&%s",strMarkTemp,taxiwayName);
//							else
//							{
//								int nIntersecNodeID = pRwTakeOffRTIntersectionData->GetIntersecNodeID();
//								intersectionNode.ReadData(nIntersecNodeID);
//								idx = intersectionNode.GetIndex();		
//								strMarkIntTemp.Format("%s&%s(%d)",strMarkTemp,taxiwayName,idx+1);
//							}
//							return (strMarkIntTemp);
//						}
//					}
//				}
//
//			}
//
//			/*pRwTakeOffRunwayMarkData = 0;
//			nRunwayID = -1;
//			nRunwayMarkIndex = -1;
//			nTaxiwayCount = -1;
//			nTaxiwayID = -1;
//			pRwTakeOffTaxiwayData = 0;
//			pRwTakeOffRTIntersectionData = 0;
//			strMarkTemp = _T("");
//			taxiway = 0;
//			taxiwayName = _T("");
//			strMarkIntTemp = _T("");
//			idx = -1;
//
//			pRwTakeOffRunwayMarkData = pRwTakeOffRunwayData->getRunwayMark2Data();
//			if(pRwTakeOffRunwayMarkData)
//			{
//				altRunway.ReadObject(pRwTakeOffRunwayMarkData->getRunwayID());
//				if(pRwTakeOffRunwayMarkData->getRunwayMarkIndex() == 0)
//					strMarkTemp = altRunway.GetMarking1().c_str();
//				else
//					strMarkTemp = altRunway.GetMarking2().c_str();
//
//				nTaxiwayCount = pRwTakeOffRunwayMarkData->GetTaxiwayCount();
//				for (int k = 0;k < nTaxiwayCount;++k) {
//					pRwTakeOffTaxiwayData = pRwTakeOffRunwayMarkData->GetTaxiwayItem(k);
//					if(!pRwTakeOffTaxiwayData)continue;
//
//					taxiway = pRwTakeOffTaxiwayData->getTaxiway();
//					taxiwayName = taxiway->GetMarking().c_str();
//
//					int nRwTakeOffRTInetersectionCount = pRwTakeOffTaxiwayData->GetInterCount();
//					for (int u = 0;u < nRwTakeOffRTInetersectionCount; ++u) {
//						pRwTakeOffRTIntersectionData = pRwTakeOffTaxiwayData->GetInterItem(u); 
//						if(!pRwTakeOffRTIntersectionData)continue;
//						if(pRwTakeOffRTIntersectionData->getID() == nTakeOffPositionID){	
//							if(nRwTakeOffRTInetersectionCount == 1)
//								strMarkIntTemp.Format("%s&%s",strMarkTemp,taxiwayName);
//							else
//							{
//								int nIntersecNodeID = pRwTakeOffRTIntersectionData->GetIntersecNodeID();
//								intersectionNode.ReadData(nIntersecNodeID);
//								idx = intersectionNode.GetIndex();		
//								strMarkIntTemp.Format("%s&%s(%d)",strMarkTemp,taxiwayName,idx+1);
//							}
//						}
//						return (strMarkIntTemp);
//					}
//				}
//
//			}*/
//		}
//	}
//
//	bIDExist = false;
//	return (strMarkIntTemp);
//}

void TakeoffSequencing::SaveData(void)
{
	CString strSQL = _T("");
	if (m_nID < 0)
	{
		strSQL.Format(_T("INSERT INTO TAKEOFFSEQUENCE (PROJID,AIRPORTID) VALUES (%d,%d)"),m_nProjID,m_nAirportID);

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{		
		strSQL.Format(_T("UPDATE TAKEOFFSEQUENCE SET PROJID = %d,AIRPORTID = %d WHERE (ID = %d)"),m_nProjID,m_nAirportID,m_nID);

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
	SaveTakeOffSequenceInfo();
	SaveTakeOffPositionInfo();
}

void TakeoffSequencing::DeleteData(void)
{
	DeleteTakeOffSequenceInfo();
	DeleteTakeOffPositionInfo();


	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TAKEOFFSEQUENCE WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
	m_nID = -1;
	m_nProjID = -1;
	m_nAirportID = -1;
}


void TakeoffSequencing::ImportData(CAirsideImportFile& importFile)
{
	m_nID = -1;
	Clear();
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	importFile.getFile().getInteger(m_nAirportID);
	m_nAirportID = importFile.getNewAirspaceID();
	int nCount = -1;
	int i = 0;
	importFile.getFile().getInteger(nCount);
	for (i = 0; i < nCount;++i) {
		TakeOffSequenceInfo * pSeqItem = new TakeOffSequenceInfo(-1,m_AirportDB);
		if(!pSeqItem)continue;
		pSeqItem->ImportData(importFile);
		m_vrTakeOffSequenceInfo.push_back(pSeqItem);
	}

	importFile.getFile().getInteger(nCount);
	for (i = 0; i < nCount;++i) {
		TakeOffPositionInfo * pPosItem = new TakeOffPositionInfo(-1);
		if(!pPosItem)continue;
		pPosItem->ImportData(importFile);
		m_vrTakeOffPositionInfo.push_back(pPosItem);
	}

	importFile.getFile().getLine();
}
void TakeoffSequencing::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_nAirportID);
	exportFile.getFile().writeInt((int)m_vrTakeOffSequenceInfo.size());
	for (std::vector<TakeOffSequenceInfo *>::iterator itrSeqItem = m_vrTakeOffSequenceInfo.begin();\
		itrSeqItem != m_vrTakeOffSequenceInfo.end();++itrSeqItem) {
			if(*itrSeqItem)
				(*itrSeqItem)->ExportData(exportFile);
	}

	exportFile.getFile().writeInt((int)m_vrTakeOffPositionInfo.size());
	for (std::vector<TakeOffPositionInfo *>::iterator itrPosItem = m_vrTakeOffPositionInfo.begin();\
		itrPosItem != m_vrTakeOffPositionInfo.end();++itrPosItem) {
			if(*itrPosItem)
				(*itrPosItem)->ExportData(exportFile);
	}
	exportFile.getFile().writeLine();
}

//////////////////////////////////////////////////////////////////////////
//TakeoffSequencingList
CString TakeoffSequencingList::GetPriorityName(Priority priority)
{
	CString strName = _T("");

	switch(priority)
	{
	case FIFO:
		strName = _T("FIFO");
		break;

	case LongestQFirst:
		strName = _T("LongestQFirst");
		break;

	case RoundRobin:
		strName = _T("RoundRobin");
		break;

	case DepartureSlot:
		strName = _T("DepartureSlot");
		break;

	case PriorityFlightType:
		strName = _T("PriorityFlightType");
		break;
	}

	return strName;
}


TakeoffSequencingList::TakeoffSequencingList(int nProjectID,CAirportDatabase* _AirportDB )
 : m_nProjID(nProjectID),m_AirportDB(_AirportDB)
{
}

TakeoffSequencingList::~TakeoffSequencingList(void)
{
	Clear();
}

int TakeoffSequencingList::GetTakeoffSequencingCount() const
{
	return (int)m_vrTakeOffSequenceItem.size();
}

const TakeoffSequencing* TakeoffSequencingList::GetItem(int nIndex) const
{
	if(nIndex < 0 || nIndex >= (int)m_vrTakeOffSequenceItem.size())
		return (0);
	return m_vrTakeOffSequenceItem.at(nIndex);
}

TakeoffSequencing* TakeoffSequencingList::GetItem(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_vrTakeOffSequenceItem.size())
		return (0);
	return m_vrTakeOffSequenceItem.at(nIndex);
}

bool TakeoffSequencingList::AddItem(TakeoffSequencing* pNewItem)
{	
	if(!pNewItem)return (0);
	m_vrTakeOffSequenceItem.push_back(pNewItem);
	return (true);
}

bool TakeoffSequencingList::DeleteItem(TakeoffSequencing* pItem)
{
	if(!pItem)return (false);
	std::vector<TakeoffSequencing *>::iterator itrItem;
	for (itrItem = m_vrTakeOffSequenceItem.begin();itrItem != m_vrTakeOffSequenceItem.end();++ itrItem) {
		if(*itrItem == pItem){
			m_vrTakeOffSequenceItemDeleted.push_back(*itrItem);
			m_vrTakeOffSequenceItem.erase(itrItem);
			return (true);
		}
	}
	return (false);
}

void TakeoffSequencingList::Clear(void)
{
	std::vector<TakeoffSequencing *>::iterator itrItem;
	for (itrItem = m_vrTakeOffSequenceItem.begin();\
		itrItem != m_vrTakeOffSequenceItem.end();++ itrItem) {
			if(*itrItem)
				delete (*itrItem);
	}
	m_vrTakeOffSequenceItem.clear();

	for (itrItem = m_vrTakeOffSequenceItemDeleted.begin();\
		itrItem != m_vrTakeOffSequenceItemDeleted.end();++ itrItem) {
			if(*itrItem)
				delete (*itrItem);
		}
	m_vrTakeOffSequenceItemDeleted.clear();
}


void TakeoffSequencingList::ReadData(void)
{
	CString strSQL = _T("");
	// TODO: Set the select SQL
	strSQL.Format(_T("SELECT * FROM TAKEOFFSEQUENCE WHERE (PROJID = %d)"),m_nProjID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount,adoRecordset);

	Clear();
	while (!adoRecordset.IsEOF())
	{
		m_vrTakeOffSequenceItem.push_back(new TakeoffSequencing(adoRecordset,m_AirportDB));
		adoRecordset.MoveNextData();
	}
}

void TakeoffSequencingList::SaveData(void)
{
	std::vector<TakeoffSequencing *>::iterator itrTOSItem;
	for (itrTOSItem = m_vrTakeOffSequenceItem.begin();\
		itrTOSItem != m_vrTakeOffSequenceItem.end(); ++itrTOSItem){
			if(*itrTOSItem)
				(*itrTOSItem)->SaveData();
	}
	
	for (itrTOSItem = m_vrTakeOffSequenceItemDeleted.begin();\
		itrTOSItem != m_vrTakeOffSequenceItemDeleted.end(); ++itrTOSItem){
			if(*itrTOSItem){
				(*itrTOSItem)->DeleteData();
				delete (*itrTOSItem);
			}
	}
	m_vrTakeOffSequenceItemDeleted.clear();
}

void TakeoffSequencingList::DeleteData(void)
{
	std::vector<TakeoffSequencing *>::iterator itrTOSItem;
	for (itrTOSItem = m_vrTakeOffSequenceItem.begin();\
		itrTOSItem != m_vrTakeOffSequenceItem.end(); ++itrTOSItem){
			if(*itrTOSItem){
				(*itrTOSItem)->DeleteData();
				delete (*itrTOSItem);
			}
		}
	m_vrTakeOffSequenceItem.clear();

	for (itrTOSItem = m_vrTakeOffSequenceItemDeleted.begin();\
		itrTOSItem != m_vrTakeOffSequenceItemDeleted.end(); ++itrTOSItem){
			if(*itrTOSItem){
				(*itrTOSItem)->DeleteData();
				delete (*itrTOSItem);
			}
		}
	m_vrTakeOffSequenceItemDeleted.clear();
}

void TakeoffSequencingList::ExportTakeoffSequencing(CAirsideExportFile& exportFile,CAirportDatabase* _AirportDB)
{
	TakeoffSequencingList takeOffSequencing(exportFile.GetProjectID(),_AirportDB);
	exportFile.getFile().writeField("TakeoffSequencing");
	exportFile.getFile().writeLine();
	takeOffSequencing.ReadData();
	takeOffSequencing.ExportData(exportFile);
	exportFile.getFile().endFile();
}
void TakeoffSequencingList::ExportData(CAirsideExportFile& exportFile)
{	
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt((int)m_vrTakeOffSequenceItem.size());
	std::vector<TakeoffSequencing *>::iterator itrTakeOffItem;
	for (itrTakeOffItem = m_vrTakeOffSequenceItem.begin();itrTakeOffItem != m_vrTakeOffSequenceItem.end(); ++itrTakeOffItem)
	{
		if(*itrTakeOffItem)
			(*itrTakeOffItem)->ExportData(exportFile);
	}
	exportFile.getFile().writeLine();
}
void TakeoffSequencingList::ImportTakeoffSequencing(CAirsideImportFile& importFile,CAirportDatabase* _AirportDB)
{
	if(importFile.getVersion() < 1033)
		return;
	TakeoffSequencingList takeOffSequencing(importFile.getNewProjectID(),_AirportDB);
	while (!importFile.getFile().isBlank())
	{
		takeOffSequencing.ImportData(importFile);
	}
	takeOffSequencing.SaveData();
}
void TakeoffSequencingList::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	int nCount = -1;
	importFile.getFile().getInteger(nCount);
	for (int i = 0;i < nCount;++i) {
		TakeoffSequencing * pTakeOffSequenceItme = new TakeoffSequencing(m_nProjID,m_AirportDB);
		if(!pTakeOffSequenceItme)continue;
		pTakeOffSequenceItme->ImportData(importFile);
		m_vrTakeOffSequenceItem.push_back(pTakeOffSequenceItme);
	}
	importFile.getFile().getLine();
}

CPriorityData::CPriorityData() :m_ID(-1),m_Type(FIFO),m_CrossPoint(TRUE),m_CanTakeOff(TRUE),m_CanEncroach(TRUE)
{

}

CPriorityData::~CPriorityData()
{

}