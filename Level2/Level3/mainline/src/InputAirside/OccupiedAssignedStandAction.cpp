#include "StdAfx.h"
#include "OccupiedAssignedStandAction.h"
#include "..\Common\Flt_cnst.h"
#include "..\Common\AirportDatabase.h"
#include ".\ALTObjectGroup.h"

CirculateRoute::CirculateRoute( void )
	:m_nParentID(-1)
	, m_nRouteID(-1)
	, m_nAltObjectID(-1)
	, m_nIntersectNodeID(-1)
{

}

CirculateRoute::~CirculateRoute( void )
{

}

void CirculateRoute::SetParentID( int nParentID )
{
	m_nParentID = nParentID;
}

int CirculateRoute::GetParentID()const
{
	return m_nParentID;
}

void CirculateRoute::SetRouteID( int nRouteID )
{
	m_nRouteID = nRouteID;
}

int CirculateRoute::GetRouteID() const
{
	return m_nRouteID;
}

void CirculateRoute::SetALTObjectID( int nALTObjectID )
{
	m_nAltObjectID = nALTObjectID;
}

int CirculateRoute::GetALTObjectID() const
{
	return m_nAltObjectID;
}

void CirculateRoute::SetIntersectNodeID( int nINodeID )
{
	m_nIntersectNodeID = nINodeID;
}

int CirculateRoute::GetIntersectNodeID() const
{
	return m_nIntersectNodeID;
}

void CirculateRoute::SetItemName( CString strName )
{
	m_strItemName = strName;
}

CString CirculateRoute::GetItemName() const
{
	return m_strItemName;
}

void CirculateRoute::DeleteDataFromDB()
{
	ASSERT(0 == (int)m_dataList.size());

	for (std::vector<CirculateRoute *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();

	CString strSQL;
	GetDeleteSQL(strSQL);

	if (strSQL.IsEmpty())
	{
		return;
	}

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CirculateRoute::SaveDataToDB()
{
	CString strSQL;

	//not exist
	if (m_nID < 0)
	{
		GetInsertSQL(strSQL);

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

	for (std::vector<CirculateRoute *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetParentID(m_nID);
		(*iter)->SetRouteID(m_nRouteID);
		(*iter)->SaveDataToDB();
	}

	for (std::vector<CirculateRoute *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CirculateRoute::SaveData(int nParentID)
{
	for (std::vector<CirculateRoute *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetRouteID(nParentID);
		(*iter)->SaveDataToDB();
	}

	for (std::vector<CirculateRoute *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CirculateRoute::DeleteData()
{
	//delete content
	for (std::vector<CirculateRoute *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteData();
		m_deleteddataList.push_back(*iter);
	}

	m_dataList.clear();
}

void CirculateRoute::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("PARENTID"), m_nParentID);
	recordset.GetFieldValue(_T("CIRCULATEOUTEID"), m_nRouteID);
	recordset.GetFieldValue(_T("OBJID"), m_nAltObjectID);
	recordset.GetFieldValue(_T("INTERSECTNODEID"), m_nIntersectNodeID);
	recordset.GetFieldValue(_T("ITEMNAME"), m_strItemName);

	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT * \
						   FROM IN_CIRCULATEROUTE_DATA \
						   WHERE (CIRCULATEOUTEID = %d AND PARENTID = %d)"),
						   m_nRouteID, m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CirculateRoute *pOutboundRouteItem = new CirculateRoute();
		pOutboundRouteItem->InitFromDBRecordset(adoRecordset);

		AddNewItem(pOutboundRouteItem);

		adoRecordset.MoveNextData();
	}
}

void CirculateRoute::GetInsertSQL( CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO IN_CIRCULATEROUTE_DATA\
					 (CIRCULATEOUTEID, PARENTID, OBJID, INTERSECTNODEID, ITEMNAME)\
					 VALUES (%d, %d, %d, %d, '%s')"),\
					 m_nRouteID,
					 m_nParentID,
					 m_nAltObjectID,
					 m_nIntersectNodeID,
					 m_strItemName);
}

void CirculateRoute::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE IN_CIRCULATEROUTE_DATA \
					 SET CIRCULATEOUTEID = %d, \
					 PARENTID = %d, \
					 OBJID = %d, \
					 INTERSECTNODEID = %d, \
					 ITEMNAME = '%s'  \
					 WHERE (ID = %d)"),\
					 m_nRouteID,
					 m_nParentID,
					 m_nAltObjectID,
					 m_nIntersectNodeID,
					 m_strItemName,
					 m_nID);
}

void CirculateRoute::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM IN_CIRCULATEROUTE_DATA\
					 WHERE (ID = %d)"),m_nID);
}



CirclateRouteStrategy::CirclateRouteStrategy()
	:m_iType(0)
	,m_nIntersectionID(-1)
{
	m_pCirRoute = new CirculateRoute();
}

CirclateRouteStrategy::~CirclateRouteStrategy()
{
	if (m_pCirRoute)
	{
		delete m_pCirRoute;
		m_pCirRoute = NULL;
	}
}

void CirclateRouteStrategy::SetIntersectionID( int nIntersectionID )
{
	m_nIntersectionID = nIntersectionID;
}

int CirclateRouteStrategy::GetIntersectionID()const
{
	return m_nIntersectionID;
}

void CirclateRouteStrategy::SaveDataToDB(int nParentID)
{
	//for (size_t i = 0; i < m_pCirRoute->GetElementCount(); i++)
	//{
	//	CirculateRoute* pRoute = m_pCirRoute->GetItem(i);
	//	pRoute->SetParentID(-1);
	//	pRoute->SetRouteID(nParentID);
	//	pRoute->SaveDataToDB();
	//}
	m_pCirRoute->SaveData(nParentID);
}

CirculateRoute* CirclateRouteStrategy::GetCirculateRoute()
{
	return m_pCirRoute;
}

void CirclateRouteStrategy::SetStrategyType( int iType )
{
	m_iType = iType;
}

int CirclateRouteStrategy::GetStrategyType() const
{
	return m_iType;
}

void CirclateRouteStrategy::ReadData( int nParentID )
{
	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT * \
						   FROM IN_CIRCULATEROUTE_DATA \
						   WHERE (CIRCULATEOUTEID = %d AND PARENTID = %d)"),
						   nParentID, -1);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CirculateRoute *pCirRoute = new CirculateRoute();
		pCirRoute->InitFromDBRecordset(adoRecordset);

		m_pCirRoute->AddNewItem(pCirRoute);

		adoRecordset.MoveNextData();
	}

}

void CirclateRouteStrategy::DeleteData()
{
	m_pCirRoute->DeleteData();
	m_pCirRoute->DeleteDataFromDB();
}


//--------------------COccupiedAssignedStandStrategy--------------------

COccupiedAssignedStandStrategy::COccupiedAssignedStandStrategy()
{
	m_nID = -1;
	m_nDelaytime = 10;
	m_iTime = 0;
	//m_nIntersectionID = -1;
	addDefaultData();
}

