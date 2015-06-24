
/********************************************************************
	created:	2009/10/08
	created:	8:10:2009   13:47
	filename:	CalloutDispSpec.cpp
	author:		Benny
	
	purpose:	
*********************************************************************/

#include "stdafx.h"


//??? Benny Tag
#include <Database/DBElementCollection_Impl.h>
//???

#include "CalloutDispSpec.h"

#include "TermPlanDoc.h"
#include "Common/NewPassengerType.h"

template  DBElementCollection<CalloutDispSpecDataItem>;
template  DBElementCollection<CalloutDispSpecItem>;

CalloutDispSpecDataItem::CalloutDispSpecDataItem()
	: m_bEnabled(true)
	, m_enumEnvMode(EnvMode_AirSide)
	, m_nGSETypeID(-1)
	, m_enumStartTimeMode(StartTimeMode_Absolute)
	, m_enumEndTimeMode(EndTimeMode_Absolute)
	, m_enumMessure(Throughput)
	, m_startTime(0L)
	, m_endTime(24*60*60L - 1L)
	, m_intervalTime(10*60L) // 10 minutes
	, m_enumConnection(Connection_Single)
	, m_pOnboardPaxType(NULL)
{
	m_pMobileElementType = new CMobileElemConstraint;
}

CalloutDispSpecDataItem::~CalloutDispSpecDataItem()
{
	if (m_pMobileElementType)
	{
		delete m_pMobileElementType;
	}

	if (m_pOnboardPaxType)
	{
		delete m_pOnboardPaxType;
		m_pOnboardPaxType = NULL;
	}
}


CONST TCHAR* CONST CalloutDispSpecDataItem::m_sEnvModeStrings[EnvModeMaxLimit] =
{
	_T("Airside"),
	_T("Lanside"),
	_T("Terminal"),
	_T("Onboard"),
};

CONST int CalloutDispSpecDataItem::m_sFacilityStart[EnvModeMaxLimit] =
{
	Facility_AirsideStart,
	Facility_LandsideStart,
	Facility_TerminalStart,
	Facility_OnboardStart,
};

CONST int CalloutDispSpecDataItem::m_sFacilityEnd[EnvModeMaxLimit] =
{
	Facility_AirsideEnd,
	Facility_LandsideEnd,
	Facility_TerminalEnd,
	Facility_OnboardEnd,
};

CONST TCHAR* CONST CalloutDispSpecDataItem::m_sFacilityStrings[Facility_TypeCount] =
{
	// Airside
	_T("Waypoint"),
	_T("Hold"),
	_T("Runway"),
	_T("Takeoff Position"),
	_T("Exit"),
	_T("Taxiway Segment"),
	_T("Taxiway Intersection"),
	_T("Taxiway Interrupt Line"),
	_T("Starting Positions"),
	_T("Stand"),
	_T("Deice Pad"),
	_T("Roadway Segment"),
	_T("Roadway Intersection"),
	_T("Roundabout"),
	_T("Control Device"),
	_T("Reporting Area(for Flight Type)"),
	_T("Reporting Area(for GSE Type)"),

	// Lanside
	_T("Roadway Segment"),
	_T("Roadway Intersection"),
	_T("Control Device"),
	_T("Parking Lot"),
	_T("Area"),

	// Terminal
	_T("Processor"),
	_T("Area"),
	_T("Portal"),

	// Onboard
	_T("Aircraft"),
	_T("Entry/Exit Door"),
	_T("Storage Device"),
	_T("Vertical Devices"),
};


CONST TCHAR* CONST CalloutDispSpecDataItem::m_sClientTypeStrings[] =
{
	_T("Flight Type"),
	_T("GSE Type"),
	_T("Vehicle Type"),
	_T("Mobile Element Type"),
	_T("Mobile Element"),
	_T("Non-Passenger Mobile Element"),
};

CONST CalloutDispSpecDataItem::ClientType CalloutDispSpecDataItem::m_sClientOfFacility[Facility_TypeCount] =
{
	Client_FlightType, // Airside
	Client_FlightType,
	Client_FlightType,
	Client_FlightType,
	Client_FlightType,
	Client_FlightType,
	Client_FlightType,
	Client_FlightType,
	Client_FlightType,
	Client_FlightType,
	Client_FlightType,
	Client_GSEType,
	Client_GSEType,
	Client_GSEType,
	Client_GSEType,
	Client_FlightType,
	Client_GSEType,

	Client_VehicleType, // Lanside
	Client_VehicleType,
	Client_VehicleType,
	Client_VehicleType,
	Client_VehicleType,

	Client_MobileElementType, // Terminal
	Client_MobileElementType,
	Client_MobileElementType,

	Client_MobileElement, // Onboard
	Client_MobileElement,
	Client_NonPaxMobileElement,
	Client_MobileElement,
};


CONST TCHAR* CONST CalloutDispSpecDataItem::m_sStartTimeModeStrings[StartTimeMode_TypeCount] =
{
	_T("Boarding Start"),
	_T("Aircraft Parked"),
	_T("Absolute Time"),
};

