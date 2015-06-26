#include "StdAfx.h"
#include ".\wingspanadjacencyconstraints.h"
#include "../Common/ARCFlight.h"
#include "../Common/AirportDatabase.h"
#include "../Common/ACTypesManager.h"


CWingspanAdjacencyConstraints::CWingspanAdjacencyConstraints(void)
{
	m_nID = -1;
}

CWingspanAdjacencyConstraints::~CWingspanAdjacencyConstraints(void)
{
}

CString CWingspanAdjacencyConstraints::TaxiwayNodeItem::getTaxiwayName() const
{
	return m_pTaxiway.GetObjectName().GetIDString();
}

CString CWingspanAdjacencyConstraints::TaxiwayNodeItem::getStartNodeName() const
{
	return m_startNode.GetName();
}

CString CWingspanAdjacencyConstraints::TaxiwayNodeItem::getEndNodeName() const
{
	return m_endNode.GetName();
}

bool CWingspanAdjacencyConstraints::TaxiwayNodeItem::FlightFit(const ARCFlight* pFlight)const
{	
	//need check 
	ASSERT(m_pAirportDB);

	char str[AC_TYPE_LEN];
	pFlight->getACType(str);

	CACType* anAircraft = NULL;
	anAircraft = _g_GetActiveACMan(m_pAirportDB)->getACTypeItem(str);

	if (anAircraft == NULL)
		return false;
	
	if (anAircraft->m_fSpan <= m_dMaxValue && anAircraft->m_fSpan >= m_dMinValue)
		return true;

	return false;
}

void CWingspanAdjacencyConstraints::TaxiwayNodeItem::SaveData()
{
	CString strSQL = _T("");
	if (m_nID != -1)
	{
		return UpdateData();
	}

	strSQL.Format(_T("INSERT INTO IN_TAXIWAY_NODEINFO(TWYID,STARTNODE,ENDNODE,MINVALUE,MAXVALUE) VALUES (%d,%d,%d,%f,%f)"),\
		m_pTaxiway.getID(),m_startNode.GetID(),m_endNode.GetID(),m_dMinValue,m_dMaxValue);
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CWingspanAdjacencyConstraints::TaxiwayNodeItem::ReadData(int nodeID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM IN_TAXIWAY_NODEINFO WHERE ID = %d"),nodeID);
	CADORecordset rs;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,rs);

	if(!rs.IsEOF())
	{
		m_nID = nodeID;
		int nTwyID = -1;
		rs.GetFieldValue(_T("TWYID"),nTwyID);
		m_pTaxiway.ReadObject(nTwyID);
		int startNode = -1;
		rs.GetFieldValue(_T("STARTNODE"),startNode);
		m_startNode.ReadData(startNode);
		int endNode = -1;
		rs.GetFieldValue(_T("ENDNODE"),endNode);
		m_endNode.ReadData(endNode);

		rs.GetFieldValue(_T("MINVALUE"),m_dMinValue);
		rs.GetFieldValue(_T("MAXVALUE"),m_dMaxValue);
	}
}

