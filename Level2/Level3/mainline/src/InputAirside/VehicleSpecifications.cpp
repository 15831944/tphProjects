#include "StdAfx.h"
#include "..\Database\DBElementCollection_Impl.h"
#include ".\vehiclespecifications.h"
#include "AirsideImportExportManager.h"
#include"../Common/AirportDatabase.h"
#include "../Database/ARCportADODatabase.h"

#include "../Landside/VehicleDistributionDetail.h"
/*
NOTE: if you change Vehicle item's name, you must change associated name in ARCFlight ActiveX control!!!
*/

const CString CVehicleSpecificationItem::VehicleName[] =
{
	_T("Fuel truck"),    //0 
		_T("Fuel Browser"),			  //1
		_T("Catering truck"),   //2
		_T("Baggage tug"),            //3
		_T("Baggage cart"),    //4
		_T("Maintenance truck"),      //5
		_T("Lift"),                   //6
		_T("Cargo tug"),              //7
		_T("Cargo ULD cart"),    //8
		_T("Conveyor"),               //9
		_T("Stairs"),                 //10
		_T("Staff car"),              //11
		_T("Crew bus"),               //12   
		_T("Tow truck"),              //13
		_T("Deicing truck"),          //14
		_T("Pax bus A"),        //15
		_T("Pax bus B"),        //16
		_T("Pax bus C"),        //17
		_T("Follow me car"),    //18
		//nCout = 19

};
const enumVehicleBaseType CVehicleSpecificationItem::VehicleType[] =
{	
	VehicleType_General,
	VehicleType_General,
	VehicleType_General,
	VehicleType_BaggageTug,
	VehicleType_JointBagTug,
	VehicleType_General,
	VehicleType_General,
	VehicleType_General,
	VehicleType_General,
	VehicleType_General,
	VehicleType_General,
	VehicleType_General,
	VehicleType_General,
	VehicleType_TowTruck,
	VehicleType_DeicingTruck,
	VehicleType_PaxTruck,
	VehicleType_PaxTruck,
	VehicleType_PaxTruck,
	VehicleType_FollowMeCar,
};
int CVehicleSpecificationItem::GetVehicleIDByVehicleName(CString& _name)
{
	for (int i = 0 ; i <VehicleSize  ;i++)
	{
		if(CVehicleSpecificationItem::VehicleName[i].CompareNoCase(_name) == 0)
			return i ;
	}
	return -1 ;
}
 CString CVehicleSpecificationItem::GetVehicleTypeByID(int _type)
 {
    if((_type) >= 0 && (_type) <  VehicleSize )
	   return CVehicleSpecificationItem::VehicleName[_type] ;
	else
		return CString() ;
 }
const enumVehicleUnit CVehicleSpecificationItem::VehicleUnit[] =
{	
		Liters,
		PerVehicle,
		Pax,
		Carts,
		Bag,
		PerVehicle,
		PerVehicle,
		PerVehicle,
		Kg,
		PerVehicle,
		PerVehicle,
		PerVehicle,
		PerVehicle,
		PerVehicle,
		Liters,
		Pax,
		Pax,
		Pax,
		PerVehicle
};

