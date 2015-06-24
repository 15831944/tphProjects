#pragma once
#include "../../Common/Point2008.h"
#include "../EngineDll.h"
#include "../../InputAirside/StartPosition.h"
#include "AirsideResource.h"
#include "TaxiwayResource.h"

class CStartPosition;
class AirportResourceManager;
class ENGINE_TRANSFER StartPositionInSim : public AirsideResource
{
public:
	StartPositionInSim(CStartPosition* pPosition);
	~StartPositionInSim(void);

	TaxiwaySegInSim* GetAttachedTaxiwaySegment(){ return m_pTaxiwaySeg; }
	void SetAttachedTaxiwaySegment(TaxiwaySegInSim* pTaxiwaySeg) { m_pTaxiwaySeg = pTaxiwaySeg; }

	CPoint2008 GetPosition();
	CString GetName();
	CStartPosition* GetStartPosition();

	virtual ResourceType GetType()const;
	virtual CString GetTypeName()const;
	virtual CString PrintResource()const;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const; 

private:
	CStartPosition::RefPtr m_pStartPosition;		//input data
	TaxiwaySegInSim* m_pTaxiwaySeg;				//attached taxiway segment
	CPoint2008 m_Position;		//position
};
