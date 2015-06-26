#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\arrivaletaoffset.h"
#include <limits>
#include "../Common/point.h"

CArrivalETAOffset::CArrivalETAOffset(void)
{
}

CArrivalETAOffset::~CArrivalETAOffset(void)
{
	m_dataList.clear();

	m_deleteddataList.clear();
}

void CArrivalETAOffset::SetFlightType(FlightConstraint& flightType)
{
	m_FlightType = flightType;
	flightType.WriteSyntaxStringWithVersion(m_strFlightType.GetBuffer(1024));
}

void CArrivalETAOffset::SetFlightType(CString& strFlightType)
{
	m_strFlightType.Format(_T("%s"), strFlightType);

	if (m_pAirportDatabase)
	{
		m_FlightType.SetAirportDB(m_pAirportDatabase);
		m_FlightType.setConstraintWithVersion(m_strFlightType);
	}
}

FlightConstraint CArrivalETAOffset::GetFlightType()const
{
	return m_FlightType;
}

//DBElement
void CArrivalETAOffset::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);

	CString strFlightType;
	recordset.GetFieldValue(_T("FLTTYPE"),m_strFlightType);

	if (m_pAirportDatabase)
	{
		m_FlightType.SetAirportDB(m_pAirportDatabase);
		m_FlightType.setConstraintWithVersion(m_strFlightType);
	}

	CString strSelectSQL;
	GetSelectElementSQL(m_nID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CDistanceAndTimeFromARP* pDistanceAndTimeFromARP = new CDistanceAndTimeFromARP;
		pDistanceAndTimeFromARP->SetAirPortDB(m_pAirportDatabase);
		pDistanceAndTimeFromARP->InitFromDBRecordset(adoRecordset);
		AddNewItem(pDistanceAndTimeFromARP);

		adoRecordset.MoveNextData();
	}
}

void CArrivalETAOffset::GetSelectElementSQL(int nParentID,CString& strSQL)
{
	strSQL.Format(_T("SELECT * \
					 FROM IN_DISTANCEANDTIMEFROMARP \
					 WHERE (FLTTYPEID = %d)"),nParentID);
}

void CArrivalETAOffset::GetInsertSQL(int nProjID, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_DISTANCEANDTIMEFROMARPLIST\
					 (PROJID, FLTTYPE)\
					 VALUES (%d, '%s')"),\
					 nProjID,
					 m_strFlightType);
}

void CArrivalETAOffset::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_DISTANCEANDTIMEFROMARPLIST\
					 SET FLTTYPE = '%s' \
					 WHERE (ID =%d)"),\
					 m_strFlightType,
					 m_nID);
}

void CArrivalETAOffset::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_DISTANCEANDTIMEFROMARPLIST\
					 WHERE (ID = %d)"),m_nID);
}

void CArrivalETAOffset::SaveData(int nParentID)
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(nParentID, strSQL);

		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	for (std::vector<CDistanceAndTimeFromARP *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetFlightTypeID(m_nID);
		(*iter)->SaveData(nParentID);
	}

	for (std::vector<CDistanceAndTimeFromARP *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}

	m_deleteddataList.clear();
}

void CArrivalETAOffset::DeleteDataFromDB()
{
	ASSERT(m_dataList.size() == (size_t)0);

	for (std::vector<CDistanceAndTimeFromARP *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}

	m_deleteddataList.clear();

	CString strSQL;
	GetDeleteSQL(strSQL);
	if (strSQL.IsEmpty())
		return;

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CArrivalETAOffset::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	exportFile.getFile().writeField(m_strFlightType);
	exportFile.getFile().writeInt((int)GetElementCount());
	exportFile.getFile().writeLine();

	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CArrivalETAOffset::ImportData(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		int nOldID;
		importFile.getFile().getInteger(nOldID);
		int nOldProjID;
		importFile.getFile().getInteger(nOldProjID);

		importFile.getFile().getField(m_strFlightType.GetBuffer(1024), 1024);

		int nChildCount;
		importFile.getFile().getInteger(nChildCount);
		importFile.getFile().getLine();

		CString strSQL = _T("");
		GetInsertSQL(importFile.getNewProjectID(), strSQL);

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

		for (int i=0; i<nChildCount; i++)
		{
			CDistanceAndTimeFromARP distanceAndTimeFromARP;
			distanceAndTimeFromARP.SetFlightTypeID(m_nID);
			distanceAndTimeFromARP.ImportData(importFile);
		}
	}
}

CDistanceAndTimeFromARP* CArrivalETAOffset::GetLargestDistanceItem()const
{
	double dist = 0;
	CDistanceAndTimeFromARP * pLargestItem = NULL;
	for(int i=0;i< (int) GetElementCount(); i++)
	{
		CDistanceAndTimeFromARP * theItem = (CDistanceAndTimeFromARP*)GetItem(i);
		if( theItem->GetDistance() > dist )
		{
			dist = theItem->GetDistance();
			pLargestItem = theItem;
		}
	}
	return pLargestItem;
}


CDistanceAndTimeFromARP* CArrivalETAOffset::GetNearestDistanceItem( const double& dist ) const
{
	CDistanceAndTimeFromARP * pItem = NULL;
	if( GetElementCount() )
	{	
		pItem = (CDistanceAndTimeFromARP *)GetItem(0);
		DistanceUnit distOffset = abs(pItem->GetDistance() - dist);
		for(int i=1;i<(int)GetElementCount();++i)
		{
			CDistanceAndTimeFromARP* pTheItem = (CDistanceAndTimeFromARP *)GetItem(i);
			DistanceUnit thedistoffset = abs(pTheItem->GetDistance() - dist );
			if( thedistoffset < distOffset  )
			{
				pItem = pTheItem;
				distOffset = thedistoffset;
			}
		}
	}
	return pItem;
}