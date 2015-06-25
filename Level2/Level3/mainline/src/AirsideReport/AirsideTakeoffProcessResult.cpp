#include "StdAfx.h"
#include ".\airsidetakeoffprocessresult.h"
#include "AirsideTakeoffProcessParameter.h"
#include "../Results/ARCBaseLog.h"
#include "../Results/AirsideFlightLogItem.h"
#include "../Common/ProgressBar.h"

CAirsideTakeoffProcessResult::CAirsideTakeoffProcessResult(void)
{
}

CAirsideTakeoffProcessResult::~CAirsideTakeoffProcessResult(void)
{
}

//-----------------------------------------------------------------------------------
//Summary:
//		check flight fit parameter that user input
//-----------------------------------------------------------------------------------
bool CAirsideTakeoffProcessResult::fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons)
{
	size_t nFltConsCount = parameter->getFlightConstraintCount();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		if(parameter->getFlightConstraint(nfltCons).fits(fltDesc))
		{
			fltCons = parameter->getFlightConstraint(nfltCons);
			return true;
		}
	}

	return false;
}

void CAirsideTakeoffProcessResult::GenerateResult( CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter )
{
	//check parameter
	if (pParameter == NULL)
		return;

	CAirsideTakeoffProcessParameter* pTakeoffPara = (CAirsideTakeoffProcessParameter*)pParameter;

	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	m_vResult.clear();
	ARCBaseLog<AirsideFlightLogItem> mFlightLogData;
	mFlightLogData.OpenInput(strDescFilepath.GetString(), strEventFilePath.GetString());
	int nCount = mFlightLogData.getItemCount();
	CProgressBar bar( "Generating Takeoff Process Report", 100, nCount, TRUE );
	for (int i = 0; i < nCount; i++)
	{
		bar.StepIt();
		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);
		int nLogCount = (int)item.mpEventList->mEventList.size();
		CTakeoffProcessDetailData detailData;
		AirsideFlightLogDesc fltdesc = item.mFltDesc;
		AirsideFlightDescStruct descStruct;
		fltdesc.getFlightDescStruct(descStruct);
		FlightConstraint fltconst;
		//check flight constraint
		if (!fits(pParameter,descStruct,fltconst))
			continue;

		fltconst.screenPrint(detailData.m_sFlightType);
		detailData.m_sFlightType.ReleaseBuffer();

		if (descStruct._arrID.HasValue() && descStruct._depID.HasValue())
		{
			detailData.m_sFlightID.Format(_T("%s%s/%s"),descStruct._airline.GetValue(),descStruct._arrID.GetValue(),descStruct._depID.GetValue());
		}
		else
		{
			detailData.m_sFlightID.Format(_T("%s%s%s"),descStruct._airline.GetValue(),descStruct._arrID.GetValue(),descStruct._depID.GetValue());
		}

		double dSpeed = 0.0;
		bool bTakeoffQueue = false;//check take off queue log
		CPoint2008 startPos;
		bool bWaitEnterRunway = false;//check wait for enter runway log
		bool bEnterRunway = false;//check enter runway log
		long lLeaveHoldShortLine = 0l;
		bool bPreTakeoff = false;//check pre take off log
		bool bTakeoff = false;//check take off log
		
		std::vector<AirsideFlightEventLog*> eventList;
		for (int j = 0; j < nLogCount; j++)
		{
			ARCEventLog* pLog = item.mpEventList->mEventList.at(j);
			ElapsedTime time;
			time.setPrecisely(pLog->time);

			if (!bTakeoffQueue && pLog->IsA(typeof(AirsideTakeoffProcessLog)))
			{
				AirsideTakeoffProcessLog* pTakeoffLog = (AirsideTakeoffProcessLog*)pLog;
				detailData.m_lEnterQueue = pLog->time;
				detailData.m_sTakeoffPosition = pTakeoffLog->m_sTakeoffPoistion.c_str();
				detailData.m_lCountSameQueue = pTakeoffLog->m_lCount;
				detailData.m_lTotalCountQueue = pTakeoffLog->m_lTotalCount;
				detailData.m_sDistFromQueue.Format(_T("(%s)-(%s): %.2f(m)"),\
					pTakeoffLog->m_sStartNode.c_str(),pTakeoffLog->m_sEndNode.c_str(),pTakeoffLog->m_dDist/100);
				startPos.init(pTakeoffLog->x,pTakeoffLog->y,pTakeoffLog->z);
				
				if (!pTakeoffPara->fit(detailData.m_sTakeoffPosition,time))
					break;

				bTakeoffQueue = true;
			}	
			
			if (pLog->IsA(typeof(AirsideConflictionDelayLog)))
			{
				AirsideConflictionDelayLog* pConflictLog = (AirsideConflictionDelayLog*)pLog;
				if (pConflictLog->mMode >= OnTakeOffWaitEnterRunway && pConflictLog->mMode <= OnClimbout)
				{
					detailData.m_lTakeoffDelay = pConflictLog->mDelayTime;
				}
				continue;
			}
			
			//calculate flight from stand to runway normal speed
			if (pLog->IsA(typeof(AirsideFlightEventLog)))
			{
				AirsideFlightEventLog* pFlightEventLog = (AirsideFlightEventLog*)pLog;
				if (pFlightEventLog->mMode == OnTaxiToRunway)
				{
					dSpeed = max(pFlightEventLog->dSpd,dSpeed);
				}
			}

			if (!bTakeoffQueue)
				continue;
			
			if (!pLog->IsA(typeof(AirsideFlightEventLog)))
				continue;
			
			AirsideFlightEventLog* pFlightEventLog = (AirsideFlightEventLog*)pLog;
			eventList.push_back(pFlightEventLog);
			//arrival at hold short line and first time write this log
			if (pFlightEventLog->mMode == OnTakeOffWaitEnterRunway && !bWaitEnterRunway)
			{
				bWaitEnterRunway = true;
				detailData.m_lHoldShortLine = pFlightEventLog->time;
				double dDist = CalculateDistance(eventList);
				if (!eventList.empty())
				{
					AirsideFlightEventLog* pFlightEventLog = eventList.front();
					double fDisX = pFlightEventLog->x - startPos.getX();
					double fDisY = pFlightEventLog->y - startPos.getY();
					double fDisZ = pFlightEventLog->z - startPos.getZ();
					double fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
					
					dDist += fDistance;
				}
				if (dSpeed > 0.000001)
				{
					detailData.m_lExpectTimeToPosition = static_cast<long>(dDist/dSpeed)*100;
				}
				else
				{
					detailData.m_lExpectTimeToPosition = 0l;
				}
				
				dSpeed = 0.0;
				eventList.clear();
				continue;
			}
			//start to move to take off position
			if (pFlightEventLog->mMode == OnTakeOffEnterRunway)
			{
				bEnterRunway = true;
				if (bWaitEnterRunway == true)
				{
					lLeaveHoldShortLine = pFlightEventLog->time;
				}
				else
				{
					detailData.m_lHoldShortLine = pFlightEventLog->time;
					double dDist = CalculateDistance(eventList);
					if (!eventList.empty())
					{
						AirsideFlightEventLog* pFlightEventLog = eventList.front();
						double fDisX = pFlightEventLog->x - startPos.getX();
						double fDisY = pFlightEventLog->y - startPos.getY();
						double fDisZ = pFlightEventLog->z - startPos.getZ();
						double fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);

						dDist += fDistance;
					}
					if (dSpeed > 0.000001)
					{
						detailData.m_lExpectTimeToPosition = static_cast<long>(dDist/dSpeed)*100;
					}
					else
					{
						detailData.m_lExpectTimeToPosition = 0l;
					}

					dSpeed = 0.0;
					eventList.clear();
				}
				
				continue;
			}
			//arrival at take off position
			if (pFlightEventLog->mMode == OnPreTakeoff && !bPreTakeoff)
			{
				bPreTakeoff = true;
				detailData.m_lArrivalPosition = pFlightEventLog->time;
				continue;
			}
			//flight start to take off
			if (pFlightEventLog->mMode == OnTakeoff)
			{
				if (pFlightEventLog->dSpd == 0.0)
				{
					detailData.m_lTakeoffTime = pFlightEventLog->time;
				}
				
				bTakeoff = true;
				continue;
			}
		}
		
		if (bTakeoffQueue)
		{
			//calculate actual move to take off position
			if (bWaitEnterRunway)
			{
				detailData.m_lActualTimeToPosition = detailData.m_lHoldShortLine - detailData.m_lEnterQueue;
			}

			//calculate hold at hold short line
			if (bEnterRunway)
			{
				if (bWaitEnterRunway)
				{
					detailData.m_lDelayHoldShortLine = lLeaveHoldShortLine - detailData.m_lHoldShortLine;
				}
				else
				{
					detailData.m_lActualTimeToPosition = detailData.m_lHoldShortLine - detailData.m_lEnterQueue;
				}
			}
			detailData.m_lTakeoffQueueDelay = detailData.m_lActualTimeToPosition - detailData.m_lExpectTimeToPosition;
			if(detailData.m_lTakeoffQueueDelay < 0)
				detailData.m_lTakeoffQueueDelay = 0;
			//calculate take off delay
			if(bPreTakeoff && bTakeoff && bEnterRunway)
			{
				detailData.m_lToPostionTime = detailData.m_lArrivalPosition - detailData.m_lHoldShortLine - detailData.m_lDelayHoldShortLine;
				detailData.m_lTimeInPosition = detailData.m_lTakeoffTime - detailData.m_lArrivalPosition;
			}
			
			if (bEnterRunway && bPreTakeoff && bTakeoff)//valid and add to result
			{
				m_vResult.push_back(detailData);
			}
		}
		
		
	}
}

//-----------------------------------------------------------------------------------
//Summary:
//		calculate distance of flight event log list
//-----------------------------------------------------------------------------------
double CAirsideTakeoffProcessResult::CalculateDistance( std::vector<AirsideFlightEventLog*>& eventList ) const
{
	//check capacity
	if (eventList.empty())
		return 0.0;

	double fTotalDistance = 0.0;
	AirsideFlightEventLog* firstEvent = eventList.front();
	int nCount = (int)eventList.size();
	for (int i = 1; i < nCount; i++)
	{
		AirsideFlightEventLog* pFlightEventLog = eventList[i];
		double fDisX = pFlightEventLog->x - firstEvent->x;
		double fDisY = pFlightEventLog->y - firstEvent->y;
		double fDisZ = pFlightEventLog->z - firstEvent->z;
		double fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
		fTotalDistance += fDistance;
		firstEvent = pFlightEventLog;
	}
	return fTotalDistance;
}