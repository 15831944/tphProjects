#include "StdAfx.h"
#include ".\towoffstandassignmentdata.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "Common/AirportDatabase.h"
#include "../InputAirside/ALTObjectGroup.h"
#include "../common/ALTObjectID.h"
#include "..\Database\DBElementCollection_Impl.h"

#include "Stand.h"
//////////////////////////////////////////////////////////////////////////
// CTowOffStandReturnToStandEntry

CTowOffStandReturnToStandEntry::CTowOffStandReturnToStandEntry(void)
	: m_nStandID(STAND_ID_ALL)
	, m_nTimeBeforeEmplanement(30)
{
	InitStandName();
}
CTowOffStandReturnToStandEntry::~CTowOffStandReturnToStandEntry(void)
{

}


void CTowOffStandReturnToStandEntry::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("STAND_ID"), m_nStandID);
	InitStandName();
	recordset.GetFieldValue(_T("TIME_BEFORE_EMPLANEMENT"), m_nTimeBeforeEmplanement);
}
void CTowOffStandReturnToStandEntry::GetInsertSQL(int nParentID, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO TB_TOWOFF_STAND_RETURNTO_STAND\
					 (STAND_ID, TIME_BEFORE_EMPLANEMENT, PARENTID)\
					 VALUES (%d, %d, %d)")
					 , m_nStandID
					 , m_nTimeBeforeEmplanement
					 , nParentID
					 );
}
void CTowOffStandReturnToStandEntry::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE TB_TOWOFF_STAND_RETURNTO_STAND\
					 SET STAND_ID = %d, \
					 TIME_BEFORE_EMPLANEMENT = %d \
					 WHERE (ID = %d)")
					 , m_nStandID
					 , m_nTimeBeforeEmplanement
					 , m_nID
					 );
}
void CTowOffStandReturnToStandEntry::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM TB_TOWOFF_STAND_RETURNTO_STAND\
					 WHERE (ID = %d)")
					 , m_nID
					 );
}

void CTowOffStandReturnToStandEntry::InitStandName()
{
	ALTObjectGroup altObjGroup;
	altObjGroup.ReadData(m_nStandID);
	m_StandName = altObjGroup.getName();
}

ALTObjectID CTowOffStandReturnToStandEntry::GetStandName() const
{
	return m_StandName;
}

CString CTowOffStandReturnToStandEntry::GetStandNameString() const
{
	if (STAND_ID_ALL == m_nStandID)
	{
		return _T("All");
	}
	return m_StandName.GetIDString();
}
//////////////////////////////////////////////////////////////////////////
// CTowOffStandReturnToStandList

CTowOffStandReturnToStandList::CTowOffStandReturnToStandList(TowOffDataContentNonNull NullOrNonNull/* = ContentNull*/)
{
	if (ContentNonNull == NullOrNonNull)
	{
		AddNewItem(new CTowOffStandReturnToStandEntry);
	}
}
CTowOffStandReturnToStandList::~CTowOffStandReturnToStandList(void)
{

}

void CTowOffStandReturnToStandList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT * \
					 FROM TB_TOWOFF_STAND_RETURNTO_STAND\
					 WHERE (PARENTID = %d)")
					 , nParentID
					 );
}

//////////////////////////////////////////////////////////////////////////
// CTowOffPriorityEntry

CTowOffPriorityEntry::CTowOffPriorityEntry(TowOffDataContentNonNull NullOrNonNull/* = ContentNull*/)
	: m_nStandID(STAND_ID_ALL)
	, m_listTowOffStandReturnToStand(NullOrNonNull)
{

}
CTowOffPriorityEntry::~CTowOffPriorityEntry(void)
{

}

void CTowOffPriorityEntry::SaveData(int nParentID)
{
	DBElement::SaveData(nParentID);
	m_listTowOffStandReturnToStand.SaveData(m_nID);
}
void CTowOffPriorityEntry::DeleteData()
{
	m_listTowOffStandReturnToStand.DeleteData();
	m_listTowOffStandReturnToStand.SaveData(-1/* don't care*/);
	DBElement::DeleteData();
}