CONST TCHAR* CONST CalloutDispSpecDataItem::m_sEndTimeModeStrings[EndTimeMode_TypeCount] =
{
	_T("AC Push Back"),
	_T("AC Vacated"),
	_T("Absolute Time"),
};


CONST TCHAR* CONST CalloutDispSpecDataItem::m_sConnectionTypeStrings[Connection_TypeCount] =
{
	_T("Single"),
	_T("Family"),
};


CONST TCHAR* CONST CalloutDispSpecDataItem::m_sMessureTypeStrings[Messure_TypeCount] =
{
	_T("Thoughput"),
	_T("Number in Hold"),
	_T("Average Hold Time"),
	_T("Number in Takeoff Queues"),
	_T("Number in Sequence for Landing"),
	_T("Number in Queue"),
	_T("Min, Average, Max Delay"),
	_T("Percent of Landings on Runway"),
	_T("Runway Crossings"),
	_T("Number on Segment"),
	_T("Conflicts Resolved"),
	_T("Number of AC Inbound(on Ground)"),
	_T("Min, Average, Max Service Time"),
	_T("Occupancy Current AC"),
	_T("Min, Average, Max Occupancy"),
	_T("AC in Queue"),
	_T("Deice Fluis Used"),
	_T("Anti-Ice Fluid Used"),
	_T("Time at Pad"),
	_T("Min, Average, Max Time at Pad"),
	_T("Number of GSE on Segment(Two Way, per Lane)"),
	_T("Number of GSE Inbound/Direction & Total"),
	_T("Number of AC in Area"),
	_T("Number of GSE in Area"),
	_T("Throughput/Direction/Lane & Total/Direction"),
	_T("Number of Vehicles on Segment"),
	_T("Min, Average, Max Speed"),
	_T("Number of Passenger on Segment"),
	_T("Number of Vehicles Inbound/Direction & Total"),
	_T("Number of Vehicles in Lot"),
	_T("Parking Spots Vacant"),
	_T("Percent of Space Utilized"),
	_T("Min, Average, Max Dwell Time"),
	_T("Number of Passenger in Lot(Passenger Type)"),
	_T("Number of Vehicles in Area"),
	_T("Queue Length"),
	_T("Population(Queue + Service)"),
	_T("Min, Average, Max Time in Service"),
	_T("Current Status(Busy/Idle)"),
	_T("Number in Area"),
	_T("Number Inbound to Portal"),
	_T("Number in AC"),
	_T("Aircraft Seats Filled/Empty"),
	_T("Time since Boarding Started"),
	_T("Throughput since Start"),
	_T("Number Inbound to Door"),
	_T("Number of Items in Device(s)"),
	_T("Volume Occupied/Remaining"),
	_T("Number of Events"),
	_T("Number Inbound to Device"),
};

CONST int CalloutDispSpecDataItem::m_sMessureTypeOfFacilities[Facility_TypeCount][MessureMappingArrayMaxCount] =
{
	// Airside
	/*Waypoint*/{ Throughput, -1 },
	/*Hold*/{ NumberInHold, AverageHoldTime, -1 },
	/*Runway*/{ Throughput, NumberInTakeoffQs, NumberInSequenceForLanding, -1 },
	/*Takeoff Position*/{ Throughput, NumberInQ, MinAvgMaxDelay, -1 },
	/*Exit*/{ Throughput, PercentOfLandingsOnRunway, RunwayCrossings, -1 },
	/*Taxiway Segment*/{ Throughput, NumberOnSegment, MinAvgMaxDelay, -1 },
	/*Taxiway Intersection*/{ Throughput, ConflictsResolved, MinAvgMaxDelay, NumberACInboundOnGround, -1 },
	/*Taxiway Interrupt Line*/{ Throughput, MinAvgMaxServiceTime, NumberACInboundOnGround, -1 },
	/*Starting Positions*/{ Throughput, OccupancyCurrentAC, MinAvgMaxOccupancy, -1 },
	/*Stand*/{ Throughput, OccupancyCurrentAC, MinAvgMaxOccupancy, -1 },
	/*Deice Pad*/{ Throughput, ACInQ, DeiceFluisUsed, AntiIceFluidUsed, TimeAtPad, MinAvgMaxTimeAtPad, -1 },
	/*Roadway Segment*/{ Throughput, NumberGSEOnSegment_TwoWayPerLane, -1 },
	/*Roadway Intersection*/{ Throughput, NumberGSEInboundDirectionTotal, -1 },
	/*Roundabout*/{ Throughput, NumberGSEInboundDirectionTotal, -1 },
	/*Control Device*/{ Throughput, NumberGSEInboundDirectionTotal, NumberInQ, MinAvgMaxServiceTime, -1 },
	/*Reporting Area(for Flight Type*/{ Throughput, NumberACInArea, -1 },
	/*Reporting Area(for GSE Type)*/{ Throughput, NumberGSEInArea, -1 },

	// Landside
	/*Roadway Segment*/{ ThroughputDirectionLaneTotalDirection, NumberVehiclesOnSegment, MinAvgMaxSpeed, NumberPaxOnSegment, -1 },
	/*Roadway Intersection*/{ ThroughputDirectionLaneTotalDirection, NumberVehiclesInboundDirectionTotal, -1 },
	/*Control Device*/{ Throughput, NumberVehiclesInboundDirectionTotal, NumberInQ, MinAvgMaxServiceTime, -1 },
	/*Parking Lot*/{ NumberVehiclesInLot, ParkingsSpotsVacant, PercentSpaceUtilized, MinAvgMaxDwellTime, NumberPaxInLot_PaxType, -1 },
	/*Area*/{ Throughput, NumberVehiclesInArea, -1 },

	// Terminal
	/*Processor*/{ Throughput, QLength, Population_QService, MinAvgMaxTimeInService, CurrentStatus_BusyIdle, -1 },
	/*Area*/{ Throughput, NumberInArea, -1 },
	/*Portal*/{ Throughput, NumberInboundToPortal, -1 },

	// Onboard
	/*Aircraft*/{ NumberInAC, AircraftSeatFilledEmpty, TimeSinceBoardingStarted, -1 },
	/*Entry/Exit Door*/{ ThroughputSinceStart, NumberInQ, NumberInboundToDoor, -1 },
	/*Storage Device*/{ NumberOfItemsInDevice, VolumeOccupiedRemaining, NumberOfEvents, -1 },
	/*Vertical Devices*/{ Throughput, NumberInboundToDevice, -1 },
};

