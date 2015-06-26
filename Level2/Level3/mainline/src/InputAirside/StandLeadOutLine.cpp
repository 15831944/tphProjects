#include "StdAfx.h"
#include "..\Database\ADODatabase.h"
#include ".\standleadoutline.h"

StandLeadOutLine::StandLeadOutLine(void)
{
	m_nPathID = -1;
	m_nID = -1;
	m_bPushBack =FALSE;
	m_bSetReleasePoint = FALSE;
	m_dlt = 0.0;
}

StandLeadOutLine::~StandLeadOutLine(void)
{
}

void StandLeadOutLine::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format("INSERT INTO LEADOUTLINE (PATHID, STANDID, BPUSHBACK, NAME,RELEASEPOINTDISTANCE,RELEASEPOINT) VALUES (%d, %d, %d, '%s',%f,%d)",\
		m_nPathID, nParentID, m_bPushBack, m_strName,m_dlt,m_bSetReleasePoint);
}

void StandLeadOutLine::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format("UPDATE LEADOUTLINE SET PATHID = %d , BPUSHBACK= %d ,NAME='%s',RELEASEPOINTDISTANCE=%f,RELEASEPOINT = %d WHERE ID = %d", m_nPathID,m_bPushBack, m_strName,\
		m_dlt,m_bSetReleasePoint,m_nID );
}

void StandLeadOutLine::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format("DELETE FROM LEADOUTLINE WHERE ID = %d", m_nID);
}

void StandLeadOutLine::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue("ID", m_nID);
	recordset.GetFieldValue("PATHID",m_nPathID);
	recordset.GetFieldValue("BPUSHBACK",m_bPushBack);
	recordset.GetFieldValue("NAME",m_strName);

	BOOL bInvalid = FALSE;
	bInvalid = recordset.GetFieldValue(_T("RELEASEPOINTDISTANCE"),m_dlt);
	if (!bInvalid)
	{
		m_dlt =0.0;
	}

	bInvalid = FALSE;
	bInvalid = recordset.GetFieldValue(_T("RELEASEPOINT"),m_bSetReleasePoint);
	if (!bInvalid)
	{
		m_bSetReleasePoint = FALSE;
	}
}

void StandLeadOutLine::SaveData( int nStandID )
{
	if(m_nPathID < 0)
	{
		m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);
	}else
	{
		CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	}

	CString strSQL;
	if(m_nID < 0)
	{
		GetInsertSQL(nStandID,strSQL);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}else
	{
		GetUpdateSQL(strSQL);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}


void StandLeadOutLine::DeleteData()
{	
	if( m_nPathID >= 0 ){
		CADODatabase::DeletePathFromDatabase(m_nPathID);
	}
	CString strSQL ;
	GetDeleteSQL(strSQL);
	CADODatabase::ExecuteSQLStatement(strSQL);
}
void StandLeadOutLine::ExportData( CAirsideExportFile& exportFile )
{

}

void StandLeadOutLine::ImportData( CAirsideImportFile& importFile )
{

}

void StandLeadOutLine::CopyData( const StandLeadOutLine& otherLine )
{
	m_path = otherLine.GetPath();
	m_strName = otherLine.GetName();
	m_bPushBack = otherLine.IsPushBack();
	m_bSetReleasePoint = otherLine.IsSetReleasePoint();
	m_dlt = otherLine.m_dlt;
}

void StandLeadOutLine::ReadPath()
{
	CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
}

CPath2008 StandLeadOutLine::GetPath() const
{
	return m_path;
}

CPath2008& StandLeadOutLine::getPath()
{
	return m_path;
}
void StandLeadOutLine::SetPath(CPath2008 path)
{
	m_path = path;
}

void StandLeadOutLine::SetPushBack(BOOL b)
{
	m_bPushBack = b;
}

BOOL StandLeadOutLine::IsPushBack() const
{
	return m_bPushBack;
}

BOOL StandLeadOutLine::IsSetReleasePoint()const
{
	return m_bSetReleasePoint;
}

void StandLeadOutLine::toSetReleasePoint(BOOL bSet)
{
	m_bSetReleasePoint = bSet;
}

DistanceUnit StandLeadOutLine::GetDistance()const
{
	return m_dlt;
}

CString StandLeadOutLine::GetName() const
{
	return m_strName;
}

