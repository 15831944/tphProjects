#include "StdAfx.h"
#include ".\takeofftimerangerunwayassignitem.h"
#include "..\Database\ADODatabase.h"
//////////////////////////////////////////////////////////////////////////


void CTakeOffPosition::DeleteDate(std::vector<CTakeOffPosition*>& _data)
{
	try
	{
		CADODatabase::BeginTransaction( ) ;
		CString SQL ;
		for (int i = 0 ; i < (int)_data.size() ;i++)
		{
			SQL = CTakeOffPosition::GetDeleteSQL(_data[i]) ;
			CADODatabase::ExecuteSQLStatement(SQL) ;

			_data[i]->m_runwayExitDescription.DeleteData();
		}
		CADODatabase::CommitTransaction() ;
	}catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}
}
void CTakeOffPosition::SaveData(std::vector<CTakeOffPosition*>& _data ,int _Parid)
{
	CString SQL ;
	try
	{
		CADODatabase::BeginTransaction() ;

		for (int i = 0 ;i < (int)_data.size() ;i++)
		{
			_data[i]->m_runwayExitDescription.SaveData();
			if(_data[i]->m_ID == -1)
				SQL = CTakeOffPosition::GetInsertSQL(_data[i],_Parid) ;
			else
				SQL = CTakeOffPosition::GetUpdateSQL(_data[i],_Parid) ;
			int _id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			if(_data[i]->m_ID == -1)
				_data[i]->m_ID = _id ;
		}

		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}
}
void CTakeOffPosition::ReadData(std::vector<CTakeOffPosition*>& _data ,int _Parid)
{
	CString SQL ;
	SQL = CTakeOffPosition::GetSelectSQL(_Parid) ;

	long count ;
	CADORecordset dataset ;
	try
	{
		CADODatabase::BeginTransaction() ;
		CADODatabase::ExecuteSQLStatement(SQL,count,dataset) ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CString val ;
	int _id ;
	double floatVal = 0 ;
	while(!dataset.IsEOF())
	{
		CTakeOffPosition* itemdata = new CTakeOffPosition;
		dataset.GetFieldValue(_T("ID"),_id) ;
		itemdata->m_ID = _id ;

		dataset.GetFieldValue(_T("TAKEOFF_POSITION"),val) ;
		itemdata->m_strName = val ;

		dataset.GetFieldValue(_T("RWEXITDESID"),_id);
		itemdata->m_runwayExitDescription.ReadData(_id);

		dataset.GetFieldValue(_T("PRESENT"),floatVal) ;
		itemdata->m_dPercent = floatVal ;

		_data.push_back(itemdata) ;
		dataset.MoveNextData() ;
	}
}
CString CTakeOffPosition::GetSelectSQL(int _Parid)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_TAKEOFF_POSITION WHERE PAR_ID = %d"),_Parid) ;
	return SQL ;
}
CString CTakeOffPosition::GetInsertSQL(CTakeOffPosition* _position ,int _Parid)
{
		CString SQL ;
	if(_position == NULL || _Parid == -1)
		return SQL;
	SQL.Format(_T("INSERT INTO TB_TAKEOFF_POSITION (PAR_ID,TAKEOFF_POSITION,POSITION_ID,RWEXITDESID,PRESENT) VALUES(%d,'%s',%d,%d,%0.2f)"),
		_Parid,
		_position->m_strName,
		_position->m_TakeoffID,
		_position->m_runwayExitDescription.GetID(),
		_position->m_dPercent
		) ;
	return SQL ;
}
CString CTakeOffPosition::GetUpdateSQL(CTakeOffPosition* _position,int _Parid)
{
		CString SQL ;
	if(_position == NULL || _Parid == -1)
		return SQL;	
	SQL.Format(_T("UPDATE TB_TAKEOFF_POSITION SET PAR_ID = %d ,TAKEOFF_POSITION = '%s',POSITION_ID = %d ,RWEXITDESID = %d,PRESENT = %0.2f WHERE ID = %d"),
		_Parid,
		_position->m_strName,
		_position->m_TakeoffID,
		_position->m_runwayExitDescription.GetID(),
		_position->m_dPercent,
		_position->m_ID) ;
	return SQL ;
}
CString CTakeOffPosition::GetDeleteSQL(CTakeOffPosition* _position)
{
	CString SQL ;
	if(_position == NULL)
		return SQL;

	SQL.Format(_T("DELETE * FROM TB_TAKEOFF_POSITION WHERE ID = %d"),_position->m_ID) ;
	return SQL ;
}

