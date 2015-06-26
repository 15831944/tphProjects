#include "StdAfx.h"
#include ".\baggagecartparkinglinkage.h"
#include "Database\ADODatabase.h"

BaggageParkingPlace::BaggageParkingPlace()
:m_emRelate(Random_Type)
{

}

BaggageParkingPlace::BaggageParkingPlace( const BaggageParkingPlace& parkingPlace )
{
	m_emRelate = parkingPlace.m_emRelate;
	m_strBaggageParking = parkingPlace.m_strBaggageParking;
	m_terminalSpot = parkingPlace.m_terminalSpot;
}

BaggageParkingPlace::~BaggageParkingPlace()
{

}

void BaggageParkingPlace::SetParkingRelate( ParkingRelateType emRelate )
{
	m_emRelate = emRelate;
}

BaggageParkingPlace::ParkingRelateType BaggageParkingPlace::GetParkingRelate() const
{
	return m_emRelate;
}

void BaggageParkingPlace::SetParkingProcessor( const CString& strProc )
{
	m_strBaggageParking = strProc;
}

const CString& BaggageParkingPlace::GetParkingProcessor() const
{
	return m_strBaggageParking;
}

void BaggageParkingPlace::SetBagTrainSpot( const ALTObjectID& trainSpot )
{
	m_terminalSpot = trainSpot;
}

const ALTObjectID& BaggageParkingPlace::GetBagTrainSpot() const
{
	return m_terminalSpot;
}

CString BaggageParkingPlace::GetTableName() const
{
	return _T("IN_BAGCART_PARKINGPLACE");
}

void BaggageParkingPlace::GetDataFromDBRecordset( CADORecordset& recordset )
{
	if (recordset.IsEOF())
		return;

	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("PROCNAME"),m_strBaggageParking);

	int nRelateType = 0;
	recordset.GetFieldValue(_T("RELATETYPE"),nRelateType);
	m_emRelate = (ParkingRelateType)nRelateType;

	CString strTerminalSpot;
	recordset.GetFieldValue(_T("BAGCARTSPOT"),strTerminalSpot);
	m_terminalSpot.FromString(strTerminalSpot);
}

bool BaggageParkingPlace::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO IN_BAGCART_PARKINGPLACE\
					 (PROCNAME,RELATETYPE,BAGCARTSPOT)\
					 VALUES ('%s',%d,'%s')"),
					 m_strBaggageParking,(int)m_emRelate,m_terminalSpot.GetIDString());
	return true;
}

bool BaggageParkingPlace::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE %s SET PROCNAME = '%s',RELATETYPE = %d,BAGCARTSPOT = '%s' WHERE ID = %d"),GetTableName(),
		m_strBaggageParking,(int)m_emRelate,m_terminalSpot.GetIDString(),m_nID);
	return true;
}

void BaggageParkingPlace::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM %s WHERE ID = %d"),GetTableName(),m_nID);
}

const BaggageParkingPlace& BaggageParkingPlace::operator=( const BaggageParkingPlace& parkingPlace )
{
	m_emRelate = parkingPlace.m_emRelate;
	m_strBaggageParking = parkingPlace.m_strBaggageParking;
	m_terminalSpot = parkingPlace.m_terminalSpot;
	return *this;
}

BaggageCartParkingLinkage::BaggageCartParkingLinkage( void )
{

}

BaggageCartParkingLinkage::~BaggageCartParkingLinkage( void )
{

}

void BaggageCartParkingLinkage::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM IN_BAGCART_PARKINGPLACE"));
}
