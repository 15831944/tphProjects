#include "stdafx.h"
#include "AirsideTaxiwayUtilizationResult.h"
#include "AirsideTaxiwayUtilizationChart.h"
#include "AirsideTaxiwayUtilizationPara.h"
#include "TaxiwayUtilizationData.h"
#include "../Results/AirsideFlightEventLog.h"
#include "Common/ProgressBar.h"
#include "Common/ARCUnit.h"
#include "../Results/AirsideFlightLog.h"
#include <limits>
//base class for detail and summary result///////////////////////////////////////
CAirsideTaxiwayUtilizationBaseResult::CAirsideTaxiwayUtilizationBaseResult()
:m_pChartResult(NULL)
,m_bLoadFromFile(false)
,m_pCBGetLogFilePath(NULL)
{

}

CAirsideTaxiwayUtilizationBaseResult::~CAirsideTaxiwayUtilizationBaseResult()
{
	for (int i = 0; i < (int)m_vResult.size(); i++)
	{
		delete m_vResult[i];
	}
	m_vResult.clear();

	for (int ii = 0; ii < (int)m_vResultItem.size(); ii++)
	{
		delete m_vResultItem[ii];
	}
	m_vResultItem.clear();

	for (int m = 0; m < (int)m_vFlightInfoList.size(); m++)
	{
		delete m_vFlightInfoList[m];
	}
	m_vFlightInfoList.clear();

	for (int n = 0; n < (int)m_vAllGroupNodeInOut.size();n++)
	{
		delete m_vAllGroupNodeInOut[n];
	}
	m_vAllGroupNodeInOut.clear();
}

void CAirsideTaxiwayUtilizationBaseResult::setLoadFromFile( bool bLoad )
{
	m_bLoadFromFile = bLoad;
}

bool CAirsideTaxiwayUtilizationBaseResult::IsLoadFromFile()
{
	return m_bLoadFromFile;

}

void CAirsideTaxiwayUtilizationBaseResult::SetCBGetFilePath( CBGetLogFilePath pFunc )
{
	m_pCBGetLogFilePath= pFunc;
}

void CAirsideTaxiwayUtilizationBaseResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pChartResult)
	{
		m_pChartResult->Draw3DChart(chartWnd,pParameter);
	}
}


std::vector<std::pair<CString,std::pair<CString,CString> > > CAirsideTaxiwayUtilizationBaseResult::GetTaxiwayResultCount(std::vector<TaxiwayData*>& vResult)
{
	int nResultCount = (int)vResult.size();
	std::vector<std::pair<CString,std::pair<CString,CString> > > vTaxiway;
	for (int i = 0; i < nResultCount; i++)
	{
		TaxiwayData* data = vResult[i];

		std::pair<CString,std::pair<CString,CString> >& item = std::make_pair(data->m_sTaxiway,std::make_pair(data->m_sStartNode,data->m_sEndNode));
		if (std::find(vTaxiway.begin(),vTaxiway.end(),item) == vTaxiway.end())
		{
			vTaxiway.push_back(item);
		}
	}
	return vTaxiway;
}

BOOL CAirsideTaxiwayUtilizationBaseResult::ExportListData(ArctermFile& _file,CParameters * parameter)
{
	return FALSE;
}

BOOL CAirsideTaxiwayUtilizationBaseResult::ExportReportData(ArctermFile& _file,CString& Errmsg)
{
	_file.writeField("Taxiway Utilization Report");
	_file.writeLine();

	int nCount = (int)m_vResult.size();
	_file.writeInt(nCount);
	_file.writeLine();

	for (int nIndex = 0; nIndex < nCount; ++ nIndex)
	{
		CTaxiwayUtilizationData* item = m_vResult[nIndex];
		item->WriteReportData(_file);
		_file.writeLine();
	}

	return TRUE;
}

BOOL CAirsideTaxiwayUtilizationBaseResult::ImportReportData(ArctermFile& _file,CString& Errmsg)
{
	int nCount = 0;
	
	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; ++nIndex)
	{
		CTaxiwayUtilizationData* item = NULL;
		if (GetReportType() == ASReportType_Detail)
		{
			item = new CTaxiwayUtilizationDetailData();
		}
		else
		{
			item = new CTaxiwayUtilizationSummaryData();
		}
		
		item->ReadReportData(_file);
		m_vResult.push_back(item);
		_file.getLine();

	}
	return TRUE;
}

BOOL CAirsideTaxiwayUtilizationBaseResult::WriteReportData(ArctermFile& _file)
{
	_file.writeField("Taxiway Utilization Report");
	_file.writeLine();

	_file.writeInt(GetReportType());
	_file.writeLine();

	int nCount = (int)m_vResult.size();
	_file.writeInt(nCount);
	_file.writeLine();

	for (int nIndex = 0; nIndex < nCount; ++ nIndex)
	{
		CTaxiwayUtilizationData* item = m_vResult[nIndex];
		item->WriteReportData(_file);
		_file.writeLine();
	}

	return TRUE;
}

BOOL CAirsideTaxiwayUtilizationBaseResult::ReadReportData(ArctermFile& _file)
{
	int nCount = 0;

	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; ++nIndex)
	{
		CTaxiwayUtilizationData* item = NULL;
		if (GetReportType() == ASReportType_Detail)
		{
			item = new CTaxiwayUtilizationDetailData();
		}
		else
		{
			item = new CTaxiwayUtilizationSummaryData();
		}

		item->ReadReportData(_file);
		m_vResult.push_back(item);
		_file.getLine();

	}
	return TRUE;
}

void CAirsideTaxiwayUtilizationBaseResult::GenerateIntervalTime(CParameters *pParameter)
{
	if (pParameter == NULL)
		return;

	m_vGroup.clear();
	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime());
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval();

	long lUserIntervalTime = pParameter->getInterval();
	long lStartTime = pParameter->getStartTime().asSeconds();

	for (long nIneterVer = 0 ; nIneterVer < intervalSize ;nIneterVer++)
	{
		GroupInterval group;
		group.lStart = lStartTime + nIneterVer*lUserIntervalTime;
		group.lEnd = group.lStart + lUserIntervalTime;
		m_vGroup.push_back(group);
	}
}

void CAirsideTaxiwayUtilizationBaseResult::getIntervalTime(GroupInterval& groupSource,AirsideFlightNodeLog *pNodeLog,CParameters *pParameter)
{
	CAirsideTaxiwayUtilizationPara *pParam = (CAirsideTaxiwayUtilizationPara *)pParameter;
	if (pParam == NULL)
		return;

	for (int i = 0; i < (int)m_vGroup.size(); i++)
	{
		GroupInterval& group = m_vGroup[i];
		long time = static_cast<long>(pNodeLog->time/100);
		if (time >= group.lStart && time <= group.lEnd)
		{
			groupSource.lStart = group.lStart;
			groupSource.lEnd = group.lEnd;
			return;
		}
	}
}
/////////////////detail result//////////////////////////////////////////////////////////////////
CAirsideTaxiwayUtilizationDetailResult::CAirsideTaxiwayUtilizationDetailResult()
{

}

CAirsideTaxiwayUtilizationDetailResult::~CAirsideTaxiwayUtilizationDetailResult()
{
	m_vGroup.clear();
}



