#include "stdafx.h"
#include "ApproachSeparationCirteria.h"
#include "InputAirside.h"
#include "ALTAirport.h"
#include "runway.h"
#include "..\Database\DBElementCollection_Impl.h"


CApproachSeparationCriteria::CApproachSeparationCriteria(int nProjID,const ALTObjectList& vRunways)
:m_nProjID(nProjID)
,m_runwaySeparationRelation(nProjID,vRunways)
{
	m_vrApproachTypeList.push_back(new CApproachLindingBehindLandedList(nProjID));
	m_vrApproachTypeList.push_back(new CApproachLindingBehindTakeOffList(nProjID));
}

CApproachSeparationCriteria::~CApproachSeparationCriteria(void)
{
	for (std::vector<CApproachTypeList*>::iterator itrItem = m_vrApproachTypeList.begin();\
		itrItem != m_vrApproachTypeList.end();++itrItem) {
			if (*itrItem) 
				delete (*itrItem);
		}
		m_vrApproachTypeList.clear();
}

void CApproachSeparationCriteria::ReadData(void)
{
	m_runwaySeparationRelation.ReadData();
	m_OperatorLog.Append(_T("Class CApproachSeparationCriteria::ReadData + ")) ;
	for (std::vector<CApproachTypeList*>::iterator itrItem = m_vrApproachTypeList.begin();itrItem != m_vrApproachTypeList.end();++itrItem) {
		if (*itrItem) 
			(*itrItem)->ReadData(m_OperatorLog);
	}
}

void CApproachSeparationCriteria::SaveData(void)
{
	m_runwaySeparationRelation.SaveData(m_nProjID);
	m_OperatorLog.Append(_T("Class CApproachSeparationCriteria::SaveData + ")) ;
	for (std::vector<CApproachTypeList*>::iterator itrItem = m_vrApproachTypeList.begin();itrItem != m_vrApproachTypeList.end();++itrItem) {
		if (*itrItem)
			(*itrItem)->SaveData(m_OperatorLog);
	}
}

void CApproachSeparationCriteria::ExportData(CAirsideExportFile& exportFile)
{
	m_runwaySeparationRelation.ExportData(exportFile);
	m_OperatorLog.Append(_T("Class CApproachSeparationCriteria::ExportData + ")) ;
	for (std::vector<CApproachTypeList*>::iterator itrItem = m_vrApproachTypeList.begin();itrItem != m_vrApproachTypeList.end();++itrItem) {
		if (*itrItem)
			(*itrItem)->ExportData(exportFile,m_OperatorLog);
	}
}

void CApproachSeparationCriteria::ImportData(CAirsideImportFile& importFile)
{
	m_runwaySeparationRelation.ImportData(importFile);
	m_OperatorLog.Append(_T("Class CApproachSeparationCriteria::ImportData + ")) ;
	for (std::vector<CApproachTypeList*>::iterator itrItem = m_vrApproachTypeList.begin();itrItem != m_vrApproachTypeList.end();++itrItem) {
		if (*itrItem)
			(*itrItem)->ImportData(importFile,m_OperatorLog);
	}
}

void CApproachSeparationCriteria::ExportApproachSeparationCriteria(CAirsideExportFile& exportFile)
{

	//exportFile.getFile().writeField("ApproachSeparationCriteria");
	//exportFile.getFile().writeLine();
	//CApproachSeparationCriteria approachSeparationCriteria(exportFile.GetProjectID());
	//approachSeparationCriteria.m_OperatorLog.Append(_T("Class CApproachSeparationCriteria::ImportData + ")) ;
	//approachSeparationCriteria.ReadData();
	//approachSeparationCriteria.ExportData(exportFile);

	//exportFile.getFile().endFile();
}

void CApproachSeparationCriteria::ImportApproachSeparationCriteria(CAirsideImportFile& importFile)
{
	//if(importFile.getVersion() <= 1033)
	//	return;
	//
	//CApproachSeparationCriteria approachSeparationCriteria(importFile.getNewProjectID());
	//approachSeparationCriteria.m_OperatorLog.Append(_T("Class CApproachSeparationCriteria::ImportApproachSeparationCriteria + ")) ;
	//approachSeparationCriteria.ImportData(importFile);
	//try
	//{
	//	CADODatabase::BeginTransaction() ;
	//	approachSeparationCriteria.SaveData();
	//	CADODatabase::CommitTransaction() ;
	//}
	//catch (CADOException e)
	//{
	//	CADODatabase::RollBackTransation() ;
	//}
	
}
CApproachLindingBehindLandedList *CApproachSeparationCriteria::GetLindingBehindLandedList()
{
	return (CApproachLindingBehindLandedList *)m_vrApproachTypeList.at(0);

}
CApproachLindingBehindTakeOffList *CApproachSeparationCriteria::GetLindingBehindTakeOffList()
{
	return (CApproachLindingBehindTakeOffList *)m_vrApproachTypeList.at(1);
}
//////////////////////////////////////////////////////////////////////////
//
CApproachTypeList::CApproachTypeList(int nProjID,AIRCRAFTOPERATIONTYPE emApproachType):m_nProjID(nProjID),m_emApproachType(emApproachType)
{
}

CApproachTypeList::~CApproachTypeList(void)
{
	Clear();
}

void CApproachTypeList::Clear(void)
{
	std::vector<CApproachRunwayNode * >::iterator itrItem ;
	for(itrItem = m_vrNode.begin();itrItem != m_vrNode.end();++itrItem){
		if(*itrItem)
			delete (*itrItem);
	}
	m_vrNode.clear();

	for(itrItem = m_vrNodeDeleted.begin();itrItem != m_vrNodeDeleted.end();++itrItem){
		if(*itrItem)
			delete (*itrItem);
	}
	m_vrNodeDeleted.clear();
}

int CApproachTypeList::GetProjectID(void)
{
	return (m_nProjID);
}

