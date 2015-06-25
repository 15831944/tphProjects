#pragma once
#include "../EngineDll.h"
#include "../../Common/ref_ptr.h"
#include "../../Common/Referenced.h"

class IntersectionNodeInSim;
class AirsideFlightInSim;
class ClearanceItem;
class Clearance;

class ENGINE_TRANSFER TempParkingInSim : public Referenced
{
public:
	typedef ref_ptr<TempParkingInSim> RefPtr;
	enum Type{  TempStand, TempNode, TempSegments };

	virtual bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, const DistanceUnit& radius, Clearance& newClearance)=0;
	virtual IntersectionNodeInSim* GetInNode()const =0 ;
	virtual IntersectionNodeInSim* GetOutNode(AirsideFlightInSim*pFlight)=0;
	
	virtual Type GetParkingType()const =0;

	virtual void ReleaseLock(AirsideFlightInSim* pFlight) =0 ;
	virtual bool TryLock(AirsideFlightInSim* pFlight) =0 ;
	virtual bool GetLock(AirsideFlightInSim* pFlight) =0 ;
	virtual CString PrintTempParking()const =0 ;

	virtual void notifyCirculate(AirsideFlightInSim* pFlight, const ElapsedTime& t){} //for giving way to other flight
};



