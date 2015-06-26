#include "StdAfx.h"
#include "ArrivalDelayTriggers.h"
#include "../Database/ADODatabase.h"
#include "../Database/DBElementCollection_Impl.h"
//////////////////////////////////////////////////////////////////////////
// class CArrivalDelayTrigger

//CArrivalDelayTrigger::CArrivalDelayTrigger()
//: m_nID(-1)
// ,m_nArriDelayTriListID(-1)
// ,m_waitingTime(0L)
// ,m_lQueueLength(0)
// ,m_nRunwayIndex(0)
//{
//}
//
//CArrivalDelayTrigger::~CArrivalDelayTrigger()
//{
//}
//
//void CArrivalDelayTrigger::setArriDelayTriListID(int nArriDelayTriListID)
//{
//	m_nArriDelayTriListID = nArriDelayTriListID; 
//}
//int	 CArrivalDelayTrigger::getArriDelayTriListID()
//{
//	return m_nArriDelayTriListID; 
//}
//
//void CArrivalDelayTrigger::setRunway(Runway runway,int nRunwayIndex)
//{
//	m_Runway = runway;
//	m_nRunwayIndex = nRunwayIndex;
//}
//
//Runway* CArrivalDelayTrigger::getRunway()
//{ 
//	return &m_Runway;
//}
//
//void CArrivalDelayTrigger::setWaitingTime(long waitingTime)
//{
//	m_waitingTime = waitingTime; 
//}
//long CArrivalDelayTrigger::getWaitingTime() const 
//{
//	return m_waitingTime; 
//}
//
//void CArrivalDelayTrigger::setQueueLength(long lQueueLength)
//{
//	m_lQueueLength = lQueueLength; 
//}
//
//long CArrivalDelayTrigger::getQueueLength() const
//{
//	return m_lQueueLength; 
//}
//
//void CArrivalDelayTrigger::ReadData(CADORecordset& adoRecordset)
//{
//	adoRecordset.GetFieldValue(_T("ID"), m_nID);
//	adoRecordset.GetFieldValue(_T("FLTTYPEID"),m_nArriDelayTriListID);
//	int nRunwayID = -1;
//	adoRecordset.GetFieldValue(_T("RUNWAYID"),nRunwayID);
//	m_Runway.setID(nRunwayID);
//
//	adoRecordset.GetFieldValue(_T("RWMARKINDEX"),m_nRunwayIndex);
//	long nWaitingSec = 0;
//	adoRecordset.GetFieldValue(_T("WAITTIME"),nWaitingSec);
//	m_waitingTime = nWaitingSec;
//
//	adoRecordset.GetFieldValue(_T("QUEUELENGTH"),m_lQueueLength);
//
//}
//void CArrivalDelayTrigger::UpdateData()
//{
//	CString strSQL = _T("");
//	strSQL.Format(_T("UPDATE IN_RUNWAY_ARRDELAYTRIGGERDATA\
//		SET FLTTYPEID =%d, RUNWAYID =%d, RWMARKINDEX =%d, WAITTIME =%d, \
//		QUEUELENGTH =%d\
//		WHERE (ID = %d)"),
//		m_nArriDelayTriListID,m_Runway.getID(),m_nRunwayIndex,m_waitingTime,m_lQueueLength,m_nID);
//
//	CADODatabase::ExecuteSQLStatement(strSQL);
//
//}
//void CArrivalDelayTrigger::SaveData( int nFltTypeID)
//{
//
//	if (m_nID != -1)
//	{
//		return UpdateData();
//	}
//	
//	m_nArriDelayTriListID = nFltTypeID;
//
//	CString strSQL = _T("");
//	strSQL.Format(_T("INSERT INTO IN_RUNWAY_ARRDELAYTRIGGERDATA\
//		(FLTTYPEID, RUNWAYID, RWMARKINDEX, WAITTIME, QUEUELENGTH)\
//		VALUES (%d,%d,%d,%d,%d)"),
//		m_nArriDelayTriListID,m_Runway.getID(),m_nRunwayIndex,m_waitingTime,m_lQueueLength);
//
//	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
//
//}
//void CArrivalDelayTrigger::DeleteData()
//{
//	if (m_nID == -1)
//		return;
//
//
//	CString strSQL = _T("");
//	strSQL.Format(_T("DELETE FROM IN_RUNWAY_ARRDELAYTRIGGERDATA\
//		WHERE (ID = %d)"),m_nID);
//
//	CADODatabase::ExecuteSQLStatement(strSQL);
//}
//
//int CArrivalDelayTrigger::getRunwayMarkIndex() const
//{
//	return m_nRunwayIndex;
//}
////////////////////////////////////////////////////////////////////////////
//// class CArrivalDelayTriggerList
//
//CArrivalDelayTriggerList::CArrivalDelayTriggerList(const FlightConstraint& fltType)
//: m_nID(-1)
// ,m_nProjID(-1)
// ,m_vArriDelayTri(NULL)
// ,m_vArriDelayTriNeedDel(NULL)
//{
//	m_fltType = fltType;
//}
//CArrivalDelayTriggerList::CArrivalDelayTriggerList(CAirportDatabase* pAirportDatbase)
//: m_nID(-1)
//,m_nProjID(-1)
//,m_vArriDelayTri(NULL)
// ,m_vArriDelayTriNeedDel(NULL)
//{
//	m_fltType.SetAirportDB(pAirportDatbase);
//}
//
//CArrivalDelayTriggerList::~CArrivalDelayTriggerList()
//{
//	//save and update 
//	std::vector<CArrivalDelayTrigger*>::iterator iter = m_vArriDelayTri.begin();
//	for (;iter!= m_vArriDelayTri.end();++iter)
//	{
//		delete *iter;
//	}
//
//	//delete obsolete date
//	iter = m_vArriDelayTriNeedDel.begin();
//	for (;iter != m_vArriDelayTriNeedDel.end(); ++iter)
//	{
//		delete *iter;
//	}
//}
//
//CString CArrivalDelayTriggerList::GetFltTypeStr() const
//{
//	char str[1024];
//	m_fltType.screenPrint(str);
//	return CString(str);
//}
//
//std::vector<CArrivalDelayTrigger*>& CArrivalDelayTriggerList::GetAllItem()
//{
//	return m_vArriDelayTri; 
//}
//
//int CArrivalDelayTriggerList::GetItemCount()
//{	
//	return static_cast<int>(m_vArriDelayTri.size());	
//}
//
//void CArrivalDelayTriggerList::SetFltType( const CString& strFltType )
//{
//	m_fltType.setConstraintWithVersion(strFltType);
//}
//
//CArrivalDelayTrigger* CArrivalDelayTriggerList::GetItem(int nIndex)
//{
//	ASSERT(nIndex < (int)m_vArriDelayTri.size());
//	return m_vArriDelayTri.at(nIndex);
//}
//
//void CArrivalDelayTriggerList::DeleteItem(int nIndex)
//{
//	ASSERT(nIndex < (int)m_vArriDelayTri.size());
//	m_vArriDelayTriNeedDel.push_back(m_vArriDelayTri[nIndex]);
//	m_vArriDelayTri.erase(m_vArriDelayTri.begin() + nIndex);
//}
//
//void CArrivalDelayTriggerList::AddItem(CArrivalDelayTrigger* pArriDelayTri)
//{
//	m_vArriDelayTri.push_back(pArriDelayTri); 
//}
//
//BOOL CArrivalDelayTriggerList::ReadData(CADORecordset& adoRecordset)
//{
//	adoRecordset.GetFieldValue(_T("ID"),m_nID);
//	adoRecordset.GetFieldValue(_T("PROJECTID"),m_nProjID);
//	CString strFltType;
//	adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);
//	SetFltType(strFltType);
//	ReadData();
//	return TRUE;
//}
//BOOL CArrivalDelayTriggerList::ReadData()
//{
//	CString strSQL = _T("");
//	strSQL.Format(_T("SELECT ID,FLTTYPEID, RUNWAYID, RWMARKINDEX, WAITTIME, QUEUELENGTH\
//		FROM IN_RUNWAY_ARRDELAYTRIGGERDATA\
//		WHERE (FLTTYPEID = %d)"),m_nID);
//
//	CADORecordset adoRecordset;
//	long nRowAffect = 0;
//
//	CADODatabase::ExecuteSQLStatement(strSQL,nRowAffect,adoRecordset);
//	while (!adoRecordset.IsEOF())
//	{
//		CArrivalDelayTrigger *pData = new CArrivalDelayTrigger;
//
//		pData->ReadData(adoRecordset);
//		m_vArriDelayTri.push_back(pData);
//
//		adoRecordset.MoveNextData();
//	}
//
//	return TRUE;
//}
//BOOL CArrivalDelayTriggerList::SaveData(int nProjID)
//{
//	if (m_nID != -1)
//	{
//		return UpdateData();
//	}
//	CString strSQL = _T("");
//	strSQL.Format(_T("INSERT INTO IN_RUNWAY_ARRDELAYTRIGGER\
//		(PROJECTID, FLTTYPE)\
//		VALUES (%d,'%s')"),nProjID,GetFltTypeStr() );
//
//	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
//	
//	//save and update 
//	std::vector<CArrivalDelayTrigger*>::iterator iter = m_vArriDelayTri.begin();
//	for (;iter!= m_vArriDelayTri.end();++iter)
//	{
//		(*iter)->SaveData(m_nID);
//	}
//
//	//delete obsolete date
//	iter = m_vArriDelayTriNeedDel.begin();
//	for (;iter != m_vArriDelayTriNeedDel.end(); ++iter)
//	{
//		(*iter)->DeleteData();
//		delete *iter;
//	}	
//	m_vArriDelayTriNeedDel.clear();
//	return TRUE;
//}
//BOOL CArrivalDelayTriggerList::UpdateData()
//{
//
//	CString strSQL =_T("");
//	strSQL.Format(_T("UPDATE IN_RUNWAY_ARRDELAYTRIGGER\
//		SET PROJECTID =%d, FLTTYPE ='%s'\
//		WHERE (ID = %d)"),m_nProjID,GetFltTypeStr(),m_nID);
//
//	CADODatabase::ExecuteSQLStatement(strSQL);
//
//	//save and update 
//	std::vector<CArrivalDelayTrigger*>::iterator iter = m_vArriDelayTri.begin();
//	for (;iter!= m_vArriDelayTri.end();++iter)
//	{
//		(*iter)->SaveData(m_nID);
//	}
//
//	//delete obsolete date
//	iter = m_vArriDelayTriNeedDel.begin();
//	for (;iter != m_vArriDelayTriNeedDel.end(); ++iter)
//	{
//		(*iter)->DeleteData();
//		delete *iter;
//	}	
//	m_vArriDelayTriNeedDel.clear();
//	return TRUE;
//}
//BOOL CArrivalDelayTriggerList::DeleteData()
//{
//	if (m_nID == -1)
//	 return TRUE;
//
//	CString strSQL;
//
//	strSQL.Format(_T("DELETE FROM IN_RUNWAY_ARRDELAYTRIGGER\
//		WHERE (ID = %d)"),m_nID);
//
//	CADODatabase::ExecuteSQLStatement(strSQL);
//	return TRUE;
//
//}
//
//FlightConstraint& CArrivalDelayTriggerList::GetFltType()
//{
//	return m_fltType;
//}
//
//FlightConstraint CArrivalDelayTriggerList::GetFltType() const
//{
//	return m_fltType;
//}
//////////////////////////////////////////////////////////////////////////
// class CArrivalDelayTriggerDatabase

