// CollisonGrid.h: interface for the CCollisonGrid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLISONGRID_H__8AE34E96_5B07_4DD0_ACDE_05E85F2058F3__INCLUDED_)
#define AFX_COLLISONGRID_H__8AE34E96_5B07_4DD0_ACDE_05E85F2058F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<vector>
#include"common\point.h"
#include "reports\rep_pax.h"
#include "common\termfile.h"
typedef struct
{
	int m_iPaxIdx;
	DistanceUnit m_x;
	DistanceUnit m_y;
	int m_iState;
}PAXPOSE;
typedef std::vector<PAXPOSE> PERSONINGRID;

class CCollisonGrid  
{
private:
	PERSONINGRID m_vPersonPos;
public:
	CCollisonGrid();
	virtual ~CCollisonGrid();
public:
	void ClearGridPerson(){ m_vPersonPos.clear();};
	void AddPerson( PAXPOSE& _personInfo );
	void WritePersonInGrid( ArctermFile& p_file , ElapsedTime& _time ,const CMobileElemConstraint& paxType,Terminal* _pTerm);
	void WritePersonInOtherGrid( CCollisonGrid& _otherGrid,ArctermFile& p_file , ElapsedTime& _time ,const CMobileElemConstraint& paxType,Terminal* _pTerm,DistanceUnit _gridWidth);

};

#endif // !defined(AFX_COLLISONGRID_H__8AE34E96_5B07_4DD0_ACDE_05E85F2058F3__INCLUDED_)
