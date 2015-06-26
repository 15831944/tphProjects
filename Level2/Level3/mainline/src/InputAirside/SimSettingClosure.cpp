#include "stdafx.h"
#include "altobjectgroup.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside/InputAirside.h"
#include "SimSettingClosure.h"
#include <algorithm>

CRunwayClosureNode::CRunwayClosureNode(void)
{
	m_nID = -1;//id
	m_nParentID = -1;//parent id ,i.e. CClosureAtAirport's id
	m_nRunwayID = -1;//runway id
	m_nIntersectNodeIDFrom = -1;//intersect node id from...
	m_nIntersectNodeIDTo = -1;//intersect node id to...
	m_startTime.set(0L);//start time
	m_endTime.set(86399L);//end time 
}

CRunwayClosureNode::CRunwayClosureNode(CADORecordset &adoRecordset)
{
	if(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);
		adoRecordset.GetFieldValue(_T("RUNWAYID"),m_nRunwayID);
		adoRecordset.GetFieldValue(_T("INTERNODEIDFROM"),m_nIntersectNodeIDFrom);
		adoRecordset.GetFieldValue(_T("INTERNODEIDTO"),m_nIntersectNodeIDTo);
		int nValue = -1;
		adoRecordset.GetFieldValue(_T("TIMEFROM"),nValue);
		m_startTime.set(nValue);
		adoRecordset.GetFieldValue(_T("TIMETO"),nValue);
		m_endTime.set(nValue);
	}
}

CRunwayClosureNode::~CRunwayClosureNode(void)
{
}

void CRunwayClosureNode::SaveData(int nParentID)
{
	CString strSQL = _T("");
	if(m_nID < 0){
		strSQL.Format(_T("INSERT INTO RUNWAYCLOSURENODE (PARENTID,RUNWAYID,INTERNODEIDFROM,INTERNODEIDTO,TIMEFROM,TIMETO) VALUES (%d,%d,%d,%d,%d,%d);"),\
			m_nParentID = nParentID,m_nRunwayID,m_nIntersectNodeIDFrom,m_nIntersectNodeIDTo,m_startTime.asSeconds(),m_endTime.asSeconds());

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else{
		strSQL.Format(_T("UPDATE RUNWAYCLOSURENODE SET PARENTID = %d,RUNWAYID = %d,INTERNODEIDFROM = %d,INTERNODEIDTO = %d,TIMEFROM = %d,TIMETO = %d WHERE (ID = %d);"),\
			m_nParentID = nParentID,m_nRunwayID,m_nIntersectNodeIDFrom,m_nIntersectNodeIDTo,m_startTime.asSeconds(),m_endTime.asSeconds(),m_nID);

		 CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CRunwayClosureNode::DeleteData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM RUNWAYCLOSURENODE WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CRunwayClosureNode::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nRunwayID);
	exportFile.getFile().writeInt(m_nIntersectNodeIDFrom);
	exportFile.getFile().writeInt(m_nIntersectNodeIDTo);
	exportFile.getFile().writeInt(m_startTime.asSeconds());
	exportFile.getFile().writeInt(m_endTime.asSeconds());
	exportFile.getFile().writeLine();
}

void CRunwayClosureNode::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nRunwayID);
	m_nRunwayID = importFile.GetObjectNewID(m_nRunwayID);
	importFile.getFile().getInteger(m_nIntersectNodeIDFrom);
	m_nIntersectNodeIDFrom = importFile.GetIntersectionIndexMap(m_nIntersectNodeIDFrom);
	importFile.getFile().getInteger(m_nIntersectNodeIDTo);
	m_nIntersectNodeIDTo = importFile.GetIntersectionIndexMap(m_nIntersectNodeIDTo);
	int nValue = -1;
	importFile.getFile().getInteger(nValue);
	m_startTime.set(nValue);
	importFile.getFile().getInteger(nValue);
	m_endTime.set(nValue);
	importFile.getFile().getLine();
}


CTaxiwayClosureNode::CTaxiwayClosureNode(void)
{
	m_nID = -1;//id
	m_nParentID = -1;//parent id ,i.e. CClosureAtAirport's id
	m_nTaxiwayID = -1;//runway id
	m_nIntersectNodeIDFrom = -1;//intersect node id from...
	m_nIntersectNodeIDTo = -1;//intersect node id to...
	m_startTime.set(0L);//start time
	m_endTime.set(86399L);//end time 
}

CTaxiwayClosureNode::CTaxiwayClosureNode(CADORecordset &adoRecordset)
{
	if(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);
		adoRecordset.GetFieldValue(_T("TAXIWAYID"),m_nTaxiwayID);
		adoRecordset.GetFieldValue(_T("INTERNODEIDFROM"),m_nIntersectNodeIDFrom);
		adoRecordset.GetFieldValue(_T("INTERNODEIDTO"),m_nIntersectNodeIDTo);
		int nValue = -1;
		adoRecordset.GetFieldValue(_T("TIMEFROM"),nValue);
		m_startTime.set(nValue);
		adoRecordset.GetFieldValue(_T("TIMETO"),nValue);
		m_endTime.set(nValue);
	}
}

CTaxiwayClosureNode::~CTaxiwayClosureNode(void)
{
}

