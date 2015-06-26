#include "StdAfx.h"
#include ".\airsiderunwayoperationreportdetailresult.h"
#include "AirsideRunwayOperationReportParam.h"
#include "../Common\AirportDatabase.h"
#include "AirsideRunwayOperationsReport.h"
#include <algorithm>

const static char* buf[] = {_T("Landing"),_T("Takeoff"),_T("MissApproach")}; 

CAirsideRunwayOperationReportDetailResult::CAirsideRunwayOperationReportDetailResult(void)
{
	m_pParameter = NULL;
	m_pChartResult = NULL;
}

CAirsideRunwayOperationReportDetailResult::~CAirsideRunwayOperationReportDetailResult(void)
{
	std::vector<RunwayOperationDetail *>::iterator iter =  m_vRunwayOperationDetail.begin();
	for (; iter != m_vRunwayOperationDetail.end(); ++iter)
	{
		delete *iter;
	}
	m_vRunwayOperationDetail.clear();

	if(m_pChartResult != NULL)
		delete m_pChartResult;
	m_pChartResult = NULL;

}
 
void CAirsideRunwayOperationReportDetailResult::GenerateResult(CParameters *pParameter )
{
	AirsideRunwayOperationReportParam *pParam = (AirsideRunwayOperationReportParam *)pParameter;
	if(pParam  == NULL)
		return;
	if(m_AirportDB == NULL)
		return;

	m_pParameter = pParam;

	m_airCraftClassicfication.Init(m_AirportDB->getAcMan(),pParam->GetProjID());

	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);

	CString strRunwayOpDescPath = (*m_pCBGetLogFilePath)(AirsideRunwayOperationFile);
	
	m_airsideOperationLog.ClearData();
	m_airsideOperationLog.LoadData(strRunwayOpDescPath);

	
	InitRunway(pParam);
	for (int nOperation = 0; nOperation < m_airsideOperationLog.GetItemCount(); ++ nOperation)
	{
		AirsideRunwayOperationLog::RunwayLogItem *pOperationItem = m_airsideOperationLog.GetItem(nOperation);
		if(pOperationItem == NULL)
			continue;

		if(pOperationItem->m_enumEntryOrExit != AirsideRunwayOperationLog::enumEntryOrExit_Entry)
			continue;
		
		//time range
		if( pOperationItem->m_eTime < pParameter->getStartTime() ||
			pOperationItem->m_eTime > pParameter->getEndTime())
			continue;


		//check runway 
		CAirsideReportRunwayMark reportRunwayMark;
		reportRunwayMark.SetRunway(pOperationItem->m_nRunwayID,pOperationItem->m_enumRunwayMark,pOperationItem->m_strMarkName);
		
		if(!pParam->IsRunwayMarkFit(reportRunwayMark))//runway mark not slected
			continue;
			

		//check constraint
		bool bFitFltConstraint = false;
		AirsideFlightDescStruct	fltDesc;
		int nFltLogCount = fltLog.getCount();
		for (int i =0; i < nFltLogCount; ++i)
		{
			AirsideFlightLogEntry logEntry;
			logEntry.SetEventLog(&airsideFlightEventLog);
			fltLog.getItem(logEntry,i);
			fltDesc = logEntry.GetAirsideDescStruct();
			if(fltDesc.id == pOperationItem->m_nFlightIndex )
			{
				if(fits(pParameter,fltDesc))
				{
					bFitFltConstraint = true;
					break;
				}

			}
		}
		if(bFitFltConstraint == false)//the flight constraint not fit move to next 
			continue;

		//only entry event


		CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail* pRunwayDetail = GetRunwayDetail(reportRunwayMark);
		if(pRunwayDetail == NULL)//create a new
		{
			continue;
		}
		ASSERT(pRunwayDetail != NULL);

		
		//
		//pRunwayDetail->
		//classification information
		
		if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Landing)
		{
			//landing + 1
			pRunwayDetail->m_nTotalLandingCount += 1;
			
			int nClassCount = pRunwayDetail->m_vLandingClassValue.size();
			for (int nClass = 0; nClass < nClassCount; ++ nClass)
			{
				FlightClassificationBasisType fltClassType = pRunwayDetail->m_vLandingClassValue.at(nClass).m_classType;
				AircraftClassificationItem *pACClassItem = NULL;
				if(fltClassType == ApproachSpeedBased)
				{
					pACClassItem =m_airCraftClassicfication.GetApproachSpeedClass(fltDesc.dAprchSpeed);
				}
				else
				{

				  pACClassItem =m_airCraftClassicfication.GetAircraftClass(pOperationItem->m_strFlightType,fltClassType);
				}



				if(pACClassItem == NULL)
					continue;

				int nClassSubTypeValueCount = pRunwayDetail->m_vLandingClassValue.at(nClass).m_vSubTypeValue.size();

				for (int nClassSubType = 0; nClassSubType < nClassSubTypeValueCount; ++nClassSubType)
				{
					if(pRunwayDetail->m_vLandingClassValue.at(nClass).m_vSubTypeValue.at(nClassSubType).m_nTypeID == pACClassItem->getID())
					{
						pRunwayDetail->m_vLandingClassValue[nClass].m_vSubTypeValue[nClassSubType].m_nCount += 1;
						break;
					}
				}
			}

		}
		else if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Takeoff)
		{
			pRunwayDetail->m_nTotalTakeOffCount += 1;

			int nClassCount = pRunwayDetail->m_vTakeOffClassValue.size();
			for (int nClass = 0; nClass < nClassCount; ++ nClass)
			{
				FlightClassificationBasisType fltClassType = pRunwayDetail->m_vTakeOffClassValue.at(nClass).m_classType;

				AircraftClassificationItem *pACClassItem = NULL;
				if(fltClassType == ApproachSpeedBased)
				{
					pACClassItem =m_airCraftClassicfication.GetApproachSpeedClass(fltDesc.dAprchSpeed);
				}
				else
				{
					pACClassItem =m_airCraftClassicfication.GetAircraftClass(pOperationItem->m_strFlightType,fltClassType);
				}
				if(pACClassItem == NULL)
					continue;

				int nClassSubTypeValueCount = pRunwayDetail->m_vTakeOffClassValue.at(nClass).m_vSubTypeValue.size();

				for (int nClassSubType = 0; nClassSubType < nClassSubTypeValueCount; ++nClassSubType)
				{
					if(pRunwayDetail->m_vTakeOffClassValue.at(nClass).m_vSubTypeValue.at(nClassSubType).m_nTypeID == pACClassItem->getID())
					{
						pRunwayDetail->m_vTakeOffClassValue[nClass].m_vSubTypeValue[nClassSubType].m_nCount += 1;
						break;
					}
				}
			}
		}
		else if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_MissApproach)
		{
			pRunwayDetail->m_nMissApproachCount += 1;
		}
		//add wake vortex information
		{
			if(pRunwayDetail->m_vRunwayOperations.size() > 0)
			{
				AirsideRunwayOperationLog::RunwayLogItem *pPreOperationItem = *(pRunwayDetail->m_vRunwayOperations.rbegin());				
				if(pPreOperationItem != NULL)
				{
					AircraftClassificationItem *pLeadACClassItem = m_airCraftClassicfication.GetAircraftClass(pPreOperationItem->m_strFlightType,WakeVortexWightBased);
					if(pLeadACClassItem != NULL)
					{
						AircraftClassificationItem *pTrailACClassItem = m_airCraftClassicfication.GetAircraftClass(pOperationItem->m_strFlightType,WakeVortexWightBased);


						int nLeadTrailCount = pRunwayDetail->m_vWakeVortexDetailValue.size();
						for (int nLeadTrail = 0; nLeadTrail < nLeadTrailCount; ++ nLeadTrail)
						{
							if(pRunwayDetail->m_vWakeVortexDetailValue[nLeadTrail].m_nClassLead == pLeadACClassItem->getID() &&
								pRunwayDetail->m_vWakeVortexDetailValue[nLeadTrail].m_nClassTrail == pTrailACClassItem->getID())
							{

								pRunwayDetail->m_vWakeVortexDetailValue[nLeadTrail].m_nFlightCount += 1;
								break;
							}

						}
					}
				}
			}
		}

		//add to time detail value
		int nTimeDetailCount = pRunwayDetail->m_vRunwayTimeValue.size();
		for (int nTime = 1; nTime < nTimeDetailCount; ++ nTime)
		{
			if(pOperationItem->m_eTime >= pRunwayDetail->m_vRunwayTimeValue[nTime-1].m_eTime &&
				pOperationItem->m_eTime < pRunwayDetail->m_vRunwayTimeValue[nTime].m_eTime)
			{
				if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Landing)
				{
					pRunwayDetail->m_vRunwayTimeValue[nTime-1].m_nLandingCount += 1;
					break;
				}
				else if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Takeoff)
				{
					pRunwayDetail->m_vRunwayTimeValue[nTime-1].m_nTakeOffCount += 1;				
					break;
				}

			}
		
		}

		//calculate flight operation
		if (pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Landing\
			||pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Takeoff\
			||pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_MissApproach)
		{
			if(fitClassification(fltDesc,pRunwayDetail,pOperationItem,pParam))
			{
				FlightOperationData operateData;
				operateData.m_sFlightID = pOperationItem->m_strFlightID;
				operateData.m_lTime = pOperationItem->m_eTime;
				operateData.m_sRunway = pOperationItem->m_strMarkName;
				if(pRunwayDetail->m_vRunwayOperations.size() > 0)
				{
					AirsideRunwayOperationLog::RunwayLogItem *pPreOperationItem = *(pRunwayDetail->m_vRunwayOperations.rbegin());				
					if(pPreOperationItem != NULL)
					{
						AircraftClassificationItem *pLeadACClassItem = m_airCraftClassicfication.GetAircraftClass(pPreOperationItem->m_strFlightType,WakeVortexWightBased);
						if(pLeadACClassItem != NULL)
						{
							AircraftClassificationItem *pTrailACClassItem = m_airCraftClassicfication.GetAircraftClass(pOperationItem->m_strFlightType,WakeVortexWightBased);

							operateData.m_sLeadTrail = pLeadACClassItem->getName() + CString("-") + pTrailACClassItem->getName();
						}
					}
				}

				operateData.m_sOperation = CString(buf[pOperationItem->m_enumOperation]);
				m_vFlightOperateValue.push_back(operateData);
			}
		}
	

	//remember the last one
	pRunwayDetail->m_vRunwayOperations.push_back(pOperationItem);
	
	}
}

