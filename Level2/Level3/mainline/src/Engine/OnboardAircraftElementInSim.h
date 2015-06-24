#pragma once
#include "OnboardCellInSim.h"
#include "Common/ARCVector.h"
class OnboardFlightInSim;

class OnboardAircraftElementInSim
{
public:
	OnboardAircraftElementInSim(OnboardFlightInSim* pFlightInSim);
	virtual ~OnboardAircraftElementInSim(void);
	virtual CString GetType()const=0;
	virtual void CalculateSpace() = 0;

public:
	virtual OnboardCellInSim *GetEntryCell() const;
	virtual void CreatePollygon(){}
	virtual bool GetLocation(CPoint2008& location)const{return true;}

	virtual void NotifyPaxLeave(OnboardCellInSim *pCell, const ElapsedTime& eTime );

	//return the resource index
	//for writing log
	virtual int GetResourceID() const = 0;


protected:
	OnboardFlightInSim* m_pFlightInSim;

};
