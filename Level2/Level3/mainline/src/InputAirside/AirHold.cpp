#include "StdAfx.h"
#include ".\Airhold.h"

#include ".\AirWayPoint.h"
#include "ALTAirspace.h"
#include "AirsideImportExportManager.h"
#include "ALTObjectDisplayProp.h"

const static int NcirclePts = 10;

 
AirHold::AirHold(void)
{
	m_nWayPointID = -1;
	m_inBoundDegree = 0.0;
	m_outBoundLegLength = 50000.0;
	m_outBoundLegTime = 0.0;
	m_bRightTurn = FALSE;
	m_dMaxAlt = 40000.0;
	m_dMinAlt = 30000.0;
}

AirHold::~AirHold(void)
{
}

//AME_L1, NAME_L2, NAME_L3, NAME_L4, WAYPTID, INCOUS, OUTLEGLEN, \
//OUTLEGTIME, TURNTYPT, LOWALT, HIGHALT, TSTAMP
void AirHold::ReadObject(int nObjID)
{m_nObjID = nObjID;
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	

		ALTObject::ReadObject(adoRecordset);

		adoRecordset.GetFieldValue(_T("WAYPTID"),m_nWayPointID);
		adoRecordset.GetFieldValue(_T("INCOUS"),m_inBoundDegree);
		adoRecordset.GetFieldValue(_T("OUTLEGLEN"),m_outBoundLegLength);
		adoRecordset.GetFieldValue(_T("OUTLEGTIME"),m_outBoundLegTime);
		adoRecordset.GetFieldValue(_T("TURNTYPT"),m_bRightTurn);
		adoRecordset.GetFieldValue(_T("LOWALT"),m_dMinAlt);
		adoRecordset.GetFieldValue(_T("HIGHALT"),m_dMaxAlt);

	}

}
int AirHold::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_HOLD);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void AirHold::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}
void AirHold::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CString strUpdateScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}
CString AirHold::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, WAYPTID, INCOUS, OUTLEGLEN, \
		OUTLEGTIME, TURNTYPT, LOWALT, HIGHALT, TSTAMP\
		FROM HOLD_VIEW\
		WHERE ID = %d"),nObjID);

	return strSQL;
}
CString AirHold::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("INSERT INTO HOLDPROP\
		(OBJID, WAYPTID, INCOUS, OUTLEGLEN, OUTLEGTIME, TURNTYPT, LOWALT, \
		HIGHALT)\
		VALUES (%d,%d,%f,%f,%f,%d,%f,%f)"),nObjID,m_nWayPointID,m_inBoundDegree,
		m_outBoundLegLength,m_outBoundLegTime,m_bRightTurn?1:0,m_dMinAlt,
        m_dMaxAlt);

	return strSQL;
}
CString AirHold::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("UPDATE HOLDPROP\
		SET WAYPTID =%d, INCOUS =%f, OUTLEGLEN =%f, OUTLEGTIME =%f, TURNTYPT =%d, LOWALT =%f, \
		HIGHALT =%f\
		WHERE OBJID = %d"),m_nWayPointID,m_inBoundDegree,
		m_outBoundLegLength,m_outBoundLegTime,m_bRightTurn?1:0,m_dMinAlt,
		m_dMaxAlt,nObjID);

	return strSQL;
}
CString AirHold::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("DELETE FROM HOLDPROP\
		WHERE OBJID = %d"),nObjID);

	return strSQL;
}

Path AirHold::GetPath( const Point& fixPt, double airportVariation ) const
{
	double dAngle = getInBoundDegree()/SCALE_FACTOR + airportVariation;
	double radius = 200 * SCALE_FACTOR;

	Path reslt;
	Point p1,p2;
	Point vRotate;
	double mRotateAngle;
	const static int NcirclePts = 10; 

	if( isRightTurn() )
	{
		vRotate = Point(-1,0,0)*radius;
		p1 =  Point(0,0,0)-vRotate ;
		p2 = p1 + Point(0,-1,0) * getOutBoundLegNm();
		mRotateAngle = 180/(NcirclePts-1);
	}
	else
	{
		vRotate = Point(1,0,0)*radius;
		p1 = Point(0,0,0)-vRotate;
		p2 = p1 + Point(0,-1,0) * getOutBoundLegNm();
		mRotateAngle = - 180/(NcirclePts-1);
	}

	//caculate R
	reslt.init(2*NcirclePts);
	for(int i=0;i<NcirclePts;i++){
		reslt[i] = p1 + vRotate;		
		reslt[i+NcirclePts] = p2 - vRotate;
		vRotate.rotate( mRotateAngle );
	}

	reslt.Rotate(dAngle);
	reslt.DoOffset(fixPt.getX(),fixPt.getY(),fixPt.getZ() );

	return reslt;
}