const double CVehicleSpecificationItem::Length[] = {10.0,4.0,15.0,3.0,3.0,8.0,8.0,18.0,8.0,10.0,10.0,8.0,8.0,8.0,6.0,6.0,12.0,15.0,4.0};
const double CVehicleSpecificationItem::Width[] = {4.0,4.0,3.0,2.0,2.0,4.0,4.0,5.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,3.5,4.0,4.0,4.0};
const double CVehicleSpecificationItem::Height[] = {4.0,4.0,4.0,2.0,2.0,5.0,5.0,5.0,4.0,5.0,6.0,4.0,4.0,5.0,4.0,3.5,4.0,4.0,4.0};
const double CVehicleSpecificationItem::Acceleration[] = {10.0,10.0,10.0,8.0,-1.0,10.0,8.0,8.0,10.0,8.0,8.0,10.0,10.0,8.0,10.0,10.0,10.0,10.0,10.0};
const double CVehicleSpecificationItem::MaxSpeed[] = {45,40,50,40,-1.0,45,40,40,45,40,40,50,40,40,50,50,50,50,40};
const double CVehicleSpecificationItem::Deceleration[] = {10.0,10.0,10.0,8,-1.0,10.0,8.0,8.0,10.0,8.0,8.0,10.0,10.0,8.0,10.0,10.0,10.0,10.0,10.0};
const double CVehicleSpecificationItem::Capacity[] = {10000.0,0,100.0,4.0,30.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,5000.0,20.0,50.0,100.0,0};
const double CVehicleSpecificationItem::FuelConsumed[] = {100.0,100.0,100.0,100.0,-1.0,100.0,100.0,100.0,100.0,100.0,100.0,100.0,100.0,100.0,100.0,20.0,50.0,100.0};
CVehicleSpecificationItem::CVehicleSpecificationItem(void)
:m_nProjID(-1)
{
	m_dLength = 0.0;
	m_dWidth = 0.0;
	m_dHeight = 0.0;
	m_dAcceleration = 0.0;
	m_dMaxSpeed = 0.0;
	m_dDeceleration = 0.0;
	m_dCapacity = 0.0;
	m_FuelConsumed = 100.0 ;
	m_enumBaseType = VehicleType_General;
	m_enumUnit = PerVehicle;
}
CVehicleSpecificationItem::~CVehicleSpecificationItem(void)
{

   
}

void CVehicleSpecificationItem::setName(const CString& strName)
{
	m_strName = strName;
}
CString CVehicleSpecificationItem::getName() const
{
	return m_strName;
	//return VehicleName[m_type];
}
//void CVehicleSpecificationItem::setType(enumVehicleType vtype)
//{
//	m_type = vtype;
//}
//CVehicleSpecificationItem::enumVehicleType CVehicleSpecificationItem::getType() const
//{
//	return m_type;
//}
void CVehicleSpecificationItem::setLength(double dLength)
{
	m_dLength = dLength;
}
double CVehicleSpecificationItem::getLength() const
{
	return m_dLength;
}
void CVehicleSpecificationItem::setWidth(double dWidth)
{
	m_dWidth = dWidth;
}
double CVehicleSpecificationItem::getWidth() const
{
	return m_dWidth;
}
void CVehicleSpecificationItem::setHeight(double dHeight)
{
	m_dHeight = dHeight;
}
double CVehicleSpecificationItem::getHeight() const
{
	return m_dHeight;
}
void CVehicleSpecificationItem::setAcceleration(double dAccel)
{
	m_dAcceleration = dAccel;
}
double CVehicleSpecificationItem::getAcceleration() const
{
	return m_dAcceleration;
}
void CVehicleSpecificationItem::setMaxSpeed(double dSpeed)
{
	m_dMaxSpeed = dSpeed;
}
double CVehicleSpecificationItem::getMaxSpeed() const
{
	return m_dMaxSpeed;
}
void CVehicleSpecificationItem::setDeceleration( double dDecel)
{
	m_dDeceleration = dDecel;
}
double CVehicleSpecificationItem::getDecelration() const
{
	return m_dDeceleration;
}

