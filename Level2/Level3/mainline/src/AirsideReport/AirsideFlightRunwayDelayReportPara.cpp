#include "StdAfx.h"
#include ".\airsideflightrunwaydelayreportpara.h"
#include "../Results/AirsideFllightRunwayDelayLog.h"
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelaySummaryData.h"
#include "AirsideFlightRunwayDelayReport.h"
#include "../Common/FLT_CNST.H"

AirsideFlightRunwayDelayReportPara::AirsideFlightRunwayDelayReportPara()
:CAirsideRunwayUtilizationReportParam()
{
	m_lInterval = 60l;
}

AirsideFlightRunwayDelayReportPara::~AirsideFlightRunwayDelayReportPara(void)
{
}

CString AirsideFlightRunwayDelayReportPara::GetReportParamName()
{
	return  _T("RunwayDelays\\RunwayDelays.prm");
}

bool AirsideFlightRunwayDelayReportPara::IsDelayLogFitPara(const AirsideFlightRunwayDelayLog* pLog, const AirsideFlightDescStruct& fltDesc)
{
	if(pLog == NULL)
		return false;

	//check runway 
	CAirsideReportRunwayMark reportRunwayMark;
	reportRunwayMark.SetRunway(pLog->mAtRunway.resid, (RUNWAY_MARK)(int)pLog->mAtRunway.resdir,pLog->mAtRunway.strRes.c_str());

	int nRunwayParaCount = (int)m_vReportMarkParam.size();//no any setting, means default
	if(nRunwayParaCount == 0 )
		return true;

	for (int nRunway = 0; nRunway < nRunwayParaCount; ++ nRunway)
	{
		//fit runway mark
		CAirsideRunwayUtilizationReportParam::CRunwayMarkParam& runwayMarkParam = m_vReportMarkParam[nRunway];

		if(!runwayMarkParam.IsRunwayMarkFit(reportRunwayMark))//runway mark fit
			continue;

		//fit operation
		bool bOpeationFit = false;
		int nOperationCount = (int)runwayMarkParam.m_vOperationParam.size();
		CAirsideRunwayUtilizationReportParam::RunwayOperation enumCurRunwayOperation;

		char mode = 'A';
		if(pLog->mMode <= OnExitRunway)
			enumCurRunwayOperation = CAirsideRunwayUtilizationReportParam::Landing;
		else 
		{
			enumCurRunwayOperation = CAirsideRunwayUtilizationReportParam::TakeOff;
			mode = 'D';
		}

		for (int nOperation = 0; nOperation < nOperationCount; ++nOperation)
		{
			CAirsideRunwayUtilizationReportParam::CRunwayOperationParam& runwayOperaitonPara = runwayMarkParam.m_vOperationParam[nOperation];
			if(runwayOperaitonPara.m_enumOperation == CAirsideRunwayUtilizationReportParam::Both ||
				runwayOperaitonPara.m_enumOperation == enumCurRunwayOperation)// operation fit
			{
				//fit flight type
				AirsideFlightDescStruct compfltDesc = fltDesc;
				if(runwayOperaitonPara.fitFltCons(compfltDesc,mode))
					return true;			
			}
		}
	}

	return false;
}

bool AirsideFlightRunwayDelayReportPara::IsDetailDataFitChartType(const AirsideFlightRunwayDelayData::RunwayDelayItem* pData)
{
	if (getSubType() == AirsideFlightRunwayDelayReport::ChartType_Detail_Total)
		return true;

	if (getSubType() == pData->m_PositionType +2)
		return true;

	return false;
}

bool AirsideFlightRunwayDelayReportPara::IsSummaryDataFitChartType(const AirsideFlightRunwayDelaySummaryData* pData)
{
	if (getSubType() == AirsideFlightRunwayDelayReport::ChartType_Summary_Total)
		return true;

	if (getSubType() == pData->m_nPosition + 102)
		return true;

	return false;
}

char AirsideFlightRunwayDelayReportPara::getFltTypeMode()
{
	bool bArr = false;
	bool bDep = false;

	int nRunwayParaCount = (int)m_vReportMarkParam.size();//no any setting, means default
	if(nRunwayParaCount == 0 )
		return 'B';

	for (int nRunway = 0; nRunway < nRunwayParaCount; ++ nRunway)
	{
		//fit runway mark
		CAirsideRunwayUtilizationReportParam::CRunwayMarkParam& runwayMarkParam = m_vReportMarkParam[nRunway];

		int nOperationCount = (int)runwayMarkParam.m_vOperationParam.size();

		for (int nOperation = 0; nOperation < nOperationCount; ++nOperation)
		{
			CAirsideRunwayUtilizationReportParam::CRunwayOperationParam& runwayOperaitonPara = runwayMarkParam.m_vOperationParam[nOperation];
			if(runwayOperaitonPara.m_enumOperation == CAirsideRunwayUtilizationReportParam::Landing)
				bArr = true;
			if(runwayOperaitonPara.m_enumOperation == CAirsideRunwayUtilizationReportParam::TakeOff)
				bDep = true;

			if(runwayOperaitonPara.m_enumOperation == CAirsideRunwayUtilizationReportParam::Both)
			{
				int nFltCount = runwayOperaitonPara.m_vFlightConstraint.size();
				for(int nFlt =0; nFlt < nFltCount; nFlt++)
				{
					if ((runwayOperaitonPara.m_vFlightConstraint.at(nFlt)).GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ALL
						|| (runwayOperaitonPara.m_vFlightConstraint.at(nFlt)).GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR)
						bArr = true;

					if ((runwayOperaitonPara.m_vFlightConstraint.at(nFlt)).GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ALL
						|| (runwayOperaitonPara.m_vFlightConstraint.at(nFlt)).GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP)
						bDep = true;

				}

			}

		}
	}

	if (bArr && !bDep)
		return 'A';

	if (bDep && !bArr)
		return 'D';

	return 'B';

}