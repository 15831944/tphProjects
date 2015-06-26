#include "StdAfx.h"
#include ".\sectorflightmixspecification.h"

#include "..\Database\ADODatabase.h"

//////////////////////////ACTypeLimitation/////////////////////////////
ACTypeLimitation::ACTypeLimitation()
:m_nId(-1)
,m_nMaxLimit(-1)
,m_emAcCategory(WakeVortexWightBased)
,m_nACTypeID(0)
{

}

ACTypeLimitation::~ACTypeLimitation()
{

}

void ACTypeLimitation::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nId);
	int nParentID;
	adoRecordset.GetFieldValue(_T("PARENTID"),nParentID);
	int nACCategory;
	adoRecordset.GetFieldValue(_T("ACCATEGORY"),nACCategory);
	m_emAcCategory = (FlightClassificationBasisType)nACCategory;
	adoRecordset.GetFieldValue(_T("ACTYPEID"),m_nACTypeID);
	adoRecordset.GetFieldValue(_T("MAXLIMIT"),m_nMaxLimit);
}

void ACTypeLimitation::SaveData(int nParentID)
{
	if (m_nId < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO ACTYPELIMITATION\
						 (PARENTID, ACCATEGORY,ACTYPEID,MAXLIMIT)\
						 VALUES (%d,%d,%d,%d)"),nParentID,(int)m_emAcCategory,m_nACTypeID, m_nMaxLimit);

		m_nId =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}

void ACTypeLimitation::UpdateData(int nParentID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE ACTYPELIMITATION\
					 SET PARENTID =%d, ACCATEGORY =%d,ACTYPEID = %d, MAXLIMIT = %d\
					 WHERE (ID = %d)"), nParentID, (int)m_emAcCategory, m_nACTypeID, m_nMaxLimit, m_nId);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ACTypeLimitation::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ACTYPELIMITATION\
					 WHERE (ID = %d)"),m_nId);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

int ACTypeLimitation::GetID()
{
	return m_nId;
}

int ACTypeLimitation::GetACTypeID()
{
	return m_nACTypeID;
}

FlightClassificationBasisType ACTypeLimitation::GetACCategory()
{
	return m_emAcCategory;
}

int ACTypeLimitation::GetMaxLimit()
{
	return m_nMaxLimit;
}

///////////////////////////////////SectorAltitudeLimitation/////////////////////////////////////
SectorAltitudeLimitation::SectorAltitudeLimitation()
:m_nID(-1)
,m_nMaxLimit(-1)
,m_enumAltitudeType(AnyAltitude)
{
	m_vACTypeLimitDelList.clear();
	m_vACTypeLimitList.clear();
}

