#include "StdAfx.h"
#include "VehiclePoolParking.h"
#include "../common/ARCUnit.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "ALTObjectDisplayProp.h"
#include "Database/LandParkingLotDBDef.h"
#include "Common/ARCStringConvert.h"
#include "Common/SqlScriptMaker.h"

VehiclePoolParking::VehiclePoolParking(void)
{
	m_nPathID  = -1;
}
VehiclePoolParking::~VehiclePoolParking(void)
{
	m_path.clear();
}

void VehiclePoolParking::ReadObject(int nObjID)
{
	m_nObjID = nObjID;

	CADORecordset adoRecordset;
	long nRecordCount = -1;

	CADODatabase::ExecuteSQLStatement(ALTObject::GetSelectScript(nObjID),nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
		ALTObject::ReadObject(adoRecordset);

	//area path
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{	
		adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID );
		m_path.clear();
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
	}

	//parking space
	nRecordCount = -1;
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM VEHICLEPARKINGSPACE WHERE parentid = %d"),nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	m_parkspaces.m_spaces.clear();
	while (!adoRecordset.IsEOF())
	{
		ParkingSpace space;
		CString strPath;
		adoRecordset.GetFieldValue(PARKSPACE_PATH, strPath);
		ARCStringConvert::parsePathFromDBString(space.m_ctrlPath,strPath);

		adoRecordset.GetFieldValue(PARKSPACE_TYPE,(int&)space.m_type);
		adoRecordset.GetFieldValue(PARKSPACE_WIDTH,space.m_dWidth);
		adoRecordset.GetFieldValue(PARKSPACE_LENGTH,space.m_dLength);
		adoRecordset.GetFieldValue(PARKSPACE_ANGLE,space.m_dAngle);
		adoRecordset.GetFieldValue(PARKSPACE_OPTYPE,(int&)space.m_opType);

		m_parkspaces.m_spaces.push_back(space);
		adoRecordset.MoveNextData();
	}

	//parking drivepipe
	nRecordCount = -1;
	strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM VEHICLEDRIVEPIPE WHERE parentid = %d"),nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	m_dirvepipes.m_pipes.clear();
	while (!adoRecordset.IsEOF())
	{
		ParkingDrivePipe pipe;
		CString strPipe;
		adoRecordset.GetFieldValue(PARKING_DRIVEPIPE_PATH, strPipe);
		ARCStringConvert::parsePathFromDBString(pipe.m_ctrlPath,strPipe);


		adoRecordset.GetFieldValue(PARKING_DRIVEPIPE_WIDTH,pipe.m_width);		
		adoRecordset.GetFieldValue(PARKING_DRIVEPIPE_LANENUM,pipe.m_nLaneNum);
		adoRecordset.GetFieldValue(PARKING_DRIVEPIPE_DIRECTION,(int&)pipe.m_nType);
		
		m_dirvepipes.m_pipes.push_back(pipe);
		adoRecordset.MoveNextData();

	}	

	//m_pEntryList->ReadData(getID());
	//m_pExitList->ReadData(getID());

	//m_pParkingPlaceList = new ParkingPlaceList(nObjID);
	//m_pParkingPlaceList->ReadParkingPlaceList();

	//m_pDriveRoadList = new DriveRoadList(nObjID);
	//m_pDriveRoadList->ReadDriveRoadList();

}
int VehiclePoolParking::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_VEHICLEPOOLPARKING);
	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);
	CString strSQL = GetInsertScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	m_nObjID = nObjID;

	//parking space
	strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM VEHICLEPARKINGSPACE WHERE parentid = %d"),nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);
	for(size_t i=0;i<m_parkspaces.m_spaces.size();i++)
	{
		const ParkingSpace& space = m_parkspaces.m_spaces[i];
		SqlInsertScriptMaker script("VEHICLEPARKINGSPACE");
		script.AddColumn(PARKSPACE_PARENTID,nObjID);
		script.AddColumn(PARKSPACE_PATH,ARCStringConvert::toDBString(space.m_ctrlPath));
		script.AddColumn(PARKSPACE_TYPE,space.m_type);
		script.AddColumn(PARKSPACE_WIDTH,space.m_dWidth);
		script.AddColumn(PARKSPACE_LENGTH,space.m_dLength);
		script.AddColumn(PARKSPACE_ANGLE,space.m_dAngle);
		script.AddColumn(PARKSPACE_OPTYPE,space.m_opType);

		CADODatabase::ExecuteSQLStatement(script.GetScript());
	}

	//drive pipe
	strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM VEHICLEDRIVEPIPE WHERE parentid = %d"),nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);
	for(size_t i=0;i<m_dirvepipes.m_pipes.size();i++)
	{
		const ParkingDrivePipe& pipe = m_dirvepipes.m_pipes[i];
		SqlInsertScriptMaker script("VEHICLEDRIVEPIPE");
		script.AddColumn(PARKING_DRIVEPIPE_PARENTID,nObjID);
		script.AddColumn(PARKING_DRIVEPIPE_PATH,ARCStringConvert::toDBString(pipe.m_ctrlPath));
		script.AddColumn(PARKING_DRIVEPIPE_WIDTH,pipe.m_width);
		script.AddColumn(PARKING_DRIVEPIPE_LANENUM, pipe.m_nLaneNum);
		script.AddColumn(PARKING_DRIVEPIPE_DIRECTION,pipe.m_nType);

		CADODatabase::ExecuteSQLStatement(script.GetScript());
	}
	//m_parkspaces.SaveData(getID());
	//m_pEntryList->SaveData(getID());
	//m_pExitList->SaveData(getID());

	//m_dirvepipes.SaveData(getID());
	//m_pParkingPlaceList->SaveParkingPlaceList(nAirportID);
	//m_pDriveRoadList->SaveDriveRoadList(nAirportID);

	return nObjID;
}
void VehiclePoolParking::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);

	//parking space
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM VEHICLEPARKINGSPACE WHERE parentid = %d"),nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);
	for(size_t i=0;i<m_parkspaces.m_spaces.size();i++)
	{
		const ParkingSpace& space = m_parkspaces.m_spaces[i];
		SqlInsertScriptMaker script("VEHICLEPARKINGSPACE");
		script.AddColumn(PARKSPACE_PARENTID,nObjID);
		script.AddColumn(PARKSPACE_PATH,ARCStringConvert::toDBString(space.m_ctrlPath));
		script.AddColumn(PARKSPACE_TYPE,space.m_type);
		script.AddColumn(PARKSPACE_WIDTH,space.m_dWidth);
		script.AddColumn(PARKSPACE_LENGTH,space.m_dLength);
		script.AddColumn(PARKSPACE_ANGLE,space.m_dAngle);
		script.AddColumn(PARKSPACE_OPTYPE,space.m_opType);
		
		CADODatabase::ExecuteSQLStatement(script.GetScript());
	}

	//drive pipe
	strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM VEHICLEDRIVEPIPE WHERE parentid = %d"),nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);
	for(size_t i=0;i<m_dirvepipes.m_pipes.size();i++)
	{
		const ParkingDrivePipe& pipe = m_dirvepipes.m_pipes[i];
		SqlInsertScriptMaker script("VEHICLEDRIVEPIPE");
		script.AddColumn(PARKING_DRIVEPIPE_PARENTID,nObjID);
		script.AddColumn(PARKING_DRIVEPIPE_PATH,ARCStringConvert::toDBString(pipe.m_ctrlPath));
		script.AddColumn(PARKING_DRIVEPIPE_WIDTH,pipe.m_width);
		script.AddColumn(PARKING_DRIVEPIPE_LANENUM, pipe.m_nLaneNum);
		script.AddColumn(PARKING_DRIVEPIPE_DIRECTION,pipe.m_nType);

		CADODatabase::ExecuteSQLStatement(script.GetScript());
	}
}

