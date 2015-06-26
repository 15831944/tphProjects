#pragma once

#include "vector"
#include "..\AirsideGUI\NodeViewDbClickHandler.h"
#include "VehicleSpecifications.h"
#include "ALTObjectGroup.h"
#include "..\Common\ProbabilityDistribution.h"
#include "../Common/Flt_cnst.h"
#include "InputAirsideAPI.h"
#include "Common\ProDistrubutionData.h"

enum VehicleRegimenType
{
	ServiceCloset,
	ServiceNextDep,
	ServiceRandom,
	ServiceFIFO,
	NextArrival,
	NextOperation,
	FillToCapacity
};

enum VehicleLeaveType
{
	BeforeAircraftArrival,
	AfterAircraftArrival,
	BeforeAircraftDeparts
};
class CAirsideExportFile;
class CAirsideImportFile;
class FlightConstraint;
class CAirportDatabase;
class CADORecordset;
class FollowMeCarServiceMeetingPoints;
//-------------------------------------------CVehicleServiceTimeRange-------------------------------------------------
class INPUTAIRSIDE_API CVehicleServiceTimeRange
{
public:
	CVehicleServiceTimeRange();
	~CVehicleServiceTimeRange();

	int GetID(){return m_nID;}
	void SetStartTime(ElapsedTime& startTime){m_startTime = startTime;}
	ElapsedTime GetStartTime(){return m_startTime;}
	void SetEndTime(ElapsedTime& endTime){m_endTime = endTime;}	
	ElapsedTime GetEndTime(){return m_endTime;}
	void SetRegimenType(VehicleRegimenType enumRegimenType){m_enumRegimenType = enumRegimenType;}
	VehicleRegimenType GetRegimenType(){return m_enumRegimenType;}
	void SetLeaveTime(ElapsedTime& tLeaveTime){m_tLeaveTime = tLeaveTime;}
	ElapsedTime GetLeaveTime(){return m_tLeaveTime;}
	void SetLeaveType(VehicleLeaveType enumLeaveType){m_enumLeaveType = enumLeaveType;}
	VehicleLeaveType GetLeaveType(){return m_enumLeaveType;}
	void SetAircraftServiceNum(int nAircraftServiceNum){m_nAircraftServiceNum = nAircraftServiceNum;}
	int GetAircraftServiceNum(){return m_nAircraftServiceNum;}
 	CProDistrubution* GetProDistrubution(){ return &m_proDistribution; }

	void ReadData(CADORecordset& adoRecordset);
	void DeleteData();
	void SaveData(int nFltTypeID);
	void UpdateData();

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nFltTypeID);

private:
	int m_nID;
	ElapsedTime m_startTime;
	ElapsedTime m_endTime;
	VehicleRegimenType m_enumRegimenType;
	ElapsedTime m_tLeaveTime;
	VehicleLeaveType m_enumLeaveType;
	int m_nAircraftServiceNum;
	CProDistrubution m_proDistribution;
};

//-----------------------------------------CVehicleServiceFlightType---------------------------------------------------
class INPUTAIRSIDE_API CVehicleServiceFlightType
{
public:
	typedef std::vector<CVehicleServiceTimeRange*> ServiceTimeRangeVector;
	typedef std::vector<CVehicleServiceTimeRange*>::iterator ServiceTimeRangeIter;
public:
	CVehicleServiceFlightType();
	~CVehicleServiceFlightType();

	int GetID(){return m_nID;}
	FlightConstraint GetFltType(){return m_fltType;}
	void SetFltType(const FlightConstraint& fltType){m_fltType = fltType;}

	bool FitTimeRange(const ElapsedTime& tTime);

	void ReadData(CADORecordset& adoRecordset);
	void ReadServiceTimeRangeData();
	void SaveData(int nStandID);
	void UpdateData();
	void DeleteData();

	int GetServiceTimeRangeCount();
	CVehicleServiceTimeRange* GetServiceTimeRangeItem(int nIndex);
	void AddServiceTimeRangeItem(CVehicleServiceTimeRange* pItem);
	void DelServiceTimeRangeItem(CVehicleServiceTimeRange* pItem);
	void RemoveAll();

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nStandID);	
	void ImportSaveData(int nStandID,CString strFltType);