void CalloutDispSpecDataItem::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"), m_nID);

	int nEnabled = 1;
	recordset.GetFieldValue(_T("ENABLED"), nEnabled);
	m_bEnabled = (0 != nEnabled);

	int nEnvMode = EnvMode_AirSide;
	recordset.GetFieldValue(_T("ENVMODE"), nEnvMode);
	m_enumEnvMode = (EnvironmentMode)nEnvMode;
	ASSERT(CheckEnvMode(m_enumEnvMode));

	int nFacilityType = Facility_WayPoint;
	recordset.GetFieldValue(_T("FACILITY"), nFacilityType);
	FacilityType enumFacilityType = (FacilityType)nFacilityType;
	ASSERT(CheckFacility(m_enumEnvMode, enumFacilityType));

	CString strFacilityDB;
	recordset.GetFieldValue(_T("FACILITY_DATA"), strFacilityDB);
	m_facilityData.SetFacility(enumFacilityType, strFacilityDB);

	ClientType clientType = GetClientType(enumFacilityType);
	switch (clientType)
	{
	case Client_FlightType:
		{
			recordset.GetFieldValue(_T("FLTTYPE"), m_strFltType);
		}
		break;
	case Client_GSEType:
		{
			recordset.GetFieldValue(_T("GSEID"), m_nGSETypeID);
			m_GSEType.ReadData(m_nGSETypeID);
		}
		break;
	case Client_VehicleType:
		{
			// not implemented
		}
		break;
	case Client_MobileElementType:
		{
			recordset.GetFieldValue(_T("MOBILETYPE"), m_strMobileElementType);
		}
		break;
	case Client_MobileElement:
		{
			recordset.GetFieldValue(_T("ONBOARDPAX"), m_strOnboardPaxTypeForDB);
			//InitOnboardPaxType();
		}
		break;
	case Client_NonPaxMobileElement:
		{
			// not implemented
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
	}

	if (EnvMode_OnBoard == m_enumEnvMode)
	{
		int nStartTimeMode = StartTimeMode_Absolute;
		recordset.GetFieldValue(_T("STARTTIMEMODE"), nStartTimeMode);
		m_enumStartTimeMode = (StartTimeMode)nStartTimeMode;
		ASSERT(m_enumStartTimeMode>=StartTimeMode_BoardingStart && m_enumStartTimeMode<StartTimeMode_TypeCount);

		int nEndTimeMode = EndTimeMode_Absolute;
		recordset.GetFieldValue(_T("ENDTIMEMODE"), nEndTimeMode);
		m_enumEndTimeMode = (EndTimeMode)nEndTimeMode;
		ASSERT(m_enumStartTimeMode>=EndTimeMode_ACPushBack && m_enumStartTimeMode<EndTimeMode_TypeCount);
	}

	long lStartTimeSeconds = 0;
	recordset.GetFieldValue(_T("STARTTIME"), lStartTimeSeconds);
	m_startTime.set(lStartTimeSeconds);

	long lEndTimeSeconds = 0;
	recordset.GetFieldValue(_T("ENDTIME"), lEndTimeSeconds);
	m_endTime.set(lEndTimeSeconds);

	long lIntervalSeconds = 0;
	recordset.GetFieldValue(_T("INTERVAL"), lIntervalSeconds);
	m_intervalTime.set(lIntervalSeconds);

	int nMessureType = Throughput;
	recordset.GetFieldValue(_T("MESSURETYPE"), nMessureType);
	m_enumMessure = (MessureType)nMessureType;
	ASSERT(CheckMessureType(m_facilityData.GetFacilityType(), m_enumMessure));

	int nConnectionType = Connection_Single;
	recordset.GetFieldValue(_T("CONNECTIONTYPE"), nConnectionType);
	m_enumConnection = (ConnectionType)nConnectionType;
	ASSERT(m_enumConnection>=Connection_Single && m_enumConnection<Connection_TypeCount);
}

