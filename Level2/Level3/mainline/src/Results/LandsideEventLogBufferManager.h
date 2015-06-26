#pragma once

#include "../Common/LANDSIDE_BIN.h"
#include "results\baselog.h"

#include "landside\LandsideVehicleLogEntry.h"
#include "landside\LandsideLinkGroupLog.h"
#include "landside\CrossWalkLogEntry.h"
#include "landside\LandsideCrossWalkLog.h"
class CLandsideSimLogs;
class LandsideVehicleLog;
class CTermPlanDoc;
enum  ANIMATION_MODE;
class CLandsideEventLogBuffer;
class CLandsideEventLogBufferManager
{
public:
	CLandsideEventLogBufferManager();
	~CLandsideEventLogBufferManager();
	void InitVehicleLogBuf(LandsideVehicleLog* plog);
public:	
	// just if need to read data.
	CLandsideEventLogBuffer* loadVehicleDataIfNecessary( long _lCurTime, bool bAnimForward,CTermPlanDoc* pTermDoc );

	typedef std::vector<IntersectionLinkGroupLogEntry> LinkGroupLogBuff;
	typedef std::vector<CCrossWalkLogEntry> CrossWalkLogBuff;
	LinkGroupLogBuff m_vLinkGroupLogBuff;
	CrossWalkLogBuff m_vCrossWalkLogBuff;
	
	//void LoadDataIfNecessary(const LandsideVehicleLog& logIndex, long _lCurTime);
	void LoadLinkGroupDataIfNecessary(const LandsideLinkGroupLog &logIndex,long _lCurTime);
	void LoadCrossWalkDataIfNecessary(const LandsideCrossWalkLog &logIndex,long _lCurTime);
protected:	
	bool bFirstTimeLoad;
	bool bFirstLoadGroupData;
	bool bFirstLoadCrossData;
	long m_detaTime;
	CLandsideEventLogBuffer* m_pCurrentLogBuffer;

};