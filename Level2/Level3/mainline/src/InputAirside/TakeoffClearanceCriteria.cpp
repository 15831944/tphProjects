#include "stdafx.h"
#include "TakeoffClearanceCriteria.h"
#include "InputAirside.h"
#include "ALTAirport.h"
#include "runway.h"
#include "..\Database\DBElementCollection_Impl.h"

CTakeoffClearanceCriteria::CTakeoffClearanceCriteria(int nProjID,const ALTObjectList& vRunways)
:m_nProjID(nProjID)
,m_runwaySeparationRelation(nProjID,vRunways)
{
	m_vrTakeoffClearanceTypeList.push_back(new CTakeoffClaaranceTakeOffBehindLandedList(nProjID));
	m_vrTakeoffClearanceTypeList.push_back(new CTakeoffClaaranceTakeOffBehindTakeOffList(nProjID));
}

CTakeoffClearanceCriteria::~CTakeoffClearanceCriteria(void)
{
	for (std::vector<CTakeoffClearanceTypeList*>::iterator itrItem = m_vrTakeoffClearanceTypeList.begin();\
		itrItem != m_vrTakeoffClearanceTypeList.end();++itrItem) {
			if (*itrItem) 
				delete (*itrItem);
		}
	m_vrTakeoffClearanceTypeList.clear();
}
CTakeoffClaaranceTakeOffBehindLandedList *CTakeoffClearanceCriteria::GetTakeoffBehindLandingList()
{
	return (CTakeoffClaaranceTakeOffBehindLandedList *)(m_vrTakeoffClearanceTypeList.at(0));

}
CTakeoffClaaranceTakeOffBehindTakeOffList *CTakeoffClearanceCriteria::GetTakeoffBehindTakeOffList()
{
	return (CTakeoffClaaranceTakeOffBehindTakeOffList *)(m_vrTakeoffClearanceTypeList.at(1));
}
void CTakeoffClearanceCriteria::ReadData(void)
{
	m_runwaySeparationRelation.ReadData();
	for (std::vector<CTakeoffClearanceTypeList*>::iterator itrItem = m_vrTakeoffClearanceTypeList.begin();itrItem != m_vrTakeoffClearanceTypeList.end();++itrItem) {
		if (*itrItem) 
			(*itrItem)->ReadData();
	}
}

void CTakeoffClearanceCriteria::SaveData(void)
{
	m_runwaySeparationRelation.SaveData(m_nProjID);
	for (std::vector<CTakeoffClearanceTypeList*>::iterator itrItem = m_vrTakeoffClearanceTypeList.begin();itrItem != m_vrTakeoffClearanceTypeList.end();++itrItem) {
		if (*itrItem)
			(*itrItem)->SaveData();
	}
}

void CTakeoffClearanceCriteria::ExportData(CAirsideExportFile& exportFile)
{
	m_runwaySeparationRelation.ExportData(exportFile);
	for (std::vector<CTakeoffClearanceTypeList*>::iterator itrItem = m_vrTakeoffClearanceTypeList.begin();itrItem != m_vrTakeoffClearanceTypeList.end();++itrItem) {
		if (*itrItem)
			(*itrItem)->ExportData(exportFile);
	}
}

void CTakeoffClearanceCriteria::ImportData(CAirsideImportFile& importFile)
{
	m_runwaySeparationRelation.ImportData(importFile);
	for (std::vector<CTakeoffClearanceTypeList*>::iterator itrItem = m_vrTakeoffClearanceTypeList.begin();itrItem != m_vrTakeoffClearanceTypeList.end();++itrItem) {
		if (*itrItem)
			(*itrItem)->ImportData(importFile);
	}
}

void CTakeoffClearanceCriteria::ExportTakeoffClearanceCriteria(CAirsideExportFile& exportFile)
{
	//exportFile.getFile().writeField("TakeoffClearanceCriteria");
	//exportFile.getFile().writeLine();

	//CTakeoffClearanceCriteria takeoffClearanceCriteria(exportFile.GetProjectID());
	//takeoffClearanceCriteria.ReadData();
	//takeoffClearanceCriteria.ExportData(exportFile);

	//exportFile.getFile().endFile();
}

void CTakeoffClearanceCriteria::ImportTakeoffClearanceCriteria(CAirsideImportFile& importFile)
{
	//if(importFile.getVersion() < 1037)return;
	//CTakeoffClearanceCriteria takeoffClearanceCriteria(importFile.getNewProjectID());
	//takeoffClearanceCriteria.ImportData(importFile);
	//takeoffClearanceCriteria.SaveData();
}

