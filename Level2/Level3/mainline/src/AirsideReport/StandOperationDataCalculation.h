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

static bool IfStandOperationAvailable(CStandOperationReportData* pData)
{
	return (pData->m_lSchedOn < 0 && pData->m_lSchedOff < 0) || (pData->m_lActualOn < 0 && pData->m_lActualOff < 0);
}

static void CalculateStandOccupancyTime(bool bSchedStand, std::vector<CStandOperationReportData*>& vResult, std::vector<long>& vOccupanyTime)
{
	if(bSchedStand)
	{
		std::map<CString,LongValue> standMap;
		for(size_t i=0;i< vResult.size();i++)
		{
			CStandOperationReportData* data = vResult.at(i);
			if (data->m_sSchedName.IsEmpty() == false && (data->m_lSchedOn>=0 && data->m_lSchedOff>=0))
			{
				standMap[data->m_sSchedName]+= data->m_lSchedAvailableOccupancy;
			}
		}
		for(std::map<CString,LongValue>::const_iterator itr= standMap.begin();itr!=standMap.end();++itr)
		{
			//to second
			vOccupanyTime.push_back(itr->second._data/100);
		}
	}
	else
	{
		std::map<CString,LongValue> standMap;
		for(size_t i=0;i< vResult.size();i++)
		{
			CStandOperationReportData* data = vResult.at(i);
			if (data->m_sActualName.IsEmpty() == false &&(data->m_lActualOn>=0&&data->m_lActualOff>=0))
			{
				standMap[data->m_sActualName]+= data->m_lActualAvailableOccupancy;
			}	
		}
		for(std::map<CString,LongValue>::const_iterator itr= standMap.begin();itr!=standMap.end();++itr)
		{
			//to second
			vOccupanyTime.push_back(itr->second._data/100);
		}
	}
}


static void CalculateStandConflict(char mode, std::vector<CStandOperationReportData*>& vResult, std::vector<int>& vConflicts )
{
	std::vector<CStandOperationReportData*> vReportData = vResult;
	std::vector<CStandOperationReportData*>::iterator iter = std::remove_if(vReportData.begin(),vReportData.end(),IfStandOperationAvailable);
	vReportData.erase(iter,vReportData.end());

	if (vReportData.empty())
		return;

	std::sort(vReportData.begin(), vReportData.end(),DataCompareByActualStandWithMode);

	CStandOperationReportData* pData = vReportData.at(0);
	CString sStandName = pData->m_sActualName;
	int nTotal = 0; 

	size_t nSize = vReportData.size();
	for (size_t i =0; i < nSize; i++)
	{
		pData = vReportData.at(i);
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

        if(i == nSize-1 && nTotal != 0)
        {
            vConflicts.push_back(nTotal);
        }
	}
}

static void CalculateStandConflict(std::vector<CStandOperationReportData*>& vResult, std::vector<int>& vConflicts )
{
	std::vector<CStandOperationReportData*> vReportData = vResult;
	std::vector<CStandOperationReportData*>::iterator iter = std::remove_if(vReportData.begin(),vReportData.end(),IfStandOperationAvailable);
	vReportData.erase(iter,vReportData.end());

	if (vReportData.empty())
		return;

	std::sort(vReportData.begin(), vReportData.end(),DataCompareByActualStand);

	CStandOperationReportData* pData = vReportData.at(0);
	CString sStandName;// = pData->m_sActualName;
	int nTotal = 0; 

	size_t nSize = vReportData.size();
	for (size_t i =0; i < nSize; i++)
	{
		pData = vReportData.at(i);
		if (!pData->IsConflict())
			continue;

		if (pData->m_sActualName == sStandName)
			nTotal ++;
		else
		{
			if(!sStandName.IsEmpty())
				vConflicts.push_back(nTotal);
			
			sStandName = pData->m_sActualName;
			nTotal = 1;
		}


	} 
	if(/*i == nSize-1 &&*/ nTotal != 0)
    {
        vConflicts.push_back(nTotal);
    }
}


static void CalculateStandDelay(char mode, std::vector<CStandOperationReportData*>& vResult, std::vector<long>& vOccupanyTime )
{
	std::vector<CStandOperationReportData*> vReportData = vResult;
	std::vector<CStandOperationReportData*>::iterator iter = std::remove_if(vReportData.begin(),vReportData.end(),IfStandOperationAvailable);
	vReportData.erase(iter,vReportData.end());

	if (vReportData.empty())
		return;

	std::sort(vReportData.begin(), vReportData.end(),DataCompareByActualStandWithMode);

	CStandOperationReportData* pData = vReportData.at(0);
	CString sStandName;// = pData->m_sActualName;
	long lTotal = 0; 

	size_t nSize = vReportData.size();
	for (size_t i =0; i < nSize; i++)
	{
		pData = vReportData.at(i);
		if (!pData->IsDelay() || pData->m_fltmode != mode)
			continue;

		if (pData->m_sActualName == sStandName)
		{
			lTotal += pData->m_lDelayEnter;
			lTotal += pData->m_lDelayLeaving;
		}
		else
		{
			if(!sStandName.IsEmpty())
				vOccupanyTime.push_back(lTotal/100);
			
			sStandName = pData->m_sActualName;
			lTotal = pData->m_lDelayEnter + pData->m_lDelayLeaving;
		}
	}
    if(/*i == nSize-1 &&*/ lTotal != 0)
    {
        vOccupanyTime.push_back(lTotal/100);
    }
}

static void CalculateStandDelay(std::vector<CStandOperationReportData*>& vResult, std::vector<long>& vOccupanyTime )
{
	std::vector<CStandOperationReportData*> vReportData = vResult;
	std::vector<CStandOperationReportData*>::iterator iter = std::remove_if(vReportData.begin(),vReportData.end(),IfStandOperationAvailable);
	vReportData.erase(iter,vReportData.end());

	std::sort(vReportData.begin(), vReportData.end(),DataCompareByActualStand);

	CStandOperationReportData* pData = vReportData.at(0);
	CString sStandName;// = pData->m_sActualName;
	long lTotal = 0; 

	size_t nSize = vReportData.size();
	for (size_t i =0; i < nSize; i++)
	{
		pData = vReportData.at(i);
		if (!pData->IsDelay())
			continue;

		if (pData->m_sActualName == sStandName)
		{
			lTotal += pData->m_lDelayEnter;
			lTotal += pData->m_lDelayLeaving;
		}
		else
		{
			if(!sStandName.IsEmpty())
			vOccupanyTime.push_back(lTotal/100);
			sStandName = pData->m_sActualName;
			lTotal = pData->m_lDelayEnter + pData->m_lDelayLeaving;
		}
	} 
	if(/*i == nSize-1 &&*/ lTotal != 0)
    {
        vOccupanyTime.push_back(lTotal/100);
	}
}