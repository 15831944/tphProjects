#include "template.h"


char *PROC_TYPE_NAMES[PROCESSOR_TYPECOUNT+1] =
{
    "Point Processor",
    "Dependent Source",
    "Dependent Sink",
    "Line Processor",
    "Baggage Device",
    "Holding Area Processor",
    "Gate Processor",
    "Floor Change Processor",
    "Barrier",
	"Integrated Station Processor",
	"Moving Sidewalk Processor",
	"Elevator Processor",
	"Conveyor Processor",
	"Stair Processor",
	"Escalator Processor",
	"Billboard Processor",
	"BridgeConnector Processor",
	"Retail Processor",
	//airfield
	"Airport Reference Point",
	"Runway Processor",
	"Taxiway Processor",
	"Stand Processor",
	"Node Processor",
	"Deice Bay",
	"Waypoint Processor",
	"Contour Processor",
	"Apron Processor",
	"Hold Processor",
	"Sector Processor",
	//landfield
	"Stretch Processor",
	"Intersecion Processor",
	"Turnoff Processor",
	"Under Pass Processor",
	"Over Pass Processor",
	"Round About Processor",
	"Lane Adapter Processor",
	"Clover Leaf Processor",
	"Line Parking Processor",
	"NoseInParking Processor",
	"Yield Device Processor",
	"Parking Lot Processor",
	"Traffic Light Proceesor",
	"Stop Sign Processor",
	"Toll Gate Processor",

	0
};

char *SHORT_PROC_NAMES[PROCESSOR_TYPECOUNT+1] = 
{
    "POINT",
    "DPND_SOURCE",
    "DPND_SINK",
    "LINE",
    "BAGGAGE",
    "HOLD_AREA",
    "GATE",
    "FLOOR_CHANGE",
    "BARRIER",
	"INTEGRATED_STATION",
	"MOVING_SIDE_WALK",
	"ELEVATOR",
	"CONVEYOR",
	"STAIR",
	"ESCALATOR",
	"BILLBOARD",
	"BRIDGECONNECTOR",
	"RETAIL",



	//add new terminal  before this line


	//airfield
	"ARP",
	"RUNWAY",
	"TAXIWAY",
	"STAND",
	"NODE",
	"DEICE_STATION",
	"FIX",
	"CONTOUR",
	"APRON",
	"HOLD",
	"SECTOR",
	//landfield
	"STRETCH",
	"INTERSECTION",
	"TURNOFF",
	"UNDERPASS",
	"OVERPASS",
	"ROUNDABOUT",
	"LANEADAPTER",
	"CLOVERLEAF",
	"LINEPARKING",
	"NOSEINPARKING",
	"YIELDDEVICE",
	"PARKINGLOT",
	"TRAFFICLIGHT",
	"STOPSIGN",
	"TOLLGATE",

    0
};
char* ErrorMessageDB[]=
{
	" MobileElement Without Flow ",//// error 1
	" MobileElement Without Valid Flow ",// error 2
	" MobileElement On Baggage Device Exceed Max Waiting Time   ",// error 3
	" NO Such a Processor or Processor Group :",// error 4
	" 1:1 Flow Error ",// error 5
	" Out of bound when access array or vector." ,//error 6
	" File Format Error " ,//error 7
	" HoldingArea processor has no stage info " ,//error 8
	0
};

