#pragma once
#include "Parameters.h"
class AIRSIDEREPORT_API CAirsideVehicleOperParameter :
	public CParameters
{
public:
	CAirsideVehicleOperParameter(void);
	~CAirsideVehicleOperParameter(void);
public:
	void LoadParameter() {};
	void InitParameter(CXTPPropertyGrid * pGrid){} ;
	 void UpdateParameter() {} ;

	 //write and load parameter from file
	 virtual void WriteParameter(ArctermFile& _file);
	 virtual void ReadParameter(ArctermFile& _file);
	 virtual CString GetReportParamName();
protected:
	std::vector<CString> m_VehicleType ; // vehicle type for generate report
	std::vector<int> m_PoolID ; // the pool for generating report 
public:
	void AddVehicleType(CString& _vehicleType) ;
	void DeleteVehickeType(CString& _vehicleType)  ;
	void clearVehicleType() { m_VehicleType.clear() ;};
	std::vector<CString>* GetVehicleType() { return &m_VehicleType ;} ;

	void AddPoolId(int _id) ;
	void DeletePoolID(int _id) ;
	void clearVehiclePool(){ m_PoolID.clear() ;} ;
	const std::vector<int>& GetPoolIdList()const {return m_PoolID;}
public:
	BOOL CheckVehicleType(CString& _vehicleType) ;
	BOOL CheckPoolId(int _id) ;
	BOOL CheckServerFlightID(AirsideFlightDescStruct& _constrint) ;
protected:
	int m_SubType ;
public:
	void setSubType(int _type) { m_SubType = _type ;} ;
	int GetSubType(){ return m_SubType ;} ;
protected:
	int m_SelVehicleType ; //for summary report , the vehicle type selected
protected:
	std::map<int,int> m_VehicleExist ; 
public:
	void AddVehicleTypeForShow(int _vehiclety) {  m_VehicleExist[_vehiclety] ;};
	std::vector<int> GetExistVehicleType() 
	{

		std::vector<int> m_data ;
		std::map<int,int>::iterator iter = m_VehicleExist.begin() ;
		for ( ; iter != m_VehicleExist.end() ;iter++)
		{
			m_data.push_back((*iter).first) ;
		}
		return m_data ;
	}
public:
	void SetVehicleType(int _vehicle) { m_SelVehicleType = _vehicle ;};
	int GetSelVehicleType() { return m_SelVehicleType ;} ; 

public:
	virtual BOOL ExportFile(ArctermFile& _file) ;
	virtual BOOL ImportFile(ArctermFile& _file) ;
};