CAirsideArrivalDelayTrigger::CAirsideArrivalDelayTrigger(CAirportDatabase* pAirportDatbase)
: m_nID(-1)
 ,m_nProjID(-1)
 //,m_vArriDelayTriList(NULL)
 //,m_vArriDelayTriListNeedDel(NULL)
{
	m_pAirportDB = pAirportDatbase;
}

CAirsideArrivalDelayTrigger::~CAirsideArrivalDelayTrigger()
{
	//std::vector<AirsideArrivalDelayTrigger::CFlightTypeItem *>::iterator iter = m_vArriDelayTriList.begin();
	//for(; iter != m_vArriDelayTriList.end(); ++iter)
	//{
	//	delete *iter;
	//}

	//iter = m_vArriDelayTriListNeedDel.begin();
	//for (;iter != m_vArriDelayTriListNeedDel.end(); ++iter)
	//{
	//	delete *iter;
	//}
}
void CAirsideArrivalDelayTrigger::ReadData()
{
	CString strSelectSQL;
	GetSelectElementSQL(strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		AirsideArrivalDelayTrigger::CFlightTypeItem* element = new AirsideArrivalDelayTrigger::CFlightTypeItem(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}

void CAirsideArrivalDelayTrigger::ReadData( int nProjectID )
{
	//use the pre one
	ASSERT(0);
}
//BOOL CAirsideArrivalDelayTrigger::ReadData(int nProjID)
//{
//	ASSERT(nProjID >= 0);
//	setProjID(nProjID);
//
//	CString strSQL = _T("");
//	strSQL.Format(_T("SELECT ID, PROJECTID, FLTTYPE\
//		FROM IN_RUNWAY_ARRDELAYTRIGGER\
//		WHERE (PROJECTID = %d)"),nProjID);
//
//	CADORecordset adoRecordset;
//	long nCountAffect = 0;
//
//	CADODatabase::ExecuteSQLStatement(strSQL,nCountAffect,adoRecordset);
//	
//	while( !adoRecordset.IsEOF())
//	{
//		//CArrivalDelayTriggerList * pData = new AirsideArrivalDelayTrigger::CFlightTypeItem(m_pAirportDB);
//		//pData->ReadData(adoRecordset);
//		//
//		//m_vArriDelayTriList.push_back(pData);
//
//		adoRecordset.MoveNextData();
//	}
//
//
//	return true;
//}

//BOOL CAirsideArrivalDelayTrigger::SaveData(int nProjID)
//{
//	std::vector<AirsideArrivalDelayTrigger::CFlightTypeItem *>::iterator iter = m_vArriDelayTriList.begin();
//	for(; iter != m_vArriDelayTriList.end(); ++iter)
//	{
//		//(*iter)->SaveData(nProjID);
//	}
//
//
//	iter = m_vArriDelayTriListNeedDel.begin();
//	for (;iter != m_vArriDelayTriListNeedDel.end(); ++iter)
//	{
//		//(*iter)->DeleteData();
//		delete *iter;
//	}
//	m_vArriDelayTriListNeedDel.clear();
//	return true;
//}

void CAirsideArrivalDelayTrigger::setAirportDB(CAirportDatabase *pAirportDB)
{
	m_pAirportDB = pAirportDB;
}
CAirportDatabase *CAirsideArrivalDelayTrigger::getAirportDB() const
{
	return m_pAirportDB;
}

void CAirsideArrivalDelayTrigger::GetSelectElementSQL( CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM IN_RUNWAY_ARRDELAYTRIGGER"));
}

void CAirsideArrivalDelayTrigger::setProjID( int nProjID )
{
	m_nProjID = nProjID;
}

int CAirsideArrivalDelayTrigger::getProjID() const
{
	return m_nProjID;
}

void CAirsideArrivalDelayTrigger::AddNewItem( AirsideArrivalDelayTrigger::CFlightTypeItem * dbElement )
{
	DBElementCollection<AirsideArrivalDelayTrigger::CFlightTypeItem >::AddNewItem(dbElement);
}
//BOOL CAirsideArrivalDelayTrigger::AddItem(AirsideArrivalDelayTrigger::CFlightTypeItem  *pArriFltType)
//{
//	m_vArriDelayTriList.push_back(pArriFltType);
//	return TRUE;
//}
//
//BOOL CAirsideArrivalDelayTrigger::DelItem(AirsideArrivalDelayTrigger::CFlightTypeItem  *pArriFltType)
//{
//	std::vector<AirsideArrivalDelayTrigger::CFlightTypeItem *>::iterator iter = m_vArriDelayTriList.begin();
//	for(; iter != m_vArriDelayTriList.end(); ++iter)
//	{
//		if ( pArriFltType == *iter)
//		{
//			m_vArriDelayTriList.erase(iter);
//			m_vArriDelayTriListNeedDel.push_back( pArriFltType );
//			return TRUE;
//		}
//	}
//
//	return FALSE;
//}

//int CAirsideArrivalDelayTrigger::GetItemCount()
//{
//	return static_cast<int>(m_vArriDelayTriList.size());
//}
//
//AirsideArrivalDelayTrigger::CFlightTypeItem * CAirsideArrivalDelayTrigger::GetItem(int nIndex)
//{
//	ASSERT(nIndex < static_cast<int>(m_vArriDelayTriList.size()));
//	return m_vArriDelayTriList.at(nIndex);
//}

AirsideArrivalDelayTrigger::CTimeRange::CTimeRange()
{

}

AirsideArrivalDelayTrigger::CTimeRange::~CTimeRange()
{

}

ElapsedTime AirsideArrivalDelayTrigger::CTimeRange::GetStartTime() const
{
	return m_eStartTime;
}

ElapsedTime AirsideArrivalDelayTrigger::CTimeRange::GetEndTime() const
{
	return m_eEndTime;
}

void AirsideArrivalDelayTrigger::CTimeRange::SetStartTime( const ElapsedTime& eStartTime )
{
	m_eStartTime = eStartTime;
}

void AirsideArrivalDelayTrigger::CTimeRange::SetEndTime( const ElapsedTime& strEndTime )
{
	m_eEndTime = strEndTime;
}

void AirsideArrivalDelayTrigger::CTimeRange::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);

	int nStartTime = 0;
	recordset.GetFieldValue(_T("STARTTIME"),nStartTime);
	m_eStartTime.set(nStartTime);

	int nEndTime = 24*60*60 -1;
	recordset.GetFieldValue(_T("ENDTIME"),nEndTime);
	m_eEndTime.set(nEndTime);
}

