
#include "StdAfx.h"
#include "ALTObject.h"
#include "Runway.h"
#include "Heliport.h"
#include "Taxiway.h"
#include "AirWayPoint.h"
#include "Surface.h"
#include "AirSector.h"
#include "Surface.h"
#include "Structure.h"
#include "Airhold.h"
#include "stand.h"
#include "DeicePad.h"
#include "AirRoute.h"
#include "AirsideImportExportManager.h"
#include "ARCCharManager.h"
#include "GroundRoute.h"
#include "Stretch.h"
#include "Intersections.h"
#include "VehiclePoolParking.h"
#include "TrafficLight.h"
#include "TolLGate.h"
#include "StopSign.h"
#include "YieldSign.h"
#include "contour.h"
#include "CircleStretch.h"
#include "ParkingPlace.h"
#include "DriveRoad.h"
#include "ReportingArea.h"
#include "StartPosition.h"
#include "MeetingPoint.h"
#include "ALTObjectDisplayProp.h"
#include "AirsidePaxBusParkSpot.h"
#include "AirsideBagCartParkSpot.h"


ALTObject::ALTObject() 
:m_nObjID(-1)
,m_nAptID(-1)
//,m_bLocked(false)
//,m_guid(CGuid::Create())
,m_pDispProp(NULL)
{

}

ALTObject::~ALTObject()
{
	delete m_pDispProp;
	m_pDispProp = NULL;
}


bool ALTObject::IsObjectExists(int nObjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID	 FROM ALTOBJECT	 WHERE (ID = %d)"),nObjID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		return true;
	}

	return false;
}
void ALTObject::setID(int nObjID)
{
	m_nObjID = nObjID;
}
CString ALTObject::GetObjNameString() const
{
	CString strName = _T("");
	//level1
	if(m_str4Name.at(0).length() == 0)
		return strName;

	strName += CString(m_str4Name.at(0).c_str());

	for (int i =1; i < OBJECT_STRING_LEVEL; ++i)
	{
		if (m_str4Name.at(i).length() == 0)
			break;

		strName +=_T("-");
		strName +=m_str4Name.at(i).c_str();
	}

	return strName;
}
//nLevel 0,1,2,3
CString ALTObject::GetObjNameString( int nLevel ) const
{
	ASSERT(nLevel >= 0 && nLevel < OBJECT_STRING_LEVEL);

	//if nLevel is not valid, using OBJECT_STRING_LEVEL
	if(nLevel < 0 || nLevel >= OBJECT_STRING_LEVEL)
		nLevel = OBJECT_STRING_LEVEL;


	CString strName = _T("");
	//level1
	if(m_str4Name.at(0).length() == 0)
		return strName;

	strName += CString(m_str4Name.at(0).c_str());

	for (int i =1; i <= nLevel; ++i)
	{
		if (m_str4Name.at(i).length() == 0)
			break;

		//if(i == nLevel)
		//	break;

		strName +=_T("-");
		strName +=m_str4Name.at(i).c_str();
	}

	return strName;
}

void ALTObject::ReadObject(CADORecordset& adoRecordset)
{
	CString strName1 = _T("");
	CString strName2 = _T("");
	CString strName3 = _T("");
	CString strName4 = _T("");

// 	adoRecordset.GetFieldValue(_T("ID"),m_nObjID);
	
	adoRecordset.GetFieldValue(_T("APTID"),m_nAptID);

	adoRecordset.GetFieldValue(_T("NAME_L1"),strName1);
	m_str4Name.at(0) = strName1;

	adoRecordset.GetFieldValue(_T("NAME_L2"),strName2);
	m_str4Name.at(1) = strName2;

	adoRecordset.GetFieldValue(_T("NAME_L3"),strName3);
	m_str4Name.at(2) = strName3;	

	adoRecordset.GetFieldValue(_T("NAME_L4"),strName4);
	m_str4Name.at(3) = strName4;

	int nLocked = 1;
	adoRecordset.GetFieldValue(_T("LOCKED"), nLocked);
	m_bLocked = (0 != nLocked);
}

