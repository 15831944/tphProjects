#pragma once

#include "LandsideBaseBehavior.h"

class LandsideSimulation;
class PaxLandsideBehavior : public LandsideBaseBehavior
{
public:
	PaxLandsideBehavior(Person* p);
	
	virtual ~PaxLandsideBehavior();

public:
	void InitializeVisitorBehavior();

	void WriteLogEntry(ElapsedTime time, bool _bBackup = false, bool bOffset = true );

	virtual void setDestination (const ARCVector3& p,MobDir emWalk = FORWARD);

	LandsideSimulation *getLandsideSimulation();

	LandsideResourceInSim * getDestResource() const;
	void setDestResource(LandsideResourceInSim *pDestResource);

protected:
	void WriteFollowLog(ElapsedTime time, bool _bBackup, bool bOffset = true);

	void writeVisitorLog(const ARCVector3& pax_pre_location, ElapsedTime time, bool _bBackup = false, bool bOffset  = true );

	void UpdateEntryTime(const ElapsedTime& eEntryTime);


protected:
	LandsideSimulation* m_pLandsideSim;
protected:
	LandsideResourceInSim *m_pDestResource;
};

