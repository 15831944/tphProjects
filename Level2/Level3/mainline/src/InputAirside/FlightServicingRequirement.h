#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "ServicingRequirement.h"
#include "common/AirportDatabase.h"
#include "Common/FLT_CNST.H"
#include <map>
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirportDatabase;
class CVehicleSpecifications;
class CVehicleSpecificationItem;

//VehicleType_unknown = 0,
//VehicleType_FuelTruck,
//VehicleType_FuelBrowser,
//VehicleType_CateringTruck,
//VehicleType_BaggageTug,
//VehicleType_BaggageCart,
//VehicleType_MaintenanceTruck,
//VehicleType_Lift,
//VehicleType_CargoTug,
//VehicleType_CargoULDCart,
//VehicleType_Conveyor,
//VehicleType_Stairs,
//VehicleType_StaffCar,
//VehicleType_CrewBus,
//VehicleType_TowTruck,
//VehicleType_DeicingTruck,
//VehicleType_PaxTruck,



//const static int DefaultVehicleTypeCondition[] =
//{
//		enumVehicleTypeCondition_Per100Liters,
//		enumVehicleTypeCondition_Per100Liters,
//		enumVehicleTypeCondition_Per100Liters,
//		enumVehicleTypeCondition_Per10Pax,
//		enumVehicleTypeCondition_DurationOfCarts,
//		enumVehicleTypeCondition_Per10Bags,
//		enumVehicleTypeCondition_PerVehicle,
//		enumVehicleTypeCondition_PerVehicle,
//		enumVehicleTypeCondition_PerVehicle,
//		enumVehicleTypeCondition_PerVehicle,
//		enumVehicleTypeCondition_Per10Bags,
//		enumVehicleTypeCondition_Per10Pax,
//		enumVehicleTypeCondition_PerVehicle,
//		enumVehicleTypeCondition_PerVehicle,
//		enumVehicleTypeCondition_PriorPushToPush,
//		enumVehicleTypeCondition_PerVehicle,
//		enumVehicleTypeCondition_Per10Pax,
//
//
//
//		//add new before this line
//		enumVehicleTypeCondition_Per100Liters
//};

class INPUTAIRSIDE_API CFlightServicingRequirement : public DBElementCollection<CServicingRequirement>
{
public:

public:
	CFlightServicingRequirement(void);
	CFlightServicingRequirement(CString& strFlightType, CVehicleSpecifications *pVehicleSpec);
	CFlightServicingRequirement(FlightConstraint& fltConstraint, CVehicleSpecifications *pVehicleSpec);
	~CFlightServicingRequirement(void);

	void SetFltType(FlightConstraint& fltType);
	void SetFltType(CString& strFltType);
	FlightConstraint GetFltType();
	int GetFltTypeID()                         {    return m_nFltTypeID;          }
	void SetFltTypeID(int nFltTypeID)          {    m_nFltTypeID = nFltTypeID;    }

public:
	void SetAirportDatabase(CAirportDatabase* pAirportDatabase);

	//DBElementCollection
	virtual void SaveData(int nProj);
	virtual void DeleteData();
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nProjID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

public:
	void DeleteDataFromDB();

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	void ImportSaveData(int nProj,CString strFltType);
	void ImportGetInsertSQL(int nProjID, CString& strSQL,CString strFltType) const;

	void SetDefaultValue(CServicingRequirement* pServiceRequirement,CVehicleSpecificationItem *pVehiSepcItem);
	void SetVehicleSpecification(CVehicleSpecifications* pVehicleSpecification);

private:
	FlightConstraint  m_FlightType;
	int               m_nFltTypeID;
	CAirportDatabase* m_pAirportDatabase;
	CVehicleSpecifications* m_pVehicleSpecification;
};
