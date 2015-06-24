#include "StdAfx.h"
#include ".\airsidetrainflighttypemanagerinsim.h"
#include "../../InputAirside/CTrainFlightsManage.h"
#include "AirsideFlightInSim.h"
#include "RunwayInSim.h"
#include "RunwayResourceManager.h"
AirsideTrainFlightTypeManagerInSim::AirsideTrainFlightTypeManagerInSim(CTrainingFlightTypeManage* pTrainFlightTypeManager)
:m_pTrainFlightTypeManager(pTrainFlightTypeManager)
{
}

AirsideTrainFlightTypeManagerInSim::~AirsideTrainFlightTypeManagerInSim(void)
{
}

CTrainingTimeRangeManage* AirsideTrainFlightTypeManagerInSim::getTimeItem(AirsideFlightInSim* pFlight)
{
	ElapsedTime tTime = pFlight->GetTime();
	size_t nItemCount = m_pTrainFlightTypeManager->GetCount() ;
	for (size_t i =0; i< nItemCount; i++)
	{
		CTrainingTimeRangeManage* pTimeItem = m_pTrainFlightTypeManager->GetItem(i);
		if (tTime >= pTimeItem->GetStartTime() && tTime <= pTimeItem->GetEndTime())
		{
			return pTimeItem;
		}
	}
	return NULL;
}


RunwayExitInSim* AirsideTrainFlightTypeManagerInSim::GetTakeoffPosition( AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes )
{
	if (m_pTrainFlightTypeManager == NULL)
		return NULL;
	
	CTrainingTimeRangeManage* pTimeItem = getTimeItem(pFlight);

	if (pTimeItem == NULL)
		return NULL;
	
	size_t nCount =pTimeItem->m_TakeOffPositionList.size();

	RunwayExitInSim* pTakeoffPos = NULL;
	ASSERT(nCount>0);//should define at least one

	int nRandom = random(nCount);

	CTrainingTakeOffPosition* pTrainingTakeoffPos = pTimeItem->m_TakeOffPositionList.at(nRandom);
	if (pTrainingTakeoffPos->m_runwayIdList.empty())
		return NULL;
	
	return pRunwayRes->GetRunwayExitByID(pTrainingTakeoffPos->m_runwayIdList.front());

}
