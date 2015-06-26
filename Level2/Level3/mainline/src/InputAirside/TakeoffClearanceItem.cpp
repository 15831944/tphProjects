#include "StdAfx.h"
#include "TakeoffClearanceItem.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"

//ID, TOCLEARID, TRAIL, LEAD, MINSEPARATION
void TakeoffClearanceClassfiBasisItem::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("TOCLEARID"),m_nTakeoffClearanceItemID);
	int nClsID = 0;
	adoRecordset.GetFieldValue(_T("TRAIL"),nClsID);

	m_nTrailID = nClsID;

	nClsID = 0;
	adoRecordset.GetFieldValue(_T("LEAD"),nClsID);
	m_nLeadID = nClsID;

	adoRecordset.GetFieldValue(_T("MINSEPARATION"),m_nMinSeparation);

	int nType = 0;
	adoRecordset.GetFieldValue(_T("CLSTYPE"), nType);

	m_emClassifiBasis = (FlightClassificationBasisType)nType;


}
void TakeoffClearanceClassfiBasisItem::SaveData(int nTakeOffClearanceID)
{
	if (m_nID < 0 || m_bImport)
	{
		m_nTakeoffClearanceItemID = nTakeOffClearanceID;

		CString strSQL ;
		strSQL.Format(_T("INSERT INTO IN_TAKEOFFCLEARANCE_DATA\
			(TOCLEARID, TRAIL, LEAD, MINSEPARATION, CLSTYPE)\
			VALUES (%d,%d,%d,%d,%d)"),
			nTakeOffClearanceID,m_nTrailID, m_nLeadID,m_nMinSeparation, (int)m_emClassifiBasis);

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		if(m_bImport)m_bImport = false;
	}
	else
		return UpdateData();
}

void TakeoffClearanceClassfiBasisItem::ImportData(CAirsideImportFile& importFile)
{
	m_bImport = true;
	importFile.getFile().getInteger(m_nID);
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	importFile.getFile().getInteger(m_nTakeoffClearanceItemID);
	importFile.getFile().getInteger(m_nLeadID);
	m_nLeadID = importFile.GetAircraftClassificationsNewID(m_nLeadID);
	importFile.getFile().getInteger(m_nTrailID);
	m_nTrailID = importFile.GetAircraftClassificationsNewID(m_nTrailID);
	int nClassifiBasis = -1;
	importFile.getFile().getInteger(nClassifiBasis);
	m_emClassifiBasis = (FlightClassificationBasisType)nClassifiBasis;
	importFile.getFile().getInteger(m_nMinSeparation);
	importFile.getFile().getLine();
}

void TakeoffClearanceClassfiBasisItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_nTakeoffClearanceItemID);
	exportFile.getFile().writeInt(m_nLeadID);
	exportFile.getFile().writeInt(m_nTrailID);
	exportFile.getFile().writeInt((int)m_emClassifiBasis);
	exportFile.getFile().writeInt(m_nMinSeparation);
	exportFile.getFile().writeLine();
}

void TakeoffClearanceClassfiBasisItem::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TAKEOFFCLEARANCE_DATA\
		SET TRAIL = %d, LEAD =%d, MINSEPARATION =%d, CLSTYPE=%d\
		WHERE (ID = %d)"), m_nTrailID, m_nLeadID,m_nMinSeparation, (int)m_emClassifiBasis, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}

void TakeoffClearanceClassfiBasisItem::DeleteData()
{
	if (m_nID == -1)
		return;

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_TAKEOFFCLEARANCE_DATA\
		WHERE (ID = %d)"),m_nID);
	
	CADODatabase::ExecuteSQLStatement(strSQL);
}


//----------------------------------------------------------------------------------
//
TakeoffClearanceItem::TakeoffClearanceItem(int nProjID, TakeoffClearanceType emType)
:m_nID(-1)
,m_nProjID(nProjID)
,m_emType(emType)
,m_nDistTakeoffApproach(0)
,m_nTimeTakeoffAproach(0)
,m_nTimeTakeoffSlot(0)
,m_emClassficType(WingspanBased)
,m_bDistTakeoffApproach(TRUE)
{
	m_bImport = false;
}

