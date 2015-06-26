#include "StdAfx.h"
#include ".\flightstarassignment.h"
#include "../Database/ADODatabase.h"

////////////////////////StarPercentage////////////////////////////////
StarPercentage::StarPercentage()
:m_nID(-1)
,m_nStarID(-1)
,m_nPercent(0)
{

}

StarPercentage::~StarPercentage()
{

}

void StarPercentage::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	int nParentID;
	adoRecordset.GetFieldValue(_T("PARENTID"),nParentID);
	adoRecordset.GetFieldValue(_T("STARID"),m_nStarID);
	adoRecordset.GetFieldValue(_T("PERCENTAGE"),m_nPercent);
}

void StarPercentage::SaveData(int nParentID)
{
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO STARPERCENT\
						 (PARENTID,STARID,PERCENTAGE)\
						 VALUES (%d,%d,%d)"),nParentID,m_nStarID,m_nPercent);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}

void StarPercentage::UpdateData(int nParentID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE STARPERCENT\
					 SET PARENTID =%d, STARID = %d,PERCENTAGE = %d\
					 WHERE (ID = %d)"), nParentID,  m_nStarID, m_nPercent, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void StarPercentage::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM STARPERCENT\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

/////////////////////////FlightStarAssignment////////////////////////////
FlightStarAssignment::FlightStarAssignment(void)
:m_nID(-1)
,m_pAirportDB(NULL)
{
	m_vStarPercentageList.clear();
	m_vStarPercentageDelList.clear();
}

FlightStarAssignment::~FlightStarAssignment(void)
{
	int nCount = GetSTARCount();
	for (int i = 0; i < nCount; i++)
	{
		StarPercentage* pData = m_vStarPercentageList.at(i);
		if (pData == NULL)
			continue;

		delete pData; 
		pData = NULL;
	}
	m_vStarPercentageList.clear();

	nCount = (int)m_vStarPercentageDelList.size();
	for (int j =0; j < nCount; j++)
	{
		StarPercentage* pData = m_vStarPercentageDelList.at(j);
		if (pData == NULL)
			continue;

		delete pData;
		pData = NULL;
	}
	m_vStarPercentageDelList.clear();
}

void FlightStarAssignment::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	int nPrjID;
	adoRecordset.GetFieldValue(_T("PROJID"),nPrjID);
	adoRecordset.GetFieldValue(_T("FLIGHTTYPE"),m_strFlightTypeName);
	long lTime;
	adoRecordset.GetFieldValue(_T("STARTTIME"),lTime);
	m_tStartTime = ElapsedTime(lTime);
	adoRecordset.GetFieldValue(_T("ENDTIME"),lTime);
	m_tEndTime = ElapsedTime(lTime);

	if (m_pAirportDB)
	{
		m_FlightType.SetAirportDB(m_pAirportDB);
		m_FlightType.setConstraintWithVersion(m_strFlightTypeName);
	}

	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM STARPERCENT\
					 WHERE (PARENTID = %d) "),m_nID);
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			StarPercentage* pData = new StarPercentage;
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vStarPercentageList.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void FlightStarAssignment::SaveData(int nPrjID)
{
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO FLIGHTSTARASSIGNMENT\
						 (PROJID, FLIGHTTYPE,STARTTIME,ENDTIME)\
						 VALUES (%d,'%s', %d, %d)"),nPrjID,m_strFlightTypeName,m_tStartTime.asSeconds(),m_tEndTime.asSeconds());

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

	std::vector<StarPercentage*>::iterator iter = m_vStarPercentageList.begin();

	for (;iter != m_vStarPercentageList.end(); ++iter)
	{
		if((*iter)->GetID() < 0)
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	for(iter = m_vStarPercentageDelList.begin(); iter != m_vStarPercentageDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vStarPercentageDelList.clear();
}

void FlightStarAssignment::UpdateData(int nPrjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE FLIGHTSTARASSIGNMENT\
					 SET PROJID =%d,FLIGHTTYPE = '%s',STARTTIME = %d,ENDTIME = %d\
					 WHERE (ID = %d)"),nPrjID,m_strFlightTypeName,m_tStartTime.asSeconds(),m_tEndTime.asSeconds(),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<StarPercentage *>::iterator iter = m_vStarPercentageList.begin();
	for (;iter != m_vStarPercentageList.end();++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	iter = m_vStarPercentageDelList.begin();
	for (;iter != m_vStarPercentageDelList.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vStarPercentageDelList.clear();
}

void FlightStarAssignment::DeleteData()
{
	std::vector<StarPercentage*>::iterator iter  = m_vStarPercentageList.begin();
	for (; iter != m_vStarPercentageList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (iter = m_vStarPercentageDelList.begin(); iter != m_vStarPercentageDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM FLIGHTSTARASSIGNMENT\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void FlightStarAssignment::AddStarItem(StarPercentage* pStarItem)
{
	if (pStarItem == NULL)
		return;
	m_vStarPercentageList.push_back(pStarItem);
}

void FlightStarAssignment::DeleteStarItem(StarPercentage* pStarItem)
{
	if (pStarItem == NULL)
		return;

	std::vector<StarPercentage*>::iterator iter = 
		std::find(m_vStarPercentageList.begin(),m_vStarPercentageList.end(),pStarItem);

	if (iter == m_vStarPercentageList.end())
		return;

	m_vStarPercentageDelList.push_back(pStarItem);
	m_vStarPercentageList.erase(iter);
}

int FlightStarAssignment::GetSTARCount()
{
	return (int)m_vStarPercentageList.size(); 
}

StarPercentage* FlightStarAssignment::GetStarItem(int nIndex)
{
	if (nIndex < 0 && nIndex > GetSTARCount()-1)
		return NULL;

	return m_vStarPercentageList.at(nIndex);
}

void FlightStarAssignment::setFlightType(const CString& strFltType)
{
	m_strFlightTypeName = strFltType;

	if (m_pAirportDB)
	{
		m_FlightType.SetAirportDB(m_pAirportDB);
		m_FlightType.setConstraintWithVersion(m_strFlightTypeName);
	}
}

/////////////////////////FlightStarAssignmentList/////////////////////////
FlightStarAssignmentList::FlightStarAssignmentList()
:m_nPrjID(-1)
,m_pAirportDB(NULL)
{
	m_vFlightStarAssignmentList.clear();
	m_vFlightStarAssignmentDelList.clear();
}

FlightStarAssignmentList::~FlightStarAssignmentList()
{
	int nCount = GetSTARCount();
	for (int i = 0; i < nCount; i++)
	{
		FlightStarAssignment* pData = m_vFlightStarAssignmentList.at(i);
		if (pData == NULL)
			continue;

		delete pData; 
		pData = NULL;
	}
	m_vFlightStarAssignmentList.clear();

	nCount = (int)m_vFlightStarAssignmentDelList.size();
	for (int j =0; j < nCount; j++)
	{
		FlightStarAssignment* pData = m_vFlightStarAssignmentDelList.at(j);
		if (pData == NULL)
			continue;

		delete pData;
		pData = NULL;
	}
	m_vFlightStarAssignmentDelList.clear();
}

void FlightStarAssignmentList::ReadData(int nPrjID)
{
	m_nPrjID = nPrjID;
	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM FLIGHTSTARASSIGNMENT\
					 WHERE (PROJID = %d) "),nPrjID);
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			FlightStarAssignment* pData = new FlightStarAssignment;
			pData->SetAirportDatabase(m_pAirportDB);
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vFlightStarAssignmentList.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void FlightStarAssignmentList::SaveData()
{
	std::vector<FlightStarAssignment*>::iterator iter  = m_vFlightStarAssignmentList.begin();
	for (; iter != m_vFlightStarAssignmentList.end(); ++iter)
	{
		if((*iter)->GetID() < 0)
			(*iter)->SaveData(m_nPrjID);
		else
			(*iter)->UpdateData(m_nPrjID);
	}

	for (iter = m_vFlightStarAssignmentDelList.begin(); iter != m_vFlightStarAssignmentDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vFlightStarAssignmentDelList.clear();
}

void FlightStarAssignmentList::UpdateData()
{

}

void FlightStarAssignmentList::DeleteData()
{
	std::vector<FlightStarAssignment*>::iterator iter  = m_vFlightStarAssignmentList.begin();
	for (; iter != m_vFlightStarAssignmentList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (iter = m_vFlightStarAssignmentDelList.begin(); iter != m_vFlightStarAssignmentDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
}

void FlightStarAssignmentList::AddStarItem(FlightStarAssignment* pStarItem)
{
	if (pStarItem == NULL)
		return;
	m_vFlightStarAssignmentList.push_back(pStarItem);
}

void FlightStarAssignmentList::DeleteStarItem(FlightStarAssignment* pStarItem)
{
	if (pStarItem == NULL)
		return;

	std::vector<FlightStarAssignment*>::iterator iter = 
		std::find(m_vFlightStarAssignmentList.begin(),m_vFlightStarAssignmentList.end(),pStarItem);

	if (iter == m_vFlightStarAssignmentList.end())
		return;

	m_vFlightStarAssignmentDelList.push_back(pStarItem);
	m_vFlightStarAssignmentList.erase(iter);
}

int FlightStarAssignmentList::GetSTARCount()
{
	return (int)m_vFlightStarAssignmentList.size(); 
}

FlightStarAssignment* FlightStarAssignmentList::GetStarItem(int nIndex)
{
	if (nIndex < 0 && nIndex > GetSTARCount()-1)
		return NULL;

	return m_vFlightStarAssignmentList.at(nIndex);
}

struct FlightStarAssignmentLess
{
	bool operator()(FlightStarAssignment* f1, FlightStarAssignment*f2)
	{
		if(f1 && f2)
		{
			return f1->GetFlightConstraint()<f2->GetFlightConstraint();
		}
		ASSERT(FALSE);
		return true;
	}
};

void FlightStarAssignmentList::Sort()
{
	std::sort(m_vFlightStarAssignmentList.begin(),m_vFlightStarAssignmentList.end(), FlightStarAssignmentLess() );
}