void CTaxiwayClosureNode::SaveData(int nParentID)
{
	CString strSQL = _T("");
	if(m_nID < 0){
		strSQL.Format(_T("INSERT INTO TAXIWAYCLOSURENODE  (PARENTID,TAXIWAYID,INTERNODEIDFROM,INTERNODEIDTO,TIMEFROM,TIMETO) VALUES (%d,%d,%d,%d,%d,%d);"),\
			m_nParentID = nParentID,m_nTaxiwayID,m_nIntersectNodeIDFrom,m_nIntersectNodeIDTo,m_startTime.asSeconds(),m_endTime.asSeconds());

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else{
		strSQL.Format(_T("UPDATE TAXIWAYCLOSURENODE  SET PARENTID = %d,TAXIWAYID = %d,INTERNODEIDFROM = %d,INTERNODEIDTO = %d,TIMEFROM = %d,TIMETO = %d WHERE (ID = %d);"),\
			m_nParentID = nParentID,m_nTaxiwayID,m_nIntersectNodeIDFrom,m_nIntersectNodeIDTo,m_startTime.asSeconds(),m_endTime.asSeconds(),m_nID);

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CTaxiwayClosureNode::DeleteData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TAXIWAYCLOSURENODE  WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CTaxiwayClosureNode::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nTaxiwayID);
	exportFile.getFile().writeInt(m_nIntersectNodeIDFrom);
	exportFile.getFile().writeInt(m_nIntersectNodeIDTo);
	exportFile.getFile().writeInt(m_startTime.asSeconds());
	exportFile.getFile().writeInt(m_endTime.asSeconds());
	exportFile.getFile().writeLine();
}

void CTaxiwayClosureNode::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nTaxiwayID);
	m_nTaxiwayID = importFile.GetObjectNewID(m_nTaxiwayID);
	importFile.getFile().getInteger(m_nIntersectNodeIDFrom);
	m_nIntersectNodeIDFrom = importFile.GetIntersectionIndexMap(m_nIntersectNodeIDFrom);
	importFile.getFile().getInteger(m_nIntersectNodeIDTo);
	m_nIntersectNodeIDTo = importFile.GetIntersectionIndexMap(m_nIntersectNodeIDTo);
	int nValue = -1;
	importFile.getFile().getInteger(nValue);
	m_startTime.set(nValue);
	importFile.getFile().getInteger(nValue);
	m_endTime.set(nValue);
	importFile.getFile().getLine();
}

CStandGroupClosureNode::CStandGroupClosureNode(void)
{
	m_nID = -1;
	m_nParentID = -1;
	m_nStandGroupID = -1;
	m_startTime.set(0L);
	m_endTime.set(86399L);
}

CStandGroupClosureNode::CStandGroupClosureNode(CADORecordset &adoRecordset)
{
	if(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);
		adoRecordset.GetFieldValue(_T("STANDGROUPID"),m_nStandGroupID);
		int nValue = -1;
		adoRecordset.GetFieldValue(_T("TIMEFROM"),nValue);
		m_startTime.set(nValue);
		adoRecordset.GetFieldValue(_T("TIMETO"),nValue);
		m_endTime.set(nValue);
	}
}

CStandGroupClosureNode::~CStandGroupClosureNode(void)
{
}


void CStandGroupClosureNode::SaveData(int nParentID)
{
	CString strSQL = _T("");
	if(m_nID < 0){
		strSQL.Format(_T("INSERT INTO STANDGROUPCLOSURENODE  (PARENTID,STANDGROUPID,TIMEFROM,TIMETO) VALUES (%d,%d,%d,%d);"),\
			m_nParentID = nParentID,m_nStandGroupID,m_startTime.asSeconds(),m_endTime.asSeconds());

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else{
		strSQL.Format(_T("UPDATE STANDGROUPCLOSURENODE  SET PARENTID = %d,STANDGROUPID = %d,TIMEFROM = %d,TIMETO = %d WHERE (ID = %d);"),\
			m_nParentID = nParentID,m_nStandGroupID,m_startTime.asSeconds(),m_endTime.asSeconds(),m_nID);

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CStandGroupClosureNode::DeleteData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM STANDGROUPCLOSURENODE  WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CStandGroupClosureNode::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nStandGroupID); 
	ALTObjectGroup altObjGroup;
	altObjGroup.ReadData(m_nStandGroupID);	
	ALTObjectID objName = altObjGroup.getName();
	objName.writeALTObjectID(exportFile.getFile());

	exportFile.getFile().writeInt(m_startTime.asSeconds());
	exportFile.getFile().writeInt(m_endTime.asSeconds());
	exportFile.getFile().writeLine();
}

void CStandGroupClosureNode::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nStandGroupID);
	
	ALTObjectGroup altObjGroup;	
	altObjGroup.setType(ALT_STAND);
	ALTObjectID objName;
	objName.readALTObjectID(importFile.getFile());
	altObjGroup.setName(objName); 
	altObjGroup.SaveData(importFile.getNewProjectID());
	if(m_nStandGroupID != -1){
		m_nStandGroupID = altObjGroup.getID();
	}

	int nValue = -1;
	importFile.getFile().getInteger(nValue);
	m_startTime.set(nValue);
	importFile.getFile().getInteger(nValue);
	m_endTime.set(nValue);
	importFile.getFile().getLine();
}

CClosureAtAirport::CClosureAtAirport(void)
{
	m_nID = -1;
	m_nProjID = -1;
	m_nAirportID = -1;
	m_nObjectMark = -1;
}

CClosureAtAirport::~CClosureAtAirport(void)
{
}

CClosureAtAirport::CClosureAtAirport(CADORecordset &adoRecordset)
{
	if(!adoRecordset.IsEOF()){
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("AIRPORTID"),m_nAirportID);
		adoRecordset.GetFieldValue(_T("OBJECTMARK"),m_nObjectMark);
	}
}

void CClosureAtAirport::SaveData(int nProjID)
{
	CString strSQL = _T("");
	if(m_nID < 0){
		strSQL.Format(_T("INSERT INTO CLOSUREATAIRPORT (PROJID,AIRPORTID,OBJECTMARK) VALUES (%d,%d,%d);"),m_nProjID = nProjID,m_nAirportID,m_nObjectMark);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else{
		strSQL.Format(_T("UPDATE CLOSUREATAIRPORT SET PROJID = %d,AIRPORTID = %d,OBJECTMARK = %d WHERE (ID = %d);"),m_nProjID = nProjID,m_nAirportID,m_nObjectMark,m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CClosureAtAirport::DeleteData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM CLOSUREATAIRPORT WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CClosureAtAirport::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_nAirportID);
	exportFile.getFile().writeInt(m_nObjectMark);
	exportFile.getFile().writeLine();
} 

void CClosureAtAirport::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	importFile.getFile().getInteger(m_nAirportID);
	m_nAirportID = importFile.GetAirportNewIndex(m_nAirportID);
	importFile.getFile().getInteger(m_nObjectMark);
	importFile.getFile().getLine();
}

CRunwayClosureAtAirport::CRunwayClosureAtAirport(void)
{
	m_nObjectMark = 1;
}

CRunwayClosureAtAirport::CRunwayClosureAtAirport(CADORecordset &adoRecordset):CClosureAtAirport(adoRecordset)
{
	if(!adoRecordset.IsEOF()){
		CString strSQL = _T("");
		strSQL.Format(_T("SELECT * FROM RUNWAYCLOSURENODE WHERE (PARENTID = %d);"),m_nID);
		long nRecordCount = -1;
		CADORecordset adoRecordsetChild;
		CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetChild);

		std::vector<int> vrRunwayID;
		ALTAirport::GetRunwaysIDs(m_nAirportID, vrRunwayID);
		while (!adoRecordsetChild.IsEOF()) {
			int nRunwayID = -1;
			adoRecordsetChild.GetFieldValue(_T("RUNWAYID"),nRunwayID);
			bool bAdd = false;
			if(std::find(vrRunwayID.begin(),vrRunwayID.end(),nRunwayID) != vrRunwayID.end())
				bAdd = true;
			CRunwayClosureNode * pNode = new CRunwayClosureNode(adoRecordsetChild);
			if(pNode){
				if(bAdd)
					m_vrRunwayClosure.push_back(pNode);
				else
					m_vrRunwayClosureDeleted.push_back(pNode);
			}
			adoRecordsetChild.MoveNextData();
		}
	}
}

