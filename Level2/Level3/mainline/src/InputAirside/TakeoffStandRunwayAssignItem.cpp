#include "StdAfx.h"
#include ".\takeoffstandrunwayassignitem.h"
#include "FlightTypeRunwayAssignmentStrategyItem.h"
#include "..\Database\ADODatabase.h"
#include "../Common/ALTObjectID.h"

CTakeoffFlightTypeRunwayAssignment* CTakeoffStandRunwayAssignItem::AddTakeoffFlightType(const FlightConstraint& _FltTy)
{
	CTakeoffFlightTypeRunwayAssignment* _FltItem = FindFlightTypeItem(_FltTy) ;
	if(_FltItem)
		return NULL ;

	_FltItem = new CTakeoffFlightTypeRunwayAssignment ;
	_FltItem->setFlightType(_FltTy) ;
	m_FlightData.push_back(_FltItem) ;
	return _FltItem ;
}
void CTakeoffStandRunwayAssignItem::RemoveTakeOffFlightType(CTakeoffFlightTypeRunwayAssignment* _item)
{
	TY_DATA_ITER iter = std::find(m_FlightData.begin() , m_FlightData.end() , _item) ;
	if(iter != m_FlightData.end())
	{
		m_FlightData.erase(iter) ;
		m_DelFlightData.push_back(_item) ;
	}
}
CTakeoffFlightTypeRunwayAssignment* CTakeoffStandRunwayAssignItem::FindFlightTypeItem(const FlightConstraint& _FltTy)
{
	TY_DATA_ITER iter = m_FlightData.begin() ;
	for ( ; iter != m_FlightData.end() ;iter++)
	{
		if((*iter)->GetFlightType().isEqual(&_FltTy))
			return *iter ;
	}
	return NULL ;
}
void CTakeoffStandRunwayAssignItem::ReadData(std::vector<CTakeoffStandRunwayAssignItem*>& _dataSet,int _ParID,CAirportDatabase* _AirportDB)
{

	CString SQL ;
	SQL = CTakeoffStandRunwayAssignItem::GetSelectSQL(_ParID) ;

	long count = 0 ;
	CADORecordset recordSet ;

	try
	{
		CADODatabase::BeginTransaction() ;
		CADODatabase::ExecuteSQLStatement(SQL,count,recordSet) ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation( ) ;
		return ;
	}
	CString strVal ;
	int _id = -1;
	while(!recordSet.IsEOF())
	{
		CTakeoffStandRunwayAssignItem* PStandItem = new CTakeoffStandRunwayAssignItem ;
		recordSet.GetFieldValue(_T("STAND"),strVal) ;
		PStandItem->SetStand(strVal) ;
		recordSet.GetFieldValue(_T("ID"),_id) ;
		PStandItem->SetID(_id) ;
		CTakeoffFlightTypeRunwayAssignment::ReadData(PStandItem->m_FlightData,_id,_AirportDB) ;
		_dataSet.push_back(PStandItem) ;
		recordSet.MoveNextData() ;
	}
}
void CTakeoffStandRunwayAssignItem::WriteData(std::vector<CTakeoffStandRunwayAssignItem*>&_dataSet,int _parID)
{
	CTakeoffStandRunwayAssignItem* PStandItem = NULL ;
	try
	{
		CADODatabase::BeginTransaction() ;
		for (int i = 0 ; i < (int)_dataSet.size() ;i++)
		{
			PStandItem = _dataSet[i] ;

			CString SQL ;
			if(PStandItem->GetID() == -1)
				SQL = CTakeoffStandRunwayAssignItem::GetInsertSQL(PStandItem,_parID) ;
			else
				SQL = CTakeoffStandRunwayAssignItem::GetUpdateSQL(PStandItem,_parID) ;

			int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			if(PStandItem->GetID() == -1)
				PStandItem->SetID(id) ;

			CTakeoffFlightTypeRunwayAssignment::WriteData(PStandItem->m_FlightData,PStandItem->GetID()) ;
			CTakeoffFlightTypeRunwayAssignment::DeleteData(PStandItem->m_DelFlightData) ;
			PStandItem->RemoveDeleteData() ;
		}
		CADODatabase::CommitTransaction() ;
	}catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}
}
void CTakeoffStandRunwayAssignItem::DeleteData(std::vector<CTakeoffStandRunwayAssignItem*>& _dataSet)
{
	CTakeoffStandRunwayAssignItem* PStandItem = NULL ;
	try{
		CADODatabase::BeginTransaction() ;
		for (int i = 0 ; i < (int)_dataSet.size() ;i++)
		{
			PStandItem = _dataSet[i] ;

			CString SQL ;
			SQL = CTakeoffStandRunwayAssignItem::GetDeleteSQL(PStandItem) ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
		}
		CADODatabase::CommitTransaction() ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}
}
void CTakeoffStandRunwayAssignItem::RemoveAllData()
{
	for (int i = 0 ; i < (int)m_FlightData.size() ;i++)
	{
		delete m_FlightData[i] ;
	}
	m_FlightData.clear() ;
}
void CTakeoffStandRunwayAssignItem::RemoveDeleteData()
{
	for (int i = 0 ; i < (int)m_DelFlightData.size() ;i++)
	{
		delete m_DelFlightData[i] ;
	}
	m_DelFlightData.clear() ;
}
CString CTakeoffStandRunwayAssignItem::GetSelectSQL(int _parID)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_TAKEOFFRUNWAY_STANDITEM")) ;
	return SQL ;
}
CString CTakeoffStandRunwayAssignItem::GetInsertSQL(CTakeoffStandRunwayAssignItem* _Pitem , int _parID)
{
	CString SQL ;
	SQL.Format(_T("INSERT INTO TB_TAKEOFFRUNWAY_STANDITEM (STAND) VALUES('%s')"),_Pitem->GetStand()) ;
	return SQL ;
}
CString CTakeoffStandRunwayAssignItem::GetUpdateSQL(CTakeoffStandRunwayAssignItem* _Pitem , int _parID)
{
	CString SQL ;
	SQL.Format(_T("UPDATE TB_TAKEOFFRUNWAY_STANDITEM SET STAND = '%s' WHERE ID = %d"),_Pitem->GetStand(),_Pitem->GetID()) ;
	return SQL ;
}
CString CTakeoffStandRunwayAssignItem::GetDeleteSQL(CTakeoffStandRunwayAssignItem* _Pitem)
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_TAKEOFFRUNWAY_STANDITEM WHERE ID = %d"),_Pitem->GetID()) ;
	return SQL ;
}