void CTowOffPriorityEntry::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("STAND_ID"), m_nStandID);
	InitStandName();
	m_listTowOffStandReturnToStand.ReadData(m_nID);
	if (m_listTowOffStandReturnToStand.GetElementCount()==0)
	{
		m_listTowOffStandReturnToStand.AddNewItem(new CTowOffStandReturnToStandEntry);
	}
}
void CTowOffPriorityEntry::GetInsertSQL(int nParentID, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO TB_TOWOFF_PRIORITY\
					 (STAND_ID, PARENTID) VALUES (%d, %d)")
					 , m_nStandID
					 , nParentID
					 );
}
void CTowOffPriorityEntry::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE TB_TOWOFF_PRIORITY\
					 SET STAND_ID = %d \
					 WHERE (ID = %d)")
					 , m_nStandID
					 , m_nID
					 );
}
void CTowOffPriorityEntry::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM TB_TOWOFF_PRIORITY\
					 WHERE (ID = %d)")
					 , m_nID
					 );
}

CTowOffStandReturnToStandEntry* CTowOffPriorityEntry::FindReturnToStandItem(int nReturnToStandID)
{
	size_t nCount = m_listTowOffStandReturnToStand.GetElementCount();
	for (size_t i=0;i<nCount;i++)
	{
		if (m_listTowOffStandReturnToStand.GetItem(i)->GetStandID() == nReturnToStandID)
		{
			return m_listTowOffStandReturnToStand.GetItem(i);
		}
	}
	return NULL;
}

CTowOffStandReturnToStandEntry* CTowOffPriorityEntry::AddReturnToStandItem(int nReturnToStandID)
{
	ASSERT(NULL == FindReturnToStandItem(nReturnToStandID));
	CTowOffStandReturnToStandEntry* pReturnToStandEntry = new CTowOffStandReturnToStandEntry;
	pReturnToStandEntry->SetStandID(nReturnToStandID);
	m_listTowOffStandReturnToStand.AddNewItem(pReturnToStandEntry);
	return pReturnToStandEntry;
}
void CTowOffPriorityEntry::AddReturnToStandItem(CTowOffStandReturnToStandEntry* pReturnToStandEntry)
{
	ASSERT(pReturnToStandEntry);
	m_listTowOffStandReturnToStand.AddNewItem(pReturnToStandEntry);
}
bool CTowOffPriorityEntry::SetReturnToStandItemData(int nPreReturnToStandID,int nCurReturnToStandID)
{
	size_t nCount = m_listTowOffStandReturnToStand.GetElementCount();
	for (size_t i=0;i<nCount;i++)
	{
		if (m_listTowOffStandReturnToStand.GetItem(i)->GetStandID() == nPreReturnToStandID)
		{
			m_listTowOffStandReturnToStand.GetItem(i)->SetStandID(nCurReturnToStandID);
			return true;
		}
	}
	return false;
}

void CTowOffPriorityEntry::InitStandName()
{
	ALTObjectGroup altObjGroup;
	altObjGroup.ReadData(m_nStandID);
	m_StandName = altObjGroup.getName();
}

void CTowOffPriorityEntry::RemoveReturnToStandItem( CTowOffStandReturnToStandEntry* pReturnToStandEntry )
{
	m_listTowOffStandReturnToStand.DeleteItem(pReturnToStandEntry);
}

CTowOffStandReturnToStandEntry* CTowOffPriorityEntry::GetReturnToStandItem( int nIndex )
{
	return m_listTowOffStandReturnToStand.GetItem(nIndex);
}

CTowOffStandReturnToStandList& CTowOffPriorityEntry::GetListTowOffStandReturnToStand()
{
	return m_listTowOffStandReturnToStand;
}

ALTObjectID CTowOffPriorityEntry::GetStandName() const
{
	return m_StandName;
}

CString CTowOffPriorityEntry::GetStandNameString() const
{
	if (STAND_ID_ALL == m_nStandID)
	{
		return _T("All");
	}
	return m_StandName.GetIDString();
}

int CTowOffPriorityEntry::GetReturnToStandCout()
{
	return (int)m_listTowOffStandReturnToStand.GetElementCount();
}
//////////////////////////////////////////////////////////////////////////
// CTowOffPriorityList

CTowOffPriorityList::CTowOffPriorityList(TowOffDataContentNonNull NullOrNonNull/* = ContentNull*/)
{
	if (ContentNonNull == NullOrNonNull)
	{
		AddNewItem(new CTowOffPriorityEntry(ContentNonNull));
	}
}
CTowOffPriorityList::~CTowOffPriorityList()
{

}

