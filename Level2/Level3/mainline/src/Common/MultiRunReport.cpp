#include "StdAfx.h"
#include ".\multirunreport.h"
#include "../Reports/ReportParameter.h"

CMultiRunReport::CMultiRunReport(void)
{
}

CMultiRunReport::~CMultiRunReport(void)
{
}
//void CMultiRunReport::InitReport(ENUM_REPORT_TYPE enumRepType)
//{
//	m_comModelList.Clear();
//}
void CMultiRunReport::LoadReport(ENUM_REPORT_TYPE enumRepType,const CString& strProjPath)
{
	m_comModelList.Clear();
	
	CMultiReportAndReportManager reportManager(strProjPath);
	CString strFilepath = reportManager.GetReportParamFile(enumRepType);
	m_comModelList.ReadData(strFilepath);

}
void CMultiRunReport::SaveReport(ENUM_REPORT_TYPE enumRepType,const CString& strProjPath)
{
	CMultiReportAndReportManager reportManager(strProjPath);
	CString strFilepath = reportManager.GetReportParamFile(enumRepType);
	m_comModelList.SaveData(strFilepath);
}
void CMultiRunReport::GenerateReport(ENUM_REPORT_TYPE enumRepType,int nDetailOrSummary)
{
	
	//if (nDetailOrSummary == 0 )
	//{
	//
	//	m_paxDataList.clear();
	//	m_occupancyDataList.clear();
	//	m_timeDataList.clear();

	//	switch(enumRepType)
	//	{
	//	case ENUM_DISTANCE_REP:
	//		{
	//			for (int i =0; i < m_comModelList.GetCount(); ++ i)
	//			{
	//				m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_paxDataList);
	//			}

	//		}

	//		break;
	//	case ENUM_QUEUETIME_REP:
	//	case ENUM_SERVICETIME_REP:
	//	case ENUM_DURATION_REP:
	//		{
	//			for (int i =0; i < m_comModelList.GetCount(); ++ i)
	//			{
	//				m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_timeDataList);
	//			}
	//		}
	//		break;

	//	case ENUM_ACTIVEBKDOWN_REP:
	//		break;
	//	case ENUM_PAXCOUNT_REP:
	//		{
	//			for (int i =0; i < m_comModelList.GetCount(); ++ i)
	//			{
	//				m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_occupancyDataList);
	//			}
	//		}
	//		break;
	//	case ENUM_PAXDENS_REP:
	//		{
	//			for (int i =0; i < m_comModelList.GetCount(); ++ i)
	//			{
	//				m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_timeDataList);
	//			}
	//		}
	//		break;
	//	default:
	//		break;
	//	}
	//}
	//else
	//{
	//	m_summaryCriticalQList.clear();
	//	m_summaryQTimeList.clear();
	//	m_summaryTimeServiceList.clear();
	//	switch(enumRepType)
	//	{
	//	case ENUM_QUEUETIME_REP:
	//		{
	//			for (int i =0; i < m_comModelList.GetCount(); ++ i)
	//			{
	//				m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_summaryQTimeList);
	//			}
	//			//calculate average
	//			{
	//				MultiRunsReport::Summary::SummaryQTimeLoader::DataList datalist;
	//					
	//				int nSimResCount = (int)m_summaryQTimeList.size();
	//				if (nSimResCount < 1)
	//					return;

	//				int nCount = static_cast<int>(m_summaryQTimeList[0].size());


	//				for (int i =0; i< nCount; ++i)
	//				{	
	//					MultiRunsReport::Summary::SummaryQueueTimeValue	 averageValue;
	//					for (int nSim =0; nSim < nSimResCount; ++nSim)
	//					{
	//						if (i >= static_cast<int>(m_summaryQTimeList[nSim].size()))
	//						{	
	//							MultiRunsReport::Summary::SummaryQueueTimeValue	 emptyValue;						
	//							emptyValue.strPaxType = m_summaryQTimeList[0][i].strPaxType;
	//							m_summaryQTimeList[nSim].push_back(emptyValue);
	//							continue;
	//						}
	//						averageValue.eAverage += m_summaryQTimeList[nSim][i].eAverage;
	//						averageValue.eMaximum += m_summaryQTimeList[nSim][i].eMaximum;
	//						averageValue.eMinimum += m_summaryQTimeList[nSim][i].eMinimum;
	//						averageValue.eP1 += m_summaryQTimeList[nSim][i].eP1;
	//						averageValue.eP5 += m_summaryQTimeList[nSim][i].eP5;
	//						averageValue.eP10 += m_summaryQTimeList[nSim][i].eP10;
	//						averageValue.eP90 += m_summaryQTimeList[nSim][i].eP90;
	//						averageValue.eP95 += m_summaryQTimeList[nSim][i].eP95;
	//						averageValue.eP99 += m_summaryQTimeList[nSim][i].eP99;
	//						averageValue.eQ1 += m_summaryQTimeList[nSim][i].eQ1;
	//						averageValue.eQ2 += m_summaryQTimeList[nSim][i].eQ2;
	//						averageValue.eQ3 += m_summaryQTimeList[nSim][i].eQ3;
	//						averageValue.nCount += m_summaryQTimeList[nSim][i].nCount;
	//						averageValue.eSigma += m_summaryQTimeList[nSim][i].eSigma;
	//					}

	//					averageValue.eAverage /= nSimResCount;
	//					averageValue.eMaximum /= nSimResCount;
	//					averageValue.eMinimum /= nSimResCount;
	//					averageValue.eP1 /= nSimResCount;
	//					averageValue.eP5 /= nSimResCount;
	//					averageValue.eP10 /= nSimResCount;
	//					averageValue.eP90 /= nSimResCount;
	//					averageValue.eP95 /= nSimResCount;
	//					averageValue.eP99 /= nSimResCount;
	//					averageValue.eQ1 /= nSimResCount;
	//					averageValue.eQ2 /= nSimResCount;
	//					averageValue.eQ3 /= nSimResCount;
	//					averageValue.nCount /= nSimResCount;
	//					averageValue.eSigma /= nSimResCount;
	//					
	//					averageValue.strPaxType = m_summaryQTimeList[0][i].strPaxType;

	//					datalist.push_back(averageValue);

	//				}
	//				m_summaryQTimeList.insert(m_summaryQTimeList.begin(),datalist);
	//			}



	//		}
	//		break;

	//	case ENUM_AVGQUEUELENGTH_REP:
	//		{
	//			for (int i =0; i < m_comModelList.GetCount(); ++ i)
	//			{
	//				m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_summaryCriticalQList);
	//			}

	//			//calculate average
	//			{

	//				MultiRunsReport::Summary::SummaryCriticalTimeLoader::DataList datalist;

	//				int nSimCount = (int)m_summaryCriticalQList.size();
	//				if (nSimCount < 1)
	//					return;

	//				int nCount = static_cast<int>(m_summaryCriticalQList[0].size());
	//					
	//				for (int i =0; i< nCount; ++i)
	//				{
	//					MultiRunsReport::Summary::SummaryCriticalQueueValue averageValue;

	//					for (int nSim =0; nSim < nSimCount; ++nSim)
	//					{
	//						if (i >= static_cast<int>(m_summaryCriticalQList[nSim].size()))
	//						{	
	//							MultiRunsReport::Summary::SummaryCriticalQueueValue	 emptyValue;						
	//							emptyValue.strProcName = m_summaryCriticalQList[0][i].strProcName;
	//							m_summaryCriticalQList[nSim].push_back(emptyValue);
	//							continue;
	//						}

	//						averageValue.nGroupSize += m_summaryCriticalQList[nSim][i].nGroupSize;
	//						averageValue.fAvgQueue +=  m_summaryCriticalQList[nSim][i].fAvgQueue;
	//						averageValue.nMaxQueue +=  m_summaryCriticalQList[nSim][i].nMaxQueue;
	//						averageValue.eMaxQueueTime +=  m_summaryCriticalQList[nSim][i].eMaxQueueTime;
	//					}

	//					averageValue.nGroupSize /= nSimCount;
	//					averageValue.fAvgQueue /=  nSimCount;
	//					averageValue.nMaxQueue /=  nSimCount;
	//					averageValue.eMaxQueueTime /= nSimCount;

	//					averageValue.strProcName = m_summaryCriticalQList[0][i].strProcName;
	//					
	//					datalist.push_back(averageValue);
	//				}
	//				m_summaryCriticalQList.insert(m_summaryCriticalQList.begin(),datalist);

	//			}
	//		}
	//		break;
	//	case ENUM_SERVICETIME_REP:
	//		{
	//			for (int i =0; i < m_comModelList.GetCount(); ++ i)
	//			{
	//				m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_summaryTimeServiceList);
	//			}
	//			//calculate average
	//			{
	//				MultiRunsReport::Summary::SummaryTimeServiceLoader::DataList datalist;

	//				int nSimResCount = (int)m_summaryTimeServiceList.size();
	//				if (nSimResCount < 1)
	//					return;

	//				int nCount = static_cast<int>(m_summaryTimeServiceList[0].size());


	//				for (int i =0; i< nCount; ++i)
	//				{	
	//					MultiRunsReport::Summary::SummaryTimeServiceValue	 averageValue;
	//					for (int nSim =0; nSim < nSimResCount; ++nSim)
	//					{
	//						if (i >= static_cast<int>(m_summaryTimeServiceList[nSim].size()))
	//						{	
	//							MultiRunsReport::Summary::SummaryTimeServiceValue	 emptyValue;						
	//							emptyValue.strPaxType = m_summaryTimeServiceList[0][i].strPaxType;
	//							m_summaryTimeServiceList[nSim].push_back(emptyValue);
	//							continue;
	//						}
	//						averageValue.eAverage += m_summaryTimeServiceList[nSim][i].eAverage;
	//						averageValue.eMaximum += m_summaryTimeServiceList[nSim][i].eMaximum;
	//						averageValue.eMinimum += m_summaryTimeServiceList[nSim][i].eMinimum;
	//						averageValue.eP1 += m_summaryTimeServiceList[nSim][i].eP1;
	//						averageValue.eP5 += m_summaryTimeServiceList[nSim][i].eP5;
	//						averageValue.eP10 += m_summaryTimeServiceList[nSim][i].eP10;
	//						averageValue.eP90 += m_summaryTimeServiceList[nSim][i].eP90;
	//						averageValue.eP95 += m_summaryTimeServiceList[nSim][i].eP95;
	//						averageValue.eP99 += m_summaryTimeServiceList[nSim][i].eP99;
	//						averageValue.eQ1 += m_summaryTimeServiceList[nSim][i].eQ1;
	//						averageValue.eQ2 += m_summaryTimeServiceList[nSim][i].eQ2;
	//						averageValue.eQ3 += m_summaryTimeServiceList[nSim][i].eQ3;
	//						averageValue.nCount += m_summaryTimeServiceList[nSim][i].nCount;
	//						averageValue.eSigma += m_summaryTimeServiceList[nSim][i].eSigma;
	//					}

	//					averageValue.eAverage /= nSimResCount;
	//					averageValue.eMaximum /= nSimResCount;
	//					averageValue.eMinimum /= nSimResCount;
	//					averageValue.eP1 /= nSimResCount;
	//					averageValue.eP5 /= nSimResCount;
	//					averageValue.eP10 /= nSimResCount;
	//					averageValue.eP90 /= nSimResCount;
	//					averageValue.eP95 /= nSimResCount;
	//					averageValue.eP99 /= nSimResCount;
	//					averageValue.eQ1 /= nSimResCount;
	//					averageValue.eQ2 /= nSimResCount;
	//					averageValue.eQ3 /= nSimResCount;
	//					averageValue.nCount /= nSimResCount;
	//					averageValue.eSigma /= nSimResCount;

	//					averageValue.strPaxType = m_summaryTimeServiceList[0][i].strPaxType;

	//					datalist.push_back(averageValue);

	//				}
	//				m_summaryTimeServiceList.insert(m_summaryTimeServiceList.begin(),datalist);
	//			}


	//		}
	//		break;

	//	default:
	//		break;
	//	}
	//}

	////remove the items are not valid
	//for (int i =0; i <m_comModelList.GetCount(); ++i )
	//{
	//	if (!m_comModelList.GetModel(i).IsValid())
	//	{
	//		m_comModelList.DelModel(i);
	//	}
	//}

}

