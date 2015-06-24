// PipeInterSectionPoint.h: interface for the CPipeInterSectionPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIPEINTERSECTIONPOINT_H__6A890F8A_9023_4B8A_83AF_01C369C4FC0C__INCLUDED_)
#define AFX_PIPEINTERSECTIONPOINT_H__6A890F8A_9023_4B8A_83AF_01C369C4FC0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\point.h"

class CPointToPipeXPoint : public Point  
{
protected:
	int m_nIdxSeg;		// segment index; it is segement index, not point index in the pipe
					//	pt0				pt1				pt2
						//O--------------O---------------O
						//	seg index 0	    seg index 1

	DistanceUnit m_dDistFromStartPt;		// distance from start of point of the segment.	
	int m_pipeIndex;

	int m_iOtherInfo;

	bool m_bPointOnPipe;//mark this point is a point in pipe path defined or between them
	int m_nPointIndex;//point index in the pipe path

public:
	CPointToPipeXPoint();
	virtual ~CPointToPipeXPoint();

	void SetPointToPipeXPoint( const CPointToPipeXPoint& _pt );

	void SetSegmentIndex( int _nIdx );
	int GetSegmentIndex() const;

	void SetOtherInfo( int _nIdx ){ m_iOtherInfo = _nIdx;	};
	int GetOtherInfo() const{ return m_iOtherInfo;	};
	bool operator < ( const CPointToPipeXPoint& _another ) const;

	void SetDistanceFromStartPt( DistanceUnit _dDist );
	DistanceUnit GetDistanceFromStartPt() const;
	
	void SetPipe(int pipeIndex) { m_pipeIndex = pipeIndex; }
	int GetPipeIndex() const { return m_pipeIndex; }

	bool GetPointOnPipe() const { return m_bPointOnPipe; }
	void SetPointOnPipe(bool val) { m_bPointOnPipe = val; }

	int GetPointIndex() const { return m_nPointIndex; }
	void SetPointIndex(int val) { m_nPointIndex = val; }
};


class CPipeToPipeXPoint : public CPointToPipeXPoint  
{
protected:

	// created when calculated.
	// Same Order: from 1 -> 2 to 1 -> 2
	// Diff Order: from 2 -> 1 to 2 -> 1
	Point m_ptSameOrder1;			
	Point m_ptSameOrder2;
	Point m_ptDiffOrder1;
	Point m_ptDiffOrder2;
	int m_nOtherIdxSeg;		// segment index;
	DistanceUnit m_dOtherDistFromStartPt;		// distance from start of point of the segment.	
public:
	CPipeToPipeXPoint();
	~CPipeToPipeXPoint();

	void SetPipeToPipeXPoint( const CPipeToPipeXPoint& _pt );

	void SetOtherSegIndex( int _nOtherIdxSeg ){ m_nOtherIdxSeg = _nOtherIdxSeg; }
	void SetOtherDistFromStart( DistanceUnit _dOtherDistFromStart ){ m_dOtherDistFromStartPt = _dOtherDistFromStart; }
	int GetOtherSegIndex() const{ return m_nOtherIdxSeg; }
	DistanceUnit GetOtherDistFromStart()const { return m_dOtherDistFromStartPt; }


	void SetSameOrderPoints( Point _ptSameOrder1, Point _ptSameOrder2 );
	void SetDiffOrderPoints( Point _ptSameOrder1, Point _ptSameOrder2 );

	Point GetSameOrderPoint1() const{ return m_ptSameOrder1; }
	Point GetSameOrderPoint2() const{ return m_ptSameOrder2; }
	Point GetDiffOrderPoint1() const{ return m_ptDiffOrder1; }
	Point GetDiffOrderPoint2() const{ return m_ptDiffOrder2; }
	
private:
	int m_iPipeIndex1;			
	int m_iPipeIndex2;
public:
	void SetPipeIndex1( int _index ) { m_iPipeIndex1 = _index; }
	void SetPipeIndex2( int _index ) { m_iPipeIndex2 = _index; }
	int GetPipeIndex1( void ) const { return m_iPipeIndex1; }
	int GetPipeIndex2( void ) const { return m_iPipeIndex2;	}
};



// used when mobile element 
class CMobPipeToPipeXPoint : public CPipeToPipeXPoint  
{
protected:
	bool m_bInInc;
	bool m_bOutInc;

public:
	CMobPipeToPipeXPoint();
	~CMobPipeToPipeXPoint();

	void SetMobPipeToPipeXPoint( const CMobPipeToPipeXPoint& _pt );

//	void SetPipeToPipeXPoint( const CPipeToPipeXPoint& _pt );

	void SetInInc( bool _bInInc ){ m_bInInc = _bInInc; }
	void SetOutInc( bool _bOutInc ){ m_bOutInc = _bOutInc; }
	bool GetInInc()const { return m_bInInc; }
	bool GetOutInc()const{ return m_bOutInc; }

	// get the exit point information so we can decide the outinc.
	void SetOutInc( const CPointToPipeXPoint& _pt );

	Point GetPoint1() const;
	Point GetPoint2() const;

	bool OrderChanged(){ return m_bInInc == m_bOutInc ? false : true; }
};



#endif // !defined(AFX_PIPEINTERSECTIONPOINT_H__6A890F8A_9023_4B8A_83AF_01C369C4FC0C__INCLUDED_)