void CAirsideTaxiwayUtilizationDetailResult::GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter)
{
	for (int n = 0; n < (int)m_vResult.size(); n++)
	{
		delete m_vResult[n];
	}
	m_vResult.clear();

	if (pParameter == NULL)
		return;
	CAirsideTaxiwayUtilizationPara* pParam = (CAirsideTaxiwayUtilizationPara*)pParameter;
	if(pParam == NULL)
		return;

	//read old log
	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);


	//read new log
	strDescFilepath = (*pCBGetLogFilePath)(AirsideFlightDescFileReport);
	strEventFilePath = (*pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	ElapsedTime eStartTime = pParam->getStartTime();
	ElapsedTime eEndTime = pParam->getEndTime();

	GenerateIntervalTime(pParameter);

	AirsideReportLog::AirsideFlightLogData mFlightLogData;
	mFlightLogData.OpenInput(strDescFilepath.GetString() ,strEventFilePath.GetString());
	int nCount = mFlightLogData.getItemCount();
	CProgressBar progressBar( "Generating report ", 100, nCount, TRUE, 1 );

	pParam->InitTaxiNodeIndex();

	CTaxiwayItemList& listData = pParam->GetResultTaxiwayItemList();
	listData.InitTaxiNodeIndex();

	for (int i = 0; i < nCount; i++)
	{
		progressBar.StepIt();

		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);
		int nLogCount = (int)item.mpEventList->mEventList.size();

		//old log
		AirsideFlightLogEntry logEntry;
		logEntry.SetEventLog(&airsideFlightEventLog);
		fltLog.GetItemByID(logEntry,item.mFltDesc.id);
		
		for (int nReIndex = 0; nReIndex < (int)m_vResultItem.size(); nReIndex++)
		{
			delete m_vResultItem[nReIndex];
		}
		m_vResultItem.clear();

		bool bFind = false;
		for (int j = 0; j < nLogCount; j++)
		{
			
			ARCEventLog* pLog = item.mpEventList->mEventList.at(j);
			if(pLog == NULL)
				continue;

			if(pLog->time < eStartTime.getPrecisely())
				continue;
			if(pLog->time > eEndTime.getPrecisely())
				break;

			if(pLog->IsA(typeof(AirsideFlightStandOperationLog)))
			{
				AirsideFlightStandOperationLog* pStandLog = (AirsideFlightStandOperationLog*)pLog;
				if (pStandLog->m_eOpType == AirsideFlightStandOperationLog::LeavingStand)
				{
					bFind = true;
				}
			}

			if(pLog->IsA(typeof(FlightCrossNodeLog)))
			{
				FlightCrossNodeLog *pNodeLog = (FlightCrossNodeLog *)pLog;

				if (pNodeLog && (pNodeLog->meType == FlightCrossNodeLog::OUT_NODE || pNodeLog->meType == FlightCrossNodeLog::IN_NODE))
				{
					if(!pParam->GetResultTaxiwayItemList().IsNodeSelected(pNodeLog->mNodeId))
					{
						while (!m_vResultItem.empty())
						{
							TaxiwayResultItem* lastItem = m_vResultItem.front();
							
							int nFlag = 0;
							if(lastItem->m_vVisitFlag.size() <= 1 && pNodeLog->meType == FlightCrossNodeLog::IN_NODE)
							{
								m_vResultItem.pop_front();
								delete lastItem;
								continue;
							}

							if (lastItem->isVisit())
							{
								lastItem->m_bWaitForGenerationResult = true;
								int nCount = lastItem->m_vVisitFlag.size();
								TaxiwayResultItem::GroupNodeInOut* pNodeGroup = lastItem->m_vVisitFlag[nCount-1];
								if (pNodeGroup->m_pInLog)
								{
									lastItem->BuiltDetailResult(pNodeGroup->m_pInLog,item,logEntry,pParameter,nFlag);
								}
								else if (pNodeGroup->m_pOutLog)
								{
									lastItem->BuiltDetailResult(pNodeGroup->m_pOutLog,item,logEntry,pParameter,nFlag);
								}

								m_vResultItem.pop_front();
								delete lastItem;
							}
							
							break;
						}
						
					}
					else
					{
						if (bFind == true)
						{
							pNodeLog->mEnterStand = 1;
							bFind = false;
						}
						HandleIntersectionNode(logEntry,pNodeLog,item,pParam);
					}
				}
			}
		}

		while (!m_vResultItem.empty())
		{
			TaxiwayResultItem* lastItem = m_vResultItem.front();

			int nFlag = 0;
			if(lastItem->m_vVisitFlag.size() <= 1)
			{
				m_vResultItem.pop_front();
				delete lastItem;
				continue;
			}

			if (lastItem->isVisit())
			{
				lastItem->m_bWaitForGenerationResult = true;
				int nCount = lastItem->m_vVisitFlag.size();
				TaxiwayResultItem::GroupNodeInOut* pNodeGroup = lastItem->m_vVisitFlag[nCount-1];
				if (pNodeGroup->m_pInLog)
				{
					lastItem->BuiltDetailResult(pNodeGroup->m_pInLog,item,logEntry,pParameter,nFlag);
				}
				else if (pNodeGroup->m_pOutLog)
				{
					lastItem->BuiltDetailResult(pNodeGroup->m_pOutLog,item,logEntry,pParameter,nFlag);
				}
			}
			m_vResultItem.pop_front();
			delete lastItem;
		}
	}
}
CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::GroupNodeInOut* CAirsideTaxiwayUtilizationDetailResult::TaxiwayResultItem::getGroupNode(FlightCrossNodeLog* pNodeLog)
{
	for (int i = 0; i < (int)m_vVisitFlag.size(); i++)
	{
		GroupNodeInOut* groupNode = m_vVisitFlag.at(i);
		FlightCrossNodeLog* pNode = groupNode->getNode();
		if (pNode && !groupNode->isVisit() &&pNode->mNodeId == pNodeLog->mNodeId)
		{
			return groupNode;
		}
	}
	return NULL;
}
void CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::setGroupNode(FlightCrossNodeLog* pNodeLog)
{
	GroupNodeInOut* pGroup = getGroupNode(pNodeLog);
	if (pGroup)
	{
		pGroup->setNode(pNodeLog);
	}
	else
	{
		GroupNodeInOut* groupNode = new GroupNodeInOut;
		groupNode->setNode(pNodeLog);
		m_vVisitFlag.push_back(groupNode);
		m_pParent->m_vAllGroupNodeInOut.push_back(groupNode);
	}
}

bool CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::findNode(FlightCrossNodeLog* pNodeLog)const
{
	for (int i = 0; i < (int)m_vVisitFlag.size(); i++)
	{
		const GroupNodeInOut* groupNode = m_vVisitFlag.at(i);
		if (groupNode->findNode(pNodeLog))
		{
			return true; 
		}
	}
	return false;
}

bool CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::isVisit()const
{
	for (int i = 0; i < (int)m_vVisitFlag.size(); i++)
	{
		const GroupNodeInOut* groupNode = m_vVisitFlag.at(i);
		if (!groupNode->isVisit())
		{
			return false; 
		}
	}
	return true;
}

bool CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::exsit(FlightCrossNodeLog* pNodeLog)const
{
	for (int i = 0; i < (int)m_vVisitFlag.size(); i++)
	{
		const GroupNodeInOut* groupNode = m_vVisitFlag.at(i);
		if (groupNode->exsit(pNodeLog))
		{
			return true; 
		}
	}
	return false;
}

double CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::GetNodeSpeed(long startTime,long endTime,AirsideFlightLogEntry& logEntry)
{
	if (startTime > endTime)
	{
		std::swap(startTime,endTime);
	}
	

	double dSpeed = 0.0;
	int iStart = -1;
	int iEnd = -1;
	long nLogCount = logEntry.getCount();
	for (long i = 0; i < nLogCount; i++)
	{
	    AirsideFlightEventStruct event = logEntry.getEvent(i);
		if (startTime <= event.time)
		{
			iStart = i;
			break;
		}
	}

	for (long ii = nLogCount - 1; ii >= 0; ii--)
	{
		AirsideFlightEventStruct event = logEntry.getEvent(ii);
		if (endTime >= event.time)
		{
			iEnd = ii;
			break;
		}
	}

	//not find start time
	if (iStart == -1)
	{
		iStart = 0;
	}

	//not find end time
	if (iEnd == -1)
	{
		iEnd = logEntry.getCount() - 1;// end index
	}

	if (iStart > iEnd)
	{
		std::swap(iStart,iEnd);
	}

	
	double dTime = 0.0;
	AirsideFlightEventStruct firstEvent = logEntry.getEvent(iStart);
	AirsideFlightEventStruct endEvent = logEntry.getEvent(iEnd);
//	lTime = endEvent.time - firstEvent.time;

	if (iStart == iEnd)
	{
		dSpeed = ARCUnit::ConvertVelocity(firstEvent.speed,ARCUnit::CMpS,ARCUnit::KNOT);
	}
	else
	{
		double dDist = 0.0;
		for (int i = iStart + 1; i <= iEnd; i++)
		{
			AirsideFlightEventStruct secondEvent = logEntry.getEvent(i);
			float fDisX = secondEvent.x - firstEvent.x;
			float fDisY = secondEvent.y - firstEvent.y;
			float fDisZ = secondEvent.z - firstEvent.z;
			float fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			dDist += fDistance;
			double dDurationTime = 0.0;
			if (firstEvent.speed + secondEvent.speed >= (std::numeric_limits<double>::min)())
			{
				dDurationTime = (2*100*fDistance)/(firstEvent.speed + secondEvent.speed);
			}
			
			dTime += dDurationTime;
			firstEvent = secondEvent;
		}

		if (dTime >= (std::numeric_limits<double>::min)())
		{
			dSpeed = ARCUnit::ConvertVelocity((dDist*100)/dTime,ARCUnit::CMpS,ARCUnit::KNOT);
		}
		else
		{
			dSpeed = ARCUnit::ConvertVelocity(firstEvent.speed,ARCUnit::CMpS,ARCUnit::KNOT);
		}

	}

	return dSpeed;
}