CTakeOffPosition::CTakeOffPosition()
:m_TakeoffID(-1)
,m_dPercent(0)
,m_ID(-1)
{

}

//////////////////////////////////////////////////////////////////////////
CString CTakeOffPriorityRunwayItem::GetSelectSQL(int _parid)
{
	CString SQL ;
	if(_parid == -1)
		return SQL ;
	SQL.Format(_T("SELECT * FROM TB_TAKEOFF_RUNWAY WHERE PARENT_ID = %d "),_parid) ;
	return SQL ;
}
CString CTakeOffPriorityRunwayItem::GetInsertSQL(int _parID)
{
	CString SQL ;
	if(_parID == -1 || m_ID != -1)
		return SQL ;

	SQL.Format(_T("INSERT INTO TB_TAKEOFF_RUNWAY (PARENT_ID,RUNWAY,RUNWAYID,MARKETID,\
				  DecisionPointType,TakeOffInSlotTime,NoFactorTrailingACSep,BackTrackOption) \
				  VALUES(%d,'%s',%d,%d,%d,%d,%d,%d)"),_parID,m_strRwyName,m_RunwayID,\
				  m_nRwyMark,(int)m_eDecision,m_bTakeOffInSlotTime?1:0,m_bNoFactorForTrailingACSep?1:0,(int)m_eBackTrackOption) ;
	return SQL ;
}
CString CTakeOffPriorityRunwayItem::GetUpdateSQL()
{
	CString SQL ;
	if(m_ID == -1)
		return SQL ;

	SQL.Format(_T("UPDATE TB_TAKEOFF_RUNWAY SET RUNWAY = '%s' ,RUNWAYID = %d, MARKETID = %d,\
				  DecisionPointType= %d,TakeOffInSlotTime= %d,NoFactorTrailingACSep= %d,BackTrackOption= %d WHERE ID = %d"),m_strRwyName,\
		m_RunwayID,m_nRwyMark,(int)m_eDecision,m_bTakeOffInSlotTime?1:0,m_bNoFactorForTrailingACSep?1:0,(int)m_eBackTrackOption,\
		m_ID) ;
	return SQL ;
}

