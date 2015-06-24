#include "StdAfx.h"
#include ".\standbufferinsim.h"
#include "common/AirportDatabase.h"
#include "InputAirside/CParkingStandBuffer.h"
#include "AirsideFlightInSim.h"
#include "StandInSim.h"
#include "Common/FLT_CNST.H"
#include "InputAirside/ALTObject.h"

StandBufferInSim::StandBufferInSim(void)
{
	m_pParkingStandBuffer = NULL;
}

StandBufferInSim::~StandBufferInSim(void)
{
	delete m_pParkingStandBuffer;
	m_pParkingStandBuffer = NULL;
}

void StandBufferInSim::Init(int nPrjId, CAirportDatabase* pDatabase)
{
	m_pParkingStandBuffer = new CParkingStandBuffer(nPrjId,pDatabase);
	m_pParkingStandBuffer->ReadData(nPrjId);
}

ElapsedTime StandBufferInSim::GetStandBufferTime(AirsideFlightInSim* pFlight,StandInSim* pStand)
{
	ElapsedTime tTime = 0L;
	AirsideFlightInSim* pDepartureFlight = pStand->GetLastOccupyInstance().GetFlight();
	if (pDepartureFlight && pDepartureFlight != pFlight)
	{
		int nCount = m_pParkingStandBuffer->GetParkingStandgroupCount();
		for (int i =0; i < nCount; i++)
		{
			ALTObjectID groupID = m_pParkingStandBuffer->GetParkingStandgroup(i)->GetStandGroup().getName();
			ALTObjectID standID = pStand->GetStandInput()->GetObjectName();
			if (standID.idFits(groupID))
			{					
				PSBDepartingFltType* pItem = NULL;
				for ( int j =0; j < m_pParkingStandBuffer->GetParkingStandgroup(i)->GetDepartingFltTypeCount(); j++)
				{
					FlightConstraint fltCnst = m_pParkingStandBuffer->GetParkingStandgroup(i)->GetDepartingFltType(j)->GetDepartingFltType();

					if (pDepartureFlight->fits(fltCnst))
					{
						pItem = m_pParkingStandBuffer->GetParkingStandgroup(i)->GetDepartingFltType(j);
						break;
					}
				}

				if (pItem)
					for (int j =0; j < pItem->GetArrivingFltTypeCount(); j++)
					{
						FlightConstraint fltCnst = pItem->GetArrivingFltType(j)->GetArrivingFltType();
						if (pFlight->fits(fltCnst))
						{
							tTime = ElapsedTime(pItem->GetArrivingFltType(j)->m_lBufferTime * 60);
							return tTime;
						}
					}
			}
		}
	}


	return tTime;
}