bool CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::BuiltDetailResult(AirsideFlightNodeLog *pNodeLog,const AirsideFlightLogItem& flightItem,AirsideFlightLogEntry& logEntry,CParameters *pParameter,int& nFlag)
{
	CAirsideTaxiwayUtilizationPara *pParam = (CAirsideTaxiwayUtilizationPara *)pParameter;
	if (pParam == NULL)
		return false;


	FlightCrossNodeLog* pCurLog = (FlightCrossNodeLog*)pNodeLog;

	if (m_vVisitFlag.size() <= 1)
	{
		setGroupNode(pCurLog);
		return false;
	}

	if (m_bWaitForGenerationResult && !isVisit())
	{
		if(findNode(pCurLog))
		{
			setGroupNode(pCurLog);
			return false;
		}
		return true;
	}
	//get node name list
	std::vector<CString>vNodeNameList;
	for (int i = 0; i < (int)m_vVisitFlag.size(); i++)
	{
		FlightCrossNodeLog* pLog = m_vVisitFlag[i]->getNode(); 
		if (pLog && std::find(vNodeNameList.begin(),vNodeNameList.end(),pLog->mNodeName.c_str()) == vNodeNameList.end())
		{
			vNodeNameList.push_back(pLog->mNodeName.c_str());
		}
	}

	bool bOk = false;
	if (m_bWaitForGenerationResult && isVisit())
	{
		bOk = true;
	}

	if (std::find(vNodeNameList.begin(),vNodeNameList.end(),pNodeLog->mNodeName.c_str()) == vNodeNameList.end() && !bOk)
	{
		vNodeNameList.push_back(pNodeLog->mNodeName.c_str());
	}

	CTaxiwayItemList taxiwayList1;
	if (pParam->GetResultTaxiwayItemList().IfNodeListInSameTaxiwayAndGetTaxiwayList(vNodeNameList,taxiwayList1) && !pCurLog->mEnterStand&&!bOk)
	{
		setGroupNode(pCurLog);
		return false;
	}
	else
	{
		if (m_bWaitForGenerationResult == false)
		{
			m_bWaitForGenerationResult = true;
		}
		if (!isVisit())
		{
			if (!pCurLog->mEnterStand)
			{
				nFlag = 1;//need add end nodegroup to new taxiway result item
			}
			else
			{
				nFlag = 0;//no
			}
			return true;
		}

		CTaxiwayItemList taxiwayList2;
		if (!bOk)
		{
			vNodeNameList.pop_back();
		}
		
		if (!m_bAreadyGenerateResult)
		{
			pParam->GetResultTaxiwayItemList().IfNodeListInSameTaxiwayAndGetTaxiwayList(vNodeNameList,taxiwayList2);

			for (int j = 0; j < taxiwayList2.getItemCount(); j++)
			{
				CTaxiwayItem* taxiwayItem = taxiwayList2.at(j);

				FlightCrossNodeLog* pStartLog = (FlightCrossNodeLog*)m_vVisitFlag[0]->m_pInLog;
				FlightCrossNodeLog* pEndLog = (FlightCrossNodeLog*)m_vVisitFlag[m_vVisitFlag.size() - 1]->m_pOutLog;
				

			/*	FlightCrossNodeLog* pStartLog1 = (FlightCrossNodeLog*)m_vVisitFlag[0]->m_pOutLog;
				FlightCrossNodeLog* pEngLog1 = (FlightCrossNodeLog*)m_vVisitFlag[m_vVisitFlag.size() - 1]->m_pInLog;*/
				
				CAirsideTaxiwayUtilizationDetailResult* pDetailResult = (CAirsideTaxiwayUtilizationDetailResult*)m_pParent;
				CTaxiwayUtilizationDetailData* pDetailData = pDetailResult->getResultItem(*taxiwayItem,pStartLog,pParam);

				if (pDetailData)
				{
					pDetailData->m_totalMovement++;
					long lOccupancyTime = ABS((pEndLog->time - pStartLog->time)/100);
					pDetailData->m_lOccupiedTime += (lOccupancyTime);

				//	double currentTime = fabs((double)(pEngLog1->time - pStartLog1->time)/100);

				    double dSpeed = 0.0;
					dSpeed = GetNodeSpeed(pStartLog->time,pEndLog->time,logEntry);

					pDetailData->m_statisticalTool.AddNewData(dSpeed);
					pDetailData->m_statisticalTool.SortData();
					pDetailData->m_minSpeed = (pDetailData->m_statisticalTool.GetMin());
					pDetailData->m_maxSpeed = (pDetailData->m_statisticalTool.GetMax());
					pDetailData->m_avaSpeed = (pDetailData->m_statisticalTool.GetAvarage());
					pDetailData->m_sigmaSpeed = (pDetailData->m_statisticalTool.GetSigma());

					TaxiwayUtilizationFlightInfo* flightInfo = pDetailData->GetFlightInfo(flightItem.mFltDesc.sAirline.c_str(),flightItem.mFltDesc.sAcType.c_str());
					if (flightInfo)
					{
						flightInfo->m_lMovements++;
						flightInfo->m_lOccupancy += static_cast<long>(lOccupancyTime);
						flightInfo->m_avaSpeed = (dSpeed + flightInfo->m_avaSpeed)/2;   
					}
					else
					{
						TaxiwayUtilizationFlightInfo flightInfo;  
						flightInfo.m_sAirline = flightItem.mFltDesc.sAirline.c_str();
						flightInfo.m_sActype = flightItem.mFltDesc.sAcType.c_str();
						flightInfo.m_lMovements = 1;
						flightInfo.m_avaSpeed = dSpeed;
						flightInfo.m_lOccupancy = static_cast<long>(lOccupancyTime);
						pDetailData->m_vFlightInfo.push_back(flightInfo);
					}
				}
				else
				{
					CTaxiwayUtilizationDetailData* item = new CTaxiwayUtilizationDetailData;
					CAirsideTaxiwayUtilizationDetailResult::GroupInterval groupSource;
					m_pParent->getIntervalTime(groupSource,pStartLog,pParam);
					item->m_sTaxiway = taxiwayItem->m_strTaxiwayName;
					item->m_intervalStart = groupSource.lStart;
					item->m_intervalEnd = groupSource.lEnd;
					item->m_sStartNode = taxiwayItem->m_vNodeItems[0].m_strName;
					item->m_sEndNode = taxiwayItem->m_vNodeItems[taxiwayItem->m_vNodeItems.size() - 1].m_strName;
					item->m_totalMovement = 1;
					item->m_lOccupiedTime = ABS((pEndLog->time - pStartLog->time)/100);

				//	double currentTime = fabs((double)(pEngLog1->time - pStartLog1->time)/100);
					
					double dSpeed = 0.0;

					dSpeed = GetNodeSpeed(pStartLog->time,pEndLog->time,logEntry);
					item->m_statisticalTool.AddNewData(dSpeed);
					item->m_statisticalTool.SortData();
					item->m_minSpeed = (item->m_statisticalTool.GetMin());
					item->m_maxSpeed = (item->m_statisticalTool.GetMax());
					item->m_avaSpeed = (item->m_statisticalTool.GetAvarage());
					item->m_sigmaSpeed = (item->m_statisticalTool.GetSigma());

					TaxiwayUtilizationFlightInfo flightInfo;  
					flightInfo.m_sAirline = flightItem.mFltDesc.sAirline.c_str();
					flightInfo.m_sActype = flightItem.mFltDesc.sAcType.c_str();
					flightInfo.m_lMovements = 1;
					flightInfo.m_lOccupancy = (pEndLog->time - pStartLog->time)/100;
					flightInfo.m_avaSpeed = dSpeed;
					item->m_vFlightInfo.push_back(flightInfo);
					m_pParent->m_vResult.push_back(item);
				}

			}

			
			if (!pCurLog->mEnterStand)
			{
				nFlag = 1;//need add end nodegroup to new taxiway result item
			}
			else
			{
				nFlag = 0;//no
			}
			m_bAreadyGenerateResult = true;
		}
		
	}

	return true;
}

bool CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::BuiltSummaryResult(AirsideFlightNodeLog *pNodeLog,const AirsideFlightLogItem& flightItem,AirsideFlightLogEntry& logEntry, CParameters *pParameter,std::vector<TaxiwayData*>& vResult,int& nFlag)
{
	CAirsideTaxiwayUtilizationPara *pParam = (CAirsideTaxiwayUtilizationPara *)pParameter;
	if (pParam == NULL)
		return false;


	FlightCrossNodeLog* pCurLog = (FlightCrossNodeLog*)pNodeLog;

	if (m_vVisitFlag.size() <= 1)
	{
		setGroupNode(pCurLog);
		return false;
	}

	if (m_bWaitForGenerationResult && !isVisit())
	{
		if(findNode(pCurLog))
		{
			setGroupNode(pCurLog);
			return false;
		}
		return true;
	}
	//get node name list
	std::vector<CString>vNodeNameList;
	for (int i = 0; i < (int)m_vVisitFlag.size(); i++)
	{
		FlightCrossNodeLog* pLog = m_vVisitFlag[i]->getNode(); 
		if (pLog && std::find(vNodeNameList.begin(),vNodeNameList.end(),pLog->mNodeName.c_str()) == vNodeNameList.end())
		{
			vNodeNameList.push_back(pLog->mNodeName.c_str());
		}
	}

	bool bOk = false;
	if (m_bWaitForGenerationResult && isVisit())
	{
		bOk = true;
	}

	if (std::find(vNodeNameList.begin(),vNodeNameList.end(),pNodeLog->mNodeName.c_str()) == vNodeNameList.end() && !bOk)
	{
		vNodeNameList.push_back(pNodeLog->mNodeName.c_str());
	}

	CTaxiwayItemList taxiwayList1;
	if (pParam->GetResultTaxiwayItemList().IfNodeListInSameTaxiwayAndGetTaxiwayList(vNodeNameList,taxiwayList1) && !pCurLog->mEnterStand&&!bOk)
	{
		setGroupNode(pCurLog);
		return false;
	}
	else
	{
		if (m_bWaitForGenerationResult == false)
		{
			m_bWaitForGenerationResult = true;
		}
		if (!isVisit())
		{
			if (!pCurLog->mEnterStand)
			{
				nFlag = 1;//need add end nodegroup to new taxiway result item
			}
			else
			{
				nFlag = 0;//no
			}
			return true;
		}

		CTaxiwayItemList taxiwayList2;
		if (!bOk)
		{
			vNodeNameList.pop_back();
		}

		if (!m_bAreadyGenerateResult)
		{
			CTaxiwayItemList taxiwayList2;
			pParam->GetResultTaxiwayItemList().IfNodeListInSameTaxiwayAndGetTaxiwayList(vNodeNameList,taxiwayList2);

			for (int j = 0; j < taxiwayList2.getItemCount(); j++)
			{
				CTaxiwayItem* taxiwayItem = taxiwayList2.at(j);

				FlightCrossNodeLog* pStartLog = (FlightCrossNodeLog*)m_vVisitFlag[0]->m_pInLog;
				FlightCrossNodeLog* pEndLog = (FlightCrossNodeLog*)m_vVisitFlag[m_vVisitFlag.size() - 1]->m_pOutLog;


// 				FlightCrossNodeLog* pStartLog1 = (FlightCrossNodeLog*)m_vVisitFlag[0]->m_pOutLog;
// 				FlightCrossNodeLog* pEngLog1 = (FlightCrossNodeLog*)m_vVisitFlag[m_vVisitFlag.size() - 1]->m_pInLog;

				CAirsideTaxiwayUtilizationSummaryResult* pSummaryResult = (CAirsideTaxiwayUtilizationSummaryResult*)m_pParent;
				TaxiwayData* pSummaryData = pSummaryResult->getResultItem(*taxiwayItem,pStartLog,vResult,pParam);

				if (pSummaryData)
				{
					pSummaryData->m_totalMovement++;
					double lOccupancyTime = fabs((double)(pEndLog->time - pStartLog->time)/100);
					pSummaryData->m_lOccupiedTime += static_cast<long>(lOccupancyTime);

				//	long currentTime = ABS(pEngLog1->time - pStartLog1->time)/100;

					double dSpeed = 0.0;
	
					dSpeed = GetNodeSpeed(pStartLog->time,pEndLog->time,logEntry);

					pSummaryData->m_statisticalTool.AddNewData(dSpeed);
					pSummaryData->m_statisticalTool.SortData();
					pSummaryData->m_avaSpeed = (pSummaryData->m_statisticalTool.GetAvarage());

				}
				else
				{
					TaxiwayData* item = new TaxiwayData;
					CAirsideTaxiwayUtilizationBaseResult::GroupInterval groupSource;
					m_pParent->getIntervalTime(groupSource,pStartLog,pParam);
					item->m_intervalStart = groupSource.lStart;
					item->m_intervalEnd = groupSource.lEnd;
					item->m_sTaxiway = taxiwayItem->m_strTaxiwayName;
					item->m_sStartNode = taxiwayItem->m_vNodeItems[0].m_strName;
					item->m_sEndNode = taxiwayItem->m_vNodeItems[taxiwayItem->m_vNodeItems.size() - 1].m_strName;
					item->m_totalMovement = 1;
					item->m_lOccupiedTime = ABS((pEndLog->time - pStartLog->time)/100);

			//		double currentTime = fabs((double)(pEngLog1->time - pStartLog1->time)/100);

					double dSpeed = 0.0;
					dSpeed = GetNodeSpeed(pStartLog->time,pEndLog->time,logEntry);
					item->m_statisticalTool.AddNewData(dSpeed);
					item->m_statisticalTool.SortData();
					item->m_avaSpeed = (item->m_statisticalTool.GetAvarage());

					vResult.push_back(item);
				}
				if (!pCurLog->mEnterStand)
				{
					nFlag = 1;//need add end nodegroup to new taxiway result item
				}
				else
				{
					nFlag = 0;//no
				}
				m_bAreadyGenerateResult = true;
			}
		}
		
	}
	return true;
}

