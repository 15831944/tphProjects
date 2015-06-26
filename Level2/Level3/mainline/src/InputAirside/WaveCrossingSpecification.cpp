#include "StdAfx.h"
#include ".\wavecrossingspecification.h"
#include "../Database/DBElementCollection_Impl.h"
#include "../Common/SqlScriptMaker.h"
#include "./HoldPosition.h"
CWaveCrossingRunwayTakeOff::CWaveCrossingRunwayTakeOff()
{
	m_bMinimum = true;
	m_nAllowWaves = 3;
}

CWaveCrossingRunwayTakeOff::~CWaveCrossingRunwayTakeOff()
{

}

void CWaveCrossingRunwayTakeOff::ReadData( CADORecordset& adoRecordset )
{
	int nMinMode = 0;
	adoRecordset.GetFieldValue(_T("DEPMIN"),nMinMode);
	if(nMinMode == 0)
		m_bMinimum = false;
	else
		m_bMinimum = true;

	adoRecordset.GetFieldValue(_T("DEPALLOWWAVE"),m_nAllowWaves);
}

bool CWaveCrossingRunwayTakeOff::GetMinimum() const
{
	return m_bMinimum;
}

void CWaveCrossingRunwayTakeOff::SetMinimum( bool val )
{
	m_bMinimum = val;
}

int CWaveCrossingRunwayTakeOff::GetAllowWaves() const
{
	return m_nAllowWaves;
}

void CWaveCrossingRunwayTakeOff::SetAllowWaves( int val )
{
	m_nAllowWaves = val;
}












//////////////////////////////////////////////////////////////////////////
//CWaveCrossingRunwayLanding
CWaveCrossingRunwayLanding::CWaveCrossingRunwayLanding()
{
	m_bMinimum = true;
	//if m_bminimum true
	m_nMinMiles = 6;//unit: nm
	m_nMinSecons = 180;//unit: second


	//if m_bminimum false
	m_nAllowWaves = 3;
	m_nAllowSeconds = 180;//unit: second
}
CWaveCrossingRunwayLanding::~CWaveCrossingRunwayLanding()
{

}
void CWaveCrossingRunwayLanding::ReadData( CADORecordset& recordset )
{
	int nMinMode = 0;
	recordset.GetFieldValue(_T("ARRMIN"),nMinMode);
	if(nMinMode == 0)
		m_bMinimum = false;
	else
		m_bMinimum = true;

	recordset.GetFieldValue(_T("ARRMINMILES"),m_nMinMiles);
	recordset.GetFieldValue(_T("ARRMINSECONDS"),m_nMinSecons);
	recordset.GetFieldValue(_T("ARRALLOWWAVES"),m_nAllowWaves);
	recordset.GetFieldValue(_T("ARRALLOWSECONDS"),m_nAllowSeconds);
}

bool CWaveCrossingRunwayLanding::GetMinimum() const
{
	return m_bMinimum;
}
void CWaveCrossingRunwayLanding::SetMinimum( bool val )
{
	m_bMinimum = val;
}

int CWaveCrossingRunwayLanding::GetMinMiles() const
{
	return m_nMinMiles;
}

void CWaveCrossingRunwayLanding::SetMinMiles( int val )
{
	m_nMinMiles = val;
}

int CWaveCrossingRunwayLanding::GetMinSecons() const
{
	return m_nMinSecons;
}

void CWaveCrossingRunwayLanding::SetMinSecons( int val )
{
	m_nMinSecons = val;
}

int CWaveCrossingRunwayLanding::GetAllowWaves() const
{
	return m_nAllowWaves;
}

void CWaveCrossingRunwayLanding::SetAllowWaves( int val )
{
	m_nAllowWaves = val;
}

int CWaveCrossingRunwayLanding::GetAllowSeconds() const
{
	return m_nAllowSeconds;
}

void CWaveCrossingRunwayLanding::SetAllowSeconds( int val )
{
	m_nAllowSeconds = val;
}


















//////////////////////////////////////////////////////////////////////////
//CWaveCrossingSpecificationItem
CWaveCrossingSpecificationItem::CWaveCrossingSpecificationItem()
{

}