CRunwayClosureAtAirport::~CRunwayClosureAtAirport(void)
{
}

void CRunwayClosureAtAirport::SaveData(int nProjID)
{
	CClosureAtAirport::SaveData(nProjID);

	std::vector <CRunwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrRunwayClosure.begin();itrClosureNode != m_vrRunwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode)
			(*itrClosureNode)->SaveData(m_nID);
	}
	for (itrClosureNode = m_vrRunwayClosureDeleted.begin();itrClosureNode != m_vrRunwayClosureDeleted.end();++itrClosureNode) {
		if(*itrClosureNode){
			(*itrClosureNode)->DeleteData();
			delete (*itrClosureNode);
		}
	}
	m_vrRunwayClosureDeleted.clear();
}

void CRunwayClosureAtAirport::DeleteData(void)
{
	std::vector <CRunwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrRunwayClosure.begin();itrClosureNode != m_vrRunwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode){
			(*itrClosureNode)->DeleteData();
			delete (*itrClosureNode);
		}
	}
	m_vrRunwayClosure.clear();

	for (itrClosureNode = m_vrRunwayClosureDeleted.begin();itrClosureNode != m_vrRunwayClosureDeleted.end();++itrClosureNode) {
		if(*itrClosureNode){
			(*itrClosureNode)->DeleteData();
			delete (*itrClosureNode);
		}
	}
	m_vrRunwayClosureDeleted.clear();

	CClosureAtAirport::DeleteData();
}

void CRunwayClosureAtAirport::ExportData(CAirsideExportFile& exportFile)
{
	CClosureAtAirport::ExportData(exportFile);

	exportFile.getFile().writeInt((int)m_vrRunwayClosure.size());
	std::vector <CRunwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrRunwayClosure.begin();itrClosureNode != m_vrRunwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode)
			(*itrClosureNode)->ExportData(exportFile);
	}

	exportFile.getFile().writeLine();
}

void CRunwayClosureAtAirport::ImportData(CAirsideImportFile& importFile)
{
	CClosureAtAirport::ImportData(importFile);

	Clear();
	int nCount = -1;
	importFile.getFile().getInteger(nCount);
	for (int i = 0;i < nCount;++i) {
		CRunwayClosureNode * pClosureNode = new CRunwayClosureNode;
		if(pClosureNode){
			pClosureNode->ImportData(importFile);
			m_vrRunwayClosure.push_back(pClosureNode);
		}else
			return;
	}

	importFile.getFile().getLine();
}

CRunwayClosureNode * CRunwayClosureAtAirport::FindRunwayClosureNode(int nRunwayID)
{
	std::vector <CRunwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrRunwayClosure.begin();itrClosureNode != m_vrRunwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode && (*itrClosureNode)->m_nRunwayID == nRunwayID)
			return (*itrClosureNode);
	}
	return (0);
}

bool CRunwayClosureAtAirport::AddNode(CRunwayClosureNode * pNewNode)
{
	if(!pNewNode)return (false);
//	if(FindRunwayClosureNode(pNewNode->m_nRunwayID))return (true);
	m_vrRunwayClosure.push_back(pNewNode);
	return (true);
}

bool CRunwayClosureAtAirport::DeleteNode(CRunwayClosureNode * pNodeDelete)
{
	if(!pNodeDelete)return (false);
	std::vector <CRunwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrRunwayClosure.begin();itrClosureNode != m_vrRunwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode && (*itrClosureNode) == pNodeDelete){
			m_vrRunwayClosureDeleted.push_back(*itrClosureNode);
			m_vrRunwayClosure.erase(itrClosureNode);
			return (true);
		}
	}
	return (false);
}

int CRunwayClosureAtAirport::GetRunwayClosureNodeCount(void)
{
	return ((int)m_vrRunwayClosure.size());
}

CRunwayClosureNode * CRunwayClosureAtAirport::GetRunwayClosureNode(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_vrRunwayClosure.size())
		return (0);
	else
		return (m_vrRunwayClosure.at(nIndex));
}

void CRunwayClosureAtAirport::Clear(void)
{
	std::vector <CRunwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrRunwayClosure.begin();itrClosureNode != m_vrRunwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode)
			delete (*itrClosureNode);
	}
	m_vrRunwayClosure.clear();

	for (itrClosureNode = m_vrRunwayClosureDeleted.begin();itrClosureNode != m_vrRunwayClosureDeleted.end();++itrClosureNode) {
		if(*itrClosureNode)
			delete (*itrClosureNode);
	}
	m_vrRunwayClosureDeleted.clear();
}

CTaxiwayClosureAtAirport::CTaxiwayClosureAtAirport(void)
{
	m_nObjectMark = 2;
}

CTaxiwayClosureAtAirport::CTaxiwayClosureAtAirport(CADORecordset &adoRecordset):CClosureAtAirport(adoRecordset)
{
	if(!adoRecordset.IsEOF()){ 

		CString strSQL = _T("");
		strSQL.Format(_T("SELECT * FROM TAXIWAYCLOSURENODE  WHERE (PARENTID = %d);"),m_nID);
		long nRecordCount = -1;
		CADORecordset adoRecordsetChild;
		CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetChild);

		std::vector<int> vrTaxiwayID;
		ALTAirport::GetTaxiwaysIDs(m_nAirportID,vrTaxiwayID);
		while (!adoRecordsetChild.IsEOF()) {
			int nTaxiwayID = -1;
			adoRecordsetChild.GetFieldValue(_T("TAXIWAYID"),nTaxiwayID);
			bool bAdd = false;
			if(std::find(vrTaxiwayID.begin(),vrTaxiwayID.end(),nTaxiwayID) != vrTaxiwayID.end())
				bAdd = true;

			CTaxiwayClosureNode * pNode = new CTaxiwayClosureNode(adoRecordsetChild);
			if(pNode){
				if(bAdd)
					m_vrTaxiwayClosure.push_back(pNode);
				else
					m_vrTaxiwayClosureDeleted.push_back(pNode);
			}
			adoRecordsetChild.MoveNextData();
		}
	}
}