void CTowOffPriorityList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT * \
					 FROM TB_TOWOFF_PRIORITY\
					 WHERE (PARENTID = %d)")
					 , nParentID
					 );
}

//////////////////////////////////////////////////////////////////////////
// CTowOffStandAssignmentData

CTowOffStandAssignmentData::CTowOffStandAssignmentData(TowOffDataContentNonNull NullOrNonNull/* = ContentNull*/)
	: m_strFlightType(_T("DEFAULT"))
	, m_nDePlanementTime(30)
	, m_nEnPlanementTime(30)
	, m_nParkStandID(STAND_ID_ALL)
	, m_bTowoff(FALSE)
	, m_bRepositionOrFreeup(TRUE)
	, m_nTimeLeaveArrStand(10)
	, m_enumTimeLeaveArrStandType(CTowOffStandAssignmentData::BeforeDeparture)
	, m_nTowOffCriteria(180)
	, m_bBackToArrivalStand(TRUE)
	, m_listPriorities(NullOrNonNull)
	,m_bPrefer(false)
	, m_pAirportDatabase(NULL)
{
}

CTowOffStandAssignmentData::~CTowOffStandAssignmentData(void)
{
}

void CTowOffStandAssignmentData::SetFlightType(const AirsideFlightType& flightType)
{
	m_FlightType = flightType;
	m_strFlightType =  m_FlightType.getDBString() ;
}

void CTowOffStandAssignmentData::SetFlightType(CString& strFlightType)
{
	m_strFlightType.Format(_T("%s"), strFlightType);

	if (m_pAirportDatabase)
	{
		m_FlightType.SetAirportDB(m_pAirportDatabase);
		m_FlightType.FormatDBStringToFlightType(m_strFlightType);
	}
}

const AirsideFlightType& CTowOffStandAssignmentData::GetFlightType()const
{
	return m_FlightType;
}

int CTowOffStandAssignmentData::GetDeplanementTime() const
{
	return m_nDePlanementTime;
}
int CTowOffStandAssignmentData::GetEnplanementTime() const
{
	return m_nEnPlanementTime;
}
int CTowOffStandAssignmentData::GetTowOffCriteria() const
{
	return m_nTowOffCriteria;
}
int CTowOffStandAssignmentData::GetParkStandsID() const
{
	return m_nParkStandID;
}
BOOL CTowOffStandAssignmentData::IsTow() const
{
	return m_bTowoff;
}

bool CTowOffStandAssignmentData::RemovePriorityItem(int nStandID)
{
	int nIndex = FindPriorityItemIndex(nStandID);
	if (-1 == nIndex)
	{
		return false;
	}
	m_listPriorities.DeleteItemFromIndex(nIndex);
	return true;
}

int CTowOffStandAssignmentData::FindPriorityItemIndex(int nStandID)
{
	size_t nCount = m_listPriorities.GetElementCount();
	for (int i=0;i<(int)nCount;i++)
	{
		if (m_listPriorities.GetItem(i)->GetStandID() == nStandID)
		{
			return i;
		}
	}
	return -1;
}

CTowOffPriorityEntry* CTowOffStandAssignmentData::FindPriorityItem(int nStandID)
{
	size_t nCount = m_listPriorities.GetElementCount();
	for (size_t i=0;i<nCount;i++)
	{
		if (m_listPriorities.GetItem(i)->GetStandID() == nStandID)
		{
			return m_listPriorities.GetItem(i);
		}
	}
	return NULL;
}

CTowOffPriorityEntry* CTowOffStandAssignmentData::AddPriorityItem(int nStandID)
{
	ASSERT(FindPriorityItem(nStandID) == NULL);

	CTowOffPriorityEntry* pPriorityEntry = new CTowOffPriorityEntry(ContentNonNull);
	pPriorityEntry->SetStandID(nStandID);
	m_listPriorities.AddNewItem(pPriorityEntry);
	return pPriorityEntry;
}

void CTowOffStandAssignmentData::AddPriorityItem(CTowOffPriorityEntry* pPriorityEntry)
{
	ASSERT(pPriorityEntry);

	m_listPriorities.AddNewItem(pPriorityEntry);
}

