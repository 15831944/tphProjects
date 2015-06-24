#pragma once



#include "Inputs/IN_TERM.h"
#include "Common/elaptime.h"
#include "InputAirside/ALTObjectGroup.h"

#include "Database/DBElement.h"
#include "Database/DBElementCollection.h"
#include "InputAirside/VehicleSpecificationItem.h"

#include "Common/FLT_CNST.h"
#include "Inputs/MobileElemConstraint.h"


#include <CommonData/procid.h>


#define CALLOUT_FACILITY_VERSION			1


class CTermPlanDoc;
class CPassengerType;
class CalloutDispSpecDataItem : public DBElement
{
public:
	CalloutDispSpecDataItem();
	~CalloutDispSpecDataItem();

	// enumeration types' and strings' definition
	enum EnvironmentModeLimit
	{
		EnvModeMaxLimit = EnvMode_All, // set as EnvironmentMode max limit
		// refer to enumeration type EnvironmentMode in IN_TERM.h
	};
	static CONST TCHAR* CONST m_sEnvModeStrings[];
	static CONST int m_sFacilityStart[];
	static CONST int m_sFacilityEnd[];


	enum FacilityType
	{
		Facility_AllStart = 0,
		//
		// Airside
		//
		Facility_AirsideStart = Facility_AllStart,

		Facility_WayPoint = Facility_AirsideStart,
		Facility_Hold,
		Facility_Runway,
		Facility_TakeoffPosition,
		Facility_Exit,
		Facility_TaxiwaySegment,
		Facility_TaxiwayIntersection,
		Facility_TaxiwayInterruptLine,
		Facility_StartPosition,
		Facility_Stand,
		Facility_DeicePad,
		Facility_RoadwaySegment_Airside,
		Facility_RoadwayIntersection_Airside,
		Facility_RoundAbout,
		Facility_ControlDevice_Airside,
		Facility_ReportAreaForFlightType,
		Facility_ReportAreaForGSEType,

		Facility_AirsideEnd,


		//
		// Landside
		//
		Facility_LandsideStart = Facility_AirsideEnd,

		Facility_RoadwaySegment_Landside = Facility_LandsideStart,
		Facility_RoadwayIntersection_Landside,
		Facility_ControlDevice_Landside,
		Facility_ParkingLot,
		Facility_Area_LandSide,

		Facility_LandsideEnd,


		//
		// Terminal
		//
		Facility_TerminalStart = Facility_LandsideEnd,

		Facility_Processor = Facility_LandsideEnd,
		Facility_Area_Terminal,
		Facility_Portal,

		Facility_TerminalEnd,


		//
		// Onboard
		//
		Facility_OnboardStart = Facility_TerminalEnd,

		Facility_Aircraft = Facility_OnboardStart,
		Facility_EntryExitDoor,
		Facility_StorageDevice,
		Facility_VerticalDevices,

		Facility_OnboardEnd,

		Facility_TypeCount = Facility_OnboardEnd,
	};
	static CONST TCHAR* CONST m_sFacilityStrings[];

	class FacilityData
	{
	public:
		FacilityData(FacilityType facility = Facility_WayPoint, CString strFacilityDB = _T(""));
		FacilityData(const FacilityData& facilityData);


		void SetFacility(FacilityType facility = Facility_WayPoint, CString strFacilityDB = _T(""));

		void InitFromDBString(InputTerminal* pInTerminal);

		FacilityType GetFacilityType() const { return m_enumFacilityType; }
		CString GetFacilityDBString() const { return m_strFacilityDB; }


		CString GetFacilityText() const;
		CString GetFacilityTextBase() const;

		static ALTOBJECT_TYPE m_sALTObjectGroupTypeMapping[];
		static bool IsALTObjectGroup(FacilityType facility);
		bool IsALTObjectGroup() const;


		bool CopyData(const FacilityData& rhs);

		// when facility is processor
		ProcessorID GetProcessorID() const;
		void SetProcessorID(const ProcessorID& proID);

		// when facility is an ALTObjectGroup, for example Waypoint, Runway, etc
		ALTObjectGroup GetALTObjectGroup() const;
		void SetALTObjectGroup(ALTObjectGroup val);
		void SetALTObjectGroupID( int altobjGroupID );

		// when facility is area
		CString GetAreaName_Terminal() const;
		void SetAreaName_Terminal(CString val);

		// when facility is area
		CString GetPortalName() const;
		void SetPortalName(CString val);

	protected:
		void RefreshDBString();

	private:
		// facility type
		FacilityType m_enumFacilityType;
		// facility DB string
		// will be interpreted differently depends on m_enumFacilityType
		CString m_strFacilityDB;

		// data
		// when adding a type of data, please implement the data access methods
		// and add your code to following 4 methods
		// GetFacilityTextBase()
		// CopyData()
		// RefreshDBString()
		// InitFromDBString()
		ProcessorID m_processorID;
		ALTObjectGroup m_altobjGroup;
		CString m_strAreaName_Terminal;
		CString m_strPortalName;


