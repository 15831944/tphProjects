#include "StdAfx.h"
#include "StandOperationDataProcessor.h"
#include "Parameters.h"
 #include "Results/AirsideFlightLogItem.h"
 #include "Results/AirsideFlightEventLog.h"
#include "Results/ARCBaseLog.h"
#include "Results/ARCBaseLog.hpp"
#include "Common/AIRSIDE_BIN.h"
#include "Engine/Airside/AirsideFlightInSim.h"
#include "../Common/Aircraft.h"
#include "Inputs/IN_TERM.H"
#include "Inputs/SCHEDULE.H"
#include "Common/ProgressBar.h"
#include "FlightStandOperationParameter.h"

StandOperationDataProcessor::StandOperationDataProcessor(CBCScheduleStand pFunc)
	:m_pScheduleStand(pFunc)
{
	m_nIdleActualStand = 0;
	m_nIdleScheduleStand = 0;
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
	CString strObjectPath = (*pCBGetLogFilePath)(AirsideALTObjectListFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty() || strObjectPath.IsEmpty())
		return;

	ARCBaseLog<AirsideFlightLogItem> flightLogData;
	flightLogData.OpenInput(strDescFilepath, strEventFilePath);

	CStandOperationReportData* pStandOpReportData = NULL;

	int nCount = flightLogData.getItemCount();

	CProgressBar bar(_T("Fetch logs and build original data..."), 100, nCount, TRUE);

	std::vector<AirsideFlightLogDesc> logDesList;
	for (int i = 0; i < nCount; i++)
	{
		std::vector<AirsideFlightStandOperationLog*> vStandOpLogs;

		flightLogData.LoadItem(flightLogData.ItemAt(i));
		AirsideFlightLogItem item = flightLogData.ItemAt(i);

		logDesList.push_back(item.mFltDesc);
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

			ProcessLogs(vOneStandOpLogs, pParameter,item, bOnlyStand);
		}

		bar.StepIt();
	}
	
	std::vector<ALTObjectID> vScheduleStandID;
	for (int iDesc = 0; iDesc< nCount;  iDesc++)
	{
		AirsideFlightLogDesc fltdesc = logDesList[iDesc];
		CString strAcType = fltdesc.sAcType.c_str();
		CString strFlightID;

		if (fltdesc.sArrID.empty() == false)
		{
			std::string str = fltdesc.sAirline + fltdesc.sArrID;
			strFlightID= str.c_str();
		}

		if (fltdesc.sDepID.empty() == false)
		{
			strFlightID += "/";
			std::string str = fltdesc.sAirline + fltdesc.sDepID;
			strFlightID += str.c_str();
		}
		//arr stand
		if(fltdesc.sSchedArrStand.empty() == false)
		{	
			if (TimeRangeOverlap(fltdesc.SchedArrOn,fltdesc.SchedArrOff,pParameter))//time range in start to end
			{
				ALTObjectID arrStand;
				arrStand.FromString(fltdesc.sSchedArrStand.c_str());
				if (FindFlightScheduleStand(arrStand,fltdesc.id,fltdesc.SchedArrOn,fltdesc.SchedArrOff) == false)
				{
					CStandOperationReportData* pData = new CStandOperationReportData();
					pData->m_lFlightIndex = fltdesc.id;
					pData->m_sID = strFlightID;
					pData->m_sACType = strAcType;
					pData->m_fltmode = 'A';
					pData->m_lSchedOn = fltdesc.SchedArrOn;
					pData->m_lSchedOff = fltdesc.SchedArrOff;
					pData->m_sSchedName = fltdesc.sSchedArrStand.c_str();
					pData->m_lSchedOccupancy = fltdesc.SchedArrOff - fltdesc.SchedArrOn;


					long lMinOffTime =MIN(pParameter->getEndTime().getPrecisely(),fltdesc.SchedArrOff);
					long lMaxOnTime = MAX(pParameter->getStartTime().getPrecisely(),fltdesc.SchedArrOn);
					pData->m_lSchedAvailableOccupancy = MAX(lMinOffTime - lMaxOnTime,0);
					m_vStandOperationReportData.push_back(pData);
				}
				if (std::find(vScheduleStandID.begin(),vScheduleStandID.end(),arrStand) == vScheduleStandID.end())
				{
					vScheduleStandID.push_back(arrStand);
				}
			}
		}
		//intermediate stand
		if(fltdesc.sSchedIntStand.empty() == false)
		{
			if (TimeRangeOverlap(fltdesc.SchedIntOn,fltdesc.SchedIntOff,pParameter))//time range in start to end
			{
				ALTObjectID intStand;
				intStand.FromString(fltdesc.sSchedIntStand.c_str());
				if (FindFlightScheduleStand(intStand,fltdesc.id,fltdesc.SchedIntOn,fltdesc.SchedIntOff) == false)
				{
					CStandOperationReportData* pData = new CStandOperationReportData();
					pData->m_lFlightIndex = fltdesc.id;
					pData->m_sID = strFlightID;
					pData->m_sACType = strAcType;
					pData->m_fltmode = 'D';
					pData->m_lSchedOn = fltdesc.SchedIntOn;
					pData->m_lSchedOff = fltdesc.SchedIntOff;
					pData->m_sSchedName = fltdesc.sSchedIntStand.c_str();
					pData->m_lSchedOccupancy = fltdesc.SchedIntOff - fltdesc.SchedIntOn;


					long lMinOffTime =MIN(pParameter->getEndTime().getPrecisely(),fltdesc.SchedIntOff);
					long lMaxOnTime = MAX(pParameter->getStartTime().getPrecisely(),fltdesc.SchedIntOn);
					pData->m_lSchedAvailableOccupancy = MAX(lMinOffTime - lMaxOnTime,0);
					m_vStandOperationReportData.push_back(pData);
				}
				if (std::find(vScheduleStandID.begin(),vScheduleStandID.end(),intStand) == vScheduleStandID.end())
				{
					vScheduleStandID.push_back(intStand);
				}
			}

		
		}

		//departure stand
		if(fltdesc.sSchedDepStand.empty() == false)
		{
			long lSchedOn = fltdesc.SchedDepOn;
			if (fltdesc.sSchedArrStand == fltdesc.sSchedDepStand && fltdesc.sSchedIntStand.empty() == true)
			{
				lSchedOn = fltdesc.SchedArrOn;
			}
			if (TimeRangeOverlap(lSchedOn,fltdesc.SchedDepOff,pParameter))//time range in start to end
			{
				ALTObjectID depStand;
				depStand.FromString(fltdesc.sSchedDepStand.c_str());
				if (FindFlightScheduleStand(depStand,fltdesc.id,lSchedOn,fltdesc.SchedDepOff) == false)
				{
					CStandOperationReportData* pData = new CStandOperationReportData();
					pData->m_lFlightIndex = fltdesc.id;
					pData->m_sID = strFlightID;
					pData->m_sACType = strAcType;
					pData->m_fltmode = 'D';
					pData->m_lSchedOn = lSchedOn;
					pData->m_lSchedOff = fltdesc.SchedDepOff;
					pData->m_sSchedName = fltdesc.sSchedDepStand.c_str();
					pData->m_lSchedOccupancy = fltdesc.SchedDepOff - lSchedOn;

					long lMinOffTime =MIN(pParameter->getEndTime().getPrecisely(),fltdesc.SchedDepOff);
					long lMaxOnTime = MAX(pParameter->getStartTime().getPrecisely(),lSchedOn);
					pData->m_lSchedAvailableOccupancy = MAX(lMinOffTime - lMaxOnTime,0);
					m_vStandOperationReportData.push_back(pData);
				}
				if (std::find(vScheduleStandID.begin(),vScheduleStandID.end(),depStand) == vScheduleStandID.end())
				{
					vScheduleStandID.push_back(depStand);
				}
			}
			
		}
	
	}
	ARCBaseLog<AirsideALTObjectLogItem> objectLogData;
	objectLogData.OpenObjectInput(strObjectPath);
	int nObjectCount = objectLogData.getItemCount();
	std::vector<ALTObjectID> vStandID;
	for (int nObject = 0; nObject < nObjectCount; nObject++)
	{
		AirsideALTObjectLogItem item = objectLogData.ItemAt(nObject);
		if (!pStandOpPara->fits(item.m_altObject))
			continue;

		vStandID.push_back(item.m_altObject);
	}

	unsigned nAllStandCount = vStandID.size();
	for (unsigned nStand = 0; nStand < nAllStandCount; nStand++)
	{
		ALTObjectID standID = vStandID.at(nStand);
		bool bSchedule = true;
		bool bActual = true;
		if (FindStandUseActualInformation(standID) == false)
		{
			m_nIdleActualStand++;
			bActual = false;
		}
		
		if (std::find(vScheduleStandID.begin(),vScheduleStandID.end(),standID) == vScheduleStandID.end())
		{
			m_nIdleScheduleStand++;
			bSchedule = false;
		}

		if (bSchedule == false && bActual == false)
		{
			CStandOperationReportData* pData = new CStandOperationReportData();
			pData->m_sSchedName = standID.GetIDString();
			pData->m_lSchedOccupancy = 0l;

			pData->m_sActualName = standID.GetIDString();
			pData->m_lActualOccupancy = 0l;

			m_vStandOperationReportData.push_back(pData);
		}
		else if (bSchedule == false)
		{
			CStandOperationReportData* pData = new CStandOperationReportData();
			pData->m_sSchedName = standID.GetIDString();
			pData->m_lSchedOccupancy = 0l;

			m_vStandOperationReportData.push_back(pData);
		}
		else if (bActual == false)
		{
			CStandOperationReportData* pData = new CStandOperationReportData();
			pData->m_sActualName = standID.GetIDString();
			pData->m_lActualOccupancy = 0l;

			m_vStandOperationReportData.push_back(pData);
		}
	}
}


