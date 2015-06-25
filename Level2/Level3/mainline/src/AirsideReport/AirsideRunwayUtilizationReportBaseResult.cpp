#include "StdAfx.h"
#include ".\airsiderunwayutilizationreportbaseresult.h"
#include "AirsideRunwayUtilizationReportParam.h"
#include "..\Engine\Airside\CommonInSim.h"
#include "../common/FLT_CNST.H"
#include ".\InputAirside\RunwayExit.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/Runway.h"
#include "../AirsideReport/AirsideRunwayUtilizationReport.h"
#include "../AirsideReport/AirsideReportNode.h"

using namespace AirsideRunwayUtilizationReport;

CAirsideRunwayUtilizationReportBaseResult::CAirsideRunwayUtilizationReportBaseResult(void)
{
	m_pCBGetLogFilePath = NULL;
	m_pChartResult = NULL;
	m_AirportDB = NULL;
	m_bLoadFromFile = false;

	m_nProjID = -1;
	m_bParameterDefault = true;
}

CAirsideRunwayUtilizationReportBaseResult::~CAirsideRunwayUtilizationReportBaseResult(void)
{
	Clear();
}
void CAirsideRunwayUtilizationReportBaseResult::Draw3DChart()
{
	ASSERT(0);
}

void CAirsideRunwayUtilizationReportBaseResult::SetCBGetFilePath( CBGetLogFilePath pFunc )
{
	m_pCBGetLogFilePath= pFunc;
}

void CAirsideRunwayUtilizationReportBaseResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);
}

void CAirsideRunwayUtilizationReportBaseResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{

}

void CAirsideRunwayUtilizationReportBaseResult::RefreshReport( CParameters * parameter )
{

}

CAirsideReportBaseResult * CAirsideRunwayUtilizationReportBaseResult::GetReportResult()
{
	return m_pChartResult;
}

BOOL CAirsideRunwayUtilizationReportBaseResult::WriteReportData( ArctermFile& _file )
{
	return FALSE;
}

BOOL CAirsideRunwayUtilizationReportBaseResult::ReadReportData( ArctermFile& _file )
{
	return FALSE;
}


void CAirsideRunwayUtilizationReportBaseResult::setLoadFromFile( bool bLoad )
{
	m_bLoadFromFile = bLoad;
}

bool CAirsideRunwayUtilizationReportBaseResult::IsLoadFromFile()
{
	return m_bLoadFromFile;

}