	private:
		FacilityData& operator=(const FacilityData& rhs); // forbid it, to use CopyData
	};


	enum ClientType
	{
		Client_FlightType = 0, // Airside
		Client_GSEType,

		Client_VehicleType, // Lanside

		Client_MobileElementType, // Terminal

		Client_MobileElement, // Onboard
		Client_NonPaxMobileElement,
	};
	static CONST TCHAR* CONST m_sClientTypeStrings[];
	static CONST ClientType m_sClientOfFacility[];



	enum StartTimeMode
	{
		StartTimeMode_BoardingStart = 0,
		StartTimeMode_AircraftParked,
		StartTimeMode_Absolute,

		StartTimeMode_TypeCount,
	};
	static CONST TCHAR* CONST m_sStartTimeModeStrings[];


	enum EndTimeMode
	{
		EndTimeMode_ACPushBack = 0,
		EndTimeMode_ACVacated,
		EndTimeMode_Absolute,

		EndTimeMode_TypeCount,
	};
	static CONST TCHAR* CONST m_sEndTimeModeStrings[];

	enum MessureType
	{
		Throughput = 0,
		NumberInHold,
		AverageHoldTime,
		NumberInTakeoffQs,
		NumberInSequenceForLanding,
		NumberInQ,
		MinAvgMaxDelay,
		PercentOfLandingsOnRunway,
		RunwayCrossings,
		NumberOnSegment,
		ConflictsResolved,
		NumberACInboundOnGround,
		MinAvgMaxServiceTime,
		OccupancyCurrentAC,
		MinAvgMaxOccupancy,
		ACInQ,
		DeiceFluisUsed,
		AntiIceFluidUsed,
		TimeAtPad,
		MinAvgMaxTimeAtPad,
		NumberGSEOnSegment_TwoWayPerLane,
		NumberGSEInboundDirectionTotal,
		NumberACInArea,
		NumberGSEInArea,
		ThroughputDirectionLaneTotalDirection,
		NumberVehiclesOnSegment,
		MinAvgMaxSpeed,
		NumberPaxOnSegment,
		NumberVehiclesInboundDirectionTotal,
		NumberVehiclesInLot,
		ParkingsSpotsVacant,
		PercentSpaceUtilized,
		MinAvgMaxDwellTime,
		NumberPaxInLot_PaxType,
		NumberVehiclesInArea,
		QLength,
		Population_QService,
		MinAvgMaxTimeInService,
		CurrentStatus_BusyIdle,
		NumberInArea,
		NumberInboundToPortal,
		NumberInAC,
		AircraftSeatFilledEmpty,
		TimeSinceBoardingStarted,
		ThroughputSinceStart,
		NumberInboundToDoor,
		NumberOfItemsInDevice,
		VolumeOccupiedRemaining,
		NumberOfEvents,
		NumberInboundToDevice,


		Messure_TypeCount,
	};
	static CONST TCHAR* CONST m_sMessureTypeStrings[];
	enum
	{
		MessureMappingArrayMaxCount = 7,
	};
	static CONST int m_sMessureTypeOfFacilities[][MessureMappingArrayMaxCount];

	enum ConnectionType
	{
		Connection_Single = 0,
		Connection_Family,

		Connection_TypeCount,
	};
	static CONST TCHAR* CONST m_sConnectionTypeStrings[];


	// DB
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;


	// data member access
	bool GetEnabled() const { return m_bEnabled; }
	void SetEnabled(bool val) { m_bEnabled = val; }

	EnvironmentMode GetEnvMode() const { return m_enumEnvMode; }
// 	void SetEnvMode(EnvironmentMode val) { m_enumEnvMode = val; } // discarded, use ChangeEnvMode instead

	FacilityType GetFacilityType() const { return m_facilityData.GetFacilityType(); }
//	void SetFacilityType(FacilityType val) { m_enumFacilityType = val; } // discarded, use ChangeFacilityType instead

	FacilityData GetFacilityData() const { return m_facilityData; }
	void SetFacilityData(const FacilityData& val) { m_facilityData.CopyData(val); }

	CString GetFacilityText() const { return m_facilityData.GetFacilityText(); }
	CString GetFacilityTextBase() const { return m_facilityData.GetFacilityTextBase(); }

	//------------------------START------------------------------------
	// access the client
	// must call RefreshCientFromData before accessing
	ClientType GetClientType() const { return GetClientType(GetFacilityType()); }

	CString GetClientText() const;
	CString GetClientTextBase() const;

	FlightConstraint GetFltType() const { return m_fltType; }
	void SetFltType(FlightConstraint fltType) { m_fltType = fltType; FormatFltTypeDBString(); }

	int GetGSEID() const { return m_nGSETypeID; }
	void SetGSEID(int val) { m_nGSETypeID = val; InitGSE(); }
	CVehicleSpecificationItem GetGSEItem() const { return m_GSEType; }

