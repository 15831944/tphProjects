#include "StdAfx.h"
#include ".\parkinglotexit.h"
#include "..\Database\ADODatabase.h"

ParkingLotExit::ParkingLotExit(void)
{
	m_nID = -1;
	m_nPathID = -1;

}

ParkingLotExit::~ParkingLotExit(void)
{
}

//TYPE: 0 is Entry, 1 is Exit
void ParkingLotExit::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format("INSERT INTO PARKINGENTRY_EXIT(PARENTID, PATHID, TYPE, NAME)\
				  VALUES (%d, %d, %d, '%s')", nParentID, m_nPathID, 1,m_strName);
}

void ParkingLotExit::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format("UPDATE PARKINGENTRY_EXIT SET PATHID = %d , NAME ='%s' WHERE ID = %d", m_nPathID,m_strName, m_nID );
}

void ParkingLotExit::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format("DELETE FROM PARKINGENTRY_EXIT WHERE ID = %d", m_nID);
}


void ParkingLotExit::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue("ID", m_nID);
	recordset.GetFieldValue("PATHID",m_nPathID);
	recordset.GetFieldValue("NAME",m_strName);
}


void ParkingLotExit::DeleteData()
{
	if( m_nPathID >= 0 ){
		CADODatabase::DeletePathFromDatabase(m_nPathID);
	}
	CString strSQL ;
	GetDeleteSQL(strSQL);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ParkingLotExit::SaveData( int nParentID )
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

void ParkingLotExit::SetPath( const CPath2008& _path )
{
	m_path = _path;
}

ParkingLotExit& ParkingLotExit::operator= ( const ParkingLotExit& otherLine )
{
	m_path = otherLine.GetPath();
	m_strName = otherLine.GetName();
	m_vLeadInLines = otherLine.m_vLeadInLines;
	m_vLeadOutLines = otherLine.m_vLeadOutLines;
	return *this;
}

void ParkingLotExit::ReadPath()
{
	CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
}

CPath2008 ParkingLotExit::GetPath() const
{
	return m_path;
}

CPath2008& ParkingLotExit::getPath()
{
	return m_path;
}

CString ParkingLotExit::GetName() const
{
	return m_strName;
}

int ParkingLotExit::GetID() const
{
	return m_nID;
}

LeadInLineList& ParkingLotExit::GetLeadInLineList()
{
	return m_vLeadInLines;
}

LeadOutLineList& ParkingLotExit::GetLeadOutLineList()
{
	return m_vLeadOutLines;
}
///////////////////////////////////////ParkingLotExitList///////////////////////////////////////////////
ParkingLotExitList::ParkingLotExitList()
{
	m_vExitList.clear();
	m_vDelExitList.clear();
}

ParkingLotExitList::~ParkingLotExitList()
{
	ClearList();
}

void ParkingLotExitList::ClearList()
{
	int nCount = (int)m_vExitList.size();
	for(int i =0; i < nCount; i++)
	{
		ParkingLotExit* pData = m_vExitList.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vExitList.clear();

	nCount = (int)m_vDelExitList.size();
	for(int i =0; i < nCount; i++)
	{
		ParkingLotExit* pData = m_vDelExitList.at(i);
		if (pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_vDelExitList.clear();
}

void ParkingLotExitList::CopyData(const ParkingLotExitList& otherList)
{
	ClearList();
	int nCount = otherList.GetItemCount();
	m_vExitList.resize(nCount);
	for(int i= 0;i< nCount;i++)
	{
		ParkingLotExit* pData = new ParkingLotExit;
		ParkingLotExit* pOtherData = otherList.m_vExitList[i];
		*pData = *pOtherData;
		m_vExitList.push_back(pData);
	}
}

void ParkingLotExitList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM  PARKINGENTRY_EXIT\
					  WHERE ( PARENTID= %d) AND (TYPE = 1)"), nParentID);
}

void ParkingLotExitList::AddNewItem( ParkingLotExit* newLine )
{
	m_vExitList.push_back(newLine);
	InitItemsName();
}

void ParkingLotExitList::DeleteItem( int idx )
{
	if(idx< GetItemCount() && idx >=0)
	{
		m_vDelExitList.push_back(m_vExitList.at(idx));
		m_vExitList.erase( m_vExitList.begin() + idx );
	}
	InitItemsName();
}

void ParkingLotExitList::DeleteData()
{
	for(int i=0;i< GetItemCount();i++)
	{
		ItemAt(i)->DeleteData();
	}

	int nCount =(int) m_vDelExitList.size();
	for (int i =0; i < nCount; i++)
	{
		ParkingLotExit* pData = m_vDelExitList.at(i);
		pData->DeleteData();
	}
}

void ParkingLotExitList::ReadData( int nParentID )
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
		ParkingLotExit* element = new ParkingLotExit;
		element->InitFromDBRecordset(adoRecordset);
		m_vExitList.push_back(element);
		adoRecordset.MoveNextData();
	} 
	for(int i=0;i< (int)m_vExitList.size();i++)
	{
		m_vExitList[i]->ReadPath();
	}
}

void ParkingLotExitList::SaveData( int nParentID )
{
	//delete deleted data;
	int nCount = (int)m_vDelExitList.size();
	for(int i=0;i< nCount;i++)
	{
		m_vDelExitList[i]->DeleteData();
	}

	//save data
	nCount = (int)m_vExitList.size();
	for (int i=0;i< nCount;i++)
	{
		m_vExitList[i]->SaveData(nParentID);
	}
}

int ParkingLotExitList::GetItemCount() const
{
	return (int)m_vExitList.size();
}

ParkingLotExit* ParkingLotExitList::ItemAt( int nIdx )
{
	return m_vExitList[nIdx];
}

void ParkingLotExitList::InitItemsName()
{
	for(int i=0;i< (int)GetItemCount();i++)
	{
		ParkingLotExit* pItem = ItemAt(i);
		CString strName;
		strName.Format("Exit %d",i+1);
		pItem->SetName(strName);
	}
}