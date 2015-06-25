#pragma once
#include "MobElementBehavior.h"
#include "..\Common\Point2008.h"
#include "..\Common\States.h"


class Person;

class AirsideMobElementBaseBehavior :
	public MobElementBehavior
{
public:
	AirsideMobElementBaseBehavior(Person* pMobileElement);

	~AirsideMobElementBaseBehavior(void);


public:
	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);

	void ResetTerminalToAirsideLocation();


public:
	//Returns current coordinates
	const CPoint2008& getPoint (void) const { return location; }

	//Returns current destination.
	const CPoint2008& getDest (void) const { return m_ptDestination; }
	// get pure location
	const CPoint2008& getPureLocation( void ) const { return pure_location; }

	ElapsedTime moveTime (void) const;

	ElapsedTime moveTime( DistanceUnit _dExtraSpeed, bool _bExtra ) const;

	//Sets Element's destination
	virtual void setDestination (CPoint2008 p);

	virtual void setLocation( const CPoint2008& _ptLocation );

	virtual int	WriteLog(ElapsedTime time,const double speed = 0, enum EntityEvents enumState = FreeMoving) ;


	void setState (short newState);
	int getState (void) const;

	////in airside behavior, do not use mobile element setDestnation, setLocation any more
	//void setDestination(const Point& destinationPt, bool bRealAttitude);
	//void setLocation(const Point& loactionPt, bool bRealAttitude);

	bool ConvertPointToRealAttitude(Point& point)const;


	virtual void SetFollowerDestination(const CPoint2008& _ptCurrent, const CPoint2008& _ptDestination, float* _pRotation );

	void GenetateEvent(ElapsedTime time) ;

	BehaviorType getBehaviorType()const{ return MobElementBehavior::AirsideBehavior; }

	virtual void writeLog (ElapsedTime time, bool _bBackup, bool bOffset = true ){};

	Person *getMobileElement();
protected:
	Person *m_pMobileElemment;


	//Current coordinates
	CPoint2008 location;       

//Current destination, used for
	CPoint2008 m_ptDestination;     

	//pure location( no offset )
	CPoint2008 pure_location;





};