void ALTObject::ReadObject( int nObjID ) // only read out the name string
{
	m_nObjID = nObjID;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);
	if(!adoRecordset.IsEOF())
	{
		ReadObject(adoRecordset);
	}
}
CString ALTObject::GetSelectScript(int nObjID) const
{
	//m_nObjID = nObjID;

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT APTID, TYPE, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED\
		FROM ALTOBJECT\
		WHERE (ID = %d)"),nObjID);
	return strSQL;
}
int ALTObject::SaveObject(int AirportID,ALTOBJECT_TYPE objType)
{
	int nObjID = -1;

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO ALTOBJECT \
					 (APTID, TYPE, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED) \
					 VALUES (%d,%d,'%s','%s','%s','%s', %d)"),AirportID, (int)objType,
					 m_str4Name.at(0).c_str(),m_str4Name.at(1).c_str(),m_str4Name.at(2).c_str(),m_str4Name.at(3).c_str(),
					 m_bLocked ? 1 : 0);


	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL);


	CADORecordset idRecordset;	
	CString strSQLID = _T("SELECT @@Identity AS id");
	CADODatabase::ExecuteSQLStatement(strSQLID,nRecordCount,idRecordset);

	//CADORecordset idRecordset;	
	//CADODatabase::ExecuteSQLStatement(strSQL + SCOPE_IDENTITY_SQLSCRIPT,nRecordCount,adoRecordset);
	//adoRecordset.NextRecordset(idRecordset);

	idRecordset.GetFieldValue(_T("ID"),nObjID);

	m_nObjID = nObjID;
	return nObjID;
}

int ALTObject::SaveObject( int AirportID )
{
	return -1;
}
void ALTObject::UpdateObject(int nObjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE ALTOBJECT \
					 SET NAME_L1 = '%s', NAME_L2 = '%s', NAME_L3 = '%s', NAME_L4 = '%s', LOCKED = %d \
					 WHERE ID = %d"),
					 m_str4Name.at(0).c_str(),m_str4Name.at(1).c_str(),m_str4Name.at(2).c_str(),m_str4Name.at(3).c_str(),
					 m_bLocked ? 1 : 0, nObjID);


	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ALTObject::DeleteObject(int nObjID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ALTOBJECT WHERE ID = %d"),nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}


ALTObject* ALTObject::CreateObjectByType(const ALTOBJECT_TYPE& objType)
{
            
	ALTObject *pObj = NULL;

	switch(objType)
	{
	case ALT_WAYPOINT:
		pObj = new AirWayPoint;	
		break;
	case ALT_HELIPORT:
		pObj = new Heliport;
		break;
	case ALT_TAXIWAY:
		pObj = new Taxiway;
		break;
	case ALT_DEICEBAY:
		pObj = new DeicePad;
		break;
	case ALT_RUNWAY:
		pObj = new Runway;
		break;
	case ALT_SECTOR:
		pObj = new AirSector;
		break;
	case ALT_SURFACE:
		pObj = new Surface;
		break;
	case ALT_STAND:
		pObj = new Stand;
		break;

	case ALT_HOLD:
		pObj = new AirHold;
		break;

	case ALT_STRUCTURE:
		pObj = new Structure;
		break;
	case ALT_AIRROUTE:
		pObj = new CAirRoute();
		break;

	case ALT_GROUNDROUTE:
		pObj = new GroundRoute(-1);
		break;
	
	case ALT_STRETCH:
		pObj = new Stretch();
		break;

	case ALT_INTERSECTIONS:
		pObj = new Intersections();
		break;

	case ALT_VEHICLEPOOLPARKING:
		pObj = new VehiclePoolParking();
		break;
	case ALT_TRAFFICLIGHT:
		pObj = new TrafficLight();
		break;
	case ALT_TOLLGATE:
		pObj = new TollGate();
		break;
	case ALT_STOPSIGN:
		pObj = new StopSign();
		break;
	case ALT_YIELDSIGN:
		pObj = new YieldSign();
		break;
	case ALT_CONTOUR:
		pObj = new Contour();
		break;
	case ALT_CIRCLESTRETCH:
		pObj = new CircleStretch();
		break;
	case ALT_PARKINGPLACE:
		pObj = new ParkingPlace(-1);
		break;
	case ALT_DRIVEROAD:
		pObj = new DriveRoad(-1);
		break;
	case ALT_REPORTINGAREA:
		pObj = new CReportingArea();
		break;
	case ALT_STARTPOSITION:
		pObj = new CStartPosition();
		break;
	case ALT_MEETINGPOINT:
		pObj = new CMeetingPoint();
		break;
	case ALT_APAXBUSSPOT:
		pObj = new AirsidePaxBusParkSpot();
		break;
	case ALT_ABAGCARTSPOT:
		pObj = new AirsideBagCartParkSpot();
		break;
	default:
		return NULL;
		break;
	}

	return pObj;

}




ALTObject* ALTObject::ReadObjectByID(int nObjID)
{

	CString strSelectScript = _T("");

	strSelectScript.Format(_T("SELECT TYPE	FROM ALTOBJECT	WHERE ID = %d"),nObjID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectScript,nRecordCount,adoRecordset);
	int nObjType = ALT_UNKNOWN;
	if(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("TYPE"),nObjType);
	}
	ALTObject* obj = ALTObject::CreateObjectByType((ALTOBJECT_TYPE)nObjType);

	if (NULL == obj)
	{
		return NULL;
	}

	obj->ReadObject(nObjID);
	return obj;
}