bool CTowOffStandAssignmentData::SetPriorityItemData(int nPrePriorityID,int nCurPriorityID)
{
	size_t nCount = m_listPriorities.GetElementCount();
	for (size_t i=0;i<nCount;i++)
	{
		CTowOffPriorityEntry* pPriorityEntry = m_listPriorities.GetItem(i);
		if (pPriorityEntry->GetStandID() == nPrePriorityID)
		{
			pPriorityEntry->SetStandID(nCurPriorityID);
			return true;
		}
	}
	return false;
}

CTowOffStandReturnToStandEntry* CTowOffStandAssignmentData::FindReturnToStandItem(int nPriorityStandID, int nReturnToStandID)
{
	CTowOffPriorityEntry* pPriorityEntry = FindPriorityItem(nPriorityStandID);
	if (NULL == pPriorityEntry)
	{
		return NULL;
	}

	return pPriorityEntry->FindReturnToStandItem(nReturnToStandID);
}
CTowOffStandReturnToStandEntry* CTowOffStandAssignmentData::AddReturnToStandItem(int nPriorityStandID, int nReturnToStandID)
{
	CTowOffPriorityEntry* pPriorityEntry = FindPriorityItem(nPriorityStandID);
	if (NULL == pPriorityEntry)
	{
		return NULL;
	}
	return pPriorityEntry->AddReturnToStandItem(nReturnToStandID);
}
bool CTowOffStandAssignmentData::SetReturnToStandItemData(int nPriorityStandID, int nPreReturnToStandID,int nCurReturnToStandID)
{
	CTowOffPriorityEntry* pPriorityEntry = FindPriorityItem(nPriorityStandID);
	if (NULL == pPriorityEntry)
	{
		return false;
	}
	return pPriorityEntry->SetReturnToStandItemData(nPreReturnToStandID, nCurReturnToStandID);
}


void CTowOffStandAssignmentData::DeleteData()
{
	m_listPriorities.DeleteData();
	m_listPriorities.SaveData(-1/* don't care*/);

	m_preferredTowoffData.DeleteData();
	m_preferredTowoffData.SaveData(-1);

	DBElement::DeleteData();
}

void CTowOffStandAssignmentData::SaveData(int nParentID)
{
	DBElement::SaveData(nParentID);
	m_listPriorities.SaveData(m_nID);
	m_preferredTowoffData.SaveData(m_nID);
}


//DBElement
void CTowOffStandAssignmentData::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("FLTTYPE"),m_strFlightType);
	if (m_strFlightType.IsEmpty())
	{
		m_strFlightType = _T("DEFAULT");
	}
	if (m_pAirportDatabase)
	{
		m_FlightType.SetAirportDB(m_pAirportDatabase);
		m_FlightType.FormatDBStringToFlightType(m_strFlightType);
	}
	recordset.GetFieldValue(_T("DEPLANEMENTTIME"),m_nDePlanementTime);
	recordset.GetFieldValue(_T("ENPLANEMENTTIME"),m_nEnPlanementTime);
	recordset.GetFieldValue(_T("PARKSTANDSID"),m_nParkStandID);
	if (m_nParkStandID == 0)
	{
		m_nParkStandID = STAND_ID_ALL;
	}
	InitStandName();
	recordset.GetFieldValue(_T("TOWOFF"),m_bTowoff);
	recordset.GetFieldValue(_T("REPOSITION_OR_FREEUP"), m_bRepositionOrFreeup);
	recordset.GetFieldValue(_T("TIME_LEAVE_ARR_STAND"), m_nTimeLeaveArrStand);
	recordset.GetFieldValue(_T("TIME_LEAVE_ARR_STAND_TYPE"), (int&)m_enumTimeLeaveArrStandType);
	ASSERT(CTowOffStandAssignmentData::BeforeDeparture == m_enumTimeLeaveArrStandType
		|| CTowOffStandAssignmentData::AfterArrival == m_enumTimeLeaveArrStandType
		);

	recordset.GetFieldValue(_T("TOWOFFCRITERIA"),m_nTowOffCriteria);
	recordset.GetFieldValue(_T("BACK_TO_ARRIVAL_STAND"),m_bBackToArrivalStand);
	int nPreferred = 0;
	recordset.GetFieldValue(_T("PREFERRED"),nPreferred);
	m_bPrefer = (nPreferred?true:false);
	m_listPriorities.ReadData(m_nID);
	m_preferredTowoffData.ReadData(m_nID);
	if (m_listPriorities.GetElementCount()==0)
	{
		m_listPriorities.AddNewItem(new CTowOffPriorityEntry(ContentNonNull));
	}
}

