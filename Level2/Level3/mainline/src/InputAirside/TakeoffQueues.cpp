#include "StdAfx.h"
#include "TakeoffQueues.h"
#include "TakeoffQueuePosi.h"
#include "TakeoffQueuesItem.h"
#include "AirsideImportExportManager.h"

#include <algorithm>

TakeoffQueues::TakeoffQueues(int nProjID)
:m_nProjID(nProjID)
{
		m_nMaxUniID = 0;
}

TakeoffQueues::~TakeoffQueues(void)
{
	RemoveAll();
}

void TakeoffQueues::ReadData()
{
//	InitTakeOffQueues();
	//TakeoffQueuePosiVector vReadTakeoffQueuePosi;
	CString strSQL;
	strSQL.Format(_T("SELECT ID, TAKEOFFPOSIINTERID\
					 FROM IN_TAKEOFFQUEUE\
					 WHERE (PROJID = %d)"),m_nProjID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		TakeoffQueuePosi* pTakeoffQueuePosi = new TakeoffQueuePosi;
		pTakeoffQueuePosi->ReadData(adoRecordset);

		AddTakeoffQueuePosi(pTakeoffQueuePosi);
		adoRecordset.MoveNextData();
	}
	//InsertDataIntoStructure(vReadTakeoffQueuePosi);

	TakeoffQueuePosiIter iter  = m_vTakeoffQueuePosi.begin();
	TakeoffQueuePosiIter iterEnd  = m_vTakeoffQueuePosi.end();

	for (;iter != iterEnd;++iter)
	{
		TakeoffQueuePosi* pTakeoffQueuePosi = (*iter);
		TakeoffQueuePosi::TakeoffQueuesItemVector* pvTakeoffQueuesItem = pTakeoffQueuePosi->GetQueuesItemVector();
		TakeoffQueuePosi::TakeoffQueuesItemIter iterItem = pvTakeoffQueuesItem->begin();

		for (;iterItem !=pvTakeoffQueuesItem->end();++iterItem)
		{
			TakeoffQueuesItem* pTakeoffQueuesItem = (*iterItem);
			if(pTakeoffQueuesItem->GetUniID() > m_nMaxUniID)
				m_nMaxUniID = pTakeoffQueuesItem->GetUniID();
		}		
	}
}
void TakeoffQueues::InsertDataIntoStructure(TakeoffQueuePosiVector vReadTakeoffQueuePosi)
{
	TakeoffQueuePosiIter readiter = vReadTakeoffQueuePosi.begin();
	while (readiter != vReadTakeoffQueuePosi.end())
	{
		TakeoffQueuePosiIter iter = m_vTakeoffQueuePosi.begin();
		for (;iter != m_vTakeoffQueuePosi.end(); ++iter)
		{
			if((*iter)->GetTakeoffPosiInterID() == (*readiter)->GetTakeoffPosiInterID())
			{
		//		(*iter)->CopyData(*readiter);
				(*iter)->setID((*readiter)->getID());
				(*iter)->SetQueuesItemVector(*(*readiter)->GetQueuesItemVector());
				(*readiter)->Clear();
				break;
			}
		}
		vReadTakeoffQueuePosi.erase(readiter);
		readiter = vReadTakeoffQueuePosi.begin();
	}	
}
void TakeoffQueues::SaveData()
{
	TakeoffQueuePosiIter iter  = m_vTakeoffQueuePosi.begin();
	TakeoffQueuePosiIter iterEnd  = m_vTakeoffQueuePosi.end();

	for (;iter != iterEnd;++iter)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData(m_nProjID);
		else
			(*iter)->UpdateData(m_nProjID);
	}
	iter = m_vInvalidTakeoffQueuePosi.begin();
	for (;iter != m_vInvalidTakeoffQueuePosi.end();++iter)
	{
		(*iter)->DeleteData();
	//	delete *iter;
	}
}

int TakeoffQueues::GetTakeoffQueuePosiCount()
{
	return (int)m_vTakeoffQueuePosi.size();
}

TakeoffQueuePosi* TakeoffQueues::GetItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vTakeoffQueuePosi.size());
	return m_vTakeoffQueuePosi[nIndex];
}

void TakeoffQueues::AddTakeoffQueuePosi(TakeoffQueuePosi* pItem)
{
	m_vTakeoffQueuePosi.push_back(pItem);
}
void TakeoffQueues::DeleteTakeoffQueuePosi(TakeoffQueuePosi* pItem)
{
	TakeoffQueuePosiIter iter = 
		std::find(m_vTakeoffQueuePosi.begin(),m_vTakeoffQueuePosi.end(), pItem);
	if (iter == m_vTakeoffQueuePosi.end())
		return;
	m_vInvalidTakeoffQueuePosi.push_back(pItem);
	m_vTakeoffQueuePosi.erase(iter);
}
void TakeoffQueues::RemoveAll()
{
	for (TakeoffQueuePosiIter iter = m_vTakeoffQueuePosi.begin();
		iter != m_vTakeoffQueuePosi.end(); iter++)
	{
		delete (*iter);
		(*iter) = NULL;
	}
	m_vTakeoffQueuePosi.clear();
}
void TakeoffQueues::InitTakeOffQueues()
{

	CString strSQL;
	strSQL.Format(_T("SELECT ID,TAXIWAYID,RWMARKDATAID,INTERSECTNODEID,RWEXITDESID\
					 FROM IN_RUNWAY_TAKEOFFRWMKDATA\
					 WHERE (ELIGIBLE = 1) AND RWMARKDATAID IN\
					 (SELECT ID FROM IN_RUNWAY_TAKEOFFPOSITION\
					 WHERE(PROJID = %d))"),m_nProjID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);

	while(!adoRecordset.IsEOF())
	{
		TakeoffQueuePosi* pTakeoffQueuePosi = new TakeoffQueuePosi;
		pTakeoffQueuePosi->InitData(adoRecordset);
		m_vTakeoffQueuePosi.push_back(pTakeoffQueuePosi);
		adoRecordset.MoveNextData();
	}
}
void TakeoffQueues::ExportTakeoffQueues(CAirsideExportFile& exportFile)
{
	TakeoffQueues takeoffQueues(exportFile.GetProjectID());
	takeoffQueues.ReadData();
	takeoffQueues.ExportData(exportFile);
	exportFile.getFile().endFile();
}
void TakeoffQueues::ExportData(CAirsideExportFile& exportFile)
{		
	exportFile.getFile().writeField("TakeoffQueues");
	exportFile.getFile().writeLine();
	std::vector<TakeoffQueuePosi *>::iterator iter = m_vTakeoffQueuePosi.begin();
	for (; iter!=m_vTakeoffQueuePosi.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}
void TakeoffQueues::ImportTakeoffQueues(CAirsideImportFile& importFile)
{
	while(!importFile.getFile().isBlank())
	{
		TakeoffQueuePosi takeoffQueuePosi;
		takeoffQueuePosi.ImportData(importFile);
	}
}