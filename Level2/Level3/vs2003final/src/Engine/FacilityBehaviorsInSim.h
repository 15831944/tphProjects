#pragma once

#include "Common\ALTObjectID.h"
#include "Common\ALT_TYPE.h"


class CFacilityBehaviorList;
class CFacilityBehaviorLinkageList;
class LandsideResourceManager;
class CWalkwayInSim;
class ProcessorID;
class LandsideResourceInSim;
class LandsideFacilityLayoutObjectList;
class Terminal;


class CFacilityBehaviorsInSim
{
public:
	CFacilityBehaviorsInSim(void);
	~CFacilityBehaviorsInSim(void);

public:
	void Initialize(LandsideFacilityLayoutObjectList* pObjectList,LandsideResourceManager* resManagerInst);

	void GetTerminalLinkageProcName(CFacilityBehaviorLinkageList* pLinkageList,Terminal* pTerminal,const ALTObjectID& objID,std::vector<ALTObjectID>& vProcName);
	CFacilityBehaviorList* getFacilityBehavior(){return m_pTerminalLinkage;}
	LandsideResourceInSim* GetLandsideTerminalLinkage( const ProcessorID& procID );
	LandsideResourceInSim* GetLandsideTerminalLinkage(ALTOBJECT_TYPE resType, const ProcessorID& procID );
	CWalkwayInSim * getLandsideTerminalWalkwayLinkage( const ProcessorID& procID );
	CWalkwayInSim* GetWalkwayTerminalLinkage(std::vector<CWalkwayInSim *>& vWalkwayInSim, std::vector<ALTObjectID>& vLinkTermProcID );
	void GetTerminalProcLinkedWithParkingLot( ALTObjectID objParkingLot, int nLevel, std::vector<ALTObjectID>& vLinkTermProcID );
protected:
	CFacilityBehaviorList* m_pTerminalLinkage;
	LandsideResourceManager* m_resManagerInst;
	LandsideFacilityLayoutObjectList* m_pLandsideObjectList;
};
