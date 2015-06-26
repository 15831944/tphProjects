#include "StdAfx.h"
#include ".\standcriteriaassignment.h"
#include "../Database/ADODatabase.h"
#include "ALTAirport.h"
#include "Stand.h"
///////////////////FlightTypeStandAssignment
FlightTypeStandAssignment::FlightTypeStandAssignment()
:m_nID(-1)
{

}

FlightTypeStandAssignment::~FlightTypeStandAssignment()
{

}

//void FlightTypeStandAssignment::setFlightType(const CString& strFltType)
//{
//	m_strFlightTypeName = strFltType;
//
//	if (m_pAirportDB)
//	{
//		m_FlightType.SetAirportDB(m_pAirportDB);
//		m_FlightType.setConstraintWithVersion(m_strFlightTypeName);
//	}
//}

void FlightTypeStandAssignment::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("FLIGHTTYPESTAND_ASSIGNMENTID"),m_nID);
	adoRecordset.GetFieldValue(_T("LEADINLINEID"),m_nLeadInLineID);
	adoRecordset.GetFieldValue(_T("LEADOUTLINEID"),m_nLeadOutLineID);

	CString m_strFlightTypeName;
	CString strFlightType = _T("");
	adoRecordset.GetFieldValue(_T("FLIGHTTYPE"),m_strFlightTypeName,false);

	if (m_pAirportDB)
	{
		m_FlightType.SetAirportDB(m_pAirportDB);
		m_FlightType.setConstraintWithVersion(m_strFlightTypeName);
	}
}

void FlightTypeStandAssignment::SaveData()
{
	//CString m_strFlightTypeName;	
	CString strFltType;
	m_FlightType.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));
	
	if (m_nID != -1)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO FLIGHTTYPESTAND_ASSIGNMENT(LEADINLINEID,LEADOUTLINEID,FLIGHTTYPE)\
					 VALUES(%d,%d,'%s')"),m_nLeadInLineID,m_nLeadOutLineID,strFltType.GetString());

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void FlightTypeStandAssignment::UpdateData()
{
	CString strFltType;
	m_FlightType.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));

	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE FLIGHTTYPESTAND_ASSIGNMENT SET LEADINLINEID = %d,LEADOUTLINEID = %d,FLIGHTTYPE = '%s'\
					 WHERE ID = %d"),m_nLeadInLineID,m_nLeadOutLineID,strFltType.GetString(),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void FlightTypeStandAssignment::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM FLIGHTTYPESTAND_ASSIGNMENT WHERE ID = %d"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

CString FlightTypeStandAssignment::GetFlightTypeScreenString() const
{
	CString str; 
	m_FlightType.screenFltPrint(str); 
	return str;
}

void FlightTypeStandAssignment::SetFlightType( const FlightConstraint& strFlightType )
{
	m_FlightType = strFlightType;
}
////////////////////////StandCriteriaAssignment
StandCriteriaAssignment::StandCriteriaAssignment(void)
:m_nStandId(-1)
{
}

