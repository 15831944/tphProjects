#include "StdAfx.h"
#include ".\leadoutline.h"
#include "..\Database\ADODatabase.h"

LeadOutLine::LeadOutLine(void)
{
	m_nID = -1;
	m_nPathID = -1;
}

LeadOutLine::~LeadOutLine(void)
{
}

//TYPE: 0 is Lead in, 1 is Lead out
void LeadOutLine::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format("INSERT INTO LEADLINE (PARENTID, PATHID, TYPE, NAME)\
				  VALUES (%d, %d, %d, '%s')",nParentID, m_nPathID, 1, m_strName);
}

void LeadOutLine::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format("UPDATE LEADLINE SET PATHID = %d,NAME ='%s' WHERE ID = %d", m_nPathID, m_strName, m_nID );
}

void LeadOutLine::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format("DELETE FROM LEADLINE WHERE ID = %d", m_nID);
}

void LeadOutLine::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue("ID", m_nID);
	recordset.GetFieldValue("PATHID",m_nPathID);
	recordset.GetFieldValue("NAME",m_strName);
}

void LeadOutLine::SaveData( int nStandID )
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
		GetInsertSQL(nStandID,strSQL);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}


void LeadOutLine::DeleteData()
{
	if( m_nPathID >= 0 )
	{
		CADODatabase::DeletePathFromDatabase(m_nPathID);
	}
	CString strSQL ;
	GetDeleteSQL(strSQL);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void LeadOutLine::CopyData( const LeadOutLine& otherLine )
{
	m_path = otherLine.GetPath();
	m_strName = otherLine.GetName();
}

void LeadOutLine::ReadPath()
{
	CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
}

CPath2008 LeadOutLine::GetPath() const
{
	return m_path;
}

CPath2008& LeadOutLine::getPath()
{
	return m_path;
}

CString LeadOutLine::GetName() const
{
	return m_strName;
}

int LeadOutLine::GetID() const
{
	return m_nID;
}

void LeadOutLineList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM LEADLINE\
					 WHERE (PARENTID = %d) AND (TYPE = 1)"), nParentID);

}

void LeadOutLineList::CopyData( const LeadOutLineList& otherList )
{
	m_vDataList.clear();
	m_vDataList.resize(otherList.GetItemCount());

	for(int i= 0;i< otherList.GetItemCount();i++)
	{
		m_vDataList[i].CopyData(otherList.ItemAt(i));
	}
}

void LeadOutLineList::AddNewItem( const LeadOutLine& newLine )
{
	m_vDataList.push_back(newLine);
	InitItemsName();
}

void LeadOutLineList::DeleteItem( int idx )
{
	if( idx< (int)GetItemCount() )
	{
		m_vDataList.erase( m_vDataList.begin() + idx );
	}
	InitItemsName();
}

void LeadOutLineList::DeleteData()
{
	for(int i=0;i< GetItemCount();i++)
	{
		ItemAt(i).DeleteData();
	}
	m_vDataList.clear();
}

void LeadOutLineList::ReadData( int nParentID )
{
	m_vDataList.clear();
	ReadDataList(nParentID,m_vDataList);
}

void LeadOutLineList::SaveData( int nParentID )
{
	std::vector<LeadOutLine> vDataListInDB;
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

void LeadOutLineList::ReadDataList( int nParentID, std::vector<LeadOutLine>& vList )
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
		LeadOutLine element;
		element.InitFromDBRecordset(adoRecordset);
		vList.push_back(element);
		adoRecordset.MoveNextData();
	} 
	for(int i=0;i<(int)vList.size();i++)
	{
		vList[i].ReadPath();
	}
}

LeadOutLine* LeadOutLineList::GetItemByID( int nID )
{
	for(int i=0; i< (int)GetItemCount();i++)
	{
		if( ItemAt(i).GetID() == nID)
			return &ItemAt(i);
	}
	return NULL;
}

int LeadOutLineList::GetItemCount() const
{
	return (int)m_vDataList.size();
}

LeadOutLine& LeadOutLineList::ItemAt( int idx )
{
	return m_vDataList[idx];
}

const LeadOutLine& LeadOutLineList::ItemAt( int idx ) const
{
	return m_vDataList[idx];
}

void LeadOutLineList::InitItemsName()
{
	for(int i=0;i< (int)GetItemCount();i++)
	{
		LeadOutLine& theItem = ItemAt(i);
		CString strName;
		strName.Format("Line %d",i+1);
		theItem.SetName(strName);
	}
}