void CVehicleSpecificationItem::setCapacity( double dCapacity)
{
	m_dCapacity = dCapacity;
}
double CVehicleSpecificationItem::getCapacity() const
{
	return m_dCapacity;
}
void CVehicleSpecificationItem::setBaseType(enumVehicleBaseType type)
{
	m_enumBaseType = type;
}
enumVehicleBaseType CVehicleSpecificationItem::getBaseType() const
{
	return m_enumBaseType;
}
//enumVehicleLandSideType CVehicleSpecificationItem::getLandSideBaseType() 
//{
//	return m_enumBaseLandSideType;
//}
//enumVehicleLandSideType CVehicleSpecificationItem::getLandsideOldBaseType()
//{
//	return m_oldBaseLandSideType;
//}
//void CVehicleSpecificationItem::setLandSideBaseType(enumVehicleLandSideType type)
//{
//	m_enumBaseLandSideType=type;
//}
//void CVehicleSpecificationItem::setLandsideOldBaseType(enumVehicleLandSideType tmpType)
//{
//	m_oldBaseLandSideType=tmpType;
//}
void CVehicleSpecificationItem::setUnit(enumVehicleUnit _unit)
{
	m_enumUnit = _unit;
}
//void CVehicleSpecificationItem::AddOperationVehicle(LandSideVehicleType *OperationType)
//{
//	m_OperationVehicles.push_back(OperationType);
//}
//LandSideVehicleType* CVehicleSpecificationItem::getOperationVehicleType(int item) const
//{
//	/*std::vector<LandSideVehicleType*>::iterator iter=m_OperationVehicles.begin()+item;
//	return  *iter;*/
//	return m_OperationVehicles.at(item);
//}
//int CVehicleSpecificationItem::getOperationVehicleCount()
//{
//	return (int)m_OperationVehicles.size();
//}
//void CVehicleSpecificationItem::deleteOperationTypeItem(int item)
//{
//	delete m_OperationVehicles.at(item);
//	m_OperationVehicles.erase(m_OperationVehicles.begin()+item);
//
//}
enumVehicleUnit CVehicleSpecificationItem::getUnit() const
{
	return m_enumUnit;
}

int CVehicleSpecificationItem::GetVehicleTypeID()
{
	return m_nID;
}
void CVehicleSpecificationItem::SetFuelConsumed(double _fuel) 
{ 
	m_FuelConsumed = _fuel ;
};
double CVehicleSpecificationItem::GetFuelCOnsumed() 
{ 
	return m_FuelConsumed ;
} ;
void CVehicleSpecificationItem::InitFromDBRecordset(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PROJECTID"),m_nProjID);

	int nType = 0;
	adoRecordset.GetFieldValue(_T("TYPE"),nType);
	if (nType >= VehicleType_General && nType < VehicleType_Count)
	{
		m_enumBaseType = (enumVehicleBaseType)nType;
	}
	adoRecordset.GetFieldValue(_T("NAME"),m_strName);
	adoRecordset.GetFieldValue(_T("LENGTH"),m_dLength);
	adoRecordset.GetFieldValue(_T("WIDTH"),m_dWidth);
	adoRecordset.GetFieldValue(_T("HEIGHT"),m_dHeight);
	adoRecordset.GetFieldValue(_T("ACCELERATION"),m_dAcceleration);
	adoRecordset.GetFieldValue(_T("MAXSPEED"),m_dMaxSpeed);
	adoRecordset.GetFieldValue(_T("DECELERATION"),m_dDeceleration);
	adoRecordset.GetFieldValue(_T("CAPACITY"),m_dCapacity);
	adoRecordset.GetFieldValue(_T("FUEL"),m_FuelConsumed);
	int nUnit = 0;
	adoRecordset.GetFieldValue(_T("UNIT"), nUnit);
	m_enumUnit = (enumVehicleUnit)nUnit;
}
//void CVehicleSpecificationItem::InitFromLandSideDBRecordset(CADORecordset& adoRecordset)
//{
//	adoRecordset.GetFieldValue(_T("ID"),m_nID);
//	adoRecordset.GetFieldValue(_T("PROJECTID"),m_nProjID);
//
//	int nType = 0;
//	adoRecordset.GetFieldValue(_T("TYPE"),nType);
//	if (nType >=VehiclType_Private && nType < VehicleType_LandSideCount)
//	{
//		
//		m_enumBaseLandSideType = (enumVehicleLandSideType)nType;
//	}
//	else
//	{
//		m_enumBaseLandSideType=VehiclType_Private;
//	}
//	adoRecordset.GetFieldValue(_T("NAME"),m_strName);
//	adoRecordset.GetFieldValue(_T("LENGTH"),m_dLength);
//	adoRecordset.GetFieldValue(_T("WIDTH"),m_dWidth);
//	adoRecordset.GetFieldValue(_T("HEIGHT"),m_dHeight);
//	adoRecordset.GetFieldValue(_T("ACCELERATION"),m_dAcceleration);
//	adoRecordset.GetFieldValue(_T("MAXSPEED"),m_dMaxSpeed);
//	adoRecordset.GetFieldValue(_T("DECELERATION"),m_dDeceleration);
//	adoRecordset.GetFieldValue(_T("CAPACITY"),m_dCapacity);
//	adoRecordset.GetFieldValue(_T("FUEL"),m_FuelConsumed);
//	int nUnit = 0;
//	adoRecordset.GetFieldValue(_T("UNIT"), nUnit);
//
//	m_enumUnit = (enumVehicleUnit)nUnit;
//	m_oldBaseLandSideType=m_enumBaseLandSideType;
//}

