#pragma once
#include "Common/elaptime.h"
#include "EngineDll.h"

/**
	Abstract class, mobile element behavior interfaces.
	DONOT add any data member to this class!.
*/
class ENGINE_TRANSFER MobElementBehavior
{
public:
	MobElementBehavior(){}
	virtual ~MobElementBehavior(){}

	enum BehaviorType
	{
		AirsideBehavior = 0,
		LandsideBehavior,
        TerminalBehavior,
		OnboardBehavior
	};
	enum MobDir
	{
		FORWARD,
		BACKWARD,
		LEFTSIDLE,
		RIGHTSIDLE
	};

	// for non passenger sidle walk angle	
	double GetRotateAngle(MobDir emWalk)const
	{
		if (emWalk == BACKWARD)
		{
			return 180.0;
		}
		else if (emWalk == LEFTSIDLE)
		{
			return -90.0;
		}
		else if (emWalk == RIGHTSIDLE)
		{
			return 90.0;
		}

		return 0.0;
	}
public:
	virtual int performanceMove(ElapsedTime p_time,bool bNoLog) = 0;
	virtual BehaviorType getBehaviorType()const = 0;

	//It writes next track to ElementTrack log.
	// in:
	// _bBackup: if the log is backwards.
	virtual void writeLog (ElapsedTime time, bool _bBackup, bool bOffset = true ) = 0;
};