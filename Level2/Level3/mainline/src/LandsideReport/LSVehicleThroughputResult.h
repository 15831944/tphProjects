#pragma once
#include "LandsideBaseResult.h"
#include "..\Common\LANDSIDE_BIN.h"
#include "..\Landside\LandsideVehicleLogEntry.h"
#include "..\Landside\LandsideVehicleLog.h"

class LSVehicleThroughputParam;
class LANDSIDEREPORT_API LSVehicleThroughputResult : public LandsideBaseResult
{
public:

	class ThroughputResourceData
	{
	public:
		ThroughputResourceData()
			:m_nVehicleID(-1)
			,m_nEventIndex(0)
		{

		}
		~ThroughputResourceData()
		{

		}

		bool operator == (const ThroughputResourceData& rsData)const
		{
			if (m_nVehicleID != rsData.m_nVehicleID)
				return false;

			if (m_nEventIndex != rsData.m_nEventIndex)
				return false;

			return true;
		}

		int m_nEventIndex;//for checking sequence event
		int m_nVehicleID;
	};




	
	LSVehicleThroughputResult(void);
	~LSVehicleThroughputResult(void);

	virtual void RefreshReport(LandsideBaseParam * parameter);

protected:
	void CaculateVehicleCrossPortal(LandsideVehicleLogEntry& element,LandsideBaseParam* pParameter);
	virtual void CaculateVehicleCorssResult(const std::vector<LandsideVehicleEventStruct>& vPointEvent,int nVehicleID,LSVehicleThroughputParam* pThroughputPara) = 0;

};