void CVehicleSpecificationItem::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("INSERT INTO IN_VEHICLESEPCIFICATION\
		(PROJECTID,TYPE, NAME, LENGTH, WIDTH, HEIGHT, ACCELERATION, MAXSPEED, DECELERATION, CAPACITY, UNIT,FUEL)\
		VALUES     (%d,%d,'%s',%f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f)"),
		nParentID,(int)m_enumBaseType,m_strName,m_dLength,m_dWidth,m_dHeight,m_dAcceleration,m_dMaxSpeed,m_dDeceleration,m_dCapacity,(int)m_enumUnit,m_FuelConsumed);

}
//void CVehicleSpecificationItem::GetLandSideInsertSQL(int nParentID,CString &strSQL) const
//{
//	strSQL.Empty();
//	strSQL.Format(_T("INSERT INTO IN_LandSide_VEHICLESEPCIFICATION\
//					 (PROJECTID,TYPE, NAME, LENGTH, WIDTH, HEIGHT, ACCELERATION, MAXSPEED, DECELERATION, CAPACITY, UNIT,FUEL)\
//					 VALUES     (%d,%d,'%s',%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f)"),
//					 nParentID,(int)m_enumBaseLandSideType,m_strName,m_dLength,m_dWidth,m_dHeight,m_dAcceleration,m_dMaxSpeed,m_dDeceleration,m_dCapacity,(int)m_enumUnit,m_FuelConsumed);
//}
//void CVehicleSpecificationItem::GetLandSideUpdateSQL(CString& strSQL) const
//{
//	strSQL.Empty();
//	strSQL.Format(_T("UPDATE IN_LandSide_VEHICLESEPCIFICATION\
//					 SET TYPE= %d, NAME = '%s', LENGTH = %.2f, WIDTH = %.2f , HEIGHT = %.2f , ACCELERATION = %.2f , MAXSPEED = %.2f , DECELERATION = %.2f , CAPACITY = %.2f , UNIT = %d ,\
//					 FUEL = %.2f \
//					 WHERE (ID = %d)"),
//					 (int)m_enumBaseLandSideType,m_strName,m_dLength,m_dWidth,m_dHeight,m_dAcceleration,m_dMaxSpeed,m_dDeceleration,m_dCapacity,(int)m_enumUnit,m_FuelConsumed, m_nID);
//
//}
//void CVehicleSpecificationItem::GetLandSideDeleteSQL(CString& strSQL)const
//{
//	strSQL.Empty();
//	strSQL.Format(_T("DELETE FROM IN_LandSide_VEHICLESEPCIFICATION\
//					 WHERE     (ID = %d)"),m_nID);
//}
//void CVehicleSpecificationItem::GetLandSideSelectSQL(int nID,CString& strSQL) const
//{
//	strSQL.Empty();
//	strSQL.Format(_T("SELECT * FROM IN_LandSide_VEHICLESEPCIFICATION WHERE (ID = %d)"),nID);
//}
void CVehicleSpecificationItem::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("UPDATE IN_VEHICLESEPCIFICATION\
		SET TYPE= %d, NAME = '%s', LENGTH = %.2f, WIDTH = %.2f , HEIGHT = %.2f , ACCELERATION = %.2f , MAXSPEED = %.2f , DECELERATION = %.2f , CAPACITY = %.2f , UNIT = %d ,\
        FUEL = %.2f \
		WHERE (ID = %d)"),
		(int)m_enumBaseType,m_strName,m_dLength,m_dWidth,m_dHeight,m_dAcceleration,m_dMaxSpeed,m_dDeceleration,m_dCapacity,(int)m_enumUnit,m_FuelConsumed, m_nID);

}
void CVehicleSpecificationItem::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("DELETE FROM IN_VEHICLESEPCIFICATION\
		WHERE     (ID = %d)"),m_nID);
}
void CVehicleSpecificationItem::GetSelectSQL(int nID,CString& strSQL)const
{
	strSQL.Empty();
	strSQL.Format(_T("SELECT * FROM IN_VEHICLESEPCIFICATION WHERE (ID = %d)"),nID);
}
void CVehicleSpecificationItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeField(m_strName);
	exportFile.getFile().writeDouble(m_dLength);
	exportFile.getFile().writeDouble(m_dWidth);
	exportFile.getFile().writeDouble(m_dHeight);
	exportFile.getFile().writeDouble(m_dAcceleration);
	exportFile.getFile().writeDouble(m_dMaxSpeed);
	exportFile.getFile().writeDouble(m_dDeceleration);
	exportFile.getFile().writeDouble(m_dCapacity);
	exportFile.getFile().writeDouble(m_FuelConsumed);
}
void CVehicleSpecificationItem::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);
	m_nProjID = importFile.getNewProjectID();

	importFile.getFile().getField(m_strName.GetBuffer(1024),1024);
	importFile.getFile().getFloat(m_dLength);
	importFile.getFile().getFloat(m_dWidth);
	importFile.getFile().getFloat(m_dHeight);
	importFile.getFile().getFloat(m_dAcceleration);
	importFile.getFile().getFloat(m_dMaxSpeed);
	importFile.getFile().getFloat(m_dDeceleration);
	importFile.getFile().getFloat(m_dCapacity);
    if(importFile.getVersion() > 1000)
		importFile.getFile().getFloat(m_FuelConsumed) ;
	SaveData(importFile.getNewProjectID());
	importFile.AddVehicleDataSpecificationIndexMap(nOldID,m_nID);
}
//void CVehicleSpecificationItem::DeleteOperationVehicleTypeDB()
//{
//	LandSideVehicleType *operationLandsideType;
//	if (this->getLandSideBaseType()==VehiclType_Private)
//	{
//		operationLandsideType=new LandSidePrivateVehicleType();
//
//	}
//	else if(this->getLandSideBaseType()==VehicleType_Taxi)
//	{
//		operationLandsideType=new LandSideTaxiVehicleType();
//	}
//	else if(this->getLandSideBaseType()==VehicleType_Public)
//	{
//		operationLandsideType=new LandSidePublicVehicleType();
//	}
//	else if(this->getLandSideBaseType()==VehicleType_Iterate)
//	{
//		operationLandsideType =new LandSideIterateVehicle();
//
//	}
//	else 
//	{
//		operationLandsideType=new LandSideVehicleType();
//	}
//
//	operationLandsideType->DeleteData(this->GetID());
//	delete operationLandsideType;
//	
//}
//////////////////////////////////////////////////////////////////////////
//CVehicleSpecifications
CVehicleSpecifications::CVehicleSpecifications(void)
{
}

