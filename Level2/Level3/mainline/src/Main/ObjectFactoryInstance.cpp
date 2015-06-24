#include "stdafx.h"
#include "ObjectFactoryInstance.h"
#include "landside/LandsideIntersectionNode.h"
#include "landside/LandsideStretch.h"
#include "landside/LandsideParkingLot.h"
#include "landside/LandsideCurbSide.h"
#include "Landside/LandsideExternalNode.h"
#include "Landside/LandsideCrosswalk.h"
#include "Landside/LandsideWalkway.h"
#include "Landside/LandsideBusStation.h"

#define STATIC_FACTORY_INSTANCE(TCLASS) static TCLASS TCLASS##Inst = TCLASS();


//void ObjectFactoryInstance::Init()
//{
//	STATIC_FACTORY_INSTANCE(LandsideCurbSideFactory)
//	STATIC_FACTORY_INSTANCE(LandsideParkingLotFactory)
//	STATIC_FACTORY_INSTANCE(LandsideStretchFactory)
//	STATIC_FACTORY_INSTANCE(LandsideIntersectionFactory)
//	STATIC_FACTORY_INSTANCE(LandsideExternalNodeFactory)
//	STATIC_FACTORY_INSTANCE(LandsideCrosswalkFactory)
//	STATIC_FACTORY_INSTANCE(LandsideWalkwayFactory)
//	STATIC_FACTORY_INSTANCE(LandsideBusStationFactory)
//	
//}