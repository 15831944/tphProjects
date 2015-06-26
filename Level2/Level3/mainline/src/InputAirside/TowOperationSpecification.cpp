#include "StdAfx.h"
#include ".\towoperationspecification.h"
#include "../common/SqlScriptMaker.h"
#include "../Database/DBElementCollection_Impl.h"
//////////////////////////////////////////////////////////////////////////
//CTowOperationSpecFltTypeData::CReleasePoint
CReleasePoint::CReleasePoint()
:m_outBoundRoute(-1,0)
,m_retRoute(-1,1)
{
	m_nObjectID = -1;
	m_enumOutRouteType = TOWOPERATIONROUTETYPE_SHORTESTPATH;
	m_enumRetRouteType = TOWOPERATIONROUTETYPE_SHORTESTPATH;
	m_enumDestType = TOWOPERATIONDESTINATION_COUNT;
	m_runwayMark = RUNWAYMARK_FIRST;
}

CReleasePoint::~CReleasePoint()
{

}

void CReleasePoint::InitFromDBRecordset( CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	//dest type
	int nDestType = 0;
	TOWOPERATIONDESTINATION enumTowOpDestType = TOWOPERATIONDESTINATION_STAND;
	recordset.GetFieldValue(_T("DESTTYPE"),nDestType);
	if(nDestType >= TOWOPERATIONDESTINATION_STAND && nDestType < TOWOPERATIONDESTINATION_COUNT)
	{
		m_enumDestType = (TOWOPERATIONDESTINATION)nDestType;
	}
	//object id
	int nObjectID = 0;
	recordset.GetFieldValue(_T("OBJECTID"),m_nObjectID);

	//runway mark
	int nRunwayMark = 0;
	recordset.GetFieldValue(_T("RUNWAYMARK"),nRunwayMark);
	if(nRunwayMark == RUNWAYMARK_FIRST)
		m_runwayMark = RUNWAYMARK_FIRST;
	else
		m_runwayMark = RUNWAYMARK_SECOND;

	recordset.GetFieldValue(_T("NAME"),m_strName);

	int nOutRouteType = 0;
	recordset.GetFieldValue(_T("OUTTOWROUTETYPE"), nOutRouteType);

	if(nOutRouteType >= TOWOPERATIONROUTETYPE_SHORTESTPATH && nOutRouteType < TOWOPERATIONROUTETYPE_COUNT)
	{
		m_enumOutRouteType = (TOWOPERATIONROUTETYPE)nOutRouteType;
	}

	//read tow route data
	m_outBoundRoute.ReadData(m_nID);

	int nRetRouteType = 0;
	recordset.GetFieldValue(_T("RETTOWROUTETYPE"), nRetRouteType);

	if(nRetRouteType >= TOWOPERATIONROUTETYPE_SHORTESTPATH && nRetRouteType < TOWOPERATIONROUTETYPE_COUNT)
	{
		m_enumRetRouteType = (TOWOPERATIONROUTETYPE)nRetRouteType;
	}

	m_retRoute.ReadData(m_nID);
	
}

void CReleasePoint::SaveData( int nFltID )
{
	DBElement::SaveData(nFltID);

	m_outBoundRoute.SaveData(m_nID);
	m_retRoute.SaveData(m_nID);

}

void CReleasePoint::DeleteData()
{	
	m_outBoundRoute.DeleteDataFromDB();
	m_retRoute.DeleteDataFromDB();

	DBElement::DeleteData();

}


void CReleasePoint::GetInsertSQL( int nFltID,CString& strSQL ) const
{
	SqlInsertScriptMaker sqlInsert(_T("AS_RELEASEPOINT"));

	sqlInsert.AddColumn(SqlScriptColumn(_T("PARENTID"),nFltID));

	sqlInsert.AddColumn(SqlScriptColumn(_T("DESTTYPE"),(int)m_enumDestType));
	sqlInsert.AddColumn(SqlScriptColumn(_T("OBJECTID"),m_nObjectID));

	sqlInsert.AddColumn(SqlScriptColumn(_T("RUNWAYMARK"),(int)m_runwayMark));

	sqlInsert.AddColumn(SqlScriptColumn(_T("NAME"),m_strName));

	sqlInsert.AddColumn(SqlScriptColumn(_T("OUTTOWROUTETYPE"),(int)m_enumOutRouteType));

	sqlInsert.AddColumn(SqlScriptColumn(_T("RETTOWROUTETYPE"),(int)m_enumRetRouteType));

	strSQL = sqlInsert.GetScript();
}

