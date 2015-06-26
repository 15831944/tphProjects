#pragma once
#include <vector>
#include "StandOperationData.h"

static bool DataCompareBySchedStand(CStandOperationReportData* pFirst, CStandOperationReportData* pSecond)
{
	if (pFirst->m_sSchedName < pSecond->m_sSchedName)
		return true;

	return false;
}

static bool DataCompareByActualStandWithMode(CStandOperationReportData* pFirst, CStandOperationReportData* pSecond)
{
	if (pFirst->m_sActualName < pSecond->m_sActualName)
		return true;

	if (pFirst->m_sActualName == pSecond->m_sActualName)
	{
		if (pFirst->m_fltmode < pSecond->m_fltmode)
			return true;
	}


	return false;
}

static bool DataCompareByActualStand(CStandOperationReportData* pFirst, CStandOperationReportData* pSecond)
{
	if (pFirst->m_sActualName < pSecond->m_sActualName)
		return true;

	return false;
}

class LongValue
{
public:
	LongValue():_data(0){}
	LongValue& operator+=(long d){ _data+=d; return *this; }
	long _data;
};

static void CalculateStandOccupancyTime(bool bSchedStand, std::vector<CStandOperationReportData*>& vResult, std::vector<long>& vOccupanyTime)
{
	if(bSchedStand)
	{
		std::map<CString,LongValue> standMap;
		for(size_t i=0;i< vResult.size();i++)
		{
			CStandOperationReportData* data = vResult.at(i);
			standMap[data->m_sSchedName]+= data->m_lSchedOccupancy;
		}
		for(std::map<CString,LongValue>::const_iterator itr= standMap.begin();itr!=standMap.end();++itr)
		{
			vOccupanyTime.push_back(itr->second._data/100);
		}
	}
	else
	{
		std::map<CString,LongValue> standMap;
		for(size_t i=0;i< vResult.size();i++)
		{
			CStandOperationReportData* data = vResult.at(i);
			standMap[data->m_sActualName]+= data->m_lActualOccupancy;
		}
		for(std::map<CString,LongValue>::const_iterator itr= standMap.begin();itr!=standMap.end();++itr)
		{
			vOccupanyTime.push_back(itr->second._data/100);
		}
	}
}

static void CalculateStandOccupancyTime2( bool bSchedStand, std::vector<CStandOperationReportData*>& vResult, std::vector<long>& vOccupanyTime )
{
	long lTotal = 0; 

	if (!vResult.empty())
	{
		if (bSchedStand)
		{
			std::sort(vResult.begin(), vResult.end(),DataCompareBySchedStand);

			CStandOperationReportData* pData = vResult.at(0);
			CString sStandName = pData->m_sSchedName;
			
			size_t nSize = vResult.size();
			for (size_t i =0; i < nSize; i++)
			{
				pData = vResult.at(i);
				if (pData->m_sSchedName == sStandName)
					lTotal += pData->m_lSchedOccupancy;
				else
				{
					vOccupanyTime.push_back(lTotal/100);
					sStandName = pData->m_sSchedName;
					lTotal = pData->m_lSchedOccupancy;
				}
			}
		}
		else
		{
			std::sort(vResult.begin(), vResult.end(),DataCompareByActualStand);

			CStandOperationReportData* pData = vResult.at(0);
			CString sStandName = pData->m_sActualName;

			size_t nSize = vResult.size();
			for (size_t i =0; i < nSize; i++)
			{
				pData = vResult.at(i);
				if (pData->m_sActualName == sStandName)
					lTotal += pData->m_lActualOccupancy;
				else
				{
					vOccupanyTime.push_back(lTotal/100);
					sStandName = pData->m_sActualName;
					lTotal = pData->m_lActualOccupancy;
				}
			}

		}
	}
	
	if(vOccupanyTime.size()<=0)
	{
		vOccupanyTime.push_back(lTotal/100);
	}
}

static void CalculateStandConflict(char mode, std::vector<CStandOperationReportData*>& vResult, std::vector<int>& vConflicts )
{
	if (vResult.empty())
		return;

	std::sort(vResult.begin(), vResult.end(),DataCompareByActualStandWithMode);

	CStandOperationReportData* pData = vResult.at(0);
	CString sStandName = pData->m_sActualName;
	int nTotal = 0; 

	size_t nSize = vResult.size();
	for (size_t i =0; i < nSize; i++)
	{
		pData = vResult.at(i);
		if (!pData->IsConflict() || pData->m_fltmode != mode)
			continue;

		if (pData->m_sActualName == sStandName)
			nTotal ++;
		else
		{
			vConflicts.push_back(nTotal);
			sStandName = pData->m_sActualName;
			nTotal = 1;
		}
	}
}

static void CalculateStandConflict(std::vector<CStandOperationReportData*>& vResult, std::vector<int>& vConflicts )
{
	if (vResult.empty())
		return;

	std::sort(vResult.begin(), vResult.end(),DataCompareByActualStand);

	CStandOperationReportData* pData = vResult.at(0);
	CString sStandName = pData->m_sActualName;
	int nTotal = 0; 

	size_t nSize = vResult.size();
	for (size_t i =0; i < nSize; i++)
	{
		pData = vResult.at(i);
		if (!pData->IsConflict())
			continue;

		if (pData->m_sActualName == sStandName)
			nTotal ++;
		else
		{
			vConflicts.push_back(nTotal);
			sStandName = pData->m_sActualName;
			nTotal = 1;
		}
	}
}


static void CalculateStandDelay(char mode, std::vector<CStandOperationReportData*>& vResult, std::vector<long>& vOccupanyTime )
{
	if (vResult.empty())
		return;
	
	std::sort(vResult.begin(), vResult.end(),DataCompareByActualStandWithMode);

	CStandOperationReportData* pData = vResult.at(0);
	CString sStandName = pData->m_sActualName;
	long lTotal = 0; 

	size_t nSize = vResult.size();
	for (size_t i =0; i < nSize; i++)
	{
		pData = vResult.at(i);
		if (!pData->IsDelay() || pData->m_fltmode != mode)
			continue;

		if (pData->m_sActualName == sStandName)
		{
			lTotal += pData->m_lDelayEnter;
			lTotal += pData->m_lDelayLeaving;
		}
		else
		{
			vOccupanyTime.push_back(lTotal/100);
			sStandName = pData->m_sActualName;
			lTotal = pData->m_lDelayEnter + pData->m_lDelayLeaving;
		}
	}
}

static void CalculateStandDelay(std::vector<CStandOperationReportData*>& vResult, std::vector<long>& vOccupanyTime )
{
	std::sort(vResult.begin(), vResult.end(),DataCompareByActualStand);

	CStandOperationReportData* pData = vResult.at(0);
	CString sStandName = pData->m_sActualName;
	long lTotal = 0; 

	size_t nSize = vResult.size();
	for (size_t i =0; i < nSize; i++)
	{
		pData = vResult.at(i);
		if (!pData->IsDelay())
			continue;

		if (pData->m_sActualName == sStandName)
		{
			lTotal += pData->m_lDelayEnter;
			lTotal += pData->m_lDelayLeaving;
		}
		else
		{
			vOccupanyTime.push_back(lTotal/100);
			sStandName = pData->m_sActualName;
			lTotal = pData->m_lDelayEnter + pData->m_lDelayLeaving;
		}
	}
}