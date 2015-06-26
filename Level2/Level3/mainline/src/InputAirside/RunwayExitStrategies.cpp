#include "StdAfx.h"
#include "RunwayExitStrategies.h"
#include "FlightTypeRunwayExitStrategyItem.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "TimeRangeRunwayExitStrategyItem.h"
#include "RunwayExitStandStrategyItem.h"

#include <algorithm>

RunwayExitStrategies::RunwayExitStrategies(int nProjectID)
{
	m_nProjID = nProjectID;
	//m_pAirportDatabase = NULL;
}

RunwayExitStrategies::~RunwayExitStrategies(void)
{
	RemoveAll();
}

FlightTypeRunwayExitStrategyItem* RunwayExitStrategies::GetStrategyItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vectStrategy.size());

	return m_vectStrategy[nIndex];
}

bool FlightTypeCompare(FlightTypeRunwayExitStrategyItem* fItem,FlightTypeRunwayExitStrategyItem* sItem)
{
	const FlightConstraint& flcst = fItem->getFlightType();
	const FlightConstraint& slcst = sItem->getFlightType();

	if (flcst.fits(slcst))
		return false;

	if(slcst.fits(flcst))
		return true;

	return false;
}

void RunwayExitStrategies::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, FLTTYPE\
		FROM IN_RUNWAY_EXITSTRATEGY\
		WHERE (PROJID = %d)"),m_nProjID);

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

	if(adoRecordset.IsEOF()) // init default value
	{
		TimeRangeRunwayExitStrategyItem* pTimeRangeItem = new TimeRangeRunwayExitStrategyItem(-1);

		RunwayExitStandStrategyItem* pStandStrategyItem = new RunwayExitStandStrategyItem(-1);
		pStandStrategyItem->AddTimeRangeRWAssignStrategyItem(pTimeRangeItem);

		FlightTypeRunwayExitStrategyItem * pExitStrategyItem = new FlightTypeRunwayExitStrategyItem(m_nProjID);
		pExitStrategyItem->AddRunwayExitStandStrategyItem(pStandStrategyItem);
		pExitStrategyItem->SetAirportDatabase(m_pAirportDB);
		FlightConstraint fltType;
		fltType.SetAirportDB(m_pAirportDB);
		pExitStrategyItem->SetFltType(fltType);
		AddStrategyItem(pExitStrategyItem);
		
	}
	else
	{
		while (!adoRecordset.IsEOF())
		{
			FlightTypeRunwayExitStrategyItem * pItem = new FlightTypeRunwayExitStrategyItem(m_nProjID);
			int nID = -1;
			adoRecordset.GetFieldValue(_T("ID"),nID);
			pItem->setID(nID);
			pItem->SetAirportDatabase(m_pAirportDB);

			CString strFltType;
			char szFltType[1024] = {0};
			adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);
			strcpy_s(szFltType,strFltType);
			pItem->GetFltType().SetAirportDB(m_pAirportDB);
			pItem->GetFltType().setConstraintWithVersion(szFltType);

			pItem->ReadData();
			m_vectStrategy.push_back(pItem);

			adoRecordset.MoveNextData();
		}
	}


	std::sort(m_vectStrategy.begin(), m_vectStrategy.end(),FlightTypeCompare);
}
void RunwayExitStrategies::ImportReadData()
{

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, FLTTYPE\
					 FROM IN_RUNWAY_EXITSTRATEGY\
					 WHERE (PROJID = %d)"),m_nProjID);

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		FlightTypeRunwayExitStrategyItem * pItem = new FlightTypeRunwayExitStrategyItem(m_nProjID);

		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		pItem->setID(nID);

		CString strFltType;
		adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);
		FlightConstraint fltType;
		fltType.SetAirportDB(m_pAirportDB);
		fltType.setConstraintWithVersion(strFltType);
		pItem->SetFltType(fltType);

		pItem->ReadData();
		m_vectStrategy.push_back(pItem);

		adoRecordset.MoveNextData();
	}
	
}

void RunwayExitStrategies::SaveData()
{
	for (StrategyIter iter = m_vectStrategy.begin(); iter != m_vectStrategy.end(); iter++)
	{
		if(-1 == (*iter)->getID())	
			(*iter)->SaveData();
		else
			(*iter)->UpdateData();
	}
	DeleteDataFromDatabase();
}

void RunwayExitStrategies::DeleteStrategyItem(FlightTypeRunwayExitStrategyItem* pStrategyItem)
{
	StrategyIter iter = std::find(m_vectStrategy.begin(), m_vectStrategy.end(), pStrategyItem);
	if (iter == m_vectStrategy.end())
		return;
	m_vectNeedDelStrategy.push_back(*iter);
	m_vectStrategy.erase(iter);
}

void RunwayExitStrategies::AddStrategyItem(FlightTypeRunwayExitStrategyItem* pStrategyItem)
{
	m_vectStrategy.push_back(pStrategyItem);
}

void RunwayExitStrategies::RemoveAll()
{
	for (StrategyIter iter = m_vectStrategy.begin(); iter != m_vectStrategy.end(); iter++)
	{
		delete (*iter);
		(*iter) = NULL;
	}

	m_vectStrategy.clear();
}

void RunwayExitStrategies::DeleteDataFromDatabase()
{
	for (StrategyIter iter = m_vectNeedDelStrategy.begin(); iter != m_vectNeedDelStrategy.end(); iter++)
	{
		if( (*iter)->getID() != -1)
		{
			(*iter)->DeleteData();
		}
		delete (*iter);
	}
	m_vectNeedDelStrategy.clear();
}

void RunwayExitStrategies::ExportRunwayExitStrategies(CAirsideExportFile& exportFile)
{
	RunwayExitStrategies runwayExitStrategies(exportFile.GetProjectID());
	runwayExitStrategies.ImportReadData();
	runwayExitStrategies.ExportData(exportFile);

	exportFile.getFile().endFile();
}

void RunwayExitStrategies::ImportRunwayExitStrategies(CAirsideImportFile& importFile)
{
	while(!importFile.getFile().isBlank())
	{
		FlightTypeRunwayExitStrategyItem flightTypeRunwayExitStrategyItem(importFile.getNewProjectID());
		flightTypeRunwayExitStrategyItem.ImportData(importFile);
	}
}

void RunwayExitStrategies::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("RunwayExitStrategies");
	exportFile.getFile().writeLine();
	StrategyIter iter = m_vectStrategy.begin();
	for (; iter!=m_vectStrategy.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}

void RunwayExitStrategies::ImportData(CAirsideImportFile& importFile)
{

}

BOOL RunwayExitStrategies::CheckExitPercent() const
{
	for (StrategyIter_const iter = m_vectStrategy.begin();iter!=m_vectStrategy.end();iter++)
	{
		if (FALSE == (*iter)->CheckExitPercent())
		{
			return FALSE;
		}
	}
	return TRUE;
}