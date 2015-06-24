#ifndef __LANDFIELDPROCESSOR_H
#define __LANDFIELDPROCESSOR_H

#pragma once
#include "../common\containr.h"
#include "proclist.h"
#include "../common\Line.h"
#include "../Main/ProcRenderer.h"
#include "../common/ARCPath.h"
#include "../Common/SyncDateType.h"

//indicate which part of the stretch 
struct LandProcPart
{
	CString procName;            //name of the stretch
	Processor * pProc;			 //ptr to the stretch
	int part;                    //part of the stretch
	double pos;                  //relative position of the stretch path(0-1)
	LandProcPart()
	{
		double pos = 0;
		pProc = NULL;
		int part = -1;
	}
};
typedef std::vector<LandProcPart> LandProcPartArray;
/*
StretchProcessor,
IntersectionProcessor,
TurnoffProcessor,
UnderPassProcessor,
OverPassProcessor,
RoundAboutProcessor,
LaneAdapterProcessor,
LineParkingProcessor,
NoseInParkingProcessor,
YieldDeviceProcessor,
ParkingLotProcessor,
TrafficLightProceesor,
StopSignProcessor,
TollGateProcessor,
*/
class ProcessorList;
class LandfieldProcessor : public Processor , public SyncDateType
{
public:
	enum ProcessorProperty{ PropControlPoints,PropLaneNumber,PropLaneWidth,PropLinkingProces,PropLaneDir,PropClearance,/*for Parking procs*/PropSpotNumber,PropSpotWidth,PropSpotLength,PropSpotSlopAngle,};
public:
	LandfieldProcessor(void);
	virtual ~LandfieldProcessor(void);
	//processor type
	virtual int getProcessorType (void) const =0;
	//processor name
	virtual const char *getProcessorName (void) const =0;
	//whether the processor have propTpye property
	virtual bool HasProperty(ProcessorProperty propType) const{ return false; }
	
	//Properties: control points
	virtual ARCPath3 GetPath()const{ return m_path; }
	virtual void SetPath(const ARCPath3& path);

	//Properties: lane number
	virtual void SetLaneNum(int n);
	virtual int GetLaneNum()const{ return m_nLaneNum; }

	//Properties: Lane width
	virtual void SetLaneWidth(double width);	 
	virtual double GetLaneWidth()const{ return m_dLaneWidth; }

	//Properties: Clearance for OverPass and UunderPass
	virtual void SetClearance(double clearance);
	virtual double GetClearance()const { return m_dclearance; }
	
	//Properties: linking relations
	virtual int RefreshLinkedProcs(const ProcessorList& pProcList);
	virtual int RefreshLinkedProcsName(const ProcessorList& pProcList);
	virtual LandProcPartArray GetLinkedProcs()const{ return m_linkedProcs; }
	virtual void SetLinkProcParts(const LandProcPartArray& _landproc);
	
	//Properties: spot number
	virtual void SetSpotNumber(int spotN);
	virtual int GetSpotNumber()const { return m_nSpotNum;}

	//Properties : spot Length
	virtual void SetSpotLength(double spotlen);
	virtual double GetSpotLength()const{ return m_dSpotLen; }

	//Properties : spot width
	virtual void SetSpotWidth(double spotwidth);
	virtual double GetSpotWidth()const{ return m_dSpotWidth; }

	//Properties: spot slop angle
	virtual void SetSpotSlopAnlge(double angleDegree);
	virtual double GetSpotSlopAnlge()const{ return m_dSpotSlopAngle ;}

	//processor i/o	
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	// do offset for all the position related data. be careful with the derived class.
	virtual void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset );

	virtual void Rotate( DistanceUnit _degree );

	virtual void Mirror( const Path* _p ){}

	virtual void OffsetControlPoint(DistanceUnit _xOffset, DistanceUnit _yOffset , int ptidx);

	static LandfieldProcessor * NewProcessor(ProcessorClassList procType);
	
	virtual LandfieldProcessor * GetCopy() ;

		
	virtual void SetLaneDir(int iLane,int dir);
	virtual int GetLaneDir(int iLane)const;
protected:
	//int m_nFloor;
	ARCPath3 m_path;                   //control Points
	int m_nLaneNum;                    // lane number
	double m_dLaneWidth;               // lane width
	LandProcPartArray m_linkedProcs;   //linking processors
	double m_dclearance;
	std::vector<int> m_lanedir;

	//double m_rotateAngle;
	int m_nSpotNum;
	double m_dSpotLen;
	double m_dSpotWidth;
	double m_dSpotSlopAngle;

	
};
class ParkingSpot{
public:
	double width;
	double length;
	ARCVector2 dir;
	ARCVector3 pos;
	LandfieldProcessor * linkProc;
	ARCVector3 inPos;
	ARCVector3 outPos;
	bool bPush;
};

#endif