void VehiclePoolParking::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);

	//parking space
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM VEHICLEPARKINGSPACE WHERE parentid = %d"),nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	//drive pipe
	strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM VEHICLEDRIVEPIPE WHERE parentid = %d"),nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

CString VehiclePoolParking::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO VEHICLEPOOLPARKING (OBJID,PATHID ) \
					 VALUES (%d,%d)"), \
					 nObjID,m_nPathID);

	return strSQL;
}
CString VehiclePoolParking::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE VEHICLEPOOLPARKING SET PATHID = %d \
					 WHERE OBJID = %d"),m_nPathID,nObjID);

	return strSQL;
}
CString VehiclePoolParking::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM VEHICLEPOOLPARKING WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString VehiclePoolParking::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM VEHICLEPOOLPARKING WHERE OBJID = %d"),
		nObjID);

	return strSQL;
}
const CPath2008& VehiclePoolParking::GetPath()const
{
	return (m_path);
}


void VehiclePoolParking::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}

void VehiclePoolParking::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeInt(m_nPathID);

	//path
	int nPathCount = m_path.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i = 0; i < nPathCount; i++)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}

	exportFile.getFile().writeLine();
}

void VehiclePoolParking::ExportVehiclePoolParking(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vVehiclePoolParkingID;
	ALTAirport::GetVehiclePoolParkingIDs(nAirportID,vVehiclePoolParkingID);
	int nVehiclePoolParkingCount = static_cast<int>(vVehiclePoolParkingID.size());
	for (int i =0; i < nVehiclePoolParkingCount;++i)
	{
		VehiclePoolParking vehiclePoolParkingObj;
		vehiclePoolParkingObj.ReadObject(vVehiclePoolParkingID[i]);
		vehiclePoolParkingObj.ExportObject(exportFile);
	}
}

void VehiclePoolParking::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);

	importFile.getFile().getInteger(m_nPathID);

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();
	for (int i = 0; i < nPointCount; i++)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);
}

ALTObject * VehiclePoolParking::NewCopy()
{
	VehiclePoolParking * pVehiclePoolParking  = new VehiclePoolParking;
	pVehiclePoolParking->CopyData(this);
	return pVehiclePoolParking;
}

const GUID& VehiclePoolParking::getTypeGUID()
{
	// {CABC51DA-34AD-4197-A75F-CD1F73C97328}
	static const GUID name = 
	{ 0xcabc51da, 0x34ad, 0x4197, { 0xa7, 0x5f, 0xcd, 0x1f, 0x73, 0xc9, 0x73, 0x28 } };
	
	return name;
}

ALTObjectDisplayProp* VehiclePoolParking::NewDisplayProp()
{
	return new VehiclePoolParkingDisplayProp();
}

bool VehiclePoolParking::CopyData( const ALTObject* pObj )
{
	if(!pObj)
		return false;
	if(this == pObj)
		return true;


	if( GetType() == pObj->GetType() )
	{
		VehiclePoolParking *pOther = (VehiclePoolParking*)pObj;
		m_path = pOther->m_path;
		m_parkspaces = pOther->m_parkspaces;
		m_dirvepipes = pOther->m_dirvepipes;
	}

	return __super::CopyData(pObj);


}


