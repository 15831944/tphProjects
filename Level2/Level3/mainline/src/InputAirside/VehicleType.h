#ifndef _VEHICLETYPE_HEADER_
#define _VEHICLETYPE_HEADER_

#pragma once 
#include "VehicleSpecificationItem.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CVehicleType
{
public:
	CVehicleType(void);
	~CVehicleType(void);
	CString ScreenPrint( BOOL _HasReturn = FALSE ) const;
public: 
	CString m_strVehicleName;

	CVehicleSpecificationItem m_vehicleType;
	BOOL m_bNotVehicleType; 

	BOOL m_bBirthTimeApplied;
	COleDateTime m_odtStartBirthTime;
	COleDateTime m_odtEndBirthTime;
	BOOL m_bNotBirthTime;

	BOOL m_bDeathTimeApplied;
	COleDateTime m_odtStartDeathTime;
	COleDateTime m_odtEndDeathTime;
	BOOL m_bNotDeathTime;
};

#endif
