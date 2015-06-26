#include "StdAfx.h"
#include "mutilreport.h"
#include "common\template.h"

CString arrayReortName[]=
{
	// passenger
	"PaxLog",		//ENUM_PAXLOG_REP=0,
	"Distance",		//ENUM_DISTANCE_REP, 
	"QueueTime",	//ENUM_QUEUETIME_REP,
	"Duration",		//ENUM_DURATION_REP,
	"ServiceTime",	//ENUM_SERVICETIME_REP,
	"ActiveBKDown",	//ENUM_ACTIVEBKDOWN_REP,	
	"Passengerno",//ENUM_PASSENGERNO_REP

	// processor
	"Utilization",	//ENUM_UTILIZATION_REP,
	"Throughput",	//ENUM_THROUGHPUT_REP,
	"QueueLenghth",	//ENUM_QUEUELENGTH_REP,
	"AvgQueueLen",	//ENUM_AVGQUEUELENGTH_REP,
	// space
	"PaxCount",		//ENUM_PAXCOUNT_REP,
	"PaxDens",		//ENUM_PAXDENS_REP,
	"SpaceThroughPut",//ENUM_SPACETHROUGHPUT_REP,
	"Collisions",	//ENUM_COLLISIONS_REP,
	// ac 
	"Acoperation",	//ENUM_ACOPERATION_REP,
	// bag
	"Bagcount",		//ENUM_BAGCOUNT_REP,
	"Bagwaittime",	//ENUM_BAGWAITTIME_REP,
	"Bagdistribution",//ENUM_BAGDISTRIBUTION_REP,
	"Bagdelivtime",	//ENUM_BAGDELIVTIME_REP,
	// economic
	"Economic",		//ENUM_ECONOMIC_REP 
	"FireEvacution" ,//ENUM_FIREEVACUTION_REP,
	"ConveyorWaitLength",//ENUM_CONVEYOR_WAIT_LENGTH_REP
	"ConveyorWaitTime",//ENUM_CONVEYOR_WAIT_TIME_REP
	"MissFlight",//ENUM_MISSFLIGHT_REP
	"BillboardExposureIncidence",//ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP
	"BillboardInkedProcIncreVisit",//ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP
	"RetailActivity",//ENUM_RETAIL_REP,
	""

};	// count == 25		
/*
CString arrayLogName[]=
{
	"PaxDesc.log",		//PaxDescFile = PeopleMoverFile + 5,// no: 22
	"PaxEvent.log",		//PaxEventFile,
	"FlightDesc.log",	//FlightDescFile,
	"FileghtEvent.log",	//FlightEventFile,
	"ProcDesc.log",		//ProcDescFile,
	"ProcEvent.log",	//ProcEvent
	"BagDesc.log",		//BagDescFile,
	"BagEvent.log",		//BagEvent
	"StringDictBin.log",//StringDictBinFile,1
	"TypeListBin.log",	//TypeListBinFile,
	"SectorBin.log",	//SectorBinFile,
	"CategoryBin.log",	//CategoryBinFile,
	"LogBackup.log"		//LogBackupFile,// no: 34
};
*/

LOGTOFILENAME inputfilename[]=
{
    LOGTOFILENAME((int)PaxEventFile,	(CString)"paxevent.log"),
	LOGTOFILENAME((int)PaxDescFile,		(CString)"paxdesc.log"),
	LOGTOFILENAME((int)FlightDescFile,	(CString)"flightdesc.log"),
	LOGTOFILENAME((int)ProcEventFile,	(CString)"procevent.log"),
	LOGTOFILENAME((int)ProcDescFile,	(CString)"procdesc.log"),
	LOGTOFILENAME((int)BagDescFile,		(CString)"bagdesc.log"),
	LOGTOFILENAME((int)TrainEventFile,	(CString)"trainevent.log"),
	LOGTOFILENAME((int)TrainDescFile,	(CString)"traindesc.log"),
	LOGTOFILENAME((int)ElevatorEventFile,	(CString)"elevatorevent.log"),
	LOGTOFILENAME((int)ElevatorDescFile,	(CString)"elevatordesc.log"),

	LOGTOFILENAME((int)BridgeConnectorEventFile,	(CString)"bridgeconnectorevent.log"),
	LOGTOFILENAME((int)BridgeConnectorDescFile,	(CString)"bridgeconnectordesc.log"),

	LOGTOFILENAME((int)RetailActivityEventFile,	(CString)"retailactivityevent.log"),
	LOGTOFILENAME((int)RetailActivityDescFile,	(CString)"retailactivitydesc.log"),

	LOGTOFILENAME((int)ResourceDescFile, (CString)"resourcedesc.log"),
	LOGTOFILENAME((int)ResourceEventFile, (CString)"resourceevent.log"),
	LOGTOFILENAME((int)DiagnoseLogFile, (CString)"DiagnoseLog.log"),
	LOGTOFILENAME((int)DiagnoseInfoFile, (CString)"DiagnoseInfo.log"),
	LOGTOFILENAME((int)AirsideEventFile, (CString)"airsideeventfile.log"),
	LOGTOFILENAME((int)AirsideDescFile, (CString)"airsidedescfile.log"),
	LOGTOFILENAME((int)AirsideFlightEventFileReport, (CString)"airsideflighteventfilereport.log"),
	LOGTOFILENAME((int)AirsideFlightDescFileReport, (CString)"airsideflightdescfilereport.log"),
	LOGTOFILENAME((int)AirsideALTObjectListFile, (CString)"airsidealtobjectlistfile.log"),
	LOGTOFILENAME((int)LandsideDescFile, (CString)"landsidedescfile.log"),
	LOGTOFILENAME((int)LandsideEventFile, (CString)"landsideeventfile.log"),
	LOGTOFILENAME((int)LandsideLinkGroupDescFile, (CString)"landsidelinkgroupdescfile.log"),
	LOGTOFILENAME((int)LandsideLinkGroupEventFile, (CString)"landsidelinkgroupeventfile.log"),
	LOGTOFILENAME((int)LandsideCrossWalkDescFile, (CString)"landsidecrosswalkdescfile.log"),
	LOGTOFILENAME((int)LandsideCrossWalkEventFile, (CString)"landsidecrosswalkeventfile.log"),
	LOGTOFILENAME((int)AirsideVehicleEventFile, (CString)"airsideVehicleeventfile.log"),
	LOGTOFILENAME((int)AirsideVehicleDescFile, (CString)"airsideVehicledescfile.log"),
	LOGTOFILENAME((int)AirsideFlightDelayFile, (CString)"airsideDelayLogfile.log"),
	LOGTOFILENAME((int)AirsideRunwayOperationFile, (CString)"airsideRunwayOperationLogfile.log"),	
	LOGTOFILENAME((int)AirsideVehicleEventFileReport, (CString)"airsideVehicleeventfilereport.log"),
	LOGTOFILENAME((int)AirsideVehicleDescFileReport, (CString)"airsideVehicledescfilereport.log"),
	LOGTOFILENAME((int)FlightStairEventFile, (CString)"flightstairevent.log"),
	LOGTOFILENAME((int)FlightDoorEventFile, (CString)"flightdoorevent.log"),

	LOGTOFILENAME((int)LSVehicleDelayLog, (CString)"LSVehicleDelayLogfile.log"),
	LOGTOFILENAME((int)LSResourceQueueLog, (CString)"LSResourceQueuefile.log"),
	LOGTOFILENAME((int)TempMobileElementLogFile, (CString)"temppaxevent.log"),




};
