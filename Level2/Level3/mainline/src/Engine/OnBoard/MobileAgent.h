#pragma once
/*
	need improvement in the future, add more multi-agent methodology to agent object.

	By now, Mobile Agents is individual objects. need implement as 
	ALTO Mobile Elements in the future. 

	DONOT add any concrete functionality function or agent action in MobileAgent class.
	just keep MobileAgent's properties data. 
*/     
#include "OnBoardDefs.h"
#include "../mobile.h"
#include "../Person.h"
#include "Common/NewPassengerType.h"

typedef CPassengerType MobAgentType;

class LocatorSite;
class carrierSpace;
class MobAgentMovementPath;
class OnBoardSimEngineConfig;
class carrierSeat;
class PathFinder;
class CARCportEngine;
class MobileAgent : public Person
{
public:
	//Kinetics and space
	typedef struct{
		double mVarAgentSpeed;
		double mVarFreeInStep;
		double mVarCongesitionInStep;
		double mVarQueueInStep;
		double mVarFreeSideStep;
		double mVarCongesitionSideStep;
		double mVarQueueSideStep;
		double mVarInteractionDist;
		double mVarAffinityFreeDist;
		double mVarAffinityCongestionDist;
		double mVarSqeezability;
	}KineticsAndSpace;

	// Behavior Data
	typedef struct{
		double mVardDocDispTime;
		double mVarWayFindingIntervention;
		double mVarUnimpededStateTransTime;
		
		int    mVarTransitionTimeModifier_Inbibitor;
		int    mVarTransitionTimeModifier_MTrigger;
		double mVarTransitionTimeModifier_Factor;
		
		double mVarTargetLocation;
		double mVarUnexpectedBehavior;
		double mVarSeatingPerformance;
	}BehaviorData;


	// Artificial Intelligence
	typedef struct  
	{
		//Movement Detector.
		PathFinder *mDetactor;

		//Interaction (vs Neighbors).

		//Human Sensibility.

	}AI;


	// Interaction space

protected:
	
	MobAgentType			mMobAgentType;

	carrierSpace			*m_pCarrierSpace;
	OnBoardSimEngineConfig	*m_pConfig;

	KineticsAndSpace		mKineticsAndSpace;
	BehaviorData			mBehaviorData;
	AI						mHuman;

	MobAgentMovementPath	*m_pMovePath;

private:
	carrierSeat				*m_pSeat;
	LocatorSite				*m_pLocatorSite;
	LocatorSite				*m_pFinalLocatorSite;

public:
	MobileAgent(MobLogEntry& p_entry, CARCportEngine* pEngine );
	~MobileAgent();

	virtual void assignParticularConfig() = 0;

	CString screenMobAgent();

	bool isDeplanement();
	bool isEnplanement();

	void setCarrierSpace(carrierSpace* pCarrier);
	carrierSpace* getCarrierSpace();

	OnBoardSimEngineConfig* getConfig();

	virtual void writeLogEntry (ElapsedTime time, bool _bBackup, bool bOffset = true );
	virtual void flushLog( ElapsedTime, bool);
	virtual void kill (ElapsedTime killTime);

	void assignSeat( carrierSeat* pSeat );
	carrierSeat* getAssignedSeat();
	
	LocatorSite* getLocatorSite();
	void setLocatorSite(LocatorSite* pMobAgentSite);
	void initBirthLocation();

	LocatorSite* getFinalLocatorSite();
	void setFinalLocatorSite(LocatorSite* pFinalSite);

	MobAgentMovementPath* getMovementPath();
	MobAgentMovementPath* getMovementPathWithInteraction();

	void assignIntelligence(PathFinder* pDetector);

	// functionality interfaces.
	virtual ElapsedTime moveTime (void) const;
	virtual bool findMovementPath() = 0;

private:
	void assignCommonConfig();


};



