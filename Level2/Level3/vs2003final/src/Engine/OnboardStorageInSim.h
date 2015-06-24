#pragma once
#include "OnboardAircraftElementInSim.h"
#include "Common/Pollygon2008.h"
#include "GroundInSim.h"


class CStorage;
class ARCVector3;
class EntryPointInSim;
class OnboardFlightInSim;

class OnboardStorageInSim : public OnboardAircraftElementInSim
{
public:
	OnboardStorageInSim(CStorage *pStorage,OnboardFlightInSim* pFlightInSim);
	~OnboardStorageInSim(void);

	BOOL GetPosition(ARCVector3& pos) const;
	bool Contain(const CPoint2008& pt)const; 

	CString GetType()const;
	void CalculateSpace(){}

	virtual int GetResourceID() const;

	void SetEntryPoint(EntryPointInSim* pEntryPointInSim);
	EntryPointInSim* GetEntryPoint();

	//visitor flush log
	void visitorFlushLog(const ElapsedTime& eTime);
	void visitorWriteLog(const ElapsedTime& eTime);
	void addVisitor(PaxVisitor* pVisitor);
	void clearVisitor();

	bool onboardState();
	GroundInSim* GetGround();

	ARCVector3 GetRandomPoint()const;

	double GetAvailableSpace() const;
	
	void CreateEntryPoint();
protected:
	void CreatePollygon();
public:
	CStorage *m_pStorage;
	CPollygon2008 m_pollygon;
	EntryPointInSim* m_pEntryPointInSim;
	std::vector<PaxVisitor*> m_pVisitorList;

protected:
	double m_dCarryonLoad;
	
};
