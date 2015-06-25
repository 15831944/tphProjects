#include "stdafx.h"
#include "CARCUnit.h"
/////
//Length Unit
static CString LengthStr[] = 
{
	_T("km"),//0
	_T("m"), //1
	_T("cm"), //2
	_T("mm"), //3
	_T("mi"), //4
	_T("ft"), //5
	_T("in"),//6
	_T("nm"), //7
};

static CString LengthLongStr[] = 
{
	_T("kilometer"),//0
	_T("meter"),//1
	_T("centimeter"),//2
	_T("millimeter"),//3
	_T("mile"), //4	
	_T("foot"),//5
	_T("inch"),//6
	_T("nautical mile"), //7
};

//internal unit is meter for length unit,(Extend - Ext , Internal - Int)
static double LengthConversion[][2] = 
{ //ExtToInt, IntToExt
	{ 1000.0 , 0.001},//0
	{ 1.0 , 1.0 },//1
	{ 0.01 , 100.0 },//2
	{ 0.001 , 1000.0 },//3
	{ 1609.344  , 1000.0/1609344.0  },//4
	{ 0.3048 , 10000.0/3048.0 },//5
	{ 0.0254 , 10000.0/254.0 },//6
	{ 1852.0 , 1.0/1852.0 }, //7
};

/////Length Unit


/////
//Weight Unit
static CString WeightStr[] = 
{
	_T("kg"), //0	
	_T("t"), //1
	_T("lbs"), //2
	_T("g"),  //1
};

static CString WeightLongStr[] = 
{
	_T("kilogram"),//0	
	_T("ton"),   // 1
	_T("pound"),   //2
	_T("gram"),    //1
};

//internal unit is gram for weight unit,(Extend - Ext , Internal - Int)
static double WeightConversion[][2] = 
{ //ExtToInt, IntToExt
	{ 1000.0 , 0.001 },//0
	{ 1000000.0, 0.000001}, // 1 -- ton , the sequence must be the same as 'AU_WEIGHT_TON'
	{ 453.59237 , 100000.0/45359237.0 },//2
	{ 1.0 , 1.0 },//3  -- gram
	
};

/////Weight Unit

/////
//Time Unit
static CString TimeStr[] = 
{
	_T("h"),//0
	_T("min"),//1
	_T("s"),//2
};

static CString TimeLongStr[] = 
{
	_T("hour"),//0
	_T("minute"),//1
	_T("second"),//2
};

//internal unit is second for time unit,(Extend - Ext , Internal - Int)
static double TimeConversion[][2] = 
{ //ExtToInt, IntToExt
	{ 3600.0 , 1.0/3600.0 },//0
	{ 60.0 , 1.0/60.0 },//1
	{ 1.0 , 1.0},//2
};

/////Time Unit

///// 
//Velocity Unit
static CString VelocityStr[] = 
{
	_T("km/h"),//0	
	_T("m/s"),//1
	_T("kts"),//2
	_T("mi/h"),//3 May 6 2013, new add, mile per hour
};

static CString VelocityLongStr[] = 
{
	_T("kilometers per hour"),//0
	_T("meters per second"),//1
	_T("knots per hour"),//2
	_T("miles per hour"),
};

//internal unit is m/s for velocity unit,(Extend - Ext , Internal - Int)
static double VelocityConversion[][2] = 
{ //ExtToInt, IntToExt
	{ 10.0/36.0 , 3.6},//0	
	{ 1.0 , 1.0 },//1
	{ 1852.0/3600.0 , 3600.0/1852.0 },//2
	{ 1609.0/3600.0 , 3600.0/1609.0},//3 May 6 2013, new add, mile per hour conversion
};
/////Velocity Unit

///// 
//Acceleration Unit
static CString AccelerationStr[] = 
{
	_T("m/s^2"),//0
	_T("ft/s^2"),//1
};

//internal unit is m/s^2 for acceleration unit,(Extend - Ext , Internal - Int)
static double AccelerationyConversion[][2] = 
{ //ExtToInt, IntToExt
	{ 1.0 , 1.0 },//0
	{ 0.3048 , 10000.0/3048.0 },//1
};
/////Acceleration Unit


CARCUnit::CARCUnit(void)
{
}

CARCUnit::~CARCUnit(void)
{
}

CARCLengthUnit::CARCLengthUnit(void)
{
}

CARCLengthUnit::~CARCLengthUnit(void)
{
}

