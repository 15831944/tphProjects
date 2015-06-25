#include "StdAfx.h"
#include ".\alt_typestring.h"

CString ALT_TypeString::LandisdeObject[]=
{
		_T("Stretch"),
		_T("Intersection"),
		_T("ParkingLot"),
		_T("CurbSide"),
		_T("BusStation"),
		_T("Entry Pos"),
		_T("Crosswalk"),
		_T("Walkway"),
		_T("Roundabout"),
		_T("Portal"),
		_T("StretchSegment"),
		_T("TrafficLight"),
		_T("StopSign"),
		_T("TollGate"),
		_T("YieldSign"),
		_T("TaxiQueue"),
		_T("TaxiPool"),
		_T("SpeedSign"),
};



CString ALT_TypeString::Get(ALTOBJECT_TYPE t)
{
	if(t < ALT_LTYPE_END && t >= ALT_LTYPE_START){
		return LandisdeObject[t-ALT_LTYPE_START];
	}


	return _T("ALTOBJECT");
}
	