ALTObject * AirHold::NewCopy()
{
	AirHold * reslt = new AirHold;
	reslt->CopyData(this);
	return reslt;
}

void AirHold::ExportAirHolds(CAirsideExportFile& exportFile)
{
	ALTObjectUIDList vAirHold;
	ALTAirspace::GetHoldList(exportFile.GetAirspaceID(),vAirHold);

	for (ALTObjectUIDList::size_type i =0; i < vAirHold.size(); ++i)
	{
		AirHold airHoldObj;
		airHoldObj.ReadObject(vAirHold[i]);
		airHoldObj.ExportAirHold(exportFile);
	}

}
void AirHold::ExportAirHold(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);

	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeInt(m_nWayPointID);
	exportFile.getFile().writeDouble(m_inBoundDegree);
	exportFile.getFile().writeDouble(m_outBoundLegLength);
	exportFile.getFile().writeDouble(m_outBoundLegTime);
	exportFile.getFile().writeInt(m_bRightTurn?1:0);
	exportFile.getFile().writeDouble(m_dMaxAlt);
	exportFile.getFile().writeDouble(m_dMinAlt);
	exportFile.getFile().writeLine();

	AirHoldDisplayProp dspProp;
	dspProp.ReadData(m_nObjID);
	dspProp.ExportDspProp(exportFile);
}
//int m_nWayPointID;
//double m_inBoundDegree;
//double m_outBoundLegLength;
//double m_outBoundLegTime;
//bool m_bRightTurn;
//double m_dMaxAlt;
//double m_dMinAlt;
void AirHold::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirspaceID = -1;
	importFile.getFile().getInteger(nOldAirspaceID);
	m_nAptID = importFile.getNewAirspaceID();
	
	//way point
	int nOldWaypointID = -1;
	importFile.getFile().getInteger(nOldWaypointID);
	
	m_nWayPointID = importFile.GetObjectNewID(nOldWaypointID);

	importFile.getFile().getFloat(m_inBoundDegree);
	importFile.getFile().getFloat(m_outBoundLegLength);
	importFile.getFile().getFloat(m_outBoundLegTime);
	int nRightTurn = 0;
	importFile.getFile().getInteger(nRightTurn);
	m_bRightTurn = (nRightTurn == 0)?false:true;
	importFile.getFile().getFloat(m_dMaxAlt);
	importFile.getFile().getFloat(m_dMinAlt);
	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);

	AirHoldDisplayProp dspProp;
	dspProp.ImportDspProp(importFile,m_nObjID);
}

double AirHold::getMaxAltitude() const
{
	return m_dMaxAlt; 
}

double AirHold::getMinAltitude() const
{
	return m_dMinAlt;
}

int AirHold::GetWatpoint()
{
	return m_nWayPointID;
}

bool AirHold::CopyData( const ALTObject* pObj )
{
	if(this == pObj)
		return true;

	if(pObj->GetType() == GetType() )
	{
		AirHold * pHold = (AirHold*)pObj;
		 m_nWayPointID = pHold->m_nWayPointID;
		 m_inBoundDegree = pHold->m_inBoundDegree;
		 m_outBoundLegLength = pHold->m_outBoundLegLength;
		 m_outBoundLegTime = pHold->m_outBoundLegTime;
		 m_bRightTurn = pHold->m_bRightTurn;
		 m_dMaxAlt= pHold->m_dMaxAlt;
		 m_dMinAlt = pHold->m_dMinAlt;	
	}
	return __super::CopyData(pObj);
}

const GUID& AirHold::getTypeGUID()
{
	// {5C201582-2BF3-4c4a-BAF9-38785E925C2A}
	static const GUID class_guid = 
	{ 0x5c201582, 0x2bf3, 0x4c4a, { 0xba, 0xf9, 0x38, 0x78, 0x5e, 0x92, 0x5c, 0x2a } };
	
	return class_guid;

}

ALTObjectDisplayProp* AirHold::NewDisplayProp()
{
	return new AirHoldDisplayProp();
}