CTakeoffStandRunwayAssignItem::CTakeoffStandRunwayAssignItem():m_ID(-1)
{

}
CTakeoffStandRunwayAssignItem::~CTakeoffStandRunwayAssignItem()
{
	RemoveAllData() ;
}

//////////////////////////////////////////////////////////////////////////

void CTakeoffStandRunwayAssign::ClearData()
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;

	ClearDelData() ;
}
void CTakeoffStandRunwayAssign::ClearDelData()
{
	for (int i = 0 ; i < (int)m_DelData.size() ;i++)
	{
		delete m_DelData[i] ;
	}
	m_DelData.clear() ;
}
CTakeoffStandRunwayAssign::CTakeoffStandRunwayAssign(int _proID,CAirportDatabase* _airportDB):m_ProID(_proID),m_AirportDB(_airportDB)
{

}
CTakeoffStandRunwayAssign::~CTakeoffStandRunwayAssign()
{
	ClearData() ;
}
void CTakeoffStandRunwayAssign::ReadData()
{
	ClearData() ;
	CTakeoffStandRunwayAssignItem::ReadData(m_Data,-1,m_AirportDB) ;
}
void CTakeoffStandRunwayAssign::WriteData()
{
	CTakeoffStandRunwayAssignItem::WriteData(m_Data,-1) ;
	CTakeoffStandRunwayAssignItem::DeleteData(m_DelData) ;
	ClearDelData() ;
}
CTakeoffStandRunwayAssignItem* CTakeoffStandRunwayAssign::AddTakeoffStandItem(const CString& _stand)
{
	CTakeoffStandRunwayAssignItem* PStandItem = FindStandItem(_stand) ;
	if(PStandItem )
		return NULL ;
	PStandItem = new CTakeoffStandRunwayAssignItem ;
	PStandItem->SetStand(_stand) ;
	m_Data.push_back(PStandItem) ;
	return PStandItem ;
}
void CTakeoffStandRunwayAssign::RemoveStandItem(CTakeoffStandRunwayAssignItem* _standItem)
{
	std::vector<CTakeoffStandRunwayAssignItem*>::iterator iter = std::find(m_Data.begin(),m_Data.end(),_standItem) ;
	if(iter != m_Data.end())
	{
		m_Data.erase(iter) ;
		m_DelData.push_back(_standItem) ;
	}
}
CTakeoffStandRunwayAssignItem* CTakeoffStandRunwayAssign::FindStandItem(const CString& _stand)
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		if(	_stand.CompareNoCase(m_Data[i]->GetStand())	== 0)
			return m_Data[i] ;
	}
	return NULL ;
}

void CTakeoffStandRunwayAssign::GeStandRunwayAssignData( const ALTObjectID& standName, std::vector<CTakeoffStandRunwayAssignItem*>& vFitStandData )
{
	size_t nCount = m_Data.size();
	for (size_t i = 0 ; i < nCount ;i++)
	{
		CTakeoffStandRunwayAssignItem* pData = m_Data.at(i);
		if (pData->GetStand().CompareNoCase("All") == 0 || pData->GetStand().CompareNoCase("All stand") == 0)
		{
			vFitStandData.push_back(pData);
			continue;
		}

		ALTObjectID objName(pData->GetStand());
		if (standName.idFits(objName))
			vFitStandData.push_back(pData);
	}
}
bool CompareStandFamilyRunwayAssignmentStrategyItem(CTakeoffStandRunwayAssignItem* fItem,CTakeoffStandRunwayAssignItem* sItem)
{
	if (fItem->GetStand().CompareNoCase("All") == 0)
		return false;

	if (sItem->GetStand().CompareNoCase("All") == 0)
		return true;

	ALTObjectID fobjName(fItem->GetStand());
	ALTObjectID sobjName(sItem->GetStand());

	if (fobjName.idFits(sobjName))
		return true;

	return false;
}

void CTakeoffStandRunwayAssign::Sort()
{
	std::sort( m_Data.begin(), m_Data.end(),CompareStandFamilyRunwayAssignmentStrategyItem);
}