CTaxiwayClosureAtAirport::~CTaxiwayClosureAtAirport(void)
{
}

void CTaxiwayClosureAtAirport::SaveData(int nProjID)
{
	CClosureAtAirport::SaveData(nProjID);

	std::vector <CTaxiwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrTaxiwayClosure.begin();itrClosureNode != m_vrTaxiwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode)
			(*itrClosureNode)->SaveData(m_nID);
	}
	for (itrClosureNode = m_vrTaxiwayClosureDeleted.begin();itrClosureNode != m_vrTaxiwayClosureDeleted.end();++itrClosureNode) {
		if(*itrClosureNode){
			(*itrClosureNode)->DeleteData();
			delete (*itrClosureNode);
		}
	}
	m_vrTaxiwayClosureDeleted.clear();
}

void CTaxiwayClosureAtAirport::DeleteData(void)
{
	std::vector <CTaxiwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrTaxiwayClosure.begin();itrClosureNode != m_vrTaxiwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode){
			(*itrClosureNode)->DeleteData();
			delete (*itrClosureNode);
		}
	}
	m_vrTaxiwayClosure.clear();

	for (itrClosureNode = m_vrTaxiwayClosureDeleted.begin();itrClosureNode != m_vrTaxiwayClosureDeleted.end();++itrClosureNode) {
		if(*itrClosureNode){
			(*itrClosureNode)->DeleteData();
			delete (*itrClosureNode);
		}
	}
	m_vrTaxiwayClosureDeleted.clear();

	CClosureAtAirport::DeleteData();
}

void CTaxiwayClosureAtAirport::ExportData(CAirsideExportFile& exportFile)
{
	CClosureAtAirport::ExportData(exportFile);

	exportFile.getFile().writeInt((int)m_vrTaxiwayClosure.size());
	std::vector <CTaxiwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrTaxiwayClosure.begin();itrClosureNode != m_vrTaxiwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode)
			(*itrClosureNode)->ExportData(exportFile);
	}

	exportFile.getFile().writeLine();
}

void CTaxiwayClosureAtAirport::ImportData(CAirsideImportFile& importFile)
{
	CClosureAtAirport::ImportData(importFile);

	Clear();
	int nCount = -1;
	importFile.getFile().getInteger(nCount);
	for (int i = 0;i < nCount;++i) {
		CTaxiwayClosureNode * pClosureNode = new CTaxiwayClosureNode;
		if(pClosureNode){
			pClosureNode->ImportData(importFile);
			m_vrTaxiwayClosure.push_back(pClosureNode);
		}else
			return;
	}

	importFile.getFile().getLine();
}

CTaxiwayClosureNode * CTaxiwayClosureAtAirport::FindTaxiwayClosureNode(int nTaxiwayID)
{
	std::vector <CTaxiwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrTaxiwayClosure.begin();itrClosureNode != m_vrTaxiwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode && (*itrClosureNode)->m_nTaxiwayID == nTaxiwayID)
			return (*itrClosureNode);
	}
	return (0);
}

bool CTaxiwayClosureAtAirport::AddNode(CTaxiwayClosureNode * pNewNode)
{
	if(!pNewNode)return (false);
	if(FindTaxiwayClosureNode(pNewNode->m_nTaxiwayID))return (true);
	m_vrTaxiwayClosure.push_back(pNewNode);
	return (true);
}

bool CTaxiwayClosureAtAirport::DeleteNode(CTaxiwayClosureNode * pNodeDelete)
{
	if(!pNodeDelete)return (false);
	std::vector <CTaxiwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrTaxiwayClosure.begin();itrClosureNode != m_vrTaxiwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode && (*itrClosureNode) == pNodeDelete){
			m_vrTaxiwayClosureDeleted.push_back(*itrClosureNode);
			m_vrTaxiwayClosure.erase(itrClosureNode);
			return (true);
		}
	}
	return (false);
}

int CTaxiwayClosureAtAirport::GetTaxiwayClosureNodeCount(void)
{
	return ((int)m_vrTaxiwayClosure.size());
}

CTaxiwayClosureNode * CTaxiwayClosureAtAirport::GetTaxiwayClosureNode(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_vrTaxiwayClosure.size())
		return (0);
	else
		return (m_vrTaxiwayClosure.at(nIndex));
}

void CTaxiwayClosureAtAirport::Clear(void)
{
	std::vector <CTaxiwayClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrTaxiwayClosure.begin();itrClosureNode != m_vrTaxiwayClosure.end();++itrClosureNode) {
		if(*itrClosureNode)
			delete (*itrClosureNode);
	}
	m_vrTaxiwayClosure.clear();

	for (itrClosureNode = m_vrTaxiwayClosureDeleted.begin();itrClosureNode != m_vrTaxiwayClosureDeleted.end();++itrClosureNode) {
		if(*itrClosureNode)
			delete (*itrClosureNode);
	}
	m_vrTaxiwayClosureDeleted.clear();
}

CStandGroupClosureAtAirport::CStandGroupClosureAtAirport(void)
{
	m_nObjectMark = 3;
}

