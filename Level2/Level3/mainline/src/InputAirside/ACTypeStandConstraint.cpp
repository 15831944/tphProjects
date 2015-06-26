#include "StdAfx.h"
#include "ACTypeStandConstraint.h"
#include "../Database/ADODatabase.h"
#include "../Common/ARCFlight.h"


ACTypeStandConstraint::ACTypeStandConstraint(void)
:m_nID(-1)
{
	m_strACTypeList.RemoveAll();
}

ACTypeStandConstraint::~ACTypeStandConstraint(void)
{
}

bool SortData(ACTypeStandConstraint* pData1, ACTypeStandConstraint* pData2)
{
	ALTObjectID stand1 = pData1->GetStandGroup().getName();
	ALTObjectID stand2 = pData1->GetStandGroup().getName();
	if(stand2.idFits(stand1))
		return false;
	if(stand1.idFits(stand2))
	{
		return true;
	}

	return  stand1 < stand2;
}

void ACTypeStandConstraint::InitACTypesFromString(CString strACTypes)
{
	int nIdx =0;
	int nNext = 0;
	int nLength = strACTypes.GetLength();
	while(nIdx < nLength && nNext > -1)
	{
		nNext = strACTypes.Find(';',nIdx);
		CString strAC = strACTypes.Mid(nIdx,nNext-nIdx);
		if (strAC.IsEmpty())
		{
			nIdx++;
			continue;
		}
		strAC.Trim();
		m_strACTypeList.AddTail(strAC);
		nIdx = nNext+1;
	}
}

void ACTypeStandConstraint::ReadData(CADORecordset& adoRecord)
{
	if (adoRecord.IsEOF())
		return;

	adoRecord.GetFieldValue("ID",m_nID);
	int nStandID;
	adoRecord.GetFieldValue("STANDID",nStandID);
	CString strACTypes = "";
	adoRecord.GetFieldValue("ACTYPES",strACTypes);
	m_standGroup.ReadData(nStandID);
	InitACTypesFromString(strACTypes);
}