private:
	int m_nID;
	FlightConstraint m_fltType;
	ServiceTimeRangeVector m_vTimeRangeList;
	ServiceTimeRangeVector m_vNeedDelTimeRangeList;
	CAirportDatabase* m_pAirportDB;
};

//----------------------------------------CVehicleServiceStandFamily----------------------------------------------------
class INPUTAIRSIDE_API CVehicleServiceStandFamily
{
public:
	typedef std::vector<CVehicleServiceFlightType*> ServiceFlightTypeVector;
	typedef std::vector<CVehicleServiceFlightType*>::iterator ServiceFlightTypeIter;
public:
	CVehicleServiceStandFamily();
	~CVehicleServiceStandFamily();

	int GetID(){return m_nID;}
	void SetStandfamilyID(int nStandfamilyID){m_nStandfamilyID = nStandfamilyID;}
	int GetStandfamilyID(){return m_nStandfamilyID;}

	ALTObjectID GetStandALTObjectID(){return m_objName;}

	void ReadData(CADORecordset& adoRecordset);
	void ReadServiceFlightTypeData();
	void SaveData(int nVehiclePoolID);
	void UpdateData();
	void DeleteData();

	int GetServiceFlightTypeCount();
	CVehicleServiceFlightType* GetServiceFlightTypeItem(int nIndex);
	void AddServiceFlightTypeItem(CVehicleServiceFlightType* pItem);
	void DelServiceFlightTypeItem(CVehicleServiceFlightType* pItem);
	void RemoveAll();

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nVehiclePoolID);	
	void Sort();

	bool Fit(const AirsideFlightDescStruct& _fltDesc, char mode, const ElapsedTime& tTime);
	void SetLeaveTime(ElapsedTime tLeaveTime){m_tLeavePoolTime = tLeaveTime;}
	ElapsedTime GetLeaveTime(){return m_tLeavePoolTime;}
private:
	int m_nID;
	int	m_nStandfamilyID;	//stand family
	ALTObjectID m_objName;
	ServiceFlightTypeVector m_vServiceFlightTypeList;
	ServiceFlightTypeVector m_vNeedDelServiceFlightTypeList;
	CAirportDatabase* m_pAirportDB;
	ElapsedTime m_tLeavePoolTime;

};

//---------------------------------------CVehiclePool-----------------------------------------------------
class INPUTAIRSIDE_API CVehiclePool
{
public:
	typedef std::vector<CVehicleServiceStandFamily*> ServiceStandFamilyVector;
	typedef std::vector<CVehicleServiceStandFamily*>::iterator ServiceStandFamilyIter;
public:
	CVehiclePool(bool bFollowMeCar);
	~CVehiclePool();

	int GetID(){return m_nID;}
	void SetPoolName(CString strPoolName){m_strPoolName = strPoolName;}
	CString GetPoolName(){return m_strPoolName;}
	void SetVehicleQuantity(int nVehicleQuantity){m_nVehicleQuantity = nVehicleQuantity;}
	int GetVehicleQuantity(){return m_nVehicleQuantity;}
	void SetParkingLotID(int nParkingLotID){m_nParkingLotID = nParkingLotID;}
	int GetParkingLotID(){return m_nParkingLotID;}

	void SetDistName(CString strDistName){m_strDistName = strDistName;}
	CString GetDistName(){return m_strDistName;}
	void SetDistScreenPrint(CString strScreenPrint){m_strDistScreenPrint = strScreenPrint;}
	CString GetDistScreenPrint(){return m_strDistScreenPrint;}
	void SetProbTypes(ProbTypes enumProbType){m_enumProbType = enumProbType;}
	ProbabilityDistribution* GetProbDistribution();
	void SetPrintDist(CString strprintDist){m_strprintDist = strprintDist;}

	void ReadData(CADORecordset& adoRecordset);
	void ReadServiceStandFamilyData();
	void SaveData(int nVehicleTypePoolsID);
	void UpdateData();
	void DeleteData();

	int GetServiceStandFamilyCount();
	CVehicleServiceStandFamily* GetServiceStandFamilyItem(int nIndex);
	bool GetServiceFitStand(const ALTObjectID& standID, std::vector<CVehicleServiceStandFamily*>& standVector);
	void AddServiceStandFamilyItem(CVehicleServiceStandFamily* pItem);
	void DelServiceStandFamilyItem(CVehicleServiceStandFamily* pItem);
	void RemoveAll();

