#include "StdAfx.h"
#include ".\paxbusparkinginsim.h"
#include "../../InputAirside/PaxBusParking.h"
#include "Common/pollygon2008.h"



CPaxBusParkingInSim::CPaxBusParkingInSim(CPaxBusParking *pPaxParking)
{
	m_pPaxParkingInput = pPaxParking;
}

CPaxBusParkingInSim::~CPaxBusParkingInSim(void)
{
}
CString CPaxBusParkingInSim::PrintResource() const
{
	return _T("");
}
CPaxBusParking *CPaxBusParkingInSim::GetPaxParkingInput()
{
	return m_pPaxParkingInput;
}
CPoint2008 CPaxBusParkingInSim::GetDistancePoint(double dist)const
{
	CPoint2008 pt;
	if(m_pPaxParkingInput)
	{
		CPath2008 path = m_pPaxParkingInput->getPath();
		if (path.getCount())
		{
			pt = path.getPoint(0);
		}
	}
	return pt;
	
}
CPoint2008 CPaxBusParkingInSim::GetRandPoint()
{
	const CPoint2008* pointList = m_pPaxParkingInput->GetPath().getPointList();
	int nCount = m_pPaxParkingInput->GetPath().getCount();
	CPollygon2008 polygen;
	polygen.init(nCount, pointList);

	CPoint2008 pos = polygen.getRandPoint();

	return pos;
}

#include "..\..\Results\AirsideFlightEventLog.h"
void CPaxBusParkingInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetPaxParkingInput()->GetID();
	resDesc.strRes = PrintResource();
	resDesc.resType = GetType();
}