ElapsedTime CTakeoffClearanceCriteria::GetTakeoffSeparationTime(int nRunwayID, RUNWAY_MARK runWayMark, AIRCRAFTOPERATIONTYPE aircraftOperType)
{
	std::vector<CTakeoffClearanceTypeList*>::iterator iter =  m_vrTakeoffClearanceTypeList.begin();

	ElapsedTime estSeparationTime(0.2);
	for (; iter!=m_vrTakeoffClearanceTypeList.end(); iter++)
	{
		if ((*iter)->m_emTakeoffClearanceType == aircraftOperType)
		{
			int nCount = (*iter)->GetNodeCount();
			for (int i=0; i<nCount; i++)
			{
				CTakeoffClaaranceRunwayNode* pTakeoffClearanceRunwayNode = (*iter)->GetNode(i);
				ASSERT(NULL != pTakeoffClearanceRunwayNode);

				if (pTakeoffClearanceRunwayNode->m_emFirstRunwayMark == runWayMark
					&& pTakeoffClearanceRunwayNode->m_nFirstRunwayID == nRunwayID
					&&aircraftOperType == pTakeoffClearanceRunwayNode->m_emTakeoffClearanceType)
				{
					estSeparationTime = ElapsedTime(pTakeoffClearanceRunwayNode->m_dTimeAfter);
					return estSeparationTime;	
				}
			}
		}
	}

	return estSeparationTime;
}
ElapsedTime CTakeoffClearanceCriteria::GetLandingSeparationTime(int nRunwayID, RUNWAY_MARK runWayMark, AIRCRAFTOPERATIONTYPE aircraftOperType)
{

	std::vector<CTakeoffClearanceTypeList*>::iterator iter =  m_vrTakeoffClearanceTypeList.begin();

	ElapsedTime estSeparationTime(0.2);
	for (; iter!=m_vrTakeoffClearanceTypeList.end(); iter++)
	{
		if ((*iter)->m_emTakeoffClearanceType == aircraftOperType)
		{
			int nCount = (*iter)->GetNodeCount();
			for (int i=0; i<nCount; i++)
			{
				CTakeoffClaaranceRunwayNode* pTakeoffClearanceRunwayNode = (*iter)->GetNode(i);
				ASSERT(NULL != pTakeoffClearanceRunwayNode);

				if (pTakeoffClearanceRunwayNode->m_emFirstRunwayMark == runWayMark
					&& pTakeoffClearanceRunwayNode->m_nFirstRunwayID == nRunwayID
					&&aircraftOperType == pTakeoffClearanceRunwayNode->m_emTakeoffClearanceType)
				{
					estSeparationTime = ElapsedTime(pTakeoffClearanceRunwayNode->m_dApproachTime);
					return estSeparationTime;
				}
			}
		}
	}

	return estSeparationTime;
}
double CTakeoffClearanceCriteria::GetTakeoffSeparationDistance(int nRunwayID, RUNWAY_MARK runWayMark,  AIRCRAFTOPERATIONTYPE aircraftOperType)
{


	std::vector<CTakeoffClearanceTypeList*>::iterator iter =  m_vrTakeoffClearanceTypeList.begin();

	double dDistance = 0.0;
	for (; iter!=m_vrTakeoffClearanceTypeList.end(); iter++)
	{
		if ((*iter)->m_emTakeoffClearanceType == aircraftOperType)
		{
			int nCount = (*iter)->GetNodeCount();
			for (int i=0; i<nCount; i++)
			{
				CTakeoffClaaranceRunwayNode* pTakeoffClearanceRunwayNode = (*iter)->GetNode(i);
				ASSERT(NULL != pTakeoffClearanceRunwayNode);

				if (pTakeoffClearanceRunwayNode->m_emFirstRunwayMark == runWayMark
					&& pTakeoffClearanceRunwayNode->m_nFirstRunwayID == nRunwayID
					&&	aircraftOperType == pTakeoffClearanceRunwayNode->m_emTakeoffClearanceType)
				{
					dDistance = pTakeoffClearanceRunwayNode->m_dLeadDistance;
					return dDistance;
				}
			}
		}
	}

	return dDistance;
}
double CTakeoffClearanceCriteria::GetLandingSeparationDistance(int nRunwayID, RUNWAY_MARK runWayMark,  AIRCRAFTOPERATIONTYPE aircraftOperType)
{

	std::vector<CTakeoffClearanceTypeList*>::iterator iter =  m_vrTakeoffClearanceTypeList.begin();

	double dDistance = 0.0;
	for (; iter!=m_vrTakeoffClearanceTypeList.end(); iter++)
	{
		if ((*iter)->m_emTakeoffClearanceType == aircraftOperType)
		{
			int nCount = (*iter)->GetNodeCount();
			for (int i=0; i<nCount; i++)
			{
				CTakeoffClaaranceRunwayNode* pTakeoffClearanceRunwayNode = (*iter)->GetNode(i);
				ASSERT(NULL != pTakeoffClearanceRunwayNode);

				if (pTakeoffClearanceRunwayNode->m_emFirstRunwayMark == runWayMark
					&& pTakeoffClearanceRunwayNode->m_nFirstRunwayID == nRunwayID
					&& aircraftOperType == pTakeoffClearanceRunwayNode->m_emTakeoffClearanceType)
				{
					dDistance = pTakeoffClearanceRunwayNode->m_dApproachDistance;
					return dDistance;
				}
			}
		}
	}

	return dDistance;
}
//time:return true  distance:return false
BOOL CTakeoffClearanceCriteria::IsTimeOrDistance(int nRunwayID, RUNWAY_MARK runWayMark,  AIRCRAFTOPERATIONTYPE aircraftOperType)
{

	std::vector<CTakeoffClearanceTypeList*>::iterator iter =  m_vrTakeoffClearanceTypeList.begin();

	BOOL bRet = TRUE;
	for (; iter!=m_vrTakeoffClearanceTypeList.end(); iter++)
	{
		if ((*iter)->m_emTakeoffClearanceType == aircraftOperType)
		{
			int nCount = (*iter)->GetNodeCount();
			for (int i=0; i<nCount; i++)
			{
				CTakeoffClaaranceRunwayNode* pTakeoffClearanceRunwayNode = (*iter)->GetNode(i);
				ASSERT(NULL != pTakeoffClearanceRunwayNode);

				if (pTakeoffClearanceRunwayNode->m_emFirstRunwayMark == runWayMark
					&& pTakeoffClearanceRunwayNode->m_nFirstRunwayID == nRunwayID
					&&aircraftOperType == pTakeoffClearanceRunwayNode->m_emTakeoffClearanceType)
				{
					bRet = !(pTakeoffClearanceRunwayNode->m_bClearByTimeOrDistance);
					return bRet;
				}
			}
		}
	}

	return bRet;
}
//time:return true  distance:return false
BOOL CTakeoffClearanceCriteria::IsLandingTimeOrDistance(int nRunwayID, RUNWAY_MARK runWayMark,  AIRCRAFTOPERATIONTYPE aircraftOperType)
{

	std::vector<CTakeoffClearanceTypeList*>::iterator iter =  m_vrTakeoffClearanceTypeList.begin();

	BOOL bRet = TRUE;
	for (; iter!=m_vrTakeoffClearanceTypeList.end(); iter++)
	{
		if ((*iter)->m_emTakeoffClearanceType == aircraftOperType)
		{
			int nCount = (*iter)->GetNodeCount();
			for (int i=0; i<nCount; i++)
			{
				CTakeoffClaaranceRunwayNode* pTakeoffClearanceRunwayNode = (*iter)->GetNode(i);
				ASSERT(NULL != pTakeoffClearanceRunwayNode);

				if (pTakeoffClearanceRunwayNode->m_emFirstRunwayMark == runWayMark
					&& pTakeoffClearanceRunwayNode->m_nFirstRunwayID == nRunwayID
					&&aircraftOperType == pTakeoffClearanceRunwayNode->m_emTakeoffClearanceType)
				{
					bRet = !(pTakeoffClearanceRunwayNode->m_bApproachByTimeOrDistance);
					return bRet;
				}
			}
		}
	}

	return bRet;
}
CTakeoffClaaranceRunwayNode* CTakeoffClearanceCriteria::GetTakeoffClearanceRunwayNode(int nTrailRunwayID, RUNWAY_MARK TrailRunWayMark, int nLeadRunwayID, RUNWAY_MARK LeadRunWayMark, AIRCRAFTOPERATIONTYPE aircraftOperType)
{

	std::vector<CTakeoffClearanceTypeList*>::iterator iter =  m_vrTakeoffClearanceTypeList.begin();

	BOOL bRet = TRUE;
	for (; iter!=m_vrTakeoffClearanceTypeList.end(); iter++)
	{
		if ((*iter)->m_emTakeoffClearanceType == aircraftOperType)
		{
			int nCount = (*iter)->GetNodeCount();
			for (int i=0; i<nCount; i++)
			{
				CTakeoffClaaranceRunwayNode* pTakeoffClearanceRunwayNode = (*iter)->GetNode(i);
				ASSERT(NULL != pTakeoffClearanceRunwayNode);

				if (pTakeoffClearanceRunwayNode->m_emFirstRunwayMark == TrailRunWayMark
					&& pTakeoffClearanceRunwayNode->m_nFirstRunwayID == nTrailRunwayID
					&& pTakeoffClearanceRunwayNode->m_emSecondRunwayMark == LeadRunWayMark
					&& pTakeoffClearanceRunwayNode->m_nSecondRunwayID == nLeadRunwayID)
				{
					return pTakeoffClearanceRunwayNode;
				}
			}
		}
	}

	return NULL;
}
void CTakeoffClearanceCriteria::GetRelatedLogicRunway(int nRunwayID, RUNWAY_MARK runWayMark, std::vector<int > vRunwayConcerned,std::vector<std::pair<int,RUNWAY_MARK> >& vLogicRunway, AIRCRAFTOPERATIONTYPE aircraftOperType )
{
	
	if (vRunwayConcerned.size() == 0)
		return;
	
	std::vector<CTakeoffClearanceTypeList*>::iterator iter =  m_vrTakeoffClearanceTypeList.begin();

	BOOL bRet = TRUE;
	for (; iter!=m_vrTakeoffClearanceTypeList.end(); iter++)
	{
		if ((*iter)->m_emTakeoffClearanceType == aircraftOperType)
		{
			int nCount = (*iter)->GetNodeCount();
			for (int i=0; i<nCount; i++)
			{
				CTakeoffClaaranceRunwayNode* pTakeoffClearanceRunwayNode = (*iter)->GetNode(i);
				ASSERT(NULL != pTakeoffClearanceRunwayNode);

				if (pTakeoffClearanceRunwayNode->m_emFirstRunwayMark == runWayMark
					&& pTakeoffClearanceRunwayNode->m_nFirstRunwayID == nRunwayID)

				{
					int nRunwayCount = (int)vRunwayConcerned.size();
					BOOL bFind = FALSE;
					for (int i = 0; i < nRunwayCount; ++ i)
					{
						if (pTakeoffClearanceRunwayNode->m_nSecondRunwayID == vRunwayConcerned[i])
						{
							bFind = TRUE;
							break;
						}
					}
					if (bFind == FALSE)
						vLogicRunway.push_back(std::make_pair(pTakeoffClearanceRunwayNode->m_nSecondRunwayID,pTakeoffClearanceRunwayNode->m_emSecondRunwayMark));

				}
			}
		}
	}

}
CTakeoffClearanceTypeList::CTakeoffClearanceTypeList(int nProjID,AIRCRAFTOPERATIONTYPE emTakeoffClearanceType):m_nProjID(nProjID),m_emTakeoffClearanceType(emTakeoffClearanceType)
{
}