void CReleasePoint::GetUpdateSQL( CString& strSQL ) const
{
	SqlUpdateScriptMaker sqlUpdate(_T("AS_RELEASEPOINT"));

	sqlUpdate.AddColumn(SqlScriptColumn(_T("DESTTYPE"),(int)m_enumDestType));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("OBJECTID"),m_nObjectID));

	sqlUpdate.AddColumn(SqlScriptColumn(_T("RUNWAYMARK"),(int)m_runwayMark));

	sqlUpdate.AddColumn(SqlScriptColumn(_T("NAME"),m_strName));

	sqlUpdate.AddColumn(SqlScriptColumn(_T("OUTTOWROUTETYPE"),(int)m_enumOutRouteType));

	sqlUpdate.AddColumn(SqlScriptColumn(_T("RETTOWROUTETYPE"),(int)m_enumRetRouteType));

	//condition 
	CString strCondition = _T("");
	strCondition.Format(_T("ID = %d"),m_nID);
	sqlUpdate.SetCondition(strCondition);

	strSQL = sqlUpdate.GetScript();
}

void CReleasePoint::GetDeleteSQL( CString& strSQL ) const
{
	SqlDeleteScriptMaker sqlDelete(_T("AS_RELEASEPOINT"));

	CString strCondition = _T("");
	strCondition.Format(_T("ID = %d"),m_nID);
	sqlDelete.SetCondition(strCondition);

	strSQL = sqlDelete.GetScript();
}

CTowOperationRoute* CReleasePoint::GetOutBoundRoute()
{
	if (m_enumOutRouteType == TOWOPERATIONROUTETYPE_SHORTESTPATH)
		return NULL;

	return &m_outBoundRoute;
}

CTowOperationRoute* CReleasePoint::GetReturnRoute()
{
	if (m_enumRetRouteType == TOWOPERATIONROUTETYPE_SHORTESTPATH)
		return NULL;

	return &m_retRoute; 
}

//////////////////////////////////////////////////////////////////////////
//CTowOperationSpecFltTypeData
CTowOperationSpecFltTypeData::CTowOperationSpecFltTypeData()
:m_nID(-1)
{
	m_pAirportDatabase = NULL;
	m_enumOperartionType = TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY;

}

CTowOperationSpecFltTypeData::~CTowOperationSpecFltTypeData()
{

}

void CTowOperationSpecFltTypeData::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);

	CString strConstraint;
	recordset.GetFieldValue(_T("FLTTYPE"),strConstraint);
	m_fltConstraint.SetAirportDB(m_pAirportDatabase);
	m_fltConstraint.setConstraintWithVersion(strConstraint);

	int nOperationType = 0;
	recordset.GetFieldValue(_T("OPERATIONTYPE"),nOperationType);
	if(nOperationType >= TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY&& nOperationType < TOWOPERATIONTYPE_COUNT)
	{
		m_enumOperartionType = (TOWOPERATIONTYPE)nOperationType;
	}
	else
	{
		m_enumOperartionType = TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY;
	}

	{
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
			CReleasePoint* element = new CReleasePoint;
			element->InitFromDBRecordset(adoRecordset);
			m_dataList.push_back(element);
			adoRecordset.MoveNextData();
		}
	}	
}

void CTowOperationSpecFltTypeData::GetInsertSQL( int StandGroupID,CString& strSQL ) const
{
	SqlInsertScriptMaker sqlInsert(_T("AS_TOWOPFLIGHTTYPEDATA"));

	sqlInsert.AddColumn(SqlScriptColumn(_T("STANDDATAID"),StandGroupID));
	//flight type
	CString strFltType;
	m_fltConstraint.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));
	strFltType.ReleaseBuffer();
	sqlInsert.AddColumn(SqlScriptColumn(_T("FLTTYPE"),strFltType));
	
	//operation type
	sqlInsert.AddColumn(SqlScriptColumn(_T("OPERATIONTYPE"),(int)m_enumOperartionType));
	
	strSQL = sqlInsert.GetScript();
}

void CTowOperationSpecFltTypeData::GetUpdateSQL( CString& strSQL ) const
{
	SqlUpdateScriptMaker sqlUpdate(_T("AS_TOWOPFLIGHTTYPEDATA"));

	//sqlUpdate.AddColumn(SqlScriptColumn(_T("STANDDATAID"),StandGroupID));
	//flight type
	CString strFltType;
	m_fltConstraint.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));
	strFltType.ReleaseBuffer();
	sqlUpdate.AddColumn(SqlScriptColumn(_T("FLTTYPE"),strFltType));
	
	//operation type
	sqlUpdate.AddColumn(SqlScriptColumn(_T("OPERATIONTYPE"),(int)m_enumOperartionType));
	
	//condition 
	CString strCondition = _T("");
	strCondition.Format(_T("ID = %d"),m_nID);
	sqlUpdate.SetCondition(strCondition);

	strSQL = sqlUpdate.GetScript();
}

