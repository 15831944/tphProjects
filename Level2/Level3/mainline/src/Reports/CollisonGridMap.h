// CollisonGridMap.h: interface for the CCollisonGridMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLISONGRIDMAP_H__5361768E_74D9_44FA_BA1C_3942A8F56988__INCLUDED_)
#define AFX_COLLISONGRIDMAP_H__5361768E_74D9_44FA_BA1C_3942A8F56988__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CollisonGrid.h"
typedef std::vector<CCollisonGrid> GRIDMAP;
class Point;

class CCollisonGridMap  
{
private:
	GRIDMAP m_vGridMap;
	Point m_leftBottom;
	Point m_rightTop;
	//DistanceUnit m_length;
	//DistanceUnit m_width;
	DistanceUnit m_gridWidth;
	long m_iXCount;
	long m_iYCount;
public:
	CCollisonGridMap();
	virtual ~CCollisonGridMap();
public:
	void ClearGridPerson();
	void InitMapStruct(Point& _leftBottom, Point& _rightTop, DistanceUnit _gridWidth );
	void AddPersonIntoGrid( PAXPOSE& _personInfo );
	void ComputeAndWriteToFile( ArctermFile& p_file , ElapsedTime& _time ,const CMobileElemConstraint& _sPaxType ,Terminal* _pTerm);

};

#endif // !defined(AFX_COLLISONGRIDMAP_H__5361768E_74D9_44FA_BA1C_3942A8F56988__INCLUDED_)