void AirsideArrivalDelayTrigger::CTimeRange::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_ARRDELAYTRIGGER_TIME (FLTTYPEID, STARTTIME,ENDTIME) VALUES (%d,%d,%d)"),
		nParentID,m_eStartTime.asSeconds(), m_eEndTime.asSeconds());
}

void AirsideArrivalDelayTrigger::CTimeRange::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE IN_RUNWAY_ARRDELAYTRIGGER_TIME  SET STARTTIME = %d, ENDTIME = %d  WHERE (ID = %d)"),
		m_eStartTime.asSeconds(), m_eEndTime.asSeconds(), m_nID);
}

void AirsideArrivalDelayTrigger::CTimeRange::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_ARRDELAYTRIGGER_TIME WHERE (ID = %d)"),m_nID);
}

void AirsideArrivalDelayTrigger::CTimeRange::GetSelectSQL( int nID,CString& strSQL ) const
{
	ASSERT(0);
}

AirsideArrivalDelayTrigger::CTimeRangeList::CTimeRangeList()
{
	m_nFltTypeID = -1;
}

AirsideArrivalDelayTrigger::CTimeRangeList::~CTimeRangeList()
{

}

void AirsideArrivalDelayTrigger::CTimeRangeList::GetSelectElementSQL( int nFltTypeID,CString& strSQL ) const
{
	m_nFltTypeID = nFltTypeID;
	strSQL.Format(_T("SELECT * FROM IN_RUNWAY_ARRDELAYTRIGGER_TIME WHERE FLTTYPEID = %d"),nFltTypeID);
}