CTakeoffClearanceTypeList::~CTakeoffClearanceTypeList(void)
{
	Clear();
}

void CTakeoffClearanceTypeList::Clear(void)
{
	std::vector<CTakeoffClaaranceRunwayNode * >::iterator itrItem ;
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

int CTakeoffClearanceTypeList::GetProjectID(void)
{
	return (m_nProjID);
}

void CTakeoffClearanceTypeList::ReadData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TAKEOFFCLEARANCERUNWAY WHERE (PROJID = %d AND TOCLEARTYPE = %d);"),m_nProjID,(int)m_emTakeoffClearanceType);

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
				CTakeoffClaaranceRunwayNode * pNewNode = new CTakeoffClaaranceRunwayNode(m_nProjID,m_emTakeoffClearanceType);
				if (pNewNode) {
					pNewNode->m_nFirstRunwayID = *itrRunwayID;
					pNewNode->m_emFirstRunwayMark = RUNWAYMARK_FIRST;
					pNewNode->m_nSecondRunwayID = *itrRunwayID;
					pNewNode->m_emSecondRunwayMark = RUNWAYMARK_FIRST;

					pNewNode->GetItemCompounding(pNewNode->m_emClassType);
					AddNode(pNewNode);
					pNewNode = 0;
				}
				pNewNode = new CTakeoffClaaranceRunwayNode(m_nProjID,m_emTakeoffClearanceType);
				if (pNewNode) {
					pNewNode->m_nFirstRunwayID = *itrRunwayID;
					pNewNode->m_emFirstRunwayMark = RUNWAYMARK_SECOND;
					pNewNode->m_nSecondRunwayID = *itrRunwayID;
					pNewNode->m_emSecondRunwayMark = RUNWAYMARK_SECOND;

					pNewNode->GetItemCompounding(pNewNode->m_emClassType);
					AddNode(pNewNode);
					pNewNode = 0;
				}
			}
		}

	}else 
	while (!adoRecordset.IsEOF()) {
		CTakeoffClaaranceRunwayNode * pNewNode = 0;
		pNewNode = new CTakeoffClaaranceRunwayNode(adoRecordset);
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
	}
}

