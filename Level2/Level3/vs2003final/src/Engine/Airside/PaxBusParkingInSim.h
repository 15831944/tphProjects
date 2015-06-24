#pragma once
#include "airsideresource.h"


class CPaxBusParking;

class CPaxBusParkingInSim :
	public AirsideResource, public CAirsideSubject
{
public:

	typedef ref_ptr<CPaxBusParkingInSim> RefPtr;
	CPaxBusParkingInSim(CPaxBusParking *pPaxParking);
	~CPaxBusParkingInSim(void);

public:
	ResourceType GetType()const{ return ResType_PaxBusParking; }
	virtual CString GetTypeName()const{ return _T("Pax bus parking"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);
	virtual CPoint2008 GetDistancePoint(double dist)const;

	CPaxBusParking *GetPaxParkingInput();

	CPoint2008 GetRandPoint();
protected:
	CPaxBusParking *m_pPaxParkingInput;
};