COccupiedAssignedStandStrategy::~COccupiedAssignedStandStrategy()
{
	CleanData();
}
void COccupiedAssignedStandStrategy::CleanData()
{
// 	m_nID = -1;
// 	m_nDelaytime = 10;
// 	m_nIntersectionID = -1;
	m_vTmpParkingList.clear();
	m_vTmpStandList.clear();
	m_vOrder.clear();
}
// void COccupiedAssignedStandStrategy::SetTmpParkingList(const ObjIDList &vTmpParkingList)
// {
// 	m_vTmpParkingList.clear();
// 	m_vTmpParkingList.assign(vTmpParkingList.begin(),vTmpParkingList.end());
// }
// void COccupiedAssignedStandStrategy::GetTmpParkingList(ObjIDList &vTmpParkingList)const
// {
// 	vTmpParkingList.clear();
// 	vTmpParkingList.assign(m_vTmpParkingList.begin(),m_vTmpParkingList.end());
// }
// void COccupiedAssignedStandStrategy::SetTmpStandList(const ObjIDList &vTmpStandList)
// {
// 	m_vTmpStandList.clear();
// 	m_vTmpStandList.assign(vTmpStandList.begin(),vTmpStandList.end());
// }
// void COccupiedAssignedStandStrategy::GetTmpStandList(ObjIDList &vTmpStandList)const
// {
// 	vTmpStandList.clear();
// 	vTmpStandList.assign(m_vTmpStandList.begin(),m_vTmpStandList.end());
// }
// void COccupiedAssignedStandStrategy::GetOrder(StrategyType &vOrder[StrategyType::StrategyNum])
// {
// 	for (int i=0;i<StrategyType::StrategyNum;i++)
// 	{
// 		vOrder[i]=m_vOrder[i];
// 	}
// }


void COccupiedAssignedStandStrategy::SetDelaytime(int nDelaytime)
{
	m_nDelaytime = nDelaytime;
}
int COccupiedAssignedStandStrategy::GetDelaytime()
{
	return m_nDelaytime;
}
void COccupiedAssignedStandStrategy::SetIntersectionID(int nIntersectionID)
{
	//m_nIntersectionID = nIntersectionID;
	m_cirRouteStrategy.SetIntersectionID(nIntersectionID);
}
int COccupiedAssignedStandStrategy::GetIntersectionID()
{
	//return m_nIntersectionID;
	return m_cirRouteStrategy.GetIntersectionID();
}
ObjIDList &COccupiedAssignedStandStrategy::GetTmpParkingList()
{
	return m_vTmpParkingList;
}
ObjIDList &COccupiedAssignedStandStrategy::GetTmpStandList()
{
	return m_vTmpStandList;
}
const StrategyTypeList &COccupiedAssignedStandStrategy::GetOrder()
{
	return m_vOrder;
}
void COccupiedAssignedStandStrategy::SetTmpParkingList(const ObjIDList &parkingList)
{
	m_vTmpParkingList.clear();
	m_vTmpParkingList.assign(parkingList.begin(),parkingList.end());
}
void COccupiedAssignedStandStrategy::SetTmpStandList(const ObjIDList &standList)
{
	m_vTmpStandList.clear();
	m_vTmpStandList.assign(standList.begin(),standList.end());
}

void COccupiedAssignedStandStrategy::StrategyUp(int strategyIndex)
{
	//make the strategy that the index indicate up by one position
	if (strategyIndex<0||strategyIndex>=(int)m_vOrder.size())
	{
		return;
	}	
	StrategyType tmpStrategy=m_vOrder[strategyIndex-1];
    m_vOrder[strategyIndex-1]=m_vOrder[strategyIndex];
	m_vOrder[strategyIndex]=tmpStrategy;

}
void COccupiedAssignedStandStrategy::StrategyDown(int strategyIndex)
{
	//make the strategy that the index indicate down by one position
	if (strategyIndex<0||strategyIndex>=(int)m_vOrder.size())
	{
		return;
	}
	StrategyType tmpStrategy=m_vOrder[strategyIndex+1];
	m_vOrder[strategyIndex+1]=m_vOrder[strategyIndex];
	m_vOrder[strategyIndex]=tmpStrategy;
}
void COccupiedAssignedStandStrategy::addDefaultData()
{
	for (int i=0;i<StrategyNum;i++)
	{
		m_vOrder.push_back((StrategyType)i);
	}

}
void COccupiedAssignedStandStrategy::ReadData(CADORecordset& adoRecordset)
{
//ParentID,DelayTime,IntersectionID,Strategy1

	if (adoRecordset.IsEOF())
	{
		return;
	}

	CleanData();

	adoRecordset.GetFieldValue(_T("ID"),m_nID);

	adoRecordset.GetFieldValue(_T("DelayTime"),m_nDelaytime);

	//adoRecordset.GetFieldValue(_T("INTERSECTIONTAXIWAY1ID"),m_nIntersectionTaxiway1ID);
	//adoRecordset.GetFieldValue(_T("INTERSECTIONTAXIWAY2ID"),m_nIntersectionTaxiway2ID);
	//adoRecordset.GetFieldValue(_T("INTERSECTIONINDEX"),m_nIntersectionIndex);
	int nItersectionID = -1;
	adoRecordset.GetFieldValue(_T("IntersectionID"),nItersectionID);
	m_cirRouteStrategy.SetIntersectionID(nItersectionID);
	adoRecordset.GetFieldValue(_T("OccupyTime"),m_iTime);
	int iStrategyType = 0;
	adoRecordset.GetFieldValue(_T("strategyType"),iStrategyType);
	m_cirRouteStrategy.SetStrategyType(iStrategyType);
	int nStrategy;
	CString strStrategy;
	for (int i=0;i<StrategyNum;i++)
	{
		strStrategy.Format("Strategy%d",i);
		adoRecordset.GetFieldValue(strStrategy,nStrategy);
		m_vOrder.push_back((StrategyType)nStrategy);
	}
	CString strSQL;
	CADORecordset tmpRecordset;
	long nCount;
	int tmpItem;
	strSQL.Format("SELECT * FROM IN_AIRSIDE_CRITERIA_TMP_PARKING \
				  WHERE ParentID=%d",m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,tmpRecordset);
	while (!tmpRecordset.IsEOF())
	{
		tmpRecordset.GetFieldValue(_T("TmpParking"),tmpItem);
		m_vTmpParkingList.push_back(tmpItem);
		tmpRecordset.MoveNextData();
	}
	strSQL.Format("SELECT * FROM IN_AIRSIDE_CRITERIA_TMP_STAND \
				  WHERE ParentID=%d",m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,tmpRecordset);
	while (!tmpRecordset.IsEOF())
	{
		tmpRecordset.GetFieldValue(_T("TmpStand"),tmpItem);
		m_vTmpStandList.push_back(tmpItem);
		tmpRecordset.MoveNextData();
	}

	m_cirRouteStrategy.ReadData(m_nID);
}
void COccupiedAssignedStandStrategy::DeleteData() 
{

	CString strSQL = _T("");
	if(m_nID!=-1)
	{
		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_PRIORITY\
						 WHERE (ID = %d)"),m_nID);

		CADODatabase::ExecuteSQLStatement(strSQL);

		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_TMP_PARKING \
						 WHERE (ParentID=%d)"),m_nID);
		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_TMP_STAND \
						 WHERE (ParentID=%d)"),m_nID);
		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

		m_cirRouteStrategy.DeleteData();
	}

}
void COccupiedAssignedStandStrategy::SaveData(int nTimeWinID)
{
	CString strSQL = _T("");
	if(m_nID==-1)
	{
		strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_PRIORITY\
						 (ParentID,DelayTime,OccupyTime,strategyType,IntersectionID,Strategy0,Strategy1,Strategy2,Strategy3,Strategy4)\
						 VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)"),nTimeWinID,m_nDelaytime,m_iTime,m_cirRouteStrategy.GetStrategyType(),m_cirRouteStrategy.GetIntersectionID(),(int)m_vOrder[0],(int)m_vOrder[1],(int)m_vOrder[2],(int)m_vOrder[3],(int)m_vOrder[4]);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		for (int i=0;i<(int)m_vTmpParkingList.size();i++)
		{
			strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_TMP_PARKING\
							  (ParentID,TmpParking) \
							  VALUES (%d,%d)"),m_nID,m_vTmpParkingList[i]);
			CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		}
		for (int i=0;i<(int)m_vTmpStandList.size();i++)
		{
			strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_TMP_STAND\
							 (ParentID,TmpStand) \
							 VALUES (%d,%d)"),m_nID,m_vTmpStandList[i]);
			CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		}

	}else	
	{
		strSQL.Format(_T("UPDATE IN_AIRSIDE_CRITERIA_PRIORITY \
						 SET ParentID=%d,DelayTime=%d,OccupyTime = %d,strategyType = %d,IntersectionID=%d,Strategy0=%d,Strategy1=%d,Strategy2=%d,Strategy3=%d,Strategy4=%d \
						 WHERE (ID=%d)"),nTimeWinID,m_nDelaytime,m_iTime,m_cirRouteStrategy.GetStrategyType(),m_cirRouteStrategy.GetIntersectionID(),(int)m_vOrder[0],(int)m_vOrder[1],(int)m_vOrder[2],(int)m_vOrder[3],(int)m_vOrder[4],m_nID);
		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_TMP_PARKING \
						 WHERE (ParentID=%d)"),m_nID);
		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_TMP_STAND \
						 WHERE (ParentID=%d)"),m_nID);
		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		for (int i=0;i<(int)m_vTmpParkingList.size();i++)
		{
			strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_TMP_PARKING\
							 (ParentID,TmpParking) \
							 VALUES (%d,%d)"),m_nID,m_vTmpParkingList[i]);
							 CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		}
		for (int i=0;i<(int)m_vTmpStandList.size();i++)
		{
			strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_TMP_STAND\
							 (ParentID,TmpStand) \
							 VALUES (%d,%d)"),m_nID,m_vTmpStandList[i]);
							 CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		}

	}
	m_cirRouteStrategy.SaveDataToDB(m_nID);
}
void COccupiedAssignedStandStrategy::addTmpParking(ObjID tmpParking)
{
	m_vTmpParkingList.push_back(tmpParking);
}
void COccupiedAssignedStandStrategy::deleteTmpParking(int parkingIndex)
{
	if (parkingIndex<0||parkingIndex>=(int)m_vTmpParkingList.size())
	{
        return;
	}
	ObjIDList::iterator iter=m_vTmpParkingList.begin()+parkingIndex;
// 	m_vDelTmpParkingList.push_back(*iter);
    m_vTmpParkingList.erase(iter);
}
void COccupiedAssignedStandStrategy::addTmpStand(ObjID tmpStand)
{
	m_vTmpStandList.push_back(tmpStand);

}
void COccupiedAssignedStandStrategy::deleteTmpStand(int standIndex)
{
	if (standIndex<0||standIndex>=(int)m_vTmpStandList.size())
	{
		return;
	}
	ObjIDList::iterator iter=m_vTmpStandList.begin()+standIndex;
// 	m_vDelTmpStandList.push_back(*iter);
	m_vTmpStandList.erase(iter);
}
COccupiedAssignedStandStrategy& COccupiedAssignedStandStrategy::operator =(COccupiedAssignedStandStrategy &strategy)
{
	m_nDelaytime=strategy.GetDelaytime();
//	m_nIntersectionID=strategy.GetIntersectionID();
	SetTmpParkingList(strategy.GetTmpParkingList());
	SetTmpStandList(strategy.GetTmpStandList());
	m_vOrder.clear();
	m_vOrder.assign(strategy.GetOrder().begin(),strategy.GetOrder().end());
	return *this;
}