CStandGroupClosureAtAirport::CStandGroupClosureAtAirport(CADORecordset &adoRecordset):CClosureAtAirport(adoRecordset)
{
	if(!adoRecordset.IsEOF()){

		CString strSQL = _T("");
		strSQL.Format(_T("SELECT * FROM STANDGROUPCLOSURENODE   WHERE (PARENTID = %d);"),m_nID);
		long nRecordCount = -1;
		CADORecordset adoRecordsetChild;
		CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetChild);

		/*std::vector<int> vStands;
		ALTAirport::GetStandsIDs(m_nAirportID, vStands);*/	
		/*ALTObjectGroup altObjGroup;	
		altObjGroup.ReadData(pStandGroupClosureNode->m_nStandGroupID);*/
		while (!adoRecordsetChild.IsEOF()) {
			int nStandGroupID = -1;
			adoRecordsetChild.GetFieldValue(_T("STANDGROUPID"),nStandGroupID);
			/*bool bAdd = false;
			if(std::find(vStands.begin(),vStands.end(),nStandGroupID) != vStands.end())*/
			bool bAdd = true;
		
			CStandGroupClosureNode * pNode = new CStandGroupClosureNode(adoRecordsetChild);
			if(pNode){
				if(bAdd)
					m_vrStandGroupClosure.push_back(pNode);
				else 
					m_vrStandGroupClosureDeleted.push_back(pNode);
			}

			adoRecordsetChild.MoveNextData();
		}
	}
}

CStandGroupClosureAtAirport::~CStandGroupClosureAtAirport(void)
{
}

void CStandGroupClosureAtAirport::SaveData(int nProjID)
{
	CClosureAtAirport::SaveData(nProjID);

	std::vector <CStandGroupClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrStandGroupClosure.begin();itrClosureNode != m_vrStandGroupClosure.end();++itrClosureNode) {
		if(*itrClosureNode)
			(*itrClosureNode)->SaveData(m_nID);
	}
	for (itrClosureNode = m_vrStandGroupClosureDeleted.begin();itrClosureNode != m_vrStandGroupClosureDeleted.end();++itrClosureNode) {
		if(*itrClosureNode){
			(*itrClosureNode)->DeleteData();
			delete (*itrClosureNode);
		}
	}
	m_vrStandGroupClosureDeleted.clear();
}

void CStandGroupClosureAtAirport::DeleteData(void)
{
	std::vector <CStandGroupClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrStandGroupClosure.begin();itrClosureNode != m_vrStandGroupClosure.end();++itrClosureNode) {
		if(*itrClosureNode){
			(*itrClosureNode)->DeleteData();
			delete (*itrClosureNode);
		}
	}
	m_vrStandGroupClosure.clear();

	for (itrClosureNode = m_vrStandGroupClosureDeleted.begin();itrClosureNode != m_vrStandGroupClosureDeleted.end();++itrClosureNode) {
		if(*itrClosureNode){
			(*itrClosureNode)->DeleteData();
			delete (*itrClosureNode);
		}
	}
	m_vrStandGroupClosureDeleted.clear();

	CClosureAtAirport::DeleteData();
}

void CStandGroupClosureAtAirport::ExportData(CAirsideExportFile& exportFile)
{
	CClosureAtAirport::ExportData(exportFile);

	exportFile.getFile().writeInt((int)m_vrStandGroupClosure.size());
	std::vector <CStandGroupClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrStandGroupClosure.begin();itrClosureNode != m_vrStandGroupClosure.end();++itrClosureNode) {
		if(*itrClosureNode)
			(*itrClosureNode)->ExportData(exportFile);
	}

	exportFile.getFile().writeLine();
}

void CStandGroupClosureAtAirport::ImportData(CAirsideImportFile& importFile)
{
	CClosureAtAirport::ImportData(importFile);

	Clear();
	int nCount = -1;
	importFile.getFile().getInteger(nCount);
	for (int i = 0;i < nCount;++i) {
		CStandGroupClosureNode * pClosureNode = new CStandGroupClosureNode;
		if(pClosureNode){
			pClosureNode->ImportData(importFile);
			m_vrStandGroupClosure.push_back(pClosureNode);
		}else 
			return;
	}

	importFile.getFile().getLine();
}

CStandGroupClosureNode * CStandGroupClosureAtAirport::FindStandGroupClosureNode(int nStandGroupID)
{
	std::vector <CStandGroupClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrStandGroupClosure.begin();itrClosureNode != m_vrStandGroupClosure.end();++itrClosureNode) {
		if(*itrClosureNode && (*itrClosureNode)->m_nStandGroupID == nStandGroupID)
			return (*itrClosureNode);
	}
	return (0);
}

int CStandGroupClosureAtAirport::GetStandGroupClosureNodeCount(void)
{
	return ((int)m_vrStandGroupClosure.size());
}

CStandGroupClosureNode * CStandGroupClosureAtAirport::GetStandGroupClosureNode(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_vrStandGroupClosure.size())
		return (0);
	else
		return (m_vrStandGroupClosure.at(nIndex));
}

bool CStandGroupClosureAtAirport::AddNode(CStandGroupClosureNode * pNewNode)
{
	if(!pNewNode)return (false);
	if(FindStandGroupClosureNode(pNewNode->m_nStandGroupID))return (true);
	m_vrStandGroupClosure.push_back(pNewNode);
	return (true);
}

bool CStandGroupClosureAtAirport::DeleteNode(CStandGroupClosureNode * pNodeDelete)
{
	if(!pNodeDelete)return (false);
	std::vector <CStandGroupClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrStandGroupClosure.begin();itrClosureNode != m_vrStandGroupClosure.end();++itrClosureNode) {
		if(*itrClosureNode && (*itrClosureNode) == pNodeDelete){
			m_vrStandGroupClosureDeleted.push_back(*itrClosureNode);
			m_vrStandGroupClosure.erase(itrClosureNode);
			return (true);
		}
	}
	return (false);
}

void CStandGroupClosureAtAirport::Clear(void)
{
	std::vector <CStandGroupClosureNode * >::iterator itrClosureNode;
	for (itrClosureNode = m_vrStandGroupClosure.begin();itrClosureNode != m_vrStandGroupClosure.end();++itrClosureNode) {
		if(*itrClosureNode)
			delete (*itrClosureNode);
	}
	m_vrStandGroupClosure.clear();

	for (itrClosureNode = m_vrStandGroupClosureDeleted.begin();itrClosureNode != m_vrStandGroupClosureDeleted.end();++itrClosureNode) {
		if(*itrClosureNode)
			delete (*itrClosureNode);
	}
	m_vrStandGroupClosureDeleted.clear();
}

CClosure::CClosure(int nProjID):m_nProjID(nProjID),m_nObjectMark(0)
{
}

CClosure::~CClosure(void)
{
	Clear();
}

