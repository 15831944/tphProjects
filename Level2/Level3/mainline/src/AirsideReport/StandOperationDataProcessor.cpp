#include "StdAfx.h"
#include "StandOperationDataProcessor.h"
#include "Parameters.h"

#include "Results/AirsideFlightEventLog.h"
#include "Common/AIRSIDE_BIN.h"
#include "Engine/Airside/AirsideFlightInSim.h"
#include "../Common/Aircraft.h"

#include "Common/ProgressBar.h"
#include "FlightStandOperationParameter.h"

StandOperationDataProcessor::StandOperationDataProcessor()
{

}

StandOperationDataProcessor::~StandOperationDataProcessor()
{
	size_t nSize = m_vStandOperationReportData.size();
	for (size_t i =0; i < nSize; i++)
	{
		delete m_vStandOperationReportData.at(i);
	}
	m_vStandOperationReportData.clear();
}

bool StandOperationLogCompare(AirsideFlightStandOperationLog* pFirst, AirsideFlightStandOperationLog* pSecond)
{
	if (pFirst->m_altStandID == pSecond->m_altStandID)
	{
		if (pFirst->time > pSecond->time)
			return false;

		return true;
	}

	return false;
}

bool IsOpTypeLeavingStand(AirsideFlightStandOperationLog* pLog)
{
	return pLog->m_eOpType == AirsideFlightStandOperationLog::LeavingStand;
}

void StandOperationDataProcessor::LoadDataAndProcess( CBGetLogFilePath pCBGetLogFilePath, CParameters* pParameter )
{
	ASSERT(pParameter);
	CFlightStandOperationParameter* pStandOpPara = (CFlightStandOperationParameter*)pParameter;

	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	ARCBaseLog<AirsideFlightLogItem> flightLogData;
	flightLogData.OpenInput(strDescFilepath, strEventFilePath);

	CStandOperationReportData* pStandOpReportData = NULL;

	int nCount = flightLogData.getItemCount();
	
	CProgressBar bar(_T("Fetch logs and build original data..."), 100, nCount, TRUE);

	for (int i = 0; i < nCount; i++)
	{
		std::vector<AirsideFlightStandOperationLog*> vStandOpLogs;

		flightLogData.LoadItem(flightLogData.ItemAt(i));
		AirsideFlightLogItem item = flightLogData.ItemAt(i);

		ARCEventLog* pLog = NULL;

		size_t nLogCount = item.mpEventList->mEventList.size();
		for (size_t j = 0; j < nLogCount; j++)
		{
			pLog = item.mpEventList->mEventList.at(j);

			if (!pLog->IsKindof(typeof(AirsideFlightStandOperationLog)))
				continue;
			
			if (!pStandOpPara->fits(((AirsideFlightStandOperationLog*)pLog)->m_altStandID))
				continue;
			
			vStandOpLogs.push_back((AirsideFlightStandOperationLog*)pLog);
			
		}

		if (vStandOpLogs.empty())
			continue;

		std::sort(vStandOpLogs.begin(), vStandOpLogs.end(), StandOperationLogCompare);

		bool bOnlyStand = true;

		int nStand = std::count_if(vStandOpLogs.begin(), vStandOpLogs.end(),IsOpTypeLeavingStand);
		if (nStand >1)
			bOnlyStand = false;

		std::vector<AirsideFlightStandOperationLog*>::iterator iter = vStandOpLogs.begin();
		std::vector<AirsideFlightStandOperationLog*> vOneStandOpLogs;

		while (!vStandOpLogs.empty())
		{
			iter = std::find_if(vStandOpLogs.begin(), vStandOpLogs.end(),IsOpTypeLeavingStand);

			if (iter != vStandOpLogs.end())
			{
				++iter;
			}
		
			vOneStandOpLogs.assign(vStandOpLogs.begin(),iter);
			vStandOpLogs.erase(vStandOpLogs.begin(),iter);
			
			ProcessLogs(vOneStandOpLogs, item, bOnlyStand);
		}

		bar.StepIt();
	}
	
}