void ALTObject::GetObjectList(ALTOBJECT_TYPE objType,int nAirportID,std::vector<ALTObject>& vObject)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED\
		FROM ALTOBJECT\
		WHERE TYPE = %d AND APTID = %d\
		ORDER BY NAME_L1, NAME_L2, NAME_L3, NAME_L4"),
		(int)objType,nAirportID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	

	while (!adoRecordset.IsEOF())
	{
		ALTObject altObject;
		altObject.ReadObject(adoRecordset);

		int nObjID = -1;
		adoRecordset.GetFieldValue(_T("ID"), nObjID);
		altObject.setID(nObjID);

		vObject.push_back(altObject);

		adoRecordset.MoveNextData();
	}

}

bool ALTObject::CheckObjNameIsValid(const ALTObjectID& objName,int nAirportID, int nObjID)
{
	{

		if(strlen(objName.at(0).c_str()) == 0)
			return false;


		for (int nLevel =1; nLevel < OBJECT_STRING_LEVEL; ++ nLevel)
		{
			if (strlen(objName.at(nLevel).c_str()) == 0)
			{
				for (int nLeft = nLevel+1; nLeft < OBJECT_STRING_LEVEL; ++nLeft)
				{
					if (strlen(objName.at(nLeft).c_str()) >0)
					{
						return false;
					}
				}
			}
		}
	}


	CString strSQL = _T("");
	CString strSubSQL = _T("");

	strSQL.Format(_T("SELECT ID\
					 FROM ALTOBJECT\
					 WHERE (APTID = %d) AND (NAME_L1 = '%s') "),nAirportID,objName.at(0).c_str());

	//nane2
	if (strlen(objName.at(1).c_str()) !=0)
	{
		strSubSQL.Format(_T("  AND (NAME_L2 = '%s')"),objName.at(1).c_str());
		strSQL += strSubSQL;
		//name 3
		if (strlen(objName.at(2).c_str()) !=0)
		{
			strSubSQL.Empty();
			strSubSQL.Format(_T("  AND (NAME_L3 = '%s')"),objName.at(2).c_str());
			strSQL += strSubSQL;
			//name 4
			if (strlen(objName.at(3).c_str()) !=0)
			{	
				strSubSQL.Empty();
				strSubSQL.Format(_T("  AND (NAME_L4 = '%s')"),objName.at(3).c_str());
				strSQL += strSubSQL;
			}
		}

	}

	if (nObjID != -1)
	{
		strSubSQL.Empty();
		strSubSQL.Format(_T("  AND (ID <> %d)"),nObjID );
		strSQL += strSubSQL;
	}

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		return false;
	}

	return true;
}
bool ALTObject::DeleteObjectByID(int nObjID)
{
	
	ALTObject* obj = NULL;
	try
	{

		CString strSelectScript = _T("");

		strSelectScript.Format(_T("SELECT TYPE FROM ALTOBJECT WHERE ID = %d"),nObjID);

		long nRecordCount = -1;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSelectScript,nRecordCount,adoRecordset);
		int nObjType = 0;
		if(!adoRecordset.IsEOF())
		{
			adoRecordset.GetFieldValue(_T("TYPE"),nObjType);
		}
		obj = ALTObject::CreateObjectByType((ALTOBJECT_TYPE)nObjType);
		if(obj)
		{
			obj->ReadObject(nObjID);			
		}	
		
		CADODatabase::BeginTransaction();
		if(obj)
		{
			obj->DeleteObject(nObjID);
			delete obj;
		}
		CADODatabase::CommitTransaction();
	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		delete obj;
		CString strError = e.ErrorMessage();
		return false;
	}

	return true;

}

