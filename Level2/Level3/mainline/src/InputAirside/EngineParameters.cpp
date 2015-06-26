#include "stdafx.h"
#include "EngineParameters.h"
#include "../Common/ProbDistEntry.h"
#include "..\Common\AirportDatabase.h"

//////////////////////////////////////////////////////////////////////////
////CEngineParametersItem
CEngineParametersItem::CEngineParametersItem()
:m_nID(-1)
,m_dJetBlastAngle(15)
,m_dJetBlastDistanceStart(10000)
,m_dJetBlastDistanceTaxi(15000)
,m_dJetBlastDistanceTakeoff(30000)
{
	InitStartTime();
}

CEngineParametersItem::~CEngineParametersItem()
{
	
}

void CEngineParametersItem::InitStartTime()
{
	m_StartTime.setDistName("U[60~120]");
	m_StartTime.setProType(UNIFORM);
	m_StartTime.setPrintDist("Uniform:60;120");
	m_StartTime.initProbilityDistribution();
}
void CEngineParametersItem::SaveData(int nParentID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	m_StartTime.SaveData();
	CString strSQL =_T("");
	m_nProjID = nParentID;

	strSQL = _T("");
	char szFltType[1024] = {0};
	CString strFltType = _T("");

	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strFltType = szFltType;
	
	strSQL.Format(_T("INSERT INTO AIRSIDE_ENGINE_PARAMETERS(PROJID,FLTCONSTRAINT,JETBLASTANGLE,JETBLASTDISTANCESTART,JETBLASTDISTANCETAXI,JETBLASTDISTANCETAKEOFF,JETSTARTTIMEID) VALUES\
					 (%d,'%s',%f,%f,%f,%f,%d)"),m_nProjID,strFltType,m_dJetBlastAngle,m_dJetBlastDistanceStart,m_dJetBlastDistanceTaxi,m_dJetBlastDistanceTakeoff,\
					 m_StartTime.getID());
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void CEngineParametersItem::UpdateData()
{
	CString strSQL = _T("");

	char szFltType[1024] = {0};
	CString strFltType = _T("");

	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strFltType = szFltType;

	strSQL.Format(_T("UPDATE AIRSIDE_ENGINE_PARAMETERS SET PROJID = %d,FLTCONSTRAINT = '%s',JETBLASTANGLE = %f,JETBLASTDISTANCESTART = %f\
					 ,JETBLASTDISTANCETAXI = %f,JETBLASTDISTANCETAKEOFF = %f,JETSTARTTIMEID = %d WHERE ID = %d"),m_nProjID,strFltType,\
					 m_dJetBlastAngle,m_dJetBlastDistanceStart,m_dJetBlastDistanceTaxi,m_dJetBlastDistanceTakeoff,m_StartTime.getID(),m_nID);

	m_StartTime.UpdateData();
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CEngineParametersItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AIRSIDE_ENGINE_PARAMETERS WHERE (ID = %d);"),m_nID);

	m_StartTime.RemoveData();
	
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CEngineParametersItem::InitFromDBRecordset(CADORecordset& recordset)
{
	if (!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("ID"),m_nID);
		recordset.GetFieldValue(_T("PROJID"),m_nProjID);
		CString strFltType;
		recordset.GetFieldValue(_T("FLTCONSTRAINT"),strFltType);
		if(m_pAirportDB){
			m_fltType.SetAirportDB(m_pAirportDB);
			m_fltType.setConstraintWithVersion(strFltType.GetBuffer(strFltType.GetLength()+1));
			strFltType.ReleaseBuffer(strFltType.GetLength()+1);
		}

		recordset.GetFieldValue(_T("JETBLASTANGLE"),m_dJetBlastAngle);

		recordset.GetFieldValue(_T("JETBLASTDISTANCESTART"),m_dJetBlastDistanceStart);

		recordset.GetFieldValue(_T("JETBLASTDISTANCETAXI"),m_dJetBlastDistanceTaxi);

		recordset.GetFieldValue(_T("JETBLASTDISTANCETAKEOFF"),m_dJetBlastDistanceTakeoff);

		int nStartTimeID = -1;
		recordset.GetFieldValue(_T("JETSTARTTIMEID"),nStartTimeID);
		m_StartTime.ReadData(nStartTimeID);
	}
}

//////////////////////////////////////////////////////////////////////////
////CEngineParametersList
CEngineParametersList::CEngineParametersList(CAirportDatabase* pAirportDB)
:m_pAirportDB(pAirportDB)
{

}

CEngineParametersList::~CEngineParametersList()
{
	std::vector<CEngineParametersItem*>::iterator iter = m_vEngineItem.begin();
	for (; iter != m_vEngineItem.end(); ++iter)
	{
		delete (*iter);
	}

	iter = m_vDelEngineItem.begin();
	for (; iter != m_vDelEngineItem.end(); ++iter)
	{
		delete (*iter);
	}
	m_vEngineItem.clear();
	m_vDelEngineItem.clear();
}

void CEngineParametersList::ReadData(int nParentID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AIRSIDE_ENGINE_PARAMETERS WHERE (PROJID = %d);"),nParentID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	while (!adoRecordset.IsEOF())
	{
		CEngineParametersItem* pItem = new CEngineParametersItem();
		pItem->SetAirportDB(m_pAirportDB);
		pItem->InitFromDBRecordset(adoRecordset);
		m_vEngineItem.push_back(pItem);
		adoRecordset.MoveNextData();
	}

	if (m_vEngineItem.size() == 0)
	{
		CEngineParametersItem* pItem = new CEngineParametersItem();
		pItem->SetAirportDB(m_pAirportDB);
		m_vEngineItem.push_back(pItem);
	}
}

void CEngineParametersList::SaveData(int nParentID)
{
	std::vector<CEngineParametersItem*>::iterator iter = m_vEngineItem.begin();
	for (; iter != m_vEngineItem.end(); ++iter)
	{
		(*iter)->SaveData(nParentID);
	}

	iter = m_vDelEngineItem.begin();
	for (; iter != m_vDelEngineItem.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
}

void CEngineParametersList::addItem(CEngineParametersItem* pItem)
{
	VERIFY(pItem);
	m_vEngineItem.push_back(pItem);
}

bool CEngineParametersList::findItem(CEngineParametersItem* pItem)
{
	ASSERT(pItem);
	if(std::find(m_vEngineItem.begin(),m_vEngineItem.end(),pItem)!= m_vEngineItem.end())
		return true;

	return false;
}

CEngineParametersItem* CEngineParametersList::getItem(int ndx)
{
	ASSERT(ndx>=0 && ndx < getCount());
	return m_vEngineItem[ndx];
}

void CEngineParametersList::removeItem(CEngineParametersItem* pItem)
{
	ASSERT(pItem);
	std::vector<CEngineParametersItem*>::iterator iter;
	iter = std::find(m_vEngineItem.begin(),m_vEngineItem.end(),pItem);

	if (iter != m_vEngineItem.end())
	{
		m_vDelEngineItem.push_back(*iter);
		m_vEngineItem.erase(iter);
	}
}

int CEngineParametersList::getCount()
{
	return (int)m_vEngineItem.size();
}