CWaveCrossingSpecificationItem::~CWaveCrossingSpecificationItem()
{
	std::vector<HoldPosition *>::iterator iter = m_vNodes.begin();
	for (;iter != m_vNodes.end(); ++iter)
	{
		delete *iter;
	}

	for (iter = m_vAddNodes.begin();iter != m_vAddNodes.end(); ++iter)
	{
		delete *iter;
	}

	for (iter = m_vDelNodes.begin();iter != m_vDelNodes.end(); ++iter)
	{
		delete *iter;
	}


}
void CWaveCrossingSpecificationItem::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("NAME"),m_strName);
	
	m_landingSettings.ReadData(recordset);
	m_takeoffSettings.ReadData(recordset);

	//read hold positions
	ReadHoldPosition();
}

void CWaveCrossingSpecificationItem::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	SqlInsertScriptMaker sqlInsert(_T("AS_WAVECROSSRUNWAYSPEC"));
	sqlInsert.AddColumn(SqlScriptColumn(_T("NAME"),m_strName));
	sqlInsert.AddColumn(SqlScriptColumn(_T("DEPMIN"),m_takeoffSettings.GetMinimum()?1:0));
	sqlInsert.AddColumn(SqlScriptColumn(_T("DEPALLOWWAVE"),m_takeoffSettings.GetAllowWaves()));
	sqlInsert.AddColumn(SqlScriptColumn(_T("ARRMIN"),m_landingSettings.GetMinimum()?1:0));
	sqlInsert.AddColumn(SqlScriptColumn(_T("ARRMINMILES"),m_landingSettings.GetMinMiles()));
	sqlInsert.AddColumn(SqlScriptColumn(_T("ARRMINSECONDS"),m_landingSettings.GetMinSecons()));
	sqlInsert.AddColumn(SqlScriptColumn(_T("ARRALLOWWAVES"),m_landingSettings.GetAllowWaves()));
	sqlInsert.AddColumn(SqlScriptColumn(_T("ARRALLOWSECONDS"),m_landingSettings.GetAllowSeconds()));

	strSQL = sqlInsert.GetScript();
}

void CWaveCrossingSpecificationItem::GetUpdateSQL( CString& strSQL ) const
{
	SqlUpdateScriptMaker sqlUpdate(_T("AS_WAVECROSSRUNWAYSPEC"));

	sqlUpdate.AddColumn(SqlScriptColumn(_T("NAME"),m_strName));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("DEPMIN"),m_takeoffSettings.GetMinimum()?1:0));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("DEPALLOWWAVE"),m_takeoffSettings.GetAllowWaves()));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("ARRMIN"),m_landingSettings.GetMinimum()?1:0));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("ARRMINMILES"),m_landingSettings.GetMinMiles()));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("ARRMINSECONDS"),m_landingSettings.GetMinSecons()));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("ARRALLOWWAVES"),m_landingSettings.GetAllowWaves()));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("ARRALLOWSECONDS"),m_landingSettings.GetAllowSeconds()));

	CString strCondition;
	strCondition.Format(_T("ID = %d"),m_nID);
	sqlUpdate.SetCondition(strCondition);

	strSQL = sqlUpdate.GetScript();
}

void CWaveCrossingSpecificationItem::GetDeleteSQL( CString& strSQL ) const
{
	SqlDeleteScriptMaker sqlDelete(_T("AS_WAVECROSSRUNWAYSPEC"));
	CString strCondition;
	strCondition.Format(_T("ID = %d"),m_nID);
	sqlDelete.SetCondition(strCondition);

	strSQL = sqlDelete.GetScript();
}

void CWaveCrossingSpecificationItem::GetSelectSQL( int nID,CString& strSQL ) const
{

}

void CWaveCrossingSpecificationItem::SaveData( int nProjectID )
{

	DBElement::SaveData(nProjectID);
	SaveHoldposition();
}

void CWaveCrossingSpecificationItem::DeleteData()
{
	DBElement::DeleteData();
	DeleteHoldPostion();
}

void CWaveCrossingSpecificationItem::ReadHoldPosition()
{
	SqlSelectScriptMaker sqlSelect(_T("AS_WAVECROSSRUNWAY_HOLDPOS"));
	sqlSelect.AddColumn(_T("*"));
	
	CString strCondition;
	strCondition.Format(_T("WAVECROSSID = %d"),m_nID);

	sqlSelect.SetCondition(strCondition);
	sqlSelect.GetScript();

	CADORecordset adoRecordset;
	long lCount = 0;
	CADODatabase::ExecuteSQLStatement(sqlSelect.GetScript(),lCount,adoRecordset);

	while(!adoRecordset.IsEOF())
	{
		int nHoldPosID = -1;
		adoRecordset.GetFieldValue(_T("HOLDPOSID"),nHoldPosID);

		HoldPosition* pHoldPosition = new HoldPosition;
		pHoldPosition->ReadHold(nHoldPosID);
		m_vNodes.push_back(pHoldPosition);

		adoRecordset.MoveNextData();
	}


}