CVehicleSpecifications::~CVehicleSpecifications(void)
{
	//for (int i=0;i<(int)this->GetElementCount();i++)
	//{
	//	CVehicleSpecificationItem*tmpItem= GetItem(i);
	//	if (tmpItem)
	//	{
	//		delete tmpItem;
	//		tmpItem=NULL;
	//	}
	//}
	std::vector<CVehicleSpecificationItem *>::iterator iter = m_deleteddataList.begin();
	for (;iter != m_deleteddataList.end(); ++iter)
	{
		delete (*iter);
	}

	iter = m_dataList.begin();
	for ( ;iter != m_dataList.end(); ++iter)
	{
		delete (*iter);
	}
	m_deleteddataList.clear();
	m_dataList.clear();
}
void CVehicleSpecifications::SaveData(int nParentID)
{
// 	int Allcount=(int)this->GetElementCount();
// 	for (int i=0;i<Allcount;i++)
// 	{
// 		CVehicleSpecificationItem *db=(CVehicleSpecificationItem *)this->GetItem(i);
// 		
// 		db->SaveData(nParentID);
// 	}
	DBElementCollection<CVehicleSpecificationItem>::SaveData(nParentID);
}

//void CVehicleSpecifications::SaveData(int nParentID,CAirportDatabase *pAirportDatabase)
//{
//
//	 LandSideVehicleType *operationLandsideType;
//	 int Allcount=(int)this->GetElementCount();
//	 for (int i=0;i<Allcount;i++)
//	 {
//		 CVehicleSpecificationItem *db=(CVehicleSpecificationItem *)this->GetItem(i);
//		 db->SaveData(nParentID,pAirportDatabase);
//		 if(db->getLandSideBaseType()!=db->getLandsideOldBaseType())
//		 {
//			 if (db->getLandsideOldBaseType()==VehiclType_Private)
//			 {
//				 operationLandsideType=new LandSidePrivateVehicleType();
//
//			 }
//			 else if(db->getLandsideOldBaseType()==VehicleType_Taxi)
//			 {
//				 operationLandsideType=new LandSideTaxiVehicleType();
//			 }
//			 else if(db->getLandsideOldBaseType()==VehicleType_Public)
//			 {
//				 operationLandsideType=new LandSidePublicVehicleType();
//			 }
//			 else if(db->getLandsideOldBaseType()==VehicleType_Iterate)
//			 {
//				 operationLandsideType =new LandSideIterateVehicle();
//
//			 }
//			 else 
//			 {
//				operationLandsideType=new LandSideVehicleType();
//			 }
//			
//			operationLandsideType->DeleteData(db->GetID());
//			delete operationLandsideType;
//			 
//		 }
//		 db->setLandsideOldBaseType(db->getLandSideBaseType());
//	 } 
//	 CVehicleDistributionDetail *detail;
//	 for (int i=0;i<(int)this->m_deleteddataList.size();i++)
//	 {
//		 CVehicleSpecificationItem *db=(CVehicleSpecificationItem *)this->m_deleteddataList.at(i);
//		 CADODatabase::BeginTransaction(DATABASESOURCE_TYPE_ACCESS);
//		 db->DeleteData(pAirportDatabase);
//		
//		 
//		 if (db->getLandSideBaseType()==VehiclType_Private)
//		 {
//			operationLandsideType=new LandSidePrivateVehicleType();
//
//		 }
//		 else if(db->getLandSideBaseType()==VehicleType_Taxi)
//		 {
//			operationLandsideType=new LandSideTaxiVehicleType();
//		 }
//		 else if(db->getLandSideBaseType()==VehicleType_Public)
//		 {
//			operationLandsideType=new LandSidePublicVehicleType();
//		 }
//		 else if(db->getLandSideBaseType()==VehicleType_Iterate)
//		 {
//			 operationLandsideType =new LandSideIterateVehicle();
//
//		 }
//		 else 
//		 {
//			 operationLandsideType=new LandSideVehicleType();
//		 }
//         
//		 operationLandsideType->DeleteData(db->GetID());
//		 detail=new CVehicleDistributionDetail();
//		 
//		 detail->deleteData(db->GetID());
//		 delete operationLandsideType;
//		
//         delete detail;
//		 CADODatabase::CommitTransaction(DATABASESOURCE_TYPE_ACCESS);
//		
//	 }
//	  operationLandsideType=NULL;
//	  detail=NULL;
//}