void CAirsideTaxiwayUtilizationDetailResult::HandleIntersectionNode(AirsideFlightLogEntry& logEntry,AirsideFlightNodeLog *pNodeLog,const AirsideFlightLogItem& flightItem, CParameters *pParameter)
{
	if (m_vResultItem.empty())
	{
		TaxiwayResultItem* resultItem = new TaxiwayResultItem(this);
		resultItem->setGroupNode((FlightCrossNodeLog*)pNodeLog);
		m_vResultItem.push_back(resultItem);
	}

	while(!m_vResultItem.empty())
	{
		FlightCrossNodeLog* pCurLog = (FlightCrossNodeLog*)pNodeLog;
		CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem* item = m_vResultItem.front();
		int nFlag = 0;
		if (item->m_bAreadyGenerateResult)
		{
			m_vResultItem.pop_front();
			delete item;
			continue;
		}

		if (item->BuiltDetailResult(pNodeLog,flightItem,logEntry,pParameter,nFlag))
		{
			if(item->exsit((FlightCrossNodeLog*)pNodeLog))
				break;

			std::deque<TaxiwayResultItem*> vResult = m_vResultItem;
			vResult.pop_front();
			int nIndex = 1;

			int nCurFlag = 0;
			if (vResult.empty())
			{
				nCurFlag = nFlag;
			}
		
			while (!vResult.empty())
			{
				TaxiwayResultItem* nextItem = vResult.front();
			
				int nNextFlag = 0;
				if(nextItem->BuiltDetailResult(pNodeLog,flightItem,logEntry,pParameter,nNextFlag))
				{
					nCurFlag = nNextFlag;
					vResult.pop_front();
					continue;
				}
				return;
			}

			TaxiwayResultItem* backItem = m_vResultItem.back();
			TaxiwayResultItem* newResultItem = new TaxiwayResultItem(this);
			if (nCurFlag)
			{
				int nCount = backItem->m_vVisitFlag.size();
				if (nCount == 0)
				{
					break;
				}
				CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::GroupNodeInOut* groupNode = backItem->m_vVisitFlag[nCount-1];
				newResultItem->m_vVisitFlag.push_back(groupNode);

			}
			newResultItem->setGroupNode((FlightCrossNodeLog*)pNodeLog);
			m_vResultItem.push_back(newResultItem);
			
		}
		break;
	}
}


CTaxiwayUtilizationDetailData* CAirsideTaxiwayUtilizationDetailResult::getResultItem(CTaxiwayItem& taxiwayItem,AirsideFlightNodeLog *pNodeLog,CParameters *pParameter)
{
	if (pParameter == NULL)
	{
		return NULL;
	}

	for (int i = 0; i < (int)m_vResult.size(); i++)
	{
		CTaxiwayUtilizationDetailData* item = static_cast<CTaxiwayUtilizationDetailData*>(m_vResult[i]);
		GroupInterval grouSource;
		getIntervalTime(grouSource,pNodeLog,pParameter);
		CString sStartNode(_T(""));
		CString sEndNode(_T(""));

		int nSize = (int)taxiwayItem.m_vNodeItems.size();
		if (nSize == 0)
		{
			return NULL;
		}

		sStartNode = taxiwayItem.m_vNodeItems[0].m_strName;
		sEndNode = taxiwayItem.m_vNodeItems[nSize - 1].m_strName;
		if (item->m_sTaxiway == taxiwayItem.m_strTaxiwayName && item->m_sStartNode == sStartNode &&
			item->m_sEndNode == sEndNode && grouSource.lStart == item->m_intervalStart &&
			grouSource.lEnd == item->m_intervalEnd)
		{
			return item;
		}
	}
	return NULL;
}

void CAirsideTaxiwayUtilizationDetailResult::RefreshReport(CParameters * parameter)
{
	CAirsideTaxiwayUtilizationPara* pParam = (CAirsideTaxiwayUtilizationPara*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case TAXIWAY_UTILIZATION_SPEED:
		{
			m_pChartResult = new CTaxiwayUtilizationDetailSpeedChart;
		}
		break;
	case TAXIWAY_UTILIZATION_OCCUPANCYTIME:
		{
			m_pChartResult = new CTaxiwayUtilizationDetailOccupancyChart;
		}
		break;
	case TAXIWAY_UTILIZATION_MOVEMENT:
		{
			m_pChartResult = new CTaxiwayUtilizationDetailMovementChart;
		}
		break;
	default:
		break;
	}

	if (m_pChartResult)
	{
		m_pChartResult->GenerateResult(m_vResult, parameter);
	}
}

TaxiwayFlightInfoData* CAirsideTaxiwayUtilizationDetailResult::exsitFlightInfo(const TaxiwayFlightInfoData* pData)
{
	for (int i = 0; i < (int)m_vFlightInfoList.size(); i++)
	{
		TaxiwayFlightInfoData* infoData = m_vFlightInfoList[i];
		if (*infoData == *pData)
		{
			return infoData;
		}
	}
	return NULL;
}

void CAirsideTaxiwayUtilizationDetailResult::BuiltFlightInfoList()
{
	m_vFlightInfoList.clear();
	for (int i = 0; i < (int)m_vResult.size(); i++)
	{
		CTaxiwayUtilizationDetailData* pData = static_cast<CTaxiwayUtilizationDetailData*>(m_vResult[i]);

		TaxiwayFlightInfoData* infoData = new TaxiwayFlightInfoData;
		infoData->m_sTaxiway = pData->m_sTaxiway;
		infoData->m_sStartNode = pData->m_sStartNode;
		infoData->m_sEndNode = pData->m_sEndNode;

		TaxiwayFlightInfoData* pExsitData = exsitFlightInfo(infoData);
		if (pExsitData == NULL)
		{
			infoData->m_vFlightInfoList.insert(infoData->m_vFlightInfoList.end(),pData->m_vFlightInfo.begin(),pData->m_vFlightInfo.end());
			m_vFlightInfoList.push_back(infoData);
		}
		else
		{
			for (int j = 0; j < (int)pData->m_vFlightInfo.size(); j++)
			{
				TaxiwayUtilizationFlightInfo& flightData = pData->m_vFlightInfo[j];
				FlightInfoList::iterator infoIter = std::find(pExsitData->m_vFlightInfoList.begin(),pExsitData->m_vFlightInfoList.end(),flightData);

				if (infoIter != pExsitData->m_vFlightInfoList.end())
				{
					(*infoIter).m_lMovements += flightData.m_lMovements;
				}
				else
				{
					pExsitData->m_vFlightInfoList.push_back(flightData);
				}
			}
		}
	}

	for (int i = 0; i < (int)m_vFlightInfoList.size(); i++)
	{
		TaxiwayFlightInfoData* infoData = m_vFlightInfoList.at(i);
		infoData->SortData();
	}
}

TaxiwayFlightInfoData* CAirsideTaxiwayUtilizationDetailResult::getFlightInfoData(CTaxiwayUtilizationDetailData* pDetail)
{
	for (int i = 0; i < (int)m_vFlightInfoList.size(); i++)
	{
		TaxiwayFlightInfoData* infoData = m_vFlightInfoList[i];
		if (infoData->m_sTaxiway == pDetail->m_sTaxiway && infoData->m_sStartNode == pDetail->m_sStartNode &&
			infoData->m_sEndNode == pDetail->m_sEndNode)
		{
			return infoData;
		}
	}
	return NULL;
}

bool compareDetailData(const CTaxiwayUtilizationData* data1,const CTaxiwayUtilizationData* data2)
{
	return *data1 < *data2;
}

void CAirsideTaxiwayUtilizationDetailResult::GenerateAllDetailaData()
{

	std::vector<TaxiwayData*> vTaxiwayData;
	for (int i = 0; i < (int)m_vResult.size(); i++)
	{
		CTaxiwayUtilizationDetailData* pData = static_cast<CTaxiwayUtilizationDetailData*>(m_vResult[i]);

		TaxiwayData* data = new TaxiwayData;
		data->m_sTaxiway = pData->m_sTaxiway;
		data->m_sStartNode = pData->m_sStartNode;
		data->m_sEndNode = pData->m_sEndNode;
		vTaxiwayData.push_back(data);
	}

	std::vector<std::pair<CString,std::pair<CString,CString> > > vTaxiway = GetTaxiwayResultCount(vTaxiwayData);

	for (int i = 0; i < (int)vTaxiwayData.size(); i++)
	{
		delete vTaxiwayData[i];
	}

	for (int i = 0; i < (int)vTaxiway.size(); i++)
	{
		std::pair<CString,std::pair<CString,CString> >& item = vTaxiway[i];
		for (int j = 0; j < (int)m_vGroup.size(); j++)
		{
			CTaxiwayUtilizationDetailData detailData;
			detailData.m_sTaxiway = item.first;
			detailData.m_sStartNode = item.second.first;
			detailData.m_sEndNode = item.second.second;
			detailData.m_intervalStart = m_vGroup[j].lStart;
			detailData.m_intervalEnd = m_vGroup[j].lEnd;

			if (!haveData(detailData))
			{
				m_vResult.push_back(new CTaxiwayUtilizationDetailData(detailData));
			}
		}
	}
	std::sort(m_vResult.begin(),m_vResult.end(),compareDetailData);
}