void CAirsideRunwayUtilizationReportBaseResult::SetProjID( int nProjID )
{
	m_nProjID = nProjID;
}
void CAirsideRunwayUtilizationReportBaseResult::GenerateResult( CParameters *pParameter )
{
	CAirsideRunwayUtilizationReportParam *pParam = (CAirsideRunwayUtilizationReportParam *)pParameter;
	if(pParam  == NULL)
		return;

	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);


	CString strRunwayOpDescPath = (*m_pCBGetLogFilePath)(AirsideRunwayOperationFile);

	m_airsideOperationLog.ClearData();
	m_airsideOperationLog.LoadData(strRunwayOpDescPath);

	if(pParam->IsRunwayParamDefault())
		m_bParameterDefault = true;
	else
		m_bParameterDefault = false;
	GetAllRunwayMark(pParam);
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

		//check constraint
		bool bFitParam = false;
		AirsideFlightDescStruct	fltDesc;
		int nFltLogCount = fltLog.getCount();
		AirsideFlightLogEntry logEntry;
		for (int i =0; i < nFltLogCount; ++i)
		{
			logEntry.SetEventLog(&airsideFlightEventLog);
			fltLog.getItem(logEntry,i);
			fltDesc = logEntry.GetAirsideDescStruct();
			if(fltDesc.id == pOperationItem->m_nFlightIndex )
			{
				if(fits(pParameter,fltDesc,pOperationItem))
				{
					bFitParam = true;
					break;
				}

			}
		}
		if(bFitParam == false)//the flight constraint not fit move to next 
			continue;

		DetailFlightRunwayResult *pFlightRunwayResult = new DetailFlightRunwayResult();

		//set flight information
		//airline
		pFlightRunwayResult->m_strAirline = CString(fltDesc._airline.GetValue());
		//flt id
		if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Landing)// landing 
			pFlightRunwayResult->m_strFlightID = CString(fltDesc._arrID.GetValue());
		else if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Takeoff)
			pFlightRunwayResult->m_strFlightID = CString(fltDesc._depID.GetValue());
		pFlightRunwayResult->m_strFlightType = CString(fltDesc.acType);//ac type


		//set runway information
		pFlightRunwayResult->m_nRunwayID = pOperationItem->m_nRunwayID;
		pFlightRunwayResult->m_enumRunwayMark = pOperationItem->m_enumRunwayMark;
		pFlightRunwayResult->m_strMarkName = pOperationItem->m_strMarkName;

		//airroute
		pFlightRunwayResult->m_strAirRoute = pOperationItem->GetAirRouteNames();


		m_vDetailResult.push_back(pFlightRunwayResult);
		//only entry event
		//calculate log
		int nEventCount = logEntry.getCount();
		if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Landing)// landing 
		{

			pFlightRunwayResult->m_enumRunwayOperation = CAirsideRunwayUtilizationReportParam::Landing;
			//enter from runway mark
			pFlightRunwayResult->m_strEnterObject = pOperationItem->m_strMarkName;

			bool bLandingFind = false;
			for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
			{
				AirsideFlightEventStruct event = logEntry.getEvent(nEvent);

				//find On landing event
				if(bLandingFind == false && (int)event.mode == OnLanding)
				{
					//landing
					ElapsedTime eTimeStart;
					eTimeStart.setPrecisely(event.time);
					pFlightRunwayResult->m_eTimeStart = eTimeStart;

					bLandingFind = true;
				}

				if(bLandingFind == true &&(int)event.mode == OnExitRunway)
				{
					//exit
					ElapsedTime eTimeEnd;
					eTimeEnd.setPrecisely(event.time);
					pFlightRunwayResult->m_eTimeEnd = eTimeEnd;
					//occupy time
					if(pFlightRunwayResult->m_eTimeEnd > pFlightRunwayResult->m_eTimeStart)
						pFlightRunwayResult->m_eTimeOccupancy = pFlightRunwayResult->m_eTimeEnd - pFlightRunwayResult->m_eTimeStart;

					if(event.intNodeID == pOperationItem->m_nPossibleExitID)
					{	//the minimum exit 
						pFlightRunwayResult->m_eMinimumTime = pFlightRunwayResult->m_eTimeOccupancy;
						pFlightRunwayResult->m_strExitObject = pOperationItem->m_strPossibleExitName;
						pFlightRunwayResult->m_strPossibleExit = pOperationItem->m_strPossibleExitName;
					}
					else
					{
						pFlightRunwayResult->m_strPossibleExit = pOperationItem->m_strPossibleExitName;
						pFlightRunwayResult->m_eMinimumTime = pOperationItem->m_ePossibleTime;
						CString strExitName = _T("");
						try
						{
							RunwayExit runwayExit;
							runwayExit.ReadData(event.intNodeID);
							strExitName = runwayExit.GetName();
						}
						catch(CADOException& e)
						{
							e.ErrorMessage();
						}

						pFlightRunwayResult->m_strExitObject = strExitName;

						//get the exit name
					}
					break;
				}
			}
			if(pFlightRunwayResult->m_eMinimumTime.asSeconds()<= 0)//no minimum time
			{
				if(pOperationItem->m_ePossibleTime.asSeconds() > 0)
				{
					pFlightRunwayResult->m_eMinimumTime = pOperationItem->m_ePossibleTime;//use possible time
				}
				else
				{
					pFlightRunwayResult->m_eMinimumTime = pFlightRunwayResult->m_eTimeOccupancy;//using cuurent occupancy time
				}
	
				pFlightRunwayResult->m_strPossibleExit = pOperationItem->m_strPossibleExitName;
			}
		}
		else if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Takeoff)
		{	
			pFlightRunwayResult->m_enumRunwayOperation = CAirsideRunwayUtilizationReportParam::TakeOff;
			//enter from runway mark
			pFlightRunwayResult->m_strExitObject = pOperationItem->m_strMarkName;

			bool bTakeoffFind = false;
			bool bWaitEventFind = false;
			long lStartWaitingonRunway = 0L;//start waiting on runway event time
			long lTakeOffOverRunwayTime = 0L;
			for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
			{
				AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
				
				//waiting to runway event
				if(bWaitEventFind== false && (int)event.mode == OnTakeOffWaitEnterRunway)
				{
					lStartWaitingonRunway = event.time;
					bWaitEventFind = true;
				}

				//start moving to runway
				if(bTakeoffFind == false && (int)event.mode == OnTakeOffEnterRunway)
				{
					long lTimeEntry = event.time;

					if(bWaitEventFind && lTimeEntry > lStartWaitingonRunway)//waiting time
						pFlightRunwayResult->m_eTimeWait.setPrecisely(lTimeEntry - lStartWaitingonRunway);

					//entry time
					ElapsedTime eTimeStart;
					eTimeStart.setPrecisely(lTimeEntry);
					pFlightRunwayResult->m_eTimeStart = eTimeStart;

					bTakeoffFind = true;

				}

				if((int)event.mode == OnPreTakeoff)
				{
					//entry name
					CString strExitName = _T("");
					try
					{
						RunwayExit runwayExit;
						runwayExit.ReadData(event.nodeNO);
						strExitName = runwayExit.GetName();
					}
					catch(CADOException& e)
					{
						e.ErrorMessage();
					}
					pFlightRunwayResult->m_strEnterObject = strExitName;//position time
				}
				if(bTakeoffFind == true && (int)event.mode == OnTakeoff)
				{
					lTakeOffOverRunwayTime = event.time;
					ElapsedTime eTimeEnd;
					eTimeEnd.setPrecisely(lTakeOffOverRunwayTime);
					pFlightRunwayResult->m_eTimeEnd = eTimeEnd;
					//occupy time
					if(pFlightRunwayResult->m_eTimeEnd > pFlightRunwayResult->m_eTimeStart)
						pFlightRunwayResult->m_eTimeOccupancy = pFlightRunwayResult->m_eTimeEnd - pFlightRunwayResult->m_eTimeStart;

				}

				if(bTakeoffFind == true && (int)event.mode == OnClimbout)//take off over
				{
					////the item before OnClimbOut is the take off end
					////exit
					//ElapsedTime eTimeEnd;
					//eTimeEnd.setPrecisely(lTakeOffOverRunwayTime);
					//pFlightRunwayResult->m_eTimeEnd = eTimeEnd;
					////occupy time
					//if(pFlightRunwayResult->m_eTimeEnd > pFlightRunwayResult->m_eTimeStart)
					//	pFlightRunwayResult->m_eTimeOccupancy = pFlightRunwayResult->m_eTimeEnd - pFlightRunwayResult->m_eTimeStart;
					break;
				}
			}
		}
	}

}