//void CVehicleSpecifications::SaveDataOperationType(int nParentID)
//{
//	int Allcount=(int)this->GetElementCount();
////	int deletecount;
//	int j=0;
//	for (int i=0;i<Allcount;i++)
//	{
//		CVehicleSpecificationItem *db=(CVehicleSpecificationItem *)this->GetItem(i);
//		db->SaveData(nParentID,NULL);
//		
//		if (db->getOperationVehicleCount()>0)
//		{
//			
//			for (j=0;j<db->getOperationVehicleCount();j++)
//			{
//				if (db->getOperationVehicleType(j)->getVehicleType()==db->getLandSideBaseType())
//				{
//					db->getOperationVehicleType(j)->SaveData();
//
//				}
//				else 
//				{
//
//					db->getOperationVehicleType(j)->DeleteData(db->GetID());
//					db->deleteOperationTypeItem(j);
//				}
//			}
//            
//		}
//
//	}
//	for (int i=0;i<(int)this->m_deleteddataList.size();i++)
//	{
//		CVehicleSpecificationItem *db=(CVehicleSpecificationItem *)this->m_deleteddataList.at(i);
//		db->DeleteData(NULL);
//		if (db->getOperationVehicleCount()>0)
//		{
//              db->getOperationVehicleType(0)->DeleteData(db->GetID());
//		}
//		
//	}
//}
//void CVehicleSpecifications::ReadData(int nParentID,CAirportDatabase *pAirportDatabase)
//{
//	CString strSelectSQL;
//	strSelectSQL.Empty();
//	strSelectSQL.Format(_T("SELECT * \
//					 FROM         IN_LandSide_VEHICLESEPCIFICATION\
//					 WHERE     (PROJECTID = %d)"),nParentID);
//	
//	strSelectSQL.Trim();
//	if (strSelectSQL.IsEmpty())
//		return;
//
//	long nRecordCount = -1;
//	CADORecordset adoRecordset;
//	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);
//	//CDatabaseADOConnetion::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset,pAirportDatabase->GetAirportConnection());
//	m_dataList.clear();
//	while (!adoRecordset.IsEOF())
//	{
//		CVehicleSpecificationItem* element = new CVehicleSpecificationItem;
//		element->InitFromLandSideDBRecordset(adoRecordset);
//		m_dataList.push_back(element);
//		adoRecordset.MoveNextData();
//	}
//	/*if(CheckDataList())
//	{
//		SaveData(nParentID,pAirportDatabase);
//		ReadData(nParentID,pAirportDatabase);
//	}*/
//}
//void  CVehicleSpecifications::ReadDataOperationType(int nParentID)
//{
//	CString strSelectSQL;
//	strSelectSQL.Empty();
//	strSelectSQL.Format(_T("SELECT * \
//						   FROM         IN_LandSide_VEHICLESEPCIFICATION\
//						   WHERE     (PROJECTID = %d)"),nParentID);
//
//	strSelectSQL.Trim();
//	if (strSelectSQL.IsEmpty())
//		return;
//
//	long nRecordCount = -1;
//	CADORecordset adoRecordset;
//	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);
//	for (int i=0;i<(int)this->GetElementCount();i++)
//	{
//		CVehicleSpecificationItem *tmpItem= GetItem(i);
//		if (tmpItem)
//		{
//			delete tmpItem;
//			tmpItem=NULL;
//		}
//	}
//	m_dataList.clear();
//	while (!adoRecordset.IsEOF())
//	{
//		CVehicleSpecificationItem* element = new CVehicleSpecificationItem;
//		element->InitFromLandSideDBRecordset(adoRecordset);
//	    if(element->getLandSideBaseType()==VehiclType_Private)
//		{
//            m_pOperationType=new LandSidePrivateVehicleType();
//			
//		}
//		if (element->getLandSideBaseType()==VehicleType_Public)
//		{
//
//			m_pOperationType=new  LandSidePublicVehicleType();
//		   
//		}
//		if(element->getLandSideBaseType()==VehicleType_Taxi)
//		{
//			m_pOperationType=new LandSideTaxiVehicleType();
//		}
//		if(element->getLandSideBaseType()==VehicleType_Iterate)
//		{
//			m_pOperationType=new LandSideIterateVehicle();
//		}
//		element->AddOperationVehicle(m_pOperationType);
//		m_pOperationType->setVehicelSpecificationID(element->GetID());
//		m_pOperationType->setVehicleType(element->getLandSideBaseType());
//		m_pOperationType->ReadData();
//		
//		m_dataList.push_back(element);
//		adoRecordset.MoveNextData();
//	}
//	/*if(CheckDataList())
//	{
//		SaveData(nParentID,NULL);
//		ReadData(nParentID,NULL);
//	}*/
//}
void CVehicleSpecifications::ReadData(int nParentID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nParentID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);
	m_dataList.clear();
	while (!adoRecordset.IsEOF())
	{
		CVehicleSpecificationItem* element = new CVehicleSpecificationItem;
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
	if(CheckDataList())
	{
		SaveData(nParentID);
		ReadData(nParentID);
	}
}

