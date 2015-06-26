#include "StdAfx.h"
#include "TakeoffQueuePosi.h"
#include "TakeoffQueuesItem.h"
#include "Runway.h"
#include "Taxiway.h"
#include "ALTAirport.h"
#include <algorithm>
#include "AirsideImportExportManager.h"
#include "RunwayExitDiscription.h"

TakeoffQueuePosi::TakeoffQueuePosi()
{
	m_nID = -1;
	//m_nTaxiwayID = -1;
	m_nTakeoffPosiInterID = -1;
}

TakeoffQueuePosi::~TakeoffQueuePosi()
{
	RemoveAll();
}
void TakeoffQueuePosi::InitData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("TAKEOFFPOSIINTERID"),m_nTakeoffPosiInterID);
	//adoRecordset.GetFieldValue(_T("TAXIWAYID"),m_nTaxiwayID);
	//int nRunwayExitDescription = -1;
	//adoRecordset.GetFieldValue(_T("RWEXITDESID"),nRunwayExitDescription);
	
	//m_runwayExitDescription.ReadData(nRunwayExitDescription);

	//int nRwMarkDataID;  
	//adoRecordset.GetFieldValue(_T("RWMARKDATAID"),nRwMarkDataID);

	/*CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_RUNWAY_TAKEOFFPOSITION\
					 WHERE (ID = %d)"),nRwMarkDataID);

	long lRecordCount = 0;
	CADORecordset adoRecordsetRun;*/
//	int nRunwayID = 0;
	//m_nRunwayID = 0;
//	int nRunwayMkIndex;
	//CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordsetRun);
	//while(!adoRecordsetRun.IsEOF())
	//{
	//	//adoRecordsetRun.GetFieldValue(_T("RUNWAYID"),m_nRunwayID);
	//	//adoRecordsetRun.GetFieldValue(_T("RUNWAYMKINDEX"),m_nRunwayMkIndex);
	//	//adoRecordsetRun.GetFieldValue(_T("PROJID"),m_nProjID);
	//	//if(m_nRunwayID != 0)
	//		//break;
	//	adoRecordsetRun.MoveNextData();
	//}
	//Taxiway taxiway;
	//taxiway.ReadObject(m_nTaxiwayID);
	//CString strTaxiwayMark = taxiway.GetMarking().c_str();
	//Runway runway;
	//runway.ReadObject(m_nRunwayID);
	//CString strRunwayMark;
	//if(m_nRunwayMkIndex == 0)
	//	strRunwayMark = runway.GetMarking1().c_str();
	//else
	//	strRunwayMark = runway.GetMarking2().c_str();
	//m_strRunTaxiway.Format("%s&%s",strRunwayMark,strTaxiwayMark);
}
void TakeoffQueuePosi::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("TAKEOFFPOSIINTERID"),m_nTakeoffPosiInterID);
	RunwayExit exit;
	exit.ReadData(m_nTakeoffPosiInterID);
	InitFromRunwayExit(exit);

	ReadNodeData();
}
void TakeoffQueuePosi::ReadNodeData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_TAKEOFFQUEUENODE\
					 WHERE (TAKEOFFQUEUEID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		TakeoffQueuesItem* pTakeoffQueuesItem = new TakeoffQueuesItem;
		pTakeoffQueuesItem->ReadData(adoRecordset);

		m_vTakeoffQueuesItem.push_back(pTakeoffQueuesItem);
		adoRecordset.MoveNextData();
	}
}
void TakeoffQueuePosi::SaveData(int nProjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFQUEUE\
					 (PROJID, TAKEOFFPOSIINTERID)\
					 VALUES (%d,%d)"),nProjID,m_nTakeoffPosiInterID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	TakeoffQueuesItemIter iter = m_vTakeoffQueuesItem.begin();

	for (;iter !=m_vTakeoffQueuesItem.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	for (iter = m_vInvalidTakeoffQueuesItem.begin(); iter !=m_vInvalidTakeoffQueuesItem.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
}
void TakeoffQueuePosi::UpdateData(int nProjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TAKEOFFQUEUE\
					 SET PROJID =%d, TAKEOFFPOSIINTERID =%d\
					 WHERE (ID = %d)"),nProjID,m_nTakeoffPosiInterID,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	TakeoffQueuesItemIter iter = m_vTakeoffQueuesItem.begin();

	for (;iter !=m_vTakeoffQueuesItem.end();++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	for (iter = m_vInvalidTakeoffQueuesItem.begin(); iter !=m_vInvalidTakeoffQueuesItem.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
}
int TakeoffQueuePosi::GetTakeoffQueuesItemCount()const
{
	return (int)m_vTakeoffQueuesItem.size();
}

TakeoffQueuesItem* TakeoffQueuePosi::GetItem(int nIndex)const
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vTakeoffQueuesItem.size());
	return m_vTakeoffQueuesItem[nIndex];
}

void TakeoffQueuePosi::AddTakeoffQueuesItem(TakeoffQueuesItem* pItem)
{
	m_vTakeoffQueuesItem.push_back(pItem);
}
void TakeoffQueuePosi::DeleteTakeoffQueuesItem(TakeoffQueuesItem* pItem)
{
	TakeoffQueuesItemIter iter = 
		std::find(m_vTakeoffQueuesItem.begin(),m_vTakeoffQueuesItem.end(), pItem);
	if (iter == m_vTakeoffQueuesItem.end())
		return;
	m_vInvalidTakeoffQueuesItem.push_back(pItem);
	m_vTakeoffQueuesItem.erase(iter);
}
void TakeoffQueuePosi::RemoveAll()
{
	for (TakeoffQueuesItemIter iter = m_vTakeoffQueuesItem.begin();
		iter != m_vTakeoffQueuesItem.end(); iter++)
	{
		delete (*iter);
		(*iter) = NULL;
	}
	m_vTakeoffQueuesItem.clear();
}
void TakeoffQueuePosi::CopyData(TakeoffQueuePosi* pTakeoffQueuePosi)
{
	*this = *pTakeoffQueuePosi;
}
void TakeoffQueuePosi::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_TAKEOFFQUEUE\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	TakeoffQueuesItemIter iter = m_vTakeoffQueuesItem.begin();

	for (;iter !=m_vTakeoffQueuesItem.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}

	for (iter = m_vInvalidTakeoffQueuesItem.begin(); iter !=m_vInvalidTakeoffQueuesItem.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
}
void TakeoffQueuePosi::DelFromVTakeoffPosi(TakeoffQueuesItem* pTakeoffQueuesItem)
{
	DelChildren(pTakeoffQueuesItem->GetUniID());
	TakeoffQueuesItemIter iter = std::find(m_vTakeoffQueuesItem.begin(), m_vTakeoffQueuesItem.end(), pTakeoffQueuesItem);
	if(iter == m_vTakeoffQueuesItem.end())
		return;

	m_vInvalidTakeoffQueuesItem.push_back(pTakeoffQueuesItem);
	m_vTakeoffQueuesItem.erase(iter);
}
void TakeoffQueuePosi::DelChildren(int nID)
{
	TakeoffQueuesItemIter iterchild = m_vTakeoffQueuesItem.begin();
	for (;iterchild != m_vTakeoffQueuesItem.end(); iterchild++)
	{
		if((*iterchild)->GetParentID() == nID)
		{
			DelFromVTakeoffPosi((*iterchild));		
			iterchild = m_vTakeoffQueuesItem.begin();
		}
	}
}
void TakeoffQueuePosi::UpdateVTakeoffPosi(TakeoffQueuesItem* pTakeoffQueuesItem)
{
	DelChildren(pTakeoffQueuesItem->GetUniID());
}

void TakeoffQueuePosi::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nTakeoffPosiInterID);
	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vTakeoffQueuesItem.size()));
	TakeoffQueuesItemIter iter = m_vTakeoffQueuesItem.begin();
	for (; iter!=m_vTakeoffQueuesItem.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
	/*
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFQUEUE\
	(PROJID, TAKEOFFPOSIINTERID)\
	VALUES (%d,%d)"),nProjID,m_nTakeoffPosiInterID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	*/
}