bool CAirsideRunwayOperationReportDetailResult::fitClassification(AirsideFlightDescStruct fltDesc,CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail* pRunwayDetail,
																  AirsideRunwayOperationLog::RunwayLogItem* pOperationItem, CParameters * parameter )
{
	AirsideRunwayOperationReportParam *pParam = (AirsideRunwayOperationReportParam *)parameter;
	if(pParam  == NULL)
		return false;

	if (pParam->m_vTypes.empty())
		return true;
	
	std::vector<FlightClassificationBasisType>::iterator iter = pParam->m_vTypes.begin();

	for (;iter != pParam->m_vTypes.end(); ++ iter)
	{
		AircraftClassifications* pAcClass =  m_airCraftClassicfication.GetACClassification(*iter);
		if(pAcClass == NULL)
			continue;

		int nAcCount = pAcClass->GetCount();
		for (int nAC = 0; nAC < nAcCount ; ++nAC)
		{
			AircraftClassificationItem *pItem = pAcClass->GetItem(nAC);
			if(pItem == NULL)
				continue;

			AircraftClassificationItem *pACClassItem = NULL;
			if(pItem->m_emType != WakeVortexWightBased)
				continue;
			
			pACClassItem =m_airCraftClassicfication.GetAircraftClass(pOperationItem->m_strFlightType,pItem->m_emType);
			

			if(pACClassItem == NULL)
				continue;

			if (pItem->getID() == pACClassItem->getID())
				return true;
		}

	}
	return false;
}

void CAirsideRunwayOperationReportDetailResult::Draw3DChart()
{

}

CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail* CAirsideRunwayOperationReportDetailResult::GetRunwayDetail( CAirsideReportRunwayMark& runwayMark )
{
	for (int nDetail = 0; nDetail < static_cast<int>(m_vRunwayOperationDetail.size()); ++ nDetail)
	{
		if(m_vRunwayOperationDetail.at(nDetail)->m_runWaymark.m_nRunwayID == runwayMark.m_nRunwayID &&
			m_vRunwayOperationDetail.at(nDetail)->m_runWaymark.m_enumRunwayMark == runwayMark.m_enumRunwayMark )
			return m_vRunwayOperationDetail[nDetail];
	}

	return NULL;
}
bool CAirsideRunwayOperationReportDetailResult::fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc)
{
	size_t nFltConsCount = parameter->getFlightConstraintCount();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		if(parameter->getFlightConstraint(nfltCons).fits(fltDesc))
		{
			return true;
		}
	}

	return false;
}


void CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail::InitRunwatTimeOperation( ElapsedTime eStartTime,ElapsedTime eEndTime, ElapsedTime eInterval )
{
	ASSERT(eInterval > ElapsedTime(0L));
	if(eInterval == ElapsedTime(0L) )
		return;

	for (;eStartTime <= eEndTime; eStartTime += eInterval)
	{
		RunwayOperationTimeValue timevalue;
		timevalue.m_eTime = eStartTime;
		m_vRunwayTimeValue.push_back(timevalue);
	}
}

void CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail::InitClassValue(AircraftClassificationManager *pAircraftManager, CParameters * parameter )
{
	AirsideRunwayOperationReportParam *pParam = (AirsideRunwayOperationReportParam *)parameter;
	if(pParam  == NULL || pAircraftManager == NULL)
		return;

	std::vector<FlightClassificationBasisType>::iterator iter = pParam->m_vTypes.begin();
	
	for (;iter != pParam->m_vTypes.end(); ++ iter)
	{
		AircraftClassifications* pAcClass =  pAircraftManager->GetACClassification(*iter);
		if(pAcClass == NULL)
			continue;

		RunwayClassValue runwayClassValue;
		runwayClassValue.m_classType = *iter;

		int nAcCount = pAcClass->GetCount();
		for (int nAC = 0; nAC < nAcCount ; ++nAC)
		{
			AircraftClassificationItem *pItem = pAcClass->GetItem(nAC);
			if(pItem == NULL)
				continue;
	
			RunwayClassSubTypeValue subTypeValue;
			subTypeValue.m_nTypeID = pItem->getID();
			subTypeValue.m_strTypeName = pItem->getName();

			runwayClassValue.m_vSubTypeValue.push_back(subTypeValue);
		}

		m_vLandingClassValue.push_back(runwayClassValue);
		m_vTakeOffClassValue.push_back(runwayClassValue);
	}
}

void CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail::InitWakeVortex( AircraftClassificationManager* pAircraftManager )
{

	//WingspanBased = 0,
	//	SurfaceBearingWeightBased,
	//	WakeVortexWightBased,
	//	ApproachSpeedBased,
	//	NoneBased,

	//	CategoryType_Count,

	AircraftClassifications* pAcClass =  pAircraftManager->GetACClassification(WakeVortexWightBased);
	if(pAcClass == NULL)
		return;



	int nAcCount = pAcClass->GetCount();
	for (int nACLead = 0; nACLead < nAcCount ; ++nACLead)
	{
		AircraftClassificationItem *pItemLead = pAcClass->GetItem(nACLead);
		if(pItemLead == NULL)
			continue;
		WakeVortexDetailValue wakeVortexDetail;
		wakeVortexDetail.m_nClassLead = pItemLead->getID();
		wakeVortexDetail.m_strClassLeadName = pItemLead->getName();

		for (int nACTrail = 0; nACTrail < nAcCount; ++ nACTrail)
		{
			AircraftClassificationItem *pItemTrail = pAcClass->GetItem(nACTrail);
			if(pItemTrail == NULL)
				continue;
			
			wakeVortexDetail.m_nClassTrail = pItemTrail->getID();
			wakeVortexDetail.m_strClassTrailName = pItemTrail->getName();
	
			m_vWakeVortexDetailValue.push_back(wakeVortexDetail);
		}
	}
}

void CAirsideRunwayOperationReportDetailResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	if(!IsLoadFromFile())
	{
		GetListColumns(m_lstColumns);
	}
	if (piSHC)
		piSHC->ResetAll();

	for (int nCol = 0 ; nCol < (int)m_lstColumns.size(); ++nCol)
	{
		cxListCtrl.InsertColumn(nCol,m_lstColumns[nCol],LVCFMT_LEFT, 100);
		if (piSHC)
		{
			if (nCol == 1)
				piSHC->SetDataType(nCol,dtTIME);
			else
				piSHC->SetDataType(nCol,dtSTRING);
		}
	}
}

void CAirsideRunwayOperationReportDetailResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	//add data to list ctrl
	int nFlightOperationCount = (int)m_vFlightOperateValue.size();
	for (int nOperation = 0; nOperation < nFlightOperationCount; ++nOperation)
	{
		FlightOperationData& operationData = m_vFlightOperateValue[nOperation];
		int nIndex = cxListCtrl.InsertItem(nOperation, operationData.m_sFlightID);

		//time
		ElapsedTime estTime;
		estTime.setPrecisely(operationData.m_lTime);
		CString strTime(_T(""));
		strTime.Format(_T("Day%d %02d:%02d:%02d"), estTime.GetDay(), estTime.GetHour(), estTime.GetMinute(), estTime.GetSecond());
		cxListCtrl.SetItemText(nIndex, 1,strTime);

		//runway mark
		cxListCtrl.SetItemText(nIndex,2,operationData.m_sRunway);

		//L/T/M
		cxListCtrl.SetItemText(nIndex,3,operationData.m_sOperation);

		//lead-trail category
		cxListCtrl.SetItemText(nIndex,4,operationData.m_sLeadTrail);
	}
// 	int nRunwayCount = m_vRunwayOperationDetail.size();
// 	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
// 	{
// 
// 		RunwayOperationDetail *pRunwayOperation = m_vRunwayOperationDetail.at(nRunway);
// 		if(pRunwayOperation == NULL)
// 			continue;
// 
// 		int nIndex = cxListCtrl.InsertItem(nRunway, pRunwayOperation->m_runWaymark.m_strMarkName);
// 
// 		CString strLandingCount;
// 		strLandingCount.Format(_T("%d"),pRunwayOperation->m_nTotalLandingCount);
// 		cxListCtrl.SetItemText(nIndex,1,strLandingCount);
// 
// 		//landing classification
// 		int nLandTextCount = 0;
// 		{
// 			std::vector<CString> vLandingText;
// 			int nLandingClassCount= pRunwayOperation->m_vLandingClassValue.size();
// 			for (int nLand = 0; nLand < nLandingClassCount; ++ nLand)
// 			{
// 				int nClassSubTypeValueCount = pRunwayOperation->m_vLandingClassValue.at(nLand).m_vSubTypeValue.size();
// 
// 				for (int nClassSubType = 0; nClassSubType < nClassSubTypeValueCount; ++nClassSubType)
// 				{
// 					CString strText;
// 					strText.Format(_T("%d"),pRunwayOperation->m_vLandingClassValue.at(nLand).m_vSubTypeValue[nClassSubType].m_nCount);
// 					vLandingText.push_back(strText);
// 				}
// 			}
// 			nLandTextCount = vLandingText.size();
// 			for (int nLandText = 0; nLandText < nLandTextCount; ++ nLandText)
// 			{
// 				cxListCtrl.SetItemText(nIndex,2 + nLandText,vLandingText[nLandText]);
// 			}
// 		}
// 		//take off
// 		CString strTakeOffCount;
// 		strTakeOffCount.Format(_T("%d"),pRunwayOperation->m_nTotalTakeOffCount);
// 		cxListCtrl.SetItemText(nIndex,2 + nLandTextCount,strTakeOffCount);
// 
// 		//take off classification
// 
// 		int nTakeOffTextCount = 0;
// 		{
// 			std::vector<CString> vTakeOffText;
// 			int nLandingClassCount= pRunwayOperation->m_vTakeOffClassValue.size();
// 			for (int nLand = 0; nLand < nLandingClassCount; ++ nLand)
// 			{
// 				int nClassSubTypeValueCount = pRunwayOperation->m_vTakeOffClassValue.at(nLand).m_vSubTypeValue.size();
// 
// 				for (int nClassSubType = 0; nClassSubType < nClassSubTypeValueCount; ++nClassSubType)
// 				{
// 					CString strText;
// 					strText.Format(_T("%d"),pRunwayOperation->m_vTakeOffClassValue.at(nLand).m_vSubTypeValue[nClassSubType].m_nCount);
// 					vTakeOffText.push_back(strText);
// 				}
// 			}
// 			nTakeOffTextCount = vTakeOffText.size();
// 			for (int nTakeOffText = 0; nTakeOffText < nLandTextCount; ++ nTakeOffText)
// 			{
// 				cxListCtrl.SetItemText(nIndex,2 + nLandTextCount + 1 + nTakeOffText,vTakeOffText[nTakeOffText]);
// 			}
// 		}
// 		
// 		//Miss approach list index
// 		int nMissApproachColIndex = 2 + nLandTextCount + 1 + nTakeOffTextCount;
// 		CString strMissApproach;
// 		strMissApproach.Format(_T("%d"),pRunwayOperation->m_nMissApproachCount);
// 		cxListCtrl.SetItemText(nIndex,nMissApproachColIndex,strMissApproach);
// 
// 		
// 		//wake vortex
// 		int nWakeCount = pRunwayOperation->m_vWakeVortexDetailValue.size();
// 		
// 		for (int nWake = 0; nWake < nWakeCount; ++ nWake)
// 		{
// 			CString strText;
// 			strText.Format(_T("%d"),pRunwayOperation->m_vWakeVortexDetailValue[nWake].m_nFlightCount);
// 			cxListCtrl.SetItemText(nIndex,nMissApproachColIndex + 1 + nWake,strText);	
// 		}
// 	}
}
//ChartType_Detail = 0,
//ChartType_Detail_LandingsByRunway,
//ChartType_Detail_TakeOffByRunway,
//ChartType_Detail_LeadTrailByRunway,
//ChartType_Detail_MovementsPerRunway,
//
//
//ChartType_Detail_Count,
//
//
//ChartType_Summary = 100,
//ChartType_Summary_RunwayOperationalStatistic,
//
//ChartType_Summary_Count,
void CAirsideRunwayOperationReportDetailResult::RefreshReport( CParameters * parameter )
{
	AirsideRunwayOperationReportParam *pParam = (AirsideRunwayOperationReportParam *)parameter;
	if(pParam  == NULL)
		return;

	if(m_pChartResult != NULL)
		delete m_pChartResult;
	m_pChartResult = NULL;
	if(pParam->getSubType() == -1 )
		pParam->setSubType(AirsideRunwayOperationsReport::ChartType_Detail_LandingsByRunway);

	if(pParam->getSubType() == AirsideRunwayOperationsReport::ChartType_Detail_LandingsByRunway)
	{
		m_pChartResult = new AirsideRunwayOperationReportDetail::LandingTakeOffChartResult(true);
		AirsideRunwayOperationReportDetail::LandingTakeOffChartResult *pChartResult = 
			(AirsideRunwayOperationReportDetail::LandingTakeOffChartResult *)m_pChartResult;
		
		pChartResult->GenerateResult(m_vFlightOperateValue);
	    
	}
	else if(pParam->getSubType() == AirsideRunwayOperationsReport::ChartType_Detail_TakeOffByRunway)
	{
		m_pChartResult = new AirsideRunwayOperationReportDetail::LandingTakeOffChartResult(false);
		AirsideRunwayOperationReportDetail::LandingTakeOffChartResult *pChartResult = 
			(AirsideRunwayOperationReportDetail::LandingTakeOffChartResult *)m_pChartResult;

		pChartResult->GenerateResult(m_vFlightOperateValue);

	}
	else if(pParam->getSubType() == AirsideRunwayOperationsReport::ChartType_Detail_LeadTrailByRunway)
	{
		m_pChartResult = new AirsideRunwayOperationReportDetail::LeadTrailChartResult;
		AirsideRunwayOperationReportDetail::LeadTrailChartResult *pChartResult = 
			(AirsideRunwayOperationReportDetail::LeadTrailChartResult *)m_pChartResult;

		pChartResult->GenerateResult(&m_vRunwayOperationDetail);
	}
	else if(pParam->getSubType() == AirsideRunwayOperationsReport::ChartType_Detail_MovementsPerRunway)
	{
		m_pChartResult = new AirsideRunwayOperationReportDetail::MovementsPerIntervalChartResult;
		AirsideRunwayOperationReportDetail::MovementsPerIntervalChartResult *pChartResult = 
			(AirsideRunwayOperationReportDetail::MovementsPerIntervalChartResult *)m_pChartResult;

		pChartResult->GenerateResult(&m_vRunwayOperationDetail);
	}
	else
	{

		ASSERT(0);
		//should not get here
	}

}

