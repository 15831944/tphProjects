#include "stdafx.h"
#include ".\runwaytakeoffposition.h"

#include "HoldPosition.h"
#include "..\Common\Flt_cnst.h"
#include "..\Common\AirportDatabase.h"
#include "..\Common\point.h"
#include "AirsideImportExportManager.h"


#include <limits>
#include <functional>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////

//CRunwayTakeOffTimeRangeFltType()
CRunwayTakeOffTimeRangeFltType::CRunwayTakeOffTimeRangeFltType()
:m_nID(-1)
,m_pAirportDB(NULL)
,m_eBacktrackOp(NoBacktrack)
{

}
void CRunwayTakeOffTimeRangeFltType::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("TIMERANGEID"),m_nTimeRangeID);

	CString strFltType;
	char szFltType[1024] = {0};
	adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);
	strcpy_s(szFltType,strFltType);
	m_fltType.SetAirportDB(m_pAirportDB);
	m_fltType.setConstraintWithVersion(szFltType);

	int nBacktrak;
	adoRecordset.GetFieldValue(_T("BACKTRACK"), nBacktrak);
	if (nBacktrak == 0)
		m_eBacktrackOp = MaxBacktrack;
	if (nBacktrak == 1)	
		m_eBacktrackOp = MinBacktrack;
	if (nBacktrak == 2)
		m_eBacktrackOp = NoBacktrack;
}
void CRunwayTakeOffTimeRangeFltType::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_TAKEOFFTRFLTTYPEDATA\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void CRunwayTakeOffTimeRangeFltType::SaveData(int nTimeRangeID)
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strSQL = _T("");

	strSQL.Format(_T("INSERT INTO IN_RUNWAY_TAKEOFFTRFLTTYPEDATA\
		(FLTTYPE, TIMERANGEID, BACKTRACK)\
		VALUES ('%s', %d, %d)"),szFltType,nTimeRangeID, (int)m_eBacktrackOp);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CRunwayTakeOffTimeRangeFltType::UpdateData(int nTimeRangeID)
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strSQL = _T("");

	strSQL.Format(_T("UPDATE IN_RUNWAY_TAKEOFFTRFLTTYPEDATA\
					 SET FLTTYPE ='%s',TIMERANGEID =%d, BACKTRACK = %d\
					 WHERE (ID = %d)"), szFltType, nTimeRangeID, (int)m_eBacktrackOp, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void CRunwayTakeOffTimeRangeFltType::ExportData(CAirsideExportFile& exportFile)
{
	//exportFile.getFile().writeInt(m_nID);
	//char szFltType[1024] = {0};
	//m_fltType.WriteSyntaxString(szFltType);
	//CString strFltType;
	//strFltType = szFltType;
	//exportFile.getFile().writeField(strFltType);
	//exportFile.getFile().writeLine();
}
void CRunwayTakeOffTimeRangeFltType::ImportData(CAirsideImportFile& importFile,int nTimeRangeID)
{
//	int nOldID = -1;
//	importFile.getFile().getInteger(nOldID);
//
//	CString strFltType;
//	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);
////	char szFltType[1024] = {0};
////	strcpy(szFltType,strFltType);
////	m_fltType.SetAirportDB(m_pAirportDB);
////	m_fltType.setConstraint(szFltType,VERSION_CONSTRAINT_CURRENT);
//	ImportSaveData(nTimeRangeID,strFltType);
//	importFile.getFile().getLine();
}
void CRunwayTakeOffTimeRangeFltType::ImportSaveData(int nTimeRangeID,CString strFltType)
{
////	char szFltType[1024] = {0};
////	m_fltType.WriteSyntaxString(szFltType);
//	CString strSQL = _T("");
//
//	strSQL.Format(_T("INSERT INTO IN_RUNWAY_TAKEOFFTRFLTTYPEDATA\
//					 (FLTTYPE, TIMERANGEID, BACKTRACK)\
//					 VALUES ('%s', %d, %d)"),strFltType,nTimeRangeID, (int)m_eBacktrackOp);
//
//	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
//////////////////////////////////////////////////////////////////////////
//CRunwayTakeOffTimeRangeData
CRunwayTakeOffTimeRangeData::~CRunwayTakeOffTimeRangeData()
{
	for (int i = 0; i < (int)m_vFltType.size(); i++)
	{
		CRunwayTakeOffTimeRangeFltType* pData = m_vFltType.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vFltType.clear();

	for (int i = 0; i < (int)m_vFltTypeNeedDel.size(); i++)
	{
		CRunwayTakeOffTimeRangeFltType* pData = m_vFltTypeNeedDel.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vFltTypeNeedDel.clear();
}

void CRunwayTakeOffTimeRangeData::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("RWTWINTID"),m_nRwTwIntID);

	long ltimeSec = 0l;
	adoRecordset.GetFieldValue(_T("STARTTIME"),ltimeSec);
	m_startTime.set(ltimeSec);
	adoRecordset.GetFieldValue(_T("ENDTIME"),ltimeSec);
	m_endTime.set(ltimeSec);

	ReadFltType();
}

void CRunwayTakeOffTimeRangeData::ReadFltType()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT *\
		FROM IN_RUNWAY_TAKEOFFTRFLTTYPEDATA\
		WHERE (TIMERANGEID = %d)"),m_nID);

	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{	

			CRunwayTakeOffTimeRangeFltType* pData = new CRunwayTakeOffTimeRangeFltType;
			pData->SetAirportDB(m_pAirportDB);
			pData->ReadData(adoRecordset);
			m_vFltType.push_back(pData);
			
			adoRecordset.MoveNextData();
		}
	}	
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void CRunwayTakeOffTimeRangeData::SaveData(int nRwTwIntID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_TAKOFFTIMERANGEDATA\
		(RWTWINTID, STARTTIME, ENDTIME)\
		VALUES (%d,%d,%d)"),nRwTwIntID,m_startTime.asSeconds(),m_endTime.asSeconds());

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	
	std::vector<CRunwayTakeOffTimeRangeFltType* >::iterator iter = m_vFltType.begin();
	
	for (;iter !=m_vFltType.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	for (iter = m_vFltTypeNeedDel.begin(); iter !=m_vFltTypeNeedDel.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vFltTypeNeedDel.clear();

}
void CRunwayTakeOffTimeRangeData::UpdateData(int nRwTwIntID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_TAKOFFTIMERANGEDATA\
		SET RWTWINTID =%d, STARTTIME =%d, ENDTIME =%d\
		WHERE (ID = %d)"),nRwTwIntID,m_startTime.asSeconds(),m_endTime.asSeconds(),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CRunwayTakeOffTimeRangeFltType* >::iterator iter = m_vFltType.begin();

	for (;iter !=m_vFltType.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	for (iter = m_vFltTypeNeedDel.begin(); iter !=m_vFltTypeNeedDel.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vFltTypeNeedDel.clear() ;
}
void CRunwayTakeOffTimeRangeData::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_TAKOFFTIMERANGEDATA\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CRunwayTakeOffTimeRangeFltType* >::iterator iter = m_vFltType.begin();

	for (;iter !=m_vFltType.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vFltType.clear();
	for (iter = m_vFltTypeNeedDel.begin(); iter !=m_vFltTypeNeedDel.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vFltTypeNeedDel.clear();
}
CRunwayTakeOffTimeRangeFltType* CRunwayTakeOffTimeRangeData::GetFltTypeItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vFltType.size());

	return m_vFltType[nIndex];
}

void CRunwayTakeOffTimeRangeData::DelData(CRunwayTakeOffTimeRangeFltType* pData)
{
	if(pData == NULL)
		return;

	m_vFltTypeNeedDel.push_back(pData);

	std::vector<CRunwayTakeOffTimeRangeFltType*>::iterator iter = m_vFltType.begin();
	for (;iter != m_vFltType.end();++iter)
	{
		if(pData->getID() != -1)
		{
			if(pData->getID() == (*iter)->getID())
			{
				m_vFltType.erase(iter);
				break;
			}
		}
		else
		{
			if(pData->GetFltType() == (*iter)->GetFltType())
			{
				m_vFltType.erase(iter);
				break;
			}
		}
	}
}

void CRunwayTakeOffTimeRangeData::UpdateVData(CRunwayTakeOffTimeRangeFltType* pData1,CRunwayTakeOffTimeRangeFltType* pData2)
{
	if (pData1 == NULL)
		return;

	std::vector<CRunwayTakeOffTimeRangeFltType*>::iterator iter = m_vFltType.begin();
	for (;iter != m_vFltType.end();++iter)
	{
		if(pData1->getID() != -1)
		{
			if(pData1->getID() == (*iter)->getID())
			{
				(**iter) = (*pData2);
				break;
			}
		}
		else
		{
			if(pData1->GetFltType() == (*iter)->GetFltType())
			{
				(**iter) = (*pData2);
				break;
			}	
		}
	}
}
void CRunwayTakeOffTimeRangeData::ExportData(CAirsideExportFile& exportFile)
{
	//exportFile.getFile().writeInt(m_nID);
	//exportFile.getFile().writeInt(m_startTime.asSeconds());
	//exportFile.getFile().writeInt(m_endTime.asSeconds());

	//exportFile.getFile().writeLine();

	//exportFile.getFile().writeInt(static_cast<int>(m_vFltType.size()));
	//std::vector<CRunwayTakeOffTimeRangeFltType* >::iterator iter = m_vFltType.begin();
	//for (;iter !=m_vFltType.end();++iter)
	//{
	//	(*iter)->ExportData(exportFile);
	//}
}
void CRunwayTakeOffTimeRangeData::ImportData(CAirsideImportFile& importFile,int nRwTwIntID)
{
	//int nOldID = -1;
	//importFile.getFile().getInteger(nOldID);

	//long lstartTime = 0L;
	//importFile.getFile().getInteger(lstartTime);
	//m_startTime.set(lstartTime);

	//long lendtime = 0L;
	//importFile.getFile().getInteger(lendtime);
	//m_endTime.set(lendtime);
	//SaveData(nRwTwIntID);

	//importFile.getFile().getLine();

	//int nRunwayTakeOffTimeRangeFltTypeCount = 0;
	//importFile.getFile().getInteger(nRunwayTakeOffTimeRangeFltTypeCount);
	//for (int i =0; i < nRunwayTakeOffTimeRangeFltTypeCount; ++i)
	//{
	//	CRunwayTakeOffTimeRangeFltType data;
	//	data.ImportData(importFile,m_nID);
	//}
}
//////////////////////////////////////////////////////////////////////////
//
CRunwayTakeOffPositionData::CRunwayTakeOffPositionData()
:m_nID(-1)
,m_bEligible(false)
,m_nRwyExitID(-1)
,m_pAirportDB(NULL)
//,m_nTaxiwayID(-1)
,m_nRunwayMarkDataID(-1)
,m_dRemainFt(0.0)
,m_nIntType(-1)
{

}
CRunwayTakeOffPositionData::~CRunwayTakeOffPositionData()
{
	for (int i = 0; i < (int) m_vTimeRangeData.size(); i++)
	{
		CRunwayTakeOffTimeRangeData* pData = m_vTimeRangeData.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vTimeRangeData.clear();

	for (int i = 0; i < (int) m_vTimeRangeDataNeedDel.size(); i++)
	{
		CRunwayTakeOffTimeRangeData* pData = m_vTimeRangeDataNeedDel.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vTimeRangeDataNeedDel.clear();	
}


void CRunwayTakeOffPositionData::ReadData(CADORecordset &adoRecordset)
{
	int nTaxiwayID = -1;
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("RWMARKDATAID"),m_nRunwayMarkDataID);
	adoRecordset.GetFieldValue(_T("TAXIWAYID"),nTaxiwayID);
	adoRecordset.GetFieldValue(_T("INTTYPE"),m_nIntType);
	adoRecordset.GetFieldValue(_T("ELIGIBLE"),m_bEligible);
	adoRecordset.GetFieldValue(_T("REMAINFT"),m_dRemainFt);
	int nRunwayExitDescription = -1;
	adoRecordset.GetFieldValue(_T("RWEXITDESID"),nRunwayExitDescription);
	m_runwayExitDescription.ReadData(nRunwayExitDescription);

	ReadTimeRangeData();
}

void CRunwayTakeOffPositionData::ReadTimeRangeData()
{

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID,RWTWINTID, STARTTIME, ENDTIME\
		FROM IN_RUNWAY_TAKOFFTIMERANGEDATA\
		WHERE (RWTWINTID = %d)"),m_nID);

	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			CRunwayTakeOffTimeRangeData* pData = new CRunwayTakeOffTimeRangeData;
			pData->SetAirportDB(m_pAirportDB);
			pData->ReadData(adoRecordset);
			m_vTimeRangeData.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}	
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void CRunwayTakeOffPositionData::SaveData(int nRwMKDataID/*,int nTaxiwayID,int nOrder*/)
{
	if (m_nID >=0)
	{
		UpdateData(nRwMKDataID);
		return;
	}

	m_runwayExitDescription.SaveData();

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_TAKEOFFRWMKDATA\
		(RWMARKDATAID, TAXIWAYID,RWEXITDESID,INTTYPE, ELIGIBLE, REMAINFT)\
		VALUES (%d,%d,%d,%d,%d,%f)"),nRwMKDataID,-1,m_runwayExitDescription.GetID(),m_nIntType, m_bEligible?1:0, m_dRemainFt);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	std::vector<CRunwayTakeOffTimeRangeData *>::iterator iter = m_vTimeRangeData.begin();
	for (;iter != m_vTimeRangeData.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	iter = m_vTimeRangeDataNeedDel.begin();
	for (;iter != m_vTimeRangeDataNeedDel.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vTimeRangeDataNeedDel.clear();
}
void CRunwayTakeOffPositionData::UpdateData(int nRwMKDataID/*,int nTaxiwayID,int nOrder*/)
{
	m_runwayExitDescription.UpdateData();

	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_TAKEOFFRWMKDATA\
		SET RWMARKDATAID =%d, TAXIWAYID =%d, RWEXITDESID =%d, INTTYPE =%d, ELIGIBLE =%d, REMAINFT =%f\
		WHERE (ID = %d)"),nRwMKDataID,-1,m_runwayExitDescription.GetID(),m_nIntType,m_bEligible?1:0,m_dRemainFt,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CRunwayTakeOffTimeRangeData *>::iterator iter = m_vTimeRangeData.begin();
	for (;iter != m_vTimeRangeData.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	iter = m_vTimeRangeDataNeedDel.begin();
	for (;iter != m_vTimeRangeDataNeedDel.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vTimeRangeDataNeedDel.clear();
}
void CRunwayTakeOffPositionData::DeleteData()
{
	std::vector<CRunwayTakeOffTimeRangeData *>::iterator iter = m_vTimeRangeData.begin();
	for (;iter != m_vTimeRangeData.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vTimeRangeData.clear();

	iter = m_vTimeRangeDataNeedDel.begin();
	for (;iter != m_vTimeRangeDataNeedDel.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vTimeRangeDataNeedDel.clear();

	CString strSQL = _T("");

	strSQL.Format(_T("DELETE FROM IN_RUNWAY_TAKEOFFRWMKDATA\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CRunwayTakeOffPositionData::CopyData(CRunwayTakeOffPositionData *pIntersecData)
{
	*this = *pIntersecData;
}
void CRunwayTakeOffPositionData::SetExitID(const RunwayExit& rwyExit)
{
	m_runwayExitDescription.InitData(rwyExit);
}

CRunwayTakeOffTimeRangeData* CRunwayTakeOffPositionData::GetTimeItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vTimeRangeData.size());

	return m_vTimeRangeData[nIndex];
}

void CRunwayTakeOffPositionData::DelData(CRunwayTakeOffTimeRangeData* pData)
{	
	if (pData == NULL)
		return;

	m_vTimeRangeDataNeedDel.push_back(pData);
	std::vector<CRunwayTakeOffTimeRangeData *>::iterator iter = m_vTimeRangeData.begin();
	for (;iter != m_vTimeRangeData.end();++iter)
	{
		if(pData->getID() != -1)
		{
			if(pData->getID() == (*iter)->getID())
			{
				m_vTimeRangeData.erase(iter);
				break;
			}
		}
		else
		{
			if(pData->GetStartTime() == (*iter)->GetStartTime() && pData->GetEndTime() == (*iter)->GetEndTime())
			{
				m_vTimeRangeData.erase(iter);
				break;
			}
		}
	}
}

void CRunwayTakeOffPositionData::UpdateVData(CRunwayTakeOffTimeRangeData* pData1,CRunwayTakeOffTimeRangeData* pData2)
{
	if (pData1 == NULL )
		return;

	std::vector<CRunwayTakeOffTimeRangeData *>::iterator iter = m_vTimeRangeData.begin();
	for (;iter != m_vTimeRangeData.end();++iter)
	{
		if(pData1->getID()!=-1)
		{
			if(pData1->getID() == (*iter)->getID())
			{
				(**iter) = (*pData2);
				break;
			}
		}
		else
		{
			if(pData1->GetStartTime() == (*iter)->GetStartTime() && pData1->GetEndTime() == (*iter)->GetEndTime())
			{
				(**iter) = (*pData2);
				break;
			}
		}
	}
}
void CRunwayTakeOffPositionData::ExportData(CAirsideExportFile& exportFile,int nTaxiwayID,int nOrder)
{
	//exportFile.getFile().writeInt(m_nID);
	//exportFile.getFile().writeInt(nTaxiwayID);
	//exportFile.getFile().writeInt(nOrder);
	//exportFile.getFile().writeInt(m_nIntType);
	//exportFile.getFile().writeInt(m_bEligible?1:0);
	//exportFile.getFile().writeFloat(m_nRemainFt);

	//exportFile.getFile().writeLine();

	//exportFile.getFile().writeInt(static_cast<int>(m_vTimeRangeData.size()));
	//std::vector<CRunwayTakeOffTimeRangeData *>::iterator iter = m_vTimeRangeData.begin();
	//for (;iter != m_vTimeRangeData.end();++iter)
	//{
	//	(*iter)->ExportData(exportFile);
	//}
}
void CRunwayTakeOffPositionData::ImportData(CAirsideImportFile& importFile,int nRwMKDataID)
{
	//int nOldID = -1;
	//importFile.getFile().getInteger(nOldID);

	//int nOldTaixayID = -1;
	//importFile.getFile().getInteger(nOldTaixayID);
	//int nTaxiwayID = importFile.GetObjectNewID(nOldTaixayID);

	//int nOldOrder = -1;
	//importFile.getFile().getInteger(nOldOrder);
	//int nOrder = importFile.GetIntersectionIndexMap(nOldOrder);

	//importFile.getFile().getInteger(m_nIntType);
	//importFile.getFile().getInteger(m_bEligible);
	//importFile.getFile().getFloat(m_nRemainFt);
	//SaveData(nRwMKDataID,nTaxiwayID,nOrder);
	//importFile.AddTakeoffpositionRwmkdataIndexMap(nOldID,m_nID);

	//importFile.getFile().getLine();

	//int nTimeRangeDataCount = 0;
	//importFile.getFile().getInteger(nTimeRangeDataCount);
	//for (int i =0; i < nTimeRangeDataCount; ++i)
	//{
	//	CRunwayTakeOffTimeRangeData data;
	//	data.ImportData(importFile,m_nID);
	//}
}

////////////////////////////////////////////////////////////////////////////
////CRunwayTakeOffRMTaxiwayData
//CRunwayTakeOffRMTaxiwayData::CRunwayTakeOffRMTaxiwayData()
//:m_pIntersectObject(NULL)
//,m_pAirportDB(NULL)
//{
//
//}
//CRunwayTakeOffRMTaxiwayData::~CRunwayTakeOffRMTaxiwayData()
//{
//	for (int i = 0; i < (int)m_vInterData.size(); i++ )
//	{
//		CRunwayTakeOffPositionData* pData = m_vInterData.at(i);
//		if (pData)
//		{
//			delete pData;
//			//pData = NULL;
//		}
//	}
//	m_vInterData.clear();
//
//	ASSERT(m_pIntersectObject);
//	delete m_pIntersectObject;
//	m_pIntersectObject = NULL;
//
//}
//void CRunwayTakeOffRMTaxiwayData::ReadData(int nParentID)
//{
//	CString strSQL;
//	strSQL.Format(_T("SELECT * \
//					 FROM IN_RUNWAY_TAKEOFFRWMKDATA\
//					 WHERE (RWMARKDATAID = %d AND TAXIWAYID =%d)"),nParentID, m_pIntersectObject->getID());
//
//	int nCount = (int)m_vInterData.size();
//	long lRecordCount = 0;
//	CADORecordset adoRecordset;
//	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
//	while(!adoRecordset.IsEOF())
//	{
//		int nIntersecNodeID;
//		adoRecordset.GetFieldValue(_T("INTERSECTNODEID"),nIntersecNodeID);
//		CRunwayTakeOffPositionData* pData = NULL;
//		for (int i = 0; i < nCount; i++)
//		{
//			CRunwayTakeOffPositionData* pIntersection = m_vInterData.at(i);
//			if (pIntersection->GetIntersecNodeID() == nIntersecNodeID)
//			{
//				pData = pIntersection;
//				break;
//			}
//		}
//		
//		if (pData != NULL)
//		{
//			pData->SetAirportDB(m_pAirportDB);
//			pData->ReadData(adoRecordset);
//
//		}
//		adoRecordset.MoveNextData();
//	}
//
//}
////void CRunwayTakeOffRMTaxiwayData::InitializeData()
////{
////	m_pIntersectObject->ReadObject(m_pIntersectObject.getID());
////
////}
//void CRunwayTakeOffRMTaxiwayData::SaveData(int nRWMKDataID)
//{
////	int nOrder = 0;
//	std::vector<CRunwayTakeOffPositionData *>::iterator iter = m_vInterData.begin();
//	for (;iter != m_vInterData.end();++iter)
//	{
////		nOrder+=1;
//		if(-1 == (*iter)->getID())
//			(*iter)->SaveData(nRWMKDataID,m_pIntersectObject->getID(),(*iter)->GetIntersecNodeID());
//		else
//			(*iter)->UpdateData(nRWMKDataID,m_pIntersectObject->getID(),(*iter)->GetIntersecNodeID());
//	}
//}
//void CRunwayTakeOffRMTaxiwayData::DeleteData()
//{
//	std::vector<CRunwayTakeOffPositionData *>::iterator iter = m_vInterData.begin();
//	for (;iter != m_vInterData.end();++iter)
//	{
//		(*iter)->DeleteData();
//	}
//}
//
////bool CRunwayTakeOffRMTaxiwayData::InsertDataToStructure(CRunwayTakeOffPositionData *pIntersecData)
////{
////	std::vector<CRunwayTakeOffPositionData *> ::iterator iter = m_vInterData.begin();
////	for (;iter != m_vInterData.end(); ++iter)
////	{
////		if(pIntersecData->GetIntersecNodeID() == (*iter)->GetIntersecNodeID())
////		{
////			(*iter)->CopyData(pIntersecData);
////			pIntersecData->Clear();
////			return true;
////		//	break;
////		}
////	}
////	return false;
////}
//
//void CRunwayTakeOffRMTaxiwayData::InitTakeoffPostionsData(IntersectionNodeList& intersectNodeList)
//{
//	IntersectionNodeList::iterator iter = intersectNodeList.begin();
//
//	for (;iter != intersectNodeList.end(); ++ iter)
//	{
//		CRunwayTakeOffPositionData *pData = new CRunwayTakeOffPositionData;
//
//		pData->InitTakeoffPostionsData(*iter, m_pIntersectObject->getID());
//		m_vInterData.push_back(pData);
//	}
//	
//}
//
//CRunwayTakeOffPositionData* CRunwayTakeOffRMTaxiwayData::GetInterItem(int nIndex)
//{
//	ASSERT(nIndex >= 0 && nIndex < (int)m_vInterData.size());
//
//	return m_vInterData[nIndex];
//}
//void CRunwayTakeOffRMTaxiwayData::ExportData(CAirsideExportFile& exportFile)
//{
//	exportFile.getFile().writeInt(static_cast<int>(m_vInterData.size()));
//	std::vector<CRunwayTakeOffPositionData *>::iterator iter = m_vInterData.begin();
//	for (;iter != m_vInterData.end();++iter)
//	{
//		(*iter)->ExportData(exportFile,m_pIntersectObject->getID(),(*iter)->GetIntersecNodeID());
//	}
//}
//void CRunwayTakeOffRMTaxiwayData::ImportData(CAirsideImportFile& importFile,int nRWMKDataID)
//{
//	int nInterDataCount = 0;
//	importFile.getFile().getInteger(nInterDataCount);
//	for (int i =0; i < nInterDataCount; ++i)
//	{
//		CRunwayTakeOffPositionData data;
//		data.ImportData(importFile,nRWMKDataID);
//	}
//}


//////////////////////////////////////////////////////////////////////////
//CRunwayTakeOffRunwayWithMarkData
CRunwayTakeOffRunwayWithMarkData::CRunwayTakeOffRunwayWithMarkData(int nPrjID)
:m_nID(-1)
,m_pAirportDB(NULL)
,m_nPrjID(nPrjID)
,m_nRunwayID(-1)
,m_nMarkIndex(-1)
{
	m_vRwyTakeoffPosData.clear();
	m_vInvalidRwyTakeoffPosData.clear();
}

CRunwayTakeOffRunwayWithMarkData::~CRunwayTakeOffRunwayWithMarkData()
{
	for (int i = 0; i < (int)m_vRwyTakeoffPosData.size(); i++)
	{
		CRunwayTakeOffPositionData* pData = m_vRwyTakeoffPosData.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vRwyTakeoffPosData.clear();

	for (int i = 0; i < (int)m_vInvalidRwyTakeoffPosData.size(); i++)
	{
		CRunwayTakeOffPositionData* pData = m_vInvalidRwyTakeoffPosData.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vInvalidRwyTakeoffPosData.clear();
}

void CRunwayTakeOffRunwayWithMarkData::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("RUNWAYID"),m_nRunwayID);
	adoRecordset.GetFieldValue(_T("RUNWAYMKINDEX"),m_nMarkIndex);
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	ReadTakeoffPostionData();
	//ReadData();

}

void CRunwayTakeOffRunwayWithMarkData::ReadTakeoffPostionData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT * \
						FROM IN_RUNWAY_TAKEOFFRWMKDATA\
						WHERE (RWMARKDATAID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CRunwayTakeOffPositionData* pData = new CRunwayTakeOffPositionData;
		pData->SetAirportDB(m_pAirportDB);
		pData->ReadData(adoRecordset);
		AddRwyTakeoffPosData(pData);

		adoRecordset.MoveNextData();
	}
}

void CRunwayTakeOffRunwayWithMarkData::SaveData(int nRunwayID,int nMarkIndex)
{
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO IN_RUNWAY_TAKEOFFPOSITION\
						 (PROJID, RUNWAYID, RUNWAYMKINDEX)\
						 VALUES (%d,%d,%d)"),m_nPrjID,nRunwayID,nMarkIndex);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

	std::vector<CRunwayTakeOffPositionData *>::iterator iter = m_vInvalidRwyTakeoffPosData.begin();

	for (;iter != m_vInvalidRwyTakeoffPosData.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vInvalidRwyTakeoffPosData.clear();

	iter = m_vRwyTakeoffPosData.begin();

	for (;iter != m_vRwyTakeoffPosData.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}
}

void CRunwayTakeOffRunwayWithMarkData::DeleteData()
{
	CString strSQL ;
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_TAKEOFFPOSITION\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CRunwayTakeOffPositionData *>::iterator iter = m_vRwyTakeoffPosData.begin();

	for (;iter != m_vRwyTakeoffPosData.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vRwyTakeoffPosData.clear();
}
//void CRunwayTakeOffRunwayWithMarkData::InitTakeoffPostionsData(std::map<int,IntersectionNodeList>& mapIntersectNode)
//{
//	typedef std::pair<DistanceUnit, int> toSortPair;
//	typedef std::vector<toSortPair> toSortVector;
//	typedef std::numeric_limits<DistanceUnit> here_limits_class;
//
//	toSortVector vecToSort;
//
//	// to push_back all item's distance and id as a pair into a std::vector
//	// and sort them by distance
//	ASSERT(0 == m_nMarkIndex || 1 == m_nMarkIndex);
//	if (0 == m_nMarkIndex)  // from start
//	{
//		std::map<int,IntersectionNodeList>::iterator iterMap;
//		for (iterMap = mapIntersectNode.begin();iterMap != mapIntersectNode.end(); ++iterMap)
//		{
//			DistanceUnit dist = (std::numeric_limits<DistanceUnit>::max)();/*why this cannot be used?*/
//			const IntersectionNodeList& nodeList = iterMap->second;
//			IntersectionNodeList::const_iterator ite = nodeList.begin();
//			IntersectionNodeList::const_iterator iteEn = nodeList.end();
//			for (;ite!=iteEn;ite++) // find min distance intersection
//			{
//				DistanceUnit tmp = ite->GetDistance(m_nRunwayID);
//				dist = MIN(dist, tmp);
//			}
//			// record it
//			vecToSort.push_back(toSortPair(dist, iterMap->first));
//		}
//		// sort the vector in ascend order
//		std::sort(vecToSort.begin(), vecToSort.end(), std::less<toSortPair>());
//	}
//	else  // from end
//	{
//		std::map<int,IntersectionNodeList>::iterator iterMap;
//		for (iterMap = mapIntersectNode.begin();iterMap != mapIntersectNode.end(); ++iterMap)
//		{
//			DistanceUnit dist = (std::numeric_limits<DistanceUnit>::min)();
//			const IntersectionNodeList& nodeList = iterMap->second;
//			IntersectionNodeList::const_iterator ite = nodeList.begin();
//			IntersectionNodeList::const_iterator iteEn = nodeList.end();
//			for (;ite!=iteEn;ite++) // find min distance intersection
//			{
//				DistanceUnit tmp = ite->GetDistance(m_nRunwayID);
//				dist = MAX(dist, tmp);
//			}
//			// record it
//			vecToSort.push_back(toSortPair(dist, iterMap->first));
//		}
//		// sort the vector in descend order
//		std::sort(vecToSort.begin(), vecToSort.end(), std::greater<toSortPair>());
//	}
//
//
//	// now init the data by the distance
//	toSortVector::iterator ite = vecToSort.begin();
//	toSortVector::iterator iteEn = vecToSort.end();
//    for (;ite != iteEn; ++ite)
//	{
//		IntersectionNodeList& nodeList = mapIntersectNode[ite->second];
//		ALTObject *pIntersectObject  = ALTObject::ReadObjectByID(ite->second);
//
//		CRunwayTakeOffRMTaxiwayData *pMarkData = new CRunwayTakeOffRMTaxiwayData;
//		pMarkData->setIntersectObject(pIntersectObject);
//		pMarkData->SetAirportDB(m_pAirportDB);
//		pMarkData->InitData(nodeList);
//		pMarkData->ReadData(m_nID);
//		AddData(pMarkData);
//	}
//}



CRunwayTakeOffPositionData* CRunwayTakeOffRunwayWithMarkData::GetRwyTakeoffPosItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vRwyTakeoffPosData.size());

	return m_vRwyTakeoffPosData[nIndex];
}

void CRunwayTakeOffRunwayWithMarkData::ExportData(CAirsideExportFile& exportFile,int nRunwayID,int nMarkIndex)
{
}
void CRunwayTakeOffRunwayWithMarkData::ImportData(CAirsideImportFile& importFile)
{
}

void CRunwayTakeOffRunwayWithMarkData::AddRwyTakeoffPosData( CRunwayTakeOffPositionData *pData )
{
	m_vRwyTakeoffPosData.push_back(pData);
}

void CRunwayTakeOffRunwayWithMarkData::UpdateRwyTakeoffPosData(const RunwayExitList& exitList)
{
	for (size_t i = 0; i < exitList.size(); i++)
	{
		const RunwayExit& exit = exitList.at(i);
		if (!IfRwyTakeoffPosDataIncludeRunwayExit(exit))
		{
			CRunwayTakeOffPositionData* pPosItem = new CRunwayTakeOffPositionData;
			pPosItem->SetExitID(exit);
			AddRwyTakeoffPosData(pPosItem);
		}
	}
}

bool CRunwayTakeOffRunwayWithMarkData::IfInvalidData(CRunwayTakeOffPositionData* pPosItem,const RunwayExitList& exitList)
{
	size_t nSize = exitList.size();
	
	for (size_t j =0; j < nSize; j++)
	{
		RunwayExit exit = exitList.at(j);
		if (exit.EqualToRunwayExitDescription(pPosItem->GetRunwayExitDescription()))
		{
			return false;
		}
	}
	return true;
}

void CRunwayTakeOffRunwayWithMarkData::KeepInvalidData()
{
	if (m_vInvalidRwyTakeoffPosData.size() > 0)
	{
		m_vRwyTakeoffPosData.insert(m_vRwyTakeoffPosData.end(),m_vInvalidRwyTakeoffPosData.begin(),m_vInvalidRwyTakeoffPosData.end());
		m_vInvalidRwyTakeoffPosData.clear();
	}
}

void CRunwayTakeOffRunwayWithMarkData::RemoveInvalidData(const RunwayExitList& exitList)
{
	size_t nSize = exitList.size();
	int nCount = GetRwyTakeoffPosCount();
	std::vector<CRunwayTakeOffPositionData*> vList;
	vList.clear();
	for (int i =0; i < nCount; i++)
	{
		CRunwayTakeOffPositionData* pPosItem = GetRwyTakeoffPosItem(i);
		if (IfInvalidData(pPosItem,exitList))
		{
			vList.push_back(pPosItem);
		}
	}

	for (int ii = 0; ii < (int)vList.size(); ii++)
	{
		CRunwayTakeOffPositionData* pData = vList.at(ii);
		std::vector<CRunwayTakeOffPositionData*>::iterator iter = std::find(m_vRwyTakeoffPosData.begin(),m_vRwyTakeoffPosData.end(),pData);
		if (iter != m_vRwyTakeoffPosData.end())
		{
			m_vRwyTakeoffPosData.erase(iter);
			m_vInvalidRwyTakeoffPosData.push_back(pData);
		}
	}
}

bool CRunwayTakeOffRunwayWithMarkData::IfRwyTakeoffPosDataIncludeRunwayExit(const RunwayExit& exit)
{
	std::vector<CRunwayTakeOffPositionData *>::iterator iter = m_vRwyTakeoffPosData.begin();

	for (;iter != m_vRwyTakeoffPosData.end(); ++iter)
	{
		if(exit.EqualToRunwayExitDescription((*iter)->GetRunwayExitDescription()))
		{
			return true;
		}
	}
	return false;
}

void CRunwayTakeOffRunwayWithMarkData::DelRwyTakeoffPosData()
{
	m_vInvalidRwyTakeoffPosData.assign(m_vRwyTakeoffPosData.begin(),m_vRwyTakeoffPosData.end());
	m_vRwyTakeoffPosData.clear();
}

//////////////////////////////////////////////////////////////////////////
//CRunwayTakeOffRunwayData

CRunwayTakeOffRunwayData::CRunwayTakeOffRunwayData()
:m_nID(-1)
,m_nAirportDataID(-1)
,m_pRunwayMark1Data(NULL)
,m_pRunwayMark2Data(NULL)
,m_pAirportDB(NULL)
{
}

CRunwayTakeOffRunwayData::~CRunwayTakeOffRunwayData()
{
	if(m_pRunwayMark1Data)
		delete m_pRunwayMark1Data;
	m_pRunwayMark1Data = NULL;

	if (m_pRunwayMark2Data)
		delete m_pRunwayMark2Data;
	m_pRunwayMark2Data = NULL;

}

void CRunwayTakeOffRunwayData::ReadData(int nPrjID)
{

	CString strSQL;
	strSQL.Format(_T("SELECT ID, RUNWAYID, RUNWAYMKINDEX\
						FROM IN_RUNWAY_TAKEOFFPOSITION\
						WHERE (PROJID = %d AND RUNWAYID = %d)"),nPrjID, m_runway.getID());


	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);

	if (adoRecordset.IsEOF())
	{
		m_pRunwayMark1Data = new CRunwayTakeOffRunwayWithMarkData(nPrjID);
		m_pRunwayMark1Data->SetAirportDB(m_pAirportDB);
		m_pRunwayMark1Data->setRunwayID(m_runway.getID());
		m_pRunwayMark1Data->setRunwayMarkIndex(0);

		m_pRunwayMark2Data = new CRunwayTakeOffRunwayWithMarkData(nPrjID);
		m_pRunwayMark2Data->SetAirportDB(m_pAirportDB);
		m_pRunwayMark2Data->setRunwayID(m_runway.getID());
		m_pRunwayMark2Data->setRunwayMarkIndex(1);

	}
	else
	{
		while(!adoRecordset.IsEOF())
		{
			CRunwayTakeOffRunwayWithMarkData *pRunwayMarkData = new CRunwayTakeOffRunwayWithMarkData(nPrjID);
			pRunwayMarkData->SetAirportDB(m_pAirportDB);
			pRunwayMarkData->ReadData(adoRecordset);
			InsertDataToStructure(pRunwayMarkData);
			adoRecordset.MoveNextData();
		}

		if(m_pRunwayMark1Data == NULL)
		{
			m_pRunwayMark1Data = new CRunwayTakeOffRunwayWithMarkData(nPrjID);
			m_pRunwayMark1Data->SetAirportDB(m_pAirportDB);
			m_pRunwayMark1Data->setRunwayID(m_runway.getID());
			m_pRunwayMark1Data->setRunwayMarkIndex(0);
		}

		if(m_pRunwayMark2Data == NULL)
		{
			m_pRunwayMark2Data = new CRunwayTakeOffRunwayWithMarkData(nPrjID);
			m_pRunwayMark2Data->SetAirportDB(m_pAirportDB);
			m_pRunwayMark2Data->setRunwayID(m_runway.getID());
			m_pRunwayMark2Data->setRunwayMarkIndex(1);
		}
	}
}

void CRunwayTakeOffRunwayData::SaveData(int nProjectID)
{
	m_pRunwayMark1Data->SaveData(m_runway.getID(),0);
	m_pRunwayMark2Data->SaveData(m_runway.getID(),1);
}
void CRunwayTakeOffRunwayData::DeleteData()
{
	m_pRunwayMark1Data->DeleteData();
	m_pRunwayMark2Data->DeleteData();
}

void CRunwayTakeOffRunwayData::UpdateData(int nAirportDataID)
{

}
bool CRunwayTakeOffRunwayData::InsertDataToStructure(CRunwayTakeOffRunwayWithMarkData *pRunwayMarkData)
{
	if (pRunwayMarkData == NULL)
		return false;

	if (m_runway.getID() == pRunwayMarkData->getRunwayID())
	{
		if (pRunwayMarkData->getRunwayMarkIndex() == 0)
		{
			if (m_pRunwayMark1Data)
			{
				delete m_pRunwayMark1Data;
				m_pRunwayMark1Data = NULL;
			}
			m_pRunwayMark1Data = pRunwayMarkData;

		}
		else
		{
			if (m_pRunwayMark2Data)
			{
				delete m_pRunwayMark2Data;
				m_pRunwayMark2Data = NULL;
			}
			m_pRunwayMark2Data = pRunwayMarkData;
		}

		return true;
	}


	return false;
}

//void CRunwayTakeOffRunwayData::InitData()
//{
//
//	//get runway's intersection node and push into taxiway vector
//	IntersectionNodeList intersecNodeList;
//	std::vector<ALTOBJECT_TYPE > vAltObjectType;
//	vAltObjectType.push_back(ALT_TAXIWAY);
//	vAltObjectType.push_back(ALT_RUNWAY);
//	IntersectionNode::ReadIntersecNodeListWithObjType(m_runway.getID(),vAltObjectType,intersecNodeList);
//	//taxiway id, intersectnode list
//	std::map<int,IntersectionNodeList> mapIntersectNode;
//	for (IntersectionNodeList::iterator iter= intersecNodeList.begin();  iter != intersecNodeList.end(); ++iter )
//	{
//		std::vector<TaxiwayIntersectItem*> vTaxiItems = iter->GetTaxiwayIntersectItemList();
//		for(int j=0;j< (int)vTaxiItems.size();j++)
//		{
//			TaxiwayIntersectItem* theIntersectItem = vTaxiItems[j];
//			if(theIntersectItem)
//				mapIntersectNode[theIntersectItem->GetObjectID()].push_back(*iter);
//		}
//		std::vector<RunwayIntersectItem *> vRunwayItems = iter->GetRunwayIntersectItemList();
//		for(int i=0;i< (int)vRunwayItems.size();i++)
//		{
//			RunwayIntersectItem* theIntersectItem = vRunwayItems[i];
//			if(theIntersectItem)
//				if(theIntersectItem->GetObjectID() != m_runway.getID())
//					mapIntersectNode[theIntersectItem->GetObjectID()].push_back(*iter);
//		}
//
//	}
//	if (m_pRunwayMark1Data != NULL && m_pRunwayMark2Data != NULL)
//	{
//		m_pRunwayMark1Data->InitTakeoffPostionsData(mapIntersectNode);
//		m_pRunwayMark2Data->InitTakeoffPostionsData(mapIntersectNode);
//	}
//}
void CRunwayTakeOffRunwayData::ExportData(CAirsideExportFile& exportFile)
{
	//m_runwayMark1Data.ExportData(exportFile,m_runway.getID(),0);
	//m_runwayMark2Data.ExportData(exportFile,m_runway.getID(),1);
}
void CRunwayTakeOffRunwayData::ImportData(CAirsideImportFile& importFile)
{
	//CRunwayTakeOffRunwayWithMarkData runwayMark1Data;
	//runwayMark1Data.ImportData(importFile);
	//CRunwayTakeOffRunwayWithMarkData runwayMark2Data;
	//runwayMark2Data.ImportData(importFile);
}
//////////////////////////////////////////////////////////////////////////
//CRunwayTakeOffAirportData

CRunwayTakeOffAirportData::~CRunwayTakeOffAirportData()
{
	std::vector<CRunwayTakeOffRunwayData *>::iterator ite = m_vRunwayTakeoffData.begin();
	std::vector<CRunwayTakeOffRunwayData *>::iterator iteEn = m_vRunwayTakeoffData.end();
	for (;ite!=iteEn;ite++)
	{
		delete (*ite);
	}
	m_vRunwayTakeoffData.clear();

	ite = m_vInvalidRunwayTakeoffData.begin();
	iteEn = m_vInvalidRunwayTakeoffData.end();
	for (;ite!=iteEn;ite++)
	{
		delete (*ite);
	}
	m_vInvalidRunwayTakeoffData.clear();
}
void CRunwayTakeOffAirportData::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);

	int nAirportID = -1;
	adoRecordset.GetFieldValue(_T("AIRPORTID"),nAirportID);

	setAirportID(nAirportID);
}
void CRunwayTakeOffAirportData::setAirport(const ALTAirport& airport)
{
	m_airport = airport;
}


void CRunwayTakeOffAirportData::SaveData(int nProjectID)
{
//	m_nProjectID = nProjectID;
	std::vector<CRunwayTakeOffRunwayData *>::iterator iter  = m_vRunwayTakeoffData.begin();
	std::vector<CRunwayTakeOffRunwayData *>::iterator iterEnd  = m_vRunwayTakeoffData.end();

	for (;iter != iterEnd;++iter)
	{
		(*iter)->SaveData(nProjectID);
	}
}
void CRunwayTakeOffAirportData::DeleteData()
{


}
void CRunwayTakeOffAirportData::UpdateData(int nProjectID)
{

}
void CRunwayTakeOffAirportData::InitData(int nPrjID)
{	
	std::vector<int> vRunwayID;
	ALTAirport::GetRunwaysIDs(m_airport.getID(),vRunwayID);
	
	std::vector<int>::size_type i = 0;
	for (; i < vRunwayID.size(); ++i)
	{
		Runway runway;
		runway.ReadObject(vRunwayID[i]);

		CRunwayTakeOffRunwayData *pRunwayData = new CRunwayTakeOffRunwayData();
		pRunwayData->setRunway(runway);
		pRunwayData->SetAirportDB(m_pAirportDB);
		pRunwayData->ReadData(nPrjID);
		//pRunwayData->InitData();
		m_vRunwayTakeoffData.push_back(pRunwayData);
	}


}

//bool CRunwayTakeOffAirportData::InsertDataToStructure(CRunwayTakeOffRunwayWithMarkData *pRunwayMarkData)
//{
//	
//	for (int i =0; i < (int)m_vRunwayTakeoffData.size(); ++i)
//	{
//		if(m_vRunwayTakeoffData[i]->InsertDataToStructure(pRunwayMarkData))
//			return true;
//	}
//
//	return false;
//}

CRunwayTakeOffRunwayData* CRunwayTakeOffAirportData::GetRunwayItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vRunwayTakeoffData.size());

	return m_vRunwayTakeoffData[nIndex];
}
void CRunwayTakeOffAirportData::ExportData(CAirsideExportFile& exportFile)
{
	//	m_nProjectID = nProjectID;

	//exportFile.getFile().writeInt(static_cast<int>(m_vRunwayTakeoffData.size()));
	//std::vector<CRunwayTakeOffRunwayData *>::iterator iter  = m_vRunwayTakeoffData.begin();
	//std::vector<CRunwayTakeOffRunwayData *>::iterator iterEnd  = m_vRunwayTakeoffData.end();

	//for (;iter != iterEnd;++iter)
	//{
	//	(*iter)->ExportData(exportFile);
	//}
}
void CRunwayTakeOffAirportData::ImportData(CAirsideImportFile& importFile)
{
	//int nRunwayTakeoffDataCount = 0;
	//importFile.getFile().getInteger(nRunwayTakeoffDataCount);
	//for (int i =0; i < nRunwayTakeoffDataCount; ++i)
	//{
	//	CRunwayTakeOffRunwayData data;
	//	data.ImportData(importFile);
	//}
}
//////////////////////////////////////////////////////////////////////////
//CRunwayTakeOffPosition
CRunwayTakeOffPosition::CRunwayTakeOffPosition(int nProjID)
{
	m_nProjectID = nProjID;
	//InitTakeOffPosition();
}

CRunwayTakeOffPosition::~CRunwayTakeOffPosition(void)
{
	std::vector<CRunwayTakeOffAirportData *>::iterator iter  = m_vAirportData.begin();
	std::vector<CRunwayTakeOffAirportData *>::iterator iterEn  = m_vAirportData.end();

	for (;iter != iterEn;++iter)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vAirportData.clear();


	iter  = m_vInvalidAirportData.begin();
	iterEn  = m_vInvalidAirportData.end();
	for (;iter!=iterEn;++iter)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vInvalidAirportData.clear();
}
void CRunwayTakeOffPosition::InitTakeOffPosition()
{
	std::vector<ALTAirport> vAirport;
	ALTAirport::GetAirportList(m_nProjectID,vAirport);

	std::vector<ALTAirport>::iterator iter = vAirport.begin();

	bool bFind = false;
	for (;iter != vAirport.end(); ++iter)
	{
		CRunwayTakeOffAirportData *pAirportData = new CRunwayTakeOffAirportData();
		pAirportData->setAirport(*iter);
		pAirportData->SetAirportDB(m_pAirportDB);
		pAirportData->InitData(m_nProjectID);

		m_vAirportData.push_back(pAirportData);
	}
}
void CRunwayTakeOffPosition::ReadData()
{
	InitTakeOffPosition();

	//std::vector<CRunwayTakeOffRunwayWithMarkData *> vRunwayMarkData; 

	//CString strSQL;
	//strSQL.Format(_T("SELECT ID, RUNWAYID, RUNWAYMKINDEX\
	//	FROM IN_RUNWAY_TAKEOFFPOSITION\
	//	WHERE (PROJID = %d)"),m_nProjectID);


	//long lRecordCount = 0;
	//CADORecordset adoRecordset;
	//
	//CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	//while(!adoRecordset.IsEOF())
	//{
	//	CRunwayTakeOffRunwayWithMarkData *pRunwayData = new CRunwayTakeOffRunwayWithMarkData;
	//	pRunwayData->SetAirportDB(m_pAirportDB);
	//	pRunwayData->ReadData(adoRecordset);
	//	
	//	vRunwayMarkData.push_back(pRunwayData);
	//	adoRecordset.MoveNextData();
	//}

	//InsertDataIntoStructure(vRunwayMarkData);

}	
//void CRunwayTakeOffPosition::InsertDataIntoStructure(std::vector<CRunwayTakeOffRunwayWithMarkData *>& vRunwayMakeData)
//{
//	std::vector<CRunwayTakeOffRunwayWithMarkData *>::iterator iter = vRunwayMakeData.begin();
//
//	while (iter != vRunwayMakeData.end())
//	{
//		
//		for (int i =0; i < (int)m_vAirportData.size(); ++i)
//		{
//			if(m_vAirportData[i]->InsertDataToStructure(*iter))
//			{
//				break;
//			}
//
//		}
//
//		delete *iter;
//		vRunwayMakeData.erase(iter);
//		iter = vRunwayMakeData.begin();
//	}
//
//}
void CRunwayTakeOffPosition::SaveData()
{
	std::vector<CRunwayTakeOffAirportData *>::iterator iter  = m_vAirportData.begin();
	std::vector<CRunwayTakeOffAirportData *>::iterator iterEnd  = m_vAirportData.end();

	for (;iter != iterEnd;++iter)
	{
		(*iter)->SaveData(m_nProjectID);
	}
}

CRunwayTakeOffAirportData* CRunwayTakeOffPosition::GetAirportItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vAirportData.size());

	return m_vAirportData[nIndex];
}
void CRunwayTakeOffPosition::ExportRunwayTakeOffPosition(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
	//CRunwayTakeOffPosition runwayTakeOffPosition(exportFile.GetProjectID());
	//runwayTakeOffPosition.SetAirportDB(pAirportDB);
	//runwayTakeOffPosition.ReadData();
	//runwayTakeOffPosition.ExportData(exportFile);
	//exportFile.getFile().endFile();
}
void CRunwayTakeOffPosition::ExportData(CAirsideExportFile& exportFile)
{		
	//exportFile.getFile().writeField("RunwayTakeOffPosition");
	//exportFile.getFile().writeLine();
	//std::vector<CRunwayTakeOffAirportData *>::iterator iter = m_vAirportData.begin();
	//for (; iter!=m_vAirportData.end(); iter++)
	//{
	//	(*iter)->ExportData(exportFile);
	//}
}
void CRunwayTakeOffPosition::ImportRunwayTakeOffPosition(CAirsideImportFile& importFile)
{
	//while(!importFile.getFile().isBlank())
	//{
	//	CRunwayTakeOffAirportData runwayTakeOffAirportData;
	//	runwayTakeOffAirportData.ImportData(importFile);
	//}
}

bool CRunwayTakeOffPosition::IfHaveInvalidData()
{
	for (int i=0; i<GetAirportCount(); i++)
	{
		CRunwayTakeOffAirportData* pAirtPortItem = GetAirportItem(i);
		ALTAirport* altAirport = pAirtPortItem->getAirport();
		for (int j=0; j<pAirtPortItem->GetRunwayCount(); j++)
		{
			CRunwayTakeOffRunwayData* pRunwayItem = pAirtPortItem->GetRunwayItem(j);
			CRunwayTakeOffRunwayWithMarkData* runwayMark1Data = pRunwayItem->getRunwayMark1Data(); 
			CRunwayTakeOffRunwayWithMarkData* runwayMark2Data = pRunwayItem->getRunwayMark2Data(); 

			if (runwayMark1Data->GetInvalidTakeoffPos().size() > 0 || runwayMark2Data->GetInvalidTakeoffPos().size() > 0)
			{
				return true;
			}
		}
	}
	return false;
}

void CRunwayTakeOffPosition::KeepInvalidData()
{
	for (int i=0; i<GetAirportCount(); i++)
	{
		CRunwayTakeOffAirportData* pAirtPortItem = GetAirportItem(i);
		ALTAirport* altAirport = pAirtPortItem->getAirport();
		for (int j=0; j<pAirtPortItem->GetRunwayCount(); j++)
		{
			CRunwayTakeOffRunwayData* pRunwayItem = pAirtPortItem->GetRunwayItem(j);
			CRunwayTakeOffRunwayWithMarkData* runwayMark1Data = pRunwayItem->getRunwayMark1Data(); 
			CRunwayTakeOffRunwayWithMarkData* runwayMark2Data = pRunwayItem->getRunwayMark2Data(); 

			runwayMark1Data->KeepInvalidData();
			runwayMark2Data->KeepInvalidData();
		}
	}
}

void CRunwayTakeOffPosition::RemoveInvalidData()
{
// 	for (int i=0; i<GetAirportCount(); i++)
// 	{
// 		CRunwayTakeOffAirportData* pAirtPortItem = GetAirportItem(i);
// 		ALTAirport* altAirport = pAirtPortItem->getAirport();
// 		for (int j=0; j<pAirtPortItem->GetRunwayCount(); j++)
// 		{
// 			CRunwayTakeOffRunwayData* pRunwayItem = pAirtPortItem->GetRunwayItem(j);
// 			CRunwayTakeOffRunwayWithMarkData* runwayMark1Data = pRunwayItem->getRunwayMark1Data(); 
// 			CRunwayTakeOffRunwayWithMarkData* runwayMark2Data = pRunwayItem->getRunwayMark2Data(); 
// 
// 			runwayMark1Data->RemoveInvalidData(GetRunwayExit(runwayMark1Data));
// 			runwayMark2Data->RemoveInvalidData(GetRunwayExit(runwayMark2Data));
// 		}
// 	}
}

void CRunwayTakeOffPosition::UpdateRwyTakeoffPosData()
{
	for (int i=0; i<GetAirportCount(); i++)
	{
		CRunwayTakeOffAirportData* pAirtPortItem = GetAirportItem(i);
		ALTAirport* altAirport = pAirtPortItem->getAirport();
		for (int j=0; j<pAirtPortItem->GetRunwayCount(); j++)
		{
			CRunwayTakeOffRunwayData* pRunwayItem = pAirtPortItem->GetRunwayItem(j);
			CRunwayTakeOffRunwayWithMarkData* runwayMark1Data = pRunwayItem->getRunwayMark1Data(); 
			CRunwayTakeOffRunwayWithMarkData* runwayMark2Data = pRunwayItem->getRunwayMark2Data(); 

			RunwayExitList list1 = GetRunwayExit(runwayMark1Data);
			RunwayExitList list2 = GetRunwayExit(runwayMark2Data);
			runwayMark1Data->UpdateRwyTakeoffPosData(list1);
			runwayMark2Data->UpdateRwyTakeoffPosData(list2);

			runwayMark1Data->RemoveInvalidData(list1);
			runwayMark2Data->RemoveInvalidData(list2);
		}
	}
}
RunwayExitList CRunwayTakeOffPosition::GetRunwayExit(CRunwayTakeOffRunwayWithMarkData* runwayMark1Data)const
{
	Runway altRunway;
	altRunway.ReadObject(runwayMark1Data->getRunwayID());

	RUNWAY_MARK rwyMark;
	if(runwayMark1Data->getRunwayMarkIndex() == 0)
		rwyMark = RUNWAYMARK_FIRST;
	else
		rwyMark = RUNWAYMARK_SECOND;

	RunwayExitList exitList;
	altRunway.GetExitList(rwyMark, exitList);

	return exitList;
}