void CTakeOffPriorityRunwayItem::ReadData(int _parid)
{
	if(_parid == -1)
		return ;
	CString SQL ;
	SQL = GetSelectSQL(_parid) ;
	long count = 0 ;
	CADORecordset _DataSet ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,count,_DataSet) ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}

	CString _strVal ; 
	if(!_DataSet.IsEOF())
	{
		_DataSet.GetFieldValue(_T("ID"),m_ID) ;
		_DataSet.GetFieldValue(_T("RUNWAY"),m_strRwyName) ;
		_DataSet.GetFieldValue(_T("RUNWAYID"),m_RunwayID) ;
		_DataSet.GetFieldValue(_T("MARKETID"),m_nRwyMark) ;

		int nDecision,nTakeOffInSlotTime,nNoFactorForTrailingACSep,nBackTrackOption;
		_DataSet.GetFieldValue(_T("DecisionPointType"),nDecision) ;
		_DataSet.GetFieldValue(_T("TakeOffInSlotTime"),nTakeOffInSlotTime) ;
		_DataSet.GetFieldValue(_T("NoFactorTrailingACSep"),nNoFactorForTrailingACSep) ;
		_DataSet.GetFieldValue(_T("BackTrackOption"),nBackTrackOption) ;

		m_eDecision=(TakeOffDecisionPointType)nDecision;
		m_bTakeOffInSlotTime=(nTakeOffInSlotTime==1);
		m_bNoFactorForTrailingACSep=(nNoFactorForTrailingACSep==1);
		m_eBackTrackOption=(TakeOffBackTrack)nBackTrackOption;

		CTakeOffPosition::ReadData(m_TakeOffPosition,m_ID) ;

		CString strSQL;
		strSQL.Format(_T("SELECT * FROM TB_TAKEOFF_INTERSECTION WHERE ParentID=%d"),m_ID);
		CADORecordset recordset;
		try{
			CADODatabase::ExecuteSQLStatement(strSQL,count,recordset) ;
		}catch(CADOException& e)
		{
			e.ErrorMessage() ;
			return ;
		}
		while(!recordset.IsEOF())
		{
			IntersectionNode node;
			int nIntersectionID;
			recordset.GetFieldValue(_T("Intersection"),nIntersectionID);

			node.ReadData(nIntersectionID);
			m_vIntersection.push_back(node);
			recordset.MoveNextData();
		}


		_DataSet.MoveNextData() ;
	}
}
void CTakeOffPriorityRunwayItem::SaveData(int _parid)
{
	if(_parid == -1)
		return ;

	CString strSQL;
	if(m_ID == -1)
	{
		strSQL = GetInsertSQL(_parid) ;
	}
	else
		strSQL = GetUpdateSQL() ;

	try
	{
		CADODatabase::BeginTransaction() ;
		int _id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL) ;
		if(m_ID == -1)
			m_ID = _id ;

		CTakeOffPosition::SaveData(m_TakeOffPosition,m_ID) ;
		CTakeOffPosition::DeleteDate(m_DelTakeOffPosition);
		
		strSQL.Format("DELETE * FROM TB_TAKEOFF_INTERSECTION WHERE ParentID=%d",m_ID);
		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL) ;

		for (int i=0;i<(int)m_vIntersection.size();i++)
		{
			strSQL.Format("INSERT INTO TB_TAKEOFF_INTERSECTION (ParentID,Intersection)\
						  VALUES (%d,%d)",m_ID,m_vIntersection.at(i).GetID());
			CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL) ;
		}
		

		RemoveDelData();
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage() ;
		return ;
	}

}
void CTakeOffPriorityRunwayItem::DeleteData()
{

	if(m_ID == -1)
		return ;

	CString strSQL;
	strSQL.Format(_T("DELETE * FROM TB_TAKEOFF_RUNWAY WHERE ID = %d"),m_ID) ;

	try
	{
		CADODatabase::BeginTransaction();
		CADODatabase::ExecuteSQLStatement(strSQL) ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}

}
CTakeOffPosition* CTakeOffPriorityRunwayItem::AddTakeoffPosition(CString _Name,const RunwayExit& exit)
{
	CTakeOffPosition* PTakeOffItem = FindTakeoffPosition(exit) ;
	if(PTakeOffItem)
		return NULL ;
	PTakeOffItem = new CTakeOffPosition ;
	PTakeOffItem->m_strName = _Name ;
	PTakeOffItem->m_TakeoffID = -1 ;
	PTakeOffItem->m_runwayExitDescription.InitData(exit);
	PTakeOffItem->m_dPercent = 0.0 ;
	m_TakeOffPosition.push_back(PTakeOffItem) ;
	return PTakeOffItem ;
		
}
void CTakeOffPriorityRunwayItem::RemoveTakeoffPosition(CTakeOffPosition* _posstion)
{
	std::vector<CTakeOffPosition*>::iterator iter = std::find(m_TakeOffPosition.begin() ,m_TakeOffPosition.end() ,_posstion) ;
	if(iter != m_TakeOffPosition.end())
	{
		m_TakeOffPosition.erase(iter) ;
		m_DelTakeOffPosition.push_back(_posstion) ;
	}
}

