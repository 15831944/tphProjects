#pragma once
#include "../Database/DBElement.h"
#include "VehicleDefine.h"
#include "../Database/DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"
#include <vector>

const static int DEFAULT_VEHICLE_TYPECOUNT = 19;
class INPUTAIRSIDE_API CVehicleSpecificationItem : public DBElement
{
public:


public:
	const static CString VehicleName[];
	const static enumVehicleBaseType VehicleType[];
	const static enumVehicleUnit VehicleUnit[];
	const static double Length[];
	const static double Width[];
	const static double Height[];
	const static double Acceleration[];
	const static double MaxSpeed[];
	const static double Deceleration[];
	const static double Capacity[];
	const static double FuelConsumed[];
	CVehicleSpecificationItem(void);
	~CVehicleSpecificationItem(void);

public:
	void setName(const CString& strName);
	CString getName() const;

	//void setType(enumVehicleType vtype);
	//enumVehicleType getType() const;

	void setLength(double m_dLength);
	double getLength() const;

	void setWidth(double dWidth);
	double getWidth() const;

	void setHeight(double dHeight);
	double getHeight() const;

	void setAcceleration(double dAccel);
	double getAcceleration() const;

	void setMaxSpeed(double dSpeed);
	double getMaxSpeed() const;

	void setDeceleration( double dDecel);
	double getDecelration() const;

	void setCapacity( double dCapacity);
	double getCapacity() const;
    
	void SetFuelConsumed(double _fuel) ;
	double GetFuelCOnsumed()  ;

	void setBaseType(enumVehicleBaseType type);
  
	enumVehicleBaseType getBaseType() const;
	//void setLandSideBaseType(enumVehicleLandSideType type);
	/*enumVehicleLandSideType getLandSideBaseType();
	enumVehicleLandSideType getLandsideOldBaseType();
    void setLandsideOldBaseType(enumVehicleLandSideType tmpType);*/
	void setUnit(enumVehicleUnit _unit);
	enumVehicleUnit getUnit() const;

	virtual void InitFromDBRecordset(CADORecordset& recordset);
	//virtual void InitFromLandSideDBRecordset(CADORecordset &recordset);

	int GetVehicleTypeID();
	//void AddOperationVehicle(LandSideVehicleType *OperationType);
	//LandSideVehicleType *getOperationVehicleType(int Item)const; 
	//int getOperationVehicleCount();
	//void deleteOperationTypeItem(int item);
	//void DeleteOperationVehicleTypeDB();
protected:

	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	

private:
	/*enumVehicleType m_type;*/
	int m_nProjID;
	CString m_strName;
	double m_dLength;
	double m_dWidth;
	double m_dHeight;
	double m_dAcceleration;
	double m_dMaxSpeed;
	double m_dDeceleration;
	double m_dCapacity;
    double m_FuelConsumed ; // fuel consumed 
	enumVehicleBaseType m_enumBaseType;
	enumVehicleUnit m_enumUnit;

public:
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
public:
	static CString GetVehicleTypeByID(int _type) ;
	static int GetVehicleIDByVehicleName(CString& _name) ;
};


