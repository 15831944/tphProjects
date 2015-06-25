#include "StdAfx.h"
#include "GateAssignPreferenceMan.h"
#include "Inputs\GateAssignmentMgr.h"
#include "Inputs\TerminalGateAssignmentMgr.h"
#include "in_term.h"

bool ComparePreference(CGateAssignPreferenceItem* pItem1,CGateAssignPreferenceItem* pItem2)
{
	return *pItem1 < *pItem2;
}

CGateOverLapFlightTY::CGateOverLapFlightTY():m_ID(-1)
{

}
CGateOverLapFlightTY::~CGateOverLapFlightTY()
{
ClearData() ;
}
void CGateOverLapFlightTY::ClearData() 
{
for ( int i = 0 ; i < (int)m_OverLap.size() ;i++)
{
	delete m_OverLap[i] ;
}
m_OverLap.clear() ;
}
void CGateOverLapFlightTY::readData(int _parID,InputTerminal* _Terminal)
{
	if(_parID == -1)
		return ;
	ClearData() ;

	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_GATE_ASSIGN_PREFERENCE_OVERLAP WHERE PREFERENCE_ITEM_ID = %d"),_parID) ;
	CADORecordset _dataset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,_dataset) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	FlightConstraint flghtTY(_Terminal->m_pAirportDB);
	CString flightType ;
	CGateOverLapFlightTYItem* Item = NULL ;
	while(!_dataset.IsEOF())
	{
		_dataset.GetFieldValue(_T("FLIGHT_TYPE"),flightType) ;
		flghtTY.setConstraintWithVersion(flightType) ;
		Item = new CGateOverLapFlightTYItem(flghtTY) ;
		m_OverLap.push_back(Item) ;

		_dataset.MoveNextData() ;
	}
}
void CGateOverLapFlightTY::WriteData(int _parID)
{
	if(_parID == -1 )
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_GATE_ASSIGN_PREFERENCE_OVERLAP WHERE PREFERENCE_ITEM_ID = %d"),_parID) ;
	CADODatabase::ExecuteSQLStatement(SQL) ;

	std::vector<CGateOverLapFlightTYItem*> ::iterator iter = m_OverLap.begin() ;
	TCHAR th[1024] = { 0} ;
	for ( ; iter != m_OverLap.end() ;iter++)
	{
		(*iter)->GetFlightTy().WriteSyntaxStringWithVersion(th) ;
		SQL.Format(_T("INSERT INTO TB_GATE_ASSIGN_PREFERENCE_OVERLAP (FLIGHT_TYPE,PREFERENCE_ITEM_ID) VALUES('%s',%d)"),th,_parID) ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
}
CGateOverLapFlightTYItem* CGateOverLapFlightTY::AddFlightTy(FlightConstraint& _flightTy)
{
	std::vector<CGateOverLapFlightTYItem*>::iterator iter = m_OverLap.begin() ;
	for ( ; iter != m_OverLap.end() ;iter++)
	{
		if((*iter)->GetFlightTy().isEqual(&_flightTy))
			return NULL;
	}
	CGateOverLapFlightTYItem* flighttyitem = new CGateOverLapFlightTYItem(_flightTy) ;
	m_OverLap.push_back(flighttyitem) ;
	return flighttyitem ;
}
void CGateOverLapFlightTY::DelFlightTy(CGateOverLapFlightTYItem* _Item)
{
	std::vector<CGateOverLapFlightTYItem*>::iterator iter = m_OverLap.begin() ;
	for ( ; iter != m_OverLap.end() ;iter++)
	{
		if((*iter) == _Item )
		{
			delete *iter ;
			m_OverLap.erase(iter) ;
			return ;
		}
	}
}
BOOL CGateOverLapFlightTY::CheckTheFlightTYExist(FlightConstraint& _flightTy)
{
	std::vector<CGateOverLapFlightTYItem*>::iterator iter = m_OverLap.begin() ;
	for ( ; iter != m_OverLap.end() ;iter++)
	{
		if((*iter)->GetFlightTy().fits(_flightTy))
		{
			return TRUE;
		}
	}
	return FALSE ;
}
//////////////////////////////////////////////////////////////////////////

CGatePreferenceSubItem::CGatePreferenceSubItem(FlightConstraint& _Flight):m_Flight(_Flight),m_Duration(30),m_ID(-1)
{
}
CGatePreferenceSubItem::~CGatePreferenceSubItem()
{

}
void CGatePreferenceSubItem::ReadData(std::vector<CGatePreferenceSubItem*>& _DataSet , int _ParID,InputTerminal* _Terminal)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_GATE_ASSIGN_PREFERENCE_ITEM WHERE PREFERENCE_ID = %d"),_ParID) ;

	CADORecordset dataset ;
	long count ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataset) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CGatePreferenceSubItem* SubItem = NULL ;
	FlightConstraint flitcon(_Terminal->m_pAirportDB) ;
	CString flightty ;
	int id ;
	long time ;
	while(!dataset.IsEOF())
	{
		dataset.GetFieldValue(_T("FLIGHT_TYPE"),flightty) ;
		flitcon.setConstraintWithVersion(flightty) ;
		dataset.GetFieldValue(_T("ID"),id) ;
        dataset.GetFieldValue(_T("DURATION"),time) ;
		SubItem = new CGatePreferenceSubItem(flitcon) ;
		SubItem->SetDataBaseIndex(id) ;
		SubItem->SetDuration(time) ;
		SubItem->GetOverLapFlightTY()->readData(id,_Terminal) ;
		_DataSet.push_back(SubItem) ;
		dataset.MoveNextData() ;
	}
}
void CGatePreferenceSubItem::WriteData(std::vector<CGatePreferenceSubItem*> _DataSet , int _ParID)
{
	std::vector<CGatePreferenceSubItem*>::iterator iter = _DataSet.begin() ;
	for (  ; iter != _DataSet.end() ;iter++)
	{
		CGatePreferenceSubItem::WriteDataItem(*iter,_ParID) ;
	}
}
void CGatePreferenceSubItem::DeleteData(std::vector<CGatePreferenceSubItem*> _DataSet)
{
	std::vector<CGatePreferenceSubItem*>::iterator iter = _DataSet.begin() ;
	for (  ; iter != _DataSet.end() ;iter++)
	{
		CGatePreferenceSubItem::DeleteDataItem(*iter) ;
	}
}
void CGatePreferenceSubItem::WriteDataItem(CGatePreferenceSubItem* _DataSet , int _ParID)
{
	if(_DataSet == NULL || _ParID == -1)
		return ;
	CString SQL ;
	TCHAR th[1024] = {0} ;
	int id = -1 ;
	_DataSet->GetFlightTY().WriteSyntaxStringWithVersion(th) ;
	if(_DataSet->GetDataBaseIndex() == -1)
	{

		SQL.Format(_T("INSERT INTO TB_GATE_ASSIGN_PREFERENCE_ITEM (FLIGHT_TYPE,DURATION,PREFERENCE_ID) VALUES('%s',%d,%d)"),th,_DataSet->GetDuration(),_ParID) ;
		id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
		_DataSet->SetDataBaseIndex(id) ;
	}else
	{
		SQL.Format(_T("UPDATE TB_GATE_ASSIGN_PREFERENCE_ITEM SET FLIGHT_TYPE = '%s',DURATION = %d ,PREFERENCE_ID = %d WHERE ID = %d") ,th,_DataSet->GetDuration(),_ParID,_DataSet->GetDataBaseIndex()) ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	_DataSet->GetOverLapFlightTY()->WriteData(_DataSet->GetDataBaseIndex()) ;
}
void CGatePreferenceSubItem::DeleteDataItem(CGatePreferenceSubItem* _DataSet)
{
	if(_DataSet == NULL || _DataSet->GetDataBaseIndex() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_GATE_ASSIGN_PREFERENCE_ITEM WHERE ID = %d ") ,_DataSet->GetDataBaseIndex()) ;
	CADODatabase::ExecuteSQLStatement(SQL) ;
}

//////////////////////////////////////////////////////////////////////////

CGatePreferenceSubItem* CGateAssignPreferenceItem::AddItemData(FlightConstraint& _flightTy)
{
	CGatePreferenceSubItem* subiten = FindGatePreferenceSubItem(_flightTy) ;
	if(subiten != NULL)
		return NULL;
	subiten = new CGatePreferenceSubItem(_flightTy) ;
	m_SubItems.push_back(subiten) ;
	return subiten ;
}
void CGateAssignPreferenceItem::DelItemData(CGatePreferenceSubItem* _ItemData)
{
	DATA_TY_ITER iter = std::find(m_SubItems.begin(),m_SubItems.end(),_ItemData) ;
	if(iter != m_SubItems.end())
	{
		m_DelItem.push_back(*iter) ;
		m_SubItems.erase(iter) ;
	}
}
void CGateAssignPreferenceItem::Cleardata()
{
	DATA_TY_ITER iter = m_SubItems.begin() ;
	for ( ; iter != m_SubItems.end() ;iter++)
	{
		delete *iter ;
	}
	m_SubItems.clear() ;
}
void CGateAssignPreferenceItem::ClearDelData()
{
	DATA_TY_ITER iter = m_DelItem.begin() ;
	for ( ; iter != m_DelItem.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelItem.clear() ;
}
CGateAssignPreferenceItem::CGateAssignPreferenceItem(ProcessorID _gate , ASSIGN_PREFERENCE_TYPE type):m_GateID(_gate),m_type(type),m_ID(-1)
{

}
CGateAssignPreferenceItem::~CGateAssignPreferenceItem()
{
	Cleardata() ;
	ClearDelData() ;
}
CGatePreferenceSubItem* CGateAssignPreferenceItem::FindGatePreferenceSubItem(FlightConstraint& _Flight)
{
	DATA_TY_ITER iter = m_SubItems.begin() ;
	for ( ; iter != m_SubItems.end() ;iter++)
	{
		if( ( *iter)->GetFlightTY().fits(_Flight))
			return *iter ;
	}
	return NULL ;
}
BOOL CGateAssignPreferenceItem::GetFlightDurationtime(FlightConstraint& _const,ElapsedTime& _DurationTime)
{
   CGatePreferenceSubItem* subItem = FindGatePreferenceSubItem(_const) ;
   if(subItem == NULL)
	   return FALSE ;
   _DurationTime.SetMinute(subItem->GetDuration()) ;
   return TRUE ;
}
void CGateAssignPreferenceItem::ReadData(std::vector<CGateAssignPreferenceItem*>& _dataSet ,ASSIGN_PREFERENCE_TYPE type ,InputTerminal* _Terminal )
{
	CString SQL ;

	SQL.Format(_T("SELECT * FROM TB_GATE_ASSIGN_PREFERENCE WHERE TYPE = %d") , type) ;

	CADORecordset dataset ;
	long count = 0 ;

	CADODatabase::ExecuteSQLStatement(SQL,count,dataset) ;
	ProcessorID id ;
	CString gate ;
	while (!dataset.IsEOF())
	{
		CGateAssignPreferenceItem* item = NULL ;
		CGateAssignPreferenceItem::ReadItemData(dataset,&item,_Terminal) ;

		CGatePreferenceSubItem::ReadData(*(item->GetSubItemData()),item->GetDataIndex(),_Terminal) ;
		_dataSet.push_back(item) ;
		dataset.MoveNextData() ;
	}
}
void CGateAssignPreferenceItem::WriteData(std::vector<CGateAssignPreferenceItem*> _dataSet)
{
	std::vector<CGateAssignPreferenceItem*>::iterator iter = _dataSet.begin() ;
	for ( ; iter != _dataSet.end() ;iter++)
	{
		CGateAssignPreferenceItem::WriteItemData(*iter) ;

	}
	
}
void CGateAssignPreferenceItem::ReadItemData(CADORecordset& _DBSet ,CGateAssignPreferenceItem** _PreferenceItem,InputTerminal* _Terminal)
{

   int id = -1 ;
	CString gateid ;
   _DBSet.GetFieldValue(_T("ID") ,id) ;
   _DBSet.GetFieldValue(_T("GATE_ID"),gateid) ;
   ProcessorID gateProc ;
	gateProc.SetStrDict(_Terminal->inStrDict) ;
	gateProc.setID(gateid) ;
	int type ;
	 _DBSet.GetFieldValue(_T("TYPE"),type) ;
	*_PreferenceItem = new CGateAssignPreferenceItem(gateProc,(ASSIGN_PREFERENCE_TYPE)type) ;
	(*_PreferenceItem)->SetDataIndex(id) ;
}
void CGateAssignPreferenceItem::WriteItemData(CGateAssignPreferenceItem* _PreferenceItem)
{
	if(_PreferenceItem == NULL)
		return ;
	int id = _PreferenceItem->GetDataIndex() ;
	CString SQL ;
	if(id == -1)
	{
		SQL.Format(_T("INSERT INTO TB_GATE_ASSIGN_PREFERENCE (GATE_ID,TYPE) VALUES('%s',%d)"),_PreferenceItem->GetGateID().GetIDString(),_PreferenceItem->GetType()) ;
		id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
		_PreferenceItem->SetDataIndex(id) ;
	}else
	{
		SQL.Format(_T("UPDATE TB_GATE_ASSIGN_PREFERENCE SET GATE_ID = '%s',TYPE = %d WHERE ID = %d"),_PreferenceItem->GetGateID().GetIDString(),_PreferenceItem->GetType(),id) ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	CGatePreferenceSubItem::WriteData(*_PreferenceItem->GetSubItemData(),id) ;
	CGatePreferenceSubItem::DeleteData(*_PreferenceItem->GetSubItemDelData()) ;
	_PreferenceItem->ClearDelData() ;
}
void CGateAssignPreferenceItem::DeleteData(std::vector<CGateAssignPreferenceItem*> _dataSet)
{
	std::vector<CGateAssignPreferenceItem*>::iterator iter = _dataSet.begin() ;
	for ( ; iter != _dataSet.end() ;iter++)
	{
		CGateAssignPreferenceItem::DeleteItemData(*iter) ;
	}
}
void CGateAssignPreferenceItem::DeleteItemData(CGateAssignPreferenceItem* _PreferenceItem)
{
	if(_PreferenceItem == NULL || _PreferenceItem->GetDataIndex() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_GATE_ASSIGN_PREFERENCE WHERE ID = %d") , _PreferenceItem->GetDataIndex()) ;

	CADODatabase::ExecuteSQLStatement(SQL) ;
}

bool CGateAssignPreferenceItem::operator<( const CGateAssignPreferenceItem& gateMap ) const
{
	if (m_GateID.idFits(gateMap.m_GateID))//gateMap is more detail than current object
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

CGateAssignPreferenceMan::CGateAssignPreferenceMan(CGateAssignmentMgr* _GateAssignMgr):m_GateAssignMgr(_GateAssignMgr)
{
    m_vAdjacency.clear();
}
CGateAssignPreferenceMan::~CGateAssignPreferenceMan()
{
	ClearData() ;
	ClearDelData() ;
}
void CGateAssignPreferenceMan::ReadData(InputTerminal* _Terminal)
{		
	ClearData() ;
	ClearDelData() ;
	try
	{
			CGateAssignPreferenceItem::ReadData(m_GateAssignPreference,m_type,_Terminal) ;
			std::sort(m_GateAssignPreference.begin(),m_GateAssignPreference.end(),ComparePreference);
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}

}

void CGateAssignPreferenceMan::SaveData()
{
	CGateAssignPreferenceItem::WriteData(m_GateAssignPreference) ;
	CGateAssignPreferenceItem::DeleteData(m_DelGateAssignPreference) ;
	ClearDelData() ;
}
void CGateAssignPreferenceMan::DelPreferenceItem(CGateAssignPreferenceItem* _Item)
{
	DATA_TYPE_ITER iter = std::find(m_GateAssignPreference.begin() ,m_GateAssignPreference.end(),_Item) ;
	if(iter != m_GateAssignPreference.end())
	{
		m_DelGateAssignPreference.push_back(*iter) ;
		m_GateAssignPreference.erase(iter) ;
	}
}
CGateAssignPreferenceItem* CGateAssignPreferenceMan::AddPreferenceItem(ProcessorID& _GateID)
{
	CGateAssignPreferenceItem* Item = FindItemByGateID(_GateID) ;
	if(Item != NULL)
		return NULL;
	Item = new CGateAssignPreferenceItem(_GateID,m_type) ;
	m_GateAssignPreference.push_back(Item) ;
	std::sort(m_GateAssignPreference.begin(),m_GateAssignPreference.end(),ComparePreference);
	return Item ;
}
CGateAssignPreferenceItem* CGateAssignPreferenceMan::FindItemByGateID(ProcessorID& _GateID)
{
	DATA_TYPE_ITER iter = m_GateAssignPreference.begin() ;
	for ( ; iter != m_GateAssignPreference.end() ;iter++)
	{
		CString gatename ;
		gatename = _GateID.GetIDString() ;
		ProcessorID _proID ;
		_proID = (*iter)->GetGateID() ;
		if(_proID.idFits(_GateID))
			return *iter ;
	}
	return NULL ;
}
void CGateAssignPreferenceMan::ClearData()
{
	DATA_TYPE_ITER iter = m_GateAssignPreference.begin() ;
	for ( ; iter != m_GateAssignPreference.end() ;iter++)
	{
		delete *iter ;
	}
	m_GateAssignPreference.clear() ;
}
void CGateAssignPreferenceMan::ClearDelData()
{
	DATA_TYPE_ITER iter = m_DelGateAssignPreference.begin() ;
	for ( ; iter != m_DelGateAssignPreference.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelGateAssignPreference.clear() ;
}

BOOL CGateAssignPreferenceMan::CheckTheGateByPreference( ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight , bool& bPrefer )
{
	BOOL res = FALSE ;
	if(pFlight == NULL)
		return TRUE ;

	std::sort(m_GateAssignPreference.begin(),m_GateAssignPreference.end(),ComparePreference);
	return	CheckTheGate(_GateID, pFlight, bPrefer);
}

char CGateAssignPreferenceMan::GetAssignMode() const
{
	if (m_type == CGateAssignPreferenceItem::Arrival_Preference)
	{
		return 'A';
	}

	return 'D';
}


bool CGateAssignPreferenceMan::FlightAvailableByPreference( const ProcessorID& gateID,CFlightOperationForGateAssign* pFlight)
{
	Flight* pInputFlight = (Flight*)pFlight->getFlight();

	//need sort result to find more detail gate
	std::sort(m_GateAssignPreference.begin(),m_GateAssignPreference.end(),ComparePreference);

	ProcessorID procID;
	procID = gateID;
	CGateAssignPreferenceItem* pItem = FindItemByGateID(procID);
	if(pItem == NULL || pItem->GetSubItemData()->empty())
		return false;

	char pMode = GetAssignMode();
	CGatePreferenceSubItem* pSubItem = pItem->FindGatePreferenceSubItem(pInputFlight->getType(pMode)) ;
	if(pSubItem == NULL)
		return false;

	return true;
}

int CGateAssignPreferenceMan::FindPreferenceIndex( CGateAssignPreferenceItem* pItem )
{
	DATA_TYPE_ITER iter = std::find(m_GateAssignPreference.begin() ,m_GateAssignPreference.end(),pItem);
	if(iter != m_GateAssignPreference.end())
	{
		return iter - m_GateAssignPreference.begin();
	}
	return -1;
}

//BOOL CArrivalGateAssignPreferenceMan::CheckTheGateByPreference( ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight  ,int _type)
//{
//	BOOL res = FALSE ;
//	if(pFlight ==  NULL)
//		return TRUE ;
//	if(_type == CStand2GateMapping::MapType_1to1)
//		res =	CheckTheGateByPreferenceForOneToOne(_GateID,pFlight) ;
//	else
//		res = CheckTheGateByPreferenceForRandom(_GateID,pFlight) ;
//	return res ;
//}
//BOOL CArrivalGateAssignPreferenceMan::CheckTheGateByPreferenceForOneToOne(ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight)
//{
//	Flight* _flightID = (Flight*)pFlight->getFlight();
//	CGateAssignPreferenceItem* Item = FindItemByGateID(_GateID) ;
//	if(Item == NULL|| Item->GetSubItemData()->empty())
//		return TRUE ;
//	CGatePreferenceSubItem* SubItem = Item->FindGatePreferenceSubItem(_flightID->getType('A')) ;
//	if(SubItem == NULL)
//		return TRUE ;
//
//	//check if this interval time ,the Arrival gate  has flight 
//	CGateOverLapFlightTY* Overlap = SubItem->GetOverLapFlightTY() ;
//	CAssignTerminalGate* AssignGate = (CAssignTerminalGate*)m_GateAssignMgr->GetGate(_GateID.GetIDString()) ;
//	if(AssignGate == NULL)
//		return TRUE ;
//	ElapsedTime startTime ;
//	ElapsedTime endtime ;
//	startTime = pFlight->GetStartTime() ;
//	ElapsedTime durtime ;
//	durtime.SetMinute(durtime);
//	endtime = startTime + durtime;
//	BOOL CurrentFlightTypeDefine = Overlap->CheckTheFlightTYExist(_flightID->getType('A')) ;
//	for (int i = 0 ; i< AssignGate->GetFlightCount() ;i++)
//	{
//		if(!Item->GetFlightDurationtime(((Flight*)AssignGate->GetFlight(i).getFlight())->getType('A'),durtime))
//			durtime = AssignGate->GetFlight(i).GetEndTime() - AssignGate->GetFlight(i).GetStartTime() ;
//		if(AssignGate->GetFlight(i).GetStartTime() < startTime && (AssignGate->GetFlight(i).GetStartTime() + durtime)<=startTime || AssignGate->GetFlight(i).GetStartTime()>= (startTime + durtime))
//			continue ;
//		SubItem = Item->FindGatePreferenceSubItem(((Flight*)AssignGate->GetFlight(i).getFlight())->getType('A')) ;
//		if(SubItem == NULL)
//			return TRUE ;
//		if(!SubItem->GetOverLapFlightTY()->CheckTheFlightTYExist(_flightID->getType('A')))
//				return FALSE ;
//	}
//	return TRUE ;
//}
//BOOL CArrivalGateAssignPreferenceMan::CheckTheGateByPreferenceForRandom(ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight)
//{
//
//}

BOOL CArrivalGateAssignPreferenceMan::CheckTheGate( ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight, bool& bPrefer )
{
	Flight* _flightID = (Flight*)pFlight->getFlight();
	CGateAssignPreferenceItem* Item = FindItemByGateID(_GateID) ;
	if(Item == NULL || Item->GetSubItemData()->empty())
		return TRUE ;
	CGatePreferenceSubItem* SubItem = Item->FindGatePreferenceSubItem(_flightID->getType('A')) ;
	if(SubItem == NULL)
		return TRUE ;
	CAssignTerminalGate* AssignGate = (CAssignTerminalGate*)m_GateAssignMgr->GetGate(_GateID.GetIDString()) ;
	if(AssignGate == NULL)
		return TRUE ;

	bPrefer = true;

	ElapsedTime duration ;
	duration.SetMinute(SubItem->GetDuration()) ;
	ElapsedTime startTime = pFlight->GetStartTime() ;
	ElapsedTime endtime = startTime + duration ;

	for (int i = 0 ; i< AssignGate->GetFlightCount() ;i++)
	{
		if(!Item->GetFlightDurationtime(((Flight*)AssignGate->GetFlight(i).getFlight())->getType('A'),duration))
			duration = AssignGate->GetFlight(i).GetEndTime() - AssignGate->GetFlight(i).GetStartTime() ;
		if(AssignGate->GetFlight(i).GetStartTime() < startTime && (AssignGate->GetFlight(i).GetStartTime() + duration) <= startTime ||
			AssignGate->GetFlight(i).GetStartTime()>= (startTime + duration))
			continue ;
		SubItem = Item->FindGatePreferenceSubItem(((Flight*)AssignGate->GetFlight(i).getFlight())->getType('A')) ;
		if(SubItem == NULL)
			return TRUE ;
		if(!SubItem->GetOverLapFlightTY()->CheckTheFlightTYExist(_flightID->getType('A')))
			return FALSE ;
	}
	return TRUE ;
}

//BOOL CDepGateAssignPreferenceMan::CheckTheGateByPreference( ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight, bool& bPrefer)
//{
//	BOOL res = FALSE ;
//	if(pFlight == NULL)
//		return TRUE ;
//
//	return	CheckTheGate(_GateID, pFlight, bPrefer);
//}
BOOL CDepGateAssignPreferenceMan::CheckTheGate(ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight, bool& bPrefer)
{
	Flight* _flightID = (Flight*)pFlight->getFlight();
	CGateAssignPreferenceItem* Item = FindItemByGateID(_GateID) ;
	if(Item == NULL || Item->GetSubItemData()->empty())
		return TRUE ;
	CGatePreferenceSubItem* SubItem = Item->FindGatePreferenceSubItem(_flightID->getType('D')) ;
	if(SubItem == NULL  )
		return TRUE ;

	//check if this interval time ,the Arrival gate  has flight 
	CAssignTerminalGate* AssignGate = (CAssignTerminalGate*)m_GateAssignMgr->GetGate(_GateID.GetIDString()) ;
	if(AssignGate == NULL)
		return TRUE ;

	//this gate is prefer this flight
	bPrefer = true;

	ElapsedTime startTime = pFlight->GetStartTime() ;

	ElapsedTime duration ;
	duration.SetMinute( SubItem->GetDuration()) ;

	ElapsedTime endtime = startTime + duration;

	for (int i = 0 ; i< AssignGate->GetFlightCount() ;i++)
	{
		if(!Item->GetFlightDurationtime(((Flight*)AssignGate->GetFlight(i).getFlight())->getType('D'),duration))
			duration = AssignGate->GetFlight(i).GetEndTime() - AssignGate->GetFlight(i).GetStartTime() ;
		if(AssignGate->GetFlight(i).GetStartTime() < startTime && (AssignGate->GetFlight(i).GetStartTime() + duration)<= startTime ||
			AssignGate->GetFlight(i).GetStartTime()>= (startTime + duration))
			continue ;
		SubItem = Item->FindGatePreferenceSubItem(((Flight*)AssignGate->GetFlight(i).getFlight())->getType('D')) ;
		if(SubItem == NULL)
            return TRUE ;
		if(!SubItem->GetOverLapFlightTY()->CheckTheFlightTYExist(_flightID->getType('D')))
				return FALSE ;
	}
	return TRUE ;
}


//BOOL CDepGateAssignPreferenceMan::CheckTheGateByPreferenceForRandom(ProcessorID& _GateID ,CFlightOperationForGateAssign* pFlight)
//{
//	Flight* _flightID = (Flight*)pFlight->getFlight();
//	CGateAssignPreferenceItem* Item = FindItemByGateID(_GateID) ;
//	if(Item == NULL || Item->GetSubItemData()->empty())
//		return TRUE ;
//	CGatePreferenceSubItem* SubItem = Item->FindGatePreferenceSubItem(_flightID->getType('D')) ;
//	if(SubItem == NULL)
//		return TRUE ;
//	CAssignTerminalGate* AssignGate = (CAssignTerminalGate*)m_GateAssignMgr->GetGate(_GateID.GetIDString()) ;
//	if(AssignGate == NULL)
//		return TRUE ;
//	ElapsedTime startTime ;
//	ElapsedTime endtime ;
//	ElapsedTime duration ;
//	duration.SetMinute(SubItem->GetDuration()) ;
//	startTime = pFlight->GetStartTime() ;
//	endtime = startTime + duration ;
//
//	for (int i = 0 ; i< AssignGate->GetFlightCount() ;i++)
//	{
//		if(!Item->GetFlightDurationtime(((Flight*)AssignGate->GetFlight(i).getFlight())->getType('D'),duration))
//			duration = AssignGate->GetFlight(i).GetEndTime() - AssignGate->GetFlight(i).GetStartTime() ;
//		if(AssignGate->GetFlight(i).GetStartTime() < startTime && (AssignGate->GetFlight(i).GetStartTime() + duration) <= startTime ||
//			AssignGate->GetFlight(i).GetStartTime()>= (startTime + duration))
//		   continue ;
//		SubItem = Item->FindGatePreferenceSubItem(((Flight*)AssignGate->GetFlight(i).getFlight())->getType('D')) ;
//		if(SubItem == NULL)
//			return TRUE ;
//		if(!SubItem->GetOverLapFlightTY()->CheckTheFlightTYExist(_flightID->getType('D')))
//				return FALSE ;
//	}
//	return TRUE ;
//}