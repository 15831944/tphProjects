#include "StdAfx.h"
#include ".\circlestretch.h"
#include "../common/ARCUnit.h"


CircleStretch::CircleStretch(void)
{
	m_dLaneWidth = 400;
	m_iLaneNum = 1;
	m_nPathID  = -1;
}

CircleStretch::CircleStretch( const CircleStretch& other)
{
	*this = other;
}

CircleStretch& CircleStretch::operator=(const CircleStretch& other)
{
	ALTObject::operator =(other);
	m_iLaneNum = other.m_iLaneNum;
	m_dLaneWidth = other.m_dLaneWidth;
	m_path = other.m_path;
	return *this;
}
CircleStretch::~CircleStretch(void)
{
	m_path.clear();
}

const CPath2008& CircleStretch::GetPath(void)const
{
	return m_path;
}

void CircleStretch::SetPath(const CPath2008& _path)
{

    m_path.clear();
	m_path = _path;
}

CPath2008& CircleStretch::getPath(void)
{
	return m_path;
}

double CircleStretch::GetLaneWidth(void)const
{
	return m_dLaneWidth;
}

void CircleStretch::SetLaneWidth(double dLaneWidth)
{
	m_dLaneWidth = dLaneWidth;
}

int CircleStretch::GetLaneNumber(void)const
{
	return m_iLaneNum;
}

void CircleStretch::SetLaneNumber(int iLaneNumber)
{
	m_iLaneNum = iLaneNumber;
}

ALTObject * CircleStretch::NewCopy()
{
	CircleStretch * pCircleStretch  = new CircleStretch;
	*pCircleStretch = *this;
	return pCircleStretch;
}

bool CircleStretch::CopyData(const  ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if( pObj->GetType() == GetType() )
	{
		CircleStretch * pOtherCircleStretch = (CircleStretch*)pObj;
		m_path = pOtherCircleStretch->GetPath();
		m_dLaneWidth = pOtherCircleStretch->GetLaneWidth();
		m_iLaneNum = pOtherCircleStretch->GetLaneNumber();

		m_bLocked = pOtherCircleStretch->GetLocked();
		return true;
	}
	return false;
}


void CircleStretch::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}

CString CircleStretch::GetSelectScript(int nObjID)const
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM CIRCLESTRETCH WHERE OBJID = %d"),nObjID);
	return strSQL;
}

CString CircleStretch::GetDeleteScript(int nObjID)const
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM CIRCLESTRETCH WHERE OBJID = %d"),nObjID);
	return strSQL;
}

CString CircleStretch::GetInsertScript(int nObjID)const
{
	CString strSQL;
	strSQL.Format(_T("INSERT INTO CIRCLESTRETCH (OBJID,LANEWIDTH , LANENUM, PATHID ) \
					 VALUES (%d,%f,%d,%d)"),nObjID,ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM),\
					 m_iLaneNum,m_nPathID);
	return strSQL;
}

CString CircleStretch::GetUpdateScript(int nObjID)const
{
	CString strSQL;
	strSQL.Format(_T("UPDATE CIRCLESTRETCH SET LANEWIDTH = %f, LANENUM = %d, PATHID = %d \
					 WHERE OBJID = %d"),ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM),m_iLaneNum,m_nPathID,nObjID);
	return strSQL;
}

void CircleStretch::ReadObject(int nObjID)
{
	m_nObjID = nObjID;
	CADORecordset adoRecordset;
	long nRecordCound = -1;
	CADODatabase::ExecuteSQLStatement(ALTObject::GetSelectScript(nObjID),nRecordCound,adoRecordset);
	if (!adoRecordset.IsEOF())
		ALTObject::ReadObject(adoRecordset);

	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCound,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID);

		m_path.clear();
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);

		adoRecordset.GetFieldValue(_T("LANEWIDTH"),m_dLaneWidth);
		m_dLaneWidth = ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::CM,ARCUnit::M);

		adoRecordset.GetFieldValue(_T("LANENUM"),m_iLaneNum); 
	}
}

int CircleStretch::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_CIRCLESTRETCH);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}

void CircleStretch::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);

	CString strSQL = GetUpdateScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CircleStretch::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);

	CString strSQL = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

const GUID& CircleStretch::getTypeGUID()
{
	// {87A64FAF-E285-443d-952B-C5308F5D9037}
	static const GUID name = 
	{ 0x87a64faf, 0xe285, 0x443d, { 0x95, 0x2b, 0xc5, 0x30, 0x8f, 0x5d, 0x90, 0x37 } };
	
	return name;
}