bool AirsideArrivalDelayTrigger::CTimeRangeList::IsTimeInTimeRangeList( const ElapsedTime& tTime )
{
	size_t nTimeCount = GetElementCount();
	for (size_t j =0; j < nTimeCount; j++)
	{
		CTimeRange* pTimeRange = GetItem(j);
		if (pTimeRange->GetStartTime() <= tTime && pTimeRange->GetEndTime() >= tTime)
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////
AirsideArrivalDelayTrigger::CTriggerCondition::CTriggerCondition()
{
	m_nQueueLength = 10;
	m_dMinPerAircraft = 2;
	m_emType = Final_Trigger;
}

AirsideArrivalDelayTrigger::CTriggerCondition::~CTriggerCondition()
{

}

RunwayExit* AirsideArrivalDelayTrigger::CTriggerCondition::GetTakeOffPosition()
{
	return &m_takeOffPosition;
}

int AirsideArrivalDelayTrigger::CTriggerCondition::GetQueueLength()
{
	return m_nQueueLength;
}

double AirsideArrivalDelayTrigger::CTriggerCondition::GetMinsPerAircraft()
{
	return m_dMinPerAircraft;
}

void AirsideArrivalDelayTrigger::CTriggerCondition::SetTakeOffPosition( int nExitID )
{
	m_takeOffPosition.ReadData(nExitID);
}

void AirsideArrivalDelayTrigger::CTriggerCondition::SetQueueLength( int nQueueLength )
{
	m_nQueueLength = nQueueLength;
}

void AirsideArrivalDelayTrigger::CTriggerCondition::SetMinsPerAircraft( int nMins )
{
	m_dMinPerAircraft = nMins;
}

void AirsideArrivalDelayTrigger::CTriggerCondition::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	int nExitID;
	recordset.GetFieldValue(_T("RUNWAYEXITID"),nExitID);

	m_takeOffPosition.ReadData(nExitID);
	recordset.GetFieldValue(_T("QUEUELENGTH"),m_nQueueLength);
	recordset.GetFieldValue(_T("MINSPERAC"),m_dMinPerAircraft);
	int nQueueType = 0;
	recordset.GetFieldValue(_T("QUEUERTYPE"),nQueueType);
	m_emType = QueueType(nQueueType);
}

void AirsideArrivalDelayTrigger::CTriggerCondition::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	int nExitID = m_takeOffPosition.GetID();

	strSQL.Format(_T("INSERT INTO IN_RUNWAY_ARRDELAYTRIGGER_CONDITION (FLTTYPEID, RUNWAYEXITID,QUEUELENGTH,MINSPERAC,QUEUERTYPE) VALUES (%d,%d,%d,%f,%d)"),
		nParentID,nExitID,m_nQueueLength,m_dMinPerAircraft,(int)m_emType);
}