void CalloutDispSpecDataItem::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO TB_CALLOUT_DISP_SPEC_DATA ")
		_T("(ENABLED, ENVMODE, FACILITY, FACILITY_DATA, FLTTYPE, GSEID, MOBILETYPE, ONBOARDPAX, STARTTIMEMODE, ENDTIMEMODE, STARTTIME, ENDTIME, [INTERVAL], MESSURETYPE, CONNECTIONTYPE, PARENTID) ")
		_T("VALUES (%d, %d, %d, '%s', '%s', %d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d)")
		, m_bEnabled ? 1 : 0
		, m_enumEnvMode
		, m_facilityData.GetFacilityType()
		, m_facilityData.GetFacilityDBString()
		, m_strFltType
		, m_nGSETypeID
		, m_strMobileElementType
		, m_strOnboardPaxTypeForDB
		, m_enumStartTimeMode
		, m_enumEndTimeMode
		, m_startTime.asSeconds()
		, m_endTime.asSeconds()
		, m_intervalTime.asSeconds()
		, m_enumMessure
		, m_enumConnection
		, nParentID
		);
}

void CalloutDispSpecDataItem::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE TB_CALLOUT_DISP_SPEC_DATA ")
		_T("SET ENABLED = %d, ENVMODE = %d, FACILITY = %d, FACILITY_DATA = '%s', FLTTYPE = '%s', GSEID = %d, MOBILETYPE = '%s', ONBOARDPAX = '%s', ")
		_T("STARTTIMEMODE = %d, ENDTIMEMODE = %d, STARTTIME = %d, ENDTIME = %d, [INTERVAL] = %d, ")
		_T("MESSURETYPE = %d, CONNECTIONTYPE = %d ")
		_T("WHERE (ID = %d)")
		, m_bEnabled ? 1 : 0
		, m_enumEnvMode
		, m_facilityData.GetFacilityType()
		, m_facilityData.GetFacilityDBString()
		, m_strFltType
		, m_nGSETypeID
		, m_strMobileElementType
		, m_strOnboardPaxTypeForDB
		, m_enumStartTimeMode
		, m_enumEndTimeMode
		, m_startTime.asSeconds()
		, m_endTime.asSeconds()
		, m_intervalTime.asSeconds()
		, m_enumMessure
		, m_enumConnection
		, m_nID
		);
}

void CalloutDispSpecDataItem::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM TB_CALLOUT_DISP_SPEC_DATA WHERE (ID = %d)")
		, m_nID
		);
}

bool CalloutDispSpecDataItem::CheckEnvMode( EnvironmentMode envMode )
{
	return envMode>=EnvMode_AirSide && envMode<EnvModeMaxLimit;
}

bool CalloutDispSpecDataItem::CheckFacility( EnvironmentMode envMode, FacilityType facility )
{
	return CheckEnvMode(envMode) ? (facility>=m_sFacilityStart[envMode] && facility<m_sFacilityEnd[envMode]) : false;
}

CalloutDispSpecDataItem::ClientType CalloutDispSpecDataItem::GetClientType( FacilityType facility )
{
	ASSERT(facility>=Facility_AllStart && facility<Facility_TypeCount);
	return m_sClientOfFacility[facility];
}

bool CalloutDispSpecDataItem::CheckMessureType( FacilityType facility, MessureType messure )
{
	if (facility>=Facility_AllStart && facility<Facility_TypeCount)
	{
		for(int i=0;i<MessureMappingArrayMaxCount;i++)
		{
			if (m_sMessureTypeOfFacilities[facility][i] == -1)
			{
				break;
			}
			if (m_sMessureTypeOfFacilities[facility][i] == (int)messure)
			{
				return true;
			}
		}
	}
	return false;
}

