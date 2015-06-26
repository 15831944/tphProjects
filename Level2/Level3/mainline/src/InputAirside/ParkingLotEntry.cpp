#include "StdAfx.h"
#include ".\parkinglotentry.h"
#include "..\Database\ADODatabase.h"

ParkingLotEntry::ParkingLotEntry(void)
{
	m_nID = -1;
	m_nPathID = -1;
}

ParkingLotEntry::~ParkingLotEntry(void)
{
}

//TYPE: 0 is Entry, 1 is Exit
void ParkingLotEntry::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format("INSERT INTO PARKINGENTRY_EXIT(PARENTID, PATHID, TYPE, NAME)\
				  VALUES (%d, %d, %d, '%s')", nParentID, m_nPathID, 0,m_strName);
}

void ParkingLotEntry::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format("UPDATE PARKINGENTRY_EXIT SET PATHID = %d , NAME ='%s' WHERE ID = %d", m_nPathID,m_strName, m_nID );
}

void ParkingLotEntry::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format("DELETE FROM PARKINGENTRY_EXIT WHERE ID = %d", m_nID);
}


void ParkingLotEntry::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue("ID", m_nID);
	recordset.GetFieldValue("PATHID",m_nPathID);
	recordset.GetFieldValue("NAME",m_strName);
}


void ParkingLotEntry::DeleteData()
{
	if( m_nPathID >= 0 ){
		CADODatabase::DeletePathFromDatabase(m_nPathID);
	}
	CString strSQL ;
	GetDeleteSQL(strSQL);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ParkingLotEntry::SaveData( int nParentID )
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
	}else
	{
		GetUpdateSQL(strSQL);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void ParkingLotEntry::SetPath( const CPath2008& _path )
{
	m_path = _path;
}

ParkingLotEntry& ParkingLotEntry::operator= ( const ParkingLotEntry& otherLine )
{
	m_path = otherLine.GetPath();
	m_strName = otherLine.GetName();

	return *this;
}

void ParkingLotEntry::ReadPath()
{
	CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
}

CPath2008 ParkingLotEntry::GetPath() const
{
	return m_path;
}

CPath2008& ParkingLotEntry::getPath()
{
	return m_path;
}

CString ParkingLotEntry::GetName() const
{
	return m_strName;
}

int ParkingLotEntry::GetID() const
{
	return m_nID;
}

LeadInLineList& ParkingLotEntry::GetLeadInLineList()
{
	return m_vLeadInLines;
}

LeadOutLineList& ParkingLotEntry::GetLeadOutLineList()
{
	return m_vLeadOutLines;
}
///////////////////////////////////////ParkingLotEntryList///////////////////////////////////////////////
ParkingLotEntryList::ParkingLotEntryList(void)
{
	m_vEntryList.clear();
	m_vDelEntryList.clear();
}

ParkingLotEntryList::~ParkingLotEntryList(void)
{
	ClearList();
}

void ParkingLotEntryList::ClearList()
{
	int nCount = (int)m_vEntryList.size();
	for(int i =0; i < nCount; i++)
	{
		ParkingLotEntry* pData = m_vEntryList.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vEntryList.clear();

	nCount = (int)m_vDelEntryList.size();
	for(int i =0; i < nCount; i++)
	{
		ParkingLotEntry* pData = m_vDelEntryList.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vDelEntryList.clear();
}



void ParkingLotEntryList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM  PARKINGENTRY_EXIT\
					 WHERE ( PARENTID= %d) AND (TYPE = 0)"), nParentID);
}

void ParkingLotEntryList::CopyData( const ParkingLotEntryList& otherList )
{
	ClearList();
	m_vEntryList.resize(otherList.GetItemCount());
	for(int i= 0;i< otherList.GetItemCount();i++)
	{
		ParkingLotEntry* pData = new ParkingLotEntry;
		ParkingLotEntry* pOtherData = otherList.m_vEntryList[i];
		*pData = *pOtherData;
		m_vEntryList.push_back(pData);
	}
}

void ParkingLotEntryList::AddNewItem(ParkingLotEntry* newLine )
{
	m_vEntryList.push_back(newLine);
	InitItemsName();
}

void ParkingLotEntryList::DeleteItem( int idx )
{
	if(idx< (int)GetItemCount() && idx >=0)
	{
		m_vDelEntryList.push_back(m_vEntryList.at(idx));
		m_vEntryList.erase( m_vEntryList.begin() + idx );
	}
	InitItemsName();
}

void ParkingLotEntryList::DeleteData()
{
	int nCount = GetItemCount();
	for(int i=0;i< nCount;i++)
	{
		ItemAt(i)->DeleteData();
	}
	
	nCount = (int)m_vDelEntryList.size();
	for (int i =0; nCount; i++)
	{
		m_vDelEntryList[i]->DeleteData();
	}
}

void ParkingLotEntryList::ReadData( int nParentID )
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
		ParkingLotEntry* element = new ParkingLotEntry;
		element->InitFromDBRecordset(adoRecordset);
		m_vEntryList.push_back(element);
		adoRecordset.MoveNextData();
	} 
	for(int i=0;i< (int)m_vEntryList.size();i++)
	{
		m_vEntryList[i]->ReadPath();
	}
}

void ParkingLotEntryList::SaveData( int nParentID )
{
	//delete deleted data;
	int nCount = (int)m_vDelEntryList.size();
	for(int i=0;i< nCount;i++)
	{
		m_vDelEntryList[i]->DeleteData();
	}

	//save data
	nCount = (int)m_vEntryList.size();
	for (int i=0;i< nCount;i++)
	{
		m_vEntryList[i]->SaveData(nParentID);
	}
}

int ParkingLotEntryList::GetItemCount() const
{
	return (int)m_vEntryList.size();
}

ParkingLotEntry* ParkingLotEntryList::ItemAt( int nIdx )
{
	return m_vEntryList[nIdx];
}

void ParkingLotEntryList::InitItemsName()
{
	for(int i=0;i< (int)GetItemCount();i++)
	{
		ParkingLotEntry* pItem = ItemAt(i);
		CString strName;
		strName.Format("Entry %d",i+1);
		pItem->SetName(strName);
	}
}