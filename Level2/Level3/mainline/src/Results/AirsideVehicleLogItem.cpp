#include "StdAfx.h"
#include ".\airsidevehiclelogitem.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

void AirsideVehicleLogDesc::serialize(boost::archive::text_iarchive &ar, const unsigned int file_version)
{
	 ar & id;
	 ar &  vehicletype;
	 ar & poolNO; //pool id

	 ar & vehiclelength;
	 ar & vehiclewidth;
	 ar & vehicleheight;

}
void AirsideVehicleLogDesc::serialize(boost::archive::text_oarchive &ar, const unsigned int file_version)
{
	 ar & id;
	 ar &  vehicletype;
	 ar & poolNO; //pool id

	 ar & vehiclelength;
	 ar & vehiclewidth;
	 ar & vehicleheight;

}
//

void AirsideVehicleLogItem::serialize(boost::archive::text_iarchive &ar, const unsigned int file_version)
{
	ARCLogItem::serialize(ar,file_version);
	ar & mVehicleDesc;
}
void AirsideVehicleLogItem::serialize(boost::archive::text_oarchive &ar, const unsigned int file_version)
{
	ARCLogItem::serialize(ar,file_version);
	ar & mVehicleDesc;
}