void CalloutDispSpecDataItem::ChangeEnvMode( EnvironmentMode envMode )
{
	if ( FALSE == CheckEnvMode(envMode))
	{
		ASSERT(FALSE);
		return;
	}
	if (envMode == m_enumEnvMode)
	{
		return;
	}
	m_enumEnvMode = envMode;

	switch (m_enumEnvMode)
	{
	case EnvMode_AirSide:
		{
			m_facilityData.SetFacility(Facility_WayPoint);
			m_enumStartTimeMode = StartTimeMode_Absolute;
			m_enumEndTimeMode = EndTimeMode_Absolute;
			m_enumMessure = Throughput;
		}
		break;
	case EnvMode_LandSide:
		{
			m_facilityData.SetFacility(Facility_RoadwaySegment_Landside);
			m_enumStartTimeMode = StartTimeMode_Absolute;
			m_enumEndTimeMode = EndTimeMode_Absolute;
			m_enumMessure = ThroughputDirectionLaneTotalDirection;
		}
		break;
	case EnvMode_Terminal:
		{
			m_facilityData.SetFacility(Facility_Processor);
			m_enumStartTimeMode = StartTimeMode_Absolute;
			m_enumEndTimeMode = EndTimeMode_Absolute;
			m_enumMessure = Throughput;
		}
		break;
	case EnvMode_OnBoard:
		{
			m_facilityData.SetFacility(Facility_Aircraft);
			m_enumStartTimeMode = StartTimeMode_BoardingStart;
			m_enumEndTimeMode = EndTimeMode_ACPushBack;
			m_enumMessure = NumberInAC;
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
}

void CalloutDispSpecDataItem::ChangeFacilityType( FacilityType facility )
{
	if (m_facilityData.GetFacilityType() == facility)
	{
		return;
	}
	if (false == CheckFacility(m_enumEnvMode, facility))
	{
		ASSERT(FALSE);
		return;
	}
	m_facilityData.SetFacility(facility);
	m_enumMessure = (CalloutDispSpecDataItem::MessureType)m_sMessureTypeOfFacilities[facility][0];
}

void CalloutDispSpecDataItem::InitFltType( CAirportDatabase* pAirportDB )
{
	m_fltType.SetAirportDB(pAirportDB);
	if (!m_strFltType.IsEmpty())
	{
		m_fltType.setConstraintWithVersion(m_strFltType);
	}
	FormatFltTypeDBString();
}

void CalloutDispSpecDataItem::InitGSE()
{
	m_GSEType.ReadData(m_nGSETypeID);
}

void CalloutDispSpecDataItem::InitMobileElement(InputTerminal* pInTerminal)
{
	if (m_pMobileElementType)
	{
		delete m_pMobileElementType;
		m_pMobileElementType = NULL;
	}

	m_pMobileElementType = new CMobileElemConstraint(pInTerminal);
	if (!m_strMobileElementType.IsEmpty())
	{
		m_pMobileElementType->setConstraintWithVersion(m_strMobileElementType);
	}

	FormatMobileElementTypeDBString();
}

void CalloutDispSpecDataItem::InitOnboardPaxType(InputTerminal* pInTerminal)
{
	if (m_pOnboardPaxType)
	{
		delete m_pOnboardPaxType;
		m_pOnboardPaxType = NULL;
	}
	m_pOnboardPaxType = new CPassengerType(pInTerminal->inStrDict);
	m_pOnboardPaxType->FormatLevelPaxTypeFromString(m_strOnboardPaxTypeForDB);
}

void CalloutDispSpecDataItem::FormatFltTypeDBString()
{
	TCHAR szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	m_strFltType = szFltType;
}

void CalloutDispSpecDataItem::FormatMobileElementTypeDBString()
{
	ASSERT(m_pMobileElementType);
	TCHAR szMobileTlementType[1024] = {0};
	m_pMobileElementType->WriteSyntaxStringWithVersion(szMobileTlementType);
	m_strMobileElementType = szMobileTlementType;
}

void CalloutDispSpecDataItem::RefreshCientFromData( CTermPlanDoc* pTermPlanDoc )
{
	ASSERT(pTermPlanDoc);
	InputTerminal* pInTerminal = &pTermPlanDoc->GetTerminal();

	InitFltType(pInTerminal->m_pAirportDB);

	InitMobileElement(pInTerminal);

	InitOnboardPaxType(pInTerminal);

	m_facilityData.InitFromDBString(pInTerminal);
}

CString CalloutDispSpecDataItem::GetClientText() const
{
	CString strData = GetClientTextBase();
	if (strData.IsEmpty())
	{
		strData = _T("(N/A)");
	}
	CString strText;
	strText.Format(_T("%s: %s"), m_sClientTypeStrings[GetClientType()], strData);
	return strText;
}

CString CalloutDispSpecDataItem::GetClientTextBase() const
{
	CString strData;
	switch (GetClientType())
	{
	case CalloutDispSpecDataItem::Client_FlightType:
		{
			m_fltType.screenPrint(strData);
		}
		break;
	case CalloutDispSpecDataItem::Client_GSEType:
		{
			strData = m_GSEType.getName();
		}
		break;
	case CalloutDispSpecDataItem::Client_VehicleType:
		{
			// not implemented
		}
		break;
	case CalloutDispSpecDataItem::Client_MobileElementType:
		{
			m_pMobileElementType->screenPrint(strData);
		}
		break;
	case CalloutDispSpecDataItem::Client_MobileElement:
		{
			strData = m_pOnboardPaxType->PrintStringForShow();
		}
		break;
	case CalloutDispSpecDataItem::Client_NonPaxMobileElement:
		{
			// not implemented
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
	return strData;
}

CString CalloutDispSpecDataItem::GetStartTimeString() const
{
	if (StartTimeMode_Absolute == m_enumStartTimeMode)
	{
		return m_startTime.PrintDateTime();
	}
	return m_sStartTimeModeStrings[m_enumStartTimeMode];
}

CString CalloutDispSpecDataItem::GetEndTimeString() const
{
	if (EndTimeMode_Absolute == m_enumEndTimeMode)
	{
		return m_endTime.PrintDateTime();
	}
	return m_sEndTimeModeStrings[m_enumEndTimeMode];
}

bool CalloutDispSpecDataItem::CheckValid( CString& strError ) const
{
	switch (GetClientType())
	{
	case CalloutDispSpecDataItem::Client_FlightType:
		{
			//
		}
		break;
	case CalloutDispSpecDataItem::Client_GSEType:
		{
			//
			if (m_nGSETypeID<0)
			{
				strError = _T("GSE Type is not valid");
				return false;
			}
		}
		break;
	case CalloutDispSpecDataItem::Client_VehicleType:
		{
			// not implemented
		}
		break;
	case CalloutDispSpecDataItem::Client_MobileElementType:
		{
			//
		}
		break;
	case CalloutDispSpecDataItem::Client_MobileElement:
		{
			// not implemented
		}
		break;
	case CalloutDispSpecDataItem::Client_NonPaxMobileElement:
		{
			// note implemented
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
	if (m_intervalTime<=0L)
	{
		strError = _T("The interval time can not be zero");
		return false;
	}
	return true;
}

void CalloutDispSpecDataItem::SetOnboardPaxType( CPassengerType val )
{
	m_pOnboardPaxType->copyData(val) ; 
	m_strOnboardPaxTypeForDB = val.GetStringForDatabase();
}
CalloutDispSpecDataList::CalloutDispSpecDataList()
{

}


void CalloutDispSpecDataList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM TB_CALLOUT_DISP_SPEC_DATA WHERE (PARENTID = %d)")
		, nParentID
		);
}

void CalloutDispSpecDataList::RefreshCientFromData( CTermPlanDoc* pTermPlanDoc )
{
	size_t nCount = GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		GetItem(i)->RefreshCientFromData(pTermPlanDoc);
	}
}

CalloutDispSpecItem::CalloutDispSpecItem()
{

}

void CalloutDispSpecItem::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("NAME"), m_strName);

	m_data.ReadData(m_nID);
}

void CalloutDispSpecItem::GetInsertSQL( int nProjID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO TB_CALLOUT_DISP_SPECS (NAME, PROJID) VALUES ('%s', %d)")
		, m_strName
		, nProjID
		);
}