void CTowOffStandAssignmentData::GetInsertSQL(int nParentID, CString& strSQL)	const
{
	strSQL.Format(_T("INSERT INTO IN_TOWOFFSTANDASSIGNMENT \
					 (PARENTID, FLTTYPE, DEPLANEMENTTIME, ENPLANEMENTTIME, TOWOFFCRITERIA, PARKSTANDSID,TOWOFF, \
					 REPOSITION_OR_FREEUP, TIME_LEAVE_ARR_STAND, TIME_LEAVE_ARR_STAND_TYPE, BACK_TO_ARRIVAL_STAND,PREFERRED)\
					 VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d,%d)")
					 , nParentID
					 , m_strFlightType
					 , m_nDePlanementTime
					 , m_nEnPlanementTime
					 , m_nTowOffCriteria
					 , m_nParkStandID
					 , (int)m_bTowoff
					 , m_bRepositionOrFreeup
					 , m_nTimeLeaveArrStand
					 , (int)m_enumTimeLeaveArrStandType
					 , (int)m_bBackToArrivalStand
					 ,m_bPrefer?1:0
					 );
}

void CTowOffStandAssignmentData::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_TOWOFFSTANDASSIGNMENT\
					 SET FLTTYPE = '%s', \
					 DEPLANEMENTTIME = %d,\
					 ENPLANEMENTTIME = %d, \
					 TOWOFFCRITERIA = %d,\
					 PARKSTANDSID = %d, \
					 TOWOFF = %d, \
					 REPOSITION_OR_FREEUP = %d, \
					 TIME_LEAVE_ARR_STAND = %d, \
					 TIME_LEAVE_ARR_STAND_TYPE = %d, \
					 BACK_TO_ARRIVAL_STAND = %d, \
					 PREFERRED = %d \
					 WHERE (ID =%d)")
					 , m_strFlightType
					 , m_nDePlanementTime
					 , m_nEnPlanementTime
					 , m_nTowOffCriteria
					 , m_nParkStandID
					 , m_bTowoff
					 , m_bRepositionOrFreeup
					 , m_nTimeLeaveArrStand
					 , (int)m_enumTimeLeaveArrStandType
					 , (int)m_bBackToArrivalStand
					 ,m_bPrefer
					 , m_nID
					 );
}

void CTowOffStandAssignmentData::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_TOWOFFSTANDASSIGNMENT\
					 WHERE (ID = %d)"),m_nID);
}

void CTowOffStandAssignmentData::ExportData(CAirsideExportFile& exportFile)
{
}

void CTowOffStandAssignmentData::ImportData(CAirsideImportFile& importFile)
{
}

CTowOffPriorityList& CTowOffStandAssignmentData::GetListPriorities()
{
	return m_listPriorities;
}

BOOL CTowOffStandAssignmentData::GetRepositionOrFreeup() const
{
	return m_bRepositionOrFreeup;
}

void CTowOffStandAssignmentData::SetRepositionOrFreeup( BOOL val )
{
	m_bRepositionOrFreeup = val;
}

void CTowOffStandAssignmentData::SetAirportDatabase( CAirportDatabase* pAirportDatabase )
{
	m_pAirportDatabase = pAirportDatabase;
	// re-read FlightType
	if (m_pAirportDatabase)
	{
		m_FlightType.SetAirportDB(m_pAirportDatabase);
		if (FALSE == m_strFlightType.IsEmpty())
		{
			m_FlightType.FormatDBStringToFlightType(m_strFlightType);
		}

		for (int i = 0; i < m_preferredTowoffData.GetItemCount(); i++)
		{
			PreferTowoffFlightType* pTowoffFlightType = m_preferredTowoffData.GetItem(i);
			pTowoffFlightType->SetAirportDatabase(pAirportDatabase);
		}
	}
}

int CTowOffStandAssignmentData::GetTimeLeaveArrStand() const
{
	return m_nTimeLeaveArrStand;
}