void CApproachTypeList::ReadData(CString _logmsg)
{
	CString log ;
	log.Format(_T("CApproachTypeList::ReadData ")) ;
	m_log.Append(log);

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM APPROACHRUNWAY WHERE (PROJID = %d AND APPROACHTYPE = %d);"),m_nProjID,(int)m_emApproachType);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	Clear();
	Runway runway1,runway2;
	if(adoRecordset.IsEOF()){
		std::vector<int> vAirportIds;
		InputAirside::GetAirportList(m_nProjID, vAirportIds);
		for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
		{
			ALTAirport airport;
			airport.ReadAirport(*iterAirportID);
			std::vector<int> vrRunwayIDs;
			vrRunwayIDs.clear();
			ALTAirport::GetRunwaysIDs(*iterAirportID,vrRunwayIDs);
			for (std::vector<int>::iterator itrRunwayID = vrRunwayIDs.begin();itrRunwayID != vrRunwayIDs.end();++itrRunwayID) {
				CApproachRunwayNode * pNewNode = new CApproachRunwayNode(m_nProjID,m_emApproachType);
				if (pNewNode) {
					pNewNode->m_nFirstRunwayID = *itrRunwayID;
					pNewNode->m_emFirstRunwayMark = RUNWAYMARK_FIRST;
					pNewNode->m_nSecondRunwayID = *itrRunwayID;
					pNewNode->m_emSecondRunwayMark = RUNWAYMARK_FIRST;
					if (pNewNode->m_nFirstRunwayID == pNewNode->m_nSecondRunwayID)
						pNewNode->m_emSepStd = SEPSTD_INTRAIL; 

					pNewNode->GetItemCompounding(pNewNode->m_emClassType);
					AddNode(pNewNode);
					pNewNode = 0;
				}
				pNewNode = new CApproachRunwayNode(m_nProjID,m_emApproachType);
				if (pNewNode) {
					pNewNode->m_nFirstRunwayID = *itrRunwayID;
					pNewNode->m_emFirstRunwayMark = RUNWAYMARK_SECOND;
					pNewNode->m_nSecondRunwayID = *itrRunwayID;
					pNewNode->m_emSecondRunwayMark = RUNWAYMARK_SECOND;
					if (pNewNode->m_nFirstRunwayID == pNewNode->m_nSecondRunwayID)
						pNewNode->m_emSepStd = SEPSTD_INTRAIL; 

					pNewNode->GetItemCompounding(pNewNode->m_emClassType);
					AddNode(pNewNode);
					pNewNode = 0;
				}
			}
		}
		m_log.Append(_T("CApproachTypeList::ReadData(no data)+ ")) ;
		
	}else 
	while (!adoRecordset.IsEOF()) {
		CApproachRunwayNode * pNewNode = 0;
		pNewNode = new CApproachRunwayNode(adoRecordset,_logmsg+m_log);
		if (pNewNode){
			runway1.ReadObject(pNewNode->m_nFirstRunwayID);
			runway2.ReadObject(pNewNode->m_nSecondRunwayID);
			if (runway1.GetPath().getCount() < 1 || runway2.GetPath().getCount() < 1)
				m_vrNodeDeleted.push_back(pNewNode);
			else
				m_vrNode.push_back(pNewNode);
		}			
		runway1.getPath().init(0,0);
		runway2.getPath().init(0,0);
		adoRecordset.MoveNextData();
		m_log.Append(_T("CApproachTypeList::ReadData( data)+ ")) ;
	}
}

void CApproachTypeList::SaveData(CString _logmsg)
{
	CString log ;
	log.Format(_T("CApproachTypeList::SaveData(count:%d )+ "),(int)m_vrNode.size()) ;
	m_log.Append(log);
	std::vector<CApproachRunwayNode*>::iterator itrItem;
	for (itrItem = m_vrNode.begin();itrItem != m_vrNode.end();++itrItem) {
		if(*itrItem)
			(*itrItem)->SaveData(m_nProjID,_logmsg+m_log);
	}

	for (itrItem = m_vrNodeDeleted.begin();itrItem != m_vrNodeDeleted.end();++itrItem) {
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrNodeDeleted.clear();
}

void CApproachTypeList::DeleteData(void)
{
	std::vector<CApproachRunwayNode*>::iterator itrItem;
	for (itrItem = m_vrNode.begin();itrItem != m_vrNode.end();++itrItem) {
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrNode.clear();

	for (itrItem = m_vrNodeDeleted.begin();itrItem != m_vrNodeDeleted.end();++itrItem) {
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrNodeDeleted.clear();
}

void CApproachTypeList::ExportData(CAirsideExportFile& exportFile,CString _logmsg)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt((int)m_emApproachType);

	CString log ;
	log.Format(_T("CApproachTypeList::ExportData(count:%d )+ "),(int)m_vrNode.size()) ;
	m_log.Append(log);

	exportFile.getFile().writeInt((int)m_vrNode.size());
	std::vector<CApproachRunwayNode*>::iterator itrItem;
	for (itrItem = m_vrNode.begin();itrItem != m_vrNode.end();++itrItem) {
		if(*itrItem)
			(*itrItem)->ExportData(exportFile,_logmsg + log);
	}

	exportFile.getFile().writeLine();
}

void CApproachTypeList::ImportData(CAirsideImportFile& importFile,CString _logmsg)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	int iValue = -1;
	importFile.getFile().getInteger(iValue);
	m_emApproachType = (AIRCRAFTOPERATIONTYPE)iValue;

	importFile.getFile().getInteger(iValue);
	Clear();
	Runway runway1,runway2;
	m_log.Append(_T("CApproachTypeList::ImportData"));
	for (int i = 0;i < iValue;++i) {
		CApproachRunwayNode * pNewNode = 0;
		pNewNode = new CApproachRunwayNode(m_nProjID,m_emApproachType);
		if (pNewNode){
			pNewNode->ImportData(importFile,_logmsg + m_log);
			runway1.ReadObject(pNewNode->m_nFirstRunwayID);
			runway2.ReadObject(pNewNode->m_nSecondRunwayID);
			if (runway1.GetPath().getCount() < 1 || runway2.GetPath().getCount() < 1)
				m_vrNodeDeleted.push_back(pNewNode);
			else
				m_vrNode.push_back(pNewNode);
		}			
		runway1.getPath().init(0,0);
		runway2.getPath().init(0,0);
	}


	importFile.getFile().getLine();
}

int CApproachTypeList::GetNodeCount(void)
{
	return ((int)m_vrNode.size());
}

CApproachRunwayNode* CApproachTypeList::GetNode(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_vrNode.size())
		return (0);
	else
		return (m_vrNode.at(nIndex));
}

