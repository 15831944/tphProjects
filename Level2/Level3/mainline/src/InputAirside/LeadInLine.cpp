#include "StdAfx.h"
#include ".\leadinline.h"
#include "..\Database\ADODatabase.h"

LeadInLine::LeadInLine(void)
{
	m_nID = -1;
	m_nPathID = -1;
}

LeadInLine::~LeadInLine(void)
{
}

//TYPE: 0 is Lead in, 1 is Lead out
void LeadInLine::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format("INSERT INTO LEADLINE (PARENTID, PATHID, TYPE, NAME)\
		VALUES (%d, %d, %d, '%s')",nParentID, m_nPathID, 0, m_strName);
}

void LeadInLine::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format("UPDATE LEADLINE SET PATHID = %d,NAME ='%s' WHERE ID = %d", m_nPathID, m_strName, m_nID );
}

void LeadInLine::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format("DELETE FROM LEADLINE WHERE ID = %d", m_nID);
}


void LeadInLine::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue("ID", m_nID);
	recordset.GetFieldValue("PATHID",m_nPathID);
	recordset.GetFieldValue("NAME",m_strName);
}


void LeadInLine::DeleteData()
{
	if( m_nPathID >= 0 )
	{
		CADODatabase::DeletePathFromDatabase(m_nPathID);
	}
	CString strSQL ;
	GetDeleteSQL(strSQL);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void LeadInLine::SaveData( int nParentID )
{
	if(m_nPathID < 0)
	{
		m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);
	}
	else
	{
		CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	}

	CString strSQL;
	if(m_nID < 0)
	{
		GetInsertSQL(nParentID,strSQL);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void LeadInLine::SetPath( const CPath2008& _path )
{
	m_path = _path;
}

void LeadInLine::CopyData( const LeadInLine& otherLine )
{
	m_path = otherLine.GetPath();
	m_strName = otherLine.GetName();
}

void LeadInLine::ReadPath()
{
	CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
}

CPath2008 LeadInLine::GetPath() const
{
	return m_path;
}

CPath2008& LeadInLine::getPath()
{
	return m_path;
}

CString LeadInLine::GetName() const
{
	return m_strName;
}

int LeadInLine::GetID() const
{
	return m_nID;
}

void LeadInLineList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM LEADLINE \
					 WHERE (PARENTID = %d) AND( TYPE = 0)"), nParentID);
}

void LeadInLineList::CopyData( const LeadInLineList& otherList )
{
	m_vDataList.clear();
	m_vDataList.resize(otherList.GetItemCount());
	for(int i= 0;i< (int)otherList.GetItemCount();i++)
	{
		m_vDataList[i].CopyData(otherList.ItemAt(i));
	}
}

void LeadInLineList::AddNewItem( const LeadInLine& newLine )
{
	m_vDataList.push_back(newLine);
	InitItemsName();
}

void LeadInLineList::DeleteItem( int idx )
{
	if(idx< (int)GetItemCount())
	{
		m_vDataList.erase( m_vDataList.begin() + idx );
	}
	InitItemsName();
}

void LeadInLineList::DeleteData()
{
	for(int i=0;i< (int)GetItemCount();i++)
	{
		ItemAt(i).DeleteData();
	}
	m_vDataList.clear();
}

void LeadInLineList::ReadData( int nParentID )
{
	m_vDataList.clear();
	ReadDataList(nParentID,m_vDataList);
}

void LeadInLineList::SaveData( int nParentID )
{
	std::vector<LeadInLine> vDataListInDB;
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

void LeadInLineList::ReadDataList( int nParentID, std::vector<LeadInLine>& vList )
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
		LeadInLine element;
		element.InitFromDBRecordset(adoRecordset);
		vList.push_back(element);
		adoRecordset.MoveNextData();
	} 
	for(int i=0;i< (int)vList.size();i++)
	{
		vList[i].ReadPath();
	}
}

LeadInLine* LeadInLineList::GetItemByID( int nID )
{
	for(int i=0; i< (int)GetItemCount();i++)
	{
		if( ItemAt(i).GetID() == nID)
			return &ItemAt(i);
	}
	return NULL;
}

size_t LeadInLineList::GetItemCount() const
{
	return m_vDataList.size();
}

LeadInLine& LeadInLineList::ItemAt( int nIdx )
{
	return m_vDataList[nIdx];
}

const LeadInLine& LeadInLineList::ItemAt( int nIdx ) const
{
	return m_vDataList[nIdx];
}

void LeadInLineList::InitItemsName()
{
	for(int i=0;i< (int)GetItemCount();i++)
	{
		LeadInLine& theItem = ItemAt(i);
		CString strName;
		strName.Format("Line %d",i+1);
		theItem.SetName(strName);
	}
}