void CTowOperationSpecFltTypeData::GetDeleteSQL( CString& strSQL ) const
{
	SqlDeleteScriptMaker sqlDelete(_T("AS_TOWOPFLIGHTTYPEDATA"));

	CString strCondition = _T("");
	strCondition.Format(_T("ID = %d"),m_nID);
	sqlDelete.SetCondition(strCondition);

	strSQL = sqlDelete.GetScript();
}

void CTowOperationSpecFltTypeData::SaveData( int StandGroupID )
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(StandGroupID,strSQL);

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

	{
		for (std::vector<CReleasePoint *>::iterator iter = m_dataList.begin();
			iter != m_dataList.end(); ++iter)
		{
			(*iter)->SaveData(m_nID);
		}

		for (std::vector<CReleasePoint *>::iterator iter = m_deleteddataList.begin();
			iter != m_deleteddataList.end(); ++iter)
		{
			(*iter)->DeleteData();
		}

		m_deleteddataList.clear();
	}

}

void CTowOperationSpecFltTypeData::DeleteData()
{	

	for (std::vector<CReleasePoint *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	CString strSQL;
	GetDeleteSQL(strSQL);
	if (strSQL.IsEmpty())
		return;

	CADODatabase::ExecuteSQLStatement(strSQL);

}

void CTowOperationSpecFltTypeData::GetSelectElementSQL( int nParentID, CString& strSQL ) const
{
	SqlSelectScriptMaker sqlSelect(_T("AS_RELEASEPOINT"));
	sqlSelect.AddColumn(_T("*"));
	CString strCondition;
	strCondition.Format(_T("PARENTID = %d"),nParentID);
	sqlSelect.SetCondition(strCondition);
	strSQL = sqlSelect.GetScript();
}


////////////////////////////////////////////////////////////////////////////
//CTowOperationSpecStand
CTowOperationSpecStand::CTowOperationSpecStand()
{
	m_nID = -1;//id in 

	m_nProjID = -1;

	m_pAirportDatabase = NULL;

}

CTowOperationSpecStand::~CTowOperationSpecStand()
{

}

void CTowOperationSpecStand::InitFromDBRecordset( CADORecordset& recordset )
{
	int nStandID = -1;
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("STANDGROUPID"),nStandID);
	m_standGroup.ReadData(nStandID);

	//read flight type data
	//DBElementCollection<CTowOperationSpecFltTypeData>::ReadData(m_nID);
	{
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
			CTowOperationSpecFltTypeData* element = new CTowOperationSpecFltTypeData;
			element->SetAirportDatabase(m_pAirportDatabase);
			element->InitFromDBRecordset(adoRecordset);
			m_dataList.push_back(element);
			adoRecordset.MoveNextData();
		}
	}

}

void CTowOperationSpecStand::SaveData()
{
	//update it self
	m_standGroup.SaveData(m_nProjID);
	if (m_nID >= 0)
	{
		UpdateData();
	}
	else
	{
		SqlInsertScriptMaker sqlInsert(_T("AS_TOWOPERATIONSPECIFICATION"));

		sqlInsert.AddColumn(SqlScriptColumn(_T("STANDGROUPID"),m_standGroup.getID()));

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(sqlInsert.GetScript());
	}

	//update CTowOperationSpecFltTypeData
	//DBElementCollection<CTowOperationSpecFltTypeData>::SaveData(m_nID);
	{
		for (std::vector<CTowOperationSpecFltTypeData *>::iterator iter = m_dataList.begin();
			iter != m_dataList.end(); ++iter)
		{
			(*iter)->SetAirportDatabase(m_pAirportDatabase);
			(*iter)->SaveData(m_nID);
		}

		for (std::vector<CTowOperationSpecFltTypeData *>::iterator iter = m_deleteddataList.begin();
			iter != m_deleteddataList.end(); ++iter)
		{
			(*iter)->DeleteData();
		}

		m_deleteddataList.clear();
	}
}

