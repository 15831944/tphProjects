#include "StdAfx.h"
#include ".\flightsidassignment.h"
#include "../Database/ADODatabase.h"

////////////////////////SIDPercentage////////////////////////////////
SIDPercentage::SIDPercentage()
:m_nID(-1)
,m_nSIDID(-1)
,m_nPercent(0)
{

}

SIDPercentage::~SIDPercentage()
{

}

void SIDPercentage::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	int nParentID;
	adoRecordset.GetFieldValue(_T("PARENTID"),nParentID);
	adoRecordset.GetFieldValue(_T("SIDID"),m_nSIDID);
	adoRecordset.GetFieldValue(_T("PERCENTAGE"),m_nPercent);
}

void SIDPercentage::SaveData(int nParentID)
{
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO SIDPERCENT\
						 (PARENTID,SIDID,PERCENTAGE)\
						 VALUES (%d,%d,%d)"),nParentID,m_nSIDID,m_nPercent);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}

void SIDPercentage::UpdateData(int nParentID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE SIDPERCENT\
					 SET PARENTID =%d, SIDID = %d,PERCENTAGE = %d\
					 WHERE (ID = %d)"), nParentID,  m_nSIDID, m_nPercent, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void SIDPercentage::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM SIDPERCENT\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

/////////////////////////FlightSIDAssignment////////////////////////////
FlightSIDAssignment::FlightSIDAssignment(void)
:m_nID(-1)
,m_pAirportDB(NULL)
{
	m_vSIDPercentageList.clear();
	m_vSIDPercentageDelList.clear();
}

FlightSIDAssignment::~FlightSIDAssignment(void)
{
	int nCount = GetSIDCount();
	for (int i = 0; i < nCount; i++)
	{
		SIDPercentage* pData = m_vSIDPercentageList.at(i);
		if (pData == NULL)
			continue;

		delete pData; 
		pData = NULL;
	}
	m_vSIDPercentageList.clear();

	nCount = (int)m_vSIDPercentageDelList.size();
	for (int j =0; j < nCount; j++)
	{
		SIDPercentage* pData = m_vSIDPercentageDelList.at(j);
		if (pData == NULL)
			continue;

		delete pData;
		pData = NULL;
	}
	m_vSIDPercentageDelList.clear();
}

void FlightSIDAssignment::ReadData(CADORecordset& adoRecordset)
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
					 FROM SIDPERCENT\
					 WHERE (PARENTID = %d) "),m_nID);
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			SIDPercentage* pData = new SIDPercentage;
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vSIDPercentageList.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void FlightSIDAssignment::SaveData(int nPrjID)
{
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO FLIGHTSIDASSIGNMENT\
						 (PROJID, FLIGHTTYPE,STARTTIME,ENDTIME)\
						 VALUES (%d,'%s', %d, %d)"),nPrjID,m_strFlightTypeName,m_tStartTime.asSeconds(),m_tEndTime.asSeconds());

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

	std::vector<SIDPercentage*>::iterator iter = m_vSIDPercentageList.begin();

	for (;iter != m_vSIDPercentageList.end(); ++iter)
	{
		if((*iter)->GetID() < 0)
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	for(iter = m_vSIDPercentageDelList.begin(); iter != m_vSIDPercentageDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vSIDPercentageDelList.clear();
}

void FlightSIDAssignment::UpdateData(int nPrjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE FLIGHTSIDASSIGNMENT\
					 SET PROJID =%d,FLIGHTTYPE = '%s',STARTTIME = %d,ENDTIME = %d\
					 WHERE (ID = %d)"),nPrjID,m_strFlightTypeName,m_tStartTime.asSeconds(),m_tEndTime.asSeconds(),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<SIDPercentage *>::iterator iter = m_vSIDPercentageList.begin();
	for (;iter != m_vSIDPercentageList.end();++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	iter = m_vSIDPercentageDelList.begin();
	for (;iter != m_vSIDPercentageDelList.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vSIDPercentageDelList.clear();
}

void FlightSIDAssignment::DeleteData()
{
	std::vector<SIDPercentage*>::iterator iter  = m_vSIDPercentageList.begin();
	for (; iter != m_vSIDPercentageList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (iter = m_vSIDPercentageDelList.begin(); iter != m_vSIDPercentageDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM FLIGHTSIDASSIGNMENT\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void FlightSIDAssignment::AddSIDItem(SIDPercentage* pSIDItem)
{
	if (pSIDItem == NULL)
		return;
	m_vSIDPercentageList.push_back(pSIDItem);
}

void FlightSIDAssignment::DeleteSIDItem(SIDPercentage* pSIDItem)
{
	if (pSIDItem == NULL)
		return;

	std::vector<SIDPercentage*>::iterator iter = 
		std::find(m_vSIDPercentageList.begin(),m_vSIDPercentageList.end(),pSIDItem);

	if (iter == m_vSIDPercentageList.end())
		return;

	m_vSIDPercentageDelList.push_back(pSIDItem);
	m_vSIDPercentageList.erase(iter);
}

int FlightSIDAssignment::GetSIDCount()
{
	return (int)m_vSIDPercentageList.size(); 
}

SIDPercentage* FlightSIDAssignment::GetSIDItem(int nIndex)
{
	if (nIndex < 0 && nIndex > GetSIDCount()-1)
		return NULL;

	return m_vSIDPercentageList.at(nIndex);
}

void FlightSIDAssignment::setFlightType(const CString& strFltType)
{
	m_strFlightTypeName = strFltType;

	if (m_pAirportDB)
	{
		m_FlightType.SetAirportDB(m_pAirportDB);
		m_FlightType.setConstraintWithVersion(m_strFlightTypeName);
	}
}

/////////////////////////FlightSIDAssignmentList/////////////////////////
FlightSIDAssignmentList::FlightSIDAssignmentList()
:m_nPrjID(-1)
,m_pAirportDB(NULL)
{
	m_vFlightSIDAssignmentList.clear();
	m_vFlightSIDAssignmentDelList.clear();
}

FlightSIDAssignmentList::~FlightSIDAssignmentList()
{
	int nCount = GetSIDCount();
	for (int i = 0; i < nCount; i++)
	{
		FlightSIDAssignment* pData = m_vFlightSIDAssignmentList.at(i);
		if (pData == NULL)
			continue;

		delete pData; 
		pData = NULL;
	}
	m_vFlightSIDAssignmentList.clear();

	nCount = (int)m_vFlightSIDAssignmentDelList.size();
	for (int j =0; j < nCount; j++)
	{
		FlightSIDAssignment* pData = m_vFlightSIDAssignmentDelList.at(j);
		if (pData == NULL)
			continue;

		delete pData;
		pData = NULL;
	}
	m_vFlightSIDAssignmentDelList.clear();
}

void FlightSIDAssignmentList::ReadData(int nPrjID)
{
	m_nPrjID = nPrjID;
	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM FLIGHTSIDASSIGNMENT\
					 WHERE (PROJID = %d) "),nPrjID);
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			FlightSIDAssignment* pData = new FlightSIDAssignment;
			pData->SetAirportDatabase(m_pAirportDB);
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vFlightSIDAssignmentList.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void FlightSIDAssignmentList::SaveData()
{
	std::vector<FlightSIDAssignment*>::iterator iter  = m_vFlightSIDAssignmentList.begin();
	for (; iter != m_vFlightSIDAssignmentList.end(); ++iter)
	{
		if((*iter)->GetID() < 0)
			(*iter)->SaveData(m_nPrjID);
		else
			(*iter)->UpdateData(m_nPrjID);
	}

	for (iter = m_vFlightSIDAssignmentDelList.begin(); iter != m_vFlightSIDAssignmentDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vFlightSIDAssignmentDelList.clear();
}

void FlightSIDAssignmentList::UpdateData()
{

}

void FlightSIDAssignmentList::DeleteData()
{
	std::vector<FlightSIDAssignment*>::iterator iter  = m_vFlightSIDAssignmentList.begin();
	for (; iter != m_vFlightSIDAssignmentList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (iter = m_vFlightSIDAssignmentDelList.begin(); iter != m_vFlightSIDAssignmentDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
}

void FlightSIDAssignmentList::AddSIDItem(FlightSIDAssignment* pSIDItem)
{
	if (pSIDItem == NULL)
		return;
	m_vFlightSIDAssignmentList.push_back(pSIDItem);
}

void FlightSIDAssignmentList::DeleteSIDItem(FlightSIDAssignment* pSIDItem)
{
	if (pSIDItem == NULL)
		return;

	std::vector<FlightSIDAssignment*>::iterator iter = 
		std::find(m_vFlightSIDAssignmentList.begin(),m_vFlightSIDAssignmentList.end(),pSIDItem);

	if (iter == m_vFlightSIDAssignmentList.end())
		return;

	m_vFlightSIDAssignmentDelList.push_back(pSIDItem);
	m_vFlightSIDAssignmentList.erase(iter);
}

int FlightSIDAssignmentList::GetSIDCount()
{
	return (int)m_vFlightSIDAssignmentList.size(); 
}

FlightSIDAssignment* FlightSIDAssignmentList::GetSIDItem(int nIndex)
{
	if (nIndex < 0 && nIndex > GetSIDCount()-1)
		return NULL;

	return m_vFlightSIDAssignmentList.at(nIndex);
}




struct FlightSIDAssignmentLess
{
	bool operator()(FlightSIDAssignment* f1, FlightSIDAssignment*f2)
	{
		if(f1 && f2)
		{
			return f1->GetFlightConstraint()<f2->GetFlightConstraint();
		}
		ASSERT(FALSE);
		return true;
	}
};

void FlightSIDAssignmentList::Sort()
{
	std::sort(m_vFlightSIDAssignmentList.begin(),m_vFlightSIDAssignmentList.end(), FlightSIDAssignmentLess() );
}