CTakeOffPosition* CTakeOffPriorityRunwayItem::FindTakeoffPosition(const RunwayExit& exit)
{
	for (int i = 0 ; i < (int)m_TakeOffPosition.size() ;i++)
	{
		if(exit.EqualToRunwayExitDescription(m_TakeOffPosition[i]->m_runwayExitDescription))
			return m_TakeOffPosition[i] ;
	}
	return NULL ;
}

int CTakeOffPriorityRunwayItem::AddIntersection(int nID)
{
	IntersectionNode node;
	node.ReadData(nID);
	m_vIntersection.push_back(node);
	return (int)m_vIntersection.size()-1;
}
void CTakeOffPriorityRunwayItem::DelIntersection(int nID)
{
	std::vector<IntersectionNode>::iterator iter=m_vIntersection.begin();
	for (;iter!=m_vIntersection.end();iter++)
	{
		if ((*iter).GetID()==nID)
		{
			m_vDelIntersection.push_back(*iter);
			m_vIntersection.erase(iter);
			break;
		}
	}

}
CString CTakeOffPriorityRunwayItem::GetIntersectionName(int nIndex)
{
	if (nIndex<0||nIndex>=(int)m_vIntersection.size())
	{
		return _T("");
	}
	return m_vIntersection.at(nIndex).GetName();
}
int CTakeOffPriorityRunwayItem::GetIntersectionID(int nIndex)
{
	if (nIndex<0||nIndex>=(int)m_vIntersection.size())
	{
		return -1;
	}
	return m_vIntersection.at(nIndex).GetID();
}
bool CTakeOffPriorityRunwayItem::IntersectionExist(int nID)
{
	std::vector<IntersectionNode>::iterator iter=m_vIntersection.begin();
	for (;iter!=m_vIntersection.end();iter++)
	{
		if ((*iter).GetID()==nID)
		{
			return true;
		}
	}
	return false;
}
void CTakeOffPriorityRunwayItem::RemoveAllData()
{
	for (int i = 0 ; i < (int)m_TakeOffPosition.size() ;i++)
	{
			delete m_TakeOffPosition[i] ;
	}
	m_TakeOffPosition.clear() ;
	m_vIntersection.clear();
	RemoveDelData() ;
}

void CTakeOffPriorityRunwayItem::RemoveDelData()
{
	for (int i = 0 ; i < (int)m_DelTakeOffPosition.size() ;i++)
	{
		delete m_DelTakeOffPosition[i] ;
	}
	m_DelTakeOffPosition.clear() ;

	m_vDelIntersection.clear();

}

CTakeOffPriorityRunwayItem::~CTakeOffPriorityRunwayItem()
{
	RemoveAllData() ;
}

CTakeOffPriorityRunwayItem::CTakeOffPriorityRunwayItem() 
:m_ID(-1)
,m_RunwayID(-1)
,m_nRwyMark(-1)
,m_eDecision(DP_DepStand)
,m_bTakeOffInSlotTime(false)
,m_bNoFactorForTrailingACSep(false)
,m_eBackTrackOption(BT_NoBacktrack)
{

}