CClosureAtAirport * CClosure::FindClosure(int nAirport)
{
	for(std::vector<CClosureAtAirport *>::iterator itrClosrue = m_vrClosureAtAirport.begin();\
		itrClosrue != m_vrClosureAtAirport.end();++itrClosrue){
			if(*itrClosrue && (*itrClosrue)->m_nAirportID == nAirport)
				return (*itrClosrue);
		}
	return NULL;
}

int CClosure::GetAirportCount(void)
{
	return ((int)m_vrClosureAtAirport.size());
}

CClosureAtAirport * CClosure::GetClosureAtAirport(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_vrClosureAtAirport.size())
		return NULL;
	else
		return (m_vrClosureAtAirport.at(nIndex));
}

bool CClosure::AddClosure(CClosureAtAirport * pNewAirportNode)
{
	if(!pNewAirportNode)return (false);
	m_vrClosureAtAirport.push_back(pNewAirportNode);
	return (true);
}

bool CClosure::DeleteClosure(CClosureAtAirport * pAirportNodeDelete)
{
	if(!pAirportNodeDelete)return (false);
	for(std::vector<CClosureAtAirport *>::iterator itrClosrue = m_vrClosureAtAirport.begin();\
		itrClosrue != m_vrClosureAtAirport.end();++itrClosrue){
			if(*itrClosrue && (*itrClosrue) == pAirportNodeDelete){
				m_vrClosureAtAirportDeleted.push_back(*itrClosrue);
				m_vrClosureAtAirport.erase(itrClosrue);
				return (true);
			}
		} 
	return (false);
}

void CClosure::ReadData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM CLOSUREATAIRPORT WHERE (PROJID = %d);"),m_nProjID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);	

	Clear();

	if(adoRecordset.IsEOF()){
		std::vector<int>::iterator itrID;
		for (itrID = vAirportIds.begin();itrID != vAirportIds.end();++itrID) {
			switch(m_nObjectMark) {
				case 1://CRunwayClosureAtAirport
					{
						CRunwayClosureAtAirport * pRunwayClosure = new CRunwayClosureAtAirport;
						pRunwayClosure->m_nAirportID = *itrID;
						if(pRunwayClosure)m_vrClosureAtAirport.push_back(pRunwayClosure);
					}
					break;
				case 2://CTaxiwayClosureAtAirport
					{
						CTaxiwayClosureAtAirport* pTaxiwayClosure = new CTaxiwayClosureAtAirport;
						pTaxiwayClosure->m_nAirportID = *itrID;
						if(pTaxiwayClosure)m_vrClosureAtAirport.push_back(pTaxiwayClosure);
					}
					break;
				case 3://CStandGroupClosureAtAirport
					{
						CStandGroupClosureAtAirport* pStandGroupClosure = new CStandGroupClosureAtAirport;
						pStandGroupClosure->m_nAirportID = *itrID;
						if(pStandGroupClosure)m_vrClosureAtAirport.push_back(pStandGroupClosure);
					}
					break;
				default:
					break;
			}
		}
	}

	while (!adoRecordset.IsEOF()) {
		int nID = -1;
		int nAirportID = -1;
		int nObjectMark = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		adoRecordset.GetFieldValue(_T("AIRPORTID"),nAirportID);
		adoRecordset.GetFieldValue(_T("OBJECTMARK"),nObjectMark);

		bool bAdd = false;
		if(std::find(vAirportIds.begin(),vAirportIds.end(),nAirportID) != vAirportIds.end())
			bAdd = true;

		bool bInitialize = true;
		if(nObjectMark != m_nObjectMark)
			bInitialize = false;
			
		if(bInitialize){
			switch(m_nObjectMark) 
			{
			case 1://CRunwayClosureAtAirport
				{
					CRunwayClosureAtAirport * pRunwayClosure = new CRunwayClosureAtAirport(adoRecordset);
					if(pRunwayClosure){
						if(bAdd)
							m_vrClosureAtAirport.push_back(pRunwayClosure);
						else
							m_vrClosureAtAirportDeleted.push_back(pRunwayClosure);
					}
				}
				break;
			case 2://CTaxiwayClosureAtAirport
				{
					CTaxiwayClosureAtAirport* pTaxiwayClosure = new CTaxiwayClosureAtAirport(adoRecordset);
					if(pTaxiwayClosure){
						if(bAdd)
							m_vrClosureAtAirport.push_back(pTaxiwayClosure);
						else
							m_vrClosureAtAirportDeleted.push_back(pTaxiwayClosure);
					}
				}
				break;
			case 3://CStandGroupClosureAtAirport
				{
					CStandGroupClosureAtAirport* pStandGroupClosure = new CStandGroupClosureAtAirport(adoRecordset);
					if(pStandGroupClosure){
						if(bAdd)
							m_vrClosureAtAirport.push_back(pStandGroupClosure);
						else
							m_vrClosureAtAirportDeleted.push_back(pStandGroupClosure);
					}
				}
				break;
			default:
				break;
			}
		}
		adoRecordset.MoveNextData();
	}

}

void CClosure::SaveData(void)
{
	if(m_nProjID < 0)return;
	std::vector<CClosureAtAirport *>::iterator itrClosures;
	for (itrClosures = m_vrClosureAtAirport.begin();itrClosures != m_vrClosureAtAirport.end();++ itrClosures) {
			if(*itrClosures)
				(*itrClosures)->SaveData(m_nProjID);
		}
	for (itrClosures = m_vrClosureAtAirportDeleted.begin();itrClosures != m_vrClosureAtAirportDeleted.end();++ itrClosures) {
			if(*itrClosures){
				(*itrClosures)->DeleteData();
				delete (*itrClosures);
			}
		}
	m_vrClosureAtAirportDeleted.clear();	
}

void CClosure::DeleteData(void)
{
	std::vector<CClosureAtAirport *>::iterator itrClosures;
	for (itrClosures = m_vrClosureAtAirport.begin();itrClosures != m_vrClosureAtAirport.end();++ itrClosures) {
		if(*itrClosures){
			(*itrClosures)->DeleteData();
			delete (*itrClosures);
		}
	}
	m_vrClosureAtAirport.clear();

	for (itrClosures = m_vrClosureAtAirportDeleted.begin();itrClosures != m_vrClosureAtAirportDeleted.end();++ itrClosures) {
		if(*itrClosures){
			(*itrClosures)->DeleteData();
			delete (*itrClosures);
		}
	}
	m_vrClosureAtAirportDeleted.clear();	
	m_nProjID = -1;
	m_nObjectMark = 0;
}


