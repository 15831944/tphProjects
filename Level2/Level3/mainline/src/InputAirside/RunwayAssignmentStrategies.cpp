#include "StdAfx.h"
#include "RunwayAssignmentStrategies.h"
#include "FlightTypeRunwayAssignmentStrategyItem.h"
#include "../Database/ADODatabase.h"
#include "InputAirside/AirsideImportExportManager.h"
#include "common/AirportDatabase.h"

#include <algorithm>

RunwayAssignmentStrategies::RunwayAssignmentStrategies(int nProjID)
{
	m_nProjID = nProjID;
	m_enumRunwayAssignType = RunwayAssignmentType_Landing;
	m_pAirportDatabase = NULL;
}

RunwayAssignmentStrategies::~RunwayAssignmentStrategies(void)
{
	RemoveAll();
}

FlightTypeRunwayAssignmentStrategyItem* RunwayAssignmentStrategies::GetStrategyItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vectStrategy.size());

	return m_vectStrategy[nIndex];
}

void RunwayAssignmentStrategies::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, FLTTYPE\
		FROM IN_RUNWAY_ASGN \
		WHERE (PROJID = %d) AND (ASGNTYPE = %d)"),m_nProjID,(int)m_enumRunwayAssignType);

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);
	
	while (!adoRecordset.IsEOF())
	{
		FlightTypeRunwayAssignmentStrategyItem * pItem = new FlightTypeRunwayAssignmentStrategyItem(m_nProjID);

		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		pItem->setID(nID);

		CString strFltType = _T("");
		adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);
		if (m_pAirportDatabase)
			pItem->SetAirportDatabase(m_pAirportDatabase);
		pItem->setFlightType(strFltType);

		pItem->ReadData();
		m_vectStrategy.push_back(pItem);

		adoRecordset.MoveNextData();
	}

	Sort();
}

void RunwayAssignmentStrategies::SaveData()
{
	for (StrategyIter iter = m_vectStrategy.begin(); iter != m_vectStrategy.end(); iter++)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData (m_enumRunwayAssignType);
		else
			(*iter)->UpdateData();
	}

	//Item deleted from ui need delete from database
	DeleteDataFromDatabase();
}

void RunwayAssignmentStrategies::DeleteStrategyItem(FlightTypeRunwayAssignmentStrategyItem* pStrategyItem)
{
	StrategyIter iter = std::find(m_vectStrategy.begin(), m_vectStrategy.end(), pStrategyItem);
	if (iter == m_vectStrategy.end())
		return;
	m_vectNeedDelStrategy.push_back(*iter);
	m_vectStrategy.erase(iter);
}

void RunwayAssignmentStrategies::AddStrategyItem(FlightTypeRunwayAssignmentStrategyItem* pStrategyItem)
{
	m_vectStrategy.push_back(pStrategyItem);
}

void RunwayAssignmentStrategies::RemoveAll()
{
	for (StrategyIter iter = m_vectStrategy.begin(); iter != m_vectStrategy.end(); iter++)
	{
		delete (*iter);
		(*iter) = NULL;
	}

	m_vectStrategy.clear();
}

void RunwayAssignmentStrategies::DeleteDataFromDatabase()
{
	for (StrategyIter iter = m_vectNeedDelStrategy.begin(); iter != m_vectNeedDelStrategy.end(); iter++)
	{
		if( (*iter)->getID() != -1)
		{
			(*iter)->RemoveAll();
			(*iter)->DeleteData();
		}
		delete (*iter);
		(*iter) = NULL;
	}
}
void RunwayAssignmentStrategies::ExportRunwayAssignment(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("ID, FLTTYPE,ASGNTYPE"));
	exportFile.getFile().writeLine();

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, FLTTYPE,ASGNTYPE\
					 FROM IN_RUNWAY_ASGN \
					 WHERE (PROJID = %d)"),exportFile.GetProjectID());

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		FlightTypeRunwayAssignmentStrategyItem pItem(exportFile.GetProjectID());

		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		pItem.setID(nID);

		int nAsignType = 0;
		adoRecordset.GetFieldValue(_T("ASGNTYPE"),nAsignType);

		CString strFltType = _T("");
		adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);
		pItem.setFlightType(strFltType);
		pItem.ReadData();


		pItem.ExportFlightTypeRunwayAssignment(exportFile,nAsignType);

		adoRecordset.MoveNextData();
	}

	exportFile.getFile().endFile();
}
void RunwayAssignmentStrategies::ImportRunwayAssignment(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{	
		FlightTypeRunwayAssignmentStrategyItem pItem(importFile.getNewProjectID());
		pItem.ImportFlightTypeRunwayAssignment(importFile);
	}
}
bool LessFlightTypeRunwayAssignmentStrategyItem(FlightTypeRunwayAssignmentStrategyItem* fItem,FlightTypeRunwayAssignmentStrategyItem* sItem)
{
	if(sItem->GetFlightType().fits(fItem->GetFlightType()))
		return true;
	return false;
}
void RunwayAssignmentStrategies::Sort()
{
	std::sort( m_vectStrategy.begin(), m_vectStrategy.end(),LessFlightTypeRunwayAssignmentStrategyItem);
}