void CTakeoffClearanceTypeList::SaveData(void)
{
	std::vector<CTakeoffClaaranceRunwayNode*>::iterator itrItem;
	for (itrItem = m_vrNode.begin();itrItem != m_vrNode.end();++itrItem) {
		if(*itrItem)
			(*itrItem)->SaveData(m_nProjID);
	}

	for (itrItem = m_vrNodeDeleted.begin();itrItem != m_vrNodeDeleted.end();++itrItem) {
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrNodeDeleted.clear();
}

void CTakeoffClearanceTypeList::DeleteData(void)
{
	std::vector<CTakeoffClaaranceRunwayNode*>::iterator itrItem;
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

void CTakeoffClearanceTypeList::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt((int)m_emTakeoffClearanceType);

	exportFile.getFile().writeInt((int)m_vrNode.size());
	std::vector<CTakeoffClaaranceRunwayNode*>::iterator itrItem;
	for (itrItem = m_vrNode.begin();itrItem != m_vrNode.end();++itrItem) {
		if(*itrItem)
			(*itrItem)->ExportData(exportFile);
	}

	exportFile.getFile().writeLine();
}

void CTakeoffClearanceTypeList::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	int iValue = -1;
	importFile.getFile().getInteger(iValue);
	m_emTakeoffClearanceType = (AIRCRAFTOPERATIONTYPE)iValue;

	importFile.getFile().getInteger(iValue);
	Clear();
	Runway runway1,runway2;
	for (int i = 0;i < iValue;++i) {
		CTakeoffClaaranceRunwayNode * pNewNode = 0;
		pNewNode = new CTakeoffClaaranceRunwayNode(m_nProjID,m_emTakeoffClearanceType);
		if (pNewNode){
			pNewNode->ImportData(importFile);
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

int CTakeoffClearanceTypeList::GetNodeCount(void)
{
	return ((int)m_vrNode.size());
}

CTakeoffClaaranceRunwayNode* CTakeoffClearanceTypeList::GetNode(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_vrNode.size())
		return (0);
	else
		return (m_vrNode.at(nIndex));
}

void CTakeoffClearanceTypeList::DeleteNode(CTakeoffClaaranceRunwayNode* pNodeDelete)
{
	std::vector<CTakeoffClaaranceRunwayNode*>::iterator itrItemDelete;
	itrItemDelete = std::find(m_vrNode.begin(),m_vrNode.end(),pNodeDelete);
	if(itrItemDelete != m_vrNode.end()){
		m_vrNodeDeleted.push_back(*itrItemDelete);
		m_vrNode.erase(itrItemDelete);
	}
}

bool CTakeoffClearanceTypeList::AddNode(CTakeoffClaaranceRunwayNode* pNewNode)
{
	if(!pNewNode)return (false);
	std::vector<CTakeoffClaaranceRunwayNode*>::iterator itrItemFind;
	itrItemFind = std::find(m_vrNode.begin(),m_vrNode.end(),pNewNode);
	if(itrItemFind == m_vrNode.end())
		m_vrNode.push_back(pNewNode);
	return (true);
}

CTakeoffClaaranceTakeOffBehindLandedList::CTakeoffClaaranceTakeOffBehindLandedList(int nProjID):CTakeoffClearanceTypeList(nProjID,AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED)
{
}

CTakeoffClaaranceTakeOffBehindLandedList::~CTakeoffClaaranceTakeOffBehindLandedList(void)
{
}

CTakeoffClaaranceTakeOffBehindTakeOffList::CTakeoffClaaranceTakeOffBehindTakeOffList(int nProjID):CTakeoffClearanceTypeList(nProjID,AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF)
{
}

CTakeoffClaaranceTakeOffBehindTakeOffList::~CTakeoffClaaranceTakeOffBehindTakeOffList(void)
{
}

CTakeoffClaaranceRunwayNode::CTakeoffClaaranceRunwayNode(int nProjID,AIRCRAFTOPERATIONTYPE emTakeoffClearanceType)
{
	m_nID = -1;
	m_nProjID = nProjID;
	m_nFirstRunwayID = -1;
	m_emFirstRunwayMark = RUNWAYMARK_FIRST;
	m_nSecondRunwayID = -1;
	m_emSecondRunwayMark = RUNWAYMARK_FIRST;
	m_emTakeoffClearanceType = emTakeoffClearanceType;
	m_emClassType = WakeVortexWightBased; 
	m_bClearByTimeOrDistance = false;
	m_dLeadDistance = 1.0;
	m_dTimeAfter = 60.0;
	m_bApproachByTimeOrDistance = false;
	m_dApproachDistance = 1.0;
	m_dApproachTime = 60.0;
	m_dDepartureTime = 60.0;

	m_emRRT = RRT_SAMERUNWAY;
	m_emRIT = RIT_NONE;
	m_dRw1ThresholdFromInterNode = -1.0;
	m_dRw2ThresholdFromInterNode = -1.0;
}

CTakeoffClaaranceRunwayNode::CTakeoffClaaranceRunwayNode(CADORecordset &adoRecordset)
{
	if (!adoRecordset.IsEOF()) {
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		int iValue = -1;
		adoRecordset.GetFieldValue(_T("TOCLEARTYPE"),iValue);
		m_emTakeoffClearanceType = (AIRCRAFTOPERATIONTYPE)iValue;
		adoRecordset.GetFieldValue(_T("FIRRWID"),m_nFirstRunwayID);
		adoRecordset.GetFieldValue(_T("FIRRWMARK"),iValue);
		m_emFirstRunwayMark = (RUNWAY_MARK)iValue;
		adoRecordset.GetFieldValue(_T("SECRWID"),m_nSecondRunwayID);
		adoRecordset.GetFieldValue(_T("SECRWMARK"),iValue);
		m_emSecondRunwayMark = (RUNWAY_MARK)iValue;

		adoRecordset.GetFieldValue(_T("CLASSTYPE"),iValue);
		m_emClassType = (FlightClassificationBasisType)iValue;

		adoRecordset.GetFieldValue(_T("BECLEARBYTORD"),iValue);
		m_bClearByTimeOrDistance = iValue?true:false;
		adoRecordset.GetFieldValue(_T("LEADDISTANCE"),m_dLeadDistance);
		adoRecordset.GetFieldValue(_T("TIMEAFTER"),m_dTimeAfter);

		adoRecordset.GetFieldValue(_T("BEAPPBYTORD"),iValue);
		m_bApproachByTimeOrDistance = iValue?true:false;
		adoRecordset.GetFieldValue(_T("APPDISTANCE"),m_dApproachDistance);
		adoRecordset.GetFieldValue(_T("APPTIME"),m_dApproachTime);

		adoRecordset.GetFieldValue(_T("DEPARTURETIME"),m_dDepartureTime);

		CString strSQL = _T("");
		int bTakeOff = 1;
		strSQL.Format(_T("SELECT * FROM OPERATIONCRITERIA WHERE (PARENTID = %d AND TAKEOFFAPPSEPTYPE = %d);"),m_nID,bTakeOff);
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
			CTakeoffClaaranceSeparationItem * pNewItem = new CTakeoffClaaranceSeparationItem(adoRecordsetChild);
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
						m_vrTakeoffSep.push_back(pNewItem);
					else
						m_vrTakeoffSepDeleted.push_back(pNewItem);
				}
			}
			bAdd = true;
			adoRecordsetChild.MoveNextData();
		}

		MakeDataValid(m_emClassType);
		//if(!CheckTheItemCountAvailability())
		//{
		//	strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE PARENTID = %d AND TAKEOFFAPPSEPTYPE = %d AND CLASSTYPE = %d"),m_nID,bTakeOff,m_emClassType) ;
		//	CADODatabase::ExecuteSQLStatement(strSQL);
		//	RemoveClassType(m_emClassType);
		//	GetItemCompounding(m_emClassType) ;
		//}
	}

	m_emRRT = (RunwayRelationType)-1;
	m_emRIT = (RunwayIntersectionType)-1;
	m_dRw1ThresholdFromInterNode = -1.0;
	m_dRw2ThresholdFromInterNode = -1.0;
}