bool CTakeOffPriorityRunwayItem::HasIntersecion( int intID ) const
{
	for(size_t i=0;i<m_vIntersection.size();++i)
	{
		if(m_vIntersection.at(i).GetID()==intID)
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////



void CManageAssignPriorityData::RemoveData()
{
	delete m_pRunwayItem;
	m_pRunwayItem = NULL;
}
void CManageAssignPriorityData::RemoveDelData()
{
	for (int i = 0 ; i < (int)m_DelItem.size() ;i++)
	{
		CTakeOffPriorityRunwayItem* pItem = m_DelItem.at(i);
		pItem->DeleteData();

		delete pItem ;
		pItem = NULL;
	}
	m_DelItem.clear() ;
}

void CManageAssignPriorityData::RemoveRunwayItem(CTakeOffPriorityRunwayItem* _item)
{
	if (m_pRunwayItem == _item)
		m_DelItem.push_back(m_pRunwayItem);

	m_pRunwayItem = NULL;

}
CString CManageAssignPriorityData::GetSelectSQL(int _ParID)
{
	CString SQL ;
	if(_ParID == -1)
		return SQL;
	SQL.Format(_T("SELECT * FROM TB_TIMERANGETAKEOFF_PRIORITY WHERE TIMERANGE_ID = %d"),_ParID) ;

	return SQL ;
}
CString CManageAssignPriorityData::GetInsertSQL(CManageAssignPriorityData* _data ,int _ParID)
{
	CString SQL ;
	if(_data->m_ID != -1)
		return GetUpdateSQL(_data,_ParID) ;
	SQL.Format(_T("INSERT INTO TB_TIMERANGETAKEOFF_PRIORITY (PRIORITY_NAME,AIRCRAFT_NUM,PRIORITY_TIME,TIMERANGE_ID) VALUES('%s',%d,%d,%d)") ,
		_data->m_Name,
		_data->m_AircraftNum,
		_data->m_time,
		_ParID) ;
	return SQL ;
}
CString CManageAssignPriorityData::GetUpdateSQL(CManageAssignPriorityData* _data , int _parID)
{
	CString SQL ;
	if(_data->m_ID == -1)
		return GetInsertSQL(_data,_parID) ;
	SQL.Format(_T("UPDATE TB_TIMERANGETAKEOFF_PRIORITY SET PRIORITY_NAME = '%s',AIRCRAFT_NUM = %d ,PRIORITY_TIME = %d ,TIMERANGE_ID = %d WHERE ID = %d") ,
		_data->m_Name,
		_data->m_AircraftNum,
		_data->m_time,
		_parID,
		_data->m_ID) ;
	return SQL ;
}
void CManageAssignPriorityData::ReadData(std::vector<CManageAssignPriorityData*>& _data ,int _parID)
{
	if(_parID == -1)
		return ;
	CString SQL ;
	SQL = CManageAssignPriorityData::GetSelectSQL(_parID) ;
	long Count ;
	CADORecordset _DataSet ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,Count,_DataSet) ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CManageAssignPriorityData* PManageData = NULL ;
	CString _strName ;
	int Val = 0 ;
	while(!_DataSet.IsEOF())
	{
		PManageData = new CManageAssignPriorityData ;

		_DataSet.GetFieldValue(_T("PRIORITY_NAME"),_strName) ;
		PManageData->m_Name = _strName ;
		_DataSet.GetFieldValue(_T("AIRCRAFT_NUM"),Val) ;
		PManageData->m_AircraftNum = Val ;
		_DataSet.GetFieldValue(_T("PRIORITY_TIME"),Val) ;
		PManageData->m_time = (Val) ;
		_DataSet.GetFieldValue(_T("ID"),Val) ;
		PManageData->m_ID = Val ;

		PManageData->m_pRunwayItem = new CTakeOffPriorityRunwayItem;
		
		PManageData->m_pRunwayItem->ReadData(PManageData->m_ID) ;
		_data.push_back(PManageData) ;

		_DataSet.MoveNextData() ;
	}
	if(PManageData)
	{
		PManageData->SetHasChangeCondition(false);
	}
}
void CManageAssignPriorityData::SaveData(std::vector<CManageAssignPriorityData*>& _data ,int _parID)
{
	CManageAssignPriorityData* PData = NULL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		CString SQL ;
		int _id = -1 ;
		PData = _data[i] ;
		if(PData->m_ID == -1)
			SQL = CManageAssignPriorityData::GetInsertSQL(PData,_parID) ;
		else
			SQL = CManageAssignPriorityData::GetUpdateSQL(PData,_parID) ;

		try
		{
			CADODatabase::BeginTransaction() ;
			_id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			if(PData->m_ID == -1)
				PData->m_ID = _id ;
			PData->m_pRunwayItem->SaveData(PData->m_ID) ;
			PData->RemoveDelData() ;
			CADODatabase::CommitTransaction() ;
		}catch(CADOException& e)
		{
			e.ErrorMessage() ;
			CADODatabase::RollBackTransation() ;
			return ;
		}
	}
}
void CManageAssignPriorityData::DeleteData(std::vector<CManageAssignPriorityData*> _data)
{
	CManageAssignPriorityData* PData = NULL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		CString SQL ;
		int _id = -1 ;
		PData = _data[i] ;
		if(PData->m_ID == -1)
			continue ;
		SQL = CManageAssignPriorityData::GetDeleteSQL(PData);

		try
		{
			CADODatabase::BeginTransaction() ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
			CADODatabase::CommitTransaction() ;
		}catch(CADOException& e)
		{
			e.ErrorMessage() ;
			CADODatabase::RollBackTransation() ;
			return ;
		}

	}
}
CString CManageAssignPriorityData::GetDeleteSQL(CManageAssignPriorityData* _data)
{
	CString SQL ;

	SQL.Format(_T("DELETE * FROM TB_TIMERANGETAKEOFF_PRIORITY WHERE ID = %d"),_data->m_ID) ;

	return SQL ;
}

