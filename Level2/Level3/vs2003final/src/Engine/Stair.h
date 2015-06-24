// Stair.h: interface for the Stair class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STAIR_H__6F200C90_284A_4772_B755_5FCE8D4695A4__INCLUDED_)
#define AFX_STAIR_H__6F200C90_284A_4772_B755_5FCE8D4695A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCESS.H"
#include "inputs\Pipe.h"
#include <CommonData/ExtraProcProp.h>

class Stair : public Processor, public StairProp
{
public:
	Stair();
	virtual ~Stair();
public:
	//virtual Point GetPipeExitPoint( int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob) ;
	// Returns ProcessorClassType corresponding to proc type.
    virtual int getProcessorType (void) const { return StairProc; };

	virtual ExtraProcProp* GetExtraProp() { return this; }

    //Returns processor name. It will be override by subclasses.
    //Without special specific, define a processor name value as "Point". 
    virtual const char *getProcessorName (void) const { return "Stair"; };

	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile)const;

	virtual void initServiceLocation (int pathCount, const Point *pointList);

	virtual bool CopyOtherData(Processor* _pDestProc);

	virtual Point AcquireServiceLocation( Person* _pPerson  );
public:
	    // It is called by Person::processServerArrival
    // The different processor types serve clients in different ways.
    virtual void beginService (Person *_pPerson, ElapsedTime _curTime);
	
	DistanceUnit GetWidth()const { return m_dWidth;	}
	void CalculateTheBisectLine();
	//get all region the conveyor covered, it consisted of a list of pollygon.
	void GetCoveredRegion( POLLYGONVECTOR& _regions ) const;
	void SetWidth( DistanceUnit _dWidth ) ;
	virtual Point GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime);
    virtual bool CheckIfOnTheSameFloor(int _iCurFloor);

	virtual Processor* CreateNewProc();

private:
	bool IsCrossOver( Point _pt11, Point _pt12, Point _pt21, Point _pt22 );


};

#endif // !defined(AFX_STAIR_H__6F200C90_284A_4772_B755_5FCE8D4695A4__INCLUDED_)