int StandLeadOutLine::GetID() const
{
	return m_nID;
}
void StandLeadOutLine::SetName(const CString& str)
{
	m_strName = str;
}

void StandLeadOutLine::SetID(int nID)
{
	m_nID = nID;
}

void StandLeadOutLine::SetReleasePoint(const CPoint2008& pt)
{
	m_dlt = m_path.getPoint(0).distance(pt);
}

const CPoint2008 StandLeadOutLine::GetReleasePoint()const
{
	return m_path.GetDistPoint(m_dlt);
}

BOOL StandLeadOutLine::PathPointMoreThanOne() const
{
	return m_path.getCount()>=2;
}

void StandLeadOutLineList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT *\
					 FROM LEADOUTLINE\
					 WHERE (STANDID = %d)"), nParentID);

}

void StandLeadOutLineList::CopyData( const StandLeadOutLineList& otherList )
{
	m_vDataList.clear();
	m_vDataList.resize(otherList.GetItemCount());
	for(size_t i= 0;i< otherList.GetItemCount();i++)
	{
		m_vDataList[i].CopyData(otherList.ItemAt(i));
	}
}

void StandLeadOutLineList::AddNewItem( const StandLeadOutLine& newLine )
{
	m_vDataList.push_back(newLine);
	InitItemsName();
}

void StandLeadOutLineList::DeleteItem( int idx )
{
	if( idx< (int)GetItemCount() )
	{
		m_vDataList.erase( m_vDataList.begin() + idx );
	}
	InitItemsName();
}

void StandLeadOutLineList::DeleteData()
{
	for(size_t i=0;i< GetItemCount();i++)
	{
		ItemAt(i).DeleteData();
	}
	m_vDataList.clear();
}

void StandLeadOutLineList::ReadData( int nParentID )
{
	m_vDataList.clear();
	ReadDataList(nParentID,m_vDataList);
}

void StandLeadOutLineList::SaveData( int nParentID )
{
	std::vector<StandLeadOutLine> vDataListInDB;
	ReadDataList(nParentID,vDataListInDB);
	
	//save data
	for (size_t i=0;i< m_vDataList.size();i++)
	{
		m_vDataList[i].SaveData(nParentID);
	}
	//delete data;
	for(size_t i=0;i<vDataListInDB.size();i++)
	{
		bool bTodelete = true;
		for(size_t j=0;j<m_vDataList.size();j++)
		{
			if( vDataListInDB[i].m_nID == m_vDataList[j].m_nID )
			{
				bTodelete = false;
			}
		}
		if(bTodelete)
			vDataListInDB[i].DeleteData();
	}
}

void StandLeadOutLineList::ReadDataList( int nParentID, std::vector<StandLeadOutLine>& vList )
{
	CString strSelectSQL;
	GetSelectElementSQL(nParentID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		StandLeadOutLine element;
		element.InitFromDBRecordset(adoRecordset);
		vList.push_back(element);
		adoRecordset.MoveNextData();
	} 
	for(int i=0;i<(int)vList.size();i++)
	{
		vList[i].ReadPath();
	}
}

StandLeadOutLine* StandLeadOutLineList::GetItemByID( int nID )
{
	for(int i=0; i< (int)GetItemCount();i++)
	{
		if( ItemAt(i).GetID() == nID)
			return &ItemAt(i);
	}
	return NULL;
}

size_t StandLeadOutLineList::GetItemCount() const
{
	return m_vDataList.size();
}

StandLeadOutLine& StandLeadOutLineList::ItemAt( size_t i )
{
	return m_vDataList[i];
}

const StandLeadOutLine& StandLeadOutLineList::ItemAt( size_t i ) const
{
	return m_vDataList[i];
}

void StandLeadOutLineList::InitItemsName()
{
	for(int i=0;i< (int)GetItemCount();i++)
	{
		StandLeadOutLine& theItem = ItemAt(i);
		CString strName;
		strName.Format("Line %d",i+1);
		theItem.SetName(strName);
	}
}

BOOL StandLeadOutLineList::IsValid() const
{
	std::vector<StandLeadOutLine>::const_iterator ite = m_vDataList.begin();
	std::vector<StandLeadOutLine>::const_iterator iteEn = m_vDataList.end();
	for (;ite!=iteEn;ite++)
	{
		if (FALSE == ite->PathPointMoreThanOne())
		{
			return FALSE;
		}
	}
	return TRUE;
}