void AirsideArrivalDelayTrigger::CTriggerCondition::GetUpdateSQL( CString& strSQL ) const
{
	int nExitID = m_takeOffPosition.GetID();
	strSQL.Format(_T("UPDATE IN_RUNWAY_ARRDELAYTRIGGER_CONDITION  SET RUNWAYEXITID = %d, QUEUELENGTH = %d,MINSPERAC = %f,QUEUERTYPE = %d  WHERE (ID = %d)"),
		nExitID, m_nQueueLength, m_dMinPerAircraft, (int)m_emType,m_nID);
}

void AirsideArrivalDelayTrigger::CTriggerCondition::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_ARRDELAYTRIGGER_CONDITION WHERE (ID = %d)"),m_nID);
}

void AirsideArrivalDelayTrigger::CTriggerCondition::GetSelectSQL( int nID,CString& strSQL ) const
{
	ASSERT(0);
}




AirsideArrivalDelayTrigger::CTriggerConditionList::CTriggerConditionList()
{
	m_nFltTypeID = -1;
}

AirsideArrivalDelayTrigger::CTriggerConditionList::~CTriggerConditionList()
{

}

void AirsideArrivalDelayTrigger::CTriggerConditionList::GetSelectElementSQL( int nFltTypeID,CTriggerCondition::QueueType emType,CString& strSQL ) const
{
	m_nFltTypeID = nFltTypeID;
	strSQL.Format(_T("SELECT * FROM IN_RUNWAY_ARRDELAYTRIGGER_CONDITION WHERE FLTTYPEID = %d AND QUEUERTYPE = %d"),nFltTypeID,(int)emType);
}


