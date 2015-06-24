// PointAndBagsInBagProcessor.h: interface for the PointAndBagsInBagProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTANDBAGSINBAGPROCESSOR_H__D19FE95C_DC3D_485E_8757_FB0142052FA4__INCLUDED_)
#define AFX_POINTANDBAGSINBAGPROCESSOR_H__D19FE95C_DC3D_485E_8757_FB0142052FA4__INCLUDED_

#include<vector>
#include "common\point.h"
#include "common\elaptime.h"
//#include "engine\person.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Person;
typedef long PersonID;
typedef std::vector<PersonID> BAGS;

class PointAndBagsInBagProcessor  
{
public:
	PointAndBagsInBagProcessor();
	PointAndBagsInBagProcessor( const PointAndBagsInBagProcessor& _another );
	virtual ~PointAndBagsInBagProcessor();
private:
	Point m_ptPostion;
	BAGS m_vBags;

	// needed time which baggage move from one point to next point
	ElapsedTime m_costTimeToNextPoint;
	//long m_lStepTime;
public:
	void AddPerson( long _lPersonID ){ m_vBags.push_back( _lPersonID );	};
	void ErasePerson( PersonID _nPerson );

	Point GetPosPoint()const { return m_ptPostion;	};
	void SetPosPoint ( const Point& _ptPos ){ m_ptPostion = _ptPos ;	};

	void PickUpBags( Passenger* _pOwner ,BAGS& _vBags );

	void SetCostTime( const ElapsedTime& _time ) { m_costTimeToNextPoint = _time;	}
	ElapsedTime GetCostTime()const { return m_costTimeToNextPoint;	}

};

typedef std::vector<PointAndBagsInBagProcessor> BAGPROCUTIL;

#endif // !defined(AFX_POINTANDBAGSINBAGPROCESSOR_H__D19FE95C_DC3D_485E_8757_FB0142052FA4__INCLUDED_)
