#pragma once
#include "LandsideStretchInSim.h"
//////////////////////////////////////////////////////////////////////////
class LandsideExternalNode;
class LandsideExternalNodeInSim : public LandsideResourceInSim, public LandsideLayoutObjectInSim
{
public:
	LandsideExternalNodeInSim(LandsideExternalNode* pNode);
	//virtual ResourceTypeInSim getResType()const{ return ResInSim_ExternalNode; };
	LandsideExternalNodeInSim *toExtNode(){ return this; }
	LandsideResourceInSim* IsResource(){ return this; }
	virtual CString print()const;
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;

	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const{}
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const{}

	//LandsideExternalNode* mpInput;

	const CPoint2008& getPos()const{ return mPos;} 

	bool IsOrign()const;
	bool IsDest()const;

	LandsideExternalNode* getExtNode()const{ return (LandsideExternalNode*)getInput(); }
protected:
	std::vector<LandsideLaneEntry*> m_vLaneEntries;
	std::vector<LandsideLaneExit*> m_vLaneExits;
	CPoint2008 mPos;

};