TakeoffClearanceItem::~TakeoffClearanceItem(void)
{
}

void TakeoffClearanceItem::GetItemsByType(TakeoffClearanceBasisVector& vect, FlightClassificationBasisType emType)
{
	for (TakeoffClearanceBasisIter iter = m_vectBasis.begin();
		iter != m_vectBasis.end(); iter++) {
			if((*iter)->m_emClassifiBasis == emType){
				vect.push_back(*iter);
			}
	}

}
//ID, PROJID, TYPE, DISTAPPROACH, TIMEAPPROACH, TIMESLOT, LEADDOWNRW, 
//AFTERTIME, FLTCLASSFICTYPE
void TakeoffClearanceItem::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
	adoRecordset.GetFieldValue(_T("DISTAPPROACH"),m_nDistTakeoffApproach);
	adoRecordset.GetFieldValue(_T("TIMEAPPROACH"),m_nTimeTakeoffAproach);
	adoRecordset.GetFieldValue(_T("TIMESLOT"),m_nTimeTakeoffSlot);
	adoRecordset.GetFieldValue(_T("BDISTTAKEOFFAPPROAH"), m_bDistTakeoffApproach);
	
}
void TakeoffClearanceItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_TAKEOFFCLEARANCE\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void TakeoffClearanceItem::ReadClassficBasisData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, TOCLEARID, TRAIL, LEAD, MINSEPARATION, CLSTYPE\
		FROM IN_TAKEOFFCLEARANCE_DATA\
		WHERE (TOCLEARID = %d)"),m_nID);

	long nItemCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);
	while (!adoRecordset.IsEOF())
	{
		TakeoffClearanceClassfiBasisItem *pItem = new TakeoffClearanceClassfiBasisItem;
		pItem->ReadData(adoRecordset);
		m_vectBasis.push_back(pItem);
		adoRecordset.MoveNextData();
	}
	if(m_vectBasis.size()<1)
		InsertDefaultValue();

}

void TakeoffClearanceItem::ImportData(CAirsideImportFile& importFile)
{
	m_bImport = true;
	importFile.getFile().getInteger(m_nID);
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	int nValue = -1;
	importFile.getFile().getInteger(nValue);
	m_emType = (TakeoffClearanceType)nValue;
	nValue = -1;
	importFile.getFile().getInteger(nValue);
	m_bDistTakeoffApproach = (BOOL)nValue;
	importFile.getFile().getInteger(m_nDistTakeoffApproach);
	importFile.getFile().getInteger(m_nTimeTakeoffAproach);
	importFile.getFile().getInteger(m_nTimeTakeoffSlot);
	nValue = -1;
	importFile.getFile().getInteger(nValue);
	m_emClassficType = (FlightClassificationBasisType)nValue;

	importFile.getFile().getInteger(nValue);
	for (int i = 0;i < nValue; i++) {
		TakeoffClearanceClassfiBasisItem* pNoneBasisItem = new TakeoffClearanceClassfiBasisItem;
		if(!pNoneBasisItem)return;
		pNoneBasisItem->ImportData(importFile);
		m_vectBasis.push_back(pNoneBasisItem);
		pNoneBasisItem = 0;
	}
	importFile.getFile().getLine();
}

void TakeoffClearanceItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt((int)m_emType);
	exportFile.getFile().writeInt((int)m_bDistTakeoffApproach);
	exportFile.getFile().writeInt(m_nDistTakeoffApproach);
	exportFile.getFile().writeInt(m_nTimeTakeoffAproach);
	exportFile.getFile().writeInt(m_nTimeTakeoffSlot);
	exportFile.getFile().writeInt((int)m_emClassficType);

	int nCount = (int) m_vectBasis.size();
	exportFile.getFile().writeInt(nCount);
	TakeoffClearanceBasisIter iter = m_vectBasis.begin();
	for (;iter != m_vectBasis.end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
	exportFile.getFile().writeLine();
}

