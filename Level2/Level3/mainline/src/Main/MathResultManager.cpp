// MathResultManager.cpp: implementation of the CMathResultManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MathResultManager.h"
#include "MathResultDataSet.h"
#include "../Common/template.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathResultManager::CMathResultManager()
{

}

CMathResultManager::~CMathResultManager()
{

}

void CMathResultManager::LoadResult()
{
	if (m_strResultFileDir.IsEmpty())
		return;

	CMathResultDataSet dataSetQTime(MathQTimeResult);
	dataSetQTime.SetProcessNames(&(m_rsQTime.m_vstrProcess));
	dataSetQTime.SetMathResultRecord(&(m_rsQTime.m_vResult));

	CMathResultDataSet dataSetQLength(MathQLengthResult);
	dataSetQLength.SetProcessNames(&(m_rsQLength.m_vstrProcess));
	dataSetQLength.SetMathResultRecord(&(m_rsQLength.m_vResult));

	CMathResultDataSet dataSetThroughput(MathThroughputResult);
	dataSetThroughput.SetProcessNames(&(m_rsThroughput.m_vstrProcess));
	dataSetThroughput.SetMathResultRecord(&(m_rsThroughput.m_vResult));

	CMathResultDataSet dataSetPaxCount(MathPaxCountResult);
	dataSetPaxCount.SetProcessNames(&(m_rsPaxCount.m_vstrProcess));
	dataSetPaxCount.SetMathResultRecord(&(m_rsPaxCount.m_vResult));

	CMathResultDataSet dataSetUtilization(MathUtilizationResult);
	dataSetUtilization.SetProcessNames(&(m_rsUtilization.m_vstrProcess));
	dataSetUtilization.SetMathResultRecord(&(m_rsUtilization.m_vResult));

	dataSetQTime.loadDataSet(m_strResultFileDir);
	dataSetQLength.loadDataSet(m_strResultFileDir);
	dataSetThroughput.loadDataSet(m_strResultFileDir);
	dataSetPaxCount.loadDataSet(m_strResultFileDir);
	dataSetUtilization.loadDataSet(m_strResultFileDir);
}

void CMathResultManager::SaveResult()
{
	if (m_strResultFileDir.IsEmpty())
		return;

	CheckResult(m_rsQTime);
	CheckResult(m_rsQLength);
	CheckResult(m_rsThroughput, 1.0);
	CheckResult(m_rsPaxCount);
	CheckResult(m_rsUtilization, 1.0);

	CMathResultDataSet dataSetQTime(MathQTimeResult);
	dataSetQTime.SetProcessNames(&(m_rsQTime.m_vstrProcess));
	dataSetQTime.SetMathResultRecord(&(m_rsQTime.m_vResult));

	// TRACE("resultsize: %d\n", m_rsQTime.m_vResult.size());

	CMathResultDataSet dataSetQLength(MathQLengthResult);
	dataSetQLength.SetProcessNames(&(m_rsQLength.m_vstrProcess));
	dataSetQLength.SetMathResultRecord(&(m_rsQLength.m_vResult));

	CMathResultDataSet dataSetThroughput(MathThroughputResult);
	dataSetThroughput.SetProcessNames(&(m_rsThroughput.m_vstrProcess));
	dataSetThroughput.SetMathResultRecord(&(m_rsThroughput.m_vResult));

	CMathResultDataSet dataSetPaxCount(MathPaxCountResult);
	dataSetPaxCount.SetProcessNames(&(m_rsPaxCount.m_vstrProcess));
	dataSetPaxCount.SetMathResultRecord(&(m_rsPaxCount.m_vResult));

	CMathResultDataSet dataSetUtilization(MathUtilizationResult);
	dataSetUtilization.SetProcessNames(&(m_rsUtilization.m_vstrProcess));
	dataSetUtilization.SetMathResultRecord(&(m_rsUtilization.m_vResult));


	CFileFind fd;
	BOOL bFind = fd.FindFile(m_strResultFileDir);
	if(!bFind)//not find the directory, create it
		CreateDirectory(m_strResultFileDir, NULL);
	
	dataSetQTime.saveDataSet(m_strResultFileDir, false);
	dataSetQLength.saveDataSet(m_strResultFileDir, false);
	dataSetThroughput.saveDataSet(m_strResultFileDir, false);
	dataSetPaxCount.saveDataSet(m_strResultFileDir, false);
	dataSetUtilization.saveDataSet(m_strResultFileDir, false);

	ClearResult();
}

void CMathResultManager::ClearResult()
{
	m_rsQTime.m_vResult.clear();
	m_rsQTime.m_vstrProcess.clear();

	m_rsQLength.m_vResult.clear();
	m_rsQLength.m_vstrProcess.clear();

	m_rsThroughput.m_vResult.clear();
	m_rsThroughput.m_vstrProcess.clear();

	m_rsPaxCount.m_vResult.clear();
	m_rsPaxCount.m_vstrProcess.clear();

	m_rsUtilization.m_vResult.clear();
	m_rsUtilization.m_vstrProcess.clear();
}

void CMathResultManager::CheckResult(MathResult& rs, double dDefaultValue)
{
	int nProcessCount = static_cast<int>(rs.m_vstrProcess.size());
	for (int i = 0; i < static_cast<int>(rs.m_vResult.size()); i++)
	{
		MathResultRecord& rsRecord = rs.m_vResult[i];
		for (int j = rsRecord.vProcResults.size(); j < nProcessCount; j++)
		{
			rsRecord.vProcResults.push_back(0.0);
		}
	}
}