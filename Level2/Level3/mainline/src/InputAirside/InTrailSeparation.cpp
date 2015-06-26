#include "stdAfx.h"
#include "InTrailSeparation.h"
#include "../Database/ADODatabase.h"

//////////////////////////////////////////////////////////////////////
// class CInTrailSeparationData
CInTrailSeparationData::CInTrailSeparationData()
: m_nID(-1)
 ,m_nInTrailSepID(-1)
 ,m_nMinDistance(5)
 ,m_nMinTime(2)
{
}
CInTrailSeparationData::~CInTrailSeparationData()
{
}

// classification item functions
void CInTrailSeparationData::setClsTrailItem(AircraftClassificationItem clsTrailItem)
{
	m_clsTrailItem = clsTrailItem;
}
AircraftClassificationItem* CInTrailSeparationData::getClsTrailItem()
{
	return &m_clsTrailItem;
}
void CInTrailSeparationData::setClsLeadItem(AircraftClassificationItem clsLeadItem)
{
	m_clsLeadItem = clsLeadItem;
}
AircraftClassificationItem* CInTrailSeparationData::getClsLeadItem()
{
	return &m_clsLeadItem;
}

// minimum distance functions
void CInTrailSeparationData::setMinDistance(long nMinDistance)
{
	m_nMinDistance = nMinDistance;
}
int CInTrailSeparationData::getMinDistance()
{	
    return m_nMinDistance;
}

// minimum time functions
void CInTrailSeparationData::setMinTime(long nMinTime)
{
	m_nMinTime = nMinTime;
}
int CInTrailSeparationData::getMinTime()
{
    return m_nMinTime;
}

void CInTrailSeparationData::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("INTRAILID"),m_nInTrailSepID);
	int nTrailType = 0;
	adoRecordset.GetFieldValue(_T("TRAILTYPE"),nTrailType);
	m_clsTrailItem.setID(nTrailType);

	int nLeadType =0;
	adoRecordset.GetFieldValue(_T("LEADTYPE"),nLeadType);
	m_clsLeadItem.setID(nLeadType);

	adoRecordset.GetFieldValue(_T("MINDIST"),m_nMinDistance);
	adoRecordset.GetFieldValue(_T("MINTIME"),m_nMinTime);


}
void CInTrailSeparationData::SaveData(int nInTrailSepID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}

	m_nInTrailSepID = nInTrailSepID;

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_INTRAILSEP_DATA\
		(INTRAILID, TRAILTYPE, LEADTYPE, MINDIST, MINTIME)\
		VALUES (%d,%d,%d,%d,%d)"),
		nInTrailSepID,m_clsTrailItem.getID(),m_clsLeadItem.getID(),m_nMinDistance,m_nMinTime);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

}
void CInTrailSeparationData::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_INTRAILSEP_DATA\
		SET TRAILTYPE =%d, LEADTYPE =%d, MINDIST =%d, MINTIME =%d\
		WHERE (ID = %d)"),
		m_clsTrailItem.getID(),m_clsLeadItem.getID(),m_nMinDistance,m_nMinTime,m_nID);

	 CADODatabase::ExecuteSQLStatement(strSQL);
}
void CInTrailSeparationData::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_INTRAILSEP_DATA\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
//////////////////////////////////////////////////////////////////////
// class CInTrailSeparation
CInTrailSeparation::CInTrailSeparation()
: m_nID(-1)
 ,m_nProjID(-1)
 ,m_vInTrailSepItem(NULL)
 ,m_vInTrailSepItemNeedDel(NULL)
{
}
CInTrailSeparation::CInTrailSeparation(PhaseType emPahseType, FlightClassificationBasisType emCategoryType)
: m_nID(-1)
 ,m_nProjID(-1)
 ,m_vInTrailSepItem(NULL)
 ,m_vInTrailSepItemNeedDel(NULL)
 ,m_bIsActive(FALSE)
{
	m_emPahseType = emPahseType;
	m_emCategoryType = emCategoryType;
}