CApproachRunwayNode* CApproachTypeList::GetNode(int nTrailRunwayID, RUNWAY_MARK TrailRunWayMark, int nLeadRunwayID, RUNWAY_MARK LeadRunWayMark)
{
	for (int i=0; i<GetNodeCount(); i++)
	{
		CApproachRunwayNode* pNode = GetNode(i);
		ASSERT(NULL != pNode);
		if(!pNode)
			continue;

		if (pNode->m_nFirstRunwayID == nTrailRunwayID
			&& pNode->m_emFirstRunwayMark == TrailRunWayMark
			&& pNode->m_nSecondRunwayID == nLeadRunwayID
			&& pNode->m_emSecondRunwayMark == LeadRunWayMark)
		{
			return pNode;
		}
	}

	return NULL;
}
void CApproachTypeList::GetRelatedLogicRunway(int nRunwayID, RUNWAY_MARK runWayMark,std::vector<int > vRunwayConcerned,std::vector<std::pair<int,RUNWAY_MARK> >& vRunway )
{


	if (vRunwayConcerned.size() == 0)
		return;

	for (int i=0; i<GetNodeCount(); i++)
	{
		CApproachRunwayNode* pNode = GetNode(i);
		ASSERT(NULL != pNode);
		if(!pNode)
			continue;

		if (pNode->m_nFirstRunwayID == nRunwayID
			&& pNode->m_emFirstRunwayMark == runWayMark)

		{
			int nRunwayCount = (int)vRunwayConcerned.size();
			BOOL bFind = FALSE;
			for (int i = 0; i < nRunwayCount; ++ i)
			{
				if (pNode->m_nSecondRunwayID == vRunwayConcerned[i])
				{
					bFind = TRUE;
					break;
				}
			}
			if (bFind == FALSE)
				vRunway.push_back(std::make_pair(pNode->m_nSecondRunwayID,pNode->m_emSecondRunwayMark));
		}
	}

}
void CApproachTypeList::DeleteNode(CApproachRunwayNode* pNodeDelete)
{
	std::vector<CApproachRunwayNode*>::iterator itrItemDelete;
	itrItemDelete = std::find(m_vrNode.begin(),m_vrNode.end(),pNodeDelete);
	if(itrItemDelete != m_vrNode.end()){
		m_vrNodeDeleted.push_back(*itrItemDelete);
		m_vrNode.erase(itrItemDelete);
	}
}

bool CApproachTypeList::AddNode(CApproachRunwayNode* pNewNode)
{
	if(!pNewNode)return (false);
	std::vector<CApproachRunwayNode*>::iterator itrItemFind;
	itrItemFind = std::find(m_vrNode.begin(),m_vrNode.end(),pNewNode);
	if(itrItemFind == m_vrNode.end())
		m_vrNode.push_back(pNewNode);
	return (true);
}