void CalloutDispSpecItem::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE TB_CALLOUT_DISP_SPECS SET NAME = '%s' WHERE (ID = %d)")
		, m_strName
		, m_nID
		);
}

void CalloutDispSpecItem::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM TB_CALLOUT_DISP_SPECS WHERE (ID = %d)")
		, m_nID
		);
}

void CalloutDispSpecItem::SaveData( int nProjID )
{
	DBElement::SaveData(nProjID);
	m_data.SaveData(m_nID);
}

void CalloutDispSpecItem::DeleteData()
{
	m_data.DeleteData();
	m_data.SaveData(-1/*do not care*/);
	DBElement::DeleteData();
}

bool CalloutDispSpecItem::CheckValid( CString& strError ) const
{
	CString strTmp;
	size_t nCount = m_data.GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		if (false == m_data.GetItem(i)->CheckValid(strTmp))
		{
			strError.Format(_T("One item of Callout Display Specification with name %s: %s."), m_strName, strTmp);
			return false;
		}
	}
	return true;
}
CalloutDispSpecs::CalloutDispSpecs()
{

}

void CalloutDispSpecs::GetSelectElementSQL( int nProjID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM TB_CALLOUT_DISP_SPECS WHERE (PROJID = %d)")
		, nProjID
		);
}

void CalloutDispSpecs::ReadData( int nParentID )
{
	CleanData();
	for(size_t i=0;i< m_deleteddataList.size();i++){
		delete m_deleteddataList[i];
	}
	m_deleteddataList.clear();

	DBElementCollection<CalloutDispSpecItem>::ReadData(nParentID);

	// add default data if necessary
}

void CalloutDispSpecs::ReadData( int nParentID, CTermPlanDoc* pTermPlanDoc )
{
	ASSERT(pTermPlanDoc);
	ReadData(nParentID);
	RefreshCientFromData(pTermPlanDoc);
}
void CalloutDispSpecs::RefreshCientFromData( CTermPlanDoc* pTermPlanDoc )
{
	size_t nCount = GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		GetItem(i)->GetData().RefreshCientFromData(pTermPlanDoc);
	}
}

bool CalloutDispSpecs::CheckValid( CString& strError ) const
{
	size_t nCount = GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		if (false == GetItem(i)->CheckValid(strError))
		{
			return false;
		}
	}
	return true;
}

template DBElementCollection<CalloutDispSpecDataItem>;
template DBElementCollection<CalloutDispSpecItem>;