CInTrailSeparation::~CInTrailSeparation()
{
	std::vector<CInTrailSeparationData *>::iterator iter = m_vInTrailSepItem.begin();
	for(; iter != m_vInTrailSepItem.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vInTrailSepItemNeedDel.begin();
	for (; iter != m_vInTrailSepItemNeedDel.end(); ++iter)
	{
		delete *iter;
	}
}

BOOL CInTrailSeparation::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, INTRAILID, TRAILTYPE, LEADTYPE, MINDIST, MINTIME\
		FROM IN_INTRAILSEP_DATA\
		WHERE (INTRAILID = %d)"),m_nID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CInTrailSeparationData * pData = new CInTrailSeparationData();

		pData->ReadData(adoRecordset);
		m_vInTrailSepItem.push_back(pData);

		adoRecordset.MoveNextData();
	}

	return TRUE;
}
BOOL CInTrailSeparation::SaveData(int nProjID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}

	m_nProjID = nProjID;

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_INTRAILSEPARATION\
		(PROJID, PHASETYPE, CATETYPE, ISACTIVE)\
		VALUES (%d,%d,%d,%d)"),nProjID,(int)m_emPahseType,(int)m_emCategoryType,(int)m_bIsActive);

 	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	
	std::vector<CInTrailSeparationData *>::iterator iter = m_vInTrailSepItem.begin();
	for (;iter != m_vInTrailSepItem.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	iter = m_vInTrailSepItemNeedDel.begin();
	for (;iter != m_vInTrailSepItemNeedDel.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vInTrailSepItemNeedDel.clear();
    return TRUE;
}
BOOL CInTrailSeparation::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_INTRAILSEPARATION\
		SET PHASETYPE =%d, CATETYPE =%d, ISACTIVE =%d\
		WHERE (ID = %d)"),(int)m_emPahseType,(int)m_emCategoryType,(int)m_bIsActive,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);


	std::vector<CInTrailSeparationData *>::iterator iter = m_vInTrailSepItem.begin();
	for (;iter != m_vInTrailSepItem.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	iter = m_vInTrailSepItemNeedDel.begin();
	for (;iter != m_vInTrailSepItemNeedDel.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vInTrailSepItemNeedDel.clear();
	return TRUE;
}
BOOL CInTrailSeparation::DeleteData()
{

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_INTRAILSEPARATION\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<CInTrailSeparationData *>::iterator iter = m_vInTrailSepItem.begin();
	for (;iter != m_vInTrailSepItem.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
	iter = m_vInTrailSepItemNeedDel.begin();
	for (;iter != m_vInTrailSepItemNeedDel.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vInTrailSepItemNeedDel.clear();

	return TRUE;
}

// phase type functions
void CInTrailSeparation::setPhaseType(PhaseType emPahseType)
{
	m_emPahseType = emPahseType;
}
PhaseType CInTrailSeparation::getPhaseType()
{
	return m_emPahseType;
}

// category type functions
void CInTrailSeparation::setCategoryType(FlightClassificationBasisType emCategoryType)
{
	m_emCategoryType = emCategoryType;
}
FlightClassificationBasisType CInTrailSeparation::getCategoryType()
{
	return m_emCategoryType;
}

int CInTrailSeparation::GetItemCount()const
{
	return static_cast<int> (m_vInTrailSepItem.size());
}

CInTrailSeparationData* CInTrailSeparation::GetItem(int nIndex)const
{
	ASSERT(nIndex < (int)m_vInTrailSepItem.size());
	return m_vInTrailSepItem.at( nIndex );
}

void CInTrailSeparation::AddItem(CInTrailSeparationData* pInTrailSepItem)
{
	m_vInTrailSepItem.push_back( pInTrailSepItem );
}

void CInTrailSeparation::Deltem(int nIndex)
{
	ASSERT(nIndex < (int)m_vInTrailSepItem.size());
	m_vInTrailSepItemNeedDel.push_back( m_vInTrailSepItem[nIndex] );

	m_vInTrailSepItem.erase(m_vInTrailSepItem.begin() + nIndex);
}
void CInTrailSeparation::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
	int nPhaseType = 0;//PHASETYPE_KNOWN
	adoRecordset.GetFieldValue(_T("PHASETYPE"),nPhaseType);
	//if (nPhaseType > PHASETYPE_UNKNOWN && nPhaseType < PHASETYPE_COUNT)
	//{
	//	m_emPahseType = (PhaseType)nPhaseType;
	//}
	//else
	//	m_emPahseType = PHASETYPE_UNKNOWN;

	int nCateType = 0;
	adoRecordset.GetFieldValue(_T("CATETYPE"),nCateType);

	if (nCateType >= WingspanBased && nCateType < CategoryType_Count)
	{
		m_emCategoryType = (FlightClassificationBasisType)nCateType;
	}
	else
		m_emCategoryType = WingspanBased;

	int nIsActive = 0;
	adoRecordset.GetFieldValue(_T("ISACTIVE"),nIsActive);
	m_bIsActive = (BOOL)nIsActive;

	ReadData();
}

CInTrailSeparationData * CInTrailSeparation::GetItemByACClass( const AircraftClassificationItem* trailItem, const AircraftClassificationItem* leadItem ) const
{
	ASSERT(trailItem && leadItem);

	if(! (trailItem&& leadItem) )
	{
		return NULL;
	}
	
	for(int i=0 ;i< GetItemCount();i++)
	{
		CInTrailSeparationData * sepData = GetItem(i);
		if(sepData)
		{
			if( sepData->getClsLeadItem()->getID() == leadItem->getID() && sepData->getClsTrailItem()->getID() == trailItem->getID() )
				return sepData;
		}
	}


	return NULL;
}

void CInTrailSeparation::SetIsActive(BOOL bIsActive)
{
	m_bIsActive = bIsActive;
}

BOOL CInTrailSeparation::IsActive()
{
	return m_bIsActive;
}

//////////////////////////////////////////////////////////////////////
// class CInTrailSeparationList
CInTrailSeparationList::CInTrailSeparationList()
:m_nProjID(-1)
,m_vInTrailSeparation(NULL)
,m_vInTrailSepNeedDel(NULL)
{
}
CInTrailSeparationList::~CInTrailSeparationList()
{
	std::vector<CInTrailSeparation*>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vInTrailSepNeedDel.begin();
	for (;iter != m_vInTrailSepNeedDel.end(); ++iter)
	{
		delete *iter;
	}
}

BOOL CInTrailSeparationList::ReadData(int nProjID)
{
	CString strSQL = _T("");

	strSQL.Format(_T("SELECT ID,PROJID, PHASETYPE, CATETYPE, ISACTIVE\
		FROM IN_INTRAILSEPARATION\
		WHERE (PROJID = %d)"),nProjID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CInTrailSeparation *pData = new CInTrailSeparation();
		pData->ReadData(adoRecordset);
		
		m_vInTrailSeparation.push_back(pData);

		adoRecordset.MoveNextData();
	}

	return TRUE;
}
BOOL CInTrailSeparationList::SaveData(int nProjID)
{
	std::vector<CInTrailSeparation *>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		(*iter)->SaveData(nProjID);
	}

	iter = m_vInTrailSepNeedDel.begin();
	for (; iter != m_vInTrailSepNeedDel.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vInTrailSepNeedDel.clear();
	return TRUE;
}

int CInTrailSeparationList::GetItemCount()
{
	return static_cast<int> (m_vInTrailSeparation.size());
}

CInTrailSeparation* CInTrailSeparationList::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vInTrailSeparation.size());
	return m_vInTrailSeparation.at( nIndex );
}

CInTrailSeparation* CInTrailSeparationList::GetItemByType(PhaseType emPahseType, FlightClassificationBasisType emCategoryType)
{
	std::vector<CInTrailSeparation*>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		if (emPahseType == (*iter)->getPhaseType()
			&& emCategoryType == (*iter)->getCategoryType())
		{
			return (*iter);
		}		
	}
	return NULL;
}