StandCriteriaAssignment::~StandCriteriaAssignment(void)
{
	std::vector<FlightTypeStandAssignment*>::iterator iter = m_vFlightTypeStandAssignmentList.begin();
	for(; iter != m_vFlightTypeStandAssignmentList.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vDelFlightTypeStandAssignmentList.begin();
	for (; iter != m_vDelFlightTypeStandAssignmentList.end(); ++iter)
	{
		delete *iter;
	}
}

void StandCriteriaAssignment::removeItem(int nIndex)
{
	ASSERT(nIndex >=0 && nIndex < getCount());

	m_vFlightTypeStandAssignmentList.push_back(m_vFlightTypeStandAssignmentList[nIndex]);
	m_vFlightTypeStandAssignmentList.erase(m_vFlightTypeStandAssignmentList.begin() + nIndex);
}

void StandCriteriaAssignment::removeItem(FlightTypeStandAssignment* pFlightType)
{
	if (pFlightType == NULL)
		return;

	std::vector<FlightTypeStandAssignment*>::iterator iter = 
		std::find(m_vFlightTypeStandAssignmentList.begin(),m_vFlightTypeStandAssignmentList.end(),pFlightType);

	if (iter == m_vFlightTypeStandAssignmentList.end())
		return;

	m_vDelFlightTypeStandAssignmentList.push_back(pFlightType);
	m_vFlightTypeStandAssignmentList.erase(iter);
}

void StandCriteriaAssignment::clear()
{
	m_vDelFlightTypeStandAssignmentList.insert(m_vDelFlightTypeStandAssignmentList.end(),\
		m_vFlightTypeStandAssignmentList.begin(),m_vFlightTypeStandAssignmentList.end());

	m_vFlightTypeStandAssignmentList.clear();
}

void StandCriteriaAssignment::SaveData(int nProjID)
{
	std::vector<FlightTypeStandAssignment*>::iterator iter = m_vFlightTypeStandAssignmentList.begin();
	for (;iter != m_vFlightTypeStandAssignmentList.end(); ++iter)
	{
		(*iter)->SaveData();
	}
	iter = m_vDelFlightTypeStandAssignmentList.begin();
	for (; iter != m_vDelFlightTypeStandAssignmentList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vDelFlightTypeStandAssignmentList.clear();
}

void StandCriteriaAssignment::DeleteData()
{
	std::vector<FlightTypeStandAssignment*>::iterator iter = m_vFlightTypeStandAssignmentList.begin();
	for (; iter != m_vFlightTypeStandAssignmentList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
	iter = m_vDelFlightTypeStandAssignmentList.begin();
	for (; iter != m_vDelFlightTypeStandAssignmentList.end(); ++iter)
	{
		if (*iter)
		{
			(*iter)->DeleteData();
			delete *iter;
		}
	}
	m_vDelFlightTypeStandAssignmentList.clear();
}

bool FlightTypeCompare(FlightTypeStandAssignment* fItem,FlightTypeStandAssignment* sItem)
{
	if (fItem->GetFlightConstraint().fits(sItem->GetFlightConstraint()))
		return false;
	else if (sItem->GetFlightConstraint().fits(fItem->GetFlightConstraint()))
		return true;

	return false;
}


void StandCriteriaAssignment::SortFlightType()
{
	std::sort(m_vFlightTypeStandAssignmentList.begin(),m_vFlightTypeStandAssignmentList.end(),FlightTypeCompare);
}

////////////////////////////StandCriteriaAssignmentList
StandCriteriaAssignmentList::StandCriteriaAssignmentList()
{

}

StandCriteriaAssignmentList::~StandCriteriaAssignmentList()
{
	std::vector<StandCriteriaAssignment*>::iterator iter = m_vStandCriteriaAssignmentList.begin();
	for (; iter != m_vStandCriteriaAssignmentList.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vDelStandCriteriaAssignmentList.begin();
	for (; iter != m_vDelStandCriteriaAssignmentList.end(); ++iter)
	{
		delete *iter;
	}
}

void StandCriteriaAssignmentList::ReadData(int nPrjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT STANDID,FLIGHTTYPESTAND_ASSIGNMENT.ID AS FLIGHTTYPESTAND_ASSIGNMENTID, LEADINLINEID,LEADOUTLINEID, \
					 FLIGHTTYPE FROM FLIGHTTYPESTAND_ASSIGNMENT INNER JOIN LEADINLINE \
					 ON FLIGHTTYPESTAND_ASSIGNMENT.LEADINLINEID = LEADINLINE.ID"));

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	while (!adoRecordset.IsEOF())
	{
		int nStandID = -1;
		FlightTypeStandAssignment* pFlightData = new FlightTypeStandAssignment();
		pFlightData->SetAirportDatabase(m_pAirportDB);
		pFlightData->ReadData(adoRecordset);
		adoRecordset.GetFieldValue(_T("STANDID"),nStandID);
		StandCriteriaAssignment* pStandData = findItem(nStandID);
		if (pStandData)
		{
			pStandData->addItem(pFlightData);
		}
		else
		{
			pStandData = new StandCriteriaAssignment();
			pStandData->SetAirportDatabase(m_pAirportDB);
			pStandData->setStandID(nStandID);
			pStandData->setProjID(nPrjID);
			
			pStandData->addItem(pFlightData);
			addItem(pStandData);
		}
		adoRecordset.MoveNextData();
	}
	SortFlightType();
}

void StandCriteriaAssignmentList::SaveData(int nProjID)
{
	std::vector<StandCriteriaAssignment*>::iterator iter = m_vStandCriteriaAssignmentList.begin();
	for (; iter != m_vStandCriteriaAssignmentList.end(); ++iter)
	{
		(*iter)->SaveData(nProjID);
	}
	iter = m_vDelStandCriteriaAssignmentList.begin();
	for(; iter != m_vDelStandCriteriaAssignmentList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vDelStandCriteriaAssignmentList.clear();
}

void StandCriteriaAssignmentList::removeItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < getCount());

	m_vDelStandCriteriaAssignmentList.push_back(m_vStandCriteriaAssignmentList[nIndex]);
	m_vStandCriteriaAssignmentList.erase(m_vStandCriteriaAssignmentList.begin() + nIndex);
}

bool StandCriteriaAssignmentList::IsExistInStandCriteriaList(StandCriteriaAssignment* pStandCriteria)
{
	for (int i =0; i < getCount(); i++)
	{
		if (pStandCriteria->getStandID()==getItem(i)->getStandID()\
			&& pStandCriteria != getItem(i))
		{
			return true;
		}
	}
	return false;
}

void StandCriteriaAssignmentList::removeItem(StandCriteriaAssignment* pStandCriteria)
{
	if (pStandCriteria == NULL)
		return;

	std::vector<StandCriteriaAssignment*>::iterator iter = 
		std::find(m_vStandCriteriaAssignmentList.begin(),m_vStandCriteriaAssignmentList.end(),pStandCriteria);

	if (iter == m_vStandCriteriaAssignmentList.end())
		return;

	m_vDelStandCriteriaAssignmentList.push_back(pStandCriteria);
	m_vStandCriteriaAssignmentList.erase(iter);
}

StandCriteriaAssignment* StandCriteriaAssignmentList::fit(Stand* pStand)
{
	for (int i =0; i < getCount(); i++)
	{
		if (getItem(i)->getStandID() == pStand->getID())
		{
			return getItem(i);
		}
	}
	return NULL;
}

StandCriteriaAssignment* StandCriteriaAssignmentList::findItem(int nStandID)
{
	std::vector<StandCriteriaAssignment*>::iterator iter = m_vStandCriteriaAssignmentList.begin();
	for (; iter != m_vStandCriteriaAssignmentList.end(); ++iter)
	{
		if (nStandID == (*iter)->getStandID())
		{
			return *iter;
		}
	}

	return NULL;
}

void StandCriteriaAssignmentList::SortFlightType()
{
	std::vector<StandCriteriaAssignment*>::iterator iter = m_vStandCriteriaAssignmentList.begin();
	for (; iter != m_vStandCriteriaAssignmentList.end(); ++iter)
	{
		(*iter)->SortFlightType();
	}
}