void CClosure::ImportData(CAirsideImportFile& importFile)
{
	Clear();
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	importFile.getFile().getInteger(m_nObjectMark);

	int nCount = -1;
	importFile.getFile().getInteger(nCount);
	for(int i = 0;i < nCount;++i){
		switch(m_nObjectMark) 
		{
		case 1://CRunwayClosureAtAirport
			{
				CRunwayClosureAtAirport * pRunwayClosure = new CRunwayClosureAtAirport;
				if(pRunwayClosure){
					pRunwayClosure->ImportData(importFile);
					m_vrClosureAtAirport.push_back(pRunwayClosure);
				}
			}
			break;
		case 2://CTaxiwayClosureAtAirport
			{
				CTaxiwayClosureAtAirport* pTaxiwayClosure = new CTaxiwayClosureAtAirport;
				if(pTaxiwayClosure){
					pTaxiwayClosure->ImportData(importFile);
					m_vrClosureAtAirport.push_back(pTaxiwayClosure);
				}
			}
			break;
		case 3://CStandGroupClosureAtAirport
			{
				CStandGroupClosureAtAirport* pStandGroupClosure = new CStandGroupClosureAtAirport;
				if(pStandGroupClosure){
					pStandGroupClosure->ImportData(importFile);
					m_vrClosureAtAirport.push_back(pStandGroupClosure);
				}
			}
			break;
		default:
			return;
		}
	}

	importFile.getFile().getLine();
}

void CClosure::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_nObjectMark);

	exportFile.getFile().writeInt((int)m_vrClosureAtAirport.size());
	std::vector<CClosureAtAirport *>::iterator itrClosures;
	for (itrClosures = m_vrClosureAtAirport.begin();itrClosures != m_vrClosureAtAirport.end();++ itrClosures) {
		if(*itrClosures)
			(*itrClosures)->ExportData(exportFile); 
	}
	exportFile.getFile().writeLine();
}

void CClosure::Clear(void)
{
	std::vector<CClosureAtAirport *>::iterator itrClosures;
	for (itrClosures = m_vrClosureAtAirport.begin();itrClosures != m_vrClosureAtAirport.end();++ itrClosures) {
		if(*itrClosures)
			delete (*itrClosures); 
	}
	m_vrClosureAtAirport.clear();
	for (itrClosures = m_vrClosureAtAirportDeleted.begin();itrClosures != m_vrClosureAtAirportDeleted.end();++ itrClosures) {
		if(*itrClosures)
			delete (*itrClosures); 
	}
	m_vrClosureAtAirportDeleted.clear();
}


CAirsideRunwayClosure::CAirsideRunwayClosure(int nProjID):CClosure(nProjID)
{
	m_nObjectMark = 1;
}

CAirsideRunwayClosure::~CAirsideRunwayClosure(void)
{
}

CRunwayClosureAtAirport * CAirsideRunwayClosure::FindRunwayClosure(int nAirport)
{
	return ((CRunwayClosureAtAirport *)(CClosure::FindClosure(nAirport)));
}

int CAirsideRunwayClosure::GetAirportCount(void)
{
	return (CClosure::GetAirportCount());
}

CRunwayClosureAtAirport * CAirsideRunwayClosure::GetRunwayClosureAtAirport(int nIndex)
{
	return ((CRunwayClosureAtAirport *)(CClosure::GetClosureAtAirport(nIndex)));
}

bool CAirsideRunwayClosure::AddRunwayClosure(CRunwayClosureAtAirport * pNewNode)
{
	return (CClosure::AddClosure(pNewNode));
}

bool CAirsideRunwayClosure::DeleteRunwayClosure(CRunwayClosureAtAirport * pNodeDelete)
{
	return (CClosure::DeleteClosure(pNodeDelete));
}

CAirsideTaxiwayClosure::CAirsideTaxiwayClosure(int nProjID):CClosure(nProjID)
{
	m_nObjectMark = 2;
}

CAirsideTaxiwayClosure::~CAirsideTaxiwayClosure(void)
{
}

CTaxiwayClosureAtAirport* CAirsideTaxiwayClosure::FindTaxiwayClosure(int nAirport)
{
	return ((CTaxiwayClosureAtAirport *)(CClosure::FindClosure(nAirport)));
}

int CAirsideTaxiwayClosure::GetAirportCount(void)
{
	return (CClosure::GetAirportCount());
}

CTaxiwayClosureAtAirport * CAirsideTaxiwayClosure::GetTaxiwayClosureAtAirport(int nIndex)
{
	return ((CTaxiwayClosureAtAirport *)(CClosure::GetClosureAtAirport(nIndex)));
}

bool CAirsideTaxiwayClosure::AddTaxiwayClosure(CTaxiwayClosureAtAirport * pNewNode)
{
	return (CClosure::AddClosure(pNewNode));
}

bool CAirsideTaxiwayClosure::DeleteTaxiwayClosure(CTaxiwayClosureAtAirport * pNodeDelete)
{
	return (CClosure::DeleteClosure(pNodeDelete));
}


CAirsideStandGroupClosure::CAirsideStandGroupClosure(int nProjID):CClosure(nProjID)
{
	m_nObjectMark = 3;
}

CAirsideStandGroupClosure::~CAirsideStandGroupClosure(void)
{
}

CStandGroupClosureAtAirport* CAirsideStandGroupClosure::FindStandGroupClosure(int nAirport)
{
	return ((CStandGroupClosureAtAirport *)(CClosure::FindClosure(nAirport)));
}

int CAirsideStandGroupClosure::GetAirportCount(void)
{
	return (CClosure::GetAirportCount());
}

CStandGroupClosureAtAirport * CAirsideStandGroupClosure::GetStandGroupClosureAtAirport(int nIndex)
{
	return ((CStandGroupClosureAtAirport *)(CClosure::GetClosureAtAirport(nIndex)));
}

bool CAirsideStandGroupClosure::AddStandGroupClosure(CStandGroupClosureAtAirport * pNewNode)
{
	return (CClosure::AddClosure(pNewNode));
}

bool CAirsideStandGroupClosure::DeleteStandGroupClosure(CStandGroupClosureAtAirport * pNodeDelete)
{
	return (CClosure::DeleteClosure(pNodeDelete));
}