void CAirsideRunwayOperationReportDetailResult::InitRunway( CParameters * parameter )
{
	AirsideRunwayOperationReportParam* pParam = (AirsideRunwayOperationReportParam*)parameter;
	if(pParam == NULL)
		return;

	int nRunwayCount = pParam->m_vRunway.size();
	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		RunwayOperationDetail *pRunwayDetail = new RunwayOperationDetail;
		pRunwayDetail->m_runWaymark = pParam->m_vRunway[nRunway];
		pRunwayDetail->InitRunwatTimeOperation(pParam->getStartTime(),pParam->getEndTime(),ElapsedTime(pParam->getInterval()));
		pRunwayDetail->InitClassValue(&m_airCraftClassicfication,pParam);
		pRunwayDetail->InitWakeVortex(&m_airCraftClassicfication);
		//pRunwayDetail

		m_vRunwayOperationDetail.push_back(pRunwayDetail);
	}


}

BOOL CAirsideRunwayOperationReportDetailResult::WriteReportData( ArctermFile& _file )
{
	//title
	_file.writeField("Runway Operation Detail Report");
	_file.writeLine();
	//columns
	CString strColumns= _T("RunwayID, Mark, Name, total landing count, classificatin count");
	_file.writeField(_T("Columns"));
	_file.writeLine();

	_file.writeInt((int)ASReportType_Detail);
	_file.writeLine();

	std::vector<CString> vColumns = m_lstColumns;
	if(!IsLoadFromFile())
		GetListColumns(vColumns);
	_file.writeInt((int)vColumns.size());
	for (int nCol = 0; nCol < (int)vColumns.size(); ++nCol)
	{
		_file.writeField(vColumns[nCol]);
	}
	_file.writeLine();

	_file.writeInt((int)m_vFlightOperateValue.size());
	for (int nOperation = 0; nOperation < (int)m_vFlightOperateValue.size(); ++nOperation)
	{
		FlightOperationData& operationData = m_vFlightOperateValue[nOperation];
		_file.writeField(operationData.m_sFlightID);
		_file.writeInt(operationData.m_lTime);
		_file.writeField(operationData.m_sRunway);
		_file.writeField(operationData.m_sOperation);
		_file.writeField(operationData.m_sLeadTrail);
		_file.writeLine();
	}

	int nRunwwayCount = m_vRunwayOperationDetail.size();
	_file.writeField(_T("Runway Count:"));
	_file.writeInt(nRunwwayCount);
	_file.writeLine();

	for (int nRunway = 0; nRunway < nRunwwayCount; ++ nRunway)
	{
		RunwayOperationDetail* pRunwayDetail = m_vRunwayOperationDetail[nRunway];

		if(pRunwayDetail == NULL)
			return FALSE;
		//runway information
		_file.writeInt(pRunwayDetail->m_runWaymark.m_nRunwayID);
		_file.writeInt((int)pRunwayDetail->m_runWaymark.m_enumRunwayMark);
		_file.writeField(pRunwayDetail->m_runWaymark.m_strMarkName);
		_file.writeLine();//----------------

		//landing count
		_file.writeInt(pRunwayDetail->m_nTotalLandingCount);
		
		//landing type data
		 int nClassCount = pRunwayDetail->m_vLandingClassValue.size();
		_file.writeInt(nClassCount);
		for (int nClass = 0; nClass < nClassCount; ++nClass )
		{
			RunwayClassValue& runwayClassValue =pRunwayDetail->m_vLandingClassValue[nClass];
			_file.writeInt((int)runwayClassValue.m_classType);
			int nSubTypeCount = runwayClassValue.m_vSubTypeValue.size();
			_file.writeInt(nSubTypeCount);
			for (int nSubType = 0; nSubType < nSubTypeCount; ++nSubType)
			{
				_file.writeInt(runwayClassValue.m_vSubTypeValue[nSubType].m_nTypeID);
				_file.writeField(runwayClassValue.m_vSubTypeValue[nSubType].m_strTypeName);
				_file.writeInt(runwayClassValue.m_vSubTypeValue[nSubType].m_nCount);
			}
		}
		_file.writeLine();//  [5/5/2009 user]



		//take off count
		_file.writeInt(pRunwayDetail->m_nTotalTakeOffCount);

		//landing type data
		 nClassCount = pRunwayDetail->m_vTakeOffClassValue.size();
		_file.writeInt(nClassCount);
		for (int nClass = 0; nClass < nClassCount; ++nClass )
		{
			RunwayClassValue& runwayClassValue =pRunwayDetail->m_vTakeOffClassValue[nClass];
			_file.writeInt((int)runwayClassValue.m_classType);
			int nSubTypeCount = runwayClassValue.m_vSubTypeValue.size();
			_file.writeInt(nSubTypeCount);
			for (int nSubType = 0; nSubType < nSubTypeCount; ++nSubType)
			{
				_file.writeInt(runwayClassValue.m_vSubTypeValue[nSubType].m_nTypeID);
				_file.writeField(runwayClassValue.m_vSubTypeValue[nSubType].m_strTypeName);
				_file.writeInt(runwayClassValue.m_vSubTypeValue[nSubType].m_nCount);
			}
		}
		_file.writeLine();//  [5/5/2009 user]


		//miss approach 
		_file.writeInt(pRunwayDetail->m_nMissApproachCount);

		_file.writeLine();//  [5/5/2009 user]


		//lead - trail
		int nLeadTrailCount = pRunwayDetail->m_vWakeVortexDetailValue.size();
		_file.writeInt(nLeadTrailCount);
		for (int nLeadtrail = 0; nLeadtrail < nLeadTrailCount; ++ nLeadtrail)
		{
			WakeVortexDetailValue& wakeVortex = pRunwayDetail->m_vWakeVortexDetailValue[nLeadtrail];
			_file.writeInt(wakeVortex.m_nClassLead);
			_file.writeField(wakeVortex.m_strClassLeadName);
			_file.writeInt(wakeVortex.m_nClassTrail);
			_file.writeField(wakeVortex.m_strClassTrailName);
			_file.writeInt(wakeVortex.m_nFlightCount);
		}

		_file.writeLine();//  [5/5/2009 user]

		//time value

		int nTimeValueCount = pRunwayDetail->m_vRunwayTimeValue.size();
		_file.writeInt(nTimeValueCount);
		for (int nTime = 0; nTime < nTimeValueCount; ++ nTime)
		{
			RunwayOperationTimeValue& timeValue = pRunwayDetail->m_vRunwayTimeValue[nTime];

			_file.writeInt(timeValue.m_eTime.GetSecond());
			_file.writeInt(timeValue.m_nLandingCount);
			_file.writeInt(timeValue.m_nTakeOffCount);
		}

		_file.writeLine();//  [5/5/2009 user]
	}

	return TRUE;
}

