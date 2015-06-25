#ifndef _UNIT_HEADER_
#define _UNIT_HEADER_

//length unit enum
enum ARCUnit_Length  //aul
{
	AU_LENGTH_KILOMETER  = 0 ,     //kilometer
	AU_LENGTH_METER ,              //meter
	AU_LENGTH_CENTIMETER ,         //centimeter
	AU_LENGTH_MILLIMETER ,         //millimeter
	AU_LENGTH_MILE ,               //mile
	AU_LENGTH_FEET ,               //feet
	AU_LENGTH_INCH ,               //inch
	AU_LENGTH_NAUTICALMILE ,       //nautical mile	 

	AU_LENGTH_TOTAL_COUNT //unit total count
};

//weight unit enum
enum ARCUnit_Weight  //auw
{
	AU_WEIGHT_KILOGRAM = 0 ,  //kilogram
	AU_WEIGHT_TON,			  //ton, replace the 'gram' by 'ton';the same sequence as 'WeightConversion[][2]' 
	AU_WEIGHT_POUND ,         //pound
	AU_WEIGHT_GRAM ,          //gram

	AU_WEIGHT_TOTAL_COUNT  //unit total count
};

//time unit enum
enum ARCUnit_Time //aut
{
	AU_TIME_HOUR = 0 , //hour
	AU_TIME_MINUTE ,   //minute
	AU_TIME_SECOND ,   //second

	AU_TIME_TOTAL_COUNT  //unit total count
};

//velocity unit enum
enum ARCUnit_Velocity //auv
{
	AU_VELOCITY_KMPH = 0 ,  //km/h	
	AU_VELOCITY_MPS ,       //m/s
	AU_VELOCITY_KTS ,       //kts
	AU_VELOCITY_MPH ,		//May 6 2013, new add, mile per hour
	AU_VELOCITY_TOTAL_COUNT  //unit total count
};

enum ARCUnit_Acceleration //aua
{
	AU_ACCELERATION_MPERS2 = 0,  // m/s^2
	AU_ACCELERATION_FTPERS2,     // ft/s^2

	AU_ACCELERATION_TOTAL_COUNT //unit total count
};

//set database default units:
#define DEFAULT_DATABASE_LENGTH_UNIT     AU_LENGTH_CENTIMETER //default length uniti centimeter in DB 
#define DEFAULT_DATABASE_WEIGHT_UNIT     AU_WEIGHT_GRAM //default weight unit is gram in DB
#define DEFAULT_DATABASE_TIME_UNIT       AU_TIME_MINUTE //default time unit is minute in DB
#define DEFAULT_DATABASE_VELOCITY_UNIT   AU_VELOCITY_MPS //default velocity unit is m/s in DB
#define DEFAULT_DATABASE_ACCELERATION_UNIT   AU_ACCELERATION_MPERS2 //default acceleration unit is m/s^2 in DB

class CARCUnit 
{
public:
	CARCUnit(void);
	~CARCUnit(void);
};

class CARCLengthUnit : public CARCUnit
{
public:
	CARCLengthUnit(void);
	~CARCLengthUnit(void);
public:
	//length
	static bool EnumLengthUnitString(CStringArray &ayLenStr);
	static bool EnumLengthUnitLongString(CStringArray &ayLenLongStr);
	static CString GetLengthUnitString(ARCUnit_Length aulUnit);
	static CString GetLengthUnitLongString(ARCUnit_Length aulUnit);
	static double ConvertLength(ARCUnit_Length aulUnitFrom, ARCUnit_Length aulUnitTo, double dValue);
};


class CARCWeightUnit : public CARCUnit
{
public:
	CARCWeightUnit(void);
	~CARCWeightUnit(void);
public:
	//weight
	static bool EnumWeightUnitString(CStringArray &ayWeightStr);
	static bool EnumWeightUnitLongString(CStringArray &ayWeightLongStr);
	static CString GetWeightUnitString(ARCUnit_Weight auwUnit);
	static CString GetWeightUnitLongString(ARCUnit_Weight auwUnit);
	static double ConvertWeight(ARCUnit_Weight auwUnitFrom, ARCUnit_Weight auwUnitTo, double dValue);
};


class CARCTimeUnit : public CARCUnit
{
public:
	CARCTimeUnit(void);
	~CARCTimeUnit(void);
public:
	//time
	static bool EnumTimeUnitString(CStringArray &ayTimeStr);
	static bool EnumTimeUnitLongString(CStringArray &ayTimeLongStr);
	static CString GetTimeUnitString(ARCUnit_Time autUnit);
	static CString GetTimeUnitLongString(ARCUnit_Time autUnit);
	static double ConvertTime(ARCUnit_Time autUnitFrom, ARCUnit_Time autUnitTo, double dValue);
};


class CARCVelocityUnit : public CARCUnit
{
public:
	CARCVelocityUnit(void);
	~CARCVelocityUnit(void);
public:
	//velocity
	static bool EnumVelocityUnitString(CStringArray &ayVelocityStr);
	static bool EnumVelocityUnitLongString(CStringArray &ayVelocityLongStr);
	static CString GetVelocityUnitString(ARCUnit_Velocity auvUnit);
	static CString GetVelocityUnitLongString(ARCUnit_Velocity auvUnit);
	static double ConvertVelocity(ARCUnit_Velocity auvUnitFrom, ARCUnit_Velocity auvUnitTo, double dValue);
};


class CARCAccelerationUnit : public CARCUnit
{
public:
	CARCAccelerationUnit(void);
	~CARCAccelerationUnit(void);
public:
	//acceleration
	static bool EnumAccelerationUnitString(CStringArray &ayAccelerationStr); 
	static CString GetAccelerationUnitString(ARCUnit_Acceleration auaUnit); 
	static double ConvertAcceleration(ARCUnit_Acceleration auaUnitFrom, ARCUnit_Acceleration auaUnitTo, double dValue);
};

#endif