CApproachLindingBehindLandedList::CApproachLindingBehindLandedList(int nProjID):CApproachTypeList(nProjID,AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
{
}

CApproachLindingBehindLandedList::~CApproachLindingBehindLandedList(void)
{
}

CApproachLindingBehindTakeOffList::CApproachLindingBehindTakeOffList(int nProjID):CApproachTypeList(nProjID,AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF)
{
}

CApproachLindingBehindTakeOffList::~CApproachLindingBehindTakeOffList(void)
{
}

CApproachRunwayNode::CApproachRunwayNode(int nProjID,AIRCRAFTOPERATIONTYPE emApproachType)
{
	m_nID = -1;
	m_nProjID = nProjID;
	m_nFirstRunwayID = -1;
	m_emFirstRunwayMark = RUNWAYMARK_FIRST;
	m_nSecondRunwayID = -1;
	m_emSecondRunwayMark = RUNWAYMARK_FIRST;
	m_emApproachType = emApproachType;
	m_emClassType = WakeVortexWightBased;
	m_emSepStd = SEPSTD_NONE;
	m_bLAHSO = false;

	m_emART = RRT_SAMERUNWAY;
	m_emAIT = RIT_NONE;
	m_dRw1ThresholdFromInterNode = -1.0;
	m_dRw2ThresholdFromInterNode = -1.0;
}

CApproachRunwayNode::CApproachRunwayNode(CADORecordset &adoRecordset,CString _logmsg)
{
	if (!adoRecordset.IsEOF()) {
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		int iValue = -1;
		adoRecordset.GetFieldValue(_T("APPROACHTYPE"),iValue);
		m_emApproachType = (AIRCRAFTOPERATIONTYPE)iValue;
		adoRecordset.GetFieldValue(_T("FIRRWID"),m_nFirstRunwayID);
		adoRecordset.GetFieldValue(_T("FIRRWMARK"),iValue);
		m_emFirstRunwayMark = (RUNWAY_MARK)iValue;
		adoRecordset.GetFieldValue(_T("SECRWID"),m_nSecondRunwayID);
		adoRecordset.GetFieldValue(_T("SECRWMARK"),iValue);
		m_emSecondRunwayMark = (RUNWAY_MARK)iValue;

		adoRecordset.GetFieldValue(_T("CLASSTYPE"),iValue);
		m_emClassType = (FlightClassificationBasisType)iValue;

		adoRecordset.GetFieldValue(_T("SEPSTD"),iValue);
		m_emSepStd = (SEPSTD)iValue;

		adoRecordset.GetFieldValue(_T("LAHSO"),iValue);
		m_bLAHSO = iValue?true:false;

		CString strSQL = _T("");
		int AppSepType = 0;
		strSQL.Format(_T("SELECT * FROM OPERATIONCRITERIA WHERE (PARENTID = %d AND TAKEOFFAPPSEPTYPE = %d);"),m_nID,AppSepType);
		long nRecordCount = -1;
		CADORecordset adoRecordsetChild;
		CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetChild);

		if (adoRecordsetChild.IsEOF()) {
			strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE (PARENTID = %d AND TAKEOFFAPPSEPTYPE IS NULL);"),m_nID);
			CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetChild);
			GetItemCompounding(m_emClassType);
			return;
		}
		bool bAdd = true;
		AircraftClassifications ClasNone(m_nProjID, NoneBased);
		ClasNone.ReadData();

		AircraftClassifications ClasWakeVortexWightBased(m_nProjID, WakeVortexWightBased);
		ClasWakeVortexWightBased.ReadData();

		AircraftClassifications ClasWingspanBased(m_nProjID, WingspanBased);
		ClasWingspanBased.ReadData();

		AircraftClassifications ClasSurfaceBearingWeightBased(m_nProjID, SurfaceBearingWeightBased);
		ClasSurfaceBearingWeightBased.ReadData();

		AircraftClassifications ClasApproachSpeedBased(m_nProjID, ApproachSpeedBased);
		ClasApproachSpeedBased.ReadData();
		while (!adoRecordsetChild.IsEOF()) {
			CApproachSeparationItem * pNewItem = new CApproachSeparationItem(adoRecordsetChild);
			if (pNewItem) 
			{
				switch(pNewItem->m_emClassType)
				{
				case NoneBased:
					if(!ClasNone.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasNone.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				case WakeVortexWightBased:
					if(!ClasWakeVortexWightBased.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasWakeVortexWightBased.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				case WingspanBased:
					if(!ClasWingspanBased.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasWingspanBased.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				case SurfaceBearingWeightBased:
					if(!ClasSurfaceBearingWeightBased.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasSurfaceBearingWeightBased.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				case ApproachSpeedBased:
					if(!ClasApproachSpeedBased.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasApproachSpeedBased.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				default:
					ASSERT(0);
					break;
				}
				
				if(adoRecordset.GetFieldValue(_T("CLASSTYPE"),iValue))
				{
					if (bAdd)
						m_vrAppSep.push_back(pNewItem);
					else
						m_vrAppSepDeleted.push_back(pNewItem);
				}
			
				bAdd = true;
				adoRecordsetChild.MoveNextData();
			}
		}
		MakeDataValid(m_emClassType);
 		//if(!CheckTheItemCountAvailability())
 		//{
 		//	strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE PARENTID = %d AND TAKEOFFAPPSEPTYPE = %d AND CLASSTYPE = %d"),m_nID,AppSepType,m_emClassType) ;
 		//	CADODatabase::ExecuteSQLStatement(strSQL);
 		//	CARCportDatabaseConnection::GetInstance().writeLog(_logmsg + m_log) ;
 		//	RemoveClassType(m_emClassType);
 		//	GetItemCompounding(m_emClassType) ;
 		//}

	}

	m_emART = (RunwayRelationType)-1;
	m_emAIT = (RunwayIntersectionType)-1;
	m_dRw1ThresholdFromInterNode = -1.0;
	m_dRw2ThresholdFromInterNode = -1.0;
}

CApproachRunwayNode::~CApproachRunwayNode(void)
{
	Clear();
}

void CApproachRunwayNode::Clear(void)
{
	std::vector <CApproachSeparationItem *>::iterator itrItem;
	for(itrItem = m_vrAppSep.begin();itrItem != m_vrAppSep.end();++itrItem){
		if(*itrItem)
			delete (*itrItem);
	}
	m_vrAppSep.clear();

	for (itrItem = m_vrAppSepDeleted.begin();itrItem != m_vrAppSepDeleted.end();++itrItem) {
		if(*itrItem)
			delete (*itrItem);
	}
	m_vrAppSepDeleted.clear();
}

void CApproachRunwayNode::SaveData(int nProjID,CString _logmsg)
{
	CString log ;
	log.Format(_T("CApproachRunwayNode::SaveData(count:%d )+ "),(int)m_vrAppSep.size()) ;
	m_log.Append(log);

	CString strSQL = _T("");
	if(m_nID < 0){
		strSQL.Format(_T("INSERT INTO APPROACHRUNWAY (PROJID,APPROACHTYPE,FIRRWID,FIRRWMARK ,SECRWID,SECRWMARK,CLASSTYPE,SEPSTD,LAHSO) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d);"),m_nProjID = nProjID,(int)m_emApproachType,m_nFirstRunwayID,(int)m_emFirstRunwayMark ,m_nSecondRunwayID,(int)m_emSecondRunwayMark ,(int)m_emClassType,(int)m_emSepStd,(int)m_bLAHSO);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}else{
		strSQL.Format(_T("UPDATE APPROACHRUNWAY SET PROJID = %d , APPROACHTYPE = %d ,FIRRWID = %d ,FIRRWMARK = %d ,SECRWID = %d ,SECRWMARK = %d ,CLASSTYPE = %d ,SEPSTD = %d ,LAHSO = %d WHERE (ID = %d); "),m_nProjID = nProjID,(int)m_emApproachType,m_nFirstRunwayID,(int)m_emFirstRunwayMark ,m_nSecondRunwayID,(int)m_emSecondRunwayMark ,(int)m_emClassType,(int)m_emSepStd,(int)m_bLAHSO,m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	std::vector <CApproachSeparationItem *>::iterator itrItem;
	for(itrItem = m_vrAppSep.begin();itrItem != m_vrAppSep.end();++itrItem){
		if(*itrItem)
			(*itrItem)->SaveData(m_nID);
	}

	for (itrItem = m_vrAppSepDeleted.begin();itrItem != m_vrAppSepDeleted.end();++itrItem) {
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrAppSepDeleted.clear();
}

void CApproachRunwayNode::DeleteData(void)
{
	std::vector <CApproachSeparationItem *>::iterator itrItem;
	for(itrItem = m_vrAppSep.begin();itrItem != m_vrAppSep.end();++itrItem){
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrAppSep.clear();

	for (itrItem = m_vrAppSepDeleted.begin();itrItem != m_vrAppSepDeleted.end();++itrItem) {
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrAppSepDeleted.clear();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM APPROACHRUNWAY WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}
BOOL CApproachRunwayNode::CheckTheItemCountAvailability()
{
	AircraftClassifications clsFltType(m_nProjID,m_emClassType);
	clsFltType.ReadData();
	int iSize = clsFltType.GetCount();

	int nClassTypeCount = 0;
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (GetItem(i)->m_emClassType == m_emClassType)
		{
			nClassTypeCount++;
		}
	}

	return nClassTypeCount == (iSize*iSize) ;
}


//if have data, must exit database
void CApproachRunwayNode::MakeDataValid( FlightClassificationBasisType emClassType )
{
	AircraftClassifications clsFltType(m_nProjID,emClassType);
	clsFltType.ReadData();
	int iSize = clsFltType.GetCount();

	//int nClassTypeCount = 0;
	//for (int nItem = 0; nItem < GetItemCount(); nItem++)
	//{
	//	if (GetItem(nItem)->m_emClassType == emClassType)
	//	{
	//		nClassTypeCount++;
	//	}
	//}
	//int nValidCount = iSize*iSize;

	//doesn't exsit data and push back default data
	for(int i = 0;i < iSize;i++)
	{
		for (int j = 0;j < iSize;j++)
		{
			if (GetItem(clsFltType.GetItem(j),clsFltType.GetItem(i)) == NULL)
			{
				CApproachSeparationItem * pNewItem = new CApproachSeparationItem;
				if (pNewItem) 
				{
					pNewItem->m_emClassType = emClassType;
					pNewItem->m_aciTrail = * (clsFltType.GetItem(j) );
					pNewItem->m_aciLead = * (clsFltType.GetItem(i) );
					int iPos = i*iSize + j;
					m_vrAppSep.insert(m_vrAppSep.begin() + iPos,pNewItem);
				}	
			}
		}
	}
	
	//exsit data valid
	std::vector<CApproachSeparationItem*> vInvalidData;
	for (int i = 0; i < GetItemCount(); i++)
	{
		CApproachSeparationItem* pItem = GetItem(i);
		if (pItem->m_emClassType != emClassType)
			continue;
		
		if(!clsFltType.GetItemByID(pItem->m_aciTrail.m_nID))
		{
			vInvalidData.push_back(pItem);
			continue;
		}

		if(!clsFltType.GetItemByID(pItem->m_aciLead.m_nID))
		{
			vInvalidData.push_back(pItem);
			continue;
		}
	}

	for (int i = 0; i < (int)vInvalidData.size(); i++)
	{
		DeleteItem(vInvalidData[i]);
	}
}


void CApproachRunwayNode::ReadData(int nID,CString _logmsg)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM APPROACHRUNWAY WHERE (ID = %d);"),nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	Clear();
	if (!adoRecordset.IsEOF()){
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		int iValue = -1;
		adoRecordset.GetFieldValue(_T("APPROACHTYPE"),iValue);
		m_emApproachType = (AIRCRAFTOPERATIONTYPE)iValue;
		adoRecordset.GetFieldValue(_T("FIRRWID"),m_nFirstRunwayID);
		adoRecordset.GetFieldValue(_T("FIRRWMARK"),iValue);
		m_emFirstRunwayMark = (RUNWAY_MARK)iValue;
		adoRecordset.GetFieldValue(_T("SECRWID"),m_nSecondRunwayID);
		adoRecordset.GetFieldValue(_T("SECRWMARK"),iValue);
		m_emSecondRunwayMark = (RUNWAY_MARK)iValue;

		adoRecordset.GetFieldValue(_T("CLASSTYPE"),iValue);
		m_emClassType = (FlightClassificationBasisType)iValue;

		adoRecordset.GetFieldValue(_T("SEPSTD"),iValue);
		m_emSepStd = (SEPSTD)iValue;

		adoRecordset.GetFieldValue(_T("LAHSO"),iValue);
		m_bLAHSO = iValue?true:false;
		
		int AppSepType = 0;
		strSQL.Format(_T("SELECT * FROM OPERATIONCRITERIA WHERE (PARENTID = %d AND TAKEOFFAPPSEPTYPE = %d);"),m_nID,AppSepType);
		CADORecordset adoRecordsetChild;
		CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetChild);

		if (adoRecordsetChild.IsEOF()) {
			strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE (PARENTID = %d AND TAKEOFFAPPSEPTYPE IS NULL);"),m_nID);
			CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetChild);
			GetItemCompounding(m_emClassType);
			return;
		}
		bool bAdd = true;
		AircraftClassifications ClasNone(m_nProjID, NoneBased);
		ClasNone.ReadData();

		AircraftClassifications ClasWakeVortexWightBased(m_nProjID, WakeVortexWightBased);
		ClasWakeVortexWightBased.ReadData();

		AircraftClassifications ClasWingspanBased(m_nProjID, WingspanBased);
		ClasWingspanBased.ReadData();

		AircraftClassifications ClasSurfaceBearingWeightBased(m_nProjID, SurfaceBearingWeightBased);
		ClasSurfaceBearingWeightBased.ReadData();

		AircraftClassifications ClasApproachSpeedBased(m_nProjID, ApproachSpeedBased);
		ClasApproachSpeedBased.ReadData();;

		m_log.Append(_T("CApproachRunwayNode::ReadData(count: )+ "));

		while (!adoRecordsetChild.IsEOF()) {
			CApproachSeparationItem * pNewItem = new CApproachSeparationItem(adoRecordsetChild);
			if (pNewItem) {
				switch(pNewItem->m_emClassType)
				{
				case NoneBased:
					if(!ClasNone.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasNone.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				case WakeVortexWightBased:
					if(!ClasWakeVortexWightBased.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasWakeVortexWightBased.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				case WingspanBased:
					if(!ClasWingspanBased.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasWingspanBased.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				case SurfaceBearingWeightBased:
					if(!ClasSurfaceBearingWeightBased.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasSurfaceBearingWeightBased.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				case ApproachSpeedBased:
					if(!ClasApproachSpeedBased.GetItemByID(pNewItem->m_aciTrail.m_nID))
						bAdd = false;
					if(!ClasApproachSpeedBased.GetItemByID(pNewItem->m_aciLead.m_nID))
						bAdd = false;
					break;
				default:
					ASSERT(0);
					break;
				}
				if(adoRecordset.GetFieldValue(_T("CLASSTYPE"),iValue))
				{
					if (bAdd)
						m_vrAppSep.push_back(pNewItem);
					else
						m_vrAppSepDeleted.push_back(pNewItem);
				}
			}
			bAdd = true;
			adoRecordsetChild.MoveNextData();
		}
		MakeDataValid(m_emClassType);
 		//if(!CheckTheItemCountAvailability())
 		//{
 		//	strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE PARENTID = %d AND TAKEOFFAPPSEPTYPE = %d AND CLASSTYPE = %d"),m_nID,AppSepType,m_emClassType) ;
 		//	CADODatabase::ExecuteSQLStatement(strSQL);
 		//	CARCportDatabaseConnection::GetInstance().writeLog(_logmsg + m_log) ;
 		//	RemoveClassType(m_emClassType);
 		//	GetItemCompounding(m_emClassType) ;
 		//}
	}
}

std::vector <CApproachSeparationItem *>  CApproachRunwayNode::GetItemCompounding(FlightClassificationBasisType emClassType)
{
	std::vector <CApproachSeparationItem *> vApproachSeparationItem;
	vApproachSeparationItem.clear();

	for (int i = 0; i < GetItemCount(); i++)
	{
		if (GetItem(i)->m_emClassType == emClassType)
		{
			CApproachSeparationItem* pItem = GetItem(i);
			vApproachSeparationItem.push_back(GetItem(i));
		}
	}

	if (!vApproachSeparationItem.size())
	{
		AircraftClassifications clsFltType(m_nProjID,emClassType);
		clsFltType.ReadData();

		//m_vrAppSepDeleted.insert(m_vrAppSepDeleted.end(),m_vrAppSep.begin(),m_vrAppSep.end());
		//m_vrAppSep.clear();

		int iSize = clsFltType.GetCount();
		int iID = 1;
		for(int i = 0;i < iSize;i++)
			for (int j = 0;j < iSize;j++)
			{
				CApproachSeparationItem * pNewItem = new CApproachSeparationItem;
				if (pNewItem) {
					pNewItem->m_emClassType = emClassType;
					pNewItem->m_aciLead = * (clsFltType.GetItem(i) );
					pNewItem->m_aciTrail = * (clsFltType.GetItem(j) );
					m_vrAppSep.push_back(pNewItem);
					vApproachSeparationItem.push_back(pNewItem);
				}			
			}
	}
	m_emClassType = emClassType;
	return vApproachSeparationItem;
}

void CApproachRunwayNode::ExportData(CAirsideExportFile& exportFile,CString _logmsg)
{
	CString log ;
	log.Format(_T("CApproachRunwayNode::ExportData(count:%d )+ "),(int)m_vrAppSep.size()) ;
	m_log.Append(log);

	if(!CheckTheItemCountAvailability())
	{
		CARCportDatabaseConnection::GetInstance().writeLog(_logmsg + m_log) ;
	
	}
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt((int)m_emApproachType);
	exportFile.getFile().writeInt(m_nFirstRunwayID);
	exportFile.getFile().writeInt((int)m_emFirstRunwayMark);
	exportFile.getFile().writeInt(m_nSecondRunwayID);
	exportFile.getFile().writeInt((int)m_emSecondRunwayMark);
	exportFile.getFile().writeInt((int)m_emClassType);
	exportFile.getFile().writeInt((int)m_emSepStd);
	exportFile.getFile().writeInt((int)m_bLAHSO);

	exportFile.getFile().writeInt((int)m_vrAppSep.size());
	std::vector <CApproachSeparationItem *>::iterator itrItem;
	for(itrItem = m_vrAppSep.begin();itrItem != m_vrAppSep.end();++itrItem){
		if(*itrItem)
			(*itrItem)->ExportData(exportFile);
	}

	exportFile.getFile().writeLine();
}

int CApproachRunwayNode::GetItemCount(void)
{
	return ((int)m_vrAppSep.size());
}

CApproachSeparationItem * CApproachRunwayNode::GetItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_vrAppSep.size())
		return (0);
	else
		return (m_vrAppSep.at(nIndex));
}

CApproachSeparationItem* CApproachRunwayNode::GetItem(AircraftClassificationItem* pTrailFlightItem, AircraftClassificationItem* pLeadFlightItem)
{
	ASSERT(NULL != pTrailFlightItem);
	ASSERT(NULL != pLeadFlightItem);

	for (int i=0; i<GetItemCount(); i++)
	{
		CApproachSeparationItem* pApproachSeparationItem = GetItem(i);
		ASSERT(NULL != pApproachSeparationItem);

		if (pTrailFlightItem->getID() == pApproachSeparationItem->m_aciTrail.getID()
			&& pLeadFlightItem->getID() == pApproachSeparationItem->m_aciLead.getID())
		{
			return pApproachSeparationItem;
		}
	}

	return NULL;
}

void CApproachRunwayNode::DeleteItem(CApproachSeparationItem *pItemDelete)
{
	std::vector<CApproachSeparationItem*>::iterator itrItemDelete;
	itrItemDelete = std::find(m_vrAppSep.begin(),m_vrAppSep.end(),pItemDelete);
	if(m_vrAppSep.end() !=  itrItemDelete){
		m_vrAppSepDeleted.push_back(*itrItemDelete);
		m_vrAppSep.erase(itrItemDelete);
	}
}

void CApproachRunwayNode::ImportData(CAirsideImportFile& importFile,CString _logmsg)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	int iValue = -1;
	importFile.getFile().getInteger(iValue);
	m_emApproachType = (AIRCRAFTOPERATIONTYPE)iValue;

	importFile.getFile().getInteger(m_nFirstRunwayID);
	m_nFirstRunwayID = importFile.GetObjectNewID(m_nFirstRunwayID);
	importFile.getFile().getInteger(iValue);
	m_emFirstRunwayMark = (RUNWAY_MARK)iValue;

	importFile.getFile().getInteger(m_nSecondRunwayID);
	m_nSecondRunwayID = importFile.GetObjectNewID(m_nSecondRunwayID);
	importFile.getFile().getInteger(iValue);
	m_emSecondRunwayMark = (RUNWAY_MARK)iValue;

	importFile.getFile().getInteger(iValue);
	m_emClassType = (FlightClassificationBasisType)iValue;

	importFile.getFile().getInteger(iValue);
	m_emSepStd = (SEPSTD)iValue;

	importFile.getFile().getInteger(iValue);
	m_bLAHSO = iValue?true:false;

	int nCount = -1;
	AircraftClassifications clsFltType(m_nProjID,m_emClassType);
	clsFltType.ReadData();
	importFile.getFile().getInteger(nCount);
	bool bAdd = true;
	Clear() ;

	CString log ;
	log.Format(_T("CApproachRunwayNode::ImportData(count:%d )+ "),(int)nCount) ;
	m_log.Append(log);


	for (int i = 0; i < nCount;++i) {
		CApproachSeparationItem * pNewItem = new CApproachSeparationItem;
		if (pNewItem) {
			pNewItem->ImportData(importFile);
			if(!clsFltType.GetItemByID(pNewItem->m_aciLead.m_nID))
				bAdd = false;
			if(!clsFltType.GetItemByID(pNewItem->m_aciLead.m_nID))
				bAdd = false;
			if (bAdd)
				m_vrAppSep.push_back(pNewItem);
			else
				m_vrAppSepDeleted.push_back(pNewItem);
		}
		bAdd = true;
	}

	if(!CheckTheItemCountAvailability())
	{
		CARCportDatabaseConnection::GetInstance().writeLog(_logmsg + m_log) ;
	}
    importFile.getFile().getLine();
}


void CApproachRunwayNode::RemoveClassType( FlightClassificationBasisType emClassType )
{
	int i = 0;
	while (i < GetItemCount())
	{
		if (GetItem(i)->m_emClassType == m_emClassType)
		{
			delete m_vrAppSep[i];
			m_vrAppSep.erase(m_vrAppSep.begin() + i);
		}
		else
			i++;
	}
}

CApproachSeparationItem::CApproachSeparationItem(void)
{
	m_nID = -1;
	m_nParentID = -1;

	m_dMinDistance = 4;
	m_strDisDistName = _T("U[0~0.3]");
	m_enumDisProbType = UNIFORM;
	m_strDisPrintDist = _T("Uniform:0;0.3");
	m_pDisDistribution = 0;

	m_dMinTime = 90;
	m_strTimeDistName = _T("Beta[0~30]");
	m_enumTimeProbType = BETA;
	m_strTimePrintDist = _T("Beta:0;30");
	m_pTimeDistribution = 0;
	
	GetDistanceProbDistribution();
	GetTimeProbDistribution();

}

CApproachSeparationItem::CApproachSeparationItem(CADORecordset &adoRecordset)
{
	m_pDisDistribution = 0;
	m_pTimeDistribution = 0;
	if (!adoRecordset.IsEOF()) {
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);
		int iValue = -1;
		if(adoRecordset.GetFieldValue(_T("CLASSTYPE"),iValue))
		{
			m_emClassType = (FlightClassificationBasisType)iValue;
			iValue = -1;
			adoRecordset.GetFieldValue(_T("LEADACID"),iValue);
			m_aciLead.m_nID = iValue;
			m_aciLead.ReadData(iValue);
			adoRecordset.GetFieldValue(_T("TRAILACID"),iValue);
			m_aciTrail.m_nID = iValue;
			m_aciTrail.ReadData(iValue);

			adoRecordset.GetFieldValue(_T("MINDIS"),m_dMinDistance);
			adoRecordset.GetFieldValue(_T("DISDISTNAME"),m_strDisDistName);
			adoRecordset.GetFieldValue(_T("DISDISTTYPE"),iValue);
			m_enumDisProbType = (ProbTypes)iValue;
			adoRecordset.GetFieldValue(_T("DISDISTPRINT"),m_strDisPrintDist);
			GetDistanceProbDistribution();

			adoRecordset.GetFieldValue(_T("MINTIME"),m_dMinTime);
			adoRecordset.GetFieldValue(_T("TIMEDISTNAME"),m_strTimeDistName);
			adoRecordset.GetFieldValue(_T("TIMEDISTTYPE"),iValue);
			m_enumTimeProbType = (ProbTypes)iValue;
			adoRecordset.GetFieldValue(_T("TIMEDISTPRINT"),m_strTimePrintDist);
			GetTimeProbDistribution();
		}
		else
		{
			DeleteData();
		}
	}
}

CApproachSeparationItem::~CApproachSeparationItem(void)
{
}

ProbabilityDistribution* CApproachSeparationItem::GetDistanceProbDistribution(void)
{
	if(m_pDisDistribution)
	{
		delete m_pDisDistribution;
		m_pDisDistribution = 0;
	}

	switch(m_enumDisProbType) 
	{
	case BERNOULLI:
		m_pDisDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pDisDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pDisDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pDisDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pDisDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pDisDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pDisDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pDisDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pDisDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pDisDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pDisDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pDisDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}
	if(!m_pDisDistribution)return (0);
	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strDisPrintDist, ":");

	strcpy(szprintDist,endOfName + 1);

	m_pDisDistribution->setDistribution(szprintDist);

	return m_pDisDistribution;
}

ProbabilityDistribution* CApproachSeparationItem::GetTimeProbDistribution(void)
{
	if(m_pTimeDistribution)
	{
		delete m_pTimeDistribution;
		m_pTimeDistribution = 0;
	}

	switch(m_enumTimeProbType) 
	{
	case BERNOULLI:
		m_pTimeDistribution = new BernoulliDistribution;
		break;
	case BETA:
		m_pTimeDistribution = new BetaDistribution;
		break;
	case CONSTANT:
		m_pTimeDistribution = new ConstantDistribution;
		break;
	case EMPIRICAL:
		m_pTimeDistribution = new EmpiricalDistribution;
		break;
	case EXPONENTIAL:
		m_pTimeDistribution = new ExponentialDistribution;
		break;
	case HISTOGRAM:
		m_pTimeDistribution = new HistogramDistribution;
		break;
	case NORMAL:
		m_pTimeDistribution = new NormalDistribution;
		break;
	case UNIFORM:
		m_pTimeDistribution = new UniformDistribution;
		break;
	case WEIBULL:
		m_pTimeDistribution = new WeibullDistribution;
		break;
	case GAMMA:
		m_pTimeDistribution = new GammaDistribution;
		break;
	case ERLANG:
		m_pTimeDistribution = new ErlangDistribution;
		break;
	case TRIANGLE:
		m_pTimeDistribution = new TriangleDistribution;
		break;
	default:
		break;
	}
	if(!m_pTimeDistribution)return (0);
	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strTimePrintDist, ":");

	strcpy(szprintDist,endOfName + 1);

	m_pTimeDistribution->setDistribution(szprintDist);

	return m_pTimeDistribution;
}

void CApproachSeparationItem::SaveData(int nParentID)
{
	CString strSQL = _T("");
	int bAppSep = 0;
	if (m_nID < 0) {
		strSQL.Format(_T("INSERT INTO OPERATIONCRITERIA (PARENTID,CLASSTYPE,TAKEOFFAPPSEPTYPE,LEADACID,TRAILACID,MINDIS,DISDISTNAME,DISDISTTYPE ,DISDISTPRINT,MINTIME ,TIMEDISTNAME,TIMEDISTTYPE,TIMEDISTPRINT) \
						 VALUES (%d,%d,%d,%d,%d,%f,'%s',%d,'%s',%f,'%s',%d,'%s');"),m_nParentID = nParentID,(int)m_emClassType,bAppSep,m_aciLead.m_nID,m_aciTrail.m_nID,m_dMinDistance,m_strDisDistName,(int)m_enumDisProbType,m_strDisPrintDist,m_dMinTime,m_strTimeDistName,(int)m_enumTimeProbType,m_strTimePrintDist);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}else{
		strSQL.Format(_T("UPDATE OPERATIONCRITERIA SET PARENTID = %d,CLASSTYPE = %d,TAKEOFFAPPSEPTYPE = %d,LEADACID = %d,TRAILACID = %d,MINDIS = %f,DISDISTNAME = '%s',DISDISTTYPE = %d,DISDISTPRINT = '%s',MINTIME = %f,TIMEDISTNAME = '%s',TIMEDISTTYPE = %d,TIMEDISTPRINT = '%s' WHERE (ID = %d);"),\
			m_nParentID = nParentID,(int)m_emClassType,bAppSep,m_aciLead.m_nID,m_aciTrail.m_nID,m_dMinDistance,m_strDisDistName,(int)m_enumDisProbType,m_strDisPrintDist,m_dMinTime,m_strTimeDistName,(int)m_enumTimeProbType,m_strTimePrintDist,m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CApproachSeparationItem::DeleteData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CApproachSeparationItem::ReadData(int nID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM OPERATIONCRITERIA WHERE (ID = %d);"),nID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	if (!adoRecordset.IsEOF()) {
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);
		int iValue = -1;
		if(adoRecordset.GetFieldValue(_T("CLASSTYPE"),iValue))
		{
			m_emClassType = (FlightClassificationBasisType)iValue;
			iValue = -1;
			adoRecordset.GetFieldValue(_T("LEADACID"),iValue);
			m_aciLead.m_nID = iValue;
			m_aciLead.ReadData(iValue);
			adoRecordset.GetFieldValue(_T("TRAILACID"),iValue);
			m_aciTrail.m_nID = iValue;
			m_aciTrail.ReadData(iValue);

			adoRecordset.GetFieldValue(_T("MINDIS"),m_dMinDistance);
			adoRecordset.GetFieldValue(_T("DISDISTNAME"),m_strDisDistName);
			adoRecordset.GetFieldValue(_T("DISDISTTYPE"),iValue);
			m_enumDisProbType = (ProbTypes)iValue;
			adoRecordset.GetFieldValue(_T("DISDISTPRINT"),m_strDisPrintDist);
			GetDistanceProbDistribution();

			adoRecordset.GetFieldValue(_T("MINTIME"),m_dMinTime);
			adoRecordset.GetFieldValue(_T("TIMEDISTNAME"),m_strTimeDistName);
			adoRecordset.GetFieldValue(_T("TIMEDISTTYPE"),iValue);
			m_enumTimeProbType = (ProbTypes)iValue;
			adoRecordset.GetFieldValue(_T("TIMEDISTPRINT"),m_strTimePrintDist);
			GetTimeProbDistribution();
		}
		else
		{
			DeleteData();
		}
	}
}

void CApproachSeparationItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_aciLead.m_nID);
	exportFile.getFile().writeInt(m_aciTrail.m_nID);

	exportFile.getFile().writeDouble(m_dMinDistance);

	exportFile.getFile().writeInt(m_strDisDistName.GetLength());
	exportFile.getFile().writeField(m_strDisDistName.GetBuffer(m_strDisDistName.GetLength()));
	m_strDisDistName.ReleaseBuffer(m_strDisDistName.GetLength());

	exportFile.getFile().writeInt((int)m_enumDisProbType);	

	exportFile.getFile().writeInt(m_strDisPrintDist.GetLength());
	exportFile.getFile().writeField(m_strDisPrintDist.GetBuffer(m_strDisPrintDist.GetLength()));
	m_strDisPrintDist.ReleaseBuffer(m_strDisPrintDist.GetLength());

	exportFile.getFile().writeDouble(m_dMinTime);

	exportFile.getFile().writeInt(m_strTimeDistName.GetLength());
	exportFile.getFile().writeField(m_strTimeDistName.GetBuffer(m_strTimeDistName.GetLength()));
	m_strTimeDistName.ReleaseBuffer(m_strTimeDistName.GetLength());

	exportFile.getFile().writeInt((int)m_enumTimeProbType);	

	exportFile.getFile().writeInt(m_strTimePrintDist.GetLength());
	exportFile.getFile().writeField(m_strTimePrintDist.GetBuffer(m_strTimePrintDist.GetLength()));
	m_strTimePrintDist.ReleaseBuffer(m_strTimePrintDist.GetLength());

	exportFile.getFile().writeLine();
}

void CApproachSeparationItem::ImportData(CAirsideImportFile& importFile)
{
	int iValue = -1;
	importFile.getFile().getInteger(iValue);
	m_aciLead.m_nID = iValue;
	m_aciLead.ReadData(iValue);

	importFile.getFile().getInteger(iValue);
	m_aciTrail.m_nID = iValue;
	m_aciTrail.ReadData(iValue);

	importFile.getFile().getFloat(m_dMinDistance);
	
	char * pData = 0;
	pData = new char[1024];
	if (!pData) return;

	importFile.getFile().getInteger(iValue);
	importFile.getFile().getField(pData,iValue); 
	m_strDisDistName = pData;
	memset(pData,0,1024);

	importFile.getFile().getInteger(iValue);
	m_enumDisProbType = (ProbTypes)iValue;

	importFile.getFile().getInteger(iValue);
	importFile.getFile().getField(pData,iValue); 
	m_strDisPrintDist = pData;
	memset(pData,0,1024);

	importFile.getFile().getFloat(m_dMinTime);

	importFile.getFile().getInteger(iValue);
	importFile.getFile().getField(pData,iValue); 
	m_strTimeDistName = pData;
	memset(pData,0,1024);

	importFile.getFile().getInteger(iValue);
	m_enumTimeProbType = (ProbTypes)iValue;

	importFile.getFile().getInteger(iValue);
	importFile.getFile().getField(pData,iValue); 
	m_strTimePrintDist = pData;
	memset(pData,0,1024);

	importFile.getFile().getLine();
}

