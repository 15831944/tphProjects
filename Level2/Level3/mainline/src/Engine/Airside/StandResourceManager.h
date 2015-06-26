#pragma once



#include "ResourceManager.h"
#include "../../InputAirside/StandCriteriaAssignment.h"
#include "StandInSim.h"

class TaxiwayResourceManager;
class StandCriteriaAssignmentList;
class ALTObjectGroup;
class ElapsedTime;
class OutputAirside;
class ENGINE_TRANSFER StandResourceManager : public ResourceManager
{
public:

	bool Init(int nPrjID, int nAirportID,OutputAirside *pOutput);
	bool InitRelations(IntersectionNodeInSimList& nodeList,TaxiwayResourceManager * pTaxiwayRes);
	bool InitStandCriteriaAssignment( int nPrjID,CAirportDatabase* pAirPortdb);

	int GetStandCount()const{ return m_vStands.size(); }

	StandInSim * GetStandByIdx(int idx){ return m_vStands.at(idx).get(); }
	
	StandInSim * GetStandByID(int id);
	StandInSim * GetStandByName(const ALTObjectID& nameID);

	ElapsedTime GetNextNeedTime(StandInSim* pStand, const ElapsedTime& curTime);
	StandInSim* GetStandNeedNotWithin(const ALTObjectGroup& standGrp, const ElapsedTime& curTime, const ElapsedTime& withInTime);

	void GetStandListByStandFamilyName(const ALTObjectID& nameID,std::vector<StandInSim*>& vStands);
protected:
	std::vector<StandInSim::RefPtr> m_vStands;
	StandCriteriaAssignmentList m_StandCriteriaAssignmentList;
};