	CMobileElemConstraint GetMobileElementType() const { return *m_pMobileElementType; }
	void SetMobileElementType(CMobileElemConstraint val) { *m_pMobileElementType = val; FormatMobileElementTypeDBString(); }

	CPassengerType* GetOnboardPaxType() const { return m_pOnboardPaxType; }
	void SetOnboardPaxType(CPassengerType val);

	void RefreshCientFromData(CTermPlanDoc* pTermPlanDoc);
	//-------------------------END-----------------------------------

	StartTimeMode GetStartTimeMode() const { return m_enumStartTimeMode; }
	void SetStartTimeMode(StartTimeMode val) { m_enumStartTimeMode = val; }
	ElapsedTime GetStartTime() const { return m_startTime; }
	void SetStartTime(ElapsedTime val) { m_startTime = val; }
	CString GetStartTimeString() const;

	EndTimeMode GetEndTimeMode() const { return m_enumEndTimeMode; }
	void SetEndTimeMode(EndTimeMode val) { m_enumEndTimeMode = val; }
	ElapsedTime GetEndTime() const { return m_endTime; }
	void SetEndTime(ElapsedTime val) { m_endTime = val; }
	CString GetEndTimeString() const;

	ElapsedTime GetIntervalTime() const { return m_intervalTime; }
	void SetIntervalTime(ElapsedTime val) { m_intervalTime = val; }

	MessureType GetMessure() const { return m_enumMessure; }
	void SetMessure(MessureType val) { m_enumMessure = val; }

	ConnectionType GetConnection() const { return m_enumConnection; }
	void SetConnection(ConnectionType val) { m_enumConnection = val; }


	void ChangeEnvMode(EnvironmentMode envMode);
	void ChangeFacilityType(FacilityType facility);

	bool CheckValid(CString& strError) const;

protected:
	static bool CheckEnvMode(EnvironmentMode envMode);
	static bool CheckFacility(EnvironmentMode envMode, FacilityType facility);
	static ClientType GetClientType(FacilityType facility);
	static bool CheckMessureType(FacilityType facility, MessureType messure);

	void InitFltType( CAirportDatabase* pAirportDB );
	void InitGSE();
	void InitMobileElement(InputTerminal* pInTerminal);
	void InitOnboardPaxType(InputTerminal* pInTerminal);
	void FormatFltTypeDBString();
	void FormatMobileElementTypeDBString();
private:
	bool m_bEnabled;
	EnvironmentMode m_enumEnvMode;

	FacilityData m_facilityData;
	//---------------------------START---------------------------------
	// client
	// will be interpreted differently depends on m_enumFacilityType
	// refer to m_sClientOfFacility and GetClientType() for the mapping
	CString m_strFltType;
	int     m_nGSETypeID;
	CString m_strMobileElementType;
	CString m_strOnboardPaxTypeForDB;

	FlightConstraint m_fltType; // CAirportDatabase
	CVehicleSpecificationItem m_GSEType; // ...
	CMobileElemConstraint* m_pMobileElementType;// InputTerminal
	CPassengerType* m_pOnboardPaxType;
	//----------------------------END--------------------------------

	StartTimeMode m_enumStartTimeMode; // valid only if m_enumTimeMode is set to TimeMode_DependedToSystem
	EndTimeMode   m_enumEndTimeMode; // valid only if m_enumTimeMode is set to TimeMode_DependedToSystem
	ElapsedTime   m_startTime;
	ElapsedTime   m_endTime;
	ElapsedTime   m_intervalTime;

	MessureType    m_enumMessure; // refer to m_sMessureTypeOfFacilities to get which value can be set according to FacilityType
	ConnectionType m_enumConnection;
};


class CalloutDispSpecDataList : public DBElementCollection<CalloutDispSpecDataItem>
{
public:
	CalloutDispSpecDataList();

	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

	void RefreshCientFromData(CTermPlanDoc* pTermPlanDoc);
};

class CalloutDispSpecItem : public DBElement
{
public:
	CalloutDispSpecItem();

	// DB
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nProjID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	void SaveData(int nProjID);
	void DeleteData();

	// data member access
	CString GetName() const { return m_strName; }
	void SetName(CString val) { m_strName = val; }
	CalloutDispSpecDataList& GetData() { return m_data; }

	bool CheckValid(CString& strError) const;
private:
	CString m_strName;
	CalloutDispSpecDataList m_data;
};


class CalloutDispSpecs : public DBElementCollection<CalloutDispSpecItem>
{
public:
	CalloutDispSpecs();

	virtual void GetSelectElementSQL(int nProjID,CString& strSQL) const;
	bool CheckValid(CString& strError) const;
	void ReadData(int nProjID, CTermPlanDoc* pTermPlanDoc);

protected:
	virtual void ReadData(int nProjID); // to use void ReadData(int nProjID, CTermPlanDoc* pTermPlanDoc);
	void RefreshCientFromData(CTermPlanDoc* pTermPlanDoc);
};