void AirsideArrivalDelayTrigger::CTriggerConditionList::ReadData( int nParentID,CTriggerCondition::QueueType emType )
{
	CString strSelectSQL;
	GetSelectElementSQL(nParentID,emType,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CTriggerCondition* element = new CTriggerCondition;
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}

AirsideArrivalDelayTrigger::CRunwayMark::CRunwayMark()
{
	m_pRunway = NULL;
	m_runwayMark = RUNWAYMARK_FIRST;
}

AirsideArrivalDelayTrigger::CRunwayMark::~CRunwayMark()
{
	delete m_pRunway;
}

void AirsideArrivalDelayTrigger::CRunwayMark::SetRunwayMark( int nRunwayID, RUNWAY_MARK runwayMark )
{
	delete m_pRunway;
	m_pRunway = new Runway;
	m_pRunway->ReadObject(nRunwayID);
	m_runwayMark = runwayMark;
}

CString AirsideArrivalDelayTrigger::CRunwayMark::GetRunwayMark()
{
	CString strName = _T("");
	if(m_pRunway)
	{
		if(m_runwayMark == RUNWAYMARK_FIRST)
			strName =  m_pRunway->GetMarking1().c_str();
		else if(m_runwayMark == RUNWAYMARK_SECOND)
			strName = m_pRunway->GetMarking2().c_str();
		else
			ASSERT(0);//should not be here
	}
	return strName;
}

void AirsideArrivalDelayTrigger::CRunwayMark::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	int nRunwayID;
	recordset.GetFieldValue(_T("RUNWAYID"),nRunwayID);

	m_pRunway = new Runway;
	m_pRunway->ReadObject(nRunwayID);

	int nRunwayMark = RUNWAYMARK_FIRST;
	recordset.GetFieldValue(_T("RUNWAYMARK"),nRunwayMark);
	m_runwayMark = (RUNWAY_MARK)nRunwayMark;
}