bool CAirsideRunwayUtilizationReportBaseResult::fits( CParameters * pParameter,const AirsideFlightDescStruct& fltDesc, AirsideRunwayOperationLog::RunwayLogItem *pOperationItem )
{
	CAirsideRunwayUtilizationReportParam *pParam = (CAirsideRunwayUtilizationReportParam *)pParameter;
	if(pParam  == NULL || pOperationItem == NULL)
		return false;


	//check runway 
	CAirsideReportRunwayMark reportRunwayMark;
	reportRunwayMark.SetRunway(pOperationItem->m_nRunwayID,pOperationItem->m_enumRunwayMark,pOperationItem->m_strMarkName);

	int nRunwayParamCount = (int)pParam->m_vReportMarkParam.size();//no any setting, means default
	if(nRunwayParamCount == 0 )
		return true;

	for (int nRunwayParam = 0; nRunwayParam < nRunwayParamCount; ++ nRunwayParam)
	{
		//fit runway mark
		CAirsideRunwayUtilizationReportParam::CRunwayMarkParam& runwayMarkParam = pParam->m_vReportMarkParam[nRunwayParam];

		if(!runwayMarkParam.IsRunwayMarkFit(reportRunwayMark))//runway mark fit
			continue;

		//fit operation
		bool bOpeationFit = false;
		int nOperationCount = (int)runwayMarkParam.m_vOperationParam.size();
		CAirsideRunwayUtilizationReportParam::RunwayOperation enumCurRunwayOperation = CAirsideRunwayUtilizationReportParam::Landing;
		char mode = 'A';
		if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Landing
			|| pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_MissApproach)
		{
			enumCurRunwayOperation = CAirsideRunwayUtilizationReportParam::Landing;
		}
		else if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Takeoff)
		{
			enumCurRunwayOperation = CAirsideRunwayUtilizationReportParam::TakeOff;
			mode = 'D';
		}
		else 
			continue;

		for (int nOperation = 0; nOperation < nOperationCount; ++nOperation)
		{
			CAirsideRunwayUtilizationReportParam::CRunwayOperationParam& runwayOperaitonParam = runwayMarkParam.m_vOperationParam[nOperation];
			if(runwayOperaitonParam.m_enumOperation == CAirsideRunwayUtilizationReportParam::Both ||
				runwayOperaitonParam.m_enumOperation == enumCurRunwayOperation)// operation fited
			{
				//fit air route
				bool bAirRouteFit = false;
				int nAirRoutePassedCount = (int) pOperationItem->m_vAirRoute.size();
				for (int nAirRoutePassed = 0; nAirRoutePassed < nAirRoutePassedCount; ++ nAirRoutePassed)
				{

					CAirsideRunwayUtilizationReportParam::CAirRouteParam airRoutePassed;
					airRoutePassed.m_nRouteID = pOperationItem->m_vAirRoute[nAirRoutePassed].m_nRouteID;

					if(runwayOperaitonParam.fitAirRoute(airRoutePassed))
					{
						bAirRouteFit = true;
						break;
					}
				}


				if(bAirRouteFit == false)
				{
					if(!runwayOperaitonParam.IsAirRouteDefault())//all airroute is in parameter
						continue;//move to next runway operation fit
				}
				//fit
				AirsideFlightDescStruct compfltDesc = fltDesc;

				//if(enumCurRunwayOperation == CAirsideRunwayUtilizationReportParam::Landing)
				//	compfltDesc._depID = "";
				//else if(enumCurRunwayOperation == CAirsideRunwayUtilizationReportParam::TakeOff)
				//	compfltDesc._arrID = "";
				if(runwayOperaitonParam.fitFltCons(compfltDesc,mode))
					return true;			
			}
		}
	}

	return false;
}