bool CAirsideTaxiwayUtilizationDetailResult::haveData(CTaxiwayUtilizationDetailData& data)
{
	for (int i = 0; i < (int)m_vResult.size(); i++)
	{
		CTaxiwayUtilizationDetailData* pData = static_cast<CTaxiwayUtilizationDetailData*>(m_vResult[i]);

		if (data.m_sTaxiway == pData->m_sTaxiway && data.m_sStartNode == pData->m_sStartNode &&
			data.m_sEndNode == pData->m_sEndNode && data.m_intervalStart == pData->m_intervalStart &&
			data.m_intervalEnd == pData->m_intervalEnd)
		{
			return true;
		}
	}
	return false;
}

void CAirsideTaxiwayUtilizationDetailResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	GenerateAllDetailaData();
	BuiltFlightInfoList();

	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		CTaxiwayUtilizationDetailData* pDetailData = static_cast<CTaxiwayUtilizationDetailData*>(m_vResult[i]);
		//T interval start
		{
			CString sStart(_T(""));
			ElapsedTime startTime(pDetailData->m_intervalStart);
			sStart.Format(_T("%02d:%02d"),startTime.GetHour(),startTime.GetMinute());
			cxListCtrl.InsertItem(i,sStart);
		}

		//T interval End
		{
			CString sEnd(_T(""));
			ElapsedTime endTime(pDetailData->m_intervalEnd);
			sEnd.Format(_T("%02d:%02d"),endTime.GetHour(),endTime.GetMinute());
			cxListCtrl.SetItemText(i,1,sEnd);
		}

		//Taxiway
		{
			CString sTaxiway(_T(""));
			cxListCtrl.SetItemText(i,2,pDetailData->m_sTaxiway);
		}

		//From
		{
			cxListCtrl.SetItemText(i,3,pDetailData->m_sStartNode);
		}

		//To
		{
			cxListCtrl.SetItemText(i,4,pDetailData->m_sEndNode);
		}

		//Total movements
		{
			CString sTotal(_T(""));
			sTotal.Format(_T("%d"),pDetailData->m_totalMovement);
			cxListCtrl.SetItemText(i,5,sTotal);
		}

		//V average speed
		{
			CString sAverage(_T(""));
			sAverage.Format(_T("%.2f"),pDetailData->m_avaSpeed);
			cxListCtrl.SetItemText(i,6,sAverage);
		}

		//V minimum speed
		{
			CString sMin(_T(""));
			sMin.Format(_T("%.2f"),pDetailData->m_minSpeed);
			cxListCtrl.SetItemText(i,7,sMin);
		}	

		//V Maximum speed
		{
			CString sMax(_T(""));
			sMax.Format(_T("%.2f"),pDetailData->m_maxSpeed);
			cxListCtrl.SetItemText(i,8,sMax);
		}

		//V Sigma speed
		{
			CString sSigma(_T(""));
			sSigma.Format(_T("%.2f"),pDetailData->m_sigmaSpeed);
			cxListCtrl.SetItemText(i,9,sSigma);
		}

		//Time occupied
		{
			CString sOccupancy(_T(""));
			ElapsedTime occupancyTime(pDetailData->m_lOccupiedTime);
			sOccupancy.Format(_T("%02d:%02d:%02d"),occupancyTime.GetHour(),occupancyTime.GetMinute(),occupancyTime.GetSecond());
			cxListCtrl.SetItemText(i,10,sOccupancy);
		}

		TaxiwayFlightInfoData* pInfoData = getFlightInfoData(pDetailData);
		if (pInfoData)
		{
			int iColumn = 0;
			for (int j = 0; j < (int)pInfoData->m_vFlightInfoList.size() && j < 20; j++)
			{
				TaxiwayUtilizationFlightInfo item = pInfoData->m_vFlightInfoList[j];

				if (std::find(pDetailData->m_vFlightInfo.begin(),pDetailData->m_vFlightInfo.end(),item) != pDetailData->m_vFlightInfo.end())
				{
					FlightInfoList::iterator iter = std::find(pDetailData->m_vFlightInfo.begin(),pDetailData->m_vFlightInfo.end(),item);
					item.m_avaSpeed = (*iter).m_avaSpeed;
					item.m_lMovements = (*iter).m_lMovements;
					item.m_lOccupancy = (*iter).m_lOccupancy;

					//Airline 
					{
						cxListCtrl.SetItemText(i,11+4*iColumn,item.m_sAirline);
					}

					//actype
					{
						cxListCtrl.SetItemText(i,12+4*iColumn,item.m_sActype);
					}

					//Number of movements
					{
						CString sMovement(_T(""));
						sMovement.Format(_T("%d"),item.m_lMovements);
						cxListCtrl.SetItemText(i,13+4*iColumn,sMovement);
					}

					//V average (knots)
					{
						CString sAverageSpeed(_T(""));
						sAverageSpeed.Format(_T("%.2f"),item.m_avaSpeed);
						cxListCtrl.SetItemText(i,14+4*iColumn,sAverageSpeed);
					}

					iColumn++;
				}
				
			}
		}
	}
}