void AirsideArrivalDelayTrigger::CRunwayMark::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	int nRunwayID = -1;
	if(m_pRunway)
		nRunwayID = m_pRunway->getID();

	strSQL.Format(_T("INSERT INTO IN_RUNWAY_ARRDELAYTRIGGER_RUNWAY (FLTTYPEID, RUNWAYID,RUNWAYMARK) VALUES (%d,%d,%d)"),nParentID,nRunwayID,(int)m_runwayMark);
}

void AirsideArrivalDelayTrigger::CRunwayMark::GetUpdateSQL( CString& strSQL ) const
{
	int nRunwayID = -1;
	if(m_pRunway)
		nRunwayID = m_pRunway->getID();
	strSQL.Format(_T("UPDATE IN_RUNWAY_ARRDELAYTRIGGER_RUNWAY  SET RUNWAYID = %d, RUNWAYMARK = %d WHERE (ID = %d)"),nRunwayID,(int)m_runwayMark,m_nID);
}

void AirsideArrivalDelayTrigger::CRunwayMark::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_ARRDELAYTRIGGER_RUNWAY WHERE (ID = %d)"),m_nID);
}

void AirsideArrivalDelayTrigger::CRunwayMark::GetSelectSQL( int nID,CString& strSQL ) const
{
	ASSERT(0);
}

AirsideArrivalDelayTrigger::CRunwayMarkList::CRunwayMarkList()
{
	m_nFltTypeID = -1;
}

AirsideArrivalDelayTrigger::CRunwayMarkList::~CRunwayMarkList()
{

}

void AirsideArrivalDelayTrigger::CRunwayMarkList::GetSelectElementSQL( int nFltTypeID,CString& strSQL ) const
{
	m_nFltTypeID = nFltTypeID;
	strSQL.Format(_T("SELECT * FROM IN_RUNWAY_ARRDELAYTRIGGER_RUNWAY WHERE FLTTYPEID = %d"),nFltTypeID);
}

