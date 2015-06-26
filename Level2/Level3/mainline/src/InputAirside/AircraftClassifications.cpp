#include "StdAfx.h"
#include "AircraftClassifications.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include <common/CARCUnit.h>

#include <algorithm>
//----------------------------------------------------

bool AircraftClassificationItem::ReadData(int nID)
{
	if(nID == -1)
		return (false);

	CString strSQL;

	strSQL.Format(_T("SELECT * FROM AIRCRAFTCLASSIFICATION WHERE (ID = %d) "),nID);

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

	if(adoRecordset.IsEOF())
		return(false);

	m_nID = nID;
	while (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("NAME"),m_strName);

		adoRecordset.GetFieldValue(_T("MINVALUE"),m_nMinValue);

		adoRecordset.GetFieldValue(_T("MAXVALUE"),m_nMaxValue);

		adoRecordset.MoveNextData();
	}
	return (true);
}
void AircraftClassificationItem::SaveData()
{
	if(m_nID != -1)
		return;

	CString strSQL;

	strSQL.Format(_T("INSERT INTO AIRCRAFTCLASSIFICATION\
					 (PROJID, NAME, TYPE, MINVALUE, MAXVALUE)\
					 VALUES (%d,'%s',%d,%d,%d)"), 
					 m_nProjID, m_strName, (int)m_emType, m_nMinValue, m_nMaxValue);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

}

void AircraftClassificationItem::UpdateData()
{
	if(m_nID == -1)
		return;

	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AIRCRAFTCLASSIFICATION\
					 SET PROJID =%d, NAME ='%s', TYPE= %d, MINVALUE =%d, MAXVALUE =%d\
					 WHERE (ID = %d)"),
					 m_nProjID, m_strName, (int)m_emType, m_nMinValue, m_nMaxValue, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);


}