BOOL CAirsideRunwayOperationReportDetailResult::ReadReportData( ArctermFile& _file )
{

	_file.getLine();
	//columns
	int nColCount = 0;
	_file.getInteger(nColCount);
	for (int nCol = 0; nCol < nColCount; ++nCol)
	{
		CString strCol;
		_file.getField(strCol.GetBuffer(1024),1024);
		strCol.ReleaseBuffer();
		m_lstColumns.push_back(strCol);

	}
	_file.getLine();

	int nOperationCount = 0;
	_file.getInteger(nOperationCount);
	for (int nOperation = 0; nOperation < nOperationCount; ++nOperation)
	{
		FlightOperationData operationData;
		_file.getField(operationData.m_sFlightID.GetBuffer(1024),1024);
		operationData.m_sFlightID.ReleaseBuffer();
		_file.getInteger(operationData.m_lTime);
		_file.getField(operationData.m_sRunway.GetBuffer(1024),1024);
		operationData.m_sRunway.ReleaseBuffer();
		_file.getField(operationData.m_sOperation.GetBuffer(1024),1024);
		operationData.m_sOperation.ReleaseBuffer();
		_file.getField(operationData.m_sLeadTrail.GetBuffer(1024),1024);
		operationData.m_sLeadTrail.ReleaseBuffer();
		_file.getLine();
		m_vFlightOperateValue.push_back(operationData);
	}

	int nRunwwayCount = 0;
	_file.skipField(1);//skip runway count
	_file.getInteger(nRunwwayCount);
	_file.getLine();

	for (int nRunway = 0; nRunway < nRunwwayCount; ++ nRunway)
	{
		RunwayOperationDetail* pRunwayDetail = new RunwayOperationDetail;
		
		CAirsideReportRunwayMark runWaymark;

		//runway information
		_file.getInteger(runWaymark.m_nRunwayID);
		
		_file.getInteger((int&)runWaymark.m_enumRunwayMark);
		_file.getField(runWaymark.m_strMarkName.GetBuffer(1024),1024);
		runWaymark.m_strMarkName.ReleaseBuffer();
		pRunwayDetail->m_runWaymark = runWaymark;
		_file.getLine();//----------------

		//landing count
		_file.getInteger(pRunwayDetail->m_nTotalLandingCount);

		//landing type data
		int nClassCount = 0;
		_file.getInteger(nClassCount);
		for (int nClass = 0; nClass < nClassCount; ++nClass )
		{
			RunwayClassValue runwayClassValue;
			_file.getInteger((int&)runwayClassValue.m_classType);
			int nSubTypeCount = 0;
			_file.getInteger(nSubTypeCount);
			for (int nSubType = 0; nSubType < nSubTypeCount; ++nSubType)
			{
				RunwayClassSubTypeValue subTypeValue;
				_file.getInteger(subTypeValue.m_nTypeID);
				_file.getField(subTypeValue.m_strTypeName.GetBuffer(1024),1024);
				subTypeValue.m_strTypeName.ReleaseBuffer();
				_file.getInteger(subTypeValue.m_nCount);

				runwayClassValue.m_vSubTypeValue.push_back(subTypeValue);
			}
			pRunwayDetail->m_vLandingClassValue.push_back(runwayClassValue);
			
		}
		_file.getLine();//  [5/5/2009 user]



		//take off count
		_file.getInteger(pRunwayDetail->m_nTotalTakeOffCount);

		//landing type data
		_file.getInteger(nClassCount);
		for (int nClass = 0; nClass < nClassCount; ++nClass )
		{
			RunwayClassValue runwayClassValue;
			_file.getInteger((int&)runwayClassValue.m_classType);
			int nSubTypeCount = 0;
			_file.getInteger(nSubTypeCount);
			for (int nSubType = 0; nSubType < nSubTypeCount; ++nSubType)
			{
				RunwayClassSubTypeValue subTypeValue;
				_file.getInteger(subTypeValue.m_nTypeID);
				_file.getField(subTypeValue.m_strTypeName.GetBuffer(1024),1024);
				subTypeValue.m_strTypeName.ReleaseBuffer();
				_file.getInteger(subTypeValue.m_nCount);

				runwayClassValue.m_vSubTypeValue.push_back(subTypeValue);
			}
			pRunwayDetail->m_vTakeOffClassValue.push_back(runwayClassValue);

		}
		_file.getLine();//  [5/5/2009 user]


		//miss approach 
		_file.getInteger(pRunwayDetail->m_nMissApproachCount);

		_file.getLine();//  [5/5/2009 user]


		//lead - trail
		int nLeadTrailCount = 0;
		_file.getInteger(nLeadTrailCount);
		for (int nLeadtrail = 0; nLeadtrail < nLeadTrailCount; ++ nLeadtrail)
		{
			WakeVortexDetailValue wakeVortex;
			_file.getInteger(wakeVortex.m_nClassLead);
			_file.getField(wakeVortex.m_strClassLeadName.GetBuffer(1024),1024);
			_file.getInteger(wakeVortex.m_nClassTrail);
			_file.getField(wakeVortex.m_strClassTrailName.GetBuffer(1024),1024);
			_file.getInteger(wakeVortex.m_nFlightCount);

			pRunwayDetail->m_vWakeVortexDetailValue.push_back(wakeVortex);
		}

		_file.getLine();//  [5/5/2009 user]

		//time value
		int nTimeValueCount = 0;
		_file.getInteger(nTimeValueCount);
		for (int nTime = 0; nTime < nTimeValueCount; ++ nTime)
		{
			RunwayOperationTimeValue timeValue;
			int nTimeValue = 0;
			_file.getInteger(nTimeValue);
			timeValue.m_eTime.set(nTimeValue);
			_file.getInteger(timeValue.m_nLandingCount);
			_file.getInteger(timeValue.m_nTakeOffCount);

			pRunwayDetail->m_vRunwayTimeValue.push_back(timeValue);
		}
		m_vRunwayOperationDetail.push_back(pRunwayDetail);

		_file.getLine();//  [5/5/2009 user]
	}

	return TRUE;
}