void StandOperationDataProcessor::ProcessLogs( std::vector<AirsideFlightStandOperationLog*>& vStandOpLogs, AirsideFlightLogItem& item, bool bOnlyStand)
{
	AirsideFlightLogDesc fltdesc = item.mFltDesc;

	CStandOperationReportData* pData = NULL;

	AirsideFlightStandOperationLog* pLog = NULL;
	AirsideFlightStandOperationLog* pPreLog = NULL;

	long lStandOccupiedDelay = 0;
	long lGSEDelay = 0;
	long lPushbackClearanceDelay = 0;
	long lTaxiwayOccupiedDelay = 0;

	size_t nSize = vStandOpLogs.size();
	for (size_t i = 0; i < nSize; i++)
	{
		AirsideFlightStandOperationLog* pLog = vStandOpLogs.at(i);

		if (i >0)
			pPreLog = vStandOpLogs.at(i-1);

		if (pData == NULL)//the time that the flight has no enter stand
		{

			//calculate the delay time if the destination stand is occupied
			if (pLog->m_eOpType == AirsideFlightStandOperationLog::EndDelay && pLog->m_eDelayReason == AirsideFlightStandOperationLog::StandOccupied)
			{
				if (pPreLog && pPreLog->m_eOpType == AirsideFlightStandOperationLog::StandDelay && pPreLog->m_eDelayReason == AirsideFlightStandOperationLog::StandOccupied)
					lStandOccupiedDelay = pLog->time - pPreLog->time;
				else
				{
					ASSERT(0);		//error
				}
			}

			//create new log, if has a stand operation
			if (pLog->m_eOpType == AirsideFlightStandOperationLog::EnteringStand) // enter stand
			{
				pData = new CStandOperationReportData;
				pData->m_sACType = fltdesc.sAcType.c_str();

				if(pLog->m_eParkingOpType == ARR_PARKING)
				{
					std::string str = fltdesc.sAirline + fltdesc.sArrID;
					pData->m_sID = str.c_str();
					if (bOnlyStand)			//arrival stand same as departure stand and without intermediate parking
					{
						pData->m_sID += "/";
						str = fltdesc.sAirline+ fltdesc.sDepID;
						pData->m_sID += str.c_str();
					}
					pData->m_fltmode = 'A';

					pData->m_sSchedName = fltdesc.sSchedArrStand.c_str();
					pData->m_lSchedOn = fltdesc.SchedArrOn;
					pData->m_lSchedOff = fltdesc.SchedArrOff;
					pData->m_lSchedOccupancy = fltdesc.SchedArrOff - fltdesc.SchedArrOn;
				}
				else
				{
					std::string str = fltdesc.sAirline + fltdesc.sDepID;
					pData->m_sID = str.c_str();
					pData->m_fltmode = 'D';

					if(pLog->m_eParkingOpType == INT_PARKING)
					{
						pData->m_sSchedName = fltdesc.sSchedIntStand.c_str();
						pData->m_lSchedOn = fltdesc.SchedIntOn;
						pData->m_lSchedOff = fltdesc.SchedIntOff;
						pData->m_lSchedOccupancy = fltdesc.SchedIntOff - fltdesc.SchedIntOn;
					}
					else
					{
						pData->m_sSchedName = fltdesc.sSchedDepStand.c_str();
						pData->m_lSchedOn = fltdesc.SchedDepOn;
						pData->m_lSchedOff = fltdesc.SchedDepOff;
						pData->m_lSchedOccupancy = fltdesc.SchedDepOff - fltdesc.SchedDepOn;
					}
				}
				pData->m_sActualName = pLog->m_altStandID.GetIDString();
				pData->m_lActualOn = pLog->time;

				pData->m_lDelayEnter = lStandOccupiedDelay;
				pData->m_lDueStandOccupied = lStandOccupiedDelay;
			}
		}
		else
		{
			if (pPreLog->m_eOpType == AirsideFlightStandOperationLog::StandDelay)
			{
				switch (pPreLog->m_eDelayReason)
				{
					case AirsideFlightStandOperationLog::GSEDelay:
						lGSEDelay += pLog->time - pPreLog->time;
						break;
					case AirsideFlightStandOperationLog::PushbackClearance:
						lPushbackClearanceDelay += pLog->time - pPreLog->time;
						break;
					case AirsideFlightStandOperationLog::TaxiwayOccupiedDelay:
						lTaxiwayOccupiedDelay += pLog->time - pPreLog->time;
						break;
					default:
						ASSERT(0);
						break;
				}
			}

			if (pLog->m_eOpType == AirsideFlightStandOperationLog::LeavingStand)
			{
				pData->m_lActualOff = pLog->time;
				pData->m_lActualOccupancy = pData->m_lActualOff - pData->m_lActualOn;

				pData->m_lDueTaxiwayOccupied = lTaxiwayOccupiedDelay;
				pData->m_lDueGSE = lGSEDelay;
				pData->m_lDuePushbackClearance = lPushbackClearanceDelay;
				pData->m_lDelayLeaving = lTaxiwayOccupiedDelay + lGSEDelay + lPushbackClearanceDelay;

				m_vStandOperationReportData.push_back(pData);

				pData = NULL;

				lStandOccupiedDelay = 0;
				lGSEDelay = 0;
				lPushbackClearanceDelay = 0;
				lTaxiwayOccupiedDelay = 0;
			}
		}

	}
}

const std::vector<CStandOperationReportData*>& StandOperationDataProcessor::GetData()
{
	return m_vStandOperationReportData;
}