void CMultiRunReport::GenerateReport( CReportParameter *pReportParam,const CString& strProjPath, Terminal *pTerminal, int nFloorCount )
{
	if(pReportParam == NULL)
		return;

	if(m_comModelList.GetCount() == 0)
		return;
	
	//set report runs
	CCompareModel& comparModel = m_comModelList.GetModel(0);

	std::vector<int> vReportRuns;
	if(!pReportParam->GetReportRuns(vReportRuns))
		return;

	for (int nRun = 0; nRun < static_cast<int>(vReportRuns.size()); ++nRun)
	{
		CString strSimResult;
		strSimResult.Format(_T("RUN%d"),vReportRuns.at(nRun));
		comparModel.AddSimResult(strSimResult);
	}

//detail or summary
	int nDetailOrSummary;
	pReportParam->GetReportType(nDetailOrSummary);
	ENUM_REPORT_TYPE enumRepType = pReportParam->GetReportCategory();



	if (nDetailOrSummary == 0 )
	{

		m_paxDataList.clear();
		m_occupancyDataList.clear();
		m_timeDataList.clear();

		switch(enumRepType)
		{
		case ENUM_DISTANCE_REP:
			{
				for (int i =0; i < m_comModelList.GetCount(); ++ i)
				{
					m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_paxDataList,pReportParam,strProjPath,pTerminal,nFloorCount);
				}

			}

			break;
		case ENUM_QUEUETIME_REP:
		case ENUM_SERVICETIME_REP:
		case ENUM_DURATION_REP:
			{
				for (int i =0; i < m_comModelList.GetCount(); ++ i)
				{
					m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_timeDataList,pReportParam,strProjPath,pTerminal,nFloorCount);
				}
			}
			break;

		case ENUM_ACTIVEBKDOWN_REP:
			break;
		case ENUM_PAXCOUNT_REP:
			{
				for (int i =0; i < m_comModelList.GetCount(); ++ i)
				{
					m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_occupancyDataList,pReportParam,strProjPath,pTerminal,nFloorCount);
				}
			}
			break;
		case ENUM_PAXDENS_REP:
			{
				for (int i =0; i < m_comModelList.GetCount(); ++ i)
				{
					m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_occupancyDataList,pReportParam,strProjPath,pTerminal,nFloorCount);
				}
			}
			break;
		default:
			break;
		}
	}
	else
	{
		m_summaryCriticalQList.clear();
		m_summaryQTimeList.clear();
		m_summaryTimeServiceList.clear();
		switch(enumRepType)
		{
		case ENUM_QUEUETIME_REP:
			{
				for (int i =0; i < m_comModelList.GetCount(); ++ i)
				{
					m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_summaryQTimeList,pReportParam,strProjPath,pTerminal,nFloorCount);
				}
				//calculate average
				{
					MultiRunsReport::Summary::SummaryQTimeLoader::DataList datalist;

					int nSimResCount = (int)m_summaryQTimeList.size();
					if (nSimResCount < 1)
						return;

					int nCount = static_cast<int>(m_summaryQTimeList[0].size());


					for (int i =0; i< nCount; ++i)
					{	
						MultiRunsReport::Summary::SummaryQueueTimeValue	 averageValue;
						for (int nSim =0; nSim < nSimResCount; ++nSim)
						{
							if (i >= static_cast<int>(m_summaryQTimeList[nSim].size()))
							{	
								MultiRunsReport::Summary::SummaryQueueTimeValue	 emptyValue;						
								emptyValue.strPaxType = m_summaryQTimeList[0][i].strPaxType;
								m_summaryQTimeList[nSim].push_back(emptyValue);
								continue;
							}
							averageValue.eAverage += m_summaryQTimeList[nSim][i].eAverage;
							averageValue.eMaximum += m_summaryQTimeList[nSim][i].eMaximum;
							averageValue.eMinimum += m_summaryQTimeList[nSim][i].eMinimum;
							averageValue.eP1 += m_summaryQTimeList[nSim][i].eP1;
							averageValue.eP5 += m_summaryQTimeList[nSim][i].eP5;
							averageValue.eP10 += m_summaryQTimeList[nSim][i].eP10;
							averageValue.eP90 += m_summaryQTimeList[nSim][i].eP90;
							averageValue.eP95 += m_summaryQTimeList[nSim][i].eP95;
							averageValue.eP99 += m_summaryQTimeList[nSim][i].eP99;
							averageValue.eQ1 += m_summaryQTimeList[nSim][i].eQ1;
							averageValue.eQ2 += m_summaryQTimeList[nSim][i].eQ2;
							averageValue.eQ3 += m_summaryQTimeList[nSim][i].eQ3;
							averageValue.nCount += m_summaryQTimeList[nSim][i].nCount;
							averageValue.eSigma += m_summaryQTimeList[nSim][i].eSigma;
						}

						averageValue.eAverage /= nSimResCount;
						averageValue.eMaximum /= nSimResCount;
						averageValue.eMinimum /= nSimResCount;
						averageValue.eP1 /= nSimResCount;
						averageValue.eP5 /= nSimResCount;
						averageValue.eP10 /= nSimResCount;
						averageValue.eP90 /= nSimResCount;
						averageValue.eP95 /= nSimResCount;
						averageValue.eP99 /= nSimResCount;
						averageValue.eQ1 /= nSimResCount;
						averageValue.eQ2 /= nSimResCount;
						averageValue.eQ3 /= nSimResCount;
						averageValue.nCount /= nSimResCount;
						averageValue.eSigma /= nSimResCount;

						averageValue.strPaxType = m_summaryQTimeList[0][i].strPaxType;

						datalist.push_back(averageValue);

					}
					m_summaryQTimeList.push_back(datalist);
				}



			}
			break;

		case ENUM_AVGQUEUELENGTH_REP:
			{
				for (int i =0; i < m_comModelList.GetCount(); ++ i)
				{
					m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_summaryCriticalQList,pReportParam,strProjPath,pTerminal,nFloorCount);
				}

				//calculate average
				{

					MultiRunsReport::Summary::SummaryCriticalTimeLoader::DataList datalist;

					int nSimCount = (int)m_summaryCriticalQList.size();
					if (nSimCount < 1)
						return;

					int nCount = static_cast<int>(m_summaryCriticalQList[0].size());

					for (int i =0; i< nCount; ++i)
					{
						MultiRunsReport::Summary::SummaryCriticalQueueValue averageValue;

						for (int nSim =0; nSim < nSimCount; ++nSim)
						{
							if (i >= static_cast<int>(m_summaryCriticalQList[nSim].size()))
							{	
								MultiRunsReport::Summary::SummaryCriticalQueueValue	 emptyValue;						
								emptyValue.strProcName = m_summaryCriticalQList[0][i].strProcName;
								m_summaryCriticalQList[nSim].push_back(emptyValue);
								continue;
							}

							averageValue.nGroupSize += m_summaryCriticalQList[nSim][i].nGroupSize;
							averageValue.fAvgQueue +=  m_summaryCriticalQList[nSim][i].fAvgQueue;
							averageValue.nMaxQueue +=  m_summaryCriticalQList[nSim][i].nMaxQueue;
							averageValue.eMaxQueueTime +=  m_summaryCriticalQList[nSim][i].eMaxQueueTime;
						}

						averageValue.nGroupSize /= nSimCount;
						averageValue.fAvgQueue /=  nSimCount;
						averageValue.nMaxQueue /=  nSimCount;
						averageValue.eMaxQueueTime /= nSimCount;

						averageValue.strProcName = m_summaryCriticalQList[0][i].strProcName;

						datalist.push_back(averageValue);
					}
					m_summaryCriticalQList.push_back(datalist);

				}
			}
			break;
		case ENUM_SERVICETIME_REP:
			{
				for (int i =0; i < m_comModelList.GetCount(); ++ i)
				{
					m_comModelList.GetModel(i).GenerateReport(enumRepType,nDetailOrSummary,(void *)&m_summaryTimeServiceList,pReportParam,strProjPath,pTerminal,nFloorCount);
				}
				//calculate average
				{
					MultiRunsReport::Summary::SummaryTimeServiceLoader::DataList datalist;

					int nSimResCount = (int)m_summaryTimeServiceList.size();
					if (nSimResCount < 1)
						return;

					int nCount = static_cast<int>(m_summaryTimeServiceList[0].size());


					for (int i =0; i< nCount; ++i)
					{	
						MultiRunsReport::Summary::SummaryTimeServiceValue	 averageValue;
						for (int nSim =0; nSim < nSimResCount; ++nSim)
						{
							if (i >= static_cast<int>(m_summaryTimeServiceList[nSim].size()))
							{	
								MultiRunsReport::Summary::SummaryTimeServiceValue	 emptyValue;						
								emptyValue.strPaxType = m_summaryTimeServiceList[0][i].strPaxType;
								m_summaryTimeServiceList[nSim].push_back(emptyValue);
								continue;
							}
							averageValue.eAverage += m_summaryTimeServiceList[nSim][i].eAverage;
							averageValue.eMaximum += m_summaryTimeServiceList[nSim][i].eMaximum;
							averageValue.eMinimum += m_summaryTimeServiceList[nSim][i].eMinimum;
							averageValue.eP1 += m_summaryTimeServiceList[nSim][i].eP1;
							averageValue.eP5 += m_summaryTimeServiceList[nSim][i].eP5;
							averageValue.eP10 += m_summaryTimeServiceList[nSim][i].eP10;
							averageValue.eP90 += m_summaryTimeServiceList[nSim][i].eP90;
							averageValue.eP95 += m_summaryTimeServiceList[nSim][i].eP95;
							averageValue.eP99 += m_summaryTimeServiceList[nSim][i].eP99;
							averageValue.eQ1 += m_summaryTimeServiceList[nSim][i].eQ1;
							averageValue.eQ2 += m_summaryTimeServiceList[nSim][i].eQ2;
							averageValue.eQ3 += m_summaryTimeServiceList[nSim][i].eQ3;
							averageValue.nCount += m_summaryTimeServiceList[nSim][i].nCount;
							averageValue.eSigma += m_summaryTimeServiceList[nSim][i].eSigma;
						}

						averageValue.eAverage /= nSimResCount;
						averageValue.eMaximum /= nSimResCount;
						averageValue.eMinimum /= nSimResCount;
						averageValue.eP1 /= nSimResCount;
						averageValue.eP5 /= nSimResCount;
						averageValue.eP10 /= nSimResCount;
						averageValue.eP90 /= nSimResCount;
						averageValue.eP95 /= nSimResCount;
						averageValue.eP99 /= nSimResCount;
						averageValue.eQ1 /= nSimResCount;
						averageValue.eQ2 /= nSimResCount;
						averageValue.eQ3 /= nSimResCount;
						averageValue.nCount /= nSimResCount;
						averageValue.eSigma /= nSimResCount;

						averageValue.strPaxType = m_summaryTimeServiceList[0][i].strPaxType;

						datalist.push_back(averageValue);

					}
					m_summaryTimeServiceList.push_back(datalist);
				}


			}
			break;

		default:
			break;
		}
	}

}
int CMultiRunReport::GetAllSimResultCount()
{

	int nCount = 0;
	for (int i =0; i < m_comModelList.GetCount(); ++ i)
	{
		nCount += m_comModelList.GetModel(i).GetSimResultCount();
	}
	return nCount;
}
std::vector<CString> CMultiRunReport::GetAllSimResultFullName()
{
	std::vector<CString> vLabels;
	for (int i =0; i < m_comModelList.GetCount(); ++ i)
	{
		CString strModelName = m_comModelList.GetModel(i).GetModelName();
		CString strLabelName;
		int nSimResultCount = m_comModelList.GetModel(i).GetSimResultCount();
		for (int j =0; j < nSimResultCount; ++j )
		{
			strLabelName.Format(_T("%s"),m_comModelList.GetModel(i).GetSimResult(j));
			vLabels.push_back(strLabelName);
		}
	}
	
	return vLabels;
}
