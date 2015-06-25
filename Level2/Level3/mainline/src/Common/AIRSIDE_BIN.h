#pragma once

#define ACCATLEN        4
#define CODELEN         4
#define FLIGHTIDLEN     32
#define CATEGORYLEN     32
#define ACTYPE          16
#define ORIGINDEST      16
#define VEHICLETYPE		64
#define OBJIDLEN		128
#include "../Common/IDGather.h"
#include "../Common/termfile.h"
#pragma pack(push, 1)
//arcterm log file information
//description struct
typedef struct // size = 40
{
	long id;
	char icao_code[CODELEN];
	char flightID[FLIGHTIDLEN];
	char arrID[FLIGHTIDLEN];
	char depID[FLIGHTIDLEN];
	char acType[ACTYPE];
	char origin[ORIGINDEST];
	char dest[ORIGINDEST];
	long startTime;
	long endTime;
	long startPos;
	long endPos;
	char acCategory[ACCATLEN];
	char weightCat[CATEGORYLEN];
	char speedCat[CATEGORYLEN];
	short airlineNO;
	long userCode;

	//
	CFlightID _arrID;
	CFlightID _depID;
	CAircraftType	_acType;
	CAirlineID _airline;
	CAirportCode _origin;    //the airport before the fromAirport
	CAirportCode _arrStopover;   // originating airport code
	CAirportCode _destination;     //the airport after the toAirport
	CAirportCode _depStopover; // to airport code

	float arrloadFactor;
	float deploadFactor;
	int nCapacity;
	long arrTime; 
	long depTime;
	long gateOccupancy;

	int nOnboardArrID;
	int nOnboardDepID;

	double dLength;
	double dWingSpan;
	double dWeight;
	double dHeight;
	double dAprchSpeed; //approach speed ,kts
	double dCabinWidth;

	char arrStandPlaned[OBJIDLEN];
	char intStandPlaned[OBJIDLEN];
	char depStandPlaned[OBJIDLEN];

   void ExportFile(ArctermFile& _file)
   {
      _file.writeInt(id) ;
	  _file.writeField(icao_code) ;
	  _file.writeField(flightID) ;
	  _file.writeField(arrID) ;
	  _file.writeField(depID) ;
	  _file.writeField(acType) ;
	  _file.writeField(origin) ;
	  _file.writeField(dest) ;
	  _file.writeInt(startTime) ;
	  _file.writeInt(endTime) ;
	  _file.writeInt(startPos) ;
	  _file.writeInt(endPos) ;
	  _file.writeField(acCategory) ;
	  _file.writeField(weightCat) ;
	  _file.writeField(speedCat) ;
      _file.writeInt(airlineNO) ;
	  _file.writeInt(userCode) ;
      
	  _file.writeField(_arrID) ;
	  _file.writeField(_depID) ;
	  _file.writeField(_acType) ;
	  _file.writeField(_airline) ;
	  _file.writeField(_origin) ;
	  _file.writeField(_arrStopover) ;
	  _file.writeField(_destination) ;
	  _file.writeField(_depStopover) ;

	  _file.writeFloat(arrloadFactor) ;
	  _file.writeFloat(deploadFactor);
	  _file.writeInt(nCapacity) ;
	  _file.writeInt(arrTime) ;
	  _file.writeInt(depTime) ;
	  _file.writeInt(gateOccupancy) ;

	  _file.writeDouble(dLength) ;
	  _file.writeDouble(dWingSpan) ;
	  _file.writeDouble(dWeight) ;
	  _file.writeDouble(dHeight) ;
	  _file.writeDouble(dCabinWidth);

	  _file.writeField(arrStandPlaned);
	  _file.writeField(intStandPlaned);
	  _file.writeField(depStandPlaned);
	  _file.writeLine() ;
   }
   void ImportFile(ArctermFile& _file)
   {
	   TCHAR ch[1024] = {0} ;
	   _file.getInteger(id) ;
	   _file.getField(icao_code,CODELEN) ;
	   _file.getField(flightID,FLIGHTIDLEN) ;
	   _file.getField(arrID,FLIGHTIDLEN) ;
	   _file.getField(depID,FLIGHTIDLEN) ;
	   _file.getField(acType,ACTYPE) ;
	   _file.getField(origin,ORIGINDEST) ;
	   _file.getField(dest,ORIGINDEST) ;
	   _file.getInteger(startTime) ;
	   _file.getInteger(endTime) ;
	   _file.getInteger(startPos) ;
	   _file.getInteger(endPos) ;
	   _file.getField(acCategory,ACCATLEN) ;
	   _file.getField(weightCat,CATEGORYLEN) ;
	   _file.getField(speedCat,CATEGORYLEN) ;
	   _file.getInteger(airlineNO) ;
	   _file.getInteger(userCode) ;

	   _file.getField(ch,1024) ;
	   _arrID = ch ;
	   _file.getField(ch,1024) ;
	   _depID =ch ;
	   _file.getField(ch,1024) ;
	   _acType = ch ;
	   _file.getField(ch,1024) ;
	   _airline = ch ;
	   _file.getField(ch,1024) ;
	   _origin = ch ;
	   _file.getField(ch,1024) ;
	   _arrStopover = ch ;
	   _file.getField(ch,1024) ;
	   _destination = ch ;
	   _file.getField(ch,1024) ;
	   _depStopover = ch ;

	   _file.getFloat(arrloadFactor) ;
	   _file.getFloat(deploadFactor);
	   _file.getInteger(nCapacity) ;
	   _file.getInteger(arrTime) ;
	   _file.getInteger(depTime) ;
	   _file.getInteger(gateOccupancy) ;

	   _file.getFloat(dLength) ;
	   _file.getFloat(dWingSpan) ;
	   _file.getFloat(dWeight) ;
	   _file.getFloat(dHeight) ;
	   _file.getFloat(dCabinWidth);

	   _file.getField(arrStandPlaned,OBJIDLEN);
	   _file.getField(intStandPlaned,OBJIDLEN);
	   _file.getField(depStandPlaned,OBJIDLEN);
   }

} AirsideFlightDescStruct;