CTakeoffClaaranceRunwayNode::~CTakeoffClaaranceRunwayNode(void)
{
	Clear();
}

void CTakeoffClaaranceRunwayNode::Clear(void)
{
	std::vector <CTakeoffClaaranceSeparationItem *>::iterator itrItem;
	for(itrItem = m_vrTakeoffSep.begin();itrItem != m_vrTakeoffSep.end();++itrItem){
		if(*itrItem)
			delete (*itrItem);
	}
	m_vrTakeoffSep.clear();

	for (itrItem = m_vrTakeoffSepDeleted.begin();itrItem != m_vrTakeoffSepDeleted.end();++itrItem) {
		if(*itrItem)
			delete (*itrItem);
	}
	m_vrTakeoffSepDeleted.clear();
}

void CTakeoffClaaranceRunwayNode::SaveData(int nProjID)
{
	CString strSQL = _T("");
	if(m_nID < 0){
		strSQL.Format(_T("INSERT INTO TAKEOFFCLEARANCERUNWAY (PROJID,TOCLEARTYPE,FIRRWID,FIRRWMARK ,SECRWID,SECRWMARK,CLASSTYPE,BECLEARBYTORD,LEADDISTANCE,TIMEAFTER,BEAPPBYTORD,APPDISTANCE,APPTIME,DEPARTURETIME) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%d,%f,%f,%f);"),m_nProjID = nProjID,(int)m_emTakeoffClearanceType,m_nFirstRunwayID,(int)m_emFirstRunwayMark ,m_nSecondRunwayID,(int)m_emSecondRunwayMark ,(int)m_emClassType,(int)m_bClearByTimeOrDistance,m_dLeadDistance,m_dTimeAfter,(int)m_bApproachByTimeOrDistance,m_dApproachDistance,m_dApproachTime,m_dDepartureTime);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}else{
		strSQL.Format(_T("UPDATE TAKEOFFCLEARANCERUNWAY SET PROJID = %d , TOCLEARTYPE = %d ,FIRRWID = %d ,FIRRWMARK = %d ,SECRWID = %d ,SECRWMARK = %d ,CLASSTYPE = %d ,BECLEARBYTORD = %d ,LEADDISTANCE = %f , TIMEAFTER = %f , BEAPPBYTORD = %d ,APPDISTANCE = %f , APPTIME = %f , DEPARTURETIME = %f  WHERE (ID = %d); "),m_nProjID = nProjID,(int)m_emTakeoffClearanceType,m_nFirstRunwayID,(int)m_emFirstRunwayMark ,m_nSecondRunwayID,(int)m_emSecondRunwayMark ,(int)m_emClassType,(int)m_bClearByTimeOrDistance,m_dLeadDistance,m_dTimeAfter,(int)m_bApproachByTimeOrDistance,m_dApproachDistance,m_dApproachTime,m_dDepartureTime,m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	std::vector <CTakeoffClaaranceSeparationItem *>::iterator itrItem;
	for(itrItem = m_vrTakeoffSep.begin();itrItem != m_vrTakeoffSep.end();++itrItem){
		if(*itrItem)
			(*itrItem)->SaveData(m_nID);
	}

	for (itrItem = m_vrTakeoffSepDeleted.begin();itrItem != m_vrTakeoffSepDeleted.end();++itrItem) {
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrTakeoffSepDeleted.clear();
}

void CTakeoffClaaranceRunwayNode::DeleteData(void)
{
	std::vector <CTakeoffClaaranceSeparationItem *>::iterator itrItem;
	for(itrItem = m_vrTakeoffSep.begin();itrItem != m_vrTakeoffSep.end();++itrItem){
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrTakeoffSep.clear();

	for (itrItem = m_vrTakeoffSepDeleted.begin();itrItem != m_vrTakeoffSepDeleted.end();++itrItem) {
		if(*itrItem){
			(*itrItem)->DeleteData();
			delete (*itrItem);
		}
	}
	m_vrTakeoffSepDeleted.clear();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TAKEOFFCLEARANCERUNWAY WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CTakeoffClaaranceRunwayNode::ReadData(int nID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TAKEOFFCLEARANCERUNWAY WHERE (ID = %d);"),nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	Clear();
	if (!adoRecordset.IsEOF()){
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		int iValue = -1;
		adoRecordset.GetFieldValue(_T("TOCLEARTYPE"),iValue);
		m_emTakeoffClearanceType = (AIRCRAFTOPERATIONTYPE)iValue;
		adoRecordset.GetFieldValue(_T("FIRRWID"),m_nFirstRunwayID);
		adoRecordset.GetFieldValue(_T("FIRRWMARK"),iValue);
		m_emFirstRunwayMark = (RUNWAY_MARK)iValue;
		adoRecordset.GetFieldValue(_T("SECRWID"),m_nSecondRunwayID);
		adoRecordset.GetFieldValue(_T("SECRWMARK"),iValue);
		m_emSecondRunwayMark = (RUNWAY_MARK)iValue;

		adoRecordset.GetFieldValue(_T("CLASSTYPE"),iValue);
		m_emClassType = (FlightClassificationBasisType)iValue;

		adoRecordset.GetFieldValue(_T("BECLEARBYTORD"),iValue);
		m_bClearByTimeOrDistance = iValue?true:false;
		adoRecordset.GetFieldValue(_T("LEADDISTANCE"),m_dLeadDistance);
		adoRecordset.GetFieldValue(_T("TIMEAFTER"),m_dTimeAfter);

		adoRecordset.GetFieldValue(_T("BEAPPBYTORD"),iValue);
		m_bApproachByTimeOrDistance = iValue?true:false;
		adoRecordset.GetFieldValue(_T("APPDISTANCE"),m_dApproachDistance);
		adoRecordset.GetFieldValue(_T("APPTIME"),m_dApproachTime);

		adoRecordset.GetFieldValue(_T("DEPARTURETIME"),m_dDepartureTime);

		int bTakeOff = 1;
		strSQL.Format(_T("SELECT * FROM OPERATIONCRITERIA WHERE (PARENTID = %d AND TAKEOFFAPPSEPTYPE = %d);"),m_nID,bTakeOff);
		CADORecordset adoRecordsetChild;
		CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetChild);

		if (adoRecordsetChild.IsEOF()) {
			strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE (PARENTID = %d AND TAKEOFFAPPSEPTYPE IS NULL );"),m_nID);
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
			CTakeoffClaaranceSeparationItem * pNewItem = new CTakeoffClaaranceSeparationItem(adoRecordsetChild);
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
						m_vrTakeoffSep.push_back(pNewItem);
					else
						m_vrTakeoffSepDeleted.push_back(pNewItem);
				}
			}
			bAdd = true;
			adoRecordsetChild.MoveNextData();
		}
		
		MakeDataValid(m_emClassType);
		//if(!CheckTheItemCountAvailability())
		//{
		//	strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE PARENTID = %d AND TAKEOFFAPPSEPTYPE = %d AND CLASSTYPE = %d"),m_nID,bTakeOff,m_emClassType) ;
		//	CADODatabase::ExecuteSQLStatement(strSQL);
		//	RemoveClassType(m_emClassType);
		//	GetItemCompounding(m_emClassType) ;
		//}

	}
}

std::vector <CTakeoffClaaranceSeparationItem *> CTakeoffClaaranceRunwayNode::GetItemCompounding(FlightClassificationBasisType emClassType)
{
	std::vector <CTakeoffClaaranceSeparationItem *> vTakeOffItem;
	vTakeOffItem.clear();

	int nCount = GetItemCount();
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (GetItem(i)->m_emClassType == emClassType)
		{
			CTakeoffClaaranceSeparationItem* pItem = GetItem(i);
			double dMinDis = pItem->m_dMinDistance;
			vTakeOffItem.push_back(GetItem(i));
		}
	}
	
	if (!vTakeOffItem.size())
	{
		AircraftClassifications clsFltType(m_nProjID,emClassType);
		clsFltType.ReadData();

		/*m_vrTakeoffSepDeleted.insert(m_vrTakeoffSepDeleted.end(),m_vrTakeoffSep.begin(),m_vrTakeoffSep.end());
		m_vrTakeoffSep.clear();*/

		int iSize = clsFltType.GetCount();
		int iID = 1;
		for(int ii = 0;ii < iSize;ii++)
			for (int j = 0;j < iSize;j++)
			{
				CTakeoffClaaranceSeparationItem * pNewItem = new CTakeoffClaaranceSeparationItem;
				if (pNewItem) {
					pNewItem->m_emClassType = emClassType;
					pNewItem->m_aciLead = * (clsFltType.GetItem(ii) );
					pNewItem->m_aciTrail = * (clsFltType.GetItem(j) );
					m_vrTakeoffSep.push_back(pNewItem);
					vTakeOffItem.push_back(pNewItem);
				}			
			}
	}
	m_emClassType = emClassType;
	return vTakeOffItem;
}

void CTakeoffClaaranceRunwayNode::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt((int)m_emTakeoffClearanceType);
	exportFile.getFile().writeInt(m_nFirstRunwayID);
	exportFile.getFile().writeInt((int)m_emFirstRunwayMark);
	exportFile.getFile().writeInt(m_nSecondRunwayID);
	exportFile.getFile().writeInt((int)m_emSecondRunwayMark);
	exportFile.getFile().writeInt((int)m_emClassType);
	exportFile.getFile().writeInt((int)m_bClearByTimeOrDistance);
	exportFile.getFile().writeDouble(m_dLeadDistance);
	exportFile.getFile().writeDouble(m_dTimeAfter);
	exportFile.getFile().writeInt((int)m_bApproachByTimeOrDistance);
	exportFile.getFile().writeDouble(m_dApproachDistance);
	exportFile.getFile().writeDouble(m_dApproachTime);
	exportFile.getFile().writeDouble(m_dDepartureTime);

	exportFile.getFile().writeInt((int)m_vrTakeoffSep.size());
	std::vector <CTakeoffClaaranceSeparationItem *>::iterator itrItem;
	for(itrItem = m_vrTakeoffSep.begin();itrItem != m_vrTakeoffSep.end();++itrItem){
		if(*itrItem)
			(*itrItem)->ExportData(exportFile);
	}

	exportFile.getFile().writeLine();
}