bool CVehicleSpecifications::CheckDataList()
{
	if(m_dataList.size() == 0)
	{
		//Vehicle Data Specification Initialize,insert 14 record to Table IN_VEHICLESEPCIFICATION
		for(int i = 0;i < DEFAULT_VEHICLE_TYPECOUNT ;i++)
		{
			CVehicleSpecificationItem* pNewItem = new CVehicleSpecificationItem;
			pNewItem->setName(CVehicleSpecificationItem::VehicleName[i]);
			pNewItem->setBaseType(CVehicleSpecificationItem::VehicleType[i]);
			pNewItem->setLength(CVehicleSpecificationItem::Length[i]);
			pNewItem->setWidth(CVehicleSpecificationItem::Width[i]);
			pNewItem->setHeight(CVehicleSpecificationItem::Height[i]);
			pNewItem->setAcceleration(CVehicleSpecificationItem::Acceleration[i]);
			pNewItem->setMaxSpeed(CVehicleSpecificationItem::MaxSpeed[i]);
			pNewItem->setDeceleration(CVehicleSpecificationItem::Deceleration[i]);
			pNewItem->setCapacity(CVehicleSpecificationItem::Capacity[i]);
			pNewItem->setUnit(CVehicleSpecificationItem::VehicleUnit[i]);
			pNewItem->SetFuelConsumed(CVehicleSpecificationItem::FuelConsumed[i]) ;
			m_dataList.push_back(pNewItem);
		}
		return 1;
	}
	return 0;
}