void CAirsideRunwayOperationReportDetailResult::GetListColumns( std::vector<CString>& lstColumns )
{
	lstColumns.clear();

	if(m_pParameter == NULL)
		return;

// 	std::vector<FlightClassificationBasisType>::iterator iter = m_pParameter->m_vTypes.begin();
// 	std::vector<CString> vTypesName;
// 	for (;iter != m_pParameter->m_vTypes.end(); ++ iter)
// 	{
// 		AircraftClassifications* pAcClass =  m_airCraftClassicfication.GetACClassification(*iter);
// 		if(pAcClass == NULL)
// 			continue;
// 
// 		RunwayClassValue runwayClassValue;
// 		runwayClassValue.m_classType = *iter;
// 
// 		int nAcCount = pAcClass->GetCount();
// 		for (int nAC = 0; nAC < nAcCount ; ++nAC)
// 		{
// 			AircraftClassificationItem *pItem = pAcClass->GetItem(nAC);
// 			if(pItem == NULL)
// 				continue;
// 
// 			vTypesName.push_back(pItem->getName());
// 		}
// 	}
// 
// 
// 	lstColumns.push_back(_T("Runway"));
// 	lstColumns.push_back(_T("Landing(total)"));
// 
// 	//landing classification
// 	int nTypesNameCount = vTypesName.size();
// 	for (int nType =0; nType < nTypesNameCount; ++ nType)
// 	{
// 		CString strColName;
// 		strColName.Format(_T("Landing(%s)"),vTypesName[nType]);
// 
// 		lstColumns.push_back(strColName);
// 	}
// 
// 	//take off
// 	lstColumns.push_back(_T("takeoff(total)"));
// 	for (int nType =0; nType < nTypesNameCount; ++ nType)
// 	{
// 		CString strColName;
// 		strColName.Format(_T("takeoff(%s)"),vTypesName[nType]);
// 		lstColumns.push_back(strColName);
// 	}
// 
// 	//miss approach
// 	lstColumns.push_back(_T("Miss Approach"));
// 	//wake vortex
// 
// 
// 
// 	AircraftClassifications* pAcClass =  m_airCraftClassicfication.GetACClassification(WakeVortexWightBased);
// 	if(pAcClass == NULL)
// 		return;
// 
// 	int nAcCount = pAcClass->GetCount();
// 	std::vector< CString> vWakeVortex;
// 	for (int nACLead = 0; nACLead < nAcCount ; ++nACLead)
// 	{
// 		AircraftClassificationItem *pItemLead = pAcClass->GetItem(nACLead);
// 		if(pItemLead == NULL)
// 			continue;
// 
// 		for (int nACTrail = 0; nACTrail < nAcCount; ++ nACTrail)
// 		{
// 			AircraftClassificationItem *pItemTrail = pAcClass->GetItem(nACTrail);
// 			if(pItemTrail == NULL)
// 				continue;
// 
// 			CString strWakeVortex;
// 			strWakeVortex.Format(_T("%s-%s"),pItemLead->getName(),pItemTrail->getName());
// 			vWakeVortex.push_back(strWakeVortex);
// 		}
// 	}
// 	int nWakeVortexCount = vWakeVortex.size();
// 	for (int nWake = 0; nWake < nWakeVortexCount; ++ nWake)
// 	{
// 		lstColumns.push_back( vWakeVortex[nWake]);
// 	}
	lstColumns.push_back(CString(_T("Flight")));
	lstColumns.push_back(CString(_T("Time")));
	lstColumns.push_back(CString(_T("Runway")));
	lstColumns.push_back(CString(_T("L/T/M")));
	lstColumns.push_back(CString(_T("Lead-Trail Category")));
}
	

AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::LandingTakeOffChartResult( bool bLanding )
:m_bLanding(bLanding)
{
	m_vRunwayOperationDetail = NULL;
}

AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::~LandingTakeOffChartResult()
{

}

void AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::GenerateResult( std::vector<CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail *>* vRunwayOperationDetail )
{
	m_vRunwayOperationDetail = vRunwayOperationDetail;
}

void AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::GenerateResult( std::vector<CAirsideRunwayOperationReportDetailResult::FlightOperationData> vOperationData )
{
	m_OperationData.InitResultData(vOperationData);
}

void AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if (pParameter == NULL)
		return;
	
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	if(m_bLanding)
		c2dGraphData.m_strChartTitle = _T(" Landings by Runway ");
	else
		c2dGraphData.m_strChartTitle = _T(" Take Off by Runway ");

	c2dGraphData.m_strYtitle = _T("Number of Operations");
	c2dGraphData.m_strXtitle = _T("Time");

	std::vector<CString> vRunway;
	m_OperationData.GetRunway(m_bLanding,vRunway);

	//x tick, runway
	int nRunwayCount = (int)vRunway.size();
	for (int nRunway = 0; nRunway < nRunwayCount; ++nRunway)
	{
		c2dGraphData.m_vrXTickTitle.push_back(vRunway[nRunway]);
	}

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());


	ASSERT(pParameter);

	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	CString timeInterval ;
	ElapsedTime startTime = pParameter->getStartTime();
	ElapsedTime endtime;
	std::vector<CString> vTimeRange;
	for (int i = 0 ; i < intervalSize ;i++)
	{
		endtime = startTime + ElapsedTime(pParameter->getInterval()) ;
		timeInterval.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		vTimeRange.push_back(timeInterval) ;
	}

	c2dGraphData.m_vrXTickTitle = vTimeRange;

	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	for (int i = 0; i < nRunwayCount; i++)
	{
		std::vector<double> vData;
		for (long j=0; j<intervalSize; j++)
		{
			int nCount = 0;
			ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
			ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(j + 1));
			nCount = m_OperationData.GetOperationCount(m_bLanding,vRunway[i],estTempMinDelayTime,estTempMaxDelayTime);
			vData.push_back(nCount);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
		c2dGraphData.m_vrLegend.push_back(vRunway[i]);
	}

	chartWnd.DrawChart(c2dGraphData);
}