void CTowOffStandAssignmentData::SetTimeLeaveArrStand( int val )
{
	m_nTimeLeaveArrStand = val;
}

BOOL CTowOffStandAssignmentData::GetBackToArrivalStand() const
{
	return m_bBackToArrivalStand;
}

void CTowOffStandAssignmentData::SetBackToArrivalStand( BOOL val )
{
	m_bBackToArrivalStand = val;
}

const ALTObjectID& CTowOffStandAssignmentData::GetStandALTObjectID()
{
	return m_ParkingStandName;
}
CString CTowOffStandAssignmentData::GetParStandNameString()
{
	if (STAND_ID_ALL == m_nParkStandID)
	{
		return _T("All");
	}
	return m_ParkingStandName.GetIDString();
}

CTowOffStandAssignmentData::TimeLeaveArrStandType CTowOffStandAssignmentData::GetTimeLeaveArrStandType() const
{
	return m_enumTimeLeaveArrStandType;
}

void CTowOffStandAssignmentData::SetTimeLeaveArrStandType( TimeLeaveArrStandType val )
{
	m_enumTimeLeaveArrStandType = val;
}

CTowOffPriorityEntry* CTowOffStandAssignmentData::GetPriorityItem( int nIndex )
{
	return m_listPriorities.GetItem(nIndex);
}

int CTowOffStandAssignmentData::GetPriorityCount()
{
	return (int)m_listPriorities.GetElementCount();
}

void CTowOffStandAssignmentData::ClearPrioirtyItem()
{
	m_listPriorities.CleanData();
}

ElapsedTime CTowOffStandAssignmentData::getFltDeplaneTime()
{
	return ElapsedTime(m_nDePlanementTime*60L);
}

ElapsedTime CTowOffStandAssignmentData::getFltEnplaneTime()
{
	return ElapsedTime(m_nEnPlanementTime*60L);
}

void CTowOffStandAssignmentData::SetParkStandsID( int val )
{
	m_nParkStandID = val;
	InitStandName();
}

void CTowOffStandAssignmentData::InitStandName()
{
	ALTObjectGroup altObjGroup;
	altObjGroup.ReadData(m_nParkStandID);
	m_ParkingStandName = altObjGroup.getName();
}

void CTowOffStandAssignmentData::SetPreferredTowoff( bool bPrefer )
{
	m_bPrefer = bPrefer;
}

bool CTowOffStandAssignmentData::GetPreferredTowoff() const
{
	return m_bPrefer;
}

PreferrdTowoffStandData* CTowOffStandAssignmentData::GetPreferredTowoffData()
{
	return &m_preferredTowoffData;
}

PreferTowoffFlightType::PreferTowoffFlightType()
:m_pAirportDatabase(NULL)
{

}

PreferTowoffFlightType::~PreferTowoffFlightType()
{

}

void PreferTowoffFlightType::SetFlightType( const FlightConstraint& fltType )
{
	m_fltType = fltType;
}

const FlightConstraint& PreferTowoffFlightType::GetFlightType() const
{
	return m_fltType;
}

CString PreferTowoffFlightType::GetTableName() const
{
	return _T("PREFER_TOWOFF_FLIGHTTYPE");
}

void PreferTowoffFlightType::GetDataFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("FLIGHTYPE"),m_strFlightType);
	if (m_pAirportDatabase)
	{
		m_fltType.SetAirportDB(m_pAirportDatabase);
		m_fltType.setConstraintWithVersion(m_strFlightType);
	}
}

bool PreferTowoffFlightType::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	CString strFlightType;
	m_fltType.screenPrint(strFlightType);
	
	strSQL.Format(_T("INSERT INTO PREFER_TOWOFF_FLIGHTTYPE (PARENTID,FLIGHTYPE) VALUES (%d,'%s')"),nParentID,strFlightType);
	return true;
}

bool PreferTowoffFlightType::GetUpdateSQL( CString& strSQL ) const
{
	CString strFlightType;
	m_fltType.screenPrint(strFlightType);
	strSQL.Format(_T("UPDATE PREFER_TOWOFF_FLIGHTTYPE SET FLIGHTYPE = '%s' WHERE ID = %d"),strFlightType,m_nID);
	return true;
}