void CAirsideRunwayUtilizationReportBaseResult::Clear()
{
	std::vector<DetailFlightRunwayResult *>::iterator iter = m_vDetailResult.begin();
	for(;iter != m_vDetailResult.end(); ++iter)
	{
		delete *iter;
	}

	m_vDetailResult.clear();

}

void CAirsideRunwayUtilizationReportBaseResult::GetAllRunwayMark( CParameters *pParameter )
{
	CAirsideRunwayUtilizationReportParam *pParam = (CAirsideRunwayUtilizationReportParam *)pParameter;
	if(pParam  == NULL)
		return;

	m_vRunwayParamMark.clear();

	int nRunwayParam = (int)pParam->m_vReportMarkParam.size();
	for (int nRunway = 0; nRunway < (int)pParam->m_vReportMarkParam.size(); ++nRunway)
	{
		CAirsideRunwayUtilizationReportParam::CRunwayMarkParam& runwayMarkParam = pParam->m_vReportMarkParam[nRunway];
		int nMarkCount = (int)runwayMarkParam.m_vRunwayMark.size();

		for (int nMark = 0; nMark < nMarkCount; ++nMark)
		{
			CAirsideReportRunwayMark& runwayMark = runwayMarkParam.m_vRunwayMark[nMark];

			if(runwayMark.m_nRunwayID == -1)
			{
				m_vRunwayParamMark.clear();
				//get all runway mark

				std::vector< ALTObject> vRunwayObjct;		
				try
				{
					CString strSQL = _T("");

					CString strSelect = _T("");
					strSelect.Format(_T("SELECT ID, APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED\
										FROM         ALTOBJECT\
										WHERE     (TYPE = %d) AND (APTID IN\
										(SELECT     ID\
										FROM          ALTAIRPORT\
										WHERE      (PROJID = %d)))"),ALT_RUNWAY,m_nProjID);


					long nRecordCount = -1;
					CADORecordset adoRecordset;
					CADODatabase::ExecuteSQLStatement(strSelect,nRecordCount,adoRecordset);
					while(!adoRecordset.IsEOF())
					{
						ALTObject object;
						int nObjID = -1;
						adoRecordset.GetFieldValue(_T("ID"),nObjID);
						object.ReadObject(adoRecordset);
						object.setID(nObjID);

						vRunwayObjct.push_back(object);

						adoRecordset.MoveNextData();
					}
				}
				catch (CADOException& e)
				{
					e.ErrorMessage();
				}

				//add runway node
				for (int nRunway = 0; nRunway < static_cast<int>(vRunwayObjct.size()); ++ nRunway)
				{
					int nObjID = vRunwayObjct.at(nRunway).getID();
					Runway* pRunway = (Runway *)ALTObject::ReadObjectByID(nObjID);
					if(pRunway != NULL)
					{
						CAirsideReportRunwayMark runwayMark1;
						CString strMark1 = CString(pRunway->GetMarking1().c_str());
						runwayMark1.SetRunway(pRunway->getID(),RUNWAYMARK_FIRST,strMark1);


						CAirsideReportRunwayMark runwayMark2;
						CString strMark2 = CString(pRunway->GetMarking2().c_str());
						runwayMark2.SetRunway(pRunway->getID(),RUNWAYMARK_SECOND,strMark2);

						m_vRunwayParamMark.push_back(runwayMark1);
						m_vRunwayParamMark.push_back(runwayMark2);

					}
				}

				return;
			}
			else
			{
				bool bFind = false;
				int nExistCount = (int)m_vRunwayParamMark.size();
				for (int nExist = 0; nExist < nExistCount; ++ nExist )
				{
					if(m_vRunwayParamMark[nExist].m_nRunwayID == runwayMark.m_nRunwayID)
					{
						bFind = true;
						break;
					}
				}
				if(!bFind)
					m_vRunwayParamMark.push_back(runwayMark);
			}
		}
	}





}