// void AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
// {
// 	if(m_vRunwayOperationDetail == NULL || pParameter == NULL)
// 		return;
// 
// 
// 
// 	CString strLabel = _T("");
// 	C2DChartData c2dGraphData;
// 	if(m_bLanding)
// 		c2dGraphData.m_strChartTitle = _T(" Landings by Runway ");
// 	else
// 		c2dGraphData.m_strChartTitle = _T(" Take Off by Runway ");
// 
// 	c2dGraphData.m_strYtitle = _T("Number of Operations");
// 	c2dGraphData.m_strXtitle = _T("Runway");
// 
// 	int nRunwayCount = m_vRunwayOperationDetail->size();
// 	if(nRunwayCount == 0)
// 		return;
// 
// 
// 	//x tick, runway
// 	for (int nRunway = 0; nRunway < nRunwayCount; ++nRunway)
// 	{
// 		c2dGraphData.m_vrXTickTitle.push_back(m_vRunwayOperationDetail->at(nRunway)->m_runWaymark.m_strMarkName);
// 	}
// 	
// 	//c2dGraphData.m_vrXTickTitle = m_vTimeRange;
// 
// 
// 	//set footer
// 	CString strFooter(_T(""));
// 	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
// 		pParameter->getStartTime().printTime(), 
// 		pParameter->getEndTime().printTime());
// 
// 
// 	bool bCommaFlag = true;
// 	AirsideRunwayOperationReportParam* pParam = (AirsideRunwayOperationReportParam*)pParameter;
// 	for (int i=0; i<(int)pParam->getFlightConstraintCount(); i++)
// 	{
// 		FlightConstraint fltCons = pParam->getFlightConstraint(i);
// 
// 		CString strFlight(_T(""));
// 		fltCons.screenPrint(strFlight.GetBuffer(1024));
// 
// 		if (bCommaFlag)
// 		{
// 			bCommaFlag = false;
// 			strFooter.AppendFormat("%s", strFlight);
// 		}
// 		else
// 		{
// 			strFooter.AppendFormat(",%s", strFlight);
// 		}
// 	}
// 	c2dGraphData.m_strFooter = strFooter;
// 
// 	
// 	//if the landing report 
// 	if(m_bLanding)
// 	{
// 		//check the runway has classification or not
// 		if(pParam->m_vTypes.size())//has classification
// 		{
// 			std::vector<CString> vLegend;
// 			//get legend
// 			int nClassCount = m_vRunwayOperationDetail->at(0)->m_vLandingClassValue.size();
// 			for (int nClass = 0; nClass < nClassCount; ++ nClass)
// 			{
// 				for (int nSubType = 0; nSubType < static_cast<int>(m_vRunwayOperationDetail->at(0)->m_vLandingClassValue[0].m_vSubTypeValue.size()); ++ nSubType)
// 				{
// 					vLegend.push_back(m_vRunwayOperationDetail->at(0)->m_vLandingClassValue[0].m_vSubTypeValue.at(nSubType).m_strTypeName);
// 				}
// 			}
// 			
// 					
// 	
// 			std::vector< std::vector<double> > vLegendData;
// 			vLegendData.resize(vLegend.size());
// 
// 		
// 			for (int nRunwayDetail = 0; nRunwayDetail <nRunwayCount; ++ nRunwayDetail)
// 			{	
// 				//vector<double> vSegmentData;	
// 				int nLegend = 0;
// 				int nClassTypeCount = m_vRunwayOperationDetail->at(nRunwayDetail)->m_vLandingClassValue.size();
// 				for (int nClassType = 0; nClassType < nClassTypeCount; ++ nClassType )
// 				{
// 					int nSubClassTypeCount = m_vRunwayOperationDetail->at(nRunwayDetail)->m_vLandingClassValue.at(nClassType).m_vSubTypeValue.size();
// 					for (int nSubClassType = 0; nSubClassType < nSubClassTypeCount; ++ nSubClassType)
// 					{
// 						vLegendData[nLegend].push_back(m_vRunwayOperationDetail->at(nRunwayDetail)->m_vLandingClassValue.at(nClassType).m_vSubTypeValue.at(nSubClassType).m_nCount);
// 						//vSegmentData.push_back(m_vRunwayOperationDetail->at(nRunwayDetail)->m_vLandingClassValue.at(nClassType).m_vSubTypeValue.at(nSubClassType).m_nCount);
// 						nLegend += 1;
// 					}
// 
// 				}
// 
// 				//c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
// 			}
// 
// 			int nLegendCount = vLegendData.size();
// 			for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
// 			{
// 				c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
// 			}
// 
// 			c2dGraphData.m_vrLegend = vLegend;
// 				
// 		}
// 		else //not select, show total count
// 		{
// 			std::vector<CString> vLegend;
// 			vLegend.push_back(_T("Landing"));
// 			
// 			vector<double> vSegmentData;
// 			for (int nRunwayDetail = 0; nRunwayDetail <nRunwayCount; ++ nRunwayDetail)
// 			{	
// 				vSegmentData.push_back(m_vRunwayOperationDetail->at(nRunwayDetail)->m_nTotalLandingCount);
// 			}	
// 
// 			c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
// 			c2dGraphData.m_vrLegend = vLegend;
// 		}
// 	}
// 	else
// 	{
// 		//check the runway has classification or not
// 		if(pParam->m_vTypes.size())//has classification
// 		{
// 			std::vector<CString> vLegend;
// 			//get legend
// 			int nClassCount = m_vRunwayOperationDetail->at(0)->m_vTakeOffClassValue.size();
// 			for (int nClass = 0; nClass < nClassCount; ++ nClass)
// 			{
// 				for (int nSubType = 0; nSubType < static_cast<int>(m_vRunwayOperationDetail->at(0)->m_vTakeOffClassValue[0].m_vSubTypeValue.size()); ++ nSubType)
// 				{
// 					vLegend.push_back(m_vRunwayOperationDetail->at(0)->m_vTakeOffClassValue[0].m_vSubTypeValue.at(nSubType).m_strTypeName);
// 				}
// 			}
// 
// 			
// 			std::vector< std::vector<double> > vLegendData;
// 			vLegendData.resize(vLegend.size());
// 			
// 			for (int nRunwayDetail = 0; nRunwayDetail <nRunwayCount; ++ nRunwayDetail)
// 			{	
// 				//vector<double> vSegmentData;	
// 				int nLegend = 0;
// 				int nClassTypeCount = m_vRunwayOperationDetail->at(nRunwayDetail)->m_vTakeOffClassValue.size();
// 				for (int nClassType = 0; nClassType < nClassTypeCount; ++ nClassType )
// 				{
// 					int nSubClassTypeCount = m_vRunwayOperationDetail->at(nRunwayDetail)->m_vTakeOffClassValue.at(nClassType).m_vSubTypeValue.size();
// 					for (int nSubClassType = 0; nSubClassType < nSubClassTypeCount; ++ nSubClassType)
// 					{
// 						vLegendData[nLegend].push_back(m_vRunwayOperationDetail->at(nRunwayDetail)->m_vTakeOffClassValue.at(nClassType).m_vSubTypeValue.at(nSubClassType).m_nCount);
// 						nLegend += 1;					
// 						//vSegmentData.push_back();
// 					}
// 				}
// 			}
// 			int nLegendCount = vLegendData.size();
// 			for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
// 			{
// 				c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
// 			}
// 
// 			c2dGraphData.m_vrLegend = vLegend;
// 
// 
// 		}
// 		else //not select, show total count
// 		{
// 			std::vector<CString> vLegend;
// 			vLegend.push_back(_T("Take Off"));
// 
// 			vector<double> vSegmentData;	
// 			for (int nRunwayDetail = 0; nRunwayDetail <nRunwayCount; ++ nRunwayDetail)
// 			{	
// 
// 				vSegmentData.push_back(m_vRunwayOperationDetail->at(nRunwayDetail)->m_nTotalTakeOffCount);
// 			}	
// 			c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
// 
// 			c2dGraphData.m_vrLegend = vLegend;
// 		}
// 	}
// 
// 
// 
// 	chartWnd.DrawChart(c2dGraphData);
// 	
// }









AirsideRunwayOperationReportDetail::LeadTrailChartResult::LeadTrailChartResult()
{
	m_vRunwayOperationDetail = NULL;
}

AirsideRunwayOperationReportDetail::LeadTrailChartResult::~LeadTrailChartResult()
{

}

void AirsideRunwayOperationReportDetail::LeadTrailChartResult::GenerateResult( std::vector<CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail *> * vRunwayOperationDetail )
{
	m_vRunwayOperationDetail = vRunwayOperationDetail;
}

void AirsideRunwayOperationReportDetail::LeadTrailChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(m_vRunwayOperationDetail == NULL || pParameter == NULL)
		return;
	


	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Land Trail Operations by Runway ");


	c2dGraphData.m_strYtitle = _T("Number of Operations");
	c2dGraphData.m_strXtitle = _T("Runway");

	int nRunwayCount = m_vRunwayOperationDetail->size();
	if(nRunwayCount == 0)
		return;


	//x tick, runway
	for (int nRunway = 0; nRunway < nRunwayCount; ++nRunway)
	{
		c2dGraphData.m_vrXTickTitle.push_back(m_vRunwayOperationDetail->at(nRunway)->m_runWaymark.m_strMarkName);
	}

	//c2dGraphData.m_vrXTickTitle = m_vTimeRange;


	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());


	bool bCommaFlag = true;
	AirsideRunwayOperationReportParam* pParam = (AirsideRunwayOperationReportParam*)pParameter;
	for (int i=0; i<(int)pParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pParam->getFlightConstraint(i);

		CString strFlight(_T(""));
		fltCons.screenPrint(strFlight.GetBuffer(1024));

		if (bCommaFlag)
		{
			bCommaFlag = false;
			strFooter.AppendFormat("%s", strFlight);
		}
		else
		{
			strFooter.AppendFormat(",%s", strFlight);
		}
	}
	c2dGraphData.m_strFooter = strFooter;
	

	std::vector<CString> vLegends;
	//get the legend
	int nLegendCount = m_vRunwayOperationDetail->at(0)->m_vWakeVortexDetailValue.size();
	for (int nLegend = 0; nLegend < nLegendCount; ++nLegend)
	{
		CString strLegend;
		strLegend.Format(_T("%s-%s"),
		m_vRunwayOperationDetail->at(0)->m_vWakeVortexDetailValue[nLegend].m_strClassLeadName,
		m_vRunwayOperationDetail->at(0)->m_vWakeVortexDetailValue[nLegend].m_strClassTrailName);
		vLegends.push_back(strLegend);
		
	}
	
	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());	

	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		
		int nCount = m_vRunwayOperationDetail->at(nRunway)->m_vWakeVortexDetailValue.size();
		
		for (int i = 0; i < nCount; ++ i)
		{
			vLegendData[i].push_back(m_vRunwayOperationDetail->at(nRunway)->m_vWakeVortexDetailValue[i].m_nFlightCount);
		}

	}

	nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}
















//////////////////////////////////////////////////////////////////////////

//AirsideRunwayOperationReportDetail
AirsideRunwayOperationReportDetail::MovementsPerIntervalChartResult::MovementsPerIntervalChartResult()
{
	m_vRunwayOperationDetail = NULL;
}

AirsideRunwayOperationReportDetail::MovementsPerIntervalChartResult::~MovementsPerIntervalChartResult()
{

}




