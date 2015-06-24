// Escalator.h: interface for the Escalator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ESCALATOR_H__F9C7D03B_3247_43E4_807C_E1FA9F0CF3F3__INCLUDED_)
#define AFX_ESCALATOR_H__F9C7D03B_3247_43E4_807C_E1FA9F0CF3F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCESS.H"
#include "inputs\Pipe.h"
#include <CommonData\ExtraProcProp.h>

class Escalator : public Processor, public EscalatorProp
{
public:
	Escalator();
	virtual ~Escalator();

	virtual ExtraProcProp* GetExtraProp() { return this; }

public:
	// Returns ProcessorClassType corresponding to proc type.
    virtual int getProcessorType (void) const { return EscalatorProc; };

    //Returns processor name. It will be override by subclasses.
    //Without special specific, define a processor name value as "Point". 
    virtual const char *getProcessorName (void) const { return "Escalator"; };

	virtual void initSpecificMisc (const MiscData *miscData) ;	

	//add by Mark Chen 02/04/2004
	//virtual int readSpecialField(ArctermFile& procFile);
	//virtual int writeSpecialField(ArctermFile& procFile)const;
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile)const;

	virtual void initServiceLocation (int pathCount, const Point *pointList);
	virtual Point GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime);
    virtual bool CheckIfOnTheSameFloor(int _iCurFloor);

public:
	// It is called by Person::processServerArrival
    // The different processor types serve clients in different ways.
    virtual void beginService (Person *_pPerson, ElapsedTime _curTime);
	
	DistanceUnit GetWidth()const { return m_dWidth;	}
	void CalculateTheBisectLine();
	//get all region the conveyor covered, it consisted of a list of pollygon.
	void GetCoveredRegion( POLLYGONVECTOR& _regions ) const;
	void SetWidth( DistanceUnit _dWidth ) ;
	void SetCapacity(int _capacity){ m_nCapacity = _capacity;};
	int GetCapacity(){ return m_nCapacity;};	
	void SetMovingSpeed(DistanceUnit _dMovingSpeed){ m_dMovingSpeed = _dMovingSpeed; };
	DistanceUnit GetMovingSpeed(){ return m_dMovingSpeed ;};	

	virtual Processor* CreateNewProc();
	virtual bool CopyInConstraint(Processor* _pDestProc);
	virtual bool CopyOutconstraint(Processor* _pDestProc);
	virtual bool CopyQueue(Processor* _pDestProc);
	virtual bool CopyOtherData(Processor* _pDestProc);
	
	//check the queue/in constraint and the first point of service location on the same floor
	//check the out constraint and the end point of the service location on the same floor
	//if not ,rectify it
	void CheckAndRectifyQueueInOutConstraint();
	bool CheckAndRectifyQueueLocation();
	bool CheckAndRectifyInConstraintLocation();
	bool CheckAndRectifyOutConstraintLocation();

private:
	bool IsCrossOver( Point _pt11, Point _pt12, Point _pt21, Point _pt22 );
	
};

#endif // !defined(AFX_ESCALATOR_H__F9C7D03B_3247_43E4_807C_E1FA9F0CF3F3__INCLUDED_)