void TakeoffQueuePosi::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nOldTakeoffPosiInterID;
	importFile.getFile().getInteger(nOldTakeoffPosiInterID);
	m_nTakeoffPosiInterID = importFile.GetTakeoffpositionRwmkdataNewID(nOldTakeoffPosiInterID);

	SaveData(importFile.getNewProjectID());

	importFile.getFile().getLine();

	int nTakeoffQueuesItemCount = 0;
	importFile.getFile().getInteger(nTakeoffQueuesItemCount);
	for (int i =0; i < nTakeoffQueuesItemCount; ++i)
	{
		TakeoffQueuesItem data;
		data.ImportData(importFile,m_nID);
	}
}

TakeoffQueuesItem* TakeoffQueuePosi::GetItemByTreeId( int nidx )const
{
	for(int i=0;i< GetTakeoffQueuesItemCount();i++)
	{
		TakeoffQueuesItem * pItem = GetItem(i);
		if(pItem->GetUniID() == nidx )
			return pItem;
	}
	return NULL;
}

bool TakeoffQueuePosi::IsLeafItem( TakeoffQueuesItem* pQueuItem ) const
{
	for(int i=0;i< GetTakeoffQueuesItemCount();i++)
	{
		TakeoffQueuesItem * pthisItem = GetItem(i);
		if(pthisItem->GetParentID() == pQueuItem->GetUniID())
			return false;
	}
	return true;
}

void TakeoffQueuePosi::InitFromRunwayExit( const RunwayExit& runwayExit )
{
	m_nTakeoffPosiInterID = runwayExit.GetID();
	m_runwayExitDescription.InitData(runwayExit);
	//m_nTaxiwayID = runwayExit.GetTaxiwayID();
	
	//m_runwayExitDescription.InitData(runwayExit);
	//m_nRunwayID = runwayExit.GetRunwayID();
	//m_nRunwayMkIndex = runwayExit.GetRunwayMark();

}





