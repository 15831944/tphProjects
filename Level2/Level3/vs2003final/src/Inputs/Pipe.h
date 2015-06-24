#pragma once
#include <CommonData\PipeBase.h>
#include "engine\MovingSideWalk.h"
#include "engine\proclist.h"
#include "PipeInterSectionPoint.h"
#include "TimePointOnSideWalk.h"
#include "CrossWalkAttachPipe.h"

class InputTerminal;
class CPointToPipeXPoint;
class CGraphVertex;
class LandsideSimulation;

typedef std::vector<ProcessorID> MOVINGSIDEWALKID;
typedef std::vector<CPipeToPipeXPoint> PIPEINTERSECTIONPOINTLIST;

class CPipe: public CPipeBase
{
private:
	InputTerminal *m_pTerm;
	MOVINGSIDEWALKID m_vSideWalkInPipe;

	std::vector<PIPEINTERSECTIONPOINTLIST> m_vPipeIntersectionPoint;
	PTONSIDEWALK m_vPointTimeOnSideWalk;
	PIPEINTERSECTIONPOINTLIST m_vEntryPointList;
	std::vector<CrossWalkAttachPipe> m_vCrossWalkAttach;
public:
	CPipe(InputTerminal* _pTerm);
	virtual ~CPipe();
public:


	//crosswalk attach------------------------------------------------------
	int GetAttachCrosswalkCount()const;
	void AddAttachCrosswalk(const CrossWalkAttachPipe& attachPipe);
	void RemoveAttachCrosswalk(LandsideCrosswalk* pCorsswalk);
	void RemoveAttachCrosswalk(int nIdx);
	CrossWalkAttachPipe* GetAttachCrosswalk(LandsideCrosswalk* pCorsswalk);
	CrossWalkAttachPipe& GetAtachCrosswalk(int nIdx);

	void GetPipeCrosswalkRegion( POLLYGONVECTOR& _regions, POLLYGONVECTOR& _texcoord ) const;
	//crosswalk attach------------------------------------------------------
	void WritePipePax(LandsideSimulation* pLandsideSim,const Point& pt, const Point& ptDest,
		int iPaxID,const ElapsedTime& eTime,const ElapsedTime& tMoveTime,bool bCreate = true);

	//attach the processor to this sidewalk
	void AttachSideWalk( const ProcessorID& _procID );
	//detach the processor to this sidewalk
	void DetachSideWalk( const ProcessorID& _procID );
	void ReplaceMovingSideWalk( const ProcessorID& _oldProcID, const ProcessorID& _newProcID );
	MovingSideWalk* GetSideWalkAt( int _iIndex );
	
	
	int GetSideWalkCount() const { return (int)m_vSideWalkInPipe.size();	};
	bool IsSideWalkHasAttach( const ProcessorID& _procID ) const;
	
	//test if the processor can attach with this pipe
	bool CanAttached( const ProcessorID& _procID  )const;
	
	

	virtual void WriteData( ArctermFile& p_file ) const;
	virtual void ReadData(  ArctermFile& p_file,InputLandside* pInputLandside );
	virtual void readObsoleteData ( ArctermFile& p_file );


	// get point for the given pipe intersected point and percent
	Point GetDestPoint( const CMobPipeToPipeXPoint& _pt, int _nPercent );

	// get point for the given intersected center point and percent
	Point GetDestPoint( const CPointToPipeXPoint& _pt, int _nPercent );

	// get point for the given index of pipe point and percent
	Point GetDestPoint( int _nIndex, int _nPercent );
	
	// get all intersect point on segment _iSegment
	int getIntersectPointOnSegement( int _iSegment, PIPEINTERSECTIONPOINTLIST& _vertex);

	// insert
	void addEntryPointToList(const CPipeToPipeXPoint& point);
	// clear
	void clearEntryPointList();

	// calculate the intermediate data
	// bisect line, intersection point of the pipe.
	void CalculateIntermediatData( int _pipeindex );

	void CalculatePointTimeOnSildWalk(int idx);

	// Get Intersection point on the pipe from the point.
	// input: 
	//	point
	// output:
	//  CPipeInterSectionPoint:  point index in the pipe,  the distance from start point of that segment..
	CPointToPipeXPoint GetIntersectionPoint( const Point& _targetPoint );