CalloutDispSpecDataItem::FacilityData::FacilityData( FacilityType facility /*= Facility_WayPoint*/, CString strFacilityDB /*= _T("")*/ )
{
	SetFacility(facility, strFacilityDB);
}

CalloutDispSpecDataItem::FacilityData::FacilityData( const FacilityData& facilityData )
{
	SetFacility(facilityData.GetFacilityType());

	CopyData(facilityData);
}

void CalloutDispSpecDataItem::FacilityData::SetFacility( FacilityType facility /*= Facility_WayPoint*/, CString strFacilityDB /*= _T("")*/ )
{
	m_enumFacilityType = facility;
	m_strFacilityDB = strFacilityDB;
	if (m_strFacilityDB.IsEmpty())
	{
		m_strFacilityDB.Format(_T("[Version=%d;Type=%s]"), CALLOUT_FACILITY_VERSION,
			CalloutDispSpecDataItem::m_sFacilityStrings[m_enumFacilityType]);
	}
}

ProcessorID CalloutDispSpecDataItem::FacilityData::GetProcessorID() const
{
	ASSERT(CalloutDispSpecDataItem::Facility_Processor == m_enumFacilityType);
	return m_processorID;
}

void CalloutDispSpecDataItem::FacilityData::SetProcessorID( const ProcessorID& proID )
{
	ASSERT(CalloutDispSpecDataItem::Facility_Processor == m_enumFacilityType);
	m_processorID = proID;

	RefreshDBString();
}

ALTOBJECT_TYPE CalloutDispSpecDataItem::FacilityData::m_sALTObjectGroupTypeMapping[CalloutDispSpecDataItem::Facility_AirsideEnd] =
{
	ALT_WAYPOINT,		// Facility_WayPoint
	ALT_HOLD,			// Facility_Hold
	ALT_RUNWAY,			// Facility_Runway
	ALT_UNKNOWN,		// Facility_TakeoffPosition
	ALT_UNKNOWN,		// Facility_Exit
	ALT_UNKNOWN,		// Facility_TaxiwaySegment
	ALT_UNKNOWN,		// Facility_TaxiwayIntersection
	ALT_UNKNOWN,		// Facility_TaxiwayInterruptLine
	ALT_STARTPOSITION,	// Facility_StartPosition
	ALT_STAND,			// Facility_Stand
	ALT_DEICEBAY,		// Facility_DeicePad
	ALT_UNKNOWN,		// Facility_RoadwaySegment_Airside
	ALT_UNKNOWN,		// Facility_RoadwayIntersection_Airside
	ALT_ROUNDABOUT,		// Facility_RoundAbout
	ALT_UNKNOWN,		// Facility_ControlDevice_Airside
	ALT_REPORTINGAREA,	// Facility_ReportAreaForFlightType
	ALT_REPORTINGAREA,	// Facility_ReportAreaForGSEType
};

bool CalloutDispSpecDataItem::FacilityData::IsALTObjectGroup(FacilityType facility)
{
	return facility<CalloutDispSpecDataItem::Facility_AirsideEnd && ALT_UNKNOWN != m_sALTObjectGroupTypeMapping[facility];
}
bool CalloutDispSpecDataItem::FacilityData::IsALTObjectGroup() const
{
	return IsALTObjectGroup(m_enumFacilityType);
}

ALTObjectGroup CalloutDispSpecDataItem::FacilityData::GetALTObjectGroup() const
{
	ASSERT(IsALTObjectGroup());
	return m_altobjGroup;
}

void CalloutDispSpecDataItem::FacilityData::SetALTObjectGroup( ALTObjectGroup val )
{
	ASSERT(IsALTObjectGroup());
	m_altobjGroup = val;
	RefreshDBString();
}

void CalloutDispSpecDataItem::FacilityData::SetALTObjectGroupID( int altobjGroupID )
{
	ASSERT(IsALTObjectGroup());
	if (-1 == altobjGroupID)
	{
		m_altobjGroup.setID(altobjGroupID);
	} 
	else
	{
		m_altobjGroup.ReadData(altobjGroupID);
	}
	RefreshDBString();
}


CString CalloutDispSpecDataItem::FacilityData::GetAreaName_Terminal() const
{
	ASSERT(CalloutDispSpecDataItem::Facility_Area_Terminal == m_enumFacilityType);
	return m_strAreaName_Terminal;
}

void CalloutDispSpecDataItem::FacilityData::SetAreaName_Terminal( CString val )
{
	ASSERT(CalloutDispSpecDataItem::Facility_Area_Terminal == m_enumFacilityType);
	m_strAreaName_Terminal = val;
	RefreshDBString();
}

CString CalloutDispSpecDataItem::FacilityData::GetPortalName() const
{
	ASSERT(CalloutDispSpecDataItem::Facility_Portal == m_enumFacilityType);
	return m_strPortalName;
}

void CalloutDispSpecDataItem::FacilityData::SetPortalName( CString val )
{
	ASSERT(CalloutDispSpecDataItem::Facility_Portal == m_enumFacilityType);
	m_strPortalName = val;
	RefreshDBString();
}