void ACTypeStandConstraint::SaveData()
{
	if (m_nID >=0 )
	{
		return UpdateData();
	}

	m_standGroup.SaveData(m_standGroup.GetProjID());

	POSITION pos;
	CString strACTypes = "";
	for( pos = m_strACTypeList.GetHeadPosition(); pos != NULL; )
	{
		CString strAC = m_strACTypeList.GetNext( pos );
		strACTypes += strAC +";"; 
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO ACTYPESTANDCONSTRAINT (STANDID, ACTYPES)	\
					 VALUES (%d,'%s')"), \
					 m_standGroup.getID(),strACTypes);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void ACTypeStandConstraint::UpdateData()
{
	m_standGroup.UpdateData();

	POSITION pos;
	CString strACTypes = "";
	for( pos = m_strACTypeList.GetHeadPosition(); pos != NULL; )
	{
		CString strAC = m_strACTypeList.GetNext( pos );
		strACTypes += strAC +";"; 
	}

	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE ACTYPESTANDCONSTRAINT SET STANDID= %d,ACTYPES= '%s' WHERE ID= %d"), m_standGroup.getID(),strACTypes, m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ACTypeStandConstraint::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format("DELETE FROM ACTYPESTANDCONSTRAINT WHERE ID = %d", m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	m_standGroup.DeleteData();
}

const ALTObjectGroup& ACTypeStandConstraint::GetStandGroup()
{
	return m_standGroup;
}

const CStringList& ACTypeStandConstraint::GetACTypeList()
{
	return m_strACTypeList;
}

bool ACTypeStandConstraint::Contains(const CString& strACType)
{
	POSITION pos;
	for( pos = m_strACTypeList.GetHeadPosition(); pos != NULL; )
	{
		CString strType = m_strACTypeList.GetNext(pos);
		if (strType.CompareNoCase("All AC Types") ==0)
			return true;

		strType.Trim();

		if (strType.CompareNoCase(strACType) ==0)
			return true;		
	}

	return false;
}

void ACTypeStandConstraint::SetStandGroupName( ALTObjectID objID )
{
	m_standGroup.setName(objID);
}

void ACTypeStandConstraint::AddACType(CString ACTypeName)
{
	m_strACTypeList.AddTail(ACTypeName);
}

CString ACTypeStandConstraint::GetStandGroupIDString()
{
	return m_standGroup.getName().GetIDString();
}

////////////////////////////////////////////////////////////////////////////////////////
ACTypeStandConstraintList::ACTypeStandConstraintList(void)
{
	m_vDataList.clear();
	m_vDelDataList.clear();
}

ACTypeStandConstraintList::~ACTypeStandConstraintList(void)
{
	RemoveAll();
}

void ACTypeStandConstraintList::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM ACTYPESTANDCONSTRAINT"));

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	if (adoRecordset.IsEOF())
	{
		ACTypeStandConstraint* pData = new ACTypeStandConstraint;
		ALTObjectID objID("");
		pData->SetStandGroupName(objID);
		CString strAC = "All AC Types";
		pData->AddACType(strAC);
		m_vDataList.push_back(pData);
	}

	while (!adoRecordset.IsEOF())
	{
		ACTypeStandConstraint* pData = new ACTypeStandConstraint;
		pData->ReadData(adoRecordset);
		m_vDataList.push_back(pData);
		adoRecordset.MoveNextData();
	}
	std::sort(m_vDataList.begin(),m_vDataList.end(),SortData);
}

void ACTypeStandConstraintList::SaveData()
{

		std::vector<ACTypeStandConstraint*>::iterator iter = m_vDataList.begin();
		for (; iter != m_vDataList.end(); iter++)
		{
			(*iter)->SaveData();
		}

		for (iter = m_vDelDataList.begin(); iter != m_vDelDataList.end(); iter++)
		{
			(*iter)->DeleteData();

			delete *iter;
			*iter = NULL;
		}
	
	m_vDelDataList.clear();
}

void ACTypeStandConstraintList::RemoveAll()
{
	std::vector<ACTypeStandConstraint*>::iterator iter = m_vDataList.begin();
	for (; iter != m_vDataList.end(); iter++)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vDataList.clear();

	iter = m_vDelDataList.begin();
	for (; iter != m_vDelDataList.end(); iter++)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vDelDataList.clear();
}

int ACTypeStandConstraintList::GetConstaintCount()
{
	return (int)m_vDataList.size();
}

ACTypeStandConstraint* ACTypeStandConstraintList::GetItemByIndex(int nIdx)
{
	if (nIdx <0 || nIdx > GetConstaintCount() -1)
		return NULL;

	return m_vDataList.at(nIdx);
}


void ACTypeStandConstraintList::DeleteItemByConstraint(ACTypeStandConstraint* pConst)
{
	if (pConst == NULL)
		return;

	int nCount = GetConstaintCount();
	for (int i =0; i < nCount; i++)
	{
		if (m_vDataList.at(i) == pConst)
		{
			m_vDelDataList.push_back(pConst);
			m_vDataList.erase(m_vDataList.begin() +i);
			return;
		}
	}
}

bool ACTypeStandConstraintList::IsFlightFit(const ARCFlight* flight, const ALTObjectID& gate)
{
	int nCount = GetConstaintCount();
	if (nCount ==0)
		return false;

	bool bStandFit = false;

	for (int i =0; i < nCount; i++)
	{
		ACTypeStandConstraint* pData = m_vDataList.at(i);
		if (gate.idFits(pData->GetStandGroup().getName()))
		{
			bStandFit = true;
			char strAC[AC_TYPE_LEN];
			flight->getACType(strAC);
			if (pData->Contains(strAC))
				return true;
		}
	}
	if (bStandFit)
		return false;

	return true;
}

void ACTypeStandConstraintList::DeleteItemByIndex(int nIdx)
{
	if (nIdx <0 || nIdx > GetConstaintCount() -1)
		return;
	m_vDelDataList.push_back(*(m_vDataList.begin()+nIdx));
	m_vDataList.erase(m_vDataList.begin()+nIdx);
}

void ACTypeStandConstraintList::AddItem(ACTypeStandConstraint* pConst)
{
	m_vDataList.push_back(pConst);
	std::sort(m_vDataList.begin(),m_vDataList.end(),SortData);
}

void ACTypeStandConstraintList::DeleteAllItem()
{
	int count = (int)m_vDataList.size();
	for(int i=count; i>0; i--)
	{
		m_vDelDataList.push_back(*(m_vDataList.end()-1));
		m_vDataList.pop_back();
	}
}

void ACTypeStandConstraintList::ClearConstraintPtrs()
{
	m_vDataList.clear();
	m_vDelDataList.clear();
}