void CManageAssignPriorityData::GetAssignRunway(int& nRwyID, int& nMark )
{
	nRwyID = m_pRunwayItem->m_RunwayID;
	nMark = m_pRunwayItem->m_nRwyMark;

}
void CManageAssignPriorityData::SetHasChangeCondition(bool bHasCondition)
{
	m_bHasChangeCondition=bHasCondition;
	if (!m_bHasChangeCondition)
	{
		SetDefaultChangeCondition();
	}
}
void CManageAssignPriorityData::SetDefaultChangeCondition()
{
	m_AircraftNum=10;
	m_time=10;
	m_pRunwayItem->SetCanTakeOffInSlotTime(false);
	m_pRunwayItem->SetNoFactorForTrailingACSep(false);
}

void CManageAssignPriorityData::SetRunwayItem( CTakeOffPriorityRunwayItem* pItem )
{
	if (m_pRunwayItem && m_pRunwayItem != pItem)
		m_DelItem.push_back(m_pRunwayItem);

	m_pRunwayItem = pItem;
}

CTakeOffPriorityRunwayItem* CManageAssignPriorityData::GetRunwayItem()
{
	return m_pRunwayItem;
}

CManageAssignPriorityData::~CManageAssignPriorityData()
{
	RemoveData() ; 
	RemoveDelData() ;
}

CManageAssignPriorityData::CManageAssignPriorityData() 
:m_ID(-1)
,m_AircraftNum(10)
,m_time(10)
,m_pRunwayItem(NULL)
,m_bHasChangeCondition(true)
{
	m_DelItem.clear();
}
//////////////////////////////////////////////////////////////////////////