void CVehicleSpecifications::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("SELECT * \
		FROM         IN_VEHICLESEPCIFICATION\
		WHERE     (PROJECTID = %d)"),nParentID);

}
//void CVehicleSpecifications::ImportData(CAirsideImportFile& importFile)
//{
//
//}
void CVehicleSpecifications::ExportVehicleSpec(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("ID,PROJID ,NAME, LENGTH, WIDTH, HEIGHT, ACCELERATION, MAXSPEED, DECELERATION, CAPACITY,FUEL"));
	exportFile.getFile().writeLine();
	CVehicleSpecifications vehicleSpec;
	vehicleSpec.ReadData(exportFile.GetProjectID());
	vehicleSpec.ExportData(exportFile);

	exportFile.getFile().endFile();
}
void CVehicleSpecifications::ImportVehicleSpec(CAirsideImportFile& importFile)
{
 
	CVehicleSpecifications vehicleSpec;
	vehicleSpec.ImportData(importFile);
}

CVehicleSpecificationItem* CVehicleSpecifications::GetVehicleItemByName(CString strName)
{
	CVehicleSpecificationItem* pVehicleSpecificationItem;
	size_t nVehicleCount = m_dataList.size();	
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		pVehicleSpecificationItem = m_dataList.at(i);
		if(pVehicleSpecificationItem->getName() == strName)
			return pVehicleSpecificationItem;
	}
	return NULL;
}
CVehicleSpecificationItem* CVehicleSpecifications::GetVehicleItemByID(int nID)
{
	CVehicleSpecificationItem* pVehicleSpecificationItem;
	size_t nVehicleCount = m_dataList.size();	
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		pVehicleSpecificationItem = m_dataList.at(i);
		if(pVehicleSpecificationItem->GetID() == nID)
			return pVehicleSpecificationItem;
	}
	return NULL;
}

void CVehicleSpecifications::GetVehicleIDByBaseType(enumVehicleBaseType type, std::vector<int>& vIDs)
{
	size_t nVehicleCount = m_dataList.size();	
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		CVehicleSpecificationItem* pVehicleSpecificationItem = m_dataList.at(i);
		if(pVehicleSpecificationItem->getBaseType() == type)
			vIDs.push_back(pVehicleSpecificationItem->GetID());
	}
}

void CVehicleSpecifications::DoNotCall()
{
	CVehicleSpecifications vs;
	vs.AddNewItem(NULL);
}