	// Get the intersection point with other pipe base on the intersection point, in other words, find the shortest distance.
	// input: 
	//	other pipe, CPipeInterSectionPoint to make the shortest path
	// output:
	//  result x point
	// return true / false tell if suceed.
	bool GetIntersectionPoint( CPipe* _pOtherPipe, const CPointToPipeXPoint& _pIntersectionPt, CMobPipeToPipeXPoint& _pResXPoint  );


	// give the two intersection points, get all the points for logs 
	//std::vector<Point> GetPointListForLog( const CPointToPipeXPoint& _ptFrom, const CPointToPipeXPoint& _ptTo, int _nPercent );
	void GetPointListForLog( int nPipeIndex,const CPointToPipeXPoint& _ptFrom, const CPointToPipeXPoint& _ptTo, int _nPercent, PTONSIDEWALK& _vPointResutl );
	

	// give one intersection point.
	// one intersection point between pipe.
	// get a list of point for the logs.
	// last point on the pipe to pipe. at this stage, we have to know the order information.
	//std::vector<Point> GetPointListForLog( const CPointToPipeXPoint& _ptFrom, const CMobPipeToPipeXPoint& _ptTo, int _nPercent );
	void GetPointListForLog( int nPipeIndex,const CPointToPipeXPoint& _ptFrom, const CMobPipeToPipeXPoint& _ptTo, int _nPercent, PTONSIDEWALK& _vPointResutl );

	// give two intersection point between pipe.
	// get a list of point for the logs.
	// last point on the pipe to pipe. at this stage, we have to know the order information.
	//std::vector<Point> GetPointListForLog( const CMobPipeToPipeXPoint& _ptFrom, const CMobPipeToPipeXPoint& _ptTo, int _nPercent );
	void GetPointListForLog( int nPipeIndex,const CMobPipeToPipeXPoint& _ptFrom, const CMobPipeToPipeXPoint& _ptTo,  int _nPercent,PTONSIDEWALK& _vPointResutl );

	// give one intersection point between pipe.
	// give one intersection point.
	// get a list of point for the logs.
	// last point on the pipe to pipe. at this stage, we have to know the order information.
	//std::vector<Point> GetPointListForLog( const CMobPipeToPipeXPoint& _ptFrom, const CPointToPipeXPoint& _ptTo, int _nPercent );
	void GetPointListForLog( int nPipeIndex,const CMobPipeToPipeXPoint& _ptFrom, const CPointToPipeXPoint& _ptTo, int _nPercent , PTONSIDEWALK& _vPointResutl );

	// recursive function, find the shortest path.
	// return 0 if circle or end of search.
	// return count if find the 
	int FindShortestPathToPipe( int _nDestIdx, std::vector<int>& _resList );
	
	void InsertPointOnMovingSideWalk( int _iStartIdx , int _iEndIdx, PTONSIDEWALK& _vPointResutl ,DistanceUnit _dFromDistance ,DistanceUnit _dEndDistance );

	void GetNearestVertex(const CPointToPipeXPoint& point, CGraphVertex& vertexLeft, CGraphVertex& vertexRight);

	DistanceUnit GetRealDistanceFromStartOnPipe( unsigned _iEndIdx )const;
	DistanceUnit GetRealDistanceFromStartOnPipe( const Point& _ptPoint  );

protected:

	void InsertPointOnMovingSideWalkWithOrdeer( bool _bOrder, int _iStartIdx , int _iEndIdx, PTONSIDEWALK& _vPointResutl ,DistanceUnit _dFromDistance ,DistanceUnit _dEndDistance );

	// calculate the intersection point with other pipes.
	void CalculateTheIntersectionPointWithPipe( int _pipeindex );

	// calculate the intersection point with other pipe
	PIPEINTERSECTIONPOINTLIST CalculateThisIntersectionPointWithOtherPipe( CPipe* _pOtherPipe,int _pipeindex1, int _pipeindex2 );

	void ReadAttachCrosswalkData(ArctermFile& p_file,InputLandside* pInputLandside);
	void WritAttachCrosswalkeData(ArctermFile& p_file)const;

	void GetCrosswalkRegion(const CrossWalkAttachPipe& crossPipe,POLLYGONVECTOR& _regions, POLLYGONVECTOR& _texcoord)const;

};