bool ALTObject::IsAirportElement() const
{
	if(    GetType() == ALT_RUNWAY
		|| GetType() == ALT_HELIPORT
		|| GetType() == ALT_TAXIWAY
		|| GetType() == ALT_STAND
		|| GetType() == ALT_DEICEBAY
		|| GetType() == ALT_STRETCH
		|| GetType() == ALT_INTERSECTIONS
		|| GetType() == ALT_LANEADAPTER
		||GetType() == ALT_VEHICLEPOOLPARKING
		|| GetType() == ALT_TRAFFICLIGHT
		|| GetType() == ALT_TOLLGATE
		|| GetType() == ALT_STOPSIGN
		|| GetType() == ALT_YIELDSIGN
		||GetType() == ALT_CIRCLESTRETCH
		||GetType() == ALT_STARTPOSITION
		|| GetType() == ALT_MEETINGPOINT
		|| GetType() == ALT_ROUNDABOUT
		)
		return true;

	return false;
}

bool ALTObject::IsAirspaceElement() const
{
	if(GetType() == ALT_SECTOR || GetType() == ALT_HOLD || GetType() == ALT_AIRROUTE || GetType() == ALT_WAYPOINT )
		return true ;
	return false;
}

bool ALTObject::IsTopographyElement() const
{
	if(GetType() == ALT_STRUCTURE || GetType() == ALT_SURFACE || GetType() == ALT_STRUCTURE || GetType() == ALT_CONTOUR || GetType() == ALT_REPORTINGAREA )
		return true ;
	return false;
}

