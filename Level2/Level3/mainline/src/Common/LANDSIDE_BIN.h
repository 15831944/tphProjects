#pragma once
#pragma pack(push, 1)

// Landside log file structure

// vehicle log file description structure
struct LandsideVehicleDescStruct
{
	long startTime;
	long endTime;
	long startPos;
	long endPos;
	int vehicleID;
	int type;  //resident:0,  nonresident:1
	char sName[255];
	float dlength;
	float dwidth;
	float dheight;	
	int fltid;
	int paxcount;
	char fltmode;	
};

// vehicle log file event structure
struct LandsideVehicleEventStruct
{
	long time;   //time in million second
	int mode;   // 
	int state;	//
	int eventNum; //event type	
	float x;
	float y;
	float z;	  //real z
	float level;  //level z
	float dist;	  //dist in res
	float speed;  //speed in cm/s
	float wantspeed; //speed wanted in cm/s
	int   resid;    //resource id
	bool  bpush;    //push back
};




struct IntersectionLinkGroupDescStruct
{
	long startTime;
	long endTime;

	long startPos;
	long endPos;
	int intersectionID;
	int groupID;

};

enum LinkGroupState
{
	LS_ACTIVE,	//Green Light
	LS_BUFFER,	//Yellow Light
	LS_CLOSE,	//Red Light 
	LS_DISABLE  //disable
};
struct IntersectionLinkGroupEventStruct
{
// 	int intersectionID;
// 	int groupID;
	LinkGroupState state;
 	long time;
};
enum Cross_LightState
{
	CROSSLIGHT_GREEN,
	CROSSLIGHT_RED,
	CROSSLIGHT_BUFFER
};
enum CrossType
{
	Cross_Intersection,
	Cross_Pelican,
	Cross_Zebra
};
struct CrossWalkDescStruct
{
	long startTime;
	long endTime;

	long startPos;
	long endPos;
	int crossID;
	CrossType crossType;
};
struct CrossWalkEventStruct
{
	Cross_LightState lightState;
	long time;
};
#pragma pack(pop)