int CTakeoffClaaranceRunwayNode::GetItemCount(void)
{
	return ((int)m_vrTakeoffSep.size());
}

CTakeoffClaaranceSeparationItem * CTakeoffClaaranceRunwayNode::GetItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_vrTakeoffSep.size())
		return (0);
	else
		return (m_vrTakeoffSep.at(nIndex));
}

CTakeoffClaaranceSeparationItem* CTakeoffClaaranceRunwayNode::GetItem(AircraftClassificationItem* ptrailFlight, AircraftClassificationItem* pLeadFlight)
{
	ASSERT(NULL != ptrailFlight);
	ASSERT(NULL != pLeadFlight);

	for (int i=0; i<GetItemCount(); i++)
	{
		CTakeoffClaaranceSeparationItem* pItem = GetItem(i);
		ASSERT(NULL != pItem);

		if ((ptrailFlight->m_nID== pItem->m_aciTrail.m_nID)
			&& (pLeadFlight->m_nID == pItem->m_aciLead.m_nID))
		{
			return pItem;
		}
	}

	return NULL;
}

void CTakeoffClaaranceRunwayNode::DeleteItem(CTakeoffClaaranceSeparationItem *pItemDelete)
{
	std::vector<CTakeoffClaaranceSeparationItem*>::iterator itrItemDelete;
	itrItemDelete = std::find(m_vrTakeoffSep.begin(),m_vrTakeoffSep.end(),pItemDelete);
	if(m_vrTakeoffSep.end() !=  itrItemDelete){
		m_vrTakeoffSepDeleted.push_back(*itrItemDelete);
		m_vrTakeoffSep.erase(itrItemDelete);
	}
}