void ALTObject::GetNextObjectName( ALTObjectID& nameDest )
{
	nameDest = m_str4Name;
	int lastLevel = 0;
	for(int i =0 ;i<(int) OBJECT_STRING_LEVEL;i++)
	{
		if( nameDest.m_val[i].empty() )
		{ 
			lastLevel = i - 1;
			break; 
		}else if (i == (int) OBJECT_STRING_LEVEL-1 )
		{
			lastLevel = i ;
		}
	}

	/*int lastNameSize = (int)nameDest.m_val[lastLevel].size();
	
	do
	{	
		nameDest.m_val[lastLevel][lastNameSize -1] = nameDest.m_val[lastLevel][lastNameSize -1] ++;
	}
	while( !CheckObjNameIsValid(nameDest,m_nAptID,m_nObjID) );*/

	/////
	char buf[128] = {0};
	strcpy(buf,nameDest.m_val[lastLevel].c_str());

	//check if last level of ID is numeric or alphanumeric
	char c = toupper(buf[0]);
	int len = (int)strlen(buf);
	if(isAllNumeric(buf)) {
		//numeric
		int i = atoi(buf);
		do {
			i++;
			_itoa(i,buf,10);
			nameDest.m_val[lastLevel] = buf;
		} while(!CheckObjNameIsValid(nameDest,m_nAptID,m_nObjID));
	}
	else if(len <= 2 && c >= 'A' && c <= 'Z') {
		char d = toupper(buf[len - 1]);
		if( d >= 'A' && d <= 'Z')
		{
			do {
				NextAlphaNumeric(buf);
				nameDest.m_val[lastLevel] = buf;
			} while(!CheckObjNameIsValid(nameDest,m_nAptID,m_nObjID));
		}
		else
		{
			do {
				strcat(buf, "_COPY");
				nameDest.m_val[lastLevel] = buf;
			} while(!CheckObjNameIsValid(nameDest,m_nAptID,m_nObjID));
		}
	}
	else {
		//other
		do {
			strcat(buf, "_COPY");
			nameDest.m_val[lastLevel] = buf;
		} while(!CheckObjNameIsValid(nameDest,m_nAptID,m_nObjID));
	}
	/////
}

ALTObject * ALTObject::NewCopy()
{
	ASSERT(false);
	return NULL;
}

ALTObjectID ALTObject::GetObjectName() const
{
	return m_str4Name;
}

int ALTObject::getID() const
{
	return m_nObjID;
}
void ALTObject::ExportObject(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(GetType());
	//exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeInt(m_nObjID);

	for (int i =0; i <OBJECT_STRING_LEVEL; ++i)
	{
		exportFile.getFile().writeField(m_str4Name.at(i).c_str());
	}
}

void ALTObject::ImportObjects(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		int objType = 0;
		importFile.getFile().getInteger(objType);
		ALTObject *obj = ALTObject::CreateObjectByType((ALTOBJECT_TYPE)objType);
		if (obj != NULL)
		{
			obj->ImportObject(importFile);
		}
		delete obj;
	}
}
void ALTObject::ImportObject(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nObjID);//skip object id, it will be re-generated
	//read object name
	//Database version 1000, the objects' name level is 4
	for (int i =0; i < 4; ++i)
	{
		char chName[512];
		importFile.getFile().getField(chName,512);
		m_str4Name.at(i) = chName;
	}
	
}

void ALTObject::GetObjectNameList(ALTOBJECT_TYPE objType,int nAirportID,ALTObjectIDList& lstObjectName)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT NAME_L1, NAME_L2, NAME_L3, NAME_L4\
					 FROM ALTOBJECT\
					 WHERE TYPE = %d AND APTID = %d\
					 ORDER BY NAME_L1, NAME_L2, NAME_L3, NAME_L4"),
					 (int)objType,nAirportID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	int nObjID = -1;
	CString strName1 = _T("");
	CString strName2 = _T("");
	CString strName3 = _T("");
	CString strName4 = _T("");

	while (!adoRecordset.IsEOF())
	{
		strName1.Empty();
		strName2.Empty();
		strName3.Empty();
		strName4.Empty();

		ALTObjectID objName;

//		adoRecordset.GetFieldValue(_T("ID"),nObjID);

		adoRecordset.GetFieldValue(_T("NAME_L1"),strName1);
		objName.at(0) = strName1;

		adoRecordset.GetFieldValue(_T("NAME_L2"),strName2);
		objName.at(1) = strName2;

		adoRecordset.GetFieldValue(_T("NAME_L3"),strName3);
		objName.at(2) = strName3;	

		adoRecordset.GetFieldValue(_T("NAME_L4"),strName4);
		objName.at(3) = strName4;


		lstObjectName.push_back(objName);

		adoRecordset.MoveNextData();

	}
}
//return type name 
const static CString str_ObjTypeNames[ALT_ATYPE_END] = 
{
	"Unknown",
	"Runway",
	"Taxiway",
	"Stand",
	"Deice Bay",
	"Ground Route",
	"Waypoint",
	"Hold",
	"Air Route",
	"Sector",
	"Surface Area",
	"Structure",
	"Wall Shape",
	"Contour",

	"Stretch",
	"Intersection",
	"Vehicle Pool Parking",
	"Traffic Light",
	"Toll Gate",
	"Stop Sign",
	"Yield Sign",
	"Round About",
	"Turnoff",
	"Overpass",
	"Underpass",
	"Lane Adapter",
	"Cloverleaf",
	"Line Parking",
	"Nose in Parking",
	"Parking Lot",	
	"Heliport",
	"Circle Stretch",
	"Drive Road",
	"Parking Place",
	"Reporting Area",
	"Start Position",
	"Meeting Point",
};


