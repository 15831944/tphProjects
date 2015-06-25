#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "commondll.h"
#include "ErrorMessage.h"
#pragma  once
#ifdef __FLAT__
#define far
#define huge
#endif

#define MAX_PAX_TYPES	     16
#define MAX_PAX_TYPE_LEN    1024
#define MAX_PROC_IDS        4
#define MAX_PROC_ID_LEN		128
#define MAX_PROCESSOR_LIST  8
#define MAX_FLIGHT_TYPE_LEN 128

#define EMPTY_ID            0
#define AIRLINE_LEN			32
#define FLIGHT_LEN			32
#define FULL_ID_LEN         16
#define MAX_TIME_LEN        9
#define AIRPORT_LEN         5
#define AC_TYPE_LEN         256
#define AC_NAME_LEN         50
#define SECTOR_CODE_LEN     16
#define CATEGORY_LEN        16
#define TRACK_LIMIT         256
#define BRANCHES            16
#define LEVELS              16
#define TRAIN_ID_LEN		128
#define RESOURCE_NAME_LEN	128
#define SCREENPRINT_MAX_LEN 1024
#define BRIDGECONNECTOR_ID_LEN		256

COMMON_TRANSFER extern char * ErrorMessageDB[];
// Attention: Add a new type of ErrorCode, you must add its error message
// in .cpp consistently.
enum ErrorCode
{
	Engine_Error_NoFlow,// error 1
	Engine_Error_No_Valid_Flow,// error 2
	Engine_Error_Element_OnBagDevice_Exceed_WaitingTime,// error 3
	Error_Processor_Not_Found,// error 4
	Error_Flow_OneToOne,// error 5
	Error_OutOf_Bound,//error 6
	Error_File_FormatError,//error 7
	Engine_Error_HoldingArea_No_StageInfo,//error 8
};


// File and Directory defines
enum InputFiles
{
	// Input files
    SimulationParametersFile,
    PassengerDataFile,
    FlightScheduleFile,
    FlightDataFile,
    PassengerDescriptionFile,
    PassengerDistributionFile,
    HubbingDataFile,
    BaggageDataFile,
    ProcessorLayoutFile,
    MiscProcessorDataFile,
    ProcessorScheduleFile,
    ServiceTimeFile,
    PassengerFlowFile,
    SectorFile,
    GateScheduleFile,
    AircraftDatabaseFile,
    AircraftCategoryFile,
	RailwayDataFile,

	ProbabilityDistributionFile,
	GlobalDBListFile,
	PassengerBulkFile,
	FlowSyncFile,
    RosterRulesFile ,
	// Binary log files
    PaxDescFile,
    PaxEventFile,
    FlightDescFile,
    FlightEventFile,
    ProcDescFile,
    ProcEventFile,
    BagDescFile,
    BagEventFile,
    StringDictBinFile,
    TypeListBinFile,
    SectorBinFile,
    CategoryBinFile,
    LogBackupFile,
	AirsideDescFile,
	AirsideEventFile,
	AirsideFlightDescFileReport,
	AirsideFlightEventFileReport,
	AirsideFlightDelayFile,
	AirsideRunwayOperationFile,
	AirsideVehicleDescFile,
	AirsideVehicleEventFile,
	AirsideVehicleDescFileReport,
	AirsideVehicleEventFileReport,
	LandsideDescFile,
	LandsideEventFile,	
	LandsideLinkGroupDescFile,
	LandsideLinkGroupEventFile,	
	LandsideCrossWalkDescFile,
	LandsideCrossWalkEventFile,
	FlightStairEventFile,	
	FlightDoorEventFile,

	// CSV echo files
    PaxLogEchoFile,
    FlightLogEchoFile,
    ProcLogEchoFile,
    BagLogEchoFile,
    StringDictEchoFile,

	// Report Specifications Files
    DistanceSpecFile,
    DurationSpecFile,
    PaxLogSpecFile,
    ProcLogSpecFile,
    FlightLogSpecFile,
    BagLogSpecFile,
    ServiceTimeSpecFile,
    QueueTimeSpecFile,
    ActivityBreakdownSpecFile,
    BagWaitTimeSpecFile,
    PassengerCountSpecFile,
    PassengerDensitySpecFile,
    UtilizationSpecFile,
    ThroughputSpecFile,
    AverageQueueLengthSpecFile,
    QueueLengthSpecFile,
    BagCountSpecFile,
	SpaceThroughputSpecFile,
	CollisionSpecFile,
	AcOperationsSpecFile,
	BagDistSpecFile,
	BagDeliveryTimeSpecFile,
	FireEvacuationSpecFile,
	BillboardExposureIncidenceSpecFile,
	BillboardLinkedProcIncrementlVisitSpecFile,
	RetailSpecFile,
	// Report Output Files
    DistanceReportFile,
    DurationReportFile,
    PaxLogReportFile,
    ProcLogReportFile,
    FlightLogReportFile,
    BagLogReportFile,
    ServiceTimeReportFile,
    QueueTimeReportFile,
    ActivityBreakdownReportFile,
    BagWaitTimeReportFile,
    PassengerCountReportFile,
    PassengerDensityReportFile,
    UtilizationReportFile,
    ThroughputReportFile,
    AverageQueueLengthReportFile,
    QueueLengthReportFile,
    BagCountReportFile,
	SpaceThroughputReportFile,
	CollisionReportFile,
	AcOperationsReportFile,
	BagDistReportFile,
	BagDeliveryTimeReportFile,
	FireEvacuationReportFile,
	MissFlightReportFile,
	BillboardExposureIncidenceFile,
	BillboardLinkedProcIncrementlVisitFile,
	RetailReportFile,