CAirsideSimSettingClosure::CAirsideSimSettingClosure(int nProjID):m_nProjID(nProjID),
m_bVehicleService(true),
m_bAllowCyclicGroundRoute(false),
m_nID(-1),
m_airsideRunwayClosure(nProjID),
m_airsideTaxiwayClosure(nProjID),
m_airsideStandGroupClosure(nProjID)
{	
	m_nItinerantFlight = 0;
	m_nTrainingFlight = 0;
}

CAirsideSimSettingClosure::~CAirsideSimSettingClosure(void)
{
	m_airsideRunwayClosure.Clear();
	m_airsideTaxiwayClosure.Clear();
	m_airsideStandGroupClosure.Clear();
}

void CAirsideSimSettingClosure::ReadData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM SIMENGINGSETTING WHERE (PRJID = %d);"),m_nProjID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	if(!adoRecordset.IsEOF())
	{
		int nPrjID = -1;
		int nVehicleService = -1;
		int nCyclicGroundRoute = -1;
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PRJID"),nPrjID);
		adoRecordset.GetFieldValue(_T("VEHICLESERVICE"),nVehicleService);
		adoRecordset.GetFieldValue(_T("CYCLICGROUNDROUTE"),nCyclicGroundRoute);

		if(!adoRecordset.GetFieldValue(_T("ITINERANTFLIGHT"), m_nItinerantFlight))
		{
			m_nItinerantFlight = 0;
		}

		if(!adoRecordset.GetFieldValue(_T("TRAININGFLIGHT"),m_nTrainingFlight))
		{
			m_nTrainingFlight = 0;
		}



		if(nVehicleService > 0)
			m_bVehicleService = true;
		else
			m_bVehicleService = false;

		if (nCyclicGroundRoute > 0)
		{
			m_bAllowCyclicGroundRoute = true;
		}else
		{
			m_bAllowCyclicGroundRoute = false;
		}
	}

	m_airsideRunwayClosure.ReadData();
	m_airsideTaxiwayClosure.ReadData();
	m_airsideStandGroupClosure.ReadData();
}

void CAirsideSimSettingClosure::SaveData(void)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CString strSQL = _T("");
		if(m_nID < 0){
			strSQL.Format(_T("INSERT INTO SIMENGINGSETTING (PRJID,VEHICLESERVICE,CYCLICGROUNDROUTE, ITINERANTFLIGHT, TRAININGFLIGHT) VALUES (%d,%d,%d, %d, %d);"),
				m_nProjID,int(m_bVehicleService),int(m_bAllowCyclicGroundRoute), m_nItinerantFlight, m_nTrainingFlight);

			m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		}
		else
		{
			strSQL.Format(_T("UPDATE SIMENGINGSETTING SET VEHICLESERVICE = %d, CYCLICGROUNDROUTE = %d, ITINERANTFLIGHT = %d, TRAININGFLIGHT = %d WHERE (PRJID = %d);"),int(m_bVehicleService),int(m_bAllowCyclicGroundRoute), m_nItinerantFlight, m_nTrainingFlight, m_nProjID);
			CADODatabase::ExecuteSQLStatement(strSQL);
		}
		m_airsideRunwayClosure.SaveData();
		m_airsideTaxiwayClosure.SaveData();
		m_airsideStandGroupClosure.SaveData();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}

void CAirsideSimSettingClosure::DeleteData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM SIMENGINGSETTING WHERE (PRJID = %d);"),m_nProjID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	m_airsideRunwayClosure.DeleteData();
	m_airsideTaxiwayClosure.DeleteData();
	m_airsideStandGroupClosure.DeleteData();
}

void CAirsideSimSettingClosure::ImportData(CAirsideImportFile& importFile)
{
	Clear();
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	int nVehicleService = -1;
	importFile.getFile().getInteger(nVehicleService);
	if (nVehicleService > 0)
		m_bVehicleService = true;
	else
		m_bVehicleService = false;

	m_airsideRunwayClosure.ImportData(importFile);
	m_airsideTaxiwayClosure.ImportData(importFile);
	m_airsideStandGroupClosure.ImportData(importFile);

	importFile.getFile().getLine();
}

void CAirsideSimSettingClosure::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(int(m_bVehicleService));
	m_airsideRunwayClosure.ExportData(exportFile);
	m_airsideTaxiwayClosure.ExportData(exportFile);
	m_airsideStandGroupClosure.ExportData(exportFile);

	exportFile.getFile().writeLine();
}

void CAirsideSimSettingClosure::Clear(void)
{
	m_airsideRunwayClosure.Clear();
	m_airsideTaxiwayClosure.Clear();
	m_airsideStandGroupClosure.Clear();
}

void CAirsideSimSettingClosure::ImportSimSettingClosure(CAirsideImportFile& importFile)
{
	CAirsideSimSettingClosure simClosure(importFile.getNewProjectID());
	simClosure.ImportData(importFile);
	simClosure.SaveData();
}

void CAirsideSimSettingClosure::ExportSimSettingClosure(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("SimulationSettingClosures");
	exportFile.getFile().writeLine();

	CAirsideSimSettingClosure simClosure(exportFile.GetProjectID());
	simClosure.ReadData();
	simClosure.ExportData(exportFile);

	exportFile.getFile().endFile();
}
CAirsideTaxiwayClosure& CAirsideSimSettingClosure::getTaxiwayClosure()
{
	return m_airsideTaxiwayClosure;
}

bool CAirsideSimSettingClosure::IsVehicleService()
{
	return m_bVehicleService;
}
bool CAirsideSimSettingClosure::AllowCyclicGroundRoute()
{
	return m_bAllowCyclicGroundRoute;
}

bool CAirsideSimSettingClosure::IsItinerantFlight() const
{
	if(m_nItinerantFlight > 0) 
		return true;

	return false;
}

void CAirsideSimSettingClosure::SetItinerantFlight( bool bEnable )
{
	if(bEnable)
		m_nItinerantFlight = 1; 
	else
		m_nItinerantFlight = 0;
}

bool CAirsideSimSettingClosure::IsTrainingFlight() const
{
	if(m_nTrainingFlight > 0)
		return true;
	return false;
}

void CAirsideSimSettingClosure::SetTrainingFlight( bool bEnable )
{
	if(bEnable)
		m_nTrainingFlight = 1;
	else
		m_nTrainingFlight = 0;
}