CString ALTObject::GetTypeName( ALTOBJECT_TYPE objType )
{
	return str_ObjTypeNames[objType];
}

CString ALTObject::GetTypeName() const
{
	return ALTObject::GetTypeName(GetType());
}

bool ALTObject::CopyData(const ALTObject* pObj )
{
	ALTObject* pOther = (ALTObject*)pObj;
	m_str4Name = pOther->m_str4Name;
	getDisplayProp()->CopyData(*pOther->getDisplayProp());
	m_nAptID = pOther->m_nAptID;
	m_bLocked = pOther->m_bLocked;
	return true;
}

void ALTObject::getObjName( ALTObjectID& objName ) const
{
	objName = m_str4Name;
}

void ALTObject::setObjName( const ALTObjectID& objName )
{
	m_str4Name = objName;
}

void ALTObject::SetLockedByID( int nID, bool bLocked/* = true*/ )
{
	CString strSQL;
	strSQL.Format(_T("UPDATE ALTOBJECT SET LOCKED = %d WHERE ID = %d"), bLocked ? 1 : 0, nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ALTObject::SetLockedByType( ALTOBJECT_TYPE objType, bool bLocked/* = true*/ )
{
	CString strSQL;
	strSQL.Format(_T("UPDATE ALTOBJECT SET LOCKED = %d WHERE TYPE = %d"), bLocked ? 1 : 0, objType);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

bool ALTObject::GetLockedByeID( int nID, bool& bFailed )
{
	bFailed = true;

	CString strSQL;
	strSQL.Format(_T("SELECT LOCKED FROM ALTOBJECT WHERE ID = %d"), nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if(!adoRecordset.IsEOF())
	{
		int nLocked = 1;
		adoRecordset.GetFieldValue(_T("LOCKED"), nLocked);
		bFailed = false;
		return 0 != nLocked;
	}
	return true;
}

CString ALTObject::GetSelectScriptForPath2008( int nObjID ) const
{
	return GetSelectScript(nObjID);
}

int  ALTObject::GetInsertScript() const
{
	return 0;
}

int  ALTObject::GetUpdateScript() const
{
 return 0;
}

int ALTObject::GetDeleteScript() const
{
 return 0;
}


void ALTObject::setAptID( int nAptID )
{
	m_nAptID = nAptID;
}

int ALTObject::getAptID() const
{
	return m_nAptID;
}



ALTObjectDisplayProp *ALTObject::getDisplayProp(bool breload)
{
	if(m_pDispProp == NULL)
	{
		m_pDispProp = NewDisplayProp();
		if(m_pDispProp)
			m_pDispProp->ReadData(getID());
	}
	else if(breload)
	{
		m_pDispProp->ReadData(getID());
	}

	return m_pDispProp;
}

ALTObjectDisplayProp * ALTObject::NewDisplayProp()
{
	//ASSERT(FALSE);
	return new ALTObjectDisplayProp();
}

ALTOBJECT_TYPE ALTObject::GetType() const
{
	return ALT_UNKNOWN;
}