void CAirsideTaxiwayUtilizationDetailResult::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("T interval start"), LVCFMT_LEFT, 50);	
	cxListCtrl.InsertColumn(1, _T("T interval End"), LVCFMT_LEFT, 80);
	cxListCtrl.InsertColumn(2, _T("Taxiway"), LVCFMT_LEFT, 50);
	cxListCtrl.InsertColumn(3, _T("From"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(4, _T("To"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(5, _T("Total movements"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(6, _T("V average"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(7, _T("V minimum"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(8, _T("V Maximum"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(9,_T("V sigma"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(10, _T("Time occupied"),LVCFMT_LEFT,60);

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtTIME);
		piSHC->SetDataType(1,dtTIME);
		piSHC->SetDataType(2,dtSTRING);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtSTRING);
		piSHC->SetDataType(5,dtINT);
		piSHC->SetDataType(6,dtDEC);

		piSHC->SetDataType(7,dtDEC);
		piSHC->SetDataType(8,dtDEC);
		piSHC->SetDataType(9,dtDEC);
		piSHC->SetDataType(10,dtTIME);
	}

	for (int i = 0; i < 20; i++)
	{
		CString strAirline(_T(""));
		CString strActype(_T(""));
		strAirline.Format(_T("Airline %d"),i+1);
		strActype.Format(_T("AC type %d"),i+1);

		cxListCtrl.InsertColumn(11 + 4*i, strAirline,LVCFMT_LEFT,60);
		piSHC->SetDataType(11 + i,dtSTRING);
		cxListCtrl.InsertColumn(12 + 4*i, strActype,LVCFMT_LEFT,60);
		piSHC->SetDataType(12 + i,dtSTRING);
		cxListCtrl.InsertColumn(13 + 4*i, _T("Number of movements"),LVCFMT_LEFT,100);
		piSHC->SetDataType(13 + i,dtINT);
		cxListCtrl.InsertColumn(14 + 4*i, _T("V average (knots)"),LVCFMT_LEFT,80);
		piSHC->SetDataType(14 + i,dtDEC);
	}
}


//////////////summary result///////////////////////////////////////////////////////////////////
CAirsideTaxiwayUtilizationSummaryResult::CAirsideTaxiwayUtilizationSummaryResult()
{

}

CAirsideTaxiwayUtilizationSummaryResult::~CAirsideTaxiwayUtilizationSummaryResult()
{

}

void CAirsideTaxiwayUtilizationSummaryResult::GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter)
{
	for (int n = 0; n < (int)m_vResult.size(); n++)
	{
		delete m_vResult[n];
	}
	m_vResult.clear();
	if (pParameter == NULL)
		return;
	CAirsideTaxiwayUtilizationPara* pParam = (CAirsideTaxiwayUtilizationPara*)pParameter;
	if(pParam == NULL)
		return;

	//read old log
	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);

	strDescFilepath = (*pCBGetLogFilePath)(AirsideFlightDescFileReport);
	strEventFilePath = (*pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;


	ElapsedTime eStartTime = pParam->getStartTime();
	ElapsedTime eEndTime = pParam->getEndTime();

	GenerateIntervalTime(pParameter);

	AirsideReportLog::AirsideFlightLogData mFlightLogData;
	mFlightLogData.OpenInput(strDescFilepath.GetString() ,strEventFilePath.GetString());
	int nCount = mFlightLogData.getItemCount();
	CProgressBar progressBar( "Generating report ", 100, nCount, TRUE, 1 );

	pParam->InitTaxiNodeIndex();

	CTaxiwayItemList& listData = pParam->GetResultTaxiwayItemList();
	listData.InitTaxiNodeIndex();

	std::vector<TaxiwayData*> vResult;
	for (int i = 0; i < nCount; i++)
	{
		progressBar.StepIt();

		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);
		int nLogCount = (int)item.mpEventList->mEventList.size();

		//old log
		AirsideFlightLogEntry logEntry;
		logEntry.SetEventLog(&airsideFlightEventLog);
		fltLog.GetItemByID(logEntry,item.mFltDesc.id);

		bool bFind = false;
		for (int j = 0; j < nLogCount; j++)
		{
			ARCEventLog* pLog = item.mpEventList->mEventList.at(j);
			if(pLog == NULL)
				continue;

			if(pLog->time < eStartTime.getPrecisely())
				continue;
			if(pLog->time > eEndTime.getPrecisely())
				break;

			if(pLog->IsA(typeof(AirsideFlightStandOperationLog)))
			{
				AirsideFlightStandOperationLog* pStandLog = (AirsideFlightStandOperationLog*)pLog;
				if (pStandLog->m_eOpType == AirsideFlightStandOperationLog::LeavingStand)
				{
					bFind = true;
				}
			}

			if(pLog->IsA(typeof(FlightCrossNodeLog)))
			{
				FlightCrossNodeLog *pNodeLog = (FlightCrossNodeLog *)pLog;
				if (pNodeLog && (pNodeLog->meType == FlightCrossNodeLog::OUT_NODE || pNodeLog->meType == FlightCrossNodeLog::IN_NODE))
				{
					if(!pParam->GetResultTaxiwayItemList().IsNodeSelected(pNodeLog->mNodeId))
					{
						while (!m_vResultItem.empty())
						{
							TaxiwayResultItem* lastItem = m_vResultItem.front();
							
							int nFlag = 0;
							if(lastItem->m_vVisitFlag.size() <= 1 && pNodeLog->meType == FlightCrossNodeLog::IN_NODE)
							{
								m_vResultItem.pop_front();
								delete lastItem;
								continue;
							}

							if (lastItem->isVisit())
							{
								lastItem->m_bWaitForGenerationResult = true;
								int nCount = lastItem->m_vVisitFlag.size();
								TaxiwayResultItem::GroupNodeInOut* pNodeGroup = lastItem->m_vVisitFlag[nCount-1];
								if (pNodeGroup->m_pInLog)
								{
									lastItem->BuiltSummaryResult(pNodeGroup->m_pInLog,item,logEntry,pParameter,vResult,nFlag);
								}
								else if (pNodeGroup->m_pOutLog)
								{
									lastItem->BuiltSummaryResult(pNodeGroup->m_pOutLog,item,logEntry,pParameter,vResult,nFlag);
								}
								m_vResultItem.pop_front();
								delete lastItem;

								continue;
							}
							
							break;
						}
						
					}
					else
					{
						if (bFind == true)
						{
							pNodeLog->mEnterStand = 1;
							bFind = false;
						}
						HandleSummaryData(logEntry,pNodeLog,item,vResult,pParam);
					}
				}
			}
		}

		while (!m_vResultItem.empty())
		{
			TaxiwayResultItem* lastItem = m_vResultItem.front();

			int nFlag = 0;
			if(lastItem->m_vVisitFlag.size() <= 1)
			{
				m_vResultItem.pop_front();
				delete lastItem;
				continue;
			}

			if (lastItem->isVisit())
			{
				lastItem->m_bWaitForGenerationResult = true;
				int nCount = lastItem->m_vVisitFlag.size();
				TaxiwayResultItem::GroupNodeInOut* pNodeGroup = lastItem->m_vVisitFlag[nCount-1];
				if (pNodeGroup->m_pInLog)
				{
					lastItem->BuiltSummaryResult(pNodeGroup->m_pInLog,item,logEntry,pParameter,vResult,nFlag);
				}
				else if (pNodeGroup->m_pOutLog)
				{
					lastItem->BuiltSummaryResult(pNodeGroup->m_pOutLog,item,logEntry,pParameter,vResult,nFlag);
				}
			}
			m_vResultItem.pop_front();
			delete lastItem;
		}
	}

	std::vector<std::pair<CString,std::pair<CString,CString> > > vTaxiway = GetTaxiwayResultCount(vResult);
	int nResultCount = (int)vTaxiway.size();
	for (int i = 0; i < nResultCount; i++)
	{
		CTaxiwayUtilizationSummaryData* pSummaryData = new CTaxiwayUtilizationSummaryData();
		std::pair<CString,std::pair<CString,CString> > item = vTaxiway[i];
		pSummaryData->m_sTaxiway = item.first;
		pSummaryData->m_sStartNode = item.second.first;
		pSummaryData->m_sEndNode = item.second.second;

		InitStaticData(pParam,TAXIWAY_UTILIZATION_SPEED,pSummaryData,vResult);
		InitStaticData(pParam,TAXIWAY_UTILIZATION_OCCUPANCYTIME,pSummaryData,vResult);
		InitStaticData(pParam,TAXIWAY_UTILIZATION_MOVEMENT,pSummaryData,vResult);
		m_vResult.push_back(pSummaryData);
	}

	for (int i = 0; i < (int)vResult.size(); i++)
	{
		delete vResult[i];
	}
	vResult.clear();
}

long CAirsideTaxiwayUtilizationSummaryResult::GetMovements(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const CTaxiwayUtilizationSummaryData* item,std::vector<TaxiwayData*>&vResult)
{
	long lMovements = 0;
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		TaxiwayData* data = vResult[i];
		if (data->m_sTaxiway == item->m_sTaxiway && data->m_sStartNode == item->m_sStartNode
			&& data->m_sEndNode == item->m_sEndNode
			&& startTime.asSeconds() == data->m_intervalStart)
		{
			lMovements += data->m_totalMovement;
		}
	}
	return lMovements;
}

double CAirsideTaxiwayUtilizationSummaryResult::GetSpeed(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const CTaxiwayUtilizationSummaryData* item,std::vector<TaxiwayData*>&vResult)
{
	double dSpeed = 0.0;
	int nIndex = 0;
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		TaxiwayData* data = vResult[i];
		if (data->m_sTaxiway == item->m_sTaxiway && data->m_sStartNode == item->m_sStartNode
			&& data->m_sEndNode == item->m_sEndNode
			&& startTime.asSeconds() == data->m_intervalStart)
		{
			dSpeed += data->m_avaSpeed;
			nIndex++;
		}
	}

	if (nIndex != 0)
	{
		return (dSpeed/nIndex);
	}

	return dSpeed;
}

long CAirsideTaxiwayUtilizationSummaryResult::GetOccupancyTime(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const CTaxiwayUtilizationSummaryData* item,std::vector<TaxiwayData*>&vResult)
{
	long lTime = 0;
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		TaxiwayData* data = vResult[i];
		if (data->m_sTaxiway == item->m_sTaxiway && data->m_sStartNode == item->m_sStartNode
			&& data->m_sEndNode == item->m_sEndNode
			&& startTime.asSeconds() == data->m_intervalStart)
		{
			lTime += data->m_lOccupiedTime;
		}
	}
	return lTime;
}

void CAirsideTaxiwayUtilizationSummaryResult::InitStaticData(CParameters* pPara,subReportType emType,CTaxiwayUtilizationSummaryData* item,std::vector<TaxiwayData*>& vResult)
{
	CStatisticalTools<double> statisticalTool;

	int nCount = vResult.size();
	if(nCount == 0)
		return;

	ElapsedTime eStartTime = pPara->getStartTime();
	ElapsedTime eEndTime = pPara->getEndTime();
	ElapsedTime eInterval(pPara->getInterval());
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60 * 60L);
	}

	long lMaxmoveMent = 0;
	double dMaxSpeed = 0.0;
	long lMaxOccupancyTime = 0;
	long lMaxInterval = 0;
	int nIndex = 0;
	for (;eStartTime < eEndTime + eInterval; eStartTime += eInterval)
	{
		if(emType == TAXIWAY_UTILIZATION_SPEED)
		{
			double dSpeed = GetSpeed(eStartTime,eEndTime,eInterval,item,vResult);
			if (dMaxSpeed < dSpeed)
			{
				lMaxInterval = nIndex;
			}
			statisticalTool.AddNewData(dSpeed);
		}
		else if(emType == TAXIWAY_UTILIZATION_OCCUPANCYTIME)
		{
			long lOccupancyTime = GetOccupancyTime(eStartTime,eEndTime,eInterval,item,vResult);
			if (lMaxOccupancyTime < lOccupancyTime)
			{
				lMaxInterval = nIndex;
			}
			statisticalTool.AddNewData(lOccupancyTime);
		}
		else if (emType == TAXIWAY_UTILIZATION_MOVEMENT)
		{
			long lMovements = GetMovements(eStartTime,eEndTime,eInterval,item,vResult);
			if (lMaxmoveMent < lMovements)
			{
				lMaxInterval = nIndex;
			}
			statisticalTool.AddNewData(lMovements);
		}
		nIndex++;
	}
	statisticalTool.SortData();

	if (emType == TAXIWAY_UTILIZATION_SPEED)
	{
		item->m_speedData.m_lIntervalOfMax = lMaxInterval;
		item->m_speedData.m_dTotalValue = statisticalTool.GetAvarage();
		InitToolData(item->m_speedData,statisticalTool);
	}
	else if(emType == TAXIWAY_UTILIZATION_OCCUPANCYTIME)
	{
		item->m_occupancyData.m_lIntervalOfMax = lMaxInterval;
		item->m_occupancyData.m_dTotalValue = statisticalTool.GetAvarage();
		InitToolData(item->m_occupancyData,statisticalTool);
	}
	else if (emType == TAXIWAY_UTILIZATION_MOVEMENT)
	{
		item->m_movementData.m_lIntervalOfMax = lMaxInterval;
		item->m_movementData.m_dTotalValue = statisticalTool.GetAvarage();
		InitToolData(item->m_movementData,statisticalTool);
	}

}

void CAirsideTaxiwayUtilizationSummaryResult::InitToolData(TaxiwayUtilzationSummaryData& data,CStatisticalTools<double>& statisticalTool)
{
	statisticalTool.SortData();
	data.m_dMinValue = (statisticalTool.GetMin());
	data.m_dMaxValue = (statisticalTool.GetMax());
	data.m_dVerageValue = (statisticalTool.GetAvarage());
	data.m_statisticalTool.m_dQ1 = (statisticalTool.GetPercentile(25));
	data.m_statisticalTool.m_dQ2 = (statisticalTool.GetPercentile(50));
	data.m_statisticalTool.m_dQ3 = (statisticalTool.GetPercentile(75));
	data.m_statisticalTool.m_dP1 = (statisticalTool.GetPercentile(1));
	data.m_statisticalTool.m_dP5 = (statisticalTool.GetPercentile(5));
	data.m_statisticalTool.m_dP10 = (statisticalTool.GetPercentile(10));
	data.m_statisticalTool.m_dP90 = (statisticalTool.GetPercentile(90));
	data.m_statisticalTool.m_dP95 = (statisticalTool.GetPercentile(95));
	data.m_statisticalTool.m_dP99 = (statisticalTool.GetPercentile(99));
	data.m_statisticalTool.m_dSigma = (statisticalTool.GetSigma());
}

TaxiwayData* CAirsideTaxiwayUtilizationSummaryResult::getResultItem(CTaxiwayItem& taxiwayItem,AirsideFlightNodeLog *pNodeLog,std::vector<TaxiwayData*>& vResult,CParameters *pParameter)
{
	if (pParameter == NULL)
	{
		return NULL;
	}

	for (int i = 0; i < (int)vResult.size(); i++)
	{
		TaxiwayData* item = vResult[i];
		GroupInterval grouSource;
		getIntervalTime(grouSource,pNodeLog,pParameter);
		CString sStartNode(_T(""));
		CString sEndNode(_T(""));

		int nSize = (int)taxiwayItem.m_vNodeItems.size();
		if (nSize == 0)
		{
			return NULL;
		}

		sStartNode = taxiwayItem.m_vNodeItems[0].m_strName;
		sEndNode = taxiwayItem.m_vNodeItems[nSize - 1].m_strName;
		if (item->m_sTaxiway == taxiwayItem.m_strTaxiwayName && item->m_sStartNode == sStartNode &&
			item->m_sEndNode == sEndNode && grouSource.lStart == item->m_intervalStart &&
			grouSource.lEnd == item->m_intervalEnd)
		{
			return item;
		}
	}
	return NULL;
}

void CAirsideTaxiwayUtilizationSummaryResult::HandleSummaryData(AirsideFlightLogEntry& logEntry,AirsideFlightNodeLog *pNodeLog,const AirsideFlightLogItem& flightItem,std::vector<TaxiwayData*>& vResult, CParameters *pParameter)
{
	if (m_vResultItem.empty())
	{
		TaxiwayResultItem* resultItem = new TaxiwayResultItem(this);
		resultItem->setGroupNode((FlightCrossNodeLog*)pNodeLog);
		m_vResultItem.push_back(resultItem);
	}

	while(!m_vResultItem.empty())
	{
		FlightCrossNodeLog* pCurLog = (FlightCrossNodeLog*)pNodeLog;
		CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem* item = m_vResultItem.front();
		int nFlag = 0;
		if (item->m_bAreadyGenerateResult)
		{
			m_vResultItem.pop_front();
			delete item;
			continue;
		}

		if (item->BuiltSummaryResult(pNodeLog,flightItem,logEntry,pParameter,vResult,nFlag))
		{
			if(item->exsit((FlightCrossNodeLog*)pNodeLog))
				break;

			std::deque<TaxiwayResultItem*> vSummaryResult = m_vResultItem;
			vSummaryResult.pop_front();

			int nCurFlag = 0;
			if (vSummaryResult.empty())
			{
				nCurFlag = nFlag;
			}

			while (!vSummaryResult.empty())
			{
				TaxiwayResultItem* nextItem = vSummaryResult.front();

				int nNextFlag = 0;
				if(nextItem->BuiltSummaryResult(pNodeLog,flightItem,logEntry,pParameter,vResult,nNextFlag))
				{
					nCurFlag = nNextFlag;
					vSummaryResult.pop_front();
					continue;
				}
				else
				{
					return;
				}
			}

			TaxiwayResultItem* backItem = m_vResultItem.back();
			TaxiwayResultItem* newResultItem = new TaxiwayResultItem(this);
			if (nCurFlag)
			{
				int nCount = backItem->m_vVisitFlag.size();
				if (nCount == 0)
				{
					break;
				}
				CAirsideTaxiwayUtilizationBaseResult::TaxiwayResultItem::GroupNodeInOut* groupNode = backItem->m_vVisitFlag[nCount-1];
				newResultItem->m_vVisitFlag.push_back(groupNode);

			}
			newResultItem->setGroupNode((FlightCrossNodeLog*)pNodeLog);
			m_vResultItem.push_back(newResultItem);

		}
		break;
	}
}

void CAirsideTaxiwayUtilizationSummaryResult::RefreshReport(CParameters * parameter)
{
	CAirsideTaxiwayUtilizationPara* pParam = (CAirsideTaxiwayUtilizationPara*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case TAXIWAY_UTILIZATION_SPEED:
		{
			m_pChartResult = new CTaxiwayUtilizationSummarySpeedChart;
		}
		break;
	case TAXIWAY_UTILIZATION_OCCUPANCYTIME:
		{
			m_pChartResult = new CTaxiwayUtilizationSummaryOccupancyChart;
		}
		break;
	case TAXIWAY_UTILIZATION_MOVEMENT:
		{
			m_pChartResult = new CTaxiwayUtilizationSummaryMovementChart;
		}
		break;
	default:
		break;
	}

	if (m_pChartResult)
	{
		m_pChartResult->GenerateResult(m_vResult, parameter);
	}
}

void CAirsideTaxiwayUtilizationSummaryResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		CTaxiwayUtilizationSummaryData* pSummaryData = static_cast<CTaxiwayUtilizationSummaryData*>(m_vResult[i]);

		//Taxiway
		{
			cxListCtrl.InsertItem(i,pSummaryData->m_sTaxiway);
		}

		//From intersection
		{
			cxListCtrl.SetItemText(i,1,pSummaryData->m_sStartNode);
		}

		//To intersection
		{
			cxListCtrl.SetItemText(i,2,pSummaryData->m_sEndNode);
		}

		//Min movements per interval
		{
			CString sMinMovements(_T(""));
			sMinMovements.Format(_T("%.f"),pSummaryData->m_movementData.m_dMinValue);
			cxListCtrl.SetItemText(i,3,sMinMovements);
		}

		//Interval with min movements
// 		{
// 			CString sMinIntervalMovements(_T(""));
// 			sMinIntervalMovements.Format(_T("%d"),pSummaryData->m_movementData.m_lIntervalOfMin);
// 			cxListCtrl.SetItemText(i,4,sMinIntervalMovements);
// 		}

		//Mean movements per interval
		{
			CString sMeanMovements(_T(""));
			sMeanMovements.Format(_T("%.2f"),pSummaryData->m_movementData.m_dTotalValue);
			cxListCtrl.SetItemText(i,4,sMeanMovements);
		}

		//Max movement per interval
		{
			CString sMaxMovements(_T(""));
			sMaxMovements.Format(_T("%.f"),pSummaryData->m_movementData.m_dMaxValue);
			cxListCtrl.SetItemText(i,5,sMaxMovements);
		}

		//Interval with max movements
		{
			CString sMaxIntervalMovements(_T(""));
			sMaxIntervalMovements.Format(_T("%d"),pSummaryData->m_movementData.m_lIntervalOfMax);
			cxListCtrl.SetItemText(i,6,sMaxIntervalMovements);
		}

		//Q1
		{
			CString sQ1Movements(_T(""));
			sQ1Movements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dQ1);
			cxListCtrl.SetItemText(i,7,sQ1Movements);
		}
		
		//Q2
		{
			CString sQ2Movements(_T(""));
			sQ2Movements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dQ2);
			cxListCtrl.SetItemText(i,8,sQ2Movements);
		}

		//Q3
		{
			CString sQ3Movements(_T(""));
			sQ3Movements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dQ3);
			cxListCtrl.SetItemText(i,9,sQ3Movements);
		}

		//p1
		{
			CString sP1Movements(_T(""));
			sP1Movements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dP1);
			cxListCtrl.SetItemText(i,10,sP1Movements);
		}

		//p5
		{
			CString sP5Movements(_T(""));
			sP5Movements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dP5);
			cxListCtrl.SetItemText(i,11,sP5Movements);
		}

		//p10
		{
			CString sP10Movements(_T(""));
			sP10Movements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dP10);
			cxListCtrl.SetItemText(i,12,sP10Movements);
		}

		//p90
		{
			CString sP90Movements(_T(""));
			sP90Movements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dP90);
			cxListCtrl.SetItemText(i,13,sP90Movements);
		}

		//p95
		{
			CString sP95Movements(_T(""));
			sP95Movements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dP95);
			cxListCtrl.SetItemText(i,14,sP95Movements);
		}

		//p99
		{
			CString sP99Movements(_T(""));
			sP99Movements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dP99);
			cxListCtrl.SetItemText(i,15,sP99Movements);
		}

		//std dev
		{
			CString sStdMovements(_T(""));
			sStdMovements.Format(_T("%.2f"),pSummaryData->m_movementData.m_statisticalTool.m_dSigma);
			cxListCtrl.SetItemText(i,16,sStdMovements);
		}

		//V min per interval
		{
			CString sMinSpeed(_T(""));
			sMinSpeed.Format(_T("%.2f"),pSummaryData->m_speedData.m_dMinValue);
			cxListCtrl.SetItemText(i,17,sMinSpeed);
		}

		//Interval with V min
		{
// 			CString sIntervalMinSpeed(_T(""));
// 			sIntervalMinSpeed.Format(_T("%d"),pSummaryData->m_speedData.m_lIntervalOfMin);
// 			cxListCtrl.SetItemText(i,18,sIntervalMinSpeed);
		}

		//V mean per interval
		{
			CString sMeanSpeed(_T(""));
			sMeanSpeed.Format(_T("%.2f"),pSummaryData->m_speedData.m_dTotalValue);
			cxListCtrl.SetItemText(i,18,sMeanSpeed);
		}

		//V Max per interval
		{
			CString sMaxSpeed(_T(""));
			sMaxSpeed.Format(_T("%.2f"),pSummaryData->m_speedData.m_dMaxValue);
			cxListCtrl.SetItemText(i,19,sMaxSpeed);
		}

		//Interval with V max
		{
			CString sIntervalMaxSpeed(_T(""));
			sIntervalMaxSpeed.Format(_T("%d"),pSummaryData->m_speedData.m_lIntervalOfMax);
			cxListCtrl.SetItemText(i,20,sIntervalMaxSpeed);
		}

		//Q1
		{
			CString sQ1Speed(_T(""));
			sQ1Speed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dQ1);
			cxListCtrl.SetItemText(i,21,sQ1Speed);
		}

		//Q2
		{
			CString sQ2Speed(_T(""));
			sQ2Speed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dQ2);
			cxListCtrl.SetItemText(i,22,sQ2Speed);
		}

		//Q3
		{
			CString sQ3Speed(_T(""));
			sQ3Speed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dQ3);
			cxListCtrl.SetItemText(i,23,sQ3Speed);
		}

		//p1
		{
			CString sP1Speed(_T(""));
			sP1Speed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dP1);
			cxListCtrl.SetItemText(i,24,sP1Speed);
		}

		//p5
		{
			CString sP5Speed(_T(""));
			sP5Speed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dP5);
			cxListCtrl.SetItemText(i,25,sP5Speed);
		}

		//p10
		{
			CString sP10Speed(_T(""));
			sP10Speed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dP10);
			cxListCtrl.SetItemText(i,26,sP10Speed);
		}

		//p90
		{
			CString sP90Speed(_T(""));
			sP90Speed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dP90);
			cxListCtrl.SetItemText(i,27,sP90Speed);
		}

		//p95
		{
			CString sP95Speed(_T(""));
			sP95Speed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dP95);
			cxListCtrl.SetItemText(i,28,sP95Speed);
		}

		//p99
		{
			CString sP99Speed(_T(""));
			sP99Speed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dP99);
			cxListCtrl.SetItemText(i,29,sP99Speed);
		}

		//std dev
		{
			CString sStdSpeed(_T(""));
			sStdSpeed.Format(_T("%.2f"),pSummaryData->m_speedData.m_statisticalTool.m_dSigma);
			cxListCtrl.SetItemText(i,30,sStdSpeed);
		}

		//Min occupancy per interval
		{
			CString sMinOccupancy(_T(""));
			ElapsedTime tMinOccupancy(pSummaryData->m_occupancyData.m_dMinValue);
			sMinOccupancy.Format(_T("%02d:%02d:%02d"),tMinOccupancy.GetHour(),tMinOccupancy.GetMinute(),tMinOccupancy.GetSecond());
			cxListCtrl.SetItemText(i,31,sMinOccupancy);
		}

		//Interval with min occupancy
