#include "StdAfx.h"
#include "..\Database\ADODatabase.h"
#include ".\standleadinline.h"
#include ".\StandLeadOutLine.h"

StandLeadInLine::StandLeadInLine(void)
{
	m_nID = -1;
	m_nPathID = -1;
}

StandLeadInLine::~StandLeadInLine(void)
{
}

void StandLeadInLine::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format("INSERT INTO LEADINLINE (PATHID, STANDID, NAME) VALUES (%d, %d, '%s')", m_nPathID, nParentID, m_strName);
}

void StandLeadInLine::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format("UPDATE LEADINLINE SET PATHID = %d,NAME ='%s' WHERE ID = %d", m_nPathID, m_strName, m_nID );
}

void StandLeadInLine::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format("DELETE FROM LEADINLINE WHERE ID = %d", m_nID);
}


void StandLeadInLine::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue("ID", m_nID);
	recordset.GetFieldValue("PATHID",m_nPathID);
	recordset.GetFieldValue("NAME",m_strName);
}


void StandLeadInLine::DeleteData()
{
	if( m_nPathID >= 0 ){
		CADODatabase::DeletePathFromDatabase(m_nPathID);
	}
	CString strSQL ;
	GetDeleteSQL(strSQL);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void StandLeadInLine::SaveData( int nStandID )
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

void StandLeadInLine::ExportData( CAirsideExportFile& exportFile )
{

}
void StandLeadInLine::ImportData( CAirsideImportFile& importFile )
{

}

void StandLeadInLine::SetPath( const CPath2008& _path )
{
	m_path = _path;
}

void StandLeadInLine::SetName(const CString& str)
{
	m_strName = str; 
}

void StandLeadInLine::SetID(int nID)
{
	m_nID = nID;
}

void StandLeadInLine::CopyData( const StandLeadInLine& otherLine )
{
	m_path = otherLine.GetPath();
	m_strName = otherLine.GetName();
}

void StandLeadInLine::ReadPath()
{
	CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
}

CPath2008 StandLeadInLine::GetPath() const
{
	return m_path;
}

CPath2008& StandLeadInLine::getPath()
{
	return m_path;
}

CString StandLeadInLine::GetName() const
{
	return m_strName;
}

int StandLeadInLine::GetID() const
{
	return m_nID;
}

BOOL StandLeadInLine::PathPointMoreThanOne() const
{
	return m_path.getCount()>=2;
}


void StandLeadInLineList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM LEADINLINE \
					 WHERE (STANDID = %d)"), nParentID);
}

void StandLeadInLineList::CopyData( const StandLeadInLineList& otherList )
{
	m_vDataList.clear();
	m_vDataList.resize(otherList.GetItemCount());
	for(size_t i= 0;i< otherList.GetItemCount();i++)
	{
		m_vDataList[i].CopyData(otherList.ItemAt(i));
	}
}

void StandLeadInLineList::AddNewItem( const StandLeadInLine& newLine )
{
	m_vDataList.push_back(newLine);
	InitItemsName();
}

void StandLeadInLineList::DeleteItem( int idx )
{
	if(idx< (int)GetItemCount())
	{
		m_vDataList.erase( m_vDataList.begin() + idx );
	}
	InitItemsName();
}

void StandLeadInLineList::DeleteData()
{
	for(size_t i=0;i< GetItemCount();i++)
	{
		ItemAt(i).DeleteData();
	}
	m_vDataList.clear();
}

void StandLeadInLineList::ReadData( int nParentID )
{
	m_vDataList.clear();
	ReadDataList(nParentID,m_vDataList);
}

void StandLeadInLineList::SaveData( int nParentID )
{
	std::vector<StandLeadInLine> vDataListInDB;
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

void StandLeadInLineList::ReadDataList( int nParentID, std::vector<StandLeadInLine>& vList )
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
		StandLeadInLine element;
		element.InitFromDBRecordset(adoRecordset);
		vList.push_back(element);
		adoRecordset.MoveNextData();
	} 
	for(int i=0;i< (int)vList.size();i++)
	{
		vList[i].ReadPath();
	}
}

StandLeadInLine* StandLeadInLineList::GetItemByID( int nID )
{
	for(int i=0; i< (int)GetItemCount();i++)
	{
		if( ItemAt(i).GetID() == nID)
			return &ItemAt(i);
	}
	return NULL;
}

size_t StandLeadInLineList::GetItemCount() const
{
	return m_vDataList.size();
}

StandLeadInLine& StandLeadInLineList::ItemAt( size_t i )
{
	return m_vDataList[i];
}

const StandLeadInLine& StandLeadInLineList::ItemAt( size_t i ) const
{
	return m_vDataList[i];
}

void StandLeadInLineList::InitItemsName()
{
	for(int i=0;i< (int)GetItemCount();i++)
	{
		StandLeadInLine& theItem = ItemAt(i);
		CString strName;
		strName.Format("Line %d",i+1);
		theItem.SetName(strName);
	}
}

BOOL StandLeadInLineList::IsValid() const
{
	std::vector<StandLeadInLine>::const_iterator ite = m_vDataList.begin();
	std::vector<StandLeadInLine>::const_iterator iteEn = m_vDataList.end();
	for (;ite!=iteEn;ite++)
	{
		if (FALSE == ite->PathPointMoreThanOne())
		{
			return FALSE;
		}
	}
	return TRUE;
}