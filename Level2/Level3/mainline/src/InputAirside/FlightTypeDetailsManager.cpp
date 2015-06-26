#include "StdAfx.h"
#include "FlightTypeDetailsManager.h"
#include "../Common/AirportDatabase.h"
#include <algorithm>
#include "AirsideImportExportManager.h"

FlightTypeDetailsManager::FlightTypeDetailsManager(CAirportDatabase* pAirportDB, int nProjID)
:m_pAirportDB(pAirportDB)
,m_nProjID(nProjID)
{
}

FlightTypeDetailsManager::~FlightTypeDetailsManager(void)
{
	RemoveAll();
}


void FlightTypeDetailsManager::ReadData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM FlightTypeDetails\
					 WHERE (PROJID = %d)"),m_nProjID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		FlightTypeDetailsItem* pItem = new FlightTypeDetailsItem(m_pAirportDB, m_nProjID);
		pItem->ReadData(adoRecordset);
	
		m_vectFltTypeDetailsItem.push_back(pItem);

		adoRecordset.MoveNextData();
	}

	if(m_vectFltTypeDetailsItem.size() <1)
	{
		FlightTypeDetailsItem* pDefaultItem = new FlightTypeDetailsItem(m_pAirportDB, m_nProjID);
		pDefaultItem->InitDefault();
		delete pDefaultItem;

		CString strSQL;
		strSQL.Format(_T("SELECT * \
						 FROM FlightTypeDetails\
						 WHERE (PROJID = %d)"),m_nProjID);

		long lRecordCount = 0;
		CADORecordset adoRecordset;

		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			FlightTypeDetailsItem* pItem = new FlightTypeDetailsItem(m_pAirportDB, m_nProjID);
			pItem->ReadData(adoRecordset);

			m_vectFltTypeDetailsItem.push_back(pItem);

			adoRecordset.MoveNextData();
		}
	}

	std::sort(m_vectFltTypeDetailsItem.begin(),m_vectFltTypeDetailsItem.end());

}
void FlightTypeDetailsManager::SaveData()
{
	FlightTypeDetailsItemIter iter = m_vectFltTypeDetailsItem.begin();

	for(;iter !=m_vectFltTypeDetailsItem.end(); iter++)
	{
		FlightTypeDetailsItem* pItem = (*iter);

		if(-1 ==  pItem->GetID())
			pItem->SaveData();
		else
			pItem->UpdateData();

	}

	for (iter = m_vectNeedDelete.begin(); 
		iter != m_vectNeedDelete.end(); iter++)
		(*iter)->DeleteData();

}



void FlightTypeDetailsManager::AddItem(FlightTypeDetailsItem* pItem)
{
	m_vectFltTypeDetailsItem.push_back(pItem);
}

void FlightTypeDetailsManager::DeleteItem(FlightTypeDetailsItem* pItem)
{
	FlightTypeDetailsItemIter iter = 
		std::find(m_vectFltTypeDetailsItem.begin(), m_vectFltTypeDetailsItem.end(), pItem);
	if(iter == m_vectFltTypeDetailsItem.end())
		return;
	m_vectNeedDelete.push_back(pItem);
	m_vectFltTypeDetailsItem.erase(iter);
}

void FlightTypeDetailsManager::RemoveAll()
{
	for (FlightTypeDetailsItemIter iter = m_vectFltTypeDetailsItem.begin();
		iter != m_vectFltTypeDetailsItem.end(); iter++)
		delete *iter;

	for (FlightTypeDetailsItemIter iter = m_vectNeedDelete.begin();
		iter != m_vectNeedDelete.end(); iter++)
		delete *iter;

	m_vectFltTypeDetailsItem.clear();
	m_vectNeedDelete.clear();

}

FlightTypeDetailsItem* FlightTypeDetailsManager::GetItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vectFltTypeDetailsItem.size());
	return m_vectFltTypeDetailsItem[nIndex];
}


BOOL FlightTypeDetailsManager::FlightConstraintHasDefine(FlightConstraint& cons)
{
	for (FlightTypeDetailsItemIter iter = m_vectFltTypeDetailsItem.begin();
		iter != m_vectFltTypeDetailsItem.end(); iter++)
	{
		if((*iter)->GetFltType() == cons)
			return TRUE;
	}

	return FALSE;
}
FlightTypeDetailsItem* FlightTypeDetailsManager::GetFlightDetailItem(const AirsideFlightDescStruct& fltDesc )
{
	FlightTypeDetailsItem* pReturnItem = NULL;
	
	for (FlightTypeDetailsItemIter iter = m_vectFltTypeDetailsItem.begin();
		iter != m_vectFltTypeDetailsItem.end(); iter++)
	{
		if((*iter)->GetFltType().fits(fltDesc))
		{
			if (pReturnItem == NULL || pReturnItem->GetFltType().fits((*iter)->GetFltType()))
				pReturnItem = *iter;		
		}
	}

	return pReturnItem;
}


void FlightTypeDetailsManager::ExportFlightTypeDetailsManager(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	FlightTypeDetailsManager exportObject(pAirportDB, exportFile.GetProjectID());
	exportObject.ReadData();
	exportObject.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void FlightTypeDetailsManager::ImportFlightTypeDetailsManager(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		FlightTypeDetailsItem importObject(NULL, importFile.getNewProjectID());
		importObject.ImportData(importFile);
	}
}

void FlightTypeDetailsManager::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("FlightTypeDetailsManager"));
	exportFile.getFile().writeLine();
	FlightTypeDetailsItemIter iter = m_vectFltTypeDetailsItem.begin();
	for(; iter!=m_vectFltTypeDetailsItem.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}