bool CARCLengthUnit::EnumLengthUnitString(CStringArray &ayLenStr)
{
	ayLenStr.RemoveAll();
	for (int i = 0;i < AU_LENGTH_TOTAL_COUNT ;++i) {
		ayLenStr.Add(LengthStr[i]);
	}
	return (TRUE);
}

bool CARCLengthUnit::EnumLengthUnitLongString(CStringArray &ayLenLongStr)
{
	ayLenLongStr.RemoveAll();
	for (int i = 0;i < AU_LENGTH_TOTAL_COUNT ;++i) {
		ayLenLongStr.Add(LengthLongStr[i]);
	}
	return (TRUE);
}

CString CARCLengthUnit::GetLengthUnitString(ARCUnit_Length aulUnit)
{
	if((int)aulUnit < 0 || aulUnit >= AU_LENGTH_TOTAL_COUNT)return _T("");
	return (LengthStr[int(aulUnit)]);
}

CString CARCLengthUnit::GetLengthUnitLongString(ARCUnit_Length aulUnit)
{
	if((int)aulUnit < 0 || aulUnit >= AU_LENGTH_TOTAL_COUNT)return _T("");
	return (LengthLongStr[int(aulUnit)]);
}

double CARCLengthUnit::ConvertLength(ARCUnit_Length aulUnitFrom, ARCUnit_Length aulUnitTo, double dValue)
{
	if((int)aulUnitFrom < 0 || aulUnitFrom >= AU_LENGTH_TOTAL_COUNT || (int)aulUnitTo < 0 || aulUnitTo >= AU_LENGTH_TOTAL_COUNT)return (dValue);
	return ( dValue * (LengthConversion[aulUnitFrom][0]) * (LengthConversion[aulUnitTo][1]) );
}

CARCWeightUnit::CARCWeightUnit(void)
{
}

CARCWeightUnit::~CARCWeightUnit(void)
{
}

CString CARCWeightUnit::GetWeightUnitString(ARCUnit_Weight auwUnit)
{
	if((int)auwUnit < 0 || (int)auwUnit >= (int)AU_WEIGHT_TOTAL_COUNT-1)return _T("");
	return (WeightStr[int(auwUnit)]);
}

CString CARCWeightUnit::GetWeightUnitLongString(ARCUnit_Weight auwUnit)
{
	if((int)auwUnit < 0 || auwUnit >= AU_WEIGHT_TOTAL_COUNT)return _T("");
	return (WeightLongStr[int(auwUnit)]);
}

bool CARCWeightUnit::EnumWeightUnitString(CStringArray &ayWeightStr)
{
	ayWeightStr.RemoveAll();
	for (int i = 0;i < (int)AU_WEIGHT_TOTAL_COUNT-1 ;++i) {
		ayWeightStr.Add(WeightStr[i]);
	}
	return (TRUE);
}

bool CARCWeightUnit::EnumWeightUnitLongString(CStringArray &ayWeightLongStr)
{
	ayWeightLongStr.RemoveAll();
	for (int i = 0;i < AU_WEIGHT_TOTAL_COUNT ;++i) {
		ayWeightLongStr.Add(WeightLongStr[i]);
	}
	return (TRUE);
}

double CARCWeightUnit::ConvertWeight(ARCUnit_Weight auwUnitFrom, ARCUnit_Weight auwUnitTo, double dValue)
{
	if((int)auwUnitFrom < 0 || auwUnitFrom >= AU_WEIGHT_TOTAL_COUNT || (int)auwUnitTo < 0 || auwUnitTo >= AU_WEIGHT_TOTAL_COUNT)return (dValue);
	return ( dValue * (WeightConversion[auwUnitFrom][0]) * (WeightConversion[auwUnitTo][1]) );
}

CARCTimeUnit::CARCTimeUnit(void)
{
}

CARCTimeUnit::~CARCTimeUnit(void)
{
}

CString CARCTimeUnit::GetTimeUnitString(ARCUnit_Time autUnit)
{
	if((int)autUnit < 0 || autUnit == AU_TIME_TOTAL_COUNT)return _T("");
	return (TimeStr[int(autUnit)]);
}

CString CARCTimeUnit::GetTimeUnitLongString(ARCUnit_Time autUnit)
{
	if((int)autUnit < 0 || autUnit == AU_TIME_TOTAL_COUNT)return _T("");
	return (TimeLongStr[int(autUnit)]);
}

