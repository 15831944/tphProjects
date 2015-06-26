#include "StdAfx.h"
#include ".\paxbusparkingplacelist.h"


PaxBusParkingPlace::PaxBusParkingPlace()
:m_enumGateType(DepGate)
,m_emRelate(Random_Type)
{

}

PaxBusParkingPlace::PaxBusParkingPlace( const PaxBusParkingPlace& parkingPlace )
{
	m_strGate = parkingPlace.m_strGate;
	m_terminalSpot = parkingPlace.m_terminalSpot;
	m_enumGateType = parkingPlace.m_enumGateType;
	m_emRelate = parkingPlace.m_emRelate;
}

PaxBusParkingPlace::~PaxBusParkingPlace()
{

}

void PaxBusParkingPlace::SetGate( const CString& strGate )
{
	m_strGate = strGate;
}

const CString& PaxBusParkingPlace::GetGate()
{
	return m_strGate;
}

void PaxBusParkingPlace::SetTerminalSpot( const ALTObjectID& terminalSpot )
{
	m_terminalSpot = terminalSpot;
}

const ALTObjectID& PaxBusParkingPlace::GetTerminalSpot() const
{
	return m_terminalSpot;
}

void PaxBusParkingPlace::SetGateType( PakingPlaceGate emGate )
{
	m_enumGateType = emGate;
}

PaxBusParkingPlace::PakingPlaceGate PaxBusParkingPlace::GetGateType() const
{
	return m_enumGateType;
}

void PaxBusParkingPlace::SetParkingRelate( ParkingRelateType emRelate )
{
	m_emRelate = emRelate;
}

PaxBusParkingPlace::ParkingRelateType PaxBusParkingPlace::GetParkingRelate() const
{
	return m_emRelate;
}

CString PaxBusParkingPlace::GetTableName() const
{
	return _T("IN_PAXBUS_PARKINGPLACE");
}

void PaxBusParkingPlace::GetDataFromDBRecordset( CADORecordset& recordset )
{
	if (recordset.IsEOF())
		return;
	
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("GATENAME"),m_strGate);
	int nGateType = 0;
	recordset.GetFieldValue(_T("GATETYPE"),nGateType);
	m_enumGateType = (PakingPlaceGate)nGateType;

	int nRelateType = 0;
	recordset.GetFieldValue(_T("RELATETYPE"),nRelateType);
	m_emRelate = (ParkingRelateType)nRelateType;

	CString strTerminalSpot;
	recordset.GetFieldValue(_T("TERMINALSPOT"),strTerminalSpot);
	m_terminalSpot.FromString(strTerminalSpot);
}

bool PaxBusParkingPlace::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO IN_PAXBUS_PARKINGPLACE\
					 (GATENAME, GATETYPE, RELATETYPE,TERMINALSPOT)\
					 VALUES ('%s',%d,%d,'%s')"),
					 m_strGate,(int)m_enumGateType,(int)m_emRelate,m_terminalSpot.GetIDString());
	return true;
}

bool PaxBusParkingPlace::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE %s SET GATENAME = '%s', GATETYPE = %d,RELATETYPE = %d,TERMINALSPOT = '%s' WHERE ID = %d"),GetTableName(),
		m_strGate,(int)m_enumGateType,(int)m_emRelate,m_terminalSpot.GetIDString(),m_nID);
	return true;
}

void PaxBusParkingPlace::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM %s WHERE ID = %d"),GetTableName(),m_nID);
}

const PaxBusParkingPlace& PaxBusParkingPlace::operator=( const PaxBusParkingPlace& parkingPlace )
{
	m_strGate = parkingPlace.m_strGate;
	m_terminalSpot = parkingPlace.m_terminalSpot;
	m_enumGateType = parkingPlace.m_enumGateType;
	m_emRelate = parkingPlace.m_emRelate;
	return *this;
}


PaxBusParkingPlaceList::PaxBusParkingPlaceList( void )
{

}

PaxBusParkingPlaceList::~PaxBusParkingPlaceList( void )
{

}

void PaxBusParkingPlaceList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM IN_PAXBUS_PARKINGPLACE"));
}