void TakeoffClearanceItem::SaveClassFicBasisData()
{

//	if (m_emType == TakeoffBehindLandingSameRW)//this item doesn't have 
//		return;


	TakeoffClearanceBasisIter iter = m_vectBasis.begin();
	for (;iter != m_vectBasis.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

}

void TakeoffClearanceItem::InsertDefaultBasisItem()
{
	for (int k=0; k<CategoryType_Count; k++)
	{
		AircraftClassifications* pClassification = new AircraftClassifications(m_nProjID, (FlightClassificationBasisType)k);
		pClassification->ReadData();

		for (int i=0; i<pClassification->GetCount(); i++)
		{
			AircraftClassificationItem* pLeadItem = pClassification->GetItem(i);
			for(int j=0; j<pClassification->GetCount(); j++)
			{
				AircraftClassificationItem* pTrailItem = pClassification->GetItem(j);

				TakeoffClearanceClassfiBasisItem* pBasisItem = new TakeoffClearanceClassfiBasisItem;
				pBasisItem->m_nProjID = m_nProjID;
				pBasisItem->m_nTakeoffClearanceItemID = m_nID;
				pBasisItem->m_nLeadID = pLeadItem->getID();
				pBasisItem->m_nTrailID = pTrailItem->getID();
				pBasisItem->m_nMinSeparation = 2;
				pBasisItem->m_emClassifiBasis = (FlightClassificationBasisType)k;

				m_vectBasis.push_back(pBasisItem);
			}
		}
	}

	//NoneBased
	TakeoffClearanceClassfiBasisItem* pNoneBasisItem = new TakeoffClearanceClassfiBasisItem;
	pNoneBasisItem->m_emClassifiBasis = NoneBased;
	pNoneBasisItem->m_nProjID = m_nProjID;
	pNoneBasisItem->m_nLeadID =0;
	pNoneBasisItem->m_nTrailID =0;
	pNoneBasisItem->m_nTakeoffClearanceItemID = m_nID;
	pNoneBasisItem->m_nMinSeparation = 2;
	m_vectBasis.push_back(pNoneBasisItem);

	SaveClassFicBasisData();

}
//----------------------------------------------------------------------------------
//
TakeoffBehindTakeoffSameRunwayOrCloseParalled::
TakeoffBehindTakeoffSameRunwayOrCloseParalled(int nProjID, TakeoffClearanceType emType)
:TakeoffClearanceItem(nProjID, emType)
,m_nFtLanderDownRunway(0)
,m_nTimeAfter(0)
,m_bDistAtLeast(TRUE)
{
}

TakeoffBehindTakeoffSameRunwayOrCloseParalled::~TakeoffBehindTakeoffSameRunwayOrCloseParalled(void)
{
}

void TakeoffBehindTakeoffSameRunwayOrCloseParalled::InsertDefaultValue()
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFCLEARANCE\
					 (PROJID, TYPE, DISTAPPROACH, TIMEAPPROACH, TIMESLOT, LEADDOWNRW, \
					 AFTERTIME, FLTCLASSFICTYPE)\
					 VALUES (%d,%d,%d,%d,%d,%d,%d,%d)"),
					 m_nProjID,(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,m_nFtLanderDownRunway,
					 m_nTimeAfter,(int)m_emClassficType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	InsertDefaultBasisItem();
}

void TakeoffBehindTakeoffSameRunwayOrCloseParalled::ReadData(CADORecordset& adoRecordset)
{
	TakeoffClearanceItem::ReadData(adoRecordset);

	adoRecordset.GetFieldValue(_T("LEADDOWNRW"),m_nFtLanderDownRunway);
	adoRecordset.GetFieldValue(_T("AFTERTIME"),m_nTimeAfter);
	adoRecordset.GetFieldValue(_T("BDISTATLEAST"),m_bDistAtLeast);

	int nType = 0;
	adoRecordset.GetFieldValue(_T("FLTCLASSFICTYPE"),nType);
	if (nType >=WingspanBased && nType < CategoryType_Count)
	{
		m_emClassficType = (FlightClassificationBasisType)nType;//Current user selected type
	}

	ReadClassficBasisData();

}

void TakeoffBehindTakeoffSameRunwayOrCloseParalled::SaveData()
{

	if (m_nID >=0 && !m_bImport)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFCLEARANCE\
		(PROJID, TYPE, DISTAPPROACH, TIMEAPPROACH, TIMESLOT, LEADDOWNRW, \
		AFTERTIME, FLTCLASSFICTYPE)\
		VALUES (%d,%d,%d,%d,%d,%d,%d,%d)"),
		m_nProjID,(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,m_nFtLanderDownRunway,
		m_nTimeAfter,(int)m_emClassficType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	SaveClassFicBasisData();
	if(m_bImport)m_bImport = false;
}

void TakeoffBehindTakeoffSameRunwayOrCloseParalled::ImportData(CAirsideImportFile& importFile)
{
	TakeoffClearanceItem::ImportData(importFile);
	int nValue = -1;
	importFile.getFile().getInteger(nValue);
	importFile.getFile().getInteger(m_nFtLanderDownRunway);
	importFile.getFile().getInteger(m_nTimeAfter);
	importFile.getFile().getLine();
}

void TakeoffBehindTakeoffSameRunwayOrCloseParalled::ExportData(CAirsideExportFile& exportFile)
{
	TakeoffClearanceItem::ExportData(exportFile);
	exportFile.getFile().writeInt((int)m_bDistAtLeast);
	exportFile.getFile().writeInt(m_nFtLanderDownRunway);
	exportFile.getFile().writeInt(m_nTimeAfter);
    exportFile.getFile().writeLine();
}

void TakeoffBehindTakeoffSameRunwayOrCloseParalled::UpdateData()
{

	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TAKEOFFCLEARANCE\
		SET TYPE =%d, DISTAPPROACH =%d, TIMEAPPROACH =%d, TIMESLOT =%d, \
		LEADDOWNRW =%d, AFTERTIME =%d, FLTCLASSFICTYPE =%d, BDISTATLEAST =%d, BDISTTAKEOFFAPPROAH=%d \
		WHERE (ID = %d)"),(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,m_nFtLanderDownRunway,
		m_nTimeAfter,(int)m_emClassficType, m_bDistAtLeast, m_bDistTakeoffApproach, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	SaveClassFicBasisData();
}
//----------------------------------------------------------------------------------
//
TakeoffBehindTakeoffIntersectingConvergngRunway::
TakeoffBehindTakeoffIntersectingConvergngRunway(int nProjID, TakeoffClearanceType emType)
:TakeoffClearanceItem(nProjID, emType)
{
}

TakeoffBehindTakeoffIntersectingConvergngRunway::~TakeoffBehindTakeoffIntersectingConvergngRunway(void)
{
}

void TakeoffBehindTakeoffIntersectingConvergngRunway::InsertDefaultValue()
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFCLEARANCE\
					 (PROJID, TYPE, DISTAPPROACH, TIMEAPPROACH, TIMESLOT, LEADDOWNRW, \
					 AFTERTIME, FLTCLASSFICTYPE)\
					 VALUES (%d,%d,%d,%d,%d,%d,%d,%d)"),
					 m_nProjID,(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,0,
					 0,(int)m_emClassficType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	InsertDefaultBasisItem();
}

void TakeoffBehindTakeoffIntersectingConvergngRunway::ReadData(CADORecordset& adoRecordset)
{
	TakeoffClearanceItem::ReadData(adoRecordset);

	int nType = 0;
	adoRecordset.GetFieldValue(_T("FLTCLASSFICTYPE"),nType);
	if (nType >=WingspanBased && nType < CategoryType_Count)
	{
		m_emClassficType = (FlightClassificationBasisType)nType;//Current user selected type
	}

	ReadClassficBasisData();

}

void TakeoffBehindTakeoffIntersectingConvergngRunway::ImportData(CAirsideImportFile& importFile)
{
	TakeoffClearanceItem::ImportData(importFile);
	importFile.getFile().getLine();
}

void TakeoffBehindTakeoffIntersectingConvergngRunway::ExportData(CAirsideExportFile& exportFile)
{
	TakeoffClearanceItem::ExportData(exportFile);
	exportFile.getFile().writeLine();
}

void TakeoffBehindTakeoffIntersectingConvergngRunway::SaveData()
{
	if (m_nID >=0 && !m_bImport)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFCLEARANCE\
					 (PROJID, TYPE, DISTAPPROACH, TIMEAPPROACH, TIMESLOT, LEADDOWNRW, \
					 AFTERTIME, FLTCLASSFICTYPE)\
					 VALUES (%d,%d,%d,%d,%d,%d,%d,%d)"),
					 m_nProjID,(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,0,
					 0,(int)m_emClassficType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	SaveClassFicBasisData();
	
	if(m_bImport)m_bImport = false;
}
void TakeoffBehindTakeoffIntersectingConvergngRunway::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TAKEOFFCLEARANCE\
					 SET TYPE =%d, DISTAPPROACH =%d, TIMEAPPROACH =%d, TIMESLOT =%d, \
					 LEADDOWNRW =%d, AFTERTIME =%d, FLTCLASSFICTYPE =%d, BDISTATLEAST =%d, BDISTTAKEOFFAPPROAH=%d \
					 WHERE (ID = %d)"),(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,0,
					 0,(int)m_emClassficType, 0, m_bDistTakeoffApproach, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
	SaveClassFicBasisData();
}
//----------------------------------------------------------------------------------
//
TakeoffBehindLandingSameRunway::
TakeoffBehindLandingSameRunway(int nProjID, TakeoffClearanceType emType)
:TakeoffClearanceItem(nProjID, emType)
,m_nFtLanderDownRunway(0)
{
}

TakeoffBehindLandingSameRunway::~TakeoffBehindLandingSameRunway(void)
{
}


void TakeoffBehindLandingSameRunway::InsertDefaultValue()
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFCLEARANCE\
					 (PROJID, TYPE, DISTAPPROACH, TIMEAPPROACH, TIMESLOT, LEADDOWNRW, \
					 AFTERTIME, FLTCLASSFICTYPE)\
					 VALUES (%d,%d,%d,%d,%d,%d,%d,%d)"),
					 m_nProjID,(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,m_nFtLanderDownRunway,
					 0,(int)m_emClassficType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

}

void TakeoffBehindLandingSameRunway::ReadData(CADORecordset& adoRecordset)
{
	TakeoffClearanceItem::ReadData(adoRecordset);
	adoRecordset.GetFieldValue(_T("LEADDOWNRW"),m_nFtLanderDownRunway);

	int nType = 0;
	adoRecordset.GetFieldValue(_T("FLTCLASSFICTYPE"),nType);
	if (nType >=WingspanBased && nType < CategoryType_Count)
	{
		m_emClassficType = (FlightClassificationBasisType)nType;//Current user selected type
	}

}

void TakeoffBehindLandingSameRunway::SaveData()
{
	if (m_nID >=0 && !m_bImport)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFCLEARANCE\
					 (PROJID, TYPE, DISTAPPROACH, TIMEAPPROACH, TIMESLOT, LEADDOWNRW, \
					 AFTERTIME, FLTCLASSFICTYPE)\
					 VALUES (%d,%d,%d,%d,%d,%d,%d,%d)"),
					 m_nProjID,(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,m_nFtLanderDownRunway,
					 0,(int)m_emClassficType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	
	if(m_bImport)m_bImport = false;
}

void TakeoffBehindLandingSameRunway::ImportData(CAirsideImportFile& importFile)
{
	TakeoffClearanceItem::ImportData(importFile);
	importFile.getFile().getInteger(m_nFtLanderDownRunway);
	importFile.getFile().getLine();
}

void TakeoffBehindLandingSameRunway::ExportData(CAirsideExportFile& exportFile)
{
	TakeoffClearanceItem::ExportData(exportFile);
	exportFile.getFile().writeInt(m_nFtLanderDownRunway);
	exportFile.getFile().writeLine();
}

void TakeoffBehindLandingSameRunway::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TAKEOFFCLEARANCE\
					 SET TYPE =%d, DISTAPPROACH =%d, TIMEAPPROACH =%d, TIMESLOT =%d, \
					 LEADDOWNRW =%d, AFTERTIME =%d, FLTCLASSFICTYPE =%d, BDISTATLEAST =%d, BDISTTAKEOFFAPPROAH=%d \
					 WHERE (ID = %d)"),(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,m_nFtLanderDownRunway,
					 0,(int)m_emClassficType, 0, m_bDistTakeoffApproach, m_nID);



	CADODatabase::ExecuteSQLStatement(strSQL);


}

//----------------------------------------------------------------------------------
//
TakeoffBehindLandingIntersectingConvergingRunway::
TakeoffBehindLandingIntersectingConvergingRunway(int nProjID, TakeoffClearanceType emType)
:TakeoffClearanceItem(nProjID, emType)
{
}

TakeoffBehindLandingIntersectingConvergingRunway::~TakeoffBehindLandingIntersectingConvergingRunway(void)
{
}

void TakeoffBehindLandingIntersectingConvergingRunway::InsertDefaultValue()
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFCLEARANCE\
					 (PROJID, TYPE, DISTAPPROACH, TIMEAPPROACH, TIMESLOT, LEADDOWNRW, \
					 AFTERTIME, FLTCLASSFICTYPE)\
					 VALUES (%d,%d,%d,%d,%d,%d,%d,%d)"),
					 m_nProjID,(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,0,
					 0,(int)m_emClassficType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	InsertDefaultBasisItem();
}
void TakeoffBehindLandingIntersectingConvergingRunway::ReadData(CADORecordset& adoRecordset)
{
	TakeoffClearanceItem::ReadData(adoRecordset);

	int nType = 0;
	adoRecordset.GetFieldValue(_T("FLTCLASSFICTYPE"),nType);
	if (nType >=WingspanBased && nType < CategoryType_Count)
	{
		m_emClassficType = (FlightClassificationBasisType)nType;//Current user selected type
	}

	ReadClassficBasisData();
}

void TakeoffBehindLandingIntersectingConvergingRunway::SaveData()
{
	if (m_nID >=0 && !m_bImport)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFCLEARANCE\
					 (PROJID, TYPE, DISTAPPROACH, TIMEAPPROACH, TIMESLOT, LEADDOWNRW, \
					 AFTERTIME, FLTCLASSFICTYPE)\
					 VALUES (%d,%d,%d,%d,%d,%d,%d,%d)"),
					m_nProjID,(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,0,
					 0,(int)m_emClassficType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	SaveClassFicBasisData();
	if(m_bImport)m_bImport = false;
}

void TakeoffBehindLandingIntersectingConvergingRunway::ImportData(CAirsideImportFile& importFile)
{
	TakeoffClearanceItem::ImportData(importFile);
	importFile.getFile().getLine();
}
void TakeoffBehindLandingIntersectingConvergingRunway::ExportData(CAirsideExportFile& exportFile)
{
	TakeoffClearanceItem::ExportData(exportFile);
	exportFile.getFile().writeLine();
}

void TakeoffBehindLandingIntersectingConvergingRunway::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TAKEOFFCLEARANCE\
					 SET TYPE =%d, DISTAPPROACH =%d, TIMEAPPROACH =%d, TIMESLOT =%d, \
					 LEADDOWNRW =%d, AFTERTIME =%d, FLTCLASSFICTYPE =%d, BDISTATLEAST =%d, BDISTTAKEOFFAPPROAH=%d \
					 WHERE (ID = %d)"),(int)m_emType,m_nDistTakeoffApproach,m_nTimeTakeoffAproach,m_nTimeTakeoffSlot,0,
					 0,(int)m_emClassficType, 0, m_bDistTakeoffApproach, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	SaveClassFicBasisData();
}














