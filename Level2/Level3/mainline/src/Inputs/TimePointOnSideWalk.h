// TimePointOnSideWalk.h: interface for the CTimePointOnSideWalk class.
#pragma once
#include "common\TimePoint.h"

class CTimePointOnSideWalk : public CTimePoint  
{
private:
	bool m_bOnMovingSideWalk;
	int m_iMovingSideWalkIdx;
	int m_iPipeIndex;
	int m_iStartPointIdxInPipe;
	int m_iEndPointIdxInPipe;
	DistanceUnit m_dStartDistance;
	DistanceUnit m_dEndDistance;

	DistanceUnit m_dRealDistanceFromStart;
public:
	void SetOnSideWalkFlag( bool _bFlag ) { m_bOnMovingSideWalk = _bFlag;	};
	bool GetOnSideWalkFlag()const  {  return m_bOnMovingSideWalk ;	};

	void SetSideWalkIdx( int _iIdx ) { m_iMovingSideWalkIdx = _iIdx;	};
	int GetSideWalkIdx()const  {  return m_iMovingSideWalkIdx ;	};

	void SetPipeIdx(int _iIdx ) {m_iPipeIndex = _iIdx;}
	int GetPipeIdx()const {return m_iPipeIndex;}

	void SetStartPointIdxInPipe( int _iIdx ) { m_iStartPointIdxInPipe = _iIdx;	};
	int GetStartPointIdxInPipe()const  {  return m_iStartPointIdxInPipe ;	};

	void SetEndPointIdxInPipe( int _iIdx ) { m_iEndPointIdxInPipe = _iIdx;	};
	int GetEndPointIdxInPipe()const  {  return m_iEndPointIdxInPipe ;	};

	void SetStartDistance( DistanceUnit _dDistance ) { m_dStartDistance = _dDistance;	};
	DistanceUnit GetStartDistance()const  {  return m_dStartDistance ;	};


	void SetRealDistanceFromStart( DistanceUnit _dDistance ) { m_dRealDistanceFromStart = _dDistance;	};
	DistanceUnit GetRealDistanceFromStart()const  {  return m_dRealDistanceFromStart ;	};

	void SetEndDistance( DistanceUnit _dDistance ) { m_dEndDistance = _dDistance;	};
	DistanceUnit GetEndDistance()const  {  return m_dEndDistance ;	};

	Point GetPointOnSideWalk( ){ return Point( this->x, this->y, this->z );	};
	bool operator < ( const CTimePointOnSideWalk& _anotherInstance ) const;

	CTimePointOnSideWalk();
	virtual ~CTimePointOnSideWalk();

};

typedef std::vector<CTimePointOnSideWalk> PTONSIDEWALK;
