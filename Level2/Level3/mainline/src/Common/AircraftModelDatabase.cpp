#include "StdAfx.h"
#include ".\aircraftmodeldatabase.h"
#include <Database/ARCportADODatabase.h>
#include <Common/AirportDatabase.h>
#include ".\AircraftModel.h"


AirportDatabaseConnection* CAircraftModelDatabase::GetConnection() const
{
	return m_pAirportDB ? m_pAirportDB->GetAirportConnection() : NULL;
}

BOOL CAircraftModelDatabase::loadDatabase( CAirportDatabase* pAirportDB )
{
	ClearData();
	if (NULL == pAirportDB)
	{
		pAirportDB = m_pAirportDB;
	}
	ASSERT(pAirportDB);

	AirportDatabaseConnection* pConn = pAirportDB->GetAirportConnection();
	ASSERT(pConn);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM %s"), STR_AC_MODL_DBTABLE);

		long nCount = 0L;
		CADORecordset adoRecordset;
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,nCount,adoRecordset, pConn);

		while (!adoRecordset.IsEOF())
		{
			CAircraftModel* pModel = new CAircraftModel(this);
			pModel->DBReadData(adoRecordset);
			//pModel->ReadContent(pModel->mFile);
			//pModel->mFile.Read(pModel->mFile.m_ID,pConn);
			AddModel(pModel);	
			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& /*e*/)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CAircraftModelDatabase::saveDatabase(CAirportDatabase* pAirportDB)
{
	ASSERT(pAirportDB);
	try
	{
		CDatabaseADOConnetion::BeginTransaction(pAirportDB->GetAirportConnection());
		CModelDatabase<CAircraftModel,CAirportDatabase>::saveDatabase(pAirportDB);
		CDatabaseADOConnetion::CommitTransaction(pAirportDB->GetAirportConnection()) ;
	}
	catch (CADOException& /*e*/)
	{
		CDatabaseADOConnetion::RollBackTransation(pAirportDB->GetAirportConnection()) ;
		return FALSE;
	}

	return TRUE;	
}
BOOL CAircraftModelDatabase::SaveData()
{
	return saveDatabase(m_pAirportDB);	
}

CAircraftModel * CAircraftModelDatabase::AddItem( CString sName )
{
	if(getModelByName(sName))
		return NULL;
	CAircraftModel* newModel = new CAircraftModel(this);
	newModel->m_sModelName = sName;
	AddModel(newModel);
	return newModel;
}

void CAircraftModelDatabase::RemoveItem( CString sName )
{
	DelModel(getModelByName(sName));
}

CAircraftModelDatabase::CAircraftModelDatabase( CAirportDatabase* pParent )
{
	m_pAirportDB = pParent;
}

CAircraftModelDatabase::~CAircraftModelDatabase()
{
	ClearData();
}

CAircraftModel* CAircraftModelDatabase::getModelByName( const CString& strName ) const
{
	for(TypeModelList::const_iterator itr = m_vModels.begin();itr!=m_vModels.end();itr++)
	{
		CAircraftModel* pModel = *itr;
		if(pModel->m_sModelName == strName)
			return pModel;
	}
	return NULL;
}