// 		{
// 			CString sMinIntervalOccupancy(_T(""));
// 			sMinIntervalOccupancy.Format(_T("%d"),pSummaryData->m_speedData.m_lIntervalOfMin);
// 			cxListCtrl.SetItemText(i,33,sMinIntervalOccupancy);
// 		}

		//Mean occupancy per interval
		{
			CString sMeanOccupancy(_T(""));
			ElapsedTime tMeanOccupancy(pSummaryData->m_occupancyData.m_dTotalValue);
			sMeanOccupancy.Format(_T("%02d:%02d:%02d"),tMeanOccupancy.GetHour(),tMeanOccupancy.GetMinute(),tMeanOccupancy.GetSecond());
			cxListCtrl.SetItemText(i,32,sMeanOccupancy);
		}

		//Max occupancy per interval
		{
			CString sMaxOccupancy(_T(""));
			ElapsedTime tMaxOccupancy(pSummaryData->m_occupancyData.m_dMaxValue);
			sMaxOccupancy.Format(_T("%02d:%02d:%02d"),tMaxOccupancy.GetHour(),tMaxOccupancy.GetMinute(),tMaxOccupancy.GetSecond());
			cxListCtrl.SetItemText(i,33,sMaxOccupancy);
		}

		//Interval with max occupancy
		{
			CString sMaxIntervalOccupancy(_T(""));
			sMaxIntervalOccupancy.Format(_T("%d"),pSummaryData->m_speedData.m_lIntervalOfMax);
			cxListCtrl.SetItemText(i,34,sMaxIntervalOccupancy);
		}

		//Q1
		{
			CString sQ1Occupancy(_T(""));
			ElapsedTime tQ1OOccupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dQ1);
			sQ1Occupancy.Format(_T("%02d:%02d:%02d"),tQ1OOccupancy.GetHour(),tQ1OOccupancy.GetMinute(),tQ1OOccupancy.GetSecond());
			cxListCtrl.SetItemText(i,35,sQ1Occupancy);
		}

		//Q2
		{
			CString sQ2Occupancy(_T(""));
			ElapsedTime tQ2OOccupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dQ2);
			sQ2Occupancy.Format(_T("%02d:%02d:%02d"),tQ2OOccupancy.GetHour(),tQ2OOccupancy.GetMinute(),tQ2OOccupancy.GetSecond());
			cxListCtrl.SetItemText(i,36,sQ2Occupancy);
		}

		//Q3
		{	
			CString sQ3Occupancy(_T(""));
			ElapsedTime tQ3OOccupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dQ3);
			sQ3Occupancy.Format(_T("%02d:%02d:%02d"),tQ3OOccupancy.GetHour(),tQ3OOccupancy.GetMinute(),tQ3OOccupancy.GetSecond());
			cxListCtrl.SetItemText(i,37,sQ3Occupancy);
		}

		//p1
		{
			CString sP1Occupancy(_T(""));
			ElapsedTime tQP1Occupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dP1);
			sP1Occupancy.Format(_T("%02d:%02d:%02d"),tQP1Occupancy.GetHour(),tQP1Occupancy.GetMinute(),tQP1Occupancy.GetSecond());
			cxListCtrl.SetItemText(i,38,sP1Occupancy);
		}

		//p5
		{
			CString sP5Occupancy(_T(""));
			ElapsedTime tQP5Occupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dP5);
			sP5Occupancy.Format(_T("%02d:%02d:%02d"),tQP5Occupancy.GetHour(),tQP5Occupancy.GetMinute(),tQP5Occupancy.GetSecond());
			cxListCtrl.SetItemText(i,39,sP5Occupancy);
		}

		//p10
		{
			CString sP10Occupancy(_T(""));
			ElapsedTime tQP10Occupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dP10);
			sP10Occupancy.Format(_T("%02d:%02d:%02d"),tQP10Occupancy.GetHour(),tQP10Occupancy.GetMinute(),tQP10Occupancy.GetSecond());
			cxListCtrl.SetItemText(i,40,sP10Occupancy);
		}

		//p90
		{
			CString sP90Occupancy(_T(""));
			ElapsedTime tQP90Occupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dP90);
			sP90Occupancy.Format(_T("%02d:%02d:%02d"),tQP90Occupancy.GetHour(),tQP90Occupancy.GetMinute(),tQP90Occupancy.GetSecond());
			cxListCtrl.SetItemText(i,41,sP90Occupancy);
		}

		//p95
		{
			CString sP95Occupancy(_T(""));
			ElapsedTime tQP95Occupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dP95);
			sP95Occupancy.Format(_T("%02d:%02d:%02d"),tQP95Occupancy.GetHour(),tQP95Occupancy.GetMinute(),tQP95Occupancy.GetSecond());
			cxListCtrl.SetItemText(i,42,sP95Occupancy);
		}

		//p99
		{
			CString sP99Occupancy(_T(""));
			ElapsedTime tQP99Occupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dP99);
			sP99Occupancy.Format(_T("%02d:%02d:%02d"),tQP99Occupancy.GetHour(),tQP99Occupancy.GetMinute(),tQP99Occupancy.GetSecond());
			cxListCtrl.SetItemText(i,43,sP99Occupancy);
		}

		//std dev
		{
			CString sStdOccupancy(_T(""));
			ElapsedTime tQPStdOccupancy(pSummaryData->m_occupancyData.m_statisticalTool.m_dSigma);
			sStdOccupancy.Format(_T("%02d:%02d:%02d"),tQPStdOccupancy.GetHour(),tQPStdOccupancy.GetMinute(),tQPStdOccupancy.GetSecond());
			cxListCtrl.SetItemText(i,44,sStdOccupancy);
		}
	}
}