CirculateRoute* COccupiedAssignedStandStrategy::GetCirculateRoute()
{
	return m_cirRouteStrategy.GetCirculateRoute();
}

void COccupiedAssignedStandStrategy::SetStrategyType( int iType )
{
	m_cirRouteStrategy.SetStrategyType(iType);
}

int COccupiedAssignedStandStrategy::GetStrategyType() const
{
	return m_cirRouteStrategy.GetStrategyType();
}

void COccupiedAssignedStandStrategy::SetTimes( int iTime )
{
	m_iTime = iTime;
}

int COccupiedAssignedStandStrategy::GetTimes() const
{
	return m_iTime;
}

// void COccupiedAssignedStandStrategy::UpdateData()
// {
/*
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_OCCUPIEDASSIGNEDSTANDACTION_PRIORITY\
					 SET DELAYTIME =%d, PRIORITY =%d, STRATEGYTYPE =%d, INTERSECTIONID=%d, STANDID=%d, TEMPORARYPARKINGID=%d\
					 WHERE (ID = %d)"),m_nDelaytime,(int)m_enumPriority,(int)m_enumStrategyType, m_nIntersectionID, m_nStandID,m_nTemporaryParkingID,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
*/
// }

void CAirSideCreteriaTimeWin::CleanData()
{
// 	m_nID=-1;
// 	m_tStartTime.SetSecond(0);
// 	m_tEndTime.SetSecond(0);
// 	m_Strategy.CleanData();
}
void CAirSideCreteriaTimeWin::SaveData(int nFlightTypeID)
{
	CString strSQL;
	if(m_nID==-1)
	{
		strSQL.Format("INSERT INTO IN_AIRSIDE_CRITERIA_TIMEWIN \
					  (ParentID,EstStartTime,EstEndTime) \
					  VALUES (%d,%d,%d)",nFlightTypeID,m_tStartTime.asSeconds(),m_tEndTime.asSeconds());
		m_nID=CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		
	}else
	{
		strSQL.Format("UPDATE IN_AIRSIDE_CRITERIA_TIMEWIN \
					  SET EstStartTime=%d,EstEndTime=%d \
					  WHERE (ID=%d)",m_tStartTime.asSeconds(),m_tEndTime.asSeconds(),m_nID);
		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	m_Strategy.SaveData(m_nID);
}


CAirSideCreteriaTimeWin::CAirSideCreteriaTimeWin()
{
	m_nID=-1;
	m_tEndTime.SetHour(23);
	m_tEndTime.SetMinute(59);
	m_tEndTime.SetSecond(59);
}

CAirSideCreteriaTimeWin::~CAirSideCreteriaTimeWin()
{

}

void CAirSideCreteriaTimeWin::SetStartTime(ElapsedTime& tStart)
{	
	m_tStartTime = tStart;
}

ElapsedTime CAirSideCreteriaTimeWin::GetStartTime()
{	
	return m_tStartTime;
}

void CAirSideCreteriaTimeWin::SetEndTime(ElapsedTime& tEnd)
{
	m_tEndTime = tEnd;
}



ElapsedTime CAirSideCreteriaTimeWin::GetEndTime()
{
	return m_tEndTime;
}

COccupiedAssignedStandStrategy &CAirSideCreteriaTimeWin::GetStrategy()
{
	return m_Strategy;
}
void CAirSideCreteriaTimeWin::GetStrategy(COccupiedAssignedStandStrategy &strategy)
{
	strategy=m_Strategy;
}
bool CAirSideCreteriaTimeWin::inTimeWindow(ElapsedTime time)
{
	if (time>m_tStartTime&&time<m_tEndTime)
	{
		return true; 
	}else
	{
		return false;
	}
}
void CAirSideCreteriaTimeWin::ReadData(CADORecordset& adoRecordset)
{
	if (adoRecordset.IsEOF())
	{
		return;
	}
	CleanData();
	long nTime;
	adoRecordset.GetFieldValue("ID",m_nID);
	adoRecordset.GetFieldValue("EstStartTime",nTime);
	m_tStartTime=ElapsedTime(nTime);
	adoRecordset.GetFieldValue("EstEndTime",nTime);
	m_tEndTime=ElapsedTime(nTime);
	CString strSQL;
	strSQL.Format("SELECT * FROM IN_AIRSIDE_CRITERIA_PRIORITY \
				  WHERE ParentID=%d",m_nID);
	CADORecordset recordset;
	long nCount;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,recordset);
	if (!recordset.IsEOF())
	{
		m_Strategy.ReadData(recordset);
	}
}
void CAirSideCreteriaTimeWin::DeleteData()
{
	if (m_nID!=-1)
	{
		m_Strategy.DeleteData();
		CString strSQL;
		strSQL.Format("DELETE FROM IN_AIRSIDE_CRITERIA_TIMEWIN \
					  WHERE ID=%d",m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}
CAirSideCriteriaFlightType::~CAirSideCriteriaFlightType()
{
	CleanData();
}
CAirSideCriteriaFlightType::CAirSideCriteriaFlightType()
{
	m_nID=-1;
	addDefaultData();
};
FlightConstraint &CAirSideCriteriaFlightType::getFlightType()
{
	return m_FlightType;
}
void CAirSideCriteriaFlightType::setFlightType(const CString strFlightType)
{ 
	m_FlightType.setConstraintWithVersion(strFlightType);
}
void CAirSideCriteriaFlightType::setFlightType(const FlightConstraint &flightType)
{
	m_FlightType=flightType;
}
TimeWindowList &CAirSideCriteriaFlightType::getTimeWindowList()
{
	return m_vTimeWinList;
}

void CAirSideCriteriaFlightType::addTimeWindow(CAirSideCreteriaTimeWin *timeWindow)
{
	m_vTimeWinList.push_back(timeWindow);
}
void CAirSideCriteriaFlightType::delTimeWindow(int timeWinIndex)
{
	if (timeWinIndex<0||timeWinIndex>=(int)m_vTimeWinList.size())
	{
		return;
	}
	TimeWindowList::iterator iter=m_vTimeWinList.begin()+timeWinIndex;
	m_vDelTimeWinList.push_back(*iter);
	m_vTimeWinList.erase(iter);
	if ((int)m_vTimeWinList.size()==0)
	{
		addDefaultData();
	}
}
CAirSideCreteriaTimeWin *CAirSideCriteriaFlightType::getTimeWindow(int timeWinIndex)
{
	if (timeWinIndex<0||timeWinIndex>=(int)m_vTimeWinList.size())
	{
		return NULL;
	}
	TimeWindowList::iterator iter=m_vTimeWinList.begin()+timeWinIndex;
	return *iter;

}
bool CAirSideCriteriaFlightType::isDefaultFlightType()
{
	return (m_FlightType.isDefault()==0)?false:true;
}
void CAirSideCriteriaFlightType::SetAirportDB(CAirportDatabase* pAirportDB)
{
// 	m_pAirportDB = pAirportDB;
	m_FlightType.SetAirportDB(pAirportDB);
}

void CAirSideCriteriaFlightType::CleanData()
{
	TimeWindowList::iterator iter;
	for (iter=m_vTimeWinList.begin();iter!=m_vTimeWinList.end();iter++)
	{
		delete (*iter);
		*iter=NULL;
	}
	for (iter=m_vDelTimeWinList.begin();iter!=m_vDelTimeWinList.end();iter++)
	{
		delete (*iter);
		*iter=NULL;
	}
	m_vTimeWinList.clear();
	m_vDelTimeWinList.clear();
}
CAirSideCreteriaTimeWin *CAirSideCriteriaFlightType::addDefaultData()
{
	CAirSideCreteriaTimeWin *timeWin=new CAirSideCreteriaTimeWin();
	addTimeWindow(timeWin);
	return timeWin;
}
void CAirSideCriteriaFlightType::ReadData(CADORecordset& adoRecordset)
{	
	if (adoRecordset.IsEOF())
	{
		return;
	}
	CleanData();
	CString strFlightType;
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("FlightType"),strFlightType);
// 	CString strTmp;
// 	strTmp.Format(_T("%s"),strFlightType);
	m_FlightType.setConstraintWithVersion(strFlightType);

	CString strSQL;
	strSQL.Format(_T("SELECT * FROM IN_AIRSIDE_CRITERIA_TIMEWIN \
					 WHERE ParentID=%d"),m_nID);
	CADORecordset recordset;
	long nCount;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,recordset);
	CAirSideCreteriaTimeWin* timeWindow;
	if (recordset.IsEOF())
	{
		addDefaultData();
		return;
	}
	while(!recordset.IsEOF())
	{
		timeWindow=new CAirSideCreteriaTimeWin();
		timeWindow->ReadData(recordset);
		m_vTimeWinList.push_back(timeWindow);
		recordset.MoveNextData();
	}
	
}

