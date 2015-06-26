#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\baggageparkingspecdata.h"
#include "AirsideImportExportManager.h"


Path BaggageCartParkingItem::GetParkArea() const
{
	return m_parkArea;
}

void BaggageCartParkingItem::SetParkArea( const Path& path )
{
	m_parkArea = path;
}

BaggageCartParkingItem& BaggageCartParkingItem::SetPushers( const CString& strPushers )
{
	m_strPushers = strPushers;
	return *this;
}

void BaggageCartParkingItem::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);	
	recordset.GetFieldValue(_T("PUSHERNAME"),m_strPushers);
	recordset.GetFieldValue(_T("PATHID"),m_nPathId);

	int nPointCnt = -1;
	CADODatabase::ReadPathFromDatabase(m_nPathId,m_parkArea);
	//recordset.GetFieldValue(_T("POINTCOUNT"),nPointCnt);
	//recordset.GetFieldValue(_T("PATH"),nPointCnt,m_parkArea);
	
}

void BaggageCartParkingItem::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO  BAGGAGE_PARKING \
					( PUSHERNAME \
					, PATHID \
					, PROJID)\
					VALUES\
					('%s',%d,%d)"),m_strPushers,m_nPathId,nParentID);
}

void BaggageCartParkingItem::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE BAGGAGE_PARKING\
					 SET PUSHERNAME = '%s', \
					 PATHID = %d \
					 WHERE (ID = %d)"),
					 m_strPushers,
					 m_nPathId,
					 m_nID				
				  );
}

void BaggageCartParkingItem::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM BAGGAGE_PARKING \
					 WHERE (ID = %d)"),m_nID);
}

void BaggageCartParkingItem::ExportData( CAirsideExportFile& exportFile )
{
	exportFile.getFile().writeField(m_strPushers);
	//path
	int nPathCount = m_parkArea.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i =0; i < nPathCount; ++i)
	{
		exportFile.getFile().writePoint(m_parkArea.getPoint(i));
	}
	exportFile.getFile().writeLine();
}

void BaggageCartParkingItem::ImportData( CAirsideImportFile& importFile )
{
	importFile.getFile().getField(m_strPushers.GetBuffer(1024),1024);
	int nPointCnt = -1;
	importFile.getFile().getInteger(nPointCnt);
	
	ASSERT(nPointCnt> 0);
	m_parkArea.init(nPointCnt);
	for(int i=0;i< nPointCnt;i++)
	{
		importFile.getFile().getPoint(m_parkArea[i]);
	}
	importFile.getFile().getLine();
	SaveData(importFile.getNewProjectID());
}

void BaggageCartParkingItem::SaveData( int nPrjID )
{
	if(m_nPathId < 0)
	{
		m_nPathId = CADODatabase::SavePathIntoDatabase(m_parkArea);
	}else
	{
		CADODatabase::UpdatePathInDatabase(m_parkArea,m_nPathId);
	}

	CString strSQL;
	if(m_nID < 0)
	{
		GetInsertSQL(nPrjID,strSQL);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}else
	{
		GetUpdateSQL(strSQL);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
		
}

BaggageCartParkingItem::BaggageCartParkingItem()
{
	m_nPathId = -1;
}

void BaggageCartParkingItem::DeleteData()
{
	if( m_nPathId >= 0 ){
		CADODatabase::DeletePathFromDatabase(m_nPathId);
	}

	CString strSQL ;
	GetDeleteSQL(strSQL);
	CADODatabase::ExecuteSQLStatement(strSQL);

}

void BaggageCartParkingSpecData::GetSelectElementSQL( int nParentID,CString& strSQL )const
{
	strSQL.Format(_T("SELECT ID, PUSHERNAME, PATHID\
					 FROM BAGGAGE_PARKING\
					 WHERE (PROJID = %d)"), nParentID);

}

void BaggageCartParkingSpecData::ImportBagCartParking( CAirsideImportFile& importFile )
{
	while (!importFile.getFile().isBlank())
	{
		BaggageCartParkingItem bagCartParking;
		bagCartParking.ImportData(importFile);
	}

}

void BaggageCartParkingSpecData::ExportBagCartParking( CAirsideExportFile& exportFile )
{
	BaggageCartParkingSpecData bagCartData;
	bagCartData.ReadData(exportFile.GetProjectID());

	exportFile.getFile().writeField("BagCartParking");
	exportFile.getFile().writeLine();

	size_t itemCount = bagCartData.GetElementCount();

	for (size_t i =0; i < itemCount; ++i)
	{
		bagCartData.GetItem(i)->ExportData(exportFile);	
	}
	exportFile.getFile().endFile();

}

void BaggageCartParkingSpecData::DoNotCall()
{
	BaggageCartParkingSpecData bagcart;
	bagcart.AddNewItem(NULL);
}