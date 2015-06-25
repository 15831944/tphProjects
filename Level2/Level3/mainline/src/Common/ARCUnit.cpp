#include "stdafx.h"
#include "ARCUnit.h"


const CString ARCUnit::LengthUnitLongStr[LUnitCount]={
	"millimeters","centimeter","meter","kilometer","mile","nautical mile","inch","feet","level",
};
const CString ARCUnit::LengthUnitStr[LUnitCount]={
	"mm","cm","m","km","mile","nm","inch","ft","level",
};

const CString ARCUnit::VeclocityUnitStr[VUnitCount] = {"",};
const CString ARCUnit::VeclocityUnitLongStr[VUnitCount] = {"",};

const CString ARCUnit::TimeUnitStr[TUnitCount] = {"",};
const CString ARCUnit::TimeUnitLongStr[TUnitCount] ={"",};

const CString ARCUnit::WeightUnitStr[WUnitCount] = {"",};
const CString ARCUnit::WeightUnitLongStr[WUnitCount] = {"",};

const double ARCUnit::LengthCon[LUnitCount][2] = {
	{ 0.001, 1000.0 },          // 1mm == 0.001 m ; 1000 mm == 1 m
	{ 0.01, 100 },
	{ 1.0, 1.0 },
	{ 1000.0, 0.001 },
	{ 1609.344, 1/1609.344 },
	{ 1852, 1.0/1852 },
	{ 0.0254,1/0.0254 },
	{ 0.3048,1/0.3048 },
	{ 30.48, 1/30.48 },
};

const double ARCUnit::VelocityCon[VUnitCount][2]={
	{ 1.0, 1.0 },
	{ 0.514444444, 1/0.514444444 },
	{ 0.01,	100.0 },
	{ 1.0/3.28084, 3.28084},
	{10.0/36.0,	36.0/10.0 },

};

const double ARCUnit::TimeCon[TUnitCount][2] = {
	{ 1.0, 1.0 },
	{ 60,1/60.0 },{ 3600,1/3600.0 },{ 86400,1/86400.0 },
};

const double ARCUnit::WeightCon[WUnitCount][2] = {
	{ 1.0, 1.0 },
	{ 0.001,1000.0 },{ 0.45359237,1.0/0.45359237 },
};


double ARCUnit::ConvertLength(double val , LengthUnit fromUnit, LengthUnit toUnit /* = M */){
	if( M == toUnit ){
		return val * LengthCon[fromUnit][0];
	}else{
		return ConvertLength(val,fromUnit) * LengthCon[toUnit][1];
	}	
}

double ARCUnit::ConvertVelocity(double val, VelocityUnit fromUnit, VelocityUnit toUnit /* = MPS */){
	
	if( MpS == toUnit ){
		return val * VelocityCon[fromUnit][0];
	}else{
		return ConvertVelocity(val,fromUnit) * VelocityCon[toUnit][1];
	}	
}

double ARCUnit::ConvertTime(double val ,TimeUnit fromUnit, TimeUnit toUnit /* = SEC */){

	if( SECOND == toUnit ){
		return val * TimeCon[fromUnit][0];
	}else{ 
		return ConvertTime(val,fromUnit) * TimeCon[toUnit][1];
	}	
}

double ARCUnit::ConvertWeight(double val, WeightUnit fromUnit, WeightUnit toUnit /* = KG */){
	
	if(KG == toUnit){
		return val * WeightCon[fromUnit][0];
	}else{
		return ConvertWeight(val,fromUnit) * WeightCon[toUnit][1];
	}

}

double ARCUnit::LBSToKG( double d )
{
	return ARCUnit::ConvertWeight(d,ARCUnit::POUND,ARCUnit::KG);
}

double ARCUnit::KgsToLBS( double dIn )
{
	return ARCUnit::ConvertWeight(dIn,ARCUnit::KG,ARCUnit::POUND);
}