void AirsideRunwayOperationReportDetail::MovementsPerIntervalChartResult::GenerateResult( std::vector<CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail *> * vRunwayOperationDetail )
{
	m_vRunwayOperationDetail = vRunwayOperationDetail;
}

void AirsideRunwayOperationReportDetail::MovementsPerIntervalChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{

	if(m_vRunwayOperationDetail == NULL || pParameter == NULL)
		return;



	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Movements per Interval ");


	c2dGraphData.m_strYtitle = _T("Number of Operations");
	c2dGraphData.m_strXtitle = _T("Time of Day");

	int nRunwayCount = m_vRunwayOperationDetail->size();
	if(nRunwayCount == 0)
		return;

	//c2dGraphData.m_vrXTickTitle = m_vTimeRange;


	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());


	bool bCommaFlag = true;
	AirsideRunwayOperationReportParam* pParam = (AirsideRunwayOperationReportParam*)pParameter;
	for (int i=0; i<(int)pParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pParam->getFlightConstraint(i);

		CString strFlight(_T(""));
		fltCons.screenPrint(strFlight.GetBuffer(1024));

		if (bCommaFlag)
		{
			bCommaFlag = false;
			strFooter.AppendFormat("%s", strFlight);
		}
		else
		{
			strFooter.AppendFormat(",%s", strFlight);
		}
	}
	c2dGraphData.m_strFooter = strFooter;

	//x tick, runway
	//for (int nRunway = 0; nRunway < nRunwayCount; ++nRunway)
	//{
	//	c2dGraphData.m_vrXTickTitle.push_back(m_vRunwayOperationDetail->at(nRunway)->m_runWaymark.m_strMarkName);
	//}



	std::vector<CString> vLegends;

	//get the legend
	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		CString strLandingLegend;
		strLandingLegend.Format(_T("%s Landings"),m_vRunwayOperationDetail->at(nRunway)->m_runWaymark.m_strMarkName);
		CString strTakeOffLegend;
		strTakeOffLegend.Format(_T("%s TakeOff"),m_vRunwayOperationDetail->at(nRunway)->m_runWaymark.m_strMarkName);
		vLegends.push_back(strLandingLegend);
		vLegends.push_back(strTakeOffLegend);
		
		std::vector<double > vLandLegendData;
		std::vector<double > vTakeOffLegendData;
		
		int nTimeCount = m_vRunwayOperationDetail->at(nRunway)->m_vRunwayTimeValue.size();
		for (int nTime = 0; nTime < nTimeCount; ++ nTime)
		{
			vLandLegendData.push_back(m_vRunwayOperationDetail->at(nRunway)->m_vRunwayTimeValue[nTime].m_nLandingCount);
			vTakeOffLegendData.push_back(m_vRunwayOperationDetail->at(nRunway)->m_vRunwayTimeValue[nTime].m_nTakeOffCount);
			
			if(nRunway == 0)//x tick
			{
				ElapsedTime eFromtime = m_vRunwayOperationDetail->at(nRunway)->m_vRunwayTimeValue[nTime].m_eTime;
			

				if(nTime < nTimeCount- 1)
				{
					ElapsedTime eToTime = m_vRunwayOperationDetail->at(nRunway)->m_vRunwayTimeValue[nTime + 1].m_eTime;
					CString strXTick;
	
					strXTick.Format(_T("%02d:%02d-%02d:%02d"),eFromtime.GetHour(),eFromtime.GetMinute(),eToTime.GetHour(),eToTime.GetMinute());
					c2dGraphData.m_vrXTickTitle.push_back(strXTick);
				}



			}
		}
		c2dGraphData.m_vr2DChartData.push_back(vLandLegendData);
		c2dGraphData.m_vr2DChartData.push_back(vTakeOffLegendData);

	}
	c2dGraphData.m_vrLegend = vLegends;

	
	chartWnd.DrawChart(c2dGraphData);



}


int AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::CRunwayOperationItem::GetTakeoffCount(CString sRunway, ElapsedTime eTimeFrom, ElapsedTime eTimeTo )
{
	int nTakeOffCount = 0;
	std::vector<CAirsideRunwayOperationReportDetailResult::FlightOperationData>::iterator iter = m_vTakeOff.begin();

	for (; iter != m_vTakeOff.end(); ++iter)
	{
		if ((*iter).m_sRunway != sRunway)
			continue;

		ElapsedTime tTime;
		tTime.setPrecisely((*iter).m_lTime);

		if(eTimeFrom <= tTime && eTimeTo > tTime)
		{
			nTakeOffCount += 1;
		}
	}
	return nTakeOffCount;
}

int AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::CRunwayOperationItem::GetLandingCount(CString sRunway, ElapsedTime eTimeFrom, ElapsedTime eTimeTo )
{
	int nLandingCount = 0;
	std::vector<CAirsideRunwayOperationReportDetailResult::FlightOperationData>::iterator iter = m_vLanding.begin();

	for (; iter != m_vLanding.end(); ++iter)
	{
		if ((*iter).m_sRunway != sRunway)
			continue;

		ElapsedTime tTime;
		tTime.setPrecisely((*iter).m_lTime);

		if(eTimeFrom <= tTime && eTimeTo > tTime)
		{
			nLandingCount += 1;
		}
	}
	return nLandingCount;
}

void AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::CRunwayOperationItem::GetLandingRunway( std::vector<CString>& vRunway )
{
	for (int i = 0; i < (int)m_vLanding.size(); i++)
	{
		CAirsideRunwayOperationReportDetailResult::FlightOperationData& operationData = m_vLanding[i];
		if (std::find(vRunway.begin(),vRunway.end(),operationData.m_sRunway) == vRunway.end())
		{
			vRunway.push_back(operationData.m_sRunway);
		}
	}
}

void AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::CRunwayOperationItem::GetTakeoffRunway( std::vector<CString>& vRunway )
{
	for (int i = 0; i < (int)m_vTakeOff.size(); i++)
	{
		CAirsideRunwayOperationReportDetailResult::FlightOperationData& operationData = m_vTakeOff[i];
		if (std::find(vRunway.begin(),vRunway.end(),operationData.m_sRunway) == vRunway.end())
		{
			vRunway.push_back(operationData.m_sRunway);
		}
	}
}

void AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::CRunwayOperationItem::InitResultData( std::vector<CAirsideRunwayOperationReportDetailResult::FlightOperationData> vOperationData )
{
	m_vLanding.clear();
	m_vTakeOff.clear();

	for (int i = 0; i < (int)vOperationData.size(); i++)
	{
		CAirsideRunwayOperationReportDetailResult::FlightOperationData& operationData = vOperationData[i];
		CString sLandOperation(buf[0]);
		sLandOperation.MakeUpper();

		CString sTakeoffOperation(buf[1]);
		sTakeoffOperation.MakeUpper();

		CString sOperation;
		sOperation = operationData.m_sOperation;
		sOperation.MakeUpper();
	
		if (sOperation.CompareNoCase(sLandOperation) == 0)
		{
			m_vLanding.push_back(operationData);
		}
		else if (sOperation.CompareNoCase(sTakeoffOperation) == 0)
		{
			m_vTakeOff.push_back(operationData);
		}
	}
}

bool AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::CRunwayOperationItem::empty( bool bLanding )
{
	if (bLanding)
	{
		return m_vLanding.empty();
	}
	
	return m_vTakeOff.empty();
}

int AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::CRunwayOperationItem::GetOperationCount( bool bLanding,CString sRunway,ElapsedTime eTimeFrom, ElapsedTime eTimeTo )
{
	if (bLanding)
	{
		return GetLandingCount(sRunway,eTimeFrom,eTimeTo);
	}

	return GetTakeoffCount(sRunway,eTimeFrom,eTimeTo);
}

void AirsideRunwayOperationReportDetail::LandingTakeOffChartResult::CRunwayOperationItem::GetRunway( bool bLanding,std::vector<CString>& vRunway )
{
	if (bLanding)
	{
		GetLandingRunway(vRunway);
	}

	GetTakeoffRunway(vRunway);
}