CTakeOffTimeRangeRunwayAssignItem::CTakeOffTimeRangeRunwayAssignItem()
:m_ID(-1)
,m_emStrategyType(FlightPlannedRunway)
{

}
CTakeOffTimeRangeRunwayAssignItem::~CTakeOffTimeRangeRunwayAssignItem()
{
	RemoveAll() ;
}
void CTakeOffTimeRangeRunwayAssignItem::ReadPriorityData()
{
	RemoveAll() ;
	CManageAssignPriorityData::ReadData(m_Data,m_ID) ;
}
void CTakeOffTimeRangeRunwayAssignItem::RemoveAll()
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;

	RemoveDeleteData() ;
}
void CTakeOffTimeRangeRunwayAssignItem::RemoveDeleteData()
{
	for (int i = 0 ; i < (int)m_DelData.size() ;i++)
	{
		delete m_DelData[i] ;
	}
	m_DelData.clear() ;
}
void CTakeOffTimeRangeRunwayAssignItem::WritePriorityData()
{
	CManageAssignPriorityData::SaveData(m_Data,m_ID) ;

	CManageAssignPriorityData::DeleteData(m_DelData) ;

	RemoveDeleteData() ;
}
CManageAssignPriorityData* CTakeOffTimeRangeRunwayAssignItem::AddPriorityRwyData(const CString& strPriority,int nRwyID, int nRwyMark)
{
// 	CManageAssignPriorityData* pPriorityData = FindPriorityRwyData(nRwyID, nRwyMark) ;
// 	if(pPriorityData)
// 		return NULL ;

	CManageAssignPriorityData* pPriorityData;
	pPriorityData = new CManageAssignPriorityData ;
	pPriorityData->m_Name =  strPriority;

	CTakeOffPriorityRunwayItem* pRwyItem = new CTakeOffPriorityRunwayItem;
	pRwyItem->m_RunwayID = nRwyID ;
	pRwyItem->m_nRwyMark = nRwyMark;
	pPriorityData->SetRunwayItem(pRwyItem);
	m_Data.push_back(pPriorityData) ;

	return pPriorityData ;
}
CManageAssignPriorityData* CTakeOffTimeRangeRunwayAssignItem::FindPriorityRwyData(int nRwyID, int nRwyMark)
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		CManageAssignPriorityData* pPriorityData = m_Data.at(i);
		if (pPriorityData->m_pRunwayItem->m_RunwayID == nRwyID && pPriorityData->m_pRunwayItem->m_nRwyMark == nRwyMark)
			return pPriorityData;
	}

	return NULL ;
}
void CTakeOffTimeRangeRunwayAssignItem::RemovePriorityRwyData(CManageAssignPriorityData* _PData)
{
	std::vector<CManageAssignPriorityData*>::iterator iter = std::find(m_Data.begin(),m_Data.end(),_PData) ;
	if(iter != m_Data.end())
	{
		m_Data.erase(iter);
		m_DelData.push_back(_PData) ;
	}
}
CString CTakeOffTimeRangeRunwayAssignItem::GetSelectSQL(int _parID)
{
	CString SQL ;
	if(_parID == -1)
		return SQL ;
	SQL.Format(_T("SELECT * FROM TB_TAKEOFFRUNWAY_TIMERANGE WHERE PAR_ID = %d"),_parID) ;
	return SQL ;
}
CString CTakeOffTimeRangeRunwayAssignItem::GetInsertSQL(CTakeOffTimeRangeRunwayAssignItem* _item ,int _parID)
{
	CString SQL ;
	if(_parID == -1)
		return SQL ;
	SQL.Format(_T("INSERT INTO TB_TAKEOFFRUNWAY_TIMERANGE (START_TIME,END_TIME,TIME_TYPE,PAR_ID) VALUES(%d,%d,%d,%d)") ,
		_item->GetStartTime().asSeconds(),
		_item->GetEndTime().asSeconds(),
		_item->GetTimeRangeType(),
		_parID) ;
	return SQL ;
}
CString CTakeOffTimeRangeRunwayAssignItem::GetUpdateSQL(CTakeOffTimeRangeRunwayAssignItem* _item ,int _parID)
{
	CString SQL ;
	SQL.Format(_T("UPDATE TB_TAKEOFFRUNWAY_TIMERANGE SET START_TIME = %d,END_TIME = %d,TIME_TYPE = %d ,PAR_ID = %d WHERE ID = %d"),
		_item->GetStartTime().asSeconds(),
		_item->GetEndTime().asSeconds(),
		_item->GetTimeRangeType(),
		_parID,
		_item->GetID()) ;
	return SQL ;
}
CString CTakeOffTimeRangeRunwayAssignItem::GetDeleteSQL(CTakeOffTimeRangeRunwayAssignItem* _item )
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_TAKEOFFRUNWAY_TIMERANGE WHERE ID = %d"),_item->GetID()) ;
	return SQL ;
}
void CTakeOffTimeRangeRunwayAssignItem::ReadData(std::vector<CTakeOffTimeRangeRunwayAssignItem*>& _data,int _ParID)
{
	CString SQL ;
	if(_ParID == -1)
		return ;
	SQL = CTakeOffTimeRangeRunwayAssignItem::GetSelectSQL(_ParID) ;
	long count = 0 ;
	CADORecordset recordSet ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordSet) ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	int val = 0 ;
	CTakeOffTimeRangeRunwayAssignItem*  PItemData = NULL ;
	while(!recordSet.IsEOF())
	{
		PItemData = new CTakeOffTimeRangeRunwayAssignItem ;
		ElapsedTime timeStart, timeEnd ;
		recordSet.GetFieldValue(_T("START_TIME"),val) ;
		timeStart.SetSecond(val) ;
		PItemData->SetStartTime(timeStart) ;

		recordSet.GetFieldValue(_T("END_TIME"),val) ;
		timeEnd.SetSecond(val) ;
		PItemData->SetEndTime(timeEnd) ;

		recordSet.GetFieldValue(_T("TIME_TYPE"),val) ;
		PItemData->SetTimeRangeType((RunwayAssignmentStrategyType)val) ;

		recordSet.GetFieldValue(_T("ID"),val) ;
		PItemData->SetID(val) ;

		PItemData->ReadPriorityData() ;
		_data.push_back(PItemData) ;

		recordSet.MoveNextData() ;
	}
}
void CTakeOffTimeRangeRunwayAssignItem::WriteData(std::vector<CTakeOffTimeRangeRunwayAssignItem*>& _data ,int _parID)
{
	if(_parID == -1)
		return ;
	CTakeOffTimeRangeRunwayAssignItem* PtimeRangeItem = NULL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		PtimeRangeItem = _data[i] ;
		CString SQL ;
		if(PtimeRangeItem->GetID() == -1)
			SQL = CTakeOffTimeRangeRunwayAssignItem::GetInsertSQL(PtimeRangeItem,_parID) ;
		else
			SQL = CTakeOffTimeRangeRunwayAssignItem::GetUpdateSQL(PtimeRangeItem,_parID) ;

		try
		{
			CADODatabase::BeginTransaction( ) ;
			int _id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			if(PtimeRangeItem->GetID() == -1)
				PtimeRangeItem->SetID(_id) ;
			PtimeRangeItem->WritePriorityData() ;
			CADODatabase::CommitTransaction() ;
		}
		catch (CADOException& e)
		{
			CADODatabase::RollBackTransation() ;
			e.ErrorMessage() ;
			return ;
		}
	}
}
void CTakeOffTimeRangeRunwayAssignItem::DeleteData(std::vector<CTakeOffTimeRangeRunwayAssignItem*>& _data)
{
	CTakeOffTimeRangeRunwayAssignItem* PTimeRange = NULL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		PTimeRange = _data[i] ;

		CString SQL ;
		SQL = CTakeOffTimeRangeRunwayAssignItem::GetDeleteSQL(PTimeRange) ;

		try
		{
			CADODatabase::BeginTransaction() ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
			CADODatabase::CommitTransaction() ;
		}
		catch (CADOException& e)
		{
			CADODatabase::RollBackTransation() ;
			e.ErrorMessage() ;
			return ;
		}
	}
}

bool CTakeOffTimeRangeRunwayAssignItem::IsAssignedRunway( int nRwyID, int nRwyMark )
{
	if (m_emStrategyType != ManagedAssignment)
		return true;
	
	size_t nCount = m_Data.size();
	for (size_t i =0; i < nCount; i++)
	{
		CManageAssignPriorityData* pData = m_Data.at(i);
		int nRunway, nMark;
		pData->GetAssignRunway(nRunway, nMark);
		if(nRunway == nRwyID && nMark == nRwyMark)
			return true;
	}

	return false;
}