void CTakeoffClaaranceRunwayNode::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	int iValue = -1;
	importFile.getFile().getInteger(iValue);
	m_emTakeoffClearanceType = (AIRCRAFTOPERATIONTYPE)iValue;

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
	m_bClearByTimeOrDistance = iValue?true:false;
	importFile.getFile().getFloat(m_dLeadDistance);
	importFile.getFile().getFloat(m_dTimeAfter);

	importFile.getFile().getInteger(iValue);
	m_bApproachByTimeOrDistance = iValue?true:false;
	importFile.getFile().getFloat(m_dApproachDistance);
	importFile.getFile().getFloat(m_dApproachTime);

	importFile.getFile().getFloat(m_dDepartureTime);

	int nCount = -1;
	AircraftClassifications clsFltType(m_nProjID,m_emClassType);
	clsFltType.ReadData();
	importFile.getFile().getInteger(nCount);
	bool bAdd = true;
	for (int i = 0; i < nCount;++i) {
		CTakeoffClaaranceSeparationItem * pNewItem = new CTakeoffClaaranceSeparationItem;
		if (pNewItem) {
			pNewItem->ImportData(importFile);
			if(!clsFltType.GetItemByID(pNewItem->m_aciLead.m_nID))
				bAdd = false;
			if(!clsFltType.GetItemByID(pNewItem->m_aciLead.m_nID))
				bAdd = false;
			if (bAdd)
				m_vrTakeoffSep.push_back(pNewItem);
			else
				m_vrTakeoffSepDeleted.push_back(pNewItem);
		}
		bAdd = true;
	}

	importFile.getFile().getLine();
}

BOOL CTakeoffClaaranceRunwayNode::CheckTheItemCountAvailability()
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