bool AirsideArrivalDelayTrigger::CRunwayMarkList::IsLogicRunwayInRunwayList( int RwyID, RUNWAY_MARK RwyMark )
{
	size_t nCount = GetElementCount();
	for (size_t i =0; i < nCount; i++)
	{
		CRunwayMark* pRwyItem = GetItem(i);
		if (pRwyItem->m_pRunway->getID() == RwyID && pRwyItem->m_runwayMark == RwyMark)
			return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////////////
CString AirsideArrivalDelayTrigger::CFlightTypeItem::GetFlightTypeName()
{
	CString strFltType;
	m_fltCons.screenPrint(strFltType.GetBuffer(1024), 0, 1024);
	strFltType.ReleaseBuffer();
	return strFltType;
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::SetFlightConstraint( FlightConstraint& fltCons )
{
	m_fltCons = fltCons;
}

AirsideArrivalDelayTrigger::CTimeRangeList& AirsideArrivalDelayTrigger::CFlightTypeItem::GetTimeRangeList()
{
	return m_vTimeRange;
}

AirsideArrivalDelayTrigger::CRunwayMarkList& AirsideArrivalDelayTrigger::CFlightTypeItem::GetRunwayList()
{
	return m_vRunwayMark;
}

AirsideArrivalDelayTrigger::CTriggerConditionList& AirsideArrivalDelayTrigger::CFlightTypeItem::GetTriggerConditionList()
{
	return m_vTriggerCondition;
}


AirsideArrivalDelayTrigger::CTriggerConditionList& AirsideArrivalDelayTrigger::CFlightTypeItem::GetEnRouteConditionList()
{
	return m_vEnRouteCondition;
}


void AirsideArrivalDelayTrigger::CFlightTypeItem::AddItem( CTimeRange * pTimeRange )
{
	m_vTimeRange.AddNewItem(pTimeRange);
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::AddItem( CRunwayMark *pRunwayMark )
{
	m_vRunwayMark.AddNewItem(pRunwayMark);
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::AddItem( CTriggerCondition *pTriggerCondiion )
{
	if (pTriggerCondiion == NULL)
		return;
	
	if (pTriggerCondiion->GetTriggerType() == CTriggerCondition::Final_Trigger)
	{
		m_vTriggerCondition.AddNewItem(pTriggerCondiion);
	}
	else
	{
		m_vEnRouteCondition.AddNewItem(pTriggerCondiion);
	}
}


void AirsideArrivalDelayTrigger::CFlightTypeItem::DelItem( CTimeRange * pTimeRange )
{
	m_vTimeRange.DeleteItem(pTimeRange);
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::DelItem( CRunwayMark *pRunwayMark )
{
	m_vRunwayMark.DeleteItem(pRunwayMark);
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::DelItem( CTriggerCondition *pTriggerCondiion )
{
	if (pTriggerCondiion == NULL)
		return;

	if (pTriggerCondiion->GetTriggerType() == CTriggerCondition::Final_Trigger)
	{
		m_vTriggerCondition.DeleteItem(pTriggerCondiion);
	}
	else
	{
		m_vEnRouteCondition.DeleteItem(pTriggerCondiion);
	}
	
}


void AirsideArrivalDelayTrigger::CFlightTypeItem::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	CString strFlightType;
	recordset.GetFieldValue(_T("FLTTYPE"),strFlightType);
	m_fltCons.SetAirportDB(m_pAirportDB);
	m_fltCons.setConstraintWithVersion(strFlightType);

	int iTriggerType = 0;
	recordset.GetFieldValue(_T("TRIGGERTYPE"),iTriggerType);
	m_emType = TriggerType(iTriggerType);

	int iMaxTime =0 ;
	recordset.GetFieldValue(_T("MAXDELAYTIME"), iMaxTime );
	m_tMaxDelayTime.setPrecisely(iMaxTime);

	m_vTimeRange.ReadData(m_nID);

	m_vRunwayMark.ReadData(m_nID);

	m_vTriggerCondition.ReadData(m_nID,CTriggerCondition::Final_Trigger);

	m_vEnRouteCondition.ReadData(m_nID,CTriggerCondition::Enroute_Trigger);
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	CString strFlightType;
	m_fltCons.WriteSyntaxStringWithVersion(strFlightType.GetBuffer(1024));
	strFlightType.ReleaseBuffer();

	strSQL.Format(_T("INSERT INTO IN_RUNWAY_ARRDELAYTRIGGER (PROJECTID, FLTTYPE,TRIGGERTYPE, MAXDELAYTIME) VALUES (%d,'%s',%d, %d)"),nParentID,strFlightType,(int)m_emType, m_tMaxDelayTime.getPrecisely() );
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::GetUpdateSQL( CString& strSQL ) const
{
	CString strFlightType;
	m_fltCons.WriteSyntaxStringWithVersion(strFlightType.GetBuffer(1024));
	strFlightType.ReleaseBuffer();

	strSQL.Format(_T("UPDATE IN_RUNWAY_ARRDELAYTRIGGER  SET FLTTYPE ='%s',TRIGGERTYPE = %d, MAXDELAYTIME = %d WHERE (ID = %d)"),strFlightType,(int)m_emType, m_tMaxDelayTime.getPrecisely() ,m_nID);
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_ARRDELAYTRIGGER WHERE (ID = %d)"),m_nID);
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::GetSelectSQL( int nID,CString& strSQL ) const
{
	ASSERT(0);
}

void AirsideArrivalDelayTrigger::CFlightTypeItem::SaveData( int nParentID )
{
	DBElement::SaveData(nParentID);

	m_vTimeRange.SaveData(m_nID);

	m_vRunwayMark.SaveData(m_nID);

	m_vTriggerCondition.SaveData(m_nID);
	
	m_vEnRouteCondition.SaveData(m_nID);
}

const FlightConstraint& AirsideArrivalDelayTrigger::CFlightTypeItem::GetFltType()
{
	return m_fltCons;
}