CInTrailSeparation* CInTrailSeparationList::GetItemByType(PhaseType emPahseType)
{
	std::vector<CInTrailSeparation*>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		if (emPahseType == (*iter)->getPhaseType()
			&& (*iter)->IsActive())
		{
			return (*iter);
		}		
	}
	return NULL;
}

void CInTrailSeparationList::SetItemActive(PhaseType emPahseType, FlightClassificationBasisType emCategoryType, BOOL bActive)
{
	std::vector<CInTrailSeparation*>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		if (emPahseType == (*iter)->getPhaseType())
		{
			if (emCategoryType == (*iter)->getCategoryType())
			{
				(*iter)->SetIsActive(TRUE);
			}
			else
			{
				(*iter)->SetIsActive(FALSE);
			}
		}		
	}
}

void CInTrailSeparationList::AddItem(CInTrailSeparation* pInTrailSep)
{
	m_vInTrailSeparation.push_back(pInTrailSep);    
}

void CInTrailSeparationList::Deltem(int nIndex)
{
	ASSERT(nIndex < (int)m_vInTrailSeparation.size());
	m_vInTrailSepNeedDel.push_back( m_vInTrailSeparation[nIndex] );

	m_vInTrailSeparation.erase(m_vInTrailSeparation.begin() + nIndex);
}

void CInTrailSeparationList::DelItemByType(PhaseType emPahseType)
{
	std::vector<CInTrailSeparation*>::iterator iter = m_vInTrailSeparation.begin();
	for(; iter != m_vInTrailSeparation.end(); ++iter)
	{
		if (emPahseType == (*iter)->getPhaseType())
		{
			m_vInTrailSepNeedDel.push_back( *iter );
			m_vInTrailSeparation.erase(iter);
			break;
		}		
	}
}