void CTakeoffClaaranceRunwayNode::RemoveClassType( FlightClassificationBasisType emClassType )
{
	int i = 0;
	while (i < GetItemCount())
	{
		if (GetItem(i)->m_emClassType == m_emClassType)
		{
			delete m_vrTakeoffSep[i];
			m_vrTakeoffSep.erase(m_vrTakeoffSep.begin() + i);
		}
		else
			i++;
	}
}

void CTakeoffClaaranceRunwayNode::MakeDataValid( FlightClassificationBasisType emClassType )
{
	AircraftClassifications clsFltType(m_nProjID,emClassType);
	clsFltType.ReadData();
	int iSize = clsFltType.GetCount();

	//int nClassTypeCount = 0;
	//for (int i = 0; i < GetItemCount(); i++)
	//{
	//	if (GetItem(i)->m_emClassType == emClassType)
	//	{
	//		nClassTypeCount++;
	//	}
	//}
	//int nValidCount = iSize*iSize;
	//if(nClassTypeCount != nValidCount)
	//{
	//	CString strSQL;
	//	int bTakeOff = 1;
	//	strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE PARENTID = %d AND TAKEOFFAPPSEPTYPE = %d AND CLASSTYPE = %d"),m_nID,bTakeOff,m_emClassType) ;
	//	CADODatabase::ExecuteSQLStatement(strSQL);
	//	RemoveClassType(emClassType);
	//}

	//doesn't exsit data and push back default data
	for(int i = 0;i < iSize;i++)
	{
		for (int j = 0;j < iSize;j++)
		{
			if (GetItem(clsFltType.GetItem(j),clsFltType.GetItem(i)) == NULL)
			{
				CTakeoffClaaranceSeparationItem * pNewItem = new CTakeoffClaaranceSeparationItem;
				if (pNewItem) 
				{
					pNewItem->m_emClassType = emClassType;
					pNewItem->m_aciTrail = * (clsFltType.GetItem(j) );
					pNewItem->m_aciLead = * (clsFltType.GetItem(i) );
					int iPos = i*iSize + j;
					m_vrTakeoffSep.insert(m_vrTakeoffSep.begin() + iPos,pNewItem);
				}	
			}
		}
	}

	//exsit data valid
	std::vector<CTakeoffClaaranceSeparationItem*> vInvalidData;
	for (int i = 0; i < GetItemCount(); i++)
	{
		CTakeoffClaaranceSeparationItem* pItem = GetItem(i);
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

CTakeoffClaaranceSeparationItem::CTakeoffClaaranceSeparationItem(void)
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

CTakeoffClaaranceSeparationItem::CTakeoffClaaranceSeparationItem(CADORecordset &adoRecordset)
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

CTakeoffClaaranceSeparationItem::~CTakeoffClaaranceSeparationItem(void)
{
}

ProbabilityDistribution* CTakeoffClaaranceSeparationItem::GetDistanceProbDistribution(void)
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

	strcpy_s(szprintDist,endOfName + 1);

	m_pDisDistribution->setDistribution(szprintDist);

	return m_pDisDistribution;
}

ProbabilityDistribution* CTakeoffClaaranceSeparationItem::GetTimeProbDistribution(void)
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
	if(!m_pTimeDistribution)
		return (0);

	char szprintDist[1024] = {0};
	const char *endOfName = strstr (m_strTimePrintDist, ":");

	strcpy_s(szprintDist,endOfName + 1);

	m_pTimeDistribution->setDistribution(szprintDist);

	return m_pTimeDistribution;
}

void CTakeoffClaaranceSeparationItem::SaveData(int nParentID)
{
	CString strSQL = _T("");
	int bTakeOff = 1;
	if (m_nID < 0) 
	{
		strSQL.Format(_T("INSERT INTO OPERATIONCRITERIA (PARENTID,CLASSTYPE,TAKEOFFAPPSEPTYPE,LEADACID,TRAILACID,MINDIS,DISDISTNAME,DISDISTTYPE ,DISDISTPRINT,MINTIME ,TIMEDISTNAME,TIMEDISTTYPE,TIMEDISTPRINT) \
						 VALUES (%d,%d,%d,%d,%d,%f,'%s',%d,'%s',%f,'%s',%d,'%s');"),m_nParentID = nParentID,(int)m_emClassType,bTakeOff,m_aciLead.m_nID,m_aciTrail.m_nID,m_dMinDistance,m_strDisDistName,(int)m_enumDisProbType,m_strDisPrintDist,m_dMinTime,m_strTimeDistName,(int)m_enumTimeProbType,m_strTimePrintDist);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		strSQL.Format(_T("UPDATE OPERATIONCRITERIA SET PARENTID = %d,CLASSTYPE = %d,TAKEOFFAPPSEPTYPE = %d,LEADACID = %d,TRAILACID = %d,MINDIS = %f,DISDISTNAME = '%s',DISDISTTYPE = %d,DISDISTPRINT = '%s',MINTIME = %f,TIMEDISTNAME = '%s',TIMEDISTTYPE = %d,TIMEDISTPRINT = '%s' WHERE (ID = %d);"),\
			m_nParentID = nParentID,(int)m_emClassType,bTakeOff,m_aciLead.m_nID,m_aciTrail.m_nID,m_dMinDistance,m_strDisDistName,(int)m_enumDisProbType,m_strDisPrintDist,m_dMinTime,m_strTimeDistName,(int)m_enumTimeProbType,m_strTimePrintDist,m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CTakeoffClaaranceSeparationItem::DeleteData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM OPERATIONCRITERIA WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CTakeoffClaaranceSeparationItem::ReadData(int nID)
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
		adoRecordset.GetFieldValue(_T("CLASSTYPE"),iValue);
		m_emClassType = (FlightClassificationBasisType)iValue;
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
}

void CTakeoffClaaranceSeparationItem::ExportData(CAirsideExportFile& exportFile)
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

void CTakeoffClaaranceSeparationItem::ImportData(CAirsideImportFile& importFile)
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