void CAirsideTaxiwayUtilizationSummaryResult::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Taxiway"), LVCFMT_LEFT, 50);	
	cxListCtrl.InsertColumn(1, _T("From intersection"), LVCFMT_LEFT, 50);	
	cxListCtrl.InsertColumn(2, _T("To intersection"), LVCFMT_LEFT, 50);	

	cxListCtrl.InsertColumn(3, _T("Min movements per interval"), LVCFMT_LEFT, 60);
	//cxListCtrl.InsertColumn(4, _T("Interval with min movements"), LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(4, _T("Mean movements per interval"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(5, _T("Max movement per interval"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(6, _T("Interval with max movements"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(7, _T("Q1"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(8, _T("Q2"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(9, _T("Q3"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(10, _T("P1"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(11, _T("P5"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(12, _T("P10"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(13, _T("P90"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(14, _T("P95"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(15, _T("P99"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(16, _T("Std Dev"),LVCFMT_LEFT,60);

	cxListCtrl.InsertColumn(17, _T("V min per interval"), LVCFMT_LEFT, 60);
	//cxListCtrl.InsertColumn(18, _T("Interval with V min"), LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(18, _T("V mean per interval"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(19, _T("V Max per interval"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(20, _T("Interval with V max"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(21, _T("Q1"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(22, _T("Q2"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(23, _T("Q3"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(24, _T("P1"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(25, _T("P5"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(26, _T("P10"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(27, _T("P90"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(28, _T("P95"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(29, _T("P99"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(30, _T("Std Dev"),LVCFMT_LEFT,60);

	cxListCtrl.InsertColumn(31, _T("Min occupancy per interval"), LVCFMT_LEFT, 60);
//	cxListCtrl.InsertColumn(32, _T("Interval with min occupancy"), LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(32, _T("Mean occupancy per interval"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(33, _T("Max occupancy per interval"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(34, _T("Interval with max occupancy"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(35, _T("Q1"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(36, _T("Q2"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(37, _T("Q3"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(38, _T("P1"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(39, _T("P5"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(40, _T("P10"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(41, _T("P90"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(42, _T("P95"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(43, _T("P99"),LVCFMT_LEFT,60);
	cxListCtrl.InsertColumn(44, _T("Std Dev"),LVCFMT_LEFT,60);
}