bool CARCTimeUnit::EnumTimeUnitString(CStringArray &ayTimeStr)
{
	ayTimeStr.RemoveAll();
	for (int i = 0;i < AU_TIME_TOTAL_COUNT ;++i) {
		ayTimeStr.Add(TimeStr[i]);
	}
	return (TRUE);
}

bool CARCTimeUnit::EnumTimeUnitLongString(CStringArray &ayTimeLongStr)
{
	ayTimeLongStr.RemoveAll();
	for (int i = 0;i < AU_TIME_TOTAL_COUNT ;++i) {
		ayTimeLongStr.Add(TimeLongStr[i]);
	}
	return (TRUE);
}

double CARCTimeUnit::ConvertTime(ARCUnit_Time autUnitFrom, ARCUnit_Time autUnitTo, double dValue)
{
	if((int)autUnitFrom < 0 || autUnitFrom >= AU_TIME_TOTAL_COUNT || (int)autUnitTo < 0 || autUnitTo >= AU_TIME_TOTAL_COUNT)return (dValue);
	return ( dValue * (TimeConversion[autUnitFrom][0]) * (TimeConversion[autUnitTo][1]) );
}

CARCVelocityUnit::CARCVelocityUnit(void)
{
}

CARCVelocityUnit::~CARCVelocityUnit(void)
{
}

CString CARCVelocityUnit::GetVelocityUnitString(ARCUnit_Velocity auvUnit)
{
	if((int)auvUnit < 0 || auvUnit >= AU_VELOCITY_TOTAL_COUNT)return _T("");
	return (VelocityStr[int(auvUnit)]);
}

CString CARCVelocityUnit::GetVelocityUnitLongString(ARCUnit_Velocity auvUnit)
{
	if((int)auvUnit < 0 || auvUnit >= AU_VELOCITY_TOTAL_COUNT)return _T("");
	return (VelocityLongStr[int(auvUnit)]);
}

bool CARCVelocityUnit::EnumVelocityUnitString(CStringArray &ayVelocityStr)
{
	ayVelocityStr.RemoveAll();
	for (int i = 0;i < AU_VELOCITY_TOTAL_COUNT ;++i) {
		ayVelocityStr.Add(VelocityStr[i]);
	}
	return (TRUE);
}

bool CARCVelocityUnit::EnumVelocityUnitLongString(CStringArray &ayVelocityLongStr)
{
	ayVelocityLongStr.RemoveAll();
	for (int i = 0;i < AU_VELOCITY_TOTAL_COUNT ;++i) {
		ayVelocityLongStr.Add(VelocityLongStr[i]);
	}
	return (TRUE);
}

double CARCVelocityUnit::ConvertVelocity(ARCUnit_Velocity auvUnitFrom, ARCUnit_Velocity auvUnitTo, double dValue)
{
	if((int)auvUnitFrom < 0 || auvUnitFrom >= AU_VELOCITY_TOTAL_COUNT || (int)auvUnitTo < 0 || auvUnitTo >= AU_VELOCITY_TOTAL_COUNT)return (dValue);
	return ( dValue * (VelocityConversion[auvUnitFrom][0]) * (VelocityConversion[auvUnitTo][1]) );
}

CARCAccelerationUnit::CARCAccelerationUnit(void)
{
}

CARCAccelerationUnit::~CARCAccelerationUnit(void)
{
}

bool CARCAccelerationUnit::EnumAccelerationUnitString(CStringArray &ayAccelerationStr)
{
	ayAccelerationStr.RemoveAll();
	for (int i = 0;i < AU_ACCELERATION_TOTAL_COUNT ;++i) {
		ayAccelerationStr.Add(AccelerationStr[i]);
	}
	return (TRUE);
}

CString CARCAccelerationUnit::GetAccelerationUnitString(ARCUnit_Acceleration auaUnit)
{
	if((int)auaUnit < 0 || auaUnit >= AU_ACCELERATION_TOTAL_COUNT)return _T("");
	return (AccelerationStr[int(auaUnit)]);
}

double CARCAccelerationUnit::ConvertAcceleration(ARCUnit_Acceleration auaUnitFrom, ARCUnit_Acceleration auaUnitTo, double dValue)
{
	if((int)auaUnitFrom < 0 || auaUnitFrom >= AU_ACCELERATION_TOTAL_COUNT || (int)auaUnitTo < 0 || auaUnitTo >= AU_ACCELERATION_TOTAL_COUNT)return (dValue);
	return ( dValue * (AccelerationyConversion[auaUnitFrom][0]) * (AccelerationyConversion[auaUnitTo][1]) );
}