	// New Files
	AreasFile,
	PortalsFile,
	FloorsFile,
	DefaultDisplayPropertiesFile,
	PaxDisplayPropertiesFile,
	PlacementsFile,
	PaxTagsFile,
	ActivityDensityFile,
	LayerInfoFile,
	CamerasFile,
	VRInfoFile,
	DisplayPropOverridesFile,
	ProcTagsFile,
	FlightTagsFile,
	MoviesFile,
	WalkthroughsFile,
	AirsideWalkthroughFile,
	LandsideVehicleWalkthroughsFile,
	VideoParamsFile,
	ResourceDisplayPropertiesFile,
	AircraftDisplayPropertiesFile,
	AircraftTagsFile,
	NonPaxRelativePosFile,
	PeopleMoverFile,

	// econ
	BuildingDeprecCost,
	CaptIntrCost,
	InsuranceCost,
	ContractsCost,
	ParkingLotsDeprecCost,
	LandsideFacilitiesCost,
	AirsideFacilitiesCost,

	ProcessorCost,
	PassengerCost,
	AircraftCost,
	UtilitiesCost,
	LaborCost,
	MaintenanceCost,

	LeasesRevenue,
	AdvertisingRevenue,
	ContractsRevenue,

	PaxAirportFeeRevenue,
	LandingFeesRevenue,
	GateUsageRevenue,
	ProcUsageRevenue,
	RetailPercRevenue,
	ParkingFeeRevenue,
	LandsideTransRevenue,

	WaitingCostFactors,
	CapitalBudgetingDecisions,
	AnnualActivityDeduction,

	//gate assignment priority info file
	GatePriorityInfoFile,
	FlightPriorityInfoFile,
	ConstraintSpecFile,
	GateAssignConFile,
	AdjacencyGateConFile,
	IntermediateStandAssignFile,
	BagGateAssignFile,
	StationLayoutFile,
	AllCarsScheduleFile,
	TrainDescFile,
	TrainEventFile,	
    TrainLogEchoFile,
	StationPaxFlowFile,
	SpecificFlightPaxFlowFile,
	MovingSideWalKFile,
	MovingSideWalkPaxDistFile,
	PipeDataSetFile,
	TerminalProcessorTypeSetFile,
	MobileElemTypeStrDBFile,
	ProcessorSwitchTimeFile,
	SubFlowList,
	GateMappingDBFile,
	ReportParameterFile,
	SimAndReportFile,
	UserNameListFile,
	ElevatorDescFile,
	ElevatorEventFile,	
    ElevatorLogEchoFile,
	FlightScheduleCriteriaFile,
	BridgeConnectorDescFile,
	BridgeConnectorEventFile,
    BridgeConnectorLogEchoFile,
	RetailActivityDescFile,
	RetailActivityEventFile,
	RetailActivityLogEchoFile,
	ResourecPoolFile,
	ProcessorToResourcePoolFile,
	ResourceDescFile,
	ResourceEventFile,
	ResourceLogEchoFile,
	DiagnoseLogFile,
	DiagnoseInfoFile,
	ConveyorWaitLengthReport,
	ConveyorWaitLengthSpecFile,
	ConveyorWaitTimeReport,
	ConveyorWaitTimeSpecFile,
	HostInfoFile,
	ProjectInfoFile,
	ModelInfoFile,
	ReportInfoFile,
	MMProcessInfoFile,
	MMFlowInfoFile,
	//multiply run report file
	Mult_HostInfoFile,
	Mult_ProjectInfoFile,
	//Math Report Result
	MathQTimeResult,
	MathQLengthResult,
	MathThroughputResult,
	MathPaxCountResult,
	MathUtilizationResult,
	HistogramInputFile,
	CongestionParamFile,
	AirPortInfoFile, 
	AirsideProcessorLayoutFile,
	AirsidePlacementsFile,
	AirsideFloorsFile,
	AirsideContourTreeFile,
	AirsideStructureFile,
	AirsideCamerasFile,
	TerminalStructureFile, 
	AirsideAircraftCategory,
	LogDebugFile,
	AirsideWallShapeFile,
	TerminalWallShapeFile,
	//Landside Files
	LandsideFloorsFile,
	LandsideCamerasFile,
	LandsideProcessorFile,
	LandsidePlacementsFile,
	LandsideOperationTypeFile,
	Stand2GateMapping,
	ObstructionReportFile,
	LSVehicleDelayLog,
	LSResourceQueueLog,
	TempMobileElementLogFile,
	//vehicle distribution file
	VehicleDistributionFile,
};