void PreferTowoffFlightType::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM PREFER_TOWOFF_FLIGHTTYPE WHERE ID = %d"),m_nID);
}

void PreferTowoffFlightType::GetSelectSQL( int nID,CString& strSQL ) const
{
	ASSERT(FALSE);
}

void PreferTowoffFlightType::SetAirportDatabase( CAirportDatabase* pAirportDatabase )
{
	m_pAirportDatabase = pAirportDatabase;
	if (m_pAirportDatabase)
	{
		m_fltType.SetAirportDB(m_pAirportDatabase);
		if (FALSE == m_strFlightType.IsEmpty())
		{
			m_fltType.setConstraintWithVersion(m_strFlightType);
		}
	}
}

const CString& PreferTowoffFlightType::GetFlightTypeString() const
{
	return m_strFlightType;
}

PreferrdTowoffStandData::PreferrdTowoffStandData()
:m_nID(-1)
,m_bTimeWindow(true)
,m_nOperation(0)
,m_nMins(0)
{

}

PreferrdTowoffStandData::~PreferrdTowoffStandData()
{

}

void PreferrdTowoffStandData::UseTimeRange( bool bUse )
{
	m_bTimeWindow = bUse;
}

bool PreferrdTowoffStandData::WhetherUseTimeRange() const
{
	return m_bTimeWindow;
}

void PreferrdTowoffStandData::SetTimeRange( const ElapsedTime& eStart,const ElapsedTime& eEnd )
{
	m_timeRange.SetStartTime(eStart);
	m_timeRange.SetEndTime(eEnd);
}

const TimeRange& PreferrdTowoffStandData::GetTimeRange() const
{
	return m_timeRange;
}

void PreferrdTowoffStandData::SetOperationNum( int nNum )
{
	m_nOperation = nNum;
}	

int PreferrdTowoffStandData::GetOperationNum() const
{
	return m_nOperation;
}

void PreferrdTowoffStandData::SetTimeMins( int nMins )
{
	m_nMins = nMins;
}

int PreferrdTowoffStandData::GetTimeMins() const
{
	return m_nMins;
}

void PreferrdTowoffStandData::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM PREFER_TOWOFF_FLIGHTTYPE WHERE PARENTID = %d"),nParentID);
}

void PreferrdTowoffStandData::ReadData( int nParentID )
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM PREFER_TOWOFF_PROPERTY WHERE PARENTID = %d"),nParentID);

	CADORecordset rs;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,rs);

	if (!rs.IsEOF())
	{
		rs.GetFieldValue(_T("ID"),m_nID);
		int nUse = 0;
		rs.GetFieldValue(_T("USETIMERANGE"),nUse);
		m_bTimeWindow = nUse ? true : false;
		CString strTimeRange;
		rs.GetFieldValue(_T("TIMERANGE"),strTimeRange);
		m_timeRange.ParseDatabaseString(strTimeRange);
		rs.GetFieldValue(_T("OPERATION"),m_nOperation);
		rs.GetFieldValue(_T("TIMEMINS"),m_nMins);
	
		CDBDataCollection<PreferTowoffFlightType>::ReadData(m_nID);
	}
}

void PreferrdTowoffStandData::SaveData( int nParentID )
{
	CString strSQL;
	int nUse = m_bTimeWindow ? 1 : 0;
	if (m_nID < 0)
	{
		strSQL.Format(_T("INSERT INTO PREFER_TOWOFF_PROPERTY(PARENTID,USETIMERANGE,TIMERANGE,\
						 OPERATION,TIMEMINS) VALUES (%d,%d,'%s',%d,%d)"),nParentID,nUse,m_timeRange.MakeDatabaseString(),
						 m_nOperation,m_nMins);

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		strSQL.Format(_T("UPDATE PREFER_TOWOFF_PROPERTY SET USETIMERANGE = %d,TIMERANGE = '%s',\
						 OPERATION = %d,TIMEMINS = %d WHERE ID = %d"),nUse,m_timeRange.MakeDatabaseString(),
						 m_nOperation,m_nMins,m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
	
	CDBDataCollection<PreferTowoffFlightType>::SaveData(m_nID);
}

CString PreferrdTowoffStandData::GetTimeRangeString() const
{
	CString strText;
	m_timeRange.GetString(strText);
	return strText;
}