//flight information
BOOL AirsideRunwayUtilizationReport::DetailFlightRunwayResult::WriteReportData( ArctermFile& _file )
{

	//flight information
	_file.writeField(m_strAirline);
	_file.writeField(m_strFlightID);
	_file.writeField(m_strFlightType);

	//runway information
	_file.writeInt(m_nRunwayID);
	_file.writeInt(m_enumRunwayMark);
	_file.writeField(m_strMarkName);

	_file.writeInt((int)m_enumRunwayOperation);

	//airroute passed
	_file.writeField(m_strAirRoute);

	//occupancy time and exit time
	_file.writeTime(m_eTimeStart,TRUE);
	_file.writeTime(m_eTimeEnd,TRUE);

	_file.writeTime(m_eTimeOccupancy,TRUE);

	//enter exit at the object
	_file.writeField(m_strEnterObject);
	_file.writeField(m_strExitObject);

	//possible exit
	_file.writeField(m_strPossibleExit);
	_file.writeTime(m_eMinimumTime,TRUE);

	_file.writeTime(m_eTimeWait,TRUE);

	return TRUE;

}

BOOL AirsideRunwayUtilizationReport::DetailFlightRunwayResult::ReadReportData( ArctermFile& _file )
{
	_file.getField(m_strAirline.GetBuffer(1024),1024);
	m_strAirline.ReleaseBuffer();

	_file.getField(m_strFlightID.GetBuffer(1024),1024);
	m_strFlightID.ReleaseBuffer();

	_file.getField(m_strFlightType.GetBuffer(1024),1024);
	m_strFlightType.ReleaseBuffer();

	//runway information
	_file.getInteger(m_nRunwayID);
	_file.getInteger((int&)m_enumRunwayMark);
	_file.getField(m_strMarkName.GetBuffer(1024),1024);

	_file.getInteger((int&)m_enumRunwayOperation);

	//airroute passed
	_file.getField(m_strAirRoute.GetBuffer(1024),1024);
	m_strAirRoute.ReleaseBuffer();
    
	//occupancy time and exit time
	_file.getTime(m_eTimeStart,TRUE);
	_file.getTime(m_eTimeEnd,TRUE);

	_file.getTime(m_eTimeOccupancy,TRUE);

	//enter exit at the object
	_file.getField(m_strEnterObject.GetBuffer(1024),1024);
	m_strEnterObject.ReleaseBuffer();

	_file.getField(m_strExitObject.GetBuffer(1024),1024);
	m_strExitObject.ReleaseBuffer();

	//possible exit
	_file.getField(m_strPossibleExit.GetBuffer(1024),1024);
	m_strPossibleExit.ReleaseBuffer();
	_file.getTime(m_eMinimumTime,TRUE);

	_file.getTime(m_eTimeWait,TRUE);

	return TRUE;
}
























