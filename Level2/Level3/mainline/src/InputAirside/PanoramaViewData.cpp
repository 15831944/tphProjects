#include "StdAfx.h"
#include "..\Database\DBElementCollection_Impl.h"
#include ".\panoramaviewdata.h"

#include "../Common/ARCUnit.h"

/************************************************************************/

/************************************************************************/
const double PanoramaViewData::DefaultAltitude =  ARCUnit::ConvertLength(100, ARCUnit::FEET,ARCUnit::CM);
const double PanoramaViewData::DefaultEndBearing = 360;
const double PanoramaViewData::DefaultStartBearing = 0;
const double PanoramaViewData::DefaultInclination = -45;
const double PanoramaViewData::DefaultTurnSpeed = 5; //deg/sec
const double PanoramaViewData::MaxInclination = 90;
const double PanoramaViewData::MinInclination = -90;

PanoramaViewData::PanoramaViewData(void)
{	
	m_dTurnSpeed = DefaultTurnSpeed;
	m_dStartBearing = DefaultStartBearing;
	m_dEndBearing = DefaultEndBearing;
	m_dInclination = DefaultInclination;
	m_dAltitude = DefaultAltitude;

	m_startTime = 0; 
	m_endTime = 0;
}

void PanoramaViewData::SetPos( const CPoint2008& pt )
{
	m_pos = pt;
}

void PanoramaViewData::SetTurnSpeed( const double& dTurnSpd )
{
	m_dTurnSpeed = dTurnSpd;
}

void PanoramaViewData::SetStartBearing( const double& dSbearing )
{
	m_dStartBearing = dSbearing;
}

void PanoramaViewData::SetEndBearing( const double& endBearing )
{
	m_dEndBearing = endBearing;
}

void PanoramaViewData::SetInclination( const double& dInc )
{
	m_dInclination = dInc;
}

void PanoramaViewData::SetAltitude( const double& dAlt )
{
	m_dAltitude = dAlt;
}

void PanoramaViewData::SetName( const CString& strName )
{
	m_strName = strName;
}

void PanoramaViewData::SaveData( int nAirportID )
{
	CString strSQL;
	if( GetID() < 0)
	{
		GetInsertSQL(nAirportID,strSQL);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}else
	{
		GetUpdateSQL(strSQL);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void PanoramaViewData::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("NAME"), m_strName);
	double dPosX,dPosY;
	recordset.GetFieldValue(_T("POSX"),dPosX );
	recordset.GetFieldValue(_T("POSY"),dPosY );
	m_pos.setX(dPosX); m_pos.setY(dPosY);
	recordset.GetFieldValue(_T("ALTITUDE"), m_dAltitude);
	recordset.GetFieldValue(_T("TURNSPEED"), m_dTurnSpeed);
	recordset.GetFieldValue(_T("STARTBEARING"),m_dStartBearing);
	recordset.GetFieldValue(_T("ENDBEARING"),m_dEndBearing);
	recordset.GetFieldValue(_T("INCLINATION"),m_dInclination);	
	recordset.GetFieldValue(_T("STARTTIME"), m_startTime);
	recordset.GetFieldValue(_T("ENDTIME"), m_endTime);
}


void PanoramaViewData::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE PanoramaViewData SET NAME = '%s', POSX= %f, POSY=%f, ALTITUDE = %f, TURNSPEED = %f,STARTBEARING =%f,ENDBEARING = %f,INCLINATION =%f , STARTTIME = %d, ENDTIME = %d  WHERE (ID=%d) "),
					m_strName, m_pos.getX(), m_pos.getY(), m_dAltitude, m_dTurnSpeed, m_dStartBearing, m_dEndBearing, m_dInclination, m_startTime, m_endTime , m_nID );
}

void PanoramaViewData::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO PanoramaViewData (NAME ,POSX ,POSY ,ALTITUDE ,TURNSPEED,STARTBEARING ,ENDBEARING,INCLINATION ,STARTTIME, ENDTIME, AIRPORTID) VALUES  ('%s',%f,%f,%f,%f,%f,%f,%f,%d,%d,%d)") ,
					 m_strName, m_pos.getX(),m_pos.getY(),m_dAltitude, m_dTurnSpeed, m_dStartBearing, m_dEndBearing, m_dInclination, m_startTime, m_endTime, nParentID );
}

void PanoramaViewData::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM PanoramaViewData \
					 WHERE (ID = %d)"),m_nID);
}

void PanoramaViewData::SetTimeRange( long startTime, long endTime )
{
	m_startTime = startTime;
	m_endTime = endTime;
}

double PanoramaViewData::GetMaxInclination() const
{
	return PanoramaViewData::MaxInclination;
}

double PanoramaViewData::GetMinInclination() const
{
	return PanoramaViewData::MinInclination;
}

/************************************************************************/

/************************************************************************/


PanoramaViewDataList::PanoramaViewDataList()
{
	m_nSelectItem = -1;
}


PanoramaViewData* PanoramaViewDataList::GetSelectItem() const
{
	if( m_nSelectItem < (int)GetElementCount() && m_nSelectItem >=0 )
		return (PanoramaViewData*)GetItem(m_nSelectItem);
	return NULL;
}



BOOL PanoramaViewDataList::SelectItem( int idx )
{
	if(idx < (int)GetElementCount()&& idx >=0 )
		m_nSelectItem = idx;
	else
		return FALSE;
	return TRUE;
}

BOOL PanoramaViewDataList::SelectItem( PanoramaViewData* theData )
{
	for(int i=0;i< (int)GetElementCount();++i)
	{
		if( GetItem(i) == theData )
		{
			m_nSelectItem = i;
			return TRUE;
		}
	}
	return FALSE;
}

PanoramaViewDataList::~PanoramaViewDataList()
{
}


PanoramaViewData* PanoramaViewDataList::CreateNewData()
{
	PanoramaViewData* newData = new PanoramaViewData();
	CString str = GetCreateName();
	newData->SetName(str);
	AddNewItem(newData);
	return newData;
}


CString PanoramaViewDataList::GetCreateName() const
{
	CString str;
	str.Format("View%d", GetElementCount()+1 );
	return str;
}

void PanoramaViewDataList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM PanoramaViewData WHERE AIRPORTID = %d"), nParentID );
}

void PanoramaViewDataList::DeleteItemFromIndex( size_t nIndex )
{
	DBElementCollection<PanoramaViewData>::DeleteItemFromIndex(nIndex);
	if( m_nSelectItem < (int) GetElementCount() )
	{

	}
	else
	{
		m_nSelectItem = (int)GetElementCount()-1;
	}
}