void CWaveCrossingSpecificationItem::SaveHoldposition()
{
	//insert the added items to database
	std::vector<HoldPosition *>::iterator iter = m_vAddNodes.begin();

	for (; iter != m_vAddNodes.end(); ++iter)
	{
		SqlInsertScriptMaker sqlInstert(_T("AS_WAVECROSSRUNWAY_HOLDPOS"));
		sqlInstert.AddColumn(SqlScriptColumn(_T("WAVECROSSID"),m_nID));
		sqlInstert.AddColumn(SqlScriptColumn(_T("HOLDPOSID"),(*iter)->GetUniqueID()));

		CADODatabase::ExecuteSQLStatement(sqlInstert.GetScript());
	}

	//insert the items added 
	m_vNodes.insert(m_vNodes.begin(),m_vAddNodes.begin(),m_vAddNodes.end());
	m_vAddNodes.clear();

	//delete the one deleted from database
	iter = m_vDelNodes.begin();
	for (;iter != m_vDelNodes.end(); ++iter)
	{
		SqlDeleteScriptMaker sqlDelete(_T("AS_WAVECROSSRUNWAY_HOLDPOS"));
		CString strCondition;
		strCondition.Format(_T("(WAVECROSSID = %d)&&(HOLDPOSID = %d)"),m_nID,(*iter)->GetUniqueID());
		CADODatabase::ExecuteSQLStatement(sqlDelete.GetScript());
	}
	m_vDelNodes.clear();


}
//delete all hold
void CWaveCrossingSpecificationItem::DeleteHoldPostion()
{
	SqlDeleteScriptMaker sqlDelete(_T("AS_WAVECROSSRUNWAY_HOLDPOS"));
	CString strCondition;
	strCondition.Format(_T("WAVECROSSID = %d"),m_nID);
	CADODatabase::ExecuteSQLStatement(sqlDelete.GetScript());
}
//need to check the hold exist or not
void CWaveCrossingSpecificationItem::AddHoldPosition( HoldPosition* holdNode )
{
	m_vAddNodes.push_back(holdNode);
}

void CWaveCrossingSpecificationItem::DeleteHoldPosition( HoldPosition* holdNode )
{
	std::vector<HoldPosition *>::iterator iter = std::find(m_vNodes.begin(),m_vNodes.end(), holdNode);
	if( iter!= m_vNodes.end())
	{
		m_vDelNodes.push_back(*iter);
		m_vNodes.erase(iter);
	}
	
}

int CWaveCrossingSpecificationItem::GetHoldCount()
{
	return static_cast<int>(m_vNodes.size());
}

HoldPosition * CWaveCrossingSpecificationItem::GetHoldPosition( int nIndex )
{
	ASSERT(nIndex>=0 && nIndex< GetHoldCount());

	if(nIndex >=0 && nIndex < GetHoldCount())
	{
		return m_vNodes.at(nIndex);
	}

	return NULL;

}

CWaveCrossingRunwayLanding& CWaveCrossingSpecificationItem::GetLandingSettings()
{
	return m_landingSettings;
}

CWaveCrossingRunwayTakeOff& CWaveCrossingSpecificationItem::GetTakeoffSettings()
{
	return m_takeoffSettings;
}

CString CWaveCrossingSpecificationItem::GetName() const
{
	return m_strName;
}

void CWaveCrossingSpecificationItem::SetName( CString val )
{
	m_strName = val;
}




//////////////////////////////////////////////////////////////////////////
CWaveCrossingSpecification::CWaveCrossingSpecification(void)
{
}

CWaveCrossingSpecification::~CWaveCrossingSpecification(void)
{
}

void CWaveCrossingSpecification::GetSelectElementSQL(CString& strSQL ) const
{
	
	SqlSelectScriptMaker sqlSelect(_T("AS_WAVECROSSRUNWAYSPEC"));
	sqlSelect.AddColumn(_T("*"));
	strSQL = sqlSelect.GetScript();
}

CWaveCrossingSpecificationItem* CWaveCrossingSpecification::AddNewItem( CWaveCrossingSpecificationItem* dbElement )
{
	m_dataList.push_back(dbElement);
	return m_dataList.back();
}