bool StandOperationDataProcessor::FindFlightScheduleStand( const ALTObjectID& standID,long lFlightIndex,long lScheduleOn,long lScheduleOff )
{
	unsigned nCount = m_vStandOperationReportData.size();
	for (unsigned i = 0; i < nCount; i++)
	{
		CStandOperationReportData* pStandData = m_vStandOperationReportData.at(i);
		ALTObjectID useStand;
		useStand.FromString(pStandData->m_sSchedName);
		if (standID == useStand && pStandData->m_lFlightIndex == lFlightIndex&&
			pStandData->m_lSchedOn == lScheduleOn&&pStandData->m_lSchedOff == lScheduleOff)
		{
			return true;
		}
	}
	return false;
}

void StandOperationDataProcessor::ProcessLogs( std::vector<AirsideFlightStandOperationLog*>& vStandOpLogs, CParameters* pParameter,AirsideFlightLogItem& item, bool bOnlyStand)
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

				if (fltdesc.sArrID.empty() == false)
				{
					std::string str = fltdesc.sAirline + fltdesc.sArrID;
					pData->m_sID = str.c_str();
				}

				if (fltdesc.sDepID.empty() == false)
				{
					pData->m_sID += "/";
					std::string str = fltdesc.sAirline + fltdesc.sDepID;
					pData->m_sID += str.c_str();
				}

				pData->m_lFlightIndex = fltdesc.id;

				if(pLog->m_eParkingOpType == ARR_PARKING)
				{
					//std::string str = fltdesc.sAirline + fltdesc.sArrID;
					//pData->m_sID = str.c_str();
					//if (bOnlyStand)			//arrival stand same as departure stand and without intermediate parking
					//{
					//	pData->m_sID += "/";
					//	str = fltdesc.sAirline+ fltdesc.sDepID;
					//	pData->m_sID += str.c_str();
					//}
					pData->m_fltmode = 'A';

					pData->m_sSchedName = fltdesc.sSchedArrStand.c_str();
					pData->m_lSchedOn = fltdesc.SchedArrOn;
					pData->m_lSchedOff = fltdesc.SchedArrOff;
					pData->m_lSchedOccupancy = fltdesc.SchedArrOff - fltdesc.SchedArrOn;
				}
				else
				{
					//std::string str = fltdesc.sAirline + fltdesc.sDepID;
					//pData->m_sID = str.c_str();
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
				if (TimeRangeOverlap(pData->m_lActualOn,pData->m_lActualOff,pParameter) == false)
				{
					delete pData;
				}
				else
				{
					pData->m_lActualOccupancy = pData->m_lActualOff - pData->m_lActualOn;

					long lSchedMinOffTime =MIN(pParameter->getEndTime().getPrecisely(),pData->m_lSchedOff );
					long lSchedMaxOnTime = MAX(pParameter->getStartTime().getPrecisely(),pData->m_lSchedOn);
					pData->m_lSchedAvailableOccupancy = MAX(lSchedMinOffTime - lSchedMaxOnTime,0);
					
					long lActualMinOffTime =MIN(pParameter->getEndTime().getPrecisely(),pData->m_lActualOff );
					long lActualMaxOnTime = MAX(pParameter->getStartTime().getPrecisely(),pData->m_lActualOn);
					pData->m_lActualAvailableOccupancy = MAX(lActualMinOffTime - lActualMaxOnTime,0);

					pData->m_lDueTaxiwayOccupied = lTaxiwayOccupiedDelay;
					pData->m_lDueGSE = lGSEDelay;
					pData->m_lDuePushbackClearance = lPushbackClearanceDelay;
					pData->m_lDelayLeaving = lTaxiwayOccupiedDelay + lGSEDelay + lPushbackClearanceDelay;

					m_vStandOperationReportData.push_back(pData);
				}
				

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


bool StandOperationDataProcessor::FindStandUseScheduleInformation( const ALTObjectID& standID )
{
	unsigned nCount = m_vStandOperationReportData.size();
	for (unsigned i = 0; i < nCount; i++)
	{
		CStandOperationReportData* pStandData = m_vStandOperationReportData.at(i);
		ALTObjectID useStand;
		useStand.FromString(pStandData->m_sSchedName);
		if (standID == useStand)
		{
			return true;
		}
	}
	return false;
}

bool StandOperationDataProcessor::FindStandUseActualInformation(const ALTObjectID& standID)
{
	unsigned nCount = m_vStandOperationReportData.size();
	for (unsigned i = 0; i < nCount; i++)
	{
		CStandOperationReportData* pStandData = m_vStandOperationReportData.at(i);
		ALTObjectID useStand;
		useStand.FromString(pStandData->m_sActualName);
		if (standID == useStand)
		{
			return true;
		}
	}
	return false;
}
int StandOperationDataProcessor::GetUnuseScheduleStandCount() const
{
	return m_nIdleScheduleStand;
}

int StandOperationDataProcessor::GetUnuseActualStandCount()const
{
	return m_nIdleActualStand;
}

bool StandOperationDataProcessor::TimeRangeOverlap( long eOnTime,long eOffTime,CParameters* pParameter ) const
{
	ElapsedTime eOnStand;
	ElapsedTime eOffStand;
	eOnStand.setPrecisely(eOnTime);
	eOffStand.setPrecisely(eOffTime);
	if (eOnStand > pParameter->getEndTime() || eOffStand < pParameter->getStartTime())//filter not in the time range
	{
		return false;
	}

	return true;
}