void CTowOperationSpecStand::DeleteData()
{
	SqlDeleteScriptMaker sqlDelete(_T("AS_TOWOPERATIONSPECIFICATION"));
	
	CString strCondition;
	strCondition.Format(_T("ID = %d"), m_nID);
	sqlDelete.SetCondition(strCondition);

	CADODatabase::ExecuteSQLStatement(sqlDelete.GetScript());

	DBElementCollection<CTowOperationSpecFltTypeData>::DeleteData();

}

void CTowOperationSpecStand::UpdateData()
{
	SqlUpdateScriptMaker sqlUpdate(_T("AS_TOWOPERATIONSPECIFICATION"));
	
	sqlUpdate.AddColumn(SqlScriptColumn(_T("STANDGROUPID"),m_standGroup.getID()));

	CString strCondition;
	strCondition.Format(_T("ID"), m_nID);
	sqlUpdate.SetCondition(strCondition);



}

void CTowOperationSpecStand::GetSelectElementSQL( int nStandID,CString& strSQL ) const
{
	SqlSelectScriptMaker sqlSelect(_T("AS_TOWOPFLIGHTTYPEDATA"));
	sqlSelect.AddColumn(_T("*"));
	CString strCondition;
	strCondition.Format(_T("STANDDATAID = %d"),nStandID);
	sqlSelect.SetCondition(strCondition);
	strSQL = sqlSelect.GetScript();
}








//////////////////////////////////////////////////////////////////////////
//CTowOperationSpecification
CTowOperationSpecification::CTowOperationSpecification(int nProjID,CAirportDatabase* pAirportDatabase)
{
	m_nProjID = nProjID;
	m_pAirportDatabase = pAirportDatabase;
}

CTowOperationSpecification::~CTowOperationSpecification(void)
{
}

void CTowOperationSpecification::ReadData()
{
	
	SqlSelectScriptMaker sqlSelect(_T("AS_TOWOPERATIONSPECIFICATION"));
	sqlSelect.AddColumn(_T("*"));
	CADORecordset adoRecordset;
	long lCount = 0;
	CADODatabase::ExecuteSQLStatement(sqlSelect.GetScript(),lCount,adoRecordset);
	
	while(!adoRecordset.IsEOF())
	{
		CTowOperationSpecStand *pStandData = new CTowOperationSpecStand;
		pStandData->SetAirportDatabase(m_pAirportDatabase);
		pStandData->InitFromDBRecordset(adoRecordset);

		m_vStandData.push_back(pStandData);

		adoRecordset.MoveNextData();
	}

}

void CTowOperationSpecification::SaveData()
{
	std::vector<CTowOperationSpecStand *>::iterator iter = m_vStandData.begin();
	for (; iter != m_vStandData.end(); ++iter)
	{
		(*iter)->SaveData();
	}
	
	iter = m_vDelStandData.begin();
	for(;iter != m_vDelStandData.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}

	m_vDelStandData.clear();

}

void CTowOperationSpecification::AddNewItem( CTowOperationSpecStand * pNewItem)
{
	m_vStandData.push_back(pNewItem);
}

CTowOperationSpecFltTypeData* CTowOperationSpecification::GetTowOperationSpecFltTypeData(const FlightConstraint& fltType, const ALTObjectGroup& standGroup)
{
	int nCount = static_cast<int>(m_vStandData.size());
	for (int i =0; i < nCount; i++)
	{
		CTowOperationSpecStand* pStandData = m_vStandData.at(i);
		ALTObjectID standName = standGroup.getName();
		if (!standName.idFits(pStandData->m_standGroup.getName()) )
			continue;

		int nFltCount = static_cast<int>(pStandData->GetElementCount());
		for (int j =0; j < nFltCount; j++)
		{
			CTowOperationSpecFltTypeData* pFltData = pStandData->GetItem(j);
			if (pFltData->m_fltConstraint.fits(fltType))
				return pFltData;
		}
	}
	return NULL;
}

void CTowOperationSpecification::DeleteItem( CTowOperationSpecStand * pDelItem)
{
	std::vector<CTowOperationSpecStand *>::iterator iterFind = std::find(m_vStandData.begin(),m_vStandData.end(), pDelItem);
	if(iterFind != m_vStandData.end())
	{
		m_vDelStandData.push_back(*iterFind);
		m_vStandData.erase(iterFind);
	}

}

int CTowOperationSpecification::GetItemCount()
{
	return static_cast<int>(m_vStandData.size());

}

CTowOperationSpecStand * CTowOperationSpecification::GetItem( int nIndex )
{
	ASSERT(nIndex >=0 && nIndex < GetItemCount());
	if(nIndex >=0 && nIndex < GetItemCount())
		return m_vStandData.at(nIndex);

	return NULL;
}



































