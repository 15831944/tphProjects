#pragma once

#include "ARCLogItem.h"
#include <string>

class AirsideVehicleLogDesc
{
public:
	long id;
	long vehicletype;
	long vehiclebasetype;
	long poolNO; //pool id


	double vehiclelength;
	double vehiclewidth;
	double vehicleheight;


	DECLARE_SERIALIZE
};

class AirsideVehicleLogItem : public ARCLogItem
{
public:	
	AirsideVehicleLogDesc mVehicleDesc;

	DECLARE_SERIALIZE
};