void CAirSideCriteriaFlightType::DeleteData()
{
	if (m_nID!=-1)
	{
		TimeWindowList::iterator iter;
		for (iter=m_vTimeWinList.begin();iter!=m_vTimeWinList.end();iter++)
		{
			((CAirSideCreteriaTimeWin*)(*iter))->DeleteData();
		}
		for (iter=m_vDelTimeWinList.begin();iter!=m_vDelTimeWinList.end();iter++)
		{
			((CAirSideCreteriaTimeWin*)(*iter))->DeleteData();
		}
		CString strSQL;
		strSQL.Format("DELETE FROM IN_AIRSIDE_CRITERIA_FLIGHTTYPE \
					  WHERE ID=%d",m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CAirSideCriteriaFlightType::SaveData(int nStandsID)
{
	CString strSQL/*,strFlightType*/;
	char strFlightType[1024];
	m_FlightType.WriteSyntaxStringWithVersion(strFlightType);
	
	if (m_nID==-1)
	{
		strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_FLIGHTTYPE \
						 (ParentID,FlightType) VALUES (%d,'%s')"),nStandsID,strFlightType);
		m_nID=CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		
	}else
	{
		strSQL.Format(_T("UPDATE IN_AIRSIDE_CRITERIA_FLIGHTTYPE \
						 SET FlightType='%s' \
						 WHERE ID=%d"),strFlightType,m_nID);
		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	TimeWindowList::iterator iter;
	for (iter=m_vTimeWinList.begin();iter!=m_vTimeWinList.end();iter++)
	{
		((CAirSideCreteriaTimeWin*)(*iter))->SaveData(m_nID);
	}
	for (iter=m_vDelTimeWinList.begin();iter!=m_vDelTimeWinList.end();iter++)
	{
		((CAirSideCreteriaTimeWin*)(*iter))->DeleteData();
		delete (*iter);
	}
	m_vDelTimeWinList.clear();
}
CAirSideCriteriaParkingStands::~CAirSideCriteriaParkingStands()
{
	CleanData();
}
CAirSideCriteriaParkingStands::CAirSideCriteriaParkingStands()
{
	m_nID=-1;
	addDefaultData();
}
ObjIDList &CAirSideCriteriaParkingStands::getParkingStandList()
{
	return m_vParkingStandList;
}
FlightTypeList &CAirSideCriteriaParkingStands::getFlightTypeList()
{
	return m_vFlightTypeList;
}

void CAirSideCriteriaParkingStands::addParkingStand(ObjID tmpParkingStand)
{
	m_vParkingStandList.push_back(tmpParkingStand);
}
void CAirSideCriteriaParkingStands::delParkingStand(int parkingStandIndex)
{
	if (parkingStandIndex<0||parkingStandIndex>=(int)m_vParkingStandList.size())
	{
		return;
	}
	m_vParkingStandList.erase(m_vParkingStandList.begin()+parkingStandIndex);
}
void CAirSideCriteriaParkingStands::setParkingStandList(const ObjIDList &vParkingStandList)
{
	m_vParkingStandList.clear();
	m_vParkingStandList.assign(vParkingStandList.begin(),vParkingStandList.end());
}
bool CAirSideCriteriaParkingStands::isAllParkingStand()
{
	return ((int)m_vParkingStandList.size()==0)?true:false;
}
void CAirSideCriteriaParkingStands::addFlightType(CAirSideCriteriaFlightType* flightType)
{
	flightType->SetAirportDB(m_pAirportDB);
	m_vFlightTypeList.push_back(flightType);
}
void CAirSideCriteriaParkingStands::delFlightType(int flightTypeIndex)
{
	if (flightTypeIndex<0||flightTypeIndex>=(int)m_vFlightTypeList.size())
	{
		return;
	}
	FlightTypeList::iterator iter=m_vFlightTypeList.begin()+flightTypeIndex;
	m_vDelFlightTypeList.push_back(*iter);
	m_vFlightTypeList.erase(iter);
	if ((int)m_vFlightTypeList.size()==0)
	{
		addDefaultData();
	}
}
CAirSideCriteriaFlightType *CAirSideCriteriaParkingStands::getFlightType(int flightTypeIndex)
{
	if (flightTypeIndex<0||flightTypeIndex>=(int)m_vFlightTypeList.size())
	{
		return NULL;
	}
	FlightTypeList::iterator iter=m_vFlightTypeList.begin()+flightTypeIndex;
	return *iter;
}
void CAirSideCriteriaParkingStands::SetAirportDB(CAirportDatabase* pAirportDB)
{
	m_pAirportDB = pAirportDB;
}
CAirSideCriteriaFlightType *CAirSideCriteriaParkingStands::addDefaultData()
{
	CAirSideCriteriaFlightType *flightType=new CAirSideCriteriaFlightType();
	addFlightType(flightType);
	return flightType;
}
int CAirSideCriteriaParkingStands::standMatch(ALTObjectID standID1,ALTObjectID standID2)
{
	//return two ALTObjectID
	int matchValue=0;
// 	bool matched=true;
	CString str1,str2;
	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		str1=standID1.m_val[nLevel].c_str();
		str2=standID2.m_val[nLevel].c_str();
		if ((str1.CompareNoCase(_T(""))==0)||str2.CompareNoCase(_T(""))==0)
		{
			break;
		}
		if(str1.CompareNoCase(str2)!=0)
		{
			matchValue=0;
			break;
		}
		matchValue++;
	}
	return matchValue;	
}
int CAirSideCriteriaParkingStands::getStandMatch(ALTObjectID standID)
{
	ALTObjectGroup tmpStand;
	int matchValue,maxMatchValue=0;
	for (int i=0;i<(int)m_vParkingStandList.size();i++)
	{		
		tmpStand.ReadData((int)m_vParkingStandList.at(i));		
		matchValue=standMatch(tmpStand.getName(),standID);
		if (matchValue>maxMatchValue)
		{
			maxMatchValue=matchValue;
		}
	}
	return maxMatchValue;
}
void CAirSideCriteriaParkingStands::ReadData(CADORecordset& adoRecordset)
{
	if (adoRecordset.IsEOF())
	{
		return;
	}
	CleanData();
	adoRecordset.GetFieldValue("ID",m_nID);
	CString strSQL;
	long nCount;
	CADORecordset recordset;
	strSQL.Format(_T("SELECT * FROM IN_AIRSIDE_CRITERIA_STANDLIST \
					 WHERE ParentID=%d"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,recordset);
	ObjID stand;
	while(!recordset.IsEOF())
	{
		recordset.GetFieldValue("Stand",stand);
		m_vParkingStandList.push_back(stand);
		recordset.MoveNextData();
	}
	strSQL.Format(_T("SELECT * FROM IN_AIRSIDE_CRITERIA_FLIGHTTYPE \
					 WHERE ParentID=%d"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,recordset);
	CAirSideCriteriaFlightType* flightType;
	if (recordset.IsEOF())
	{
		addDefaultData();
		return;
	}
	while(!recordset.IsEOF())
	{
        flightType=new CAirSideCriteriaFlightType();
		addFlightType(flightType);
		flightType->ReadData(recordset);		
		recordset.MoveNextData();
	}
}

void CAirSideCriteriaParkingStands::DeleteData()
{

	if (m_nID!=-1)
	{
		FlightTypeList::iterator iter;
		for (iter=m_vFlightTypeList.begin();iter!=m_vFlightTypeList.end();iter++)
		{
			((CAirSideCriteriaFlightType*)(*iter))->DeleteData();
		}
		for (iter=m_vDelFlightTypeList.begin();iter!=m_vDelFlightTypeList.end();iter++)
		{
			((CAirSideCriteriaFlightType*)(*iter))->DeleteData();
		}
		CString strSQL;
		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_STANDLIST \
						 WHERE ParentID=%d"),m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);

		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_PARKINGSTANDS \
						 WHERE ID=%d"),m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);

		
	}
}

void CAirSideCriteriaParkingStands::SaveData(int nExitsID)
{
	CString strSQL;
	if (m_nID==-1)
	{
		strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_PARKINGSTANDS \
						 (ParentID) VALUES (%d)"),nExitsID);	
		m_nID=CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	{
		FlightTypeList::iterator iter;
		for (iter=m_vFlightTypeList.begin();iter!=m_vFlightTypeList.end();iter++)
		{
			((CAirSideCriteriaFlightType*)(*iter))->SaveData(m_nID);
		}
		for (iter=m_vDelFlightTypeList.begin();iter!=m_vDelFlightTypeList.end();iter++)
		{
			((CAirSideCriteriaFlightType*)(*iter))->DeleteData();
			delete (*iter);
		}
		m_vDelFlightTypeList.clear();
	}

	{
		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_STANDLIST \
					 WHERE ParentID=%d"),m_nID);	
		CADODatabase::ExecuteSQLStatement(strSQL);	
		ObjIDList::iterator iter;
		for (iter=m_vParkingStandList.begin();iter!=m_vParkingStandList.end();iter++)
		{
			strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_STANDLIST \
							 (ParentID,Stand) VALUES (%d,%d)"),m_nID,(int)(*iter));
			CADODatabase::ExecuteSQLStatement(strSQL);
		}
	}
}

void CAirSideCriteriaParkingStands::CleanData()
{
	FlightTypeList::iterator iter;
	for (iter=m_vFlightTypeList.begin();iter!=m_vFlightTypeList.end();iter++)
	{
		delete *iter;
		*iter=NULL;
	}
	for (iter=m_vDelFlightTypeList.begin();iter!=m_vDelFlightTypeList.end();iter++)
	{
		delete *iter;
		*iter=NULL;
	}

	m_vFlightTypeList.clear();
	m_vDelFlightTypeList.clear();
	m_vParkingStandList.clear();
// 	m_vDelParkingStandList.clear();

}
CAirSideCriteriaExits::CAirSideCriteriaExits()
{
	m_nID=-1;
	addDefaultData();
}
CAirSideCriteriaExits::~CAirSideCriteriaExits()
{
	CleanData();
}
ObjIDList &CAirSideCriteriaExits::getExitList()
{
	return m_vExitList;
}
ParkingStandsList &CAirSideCriteriaExits::getParkingStandsList()
{
	return m_vParkingStandsList;
}
void CAirSideCriteriaExits::addExit(ObjID exit)
{
	m_vExitList.push_back(exit);
}
void CAirSideCriteriaExits::delExit(int exitIndex)
{
	if (exitIndex<0||exitIndex>=(int)m_vExitList.size())
	{
		return;
	}
	m_vExitList.erase(m_vExitList.begin()+exitIndex);
}
void CAirSideCriteriaExits::setExitList(const ObjIDList &vExitList)
{
	m_vExitList.clear();
	m_vExitList.assign(vExitList.begin(),vExitList.end());
}
// void CAirSideCriteriaExits::setAllExit()
// {
// 	m_vExitList.clear();
// }
void CAirSideCriteriaExits::addParkingStands(CAirSideCriteriaParkingStands* parkingStands)
{
	parkingStands->SetAirportDB(m_pAirportDB);
	m_vParkingStandsList.push_back(parkingStands);
}
void CAirSideCriteriaExits::delParkingStands(int parkingStandsIndex)
{
	if (parkingStandsIndex<0||parkingStandsIndex>=(int)m_vParkingStandsList.size())
	{
		return;
	}
	ParkingStandsList::iterator iter=m_vParkingStandsList.begin()+parkingStandsIndex;
	m_vDelParkingStandsList.push_back(*iter);
	m_vParkingStandsList.erase(iter);
	if ((int)m_vParkingStandsList.size()==0)
	{
		addDefaultData();
	}
}
CAirSideCriteriaParkingStands *CAirSideCriteriaExits::getParkingStands(int parkingStandsIndex)
{
	if (parkingStandsIndex<0||parkingStandsIndex>=(int)m_vParkingStandsList.size())
	{
		return NULL;
	}
	ParkingStandsList::iterator iter=m_vParkingStandsList.begin()+parkingStandsIndex;
	return *iter;
}
void CAirSideCriteriaExits::SetAirportDB(CAirportDatabase* pAirportDB)
{
	m_pAirportDB = pAirportDB;
}
bool CAirSideCriteriaExits::IsAllRunwayExit()
{
	return ((int)m_vExitList.size()==0)?true:false;
}
CAirSideCriteriaParkingStands *CAirSideCriteriaExits::addDefaultData()
{
	CAirSideCriteriaParkingStands *stands=new CAirSideCriteriaParkingStands();
	addParkingStands(stands);
	return stands;
}
void CAirSideCriteriaExits::ReadData(CADORecordset& adoRecordset)
{
	if(adoRecordset.IsEOF())
	{
		return;
	}
	CleanData();
	adoRecordset.GetFieldValue("ID",m_nID);
    CString strSQL;
	long nCount;
	CADORecordset recordset;
	strSQL.Format(_T("SELECT * FROM IN_AIRSIDE_CRITERIA_EXITLIST \
					 WHERE ParentID=%d"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,recordset);
	ObjID exit;
	while(!recordset.IsEOF())
	{
		recordset.GetFieldValue("Exist",exit);
		m_vExitList.push_back(exit);
		recordset.MoveNextData();
	}

	strSQL.Format(_T("SELECT * FROM IN_AIRSIDE_CRITERIA_PARKINGSTANDS \
					 WHERE ParentID=%d"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,recordset);

	if (recordset.IsEOF())
	{
		addDefaultData();
		return;
	}
	while(!recordset.IsEOF())
	{
		CAirSideCriteriaParkingStands *stands=new CAirSideCriteriaParkingStands();		
		addParkingStands(stands);
		stands->ReadData(recordset);
		recordset.MoveNextData();
	}
}

void CAirSideCriteriaExits::DeleteData()
{
	if (m_nID!=-1)
	{
		ParkingStandsList::iterator iter;
		for (iter=m_vParkingStandsList.begin();iter!=m_vParkingStandsList.end();iter++)
		{
			((CAirSideCriteriaParkingStands*)(*iter))->DeleteData();
		}
		for (iter=m_vDelParkingStandsList.begin();iter!=m_vDelParkingStandsList.end();iter++)
		{
			((CAirSideCriteriaParkingStands*)(*iter))->DeleteData();
		}
		CString strSQL;
		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_EXITLIST \
						 WHERE ParentID=%d"),m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_EXITS \
						 WHERE ID=%d"),m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}


}

void CAirSideCriteriaExits::SaveData(int projectID,int parentID)
{
	CString strSQL;
	if (m_nID==-1)
	{
        strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_EXITS \
					  (ProjectID,ParentID) VALUES (%d,%d)"),projectID,parentID);
		m_nID=CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	{
		ParkingStandsList::iterator iter;
		for (iter=m_vParkingStandsList.begin();iter!=m_vParkingStandsList.end();iter++)
		{
			((CAirSideCriteriaParkingStands*)(*iter))->SaveData(m_nID);
		}
		for (iter=m_vDelParkingStandsList.begin();iter!=m_vDelParkingStandsList.end();iter++)
		{
			((CAirSideCriteriaParkingStands*)(*iter))->DeleteData();
			delete (*iter);
		}
		m_vDelParkingStandsList.clear();
	}
	
	{
		strSQL.Format(_T("DELETE FROM IN_AIRSIDE_CRITERIA_EXITLIST \
						 WHERE ParentID=%d"),m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);

		ObjIDList::iterator iter;
		for (iter=m_vExitList.begin();iter!=m_vExitList.end();iter++)
		{
			strSQL.Format(_T("INSERT INTO IN_AIRSIDE_CRITERIA_EXITLIST \
							 (ParentID,Exist) VALUES (%d,%d)"),m_nID,(int)(*iter));
			CADODatabase::ExecuteSQLStatement(strSQL);
		}
	}

}

void CAirSideCriteriaExits::CleanData()
{
	ParkingStandsList::iterator iter;
	for (iter=m_vParkingStandsList.begin();iter!=m_vParkingStandsList.end();iter++)
	{
		delete *iter;
		*iter=NULL;
	}
	for (iter=m_vDelParkingStandsList.begin();iter!=m_vDelParkingStandsList.end();iter++)
	{
		delete *iter;
		*iter=NULL;
	}

	m_vParkingStandsList.clear();
	m_vDelParkingStandsList.clear();
	m_vExitList.clear();
}

COccupiedAssignedStandCriteria::COccupiedAssignedStandCriteria()
{
	addDefaultData();
	m_nPrjID=-1;
}
COccupiedAssignedStandCriteria::~COccupiedAssignedStandCriteria()
{
	CleanData();
}
ExitsList &COccupiedAssignedStandCriteria::getExitsList()
{
	return m_vExitsList;
}
void COccupiedAssignedStandCriteria::addExits(CAirSideCriteriaExits* exits)
{
	exits->SetAirportDB(m_pAirportDB);
	m_vExitsList.push_back(exits);
}
void COccupiedAssignedStandCriteria::delExits(int exitsIndex)
{
	if (exitsIndex<0||exitsIndex>=(int)m_vExitsList.size())
	{
		return;
	}
	ExitsList::iterator iter=m_vExitsList.begin()+exitsIndex;
	m_vDelExitsList.push_back(*iter);
	m_vExitsList.erase(iter);
	if ((int)m_vExitsList.size()==0)
	{
		addDefaultData();
	}
}
CAirSideCriteriaExits *COccupiedAssignedStandCriteria::getExits(int exitsIndex)
{
	if (exitsIndex<0||exitsIndex>=(int)m_vExitsList.size())
	{
		return NULL;
	}
	ExitsList::iterator iter=m_vExitsList.begin()+exitsIndex;
	return *iter;
}
void COccupiedAssignedStandCriteria::SaveData()
{
	ExitsList::iterator iter;
	for (iter=m_vExitsList.begin();iter!=m_vExitsList.end();iter++)
	{
		((CAirSideCriteriaExits*)(*iter))->SaveData(m_nPrjID);
	}
	for (iter=m_vDelExitsList.begin();iter!=m_vDelExitsList.end();iter++)
	{
		((CAirSideCriteriaExits*)(*iter))->DeleteData();
		delete *iter;
	}
	m_vDelExitsList.clear();
}
void COccupiedAssignedStandCriteria::addDefaultData()
{
	CAirSideCriteriaExits *exits=new CAirSideCriteriaExits();
	addExits(exits);
}
void COccupiedAssignedStandCriteria::ReadData()
{
	CleanData();
	CString strSQL;
	long nCount;
	CADORecordset recordset;
	strSQL.Format(_T("SELECT * FROM IN_AIRSIDE_CRITERIA_EXITS \
					 WHERE ProjectID=%d"),m_nPrjID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,recordset);
	CAirSideCriteriaExits* exits;
	if (recordset.IsEOF())
	{
		//insert default data
		addDefaultData();
		return;
	}
	while(!recordset.IsEOF())
	{
		exits=new CAirSideCriteriaExits();
		addExits(exits);
		exits->ReadData(recordset);		
		recordset.MoveNextData();
	}
}
void COccupiedAssignedStandCriteria::CleanData()
{
	ExitsList::iterator iter;
	for (iter=m_vExitsList.begin();iter!=m_vExitsList.end();iter++)
	{
		delete *iter;
		*iter=NULL;
	}
	for (iter=m_vDelExitsList.begin();iter!=m_vDelExitsList.end();iter++)
	{
		delete *iter;
		*iter=NULL;
	}
	m_vExitsList.clear();
	m_vDelExitsList.clear();
}
void COccupiedAssignedStandCriteria::SetAirportDB(CAirportDatabase* pAirportDB)
{
	m_pAirportDB = pAirportDB;
}
void COccupiedAssignedStandCriteria::SetPrjID(int prjID)
{
	m_nPrjID=prjID;
}
/*
COccupiedAssignedStandStrategy COccupiedAssignedStandCriteria::GetStrategyByFlightType(AirsideFlightInSim* pFlight)
{
	//find the max matched strategy for pFlight
	//if not find,return default one.
	int maxMatch,curMatch;
	int curLevel;
	const int levelNum=4;
	int levelMatchValue[levelNum];
	COccupiedAssignedStandStrategy _strategy;
	maxMatch=curMatch=0;	
	ExitsList::iterator exitsIter;
	for (exitsIter=m_vExitsList.begin();exitsIter!=m_vExitsList.end();exitsIter++)
	{
		curLevel=0;
		levelMatchValue[curLevel]=0;
		CAirSideCriteriaExits *curExits=(CAirSideCriteriaExits *)(*exitsIter);	
		if (!curExits->IsAllRunwayExit())		
		{            
			bool bInExitList=false;
			ObjIDList exitList=curExits->getExitList();
			for (int i=0;i<(int)exitList.size();i++)
			{
				pFlight->GetRunwayExit()->GetID()==(int)exitList.at(i);
				bInExitList=true;
				break;
			}
			if (bInExitList==false)
			{
				continue;
			}
			levelMatchValue[curLevel]=1;
		}

		
		ParkingStandsList::iterator parkingStandIter;
		ParkingStandsList parkingStandList=curExits->getParkingStandsList();
		for (parkingStandIter=parkingStandList.begin();parkingStandIter!=parkingStandList.end();parkingStandIter++)
		{
			curLevel=1;
			levelMatchValue[curLevel]=0;
			CAirSideCriteriaParkingStands *curParkingStand=(CAirSideCriteriaParkingStands *)(*parkingStandIter);
			if (!curParkingStand->isAllParkingStand())
			{				
				int matchValue=curParkingStand->getStandMatch(pFlight->getStand()); 
				if (matchValue==0)
				{
					continue;
				}else
				{
					levelMatchValue[curLevel]=matchValue;
				}
			}

			FlightTypeList flightTypeList=curParkingStand->getFlightTypeList();
			FlightTypeList::iterator flightTypeIter;
			for (flightTypeIter=flightTypeList.begin();flightTypeIter!=flightTypeList.end();flightTypeIter++)
			{
				curLevel=2;
				levelMatchValue[curLevel]=0;
				CAirSideCriteriaFlightType *curFlightType=(CAirSideCriteriaFlightType *)(*flightTypeIter);
				if (!curFlightType->isDefaultFlightType())
				{
					if (curFlightType->getFlightType().fits(pFlight))
					{
						levelMatchValue[curLevel]=1;
					}
				}

				TimeWindowList timeWindowList=curFlightType->getTimeWindowList();
				TimeWindowList::iterator timeWindowIter;
				for(timeWindowIter=timeWindowList.begin();timeWindowIter!=timeWindowList.end();timeWindowIter++)
				{
					curLevel=3;
					levelMatchValue[curLevel]=0;
					CAirSideCreteriaTimeWin *curTimeWin=(CAirSideCreteriaTimeWin *)(*timeWindowIter);
					if (curTimeWin->inTimeWindow(pFlight->getCurTime()))
					{
						levelMatchValue[curLevel]=1;
                        curMatch=calculateMatchValue(levelMatchValue,levelNum);
						if (curMatch>maxMatch)
						{
							maxMatch=curMatch;
							curTimeWin->GetStrategy(_strategy);
						}
					}

				}
			}



		}

	}

	return _strategy;

	
}*/




//////--c--///////-----------should be deleted---------------------

//--------------------COccupiedAssignedStandFltType----------------------

/*
COccupiedAssignedStandFltType::COccupiedAssignedStandFltType()
{
	m_nID = -1;
}

COccupiedAssignedStandFltType::~COccupiedAssignedStandFltType()
{
	RemoveAll();
}

void COccupiedAssignedStandFltType::ReadData(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	CString strFltType;
	char szFltType[1024] = {0};
	adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);
	strcpy(szFltType,strFltType);
	m_fltType.SetAirportDB(m_pAirportDB);
	m_fltType.setConstraintWithVersion(szFltType);
	ReadPriorityData();
}
void COccupiedAssignedStandFltType::ReadPriorityData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT *\
					 FROM IN_OCCUPIEDASSIGNEDSTANDACTION_PRIORITY\
					 WHERE (FLTID = %d)"),m_nID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		COccupiedAssignedStandPriority* pPriority = new COccupiedAssignedStandPriority;
		pPriority->ReadData(adoRecordset);

		m_vPriority.push_back(pPriority);
		adoRecordset.MoveNextData();
	}
}
void COccupiedAssignedStandFltType::SaveData(int nStandActionID)
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_OCCUPIEDASSIGNEDSTANDACTION_FLT\
					 (PROJID, FLTTYPE)\
					 VALUES (%d,'%s')"),nStandActionID,szFltType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	OccupiedAssignedStandPriorityIter iter = m_vPriority.begin();
	for (;iter != m_vPriority.end();++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelPriority.begin();iter !=m_vNeedDelPriority.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelPriority.clear();
}
void COccupiedAssignedStandFltType::ImportSaveData(int nStandActionID,CString strFltType)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_OCCUPIEDASSIGNEDSTANDACTION_FLT\
					 (PROJID, FLTTYPE)\
					 VALUES (%d,'%s')"),nStandActionID,strFltType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	OccupiedAssignedStandPriorityIter iter = m_vPriority.begin();
	for (;iter != m_vPriority.end();++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelPriority.begin();iter !=m_vNeedDelPriority.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelPriority.clear();
}
void COccupiedAssignedStandFltType::UpdateData()
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_OCCUPIEDASSIGNEDSTANDACTION_FLT\
					 SET FLTTYPE ='%s'\
					 WHERE (ID = %d)"),szFltType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	OccupiedAssignedStandPriorityIter iter = m_vPriority.begin();
	for (;iter != m_vPriority.end();++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData();
	}

	for (iter = m_vNeedDelPriority.begin();iter != m_vNeedDelPriority.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vNeedDelPriority.clear();
}
int COccupiedAssignedStandFltType::GetPriorityCount()
{
	return (int)m_vPriority.size();
}

COccupiedAssignedStandPriority* COccupiedAssignedStandFltType::GetPriorityItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vPriority.size());
	return m_vPriority[nIndex];
}

void COccupiedAssignedStandFltType::AddPriorityItem(COccupiedAssignedStandPriority* pItem)
{
	m_vPriority.push_back(pItem);
}
void COccupiedAssignedStandFltType::DelPriorityItem(COccupiedAssignedStandPriority* pItem)
{
	OccupiedAssignedStandPriorityIter iter = 
		std::find(m_vPriority.begin(),m_vPriority.end(), pItem);
	if (iter == m_vPriority.end())
		return;
	m_vNeedDelPriority.push_back(pItem);
	m_vPriority.erase(iter);
}
void COccupiedAssignedStandFltType::RemoveAll()
{
	OccupiedAssignedStandPriorityIter iter = m_vPriority.begin();
	for (; iter != m_vPriority.end(); iter++)
		delete *iter;

	for (iter = m_vNeedDelPriority.begin();iter != m_vNeedDelPriority.end();iter++)
		delete *iter;
}

void COccupiedAssignedStandFltType::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_OCCUPIEDASSIGNEDSTANDACTION_FLT\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	OccupiedAssignedStandPriorityIter iter = m_vPriority.begin();
	for (;iter != m_vPriority.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}

	for (iter = m_vNeedDelPriority.begin();iter != m_vNeedDelPriority.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
}
void COccupiedAssignedStandFltType::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);

	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strFltType;
	strFltType = szFltType;
	exportFile.getFile().writeField(strFltType);

	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vPriority.size()));
	OccupiedAssignedStandPriorityIter iter = m_vPriority.begin();
	for (; iter!=m_vPriority.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}

void COccupiedAssignedStandFltType::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	CString strFltType;
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

	ImportSaveData(importFile.getNewProjectID(),strFltType);

	importFile.getFile().getLine();

	int nPriorityCount = 0;
	importFile.getFile().getInteger(nPriorityCount);
	for (int i =0; i < nPriorityCount; ++i)
	{
		COccupiedAssignedStandPriority data;
		data.ImportData(importFile,m_nID);
	}
}

//------------COccupiedAssignedStandAction-----------------
COccupiedAssignedStandAction::COccupiedAssignedStandAction()
{
}

COccupiedAssignedStandAction::~COccupiedAssignedStandAction()
{
	RemoveAll();
}

bool FlightTypeCompare(COccupiedAssignedStandFltType* fItem,COccupiedAssignedStandFltType* sItem)
{
	if (fItem->GetFltType() .fits(sItem->GetFltType()))
		return false;

	return true;
}

void COccupiedAssignedStandAction::ReadData(int nProjID)
{
	CString strSQL;
	strSQL.Format(_T("SELECT ID, FLTTYPE\
					 FROM IN_OCCUPIEDASSIGNEDSTANDACTION_FLT\
					 WHERE (PROJID = %d)"),nProjID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		COccupiedAssignedStandFltType* pFltType = new COccupiedAssignedStandFltType;
		pFltType->SetAirportDB(m_pAirportDB);
		pFltType->ReadData(adoRecordset);

		m_vFltType.push_back(pFltType);
		adoRecordset.MoveNextData();
	}

	std::sort(m_vFltType.begin(),m_vFltType.end(),FlightTypeCompare);
}

void COccupiedAssignedStandAction::SaveData(int nProjID)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		OccupiedAssignedStandFltTypeIter iter  = m_vFltType.begin();
		for (;iter != m_vFltType.end();++iter)
		{
			if(-1 == (*iter)->GetID())
				(*iter)->SaveData(nProjID);
			else
				(*iter)->UpdateData();
		}
		
		for (iter = m_vNeedDelFltType.begin();iter != m_vNeedDelFltType.end();++iter)
		{
			(*iter)->DeleteData();
	//		delete *iter;
		}
		m_vNeedDelFltType.clear();
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}

int COccupiedAssignedStandAction::GetFltTypeCount()
{
	return (int)m_vFltType.size();
}

COccupiedAssignedStandFltType* COccupiedAssignedStandAction::GetFltTypeItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vFltType.size());
	return m_vFltType[nIndex];
}

void COccupiedAssignedStandAction::AddFltTypeItem(COccupiedAssignedStandFltType* pItem)
{
	m_vFltType.push_back(pItem);
}
void COccupiedAssignedStandAction::DelFltTypeItem(COccupiedAssignedStandFltType* pItem)
{
	OccupiedAssignedStandFltTypeIter iter = 
		std::find(m_vFltType.begin(),m_vFltType.end(), pItem);
	if (iter == m_vFltType.end())
		return;
	m_vNeedDelFltType.push_back(pItem);
	m_vFltType.erase(iter);
}
void COccupiedAssignedStandAction::RemoveAll()
{
	OccupiedAssignedStandFltTypeIter iter = m_vFltType.begin();
	for (; iter != m_vFltType.end(); iter++)
		delete *iter;

	for (iter = m_vNeedDelFltType.begin();iter != m_vNeedDelFltType.end();iter++)
		delete *iter;
}
void COccupiedAssignedStandAction::ExportOccupiedAssignedStandAction(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
	COccupiedAssignedStandAction occupiedAssignedStandAction;
	occupiedAssignedStandAction.SetAirportDB(pAirportDB);
	occupiedAssignedStandAction.ReadData(exportFile.GetProjectID());
	occupiedAssignedStandAction.ExportData(exportFile);
	exportFile.getFile().endFile();
}
void COccupiedAssignedStandAction::ExportData(CAirsideExportFile& exportFile)
{		
	exportFile.getFile().writeField("OccupiedAssignedStandAction");
	exportFile.getFile().writeLine();
	OccupiedAssignedStandFltTypeIter iter = m_vFltType.begin();
	for (; iter!=m_vFltType.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}
void COccupiedAssignedStandAction::ImportOccupiedAssignedStandAction(CAirsideImportFile& importFile)
{
	while(!importFile.getFile().isBlank())
	{
		COccupiedAssignedStandFltType occupiedAssignedStandFltType;
		occupiedAssignedStandFltType.ImportData(importFile);
	}
}*/