SectorAltitudeLimitation::~SectorAltitudeLimitation()
{
	int nCount = (int)m_vACTypeLimitList.size();
	for (int i = 0; i < nCount; i++)
	{
		ACTypeLimitation* pData = m_vACTypeLimitList.at(i);
		if (pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vACTypeLimitList.clear();

	nCount = (int)m_vACTypeLimitDelList.size();
	for (int j = 0; j < nCount; j++)
	{
		ACTypeLimitation* pData = m_vACTypeLimitDelList.at(j);
		if (pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vACTypeLimitDelList.clear();
}

void SectorAltitudeLimitation::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	int nParentID;
	adoRecordset.GetFieldValue(_T("PARENTID"),nParentID);
	int nAltType;
	adoRecordset.GetFieldValue(_T("ALTTYPE"),nAltType);
	m_enumAltitudeType = (AltitudeType)nAltType;
	adoRecordset.GetFieldValue(_T("MAXLIMIT"),m_nMaxLimit);


	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM ACTYPELIMITATION\
					 WHERE (PARENTID = %d) "),m_nID);
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			ACTypeLimitation* pData = new ACTypeLimitation;
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vACTypeLimitList.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void SectorAltitudeLimitation::SaveData(int nParentID)
{
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO SECTORLIMITATION\
						 (PARENTID, ALTTYPE,MAXLIMIT)\
						 VALUES (%d,%d,%d)"),nParentID,(int)m_enumAltitudeType, m_nMaxLimit);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

	std::vector<ACTypeLimitation*>::iterator iter = m_vACTypeLimitList.begin();

	for (;iter != m_vACTypeLimitList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	for(iter = m_vACTypeLimitDelList.begin(); iter != m_vACTypeLimitDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vACTypeLimitDelList.clear();
}

void SectorAltitudeLimitation::UpdateData(int nParentID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE SECTORLIMITATION\
					 SET PARENTID =%d, ALTTYPE =%d,MAXLIMIT = %d\
					 WHERE (ID = %d)"), nParentID, (int)m_enumAltitudeType, m_nMaxLimit, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<ACTypeLimitation *>::iterator iter = m_vACTypeLimitList.begin();
	for (;iter != m_vACTypeLimitList.end();++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	iter = m_vACTypeLimitDelList.begin();
	for (;iter != m_vACTypeLimitDelList.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vACTypeLimitDelList.clear();
}

void SectorAltitudeLimitation::DeleteData()
{
	std::vector<ACTypeLimitation*>::iterator iter  = m_vACTypeLimitList.begin();
	for (; iter != m_vACTypeLimitList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (iter = m_vACTypeLimitDelList.begin(); iter != m_vACTypeLimitDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM SECTORLIMITATION\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

int SectorAltitudeLimitation::GetID()
{
	return m_nID;
}

int SectorAltitudeLimitation::GetMaxLimit()
{
	return m_nMaxLimit;
}

SectorAltitudeLimitation::AltitudeType SectorAltitudeLimitation::GetAltitudeType()
{
	return m_enumAltitudeType;
}

int SectorAltitudeLimitation::GetACTypeLimitCount()
{
	return (int)m_vACTypeLimitList.size();
}

ACTypeLimitation* SectorAltitudeLimitation::GetACTypeLimitByIdx(int nIdx)
{
	if (nIdx >= (int)m_vACTypeLimitList.size())
		return NULL;

	return m_vACTypeLimitList.at(nIdx);
}

void SectorAltitudeLimitation::AddACTypeLimit(ACTypeLimitation* pItem)
{
	if (pItem == NULL)
		return;

	m_vACTypeLimitList.push_back(pItem);
}

void SectorAltitudeLimitation::DelACTypeLimit(ACTypeLimitation* pItem)
{
	if (pItem == NULL)
		return;

	std::vector<ACTypeLimitation*>::iterator iter = 
		std::find(m_vACTypeLimitList.begin(),m_vACTypeLimitList.end(),pItem);

	if (iter == m_vACTypeLimitList.end())
		return;

	m_vACTypeLimitDelList.push_back(pItem);
	m_vACTypeLimitList.erase(iter);
}

////////////////////////////////////SectorLimitation//////////////////////////
SectorLimitation::SectorLimitation()
:m_nID(-1)
,m_nSectorID(-1)
{
	m_vAltitudeLimitDelList.clear();
	m_vAltitudeLimitList.clear();
}

SectorLimitation::~SectorLimitation()
{
	int nCount = (int)m_vAltitudeLimitList.size();
	for (int i = 0; i < nCount; i++)
	{
		SectorAltitudeLimitation* pData = m_vAltitudeLimitList.at(i);
		if (pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vAltitudeLimitList.clear();

	nCount = (int)m_vAltitudeLimitDelList.size();
	for (int j = 0; j < nCount; j++)
	{
		SectorAltitudeLimitation* pData = m_vAltitudeLimitDelList.at(j);
		if (pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vAltitudeLimitDelList.clear();
}

void SectorLimitation::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	int nPrjID;
	adoRecordset.GetFieldValue(_T("PROJID"),nPrjID);
	adoRecordset.GetFieldValue(_T("SECTORID"),m_nSectorID);


	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM SECTORLIMITATION\
					 WHERE (PARENTID = %d) "),m_nID);
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			SectorAltitudeLimitation* pData = new SectorAltitudeLimitation;
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vAltitudeLimitList.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void SectorLimitation::SaveData(int nPrjID)
{
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO SECTORFLIGHTMIXSPEC\
						(PROJID, SECTORID)\
						VALUES (%d,%d)"),nPrjID,m_nSectorID);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

	std::vector<SectorAltitudeLimitation*>::iterator iter = m_vAltitudeLimitList.begin();

	for (;iter != m_vAltitudeLimitList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	for(iter = m_vAltitudeLimitDelList.begin(); iter != m_vAltitudeLimitDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vAltitudeLimitDelList.clear();
}

void SectorLimitation::UpdateData(int nPrjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE SECTORFLIGHTMIXSPEC\
					 SET PROJID =%d, SECTORID =%d\
					 WHERE (ID = %d)"),nPrjID,m_nSectorID,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<SectorAltitudeLimitation *>::iterator iter = m_vAltitudeLimitList.begin();
	for (;iter != m_vAltitudeLimitList.end();++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nID);
		else
			(*iter)->UpdateData(m_nID);
	}

	iter = m_vAltitudeLimitDelList.begin();
	for (;iter != m_vAltitudeLimitDelList.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vAltitudeLimitDelList.clear();
}

void SectorLimitation::DeleteData()
{
	std::vector<SectorAltitudeLimitation*>::iterator iter  = m_vAltitudeLimitList.begin();
	for (; iter != m_vAltitudeLimitList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (iter = m_vAltitudeLimitDelList.begin(); iter != m_vAltitudeLimitDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM SECTORFLIGHTMIXSPEC\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}

int SectorLimitation::GetID()
{
	return m_nID;
}

int SectorLimitation::GetSectorID()
{
	return m_nSectorID;
}

int SectorLimitation::GetAltitudeLimitCount()
{
	return (int)m_vAltitudeLimitList.size();
}

SectorAltitudeLimitation* SectorLimitation::GetAltitudeLimitByIdx(int nIdx)
{
	if (nIdx >= (int)m_vAltitudeLimitList.size())
		return NULL;

	return m_vAltitudeLimitList.at(nIdx);
}

void SectorLimitation::AddAltitudeLimit(SectorAltitudeLimitation* pItem)
{
	if (pItem == NULL)
		return;

	m_vAltitudeLimitList.push_back(pItem);
}

void SectorLimitation::DelAltitudeLimit(SectorAltitudeLimitation* pItem)
{
	if (pItem == NULL)
		return;

	std::vector<SectorAltitudeLimitation*>::iterator iter = 
		std::find(m_vAltitudeLimitList.begin(),m_vAltitudeLimitList.end(),pItem);

	if (iter == m_vAltitudeLimitList.end())
		return;

	m_vAltitudeLimitDelList.push_back(pItem);
	m_vAltitudeLimitList.erase(iter);
}
///////////////////////////////////////SectorFlightMixSpecification/////////////////////////////////
SectorFlightMixSpecification::SectorFlightMixSpecification(void)
:m_nPrjID(-1)
{
	m_vSectorLimitDelList.clear();
	m_vSectorLimitList.clear();
}

SectorFlightMixSpecification::~SectorFlightMixSpecification(void)
{
	int nCount =(int) m_vSectorLimitList.size();
	for (int i = 0; i < nCount; i++)
	{
		SectorLimitation* pData = m_vSectorLimitList.at(i);
		if (pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vSectorLimitList.clear();

	nCount = (int)m_vSectorLimitDelList.size();
	for (int j = 0; j < nCount; j++)
	{
		SectorLimitation* pData = m_vSectorLimitDelList.at(j);
		if (pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vSectorLimitDelList.clear();
}

void SectorFlightMixSpecification::ReadData(int nPrjID)
{
	m_nPrjID = nPrjID;
	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM SECTORFLIGHTMIXSPEC\
					 WHERE (PROJID = %d) "),nPrjID);
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			SectorLimitation* pData = new SectorLimitation;
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vSectorLimitList.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void SectorFlightMixSpecification::SaveData()
{
	std::vector<SectorLimitation*>::iterator iter  = m_vSectorLimitList.begin();
	for (; iter != m_vSectorLimitList.end(); ++iter)
	{
		if(-1 == (*iter)->GetID())
			(*iter)->SaveData(m_nPrjID);
		else
			(*iter)->UpdateData(m_nPrjID);
	}

	for (iter = m_vSectorLimitDelList.begin(); iter != m_vSectorLimitDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vSectorLimitDelList.clear();
}

void SectorFlightMixSpecification::UpdateData()
{

}

void SectorFlightMixSpecification::DeleteData()
{
	std::vector<SectorLimitation*>::iterator iter  = m_vSectorLimitList.begin();
	for (; iter != m_vSectorLimitList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (iter = m_vSectorLimitDelList.begin(); iter != m_vSectorLimitDelList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
}

int SectorFlightMixSpecification::GetSectorLimitCount()
{
	return (int)m_vSectorLimitList.size();
}

SectorLimitation* SectorFlightMixSpecification::GetSectorLimitByIdx(int nIdx)
{
	if (nIdx >= (int)m_vSectorLimitList.size())
		return NULL;

	return m_vSectorLimitList.at(nIdx);
}

void SectorFlightMixSpecification::AddSectorLimit(SectorLimitation* pItem)
{
	if (pItem == NULL)
		return;

	m_vSectorLimitList.push_back(pItem);
}

void SectorFlightMixSpecification::DelSectorLimit(SectorLimitation* pItem)
{
	if (pItem == NULL)
		return;

	std::vector<SectorLimitation*>::iterator iter = 
		std::find(m_vSectorLimitList.begin(),m_vSectorLimitList.end(),pItem);

	if (iter == m_vSectorLimitList.end())
		return;

	m_vSectorLimitDelList.push_back(pItem);
	m_vSectorLimitList.erase(iter);
}