CString CalloutDispSpecDataItem::FacilityData::GetFacilityText() const
{
	CString strFacility = GetFacilityTextBase();
	if (strFacility.IsEmpty())
	{
		strFacility = _T("(N/A)");
	}
	CString strText;
	strText.Format(_T("%s: %s"), CalloutDispSpecDataItem::m_sFacilityStrings[GetFacilityType()], strFacility);
	return strText;
}

CString CalloutDispSpecDataItem::FacilityData::GetFacilityTextBase() const
{
	if (IsALTObjectGroup())
	{
		if (m_altobjGroup.getID() == -1)
		{
			return _T("All ") + ALTObject::GetTypeName(m_sALTObjectGroupTypeMapping[m_enumFacilityType]);
		} 
		else
		{
			return m_altobjGroup.getName().GetIDString();
		}
	}

	switch (m_enumFacilityType)
	{
	case CalloutDispSpecDataItem::Facility_Processor:
		{
			return m_processorID.GetIDString();
		}
		break;
	case CalloutDispSpecDataItem::Facility_Area_Terminal:
		{
			return m_strAreaName_Terminal;
		}
		break;
	case CalloutDispSpecDataItem::Facility_Portal:
		{
			return m_strPortalName;
		}
		break;
	default:
		{
			// 			ASSERT(FALSE);
		}
		break;
	}

	return _T("");
}

bool CalloutDispSpecDataItem::FacilityData::CopyData( const FacilityData& rhs )
{
	if (rhs.GetFacilityType() != m_enumFacilityType)
	{
		ASSERT(FALSE);
		return false;
	}

	if (IsALTObjectGroup())
	{
		m_altobjGroup = rhs.GetALTObjectGroup();
	}
	else
	{
		switch (m_enumFacilityType)
		{
		case CalloutDispSpecDataItem::Facility_Processor:
			{
				SetProcessorID(rhs.GetProcessorID());
			}
			break;
		case CalloutDispSpecDataItem::Facility_Area_Terminal:
			{
				m_strAreaName_Terminal = rhs.GetAreaName_Terminal();
			}
			break;
		case CalloutDispSpecDataItem::Facility_Portal:
			{
				m_strPortalName = rhs.GetPortalName();
			}
			break;
		default:
			{
				// 			ASSERT(FALSE);
			}
			break;
		}
	}

	RefreshDBString();

	return true;
}
void CalloutDispSpecDataItem::FacilityData::RefreshDBString()
{
	CString strData;

	if (IsALTObjectGroup())
	{
		if (m_altobjGroup.getID() == -1)
		{
			strData = _T("-1");
		}
		else
		{
			ASSERT(m_altobjGroup.getID()>0);
			strData.Format(_T("%d"), m_altobjGroup.getID());
		}
	}
	else
	{
		switch (m_enumFacilityType)
		{
		case CalloutDispSpecDataItem::Facility_Processor:
			{
				strData = m_processorID.GetIDString();
			}
			break;
		case CalloutDispSpecDataItem::Facility_Area_Terminal:
			{
				strData = m_strAreaName_Terminal;
			}
			break;
		case CalloutDispSpecDataItem::Facility_Portal:
			{
				strData = m_strPortalName;
			}
			break;
		default:
			{
				// 			ASSERT(FALSE);
			}
			break;
		}
	}
	m_strFacilityDB.Format(_T("[Version=%d;Type=%s]%s"), CALLOUT_FACILITY_VERSION,
		CalloutDispSpecDataItem::m_sFacilityStrings[m_enumFacilityType], strData);
}

void CalloutDispSpecDataItem::FacilityData::InitFromDBString( InputTerminal* pInTerminal )
{
	int nFound = m_strFacilityDB.Find(']');
	if (-1 == nFound)
	{
		ASSERT(FALSE);
		return;
	}

	CString strData = m_strFacilityDB.Right(m_strFacilityDB.GetLength() - nFound - 1);

	if (IsALTObjectGroup())
	{
		if (!strData.IsEmpty())
		{
			int nALTObjectGroupID = _ttoi(strData);
			if (-1 == nALTObjectGroupID)
			{
				m_altobjGroup.setID(nALTObjectGroupID);
			} 
			else
			{
				ASSERT(nALTObjectGroupID>0);
				m_altobjGroup.ReadData(nALTObjectGroupID);
			}
		}
	}
	else
	{
		switch (m_enumFacilityType)
		{
		case CalloutDispSpecDataItem::Facility_Processor:
			{
				m_processorID.SetStrDict( pInTerminal->inStrDict );
				m_processorID.setID( strData );
			}
			break;
		case CalloutDispSpecDataItem::Facility_Area_Terminal:
			{
				m_strAreaName_Terminal = strData;
			}
			break;
		case CalloutDispSpecDataItem::Facility_Portal:
			{
				m_strPortalName = strData;
			}
			break;
		default:
			{
				// 			ASSERT(FALSE);
			}
			break;
		}
	}
}