//flight event struct
typedef struct // size = 
{
	long time;
	char state;
	long nodeNO;
	long intNodeID;
	char rwcode[FLIGHTIDLEN]; //runway mark
	char gatecode[FLIGHTIDLEN]; //stand name
	char starsidname[FLIGHTIDLEN]; //star /sid  name	
	float x;
	float y;
	float z;
	float dirx;
	float diry;
	float dirz;
	float offsetAngle;
	char eventCode; //event type
	short runwayNO;
	int mode;//state
	long segmentNO;
	short gateNO;
	long delayTime;
	long interval;
	bool IsBackUp;
	int reason;
	double speed;
	double dacc; //acceleration dec <0
	long lDelayIndex;
	bool IsSpeedConstraint;
	bool towOperation;
	bool deiceParking;
} AirsideFlightEventStruct;

// vehicle description
typedef struct // size = 40
{
	long id; // the vehicle type id  
	char vehicleType[VEHICLETYPE];
	long startTime;
	long endTime;
	long startPos;
	long endPos;
	long poolNO;
	long userCode;
	double vehiclelength;
	double vehiclewidth;
	double vehicleheight;
	long IndexID ; //the vehicle  id 
    float m_FuelConsumed ; // the fuel consumed for this type vehicle 
	long m_BaseType ;
	long m_VecileTypeID ;
} AirsideVehicleDescStruct;

//vehicle event struct
typedef struct // size = 
{
	long time;
	char state;
	long nodeNO;
	float x;
	float y;
	float z;
	float dirx;
	float diry;
	float dirz;
	char eventCode; //event type
	char mode;//state
	long segmentNO;
	short standNO;
	long delayTime;
	bool IsBackUp;
	double speed;
	double dacc;
	long  m_ServerFlightID; // the flight which vehicle is serving 
	
	char serverFlightID[FLIGHTIDLEN];
	char serverFlightAirline[AIRLINE_LEN];
	char serveStand[FLIGHTIDLEN];


} AirsideVehicleEventStruct;

#pragma pack(pop)