void CWingspanAdjacencyConstraints::TaxiwayNodeItem::RemoveData()
{
	if(m_nID >= 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("DELETE FROM IN_TAXIWAY_NODEINFO WHERE ID = %d"),m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

}

void CWingspanAdjacencyConstraints::TaxiwayNodeItem::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TAXIWAY_NODEINFO SET TWYID = %d,STARTNODE = %d,ENDNODE = %d,MINVALUE = %f,MAXVALUE = %f WHERE ID = %d"),\
		m_pTaxiway.getID(),m_startNode.GetID(),m_endNode.GetID(),m_dMinValue,m_dMaxValue,m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CWingspanAdjacencyConstraintsList::AddItem( CWingspanAdjacencyConstraints *pItem )
{
	m_vConstraints.push_back(pItem);
}

void CWingspanAdjacencyConstraints::SetAirportDB(CAirportDatabase* pAirportDB)
{
	m_firstaxiway.m_pAirportDB = pAirportDB;
	m_secondTaxiway.m_pAirportDB = pAirportDB;
}

void CWingspanAdjacencyConstraints::SaveData(int nParentID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	m_firstaxiway.SaveData();
	m_secondTaxiway.SaveData();
	CString strSQL =_T("");
	strSQL.Format(_T("INSERT INTO IN_TAXIWAY_ADJACENCY(PROJID,FIRSTID,SECONDID) VALUES\
					 (%d,%d,%d)"),nParentID,m_firstaxiway.m_nID,m_secondTaxiway.m_nID);
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CWingspanAdjacencyConstraints::UpdateData()
{
	CString strSQL = _T("");
	CString strPaxType = _T("");

	strSQL.Format(_T("UPDATE IN_TAXIWAY_ADJACENCY SET FIRSTID = %d,SECONDID = %d WHERE ID = %d"),m_firstaxiway.m_nID,m_secondTaxiway.m_nID,m_nID);

	m_firstaxiway.UpdateData();
	m_secondTaxiway.UpdateData();
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CWingspanAdjacencyConstraints::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_TAXIWAY_ADJACENCY WHERE (ID = %d);"),m_nID);

	m_firstaxiway.RemoveData();
	m_secondTaxiway.RemoveData();
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CWingspanAdjacencyConstraints::InitFromDBRecordset(CADORecordset& recordset)
{
	if (!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("ID"),m_nID);
		int nFirstID = -1;
		recordset.GetFieldValue(_T("FIRSTID"),nFirstID);
		m_firstaxiway.ReadData(nFirstID);
		int nSecondID = -1;
		recordset.GetFieldValue(_T("SECONDID"),nSecondID);
		m_secondTaxiway.ReadData(nSecondID);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
///implement for CWingspanAdjacencyConstraintsList
CWingspanAdjacencyConstraintsList::CWingspanAdjacencyConstraintsList(CAirportDatabase* pAirportDB)
:m_pAirportDB(pAirportDB)
{

}

CWingspanAdjacencyConstraintsList::~CWingspanAdjacencyConstraintsList()
{
	ClearMemory();
}

void CWingspanAdjacencyConstraintsList::ClearMemory()
{
	std::vector<CWingspanAdjacencyConstraints *>::iterator iter = m_vConstraints.begin();
	for (; iter != m_vConstraints.end(); ++iter)
	{
		delete (*iter);
	}
	m_vConstraints.clear();
}

void CWingspanAdjacencyConstraintsList::DelItem( CWingspanAdjacencyConstraints *pItem )
{
	std::vector<CWingspanAdjacencyConstraints *>::iterator iter = std::find(m_vConstraints.begin(), m_vConstraints.end(), pItem);
	if(iter != m_vConstraints.end())
	{
		m_vConstraints.erase(iter);
		m_vDelConstraints.push_back(*iter);
	}
}

int CWingspanAdjacencyConstraintsList::GetCount()const
{
	return (int)m_vConstraints.size();
}

CWingspanAdjacencyConstraints* CWingspanAdjacencyConstraintsList::GetConstraint(int idx)const
{
	ASSERT(idx >=0 && idx < GetCount());
	return m_vConstraints[idx];
}

void CWingspanAdjacencyConstraintsList::LoadData(int nProjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM IN_TAXIWAY_ADJACENCY WHERE (PROJID = %d);"),nProjID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	while (!adoRecordset.IsEOF())
	{
		CWingspanAdjacencyConstraints* pItem = new CWingspanAdjacencyConstraints();
		pItem->InitFromDBRecordset(adoRecordset);
		pItem->SetAirportDB(m_pAirportDB);
		m_vConstraints.push_back(pItem);
		adoRecordset.MoveNextData();
	}
}

void CWingspanAdjacencyConstraintsList::SaveData(int nProjID)
{
	std::vector<CWingspanAdjacencyConstraints*>::iterator iter = m_vConstraints.begin();
	for (; iter != m_vConstraints.end(); ++iter)
	{
		(*iter)->SaveData(nProjID);
	}

	iter = m_vDelConstraints.begin();
	for (; iter != m_vDelConstraints.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
}