void AircraftClassificationItem::DeleteData()
{
	if(m_nID == -1)
		return;

	CString strSQL;
	strSQL.Format("DELETE FROM AIRCRAFTCLASSIFICATION WHERE ID = %d", m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
/*
int m_nID;
CString m_strName;
int m_nMinValue;
int m_nMaxValue;
int m_nProjID;
FlightClassificationBasisType m_emType;
*/
void AircraftClassificationItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt((int)m_emType);
	
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeField(m_strName);
	exportFile.getFile().writeInt(m_nMinValue);
	exportFile.getFile().writeInt(m_nMaxValue);
	exportFile.getFile().writeLine();
}

void AircraftClassificationItem::ImportData(CAirsideImportFile& importFile)
{
	int nType  = 0;
	importFile.getFile().getInteger(nType);
	if(nType < CategoryType_Count&& nType >= 0)
		m_emType = (FlightClassificationBasisType)nType;
	int nID = -1;
	importFile.getFile().getInteger(nID);

	int nProjID = -1;
	importFile.getFile().getInteger(nProjID);

	char chName[512];
	importFile.getFile().getField(chName,512);
	m_strName = chName;

	importFile.getFile().getInteger(m_nMinValue);
	importFile.getFile().getInteger(m_nMinValue);

	importFile.getFile().getLine();

	m_nProjID = importFile.getNewProjectID();
	SaveData();
	importFile.AddAircraftClassificationsIndexMap(nID,m_nID);
}

bool AircraftClassificationItem::operator ==(const AircraftClassificationItem& item)const
{
	return ( m_nMinValue == item.m_nMinValue ) && (item.m_nMaxValue == m_nMaxValue );
}

bool AircraftClassificationItem::operator <(const AircraftClassificationItem& item)const
{
	return m_nMinValue < item.m_nMinValue;
}

bool AircraftClassificationItem::operator >(const AircraftClassificationItem& item)const
{
	return m_nMinValue > item.m_nMinValue;
}

bool AircraftClassificationItem::IsFitIn( double dval ) const
{
	return ( m_nMinValue <= dval ) && (dval<= m_nMaxValue );
}

void AircraftClassificationItem::setID( int nID )
{
	m_nID = nID;
}

void AircraftClassificationItem::setName( CString strName )
{
	m_strName = strName;
}

void AircraftClassificationItem::setMaxValue( int nMax )
{
	m_nMaxValue = nMax;
}

void AircraftClassificationItem::setMinValue( int nMin )
{
	m_nMinValue = nMin;
}

void AircraftClassificationItem::SetProjectID( int nProjID )
{
	m_nProjID = nProjID;
}

void AircraftClassificationItem::SetType( FlightClassificationBasisType emType )
{
	m_emType = emType;
}

AircraftClassificationItem::AircraftClassificationItem( FlightClassificationBasisType emType /*= NoneBased*/ ) :m_nID(-1)
,m_nMaxValue(0)
,m_nMinValue(0)
,m_nProjID(-1)
,m_strName(_T(""))
,m_emType(emType)
{

}

AircraftClassificationItem::~AircraftClassificationItem()
{

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//AircraftClassifications

AircraftClassifications::AircraftClassifications(int nProjID, FlightClassificationBasisType emType)
:m_nProjID(nProjID)
,m_emType(emType)
{
}

AircraftClassifications::AircraftClassifications()
{
	m_emType = NoneBased;
}
AircraftClassifications::~AircraftClassifications(void)
{

	for(AircraftCategoryIter iter = m_vectClassification.begin();
		iter != m_vectClassification.end(); iter++)
	{
		delete (*iter);
	}
	for(AircraftCategoryIter iter = m_vectNeedToDel.begin();
		iter != m_vectNeedToDel.end(); iter++)
	{
		delete (*iter);
	}
}
/*
A.Wake vortex weight based

Light	 			0-15000(lbs)(6803886 g)
Medium 				15000	300000(13607771 g)
Heavy				300000-600000(27215542 g)

B.Approach speed based
ICAO				0-90
ICAO				90	120
ICAO				120-	140
ICAO				140	-160
ICAO				160  200

*/
namespace{
	const static long LIGHT_MAX = (int)CARCWeightUnit::ConvertWeight(AU_WEIGHT_POUND,DEFAULT_DATABASE_WEIGHT_UNIT,15000);
	const static long MEDIUM_MAX = (int)CARCWeightUnit::ConvertWeight(AU_WEIGHT_POUND,DEFAULT_DATABASE_WEIGHT_UNIT,300000);
	const static long HEAVY_MAX = (int)CARCWeightUnit::ConvertWeight(AU_WEIGHT_POUND,DEFAULT_DATABASE_WEIGHT_UNIT,3000000);
}

void AircraftClassifications::InsertDefaultValueIfNeed()
{
	AircraftCategoryList vectClassifications;

	//WakeVortexWightBased
	BOOL bNeedInsert = TRUE;
	{
		CString strSQL;
		strSQL.Format("SELECT * FROM AircraftClassification WHERE PROJID = %d AND Type = %d ORDER BY ID",
			m_nProjID, WakeVortexWightBased);

		CADORecordset adoRecordset;
		long nRecordAfflect = 0L;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

		if(!adoRecordset.IsEOF())
			bNeedInsert = FALSE;

	}
	if(TRUE == bNeedInsert)
	{
		AircraftClassificationItem* pNewItem = NULL;

		pNewItem = new AircraftClassificationItem;
		pNewItem->SetType(WakeVortexWightBased);
		pNewItem->setName(_T("Light"));
		pNewItem->setMinValue(0);
		pNewItem->setMaxValue(LIGHT_MAX);
		pNewItem->SetProjectID(m_nProjID);
		vectClassifications.push_back(pNewItem);


		pNewItem = new AircraftClassificationItem;
		pNewItem->SetType(WakeVortexWightBased);
		pNewItem->setName(_T("Medium"));
		pNewItem->setMinValue(LIGHT_MAX);
		pNewItem->setMaxValue(MEDIUM_MAX);
		pNewItem->SetProjectID(m_nProjID);
		vectClassifications.push_back(pNewItem);


		pNewItem = new AircraftClassificationItem;
		pNewItem->SetType(WakeVortexWightBased);
		pNewItem->setName(_T("Heavy"));
		pNewItem->setMinValue(MEDIUM_MAX);
		pNewItem->setMaxValue(HEAVY_MAX);
		pNewItem->SetProjectID(m_nProjID);
		vectClassifications.push_back(pNewItem);
	}


	//ApproachSpeedBased
	bNeedInsert = TRUE;
	{
		CString strSQL;
		strSQL.Format("SELECT * FROM AircraftClassification WHERE PROJID = %d AND Type = %d ORDER BY ID",
			m_nProjID, ApproachSpeedBased);

		CADORecordset adoRecordset;
		long nRecordAfflect = 0L;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

		if(!adoRecordset.IsEOF())
			bNeedInsert = FALSE;
	}
	if(TRUE == bNeedInsert)
	{
		AircraftClassificationItem* pNewItem = NULL;

		pNewItem = new AircraftClassificationItem;
		pNewItem->SetType(ApproachSpeedBased);
		pNewItem->setName(_T("ICAO A"));
		pNewItem->setMinValue(0);
		pNewItem->setMaxValue(46);
		pNewItem->SetProjectID(m_nProjID);
		vectClassifications.push_back(pNewItem);

		pNewItem = new AircraftClassificationItem;
		pNewItem->SetType(ApproachSpeedBased);
		pNewItem->setName(_T("ICAO B"));
		pNewItem->setMinValue(47);
		pNewItem->setMaxValue(61);
		pNewItem->SetProjectID(m_nProjID);
		vectClassifications.push_back(pNewItem);

		pNewItem = new AircraftClassificationItem;
		pNewItem->SetType(ApproachSpeedBased);
		pNewItem->setName(_T("ICAO C"));
		pNewItem->setMinValue(62);
		pNewItem->setMaxValue(71);
		pNewItem->SetProjectID(m_nProjID);
		vectClassifications.push_back(pNewItem);

		pNewItem = new AircraftClassificationItem;
		pNewItem->SetType(ApproachSpeedBased);
		pNewItem->setName(_T("ICAO D"));
		pNewItem->setMinValue(72);
		pNewItem->setMaxValue(82);
		pNewItem->SetProjectID(m_nProjID);
		vectClassifications.push_back(pNewItem);

		pNewItem = new AircraftClassificationItem;
		pNewItem->SetType(ApproachSpeedBased);
		pNewItem->setName(_T("ICAO E"));
		pNewItem->setMinValue(83);
		pNewItem->setMaxValue(103);
		pNewItem->SetProjectID(m_nProjID);
		vectClassifications.push_back(pNewItem);
	}


	for(AircraftCategoryIter iter = vectClassifications.begin();
		iter != vectClassifications.end(); iter++)
	{
		(*iter)->SaveData();
	}


	for(AircraftCategoryIter iter = vectClassifications.begin();
		iter != vectClassifications.end(); iter++)
		delete (*iter);

	vectClassifications.clear();


}

bool compareVotexItem(const AircraftClassificationItem* item1, const AircraftClassificationItem* item2)
{
	return *item1 < *item2;
}

void AircraftClassifications::ReadData()
{
	InsertDefaultValueIfNeed();

	m_vectClassification.clear();

	CString strSQL;
	strSQL.Format("SELECT * FROM AircraftClassification WHERE PROJID = %d AND Type = %d ORDER BY ID",
		m_nProjID, m_emType);

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		AircraftClassificationItem* pNewItem = new AircraftClassificationItem;
		
		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		pNewItem->setID(nID);

		CString strName = _T("");
		adoRecordset.GetFieldValue(_T("Name"),strName);
		pNewItem->setName(strName);

		int nMin = -1;
		adoRecordset.GetFieldValue(_T("MINVALUE"),nMin);
		pNewItem->setMinValue(nMin);

		int nMax = -1;
		adoRecordset.GetFieldValue(_T("MAXVALUE"),nMax);
		pNewItem->setMaxValue(nMax);

		int nProjID = -1;
		adoRecordset.GetFieldValue(_T("ProjID"), nProjID);
		pNewItem->SetProjectID(nProjID);

		int nType;
		adoRecordset.GetFieldValue(_T("TYPE"), nType);
		pNewItem->SetType((FlightClassificationBasisType)nType);

		m_vectClassification.push_back(pNewItem);
	

		adoRecordset.MoveNextData();
	}

	if (m_emType == WakeVortexWightBased)
	{
		std::sort(m_vectClassification.begin(),m_vectClassification.end(),compareVotexItem);
	}
}

void AircraftClassifications::SaveData()
{

	for(AircraftCategoryIter iter = m_vectClassification.begin();
		iter != m_vectClassification.end(); iter++)
	{
		if(-1 == (*iter)->getID())
			(*iter)->SaveData();
		else
			(*iter)->UpdateData();
	}

	for(AircraftCategoryIter iter = m_vectNeedToDel.begin(); iter != m_vectNeedToDel.end(); iter++)
	{
		(*iter)->DeleteData();
		delete (*iter);
	}
	m_vectNeedToDel.clear();
}


void AircraftClassifications::AddItem(AircraftClassificationItem* pNewItem)
{
	m_vectClassification.push_back(pNewItem);
}

void AircraftClassifications::DeleteItem(AircraftClassificationItem* pNewItem)
{
	AircraftCategoryIter iter = std::find(m_vectClassification.begin(), m_vectClassification.end(), pNewItem);
	if(iter == m_vectClassification.end())
		return;

	m_vectNeedToDel.push_back(pNewItem);
	m_vectClassification.erase(iter);
}

AircraftClassificationItem*  AircraftClassifications::GetItem(int nIndex)
{
	ASSERT(nIndex >=0 && nIndex < (int)m_vectClassification.size());
	return m_vectClassification[nIndex];
}

AircraftClassificationItem* AircraftClassifications::GetItemByID(int nID)
{
	AircraftClassificationItem* pItem = 0;
	for (size_t i=0; i<m_vectClassification.size(); i++)
	{
		pItem = m_vectClassification.at(i);
		if (pItem->m_nID == nID)
			break;
		else
			pItem = 0;
	}
	return pItem;
}

void AircraftClassifications::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("Category type,id, project id,name,min value,max value"));
	exportFile.getFile().writeLine();
	for (int i =0; i < (int)CategoryType_Count; ++i)
	{
		AircraftClassifications ClassificationData(exportFile.GetProjectID(),(FlightClassificationBasisType)i);
		ClassificationData.ReadData();
		ClassificationData.ExportAircraftClassificatins(exportFile);   
	}


	exportFile.getFile().endFile();

}
void AircraftClassifications::ImportData(CAirsideImportFile& importFile)
{
	//importFile.getFile().getLine();//header

	while (!importFile.getFile().isBlank())
	{
		AircraftClassificationItem data;
		data.ImportData(importFile);
	}

}
void AircraftClassifications::ExportAircraftClassificatins(CAirsideExportFile& exportFile)
{
	for (AircraftCategoryList::size_type i = 0 ; i < m_vectClassification.size(); ++ i)
	{
		m_vectClassification.at(i)->ExportData(exportFile);
	}
}