	FollowMeCarServiceMeetingPoints* GetFollowMeCarServiceMeetingPoints();

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile,int nVehicleTypePoolsID);	
private:
	int m_nID;
	CString m_strPoolName;
	//Attributes
	int m_nVehicleQuantity;
	int m_nParkingLotID;
	ServiceStandFamilyVector m_vServiceStandFamilyList;
	ServiceStandFamilyVector m_vNeedDelServiceStandFamilyList;
	CAirportDatabase* m_pAirportDB;
	CString	m_strDistName;   //distribution
	CString	m_strDistScreenPrint;
	ProbTypes m_enumProbType;  // distribution(for Engin)
	CString m_strprintDist;

	ProbabilityDistribution* m_pDistribution;
	FollowMeCarServiceMeetingPoints* m_pFollowMeCarServiceMeetingPoints;
	bool m_bFollowMeCar;
};

//--------------------------------------CVehicleTypePools------------------------------------------------------
class INPUTAIRSIDE_API CVehicleTypePools
{
public:
	typedef std::vector<CVehiclePool*> VehiclePoolVector;
	typedef std::vector<CVehiclePool*>::iterator VehiclePoolIter;
public:
	CVehicleTypePools();
	~CVehicleTypePools();

	int GetID(){return m_nID;}
	void SetVehicleTypeID(int nVehicleTypeID){m_nVehicleTypeID = nVehicleTypeID;}
	int GetVehicleTypeID(){return m_nVehicleTypeID;}
	CString GetVehicleTypeName();
	void SetVehicleSpecifications(CVehicleSpecifications* pvehicleSpecifications);

	void ReadData(CADORecordset& adoRecordset);
	void ReadVehiclePoolData();
	void SaveData(int nProjID);
	void UpdateData();
	void DeleteData();

	int GetVehiclePoolCount();
	CVehiclePool* GetVehiclePoolItem(int nIndex);
	void AddVehiclePoolItem(CVehiclePool* pItem);
	void DelVehiclePoolItem(CVehiclePool* pItem);
	void RemoveAll();

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	bool IsFollowMeCarPoolsData();
	bool IsTowTruckPoolsData();
private:
	int m_nID;
	int m_nVehicleTypeID;
	CVehicleSpecifications* m_pvehicleSpecifications;
	VehiclePoolVector m_vPoolList;
	VehiclePoolVector m_vNeedDelPoolList;
	CAirportDatabase* m_pAirportDB;
};

//----------------------------------------CVehiclePoolsAndDeployment----------------------------------------------------
class INPUTAIRSIDE_API CVehiclePoolsAndDeployment
{
public:
	typedef std::vector<CVehicleTypePools*> VehicleTypePoolsVector;
	typedef std::vector<CVehicleTypePools*>::iterator VehicleTypePoolsIter;
public:
	CVehiclePoolsAndDeployment();
	~CVehiclePoolsAndDeployment();

	void ReadData(int nProjID);
	void SaveData(int nProjID);
	void InitVehicleType(int nProjID);
	void InsertDataIntoStructure(VehicleTypePoolsVector& vRead);

	int GetVehicleTypePoolsCount();
	CVehicleTypePools* GetVehicleTypePoolsItem(int nIndex);
	CVehicleTypePools* GetVehicleTypePools(int nVehicleTypeID);
	void AddVehicleTypePoolsItem(CVehicleTypePools* pItem);
	void DelVehicleTypePoolsItem(CVehicleTypePools* pItem);
	void RemoveAll();

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	static void ExportVehiclePoolsAndDeployment(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportVehiclePoolsAndDeployment(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	
	void GetVehicleTypePoolsByName(VehicleTypePoolsVector& vPools,const CString& VehicleName);

	enumVehicleBaseType GetVehicleTypeByID(int nID);
	const CVehicleSpecifications* GetVehicleSpecification();
		

private:
	CVehicleSpecifications m_vehicleSpecifications;
	VehicleTypePoolsVector m_vVehicleTypePools;
	VehicleTypePoolsVector m_vNeedDelVehicleTypePools;
	CAirportDatabase* m_pAirportDB;
};
