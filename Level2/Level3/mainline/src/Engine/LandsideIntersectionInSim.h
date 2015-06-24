#pragma once
#include ".\core\SAgent.h"
#include ".\core\SSignal.h"
#include "LandsideStretchInSim.h"
//////////////////////////////////////////////////////////////////////////
//class LinkStateChangeSignal:public SSignal
//{
//public:
//	CString getDesc(){ return _T("Link State Change"); }
//};

class LandsideIntersectLinkGroupInSim;
class LandsideIntersectLaneLinkInSim : public LandsideResourceInSim, public LaneNodeLinkageInSim
{
public:
	LandsideIntersectLaneLinkInSim(LandsideIntersectionInSim* pIntersect,LandsideLaneExit* pLaneExit, LandsideLaneEntry* pLaneEntry ,int nGroupID);

	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	virtual LandsideIntersectLaneLinkInSim* toIntersectionLink(){ return this; }

	//init conflict informations should called after all link is created
	//void InitConflict(double dTolerance);
	virtual LandsideVehicleInSim* GetAheadVehicle(LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist);
	//virtual LandsideVehicleInSim* CheckSpaceConflict(LandsideVehicleInSim* pVeh, const ARCPolygon& poly);
	virtual LandsideVehicleInSim* CheckPathConflict(LandsideVehicleInSim* pVeh, const CPath2008& path,DistanceUnit&dist);
		

	virtual void OnVehicleEnter(LandsideVehicleInSim* pVehicle, DistanceUnit dist,const ElapsedTime& t);
	virtual void OnVehicleExit(LandsideVehicleInSim* pVehicle,const ElapsedTime& t);
	//////////////////////	
	
	int GetGroupID()const{return m_nGroupID;}

	void SetPath(const CPath2008& path){ m_path = path; m_dist = m_path.GetTotalLength(); }
	const CPath2008& GetPath()const{ return m_path; }

	LandsideIntersectionInSim* getIntersection()const{ return mpParent; }
	virtual double getDistance()const{ return m_dist; }
	virtual LandsideLaneNodeInSim* getFromNode()const{ return mpExit; }
	virtual LandsideLaneNodeInSim* getToNode()const{ return mpEntry; };

	CPath2008 m_path;

	bool IsClosed()const;
	LandsideIntersectLinkGroupInSim* getGroup()const;

	
protected:
	LandsideIntersectionInSim * mpParent;
	double m_dist;
	LandsideLaneEntry* mpEntry;
	LandsideLaneExit* mpExit;

	int m_nGroupID;

	//get conflict vehicle with the 
	LandsideVehicleInSim* GetConflictVehicle(LandsideVehicleInSim* pVehicle, double distInLink,
		double& sep)const;

	//get  conflict with the same dest node
	//LandsideVehicleInSim* GetAheadVehicle(LandsideVehicleInSim* pVehicle, LandsideIntersectLaneLinkInSim* pOtheLink, double distInLink,
	//	double& sep)const;

	//conflict map with other linkage, other link pos
	struct IntersectWithOtherLinkPos
	{
		double dist;

		LandsideIntersectLaneLinkInSim* pOtherLink;
		double distInOtherLink;
	};
	typedef std::vector<IntersectWithOtherLinkPos> ConflitMap;
	ConflitMap mConflictMap;

};

class LandsideIntersectLinkGroupInSim:public SAgent
{
public:
	LandsideIntersectLinkGroupInSim(LandsideIntersectionInSim *pVehicle,int nGroupID);
	void InitLogEntry(OutputLandside* pOutput );
	void WriteLog(const  ElapsedTime& t);
	void SetCtrlTime(TrafficCtrlTime &ctrlTime);
	TrafficCtrlTime GetCtrlTime(){return m_ctrlTime;}

	void SetState(LinkGroupState eState);
	LinkGroupState GetState()const{ return m_eState; }	

	void SetFirstCloseTime(int nTime){m_nFirstCloseTime=nTime;}

	int GetGroupID()const{return m_nGroupID;}

	void SetEndTime(ElapsedTime tTime){m_tEndTime=tTime;}
	void AddLinkCrossWalk(CCrossWalkInSim *pCrossWalk){m_vIinkCrossWalk.push_back(pCrossWalk);}

	virtual void OnActive(CARCportEngine*pEngine);
	virtual void OnTerminate(CARCportEngine*pEngine);

	void End(const ElapsedTime& t);
	LandsideIntersectionInSim* getIntersection()const{ return mpParent; }
protected:
private:
	TrafficCtrlTime m_ctrlTime;
	int m_nGroupID;
	LinkGroupState m_eState;
	bool m_bFirstTimeActive;
	int m_nFirstCloseTime;//the close time before first active	

	LandsideIntersectionInSim * mpParent;

	ElapsedTime m_tEndTime;

	std::vector<CCrossWalkInSim *> m_vIinkCrossWalk;
public:
	IntersectionLinkGroupLogEntry m_logEntry;

};

class LandsideIntersectionNode;
class LandsideIntersectionInSim : public LandsideResourceInSim, public LandsideLayoutObjectInSim
{
public:
	LandsideIntersectionInSim(LandsideIntersectionNode* pNode);
	virtual ~LandsideIntersectionInSim(){ Clear(); }
	virtual CString print()const;

	LandsideIntersectionInSim* toIntersection(){ return this; }
	LandsideResourceInSim* IsResource(){ return this; }
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;;

	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const{};
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const;
	//lane node linkage

	int GetLinkageCount()const{ return (int)m_vLinkages.size(); }
	LandsideIntersectLaneLinkInSim* GetLinkage(int idx)const{ return  m_vLinkages[idx]; }
	LandsideIntersectLaneLinkInSim* GetLinkage(LandsideLaneExit* pExit,LandsideLaneEntry* pEntry)const;

	const ALTObjectID& GetIntersectionName()const{return getInput()->getName();}
	

	void GenerateCtrlEvents(CIntersectionTrafficControlIntersection *pTrafficCtrlNode,LandsideResourceManager* pResManager,OutputLandside* pOutput,ElapsedTime startTime,ElapsedTime endTime/*=0L*/);

	std::vector<LandsideIntersectLinkGroupInSim *> &GetLinkGroupInSimList(){return m_vLinkGroups;}

	LandsideIntersectLinkGroupInSim* GetLinkGroupById(int idx)const;	
	//get ahead vehicle
	
	void Kill(OutputLandside* pOutput, const ElapsedTime& t);
	void Deactive();
	//
protected:
	std::vector<LandsideIntersectLaneLinkInSim*> m_vLinkages;
	std::vector<LandsideIntersectLinkGroupInSim *> m_vLinkGroups;
	void Clear();
	
	//conflict map
	bool m_bKilled;
};


