//----------------------------------------------------------------------------------
//						Information:		construct graph use walkway and crosswalk
//						Author:				sky.wen
//						Date:				Jun	29, 2011
//						Purpose:			Provide some engine logic method
//----------------------------------------------------------------------------------
#pragma once
#include "LandsideObjectInSim.h"
class LandsideBaseBehavior;
class LandsideWalkway;
class CPath2008;
class LandsideTrafficGraphVertex;
class CFreeMovingLogic;
class ElapsedTime;
//----------------------------------------------------------------------------------
//Summary:
//		middle layer to call input data
//----------------------------------------------------------------------------------
class CWalkwayInSim : public CLandsideWalkTrafficObjectInSim
{
public:
	CWalkwayInSim(LandsideWalkway* pWalkway);
	~CWalkwayInSim();

	virtual CWalkwayInSim* toWalkway(){ return this; }

	//------------------------------------------------------------------------
	//Summary:
	//		retrieve traffic type
	//------------------------------------------------------------------------
	virtual CLandsideWalkTrafficObjectInSim::TrafficObjectType GetTrafficType()const;

	//-----------------------------------------------------------------------
	//Summary:
	//		retrieve crosswalk center path
	//-----------------------------------------------------------------------
	virtual CPath2008 GetCenterPath()const;

	//----------------------------------------------------------------------
	//Summary:
	//		retrieve crosswalk right path
	//----------------------------------------------------------------------
	virtual CPath2008 GetRightPath()const;

	//----------------------------------------------------------------------
	//Summary:
	//		retrieve crosswalk left path
	//----------------------------------------------------------------------
	virtual CPath2008 GetLeftPath()const;

	//----------------------------------------------------------------------
	//Summary:
	//		retrieve input walkway
	//-------------------------------------------------------------------------
	LandsideWalkway* GetInputWalkway() const;

	//----------------------------------------------------------------------
	//Summary:
	//		step on walkway
	//Parameter:
	//		pLandsideBehavior[in]: person behavior
	//		time[out]: passed time
	//		vertex: move on vertex
	//		pFreeMoveLogic[in]: person free moving logic
	//---------------------------------------------------------------------
	void stepIt(LandsideBaseBehavior* pLandsideBehavior,const LandsideTrafficGraphVertex& vertex,CFreeMovingLogic* pFreeMoveLogic,ElapsedTime& eTime);

private:
	LandsideWalkway*  m_pWalkway; //input walkway
};