AircraftClassificationItem * AircraftClassifications::GetFitInItem( double dval )
{
	if (m_emType == WakeVortexWightBased)
		return GetWakeVortexFitInItem(dval);
	
	return GetFitInItemExceptWakeVortex(dval);
}

AircraftClassificationItem* AircraftClassifications::GetWakeVortexFitInItem(double dval)
{
	int nCount = GetCount();
	for (int i = nCount - 1; i >= 0; i--)
	{
		AircraftClassificationItem * pItem = GetItem(i);

		if (pItem->m_nMinValue <= dval)
			return pItem;
	}
	return NULL;
}

AircraftClassificationItem* AircraftClassifications::GetFitInItemExceptWakeVortex(double dval)
{
	ASSERT(m_emType != WakeVortexWightBased);
	for(int i=0;i<GetCount();i++)
	{
		AircraftClassificationItem * pItem = GetItem(i);

		if(pItem->IsFitIn(dval)) return pItem;
	}
	return NULL;	
}
void AircraftClassifications::SetProjectID( int nProjID )
{
	m_nProjID = nProjID;
}

void AircraftClassifications::SetBasisType( FlightClassificationBasisType cType )
{
	m_emType = cType;
}

FlightClassificationBasisType AircraftClassifications::GetBasisType()
{
	return m_emType;
}

int AircraftClassifications::GetCount()
{
	return (int)m_vectClassification.size();
}

int AircraftClassifications::GetMaxValue(int idx)
{
	int nCount = (int)GetCount();
	int nBackIdx = nCount - 1;
	ASSERT(idx >= 0 && idx < nCount);
	//is last item
	if (idx == nBackIdx)
		return -1;
	
	int nextdx = idx + 1;
	return GetItem(nextdx)->m_nMinValue;
}

void AircraftClassifications::sort()
{
	std::sort(m_vectClassification.begin(),m_vectClassification.end(),compareVotexItem);
}