enum GeneralFiles
{
    ProjectFile,
    InputFileList,
    CurrentProjectFile,
    HelpListFile
};

enum DirectoryList
{
    InputDir,
    EchoDir,
    LogsDir,
    StatSpecDir,
    ReportDir,
    BackupDir,
	EconomicDir,
    FileSpecsDir,
    SystemDir,
    UndoDir,
	CommentsDir,
	TextureDir,
	ComparativeDir
};


#ifdef __cplusplus

// ProbabilityDistributions
class ProbabilityDistribution;
class PaxTypeDistribution;
class ProcessorDistribution;
class HistogramDistribution;
class EmpiricalDistribution;
class ConstantDistribution;
class NormalDistribution;
class ExponentialDistribution;
class BetaDistribution;
class UniformDistribution;
class BernoulliDistribution;

// typing classes
class PassengerConstraint;
class FlightConstraint;
class MultiPaxConstraint;
class PassengerConstraintDatabase;
class FlightConstraintDatabase;
class CMobileElemTypeStrDB;

// Flight related classes
class Flight;
class FlightData;
class FlightSchedule;
class FlightLog;
class CHubbingDatabase;
class SectorList;
class CategoryList;
class Sector;
class ACCategory;
class Aircraft;
class AircraftDatabase;

// Processor
class Processor;
class ProcessorID;
class ProcessorLog;
class ProcessorEntry;
class ProcessorEvent;
class ProcessorList;
class GateProcessor;
class Barrier;
class HoldingArea;
class ServiceTimeDatabase;
class PassengerFlowDatabase;
class Station;
//class Dispatcher;
//class PeopleMover;
class BagGateAssign;

// Assignment
class ProcAssignDatabase;
class ProcAssignEntry;
class ProcessorRoster;
class ProcessorRosterSchedule;
class CProcessorSwitchTimeDB;
class CGateMappingDB;

// Priority Info
class GatePriorityInfo;
class FlightPriorityInfo;

class CEconomicDatabase;

#define PROC_TYPES  42

enum ProcessorClassList
{
    PointProc = 0,
    DepSourceProc,
    DepSinkProc,
    LineProc,
    BaggageProc,
    HoldAreaProc,
    GateProc,
    FloorChangeProc,
    BarrierProc,
	IntegratedStationProc,
	MovingSideWalkProc,
	Elevator,
	ConveyorProc,
	StairProc,
	EscalatorProc,
	BillboardProcessor,
	BridgeConnectorProc,
	RetailProc,

	//add new processor 
	//NOTE: MUST put the new processor type at tail. must NOT put in the middle!




	//airfield
	ArpProcessor,
	RunwayProcessor,
	TaxiwayProcessor,
	StandProcessor,
	NodeProcessor,
	DeiceBayProcessor,
	FixProcessor,
	ContourProcessor,
	ApronProcessor,
	HoldProcessor,
	SectorProcessor,
	//landfield
	StretchProcessor,
	IntersectionProcessor,
	TurnoffProcessor,
	UnderPassProcessor,
	OverPassProcessor,
	RoundAboutProcessor,
	LaneAdapterProcessor,
	CloverLeafProcessor,
	LineParkingProcessor,
	NoseInParkingProcessor,
	YieldDeviceProcessor,
	ParkingLotProcessor,
	TrafficLightProceesor,
	StopSignProcessor,
	TollGateProcessor,
	//Processor TYPE Count
	PROCESSOR_TYPECOUNT,
};

#define LinkedDestProc -1

COMMON_TRANSFER extern char *PROC_TYPE_NAMES[];
COMMON_TRANSFER extern char *SHORT_PROC_NAMES[];

enum PORTTOPORT
{
	UNKNOWN = -1,
	PORT1_PORT1,
	PORT1_PORT2,	
	PORT2_PORT1,
	PORT2_PORT2,
};
// misc processor data
class MiscDatabase;
class MiscData;
class ProcessorEntry;
class ProcessorHierarchy;

typedef struct
{
    int start;
    int end;
} GroupIndex;

// Queues
class ProcessorQueue;
class FixedQueue;
class NonFixedQueue;

// Mobile Elements
class PassengerTypeList;
class PassengerDistributionList;
class PassengerData;
class MobileElement;
class Person;
class PaxVisitor;
class Greeter;
class Sender;
class Passenger;
class PersonList;

// Common
class ArctermFile;
class Line;
class Point;
class StringDictionary;
class StringList;
class Environment;

// Events
class EventLoop;
class Event;
class TerminalEntryEvent;
class BaggageEntryEvent;
class ProcessorAssignmentEvent;
class MobileElementMovementEvent;

// Baggage
class BaggageData;

//station and railway
class CRailWayData;
class CStationLayout;
class CAllCarSchedule;
#define TRUE    1
#define FALSE   0

// log files
class PaxLog;
class ProcLog;
class FlightLog;
class BagLog;
class CTrainLog;

class PeopleMoverDataSet;
class CSideMoveWalkDataSet;
class CSideMoveWalkPaxDist;
class CSimParameter;

class Terminal;
#endif
#endif
