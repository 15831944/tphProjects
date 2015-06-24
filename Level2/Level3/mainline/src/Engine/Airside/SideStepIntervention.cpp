#include "StdAfx.h"
#include ".\sidestepintervention.h"
#include "common/ARCUnit.h"
#include "../../InputAirside/SideStepSpecification.h"
#include "AirportResourceManager.h"
#include "AirsideFlightInSim.h"

SideStepIntervention::SideStepIntervention(void)
{
	m_pSideStepSpec = NULL;
	m_pAirportRes = NULL;
}

SideStepIntervention::~SideStepIntervention(void)
{
	delete m_pSideStepSpec;
}

void SideStepIntervention::Init( int nprjId, AirportResourceManager * pAirportRes )
{
	m_pSideStepSpec = new CSideStepSpecification(nprjId);
	m_pSideStepSpec->ReadData();
	m_pAirportRes = pAirportRes;
}

LogicRunwayInSim * SideStepIntervention::GetSideStepRunway( AirsideFlightInSim * pFlight, ClearanceItem& lastItem )
{
	CPoint2008 FlightPos = lastItem.GetPosition();
	LogicRunwayInSim * plandRunway = pFlight->GetLandingRunway();
	CPoint2008 nextPos = plandRunway->GetDistancePoint(0);
	
	ARCVector2 vFlightDir(plandRunway->GetDirection());
	


	for(int i=0;i< m_pSideStepSpec->GetFlightTypeCount();i++)
	{
		CSideStepFlightType * pItem = m_pSideStepSpec->GetFlightTypeItem(i);
		if( pFlight->fits(pItem->GetFltType()) )
		{
			for(int j=0 ; j < pItem->GetApproachRunwayCount();j++)
			{
				CSideStepApproachRunway * pThisRunwayItem = pItem->GetApproachRunwayItem(j);
				LogicRunwayInSim * pRunway = m_pAirportRes->getRunwayResource()->GetLogicRunway(pThisRunwayItem->GetRunwayID(),(RUNWAY_MARK)pThisRunwayItem->GetMarkIndex());
				if(pRunway && pRunway == plandRunway)
				{
					for(int k=0;k<pThisRunwayItem->GetToRunwayCount();k++)
					{
						CSideStepToRunway *pSideRunwayItem = pThisRunwayItem->GetToRunwayItem(k);
						LogicRunwayInSim * pSideRunway = m_pAirportRes->getRunwayResource()->GetLogicRunway(pSideRunwayItem->GetRunwayID(),(RUNWAY_MARK)pSideRunwayItem->GetMarkIndex());
						
						if(pSideRunway)
						{
							ARCVector2 vSideRwDir( pSideRunway->GetDirection());
							double dAngle = vFlightDir.GetAngleOfTwoVector(vSideRwDir);
							CPoint2008 rwPos = pSideRunway->GetDistancePoint(0);
							DistanceUnit dLength = FlightPos.distance(rwPos);
							dLength = ARCUnit::ConvertLength(dLength,ARCUnit::CM,ARCUnit::M);

							if( pSideRunwayItem->GetMaxTurnAngle() > dAngle && pSideRunwayItem->GetMinFinalLeg() < dLength  ) 
							{
								if(!pSideRunway->GetLastInResourceFlight())
									return pSideRunway;
							}
						}
						
					}
					
					
				}
			}
		}
	}

	return NULL;
}