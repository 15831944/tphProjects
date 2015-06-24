#pragma once
#include "../../Common/Point2008.h"
#include "../EngineDll.h"
#include "TaxiwayResource.h"
#include "AirsideResource.h"

class CTaxiInterruptLine;
class TaxiInterruptLineList;
class AirportResourceManager;

class TaxiInterruptLineInSimList;
class CTaxiInterruptTimeItem;
class ENGINE_TRANSFER TaxiInterruptLineInSim: public AirsideResource
{
public:
	TaxiInterruptLineInSim(CTaxiInterruptLine* pHoldShortLine);
	~TaxiInterruptLineInSim(void);

	TaxiwaySegInSim* GetAttachedTaxiwaySegment(){ return m_pTaxiwaySeg; }
	void SetAttachedTaxiwaySegment(TaxiwaySegInSim* pTaxiwaySeg) { m_pTaxiwaySeg = pTaxiwaySeg; }

	CPoint2008 GetPosition();
	CString GetName();
	CTaxiInterruptLine* GetInterruptLineInput();

	virtual ResourceType GetType()const;
	virtual CString GetTypeName()const;
	virtual CString PrintResource()const;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const; 

	void SetWaitFlight(AirsideFlightInSim* pFlight, const ElapsedTime& tTime);
	AirsideFlightInSim* GetWaitFlight()const{ return m_pCurWaitFlight; }

	bool bInterruptFlight(AirsideFlightInSim* pflt,int StartNodeId, ElapsedTime& inttime);

private:
	ElapsedTime GetFltHoldTime(AirsideFlightInSim* pflt, CTaxiInterruptTimeItem* pSettingItem);
	
private:
	CTaxiInterruptLine* m_pHoldShortLine;		//input data
	TaxiwaySegInSim* m_pTaxiwaySeg;				//attached taxiway segment
	AirsideFlightInSim* m_pCurWaitFlight;       //current waiting flight
};

//////////////////////////////////////////////////////////
class ENGINE_TRANSFER TaxiInterruptLineInSimList
{
public:
	TaxiInterruptLineInSimList();
	~TaxiInterruptLineInSimList();

	void Init(AirportResourceManager* pAirportRes);

	TaxiInterruptLineInSim* GetHoldShortLineByIdx(int nIdx);
	TaxiInterruptLineInSim* GetHoldShortLineById(int nId) const;

	int GetCount();

private:
	TaxiInterruptLineList* m_pHoldShortLines;			//input data
	std::vector<TaxiInterruptLineInSim*> m_vHoldShortLines;			//hold short line set
};
