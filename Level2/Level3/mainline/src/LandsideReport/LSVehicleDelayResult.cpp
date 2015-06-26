#include "StdAfx.h"
#include ".\lsvehicledelayresult.h"
#include "LSVehicleDelayParam.h"
#include "..\Results\LandsideVehicleDelayLog.h"
#include "..\Common\ProgressBar.h"
#include "..\Landside\LandsideVehicleLogEntry.h"
#include "..\Landside\LandsideVehicleLog.h"
#include "..\Landside\InputLandside.h"

LSVehicleDelayResult::LSVehicleDelayResult(void)
{
}

LSVehicleDelayResult::~LSVehicleDelayResult(void)
{
}

LSVehicleDelayResult::DelayItem::DelayItem()
{
	//vehicle
	m_nVehicleID = -1;
//	m_nVehicleType = -1;

	//Resource
	m_nResourceID = -1;

	//Reason
	m_enumReason = LandsideVehicleDelayLog::DR_TrafficLight;

	//Operation
	m_enumOperation = LandsideVehicleDelayLog::DO_Stop;
}

LSVehicleDelayResult::DelayItem::~DelayItem()
{

}

void LSVehicleDelayResult::GenerateResult(CBGetLogFilePath pFunc, LandsideBaseParam* pParameter, InputLandside *pLandisde)
{
	m_mapDelayResult.clear();
	
	std::vector<ReportVehicle> vReportVehicles;

	{


		EventLog<LandsideVehicleEventStruct> vehicleEventLog;
		LandsideVehicleLog vehicleLog;
		vehicleLog.SetEventLog(&vehicleEventLog);

		CString szDesPath = (*pFunc)(LandsideDescFile);
		CString szEventPath = (*pFunc)(LandsideEventFile);

		if (szEventPath.IsEmpty() || szEventPath.IsEmpty())
			return;
		//load description
		vehicleLog.LoadDataIfNecessary(szDesPath);
		//load log event
		vehicleLog.LoadEventDataIfNecessary(szEventPath);

		LandsideVehicleLogEntry element;
		element.SetEventLog(vehicleLog.getEventLog());
		long vehicleCount = vehicleLog.getCount();

		ASSERT(pParameter);
		CProgressBar bar( "Generating Vehicle Delay Report", 100, vehicleCount, TRUE );


		for (long i = 0; i < vehicleCount; i++)
		{
			bar.StepIt();
			vehicleLog.getItem( element, i );
			
			bool bFromValid = false;
			bool bToValid = false;		
			
			ReportVehicle rpVehicle;
			rpVehicle.nVehicleID = element.GetVehicleDesc().vehicleID;
			rpVehicle.sVehicleName = element.GetVehicleDesc().sName;

			CHierachyName vehicleName;
			vehicleName.fromString( element.GetVehicleDesc().sName);
			

			if (!pParameter->FilterName(vehicleName))
				continue;

			for (long nEvent = 0; nEvent < element.getCount(); nEvent++)
			{
				LandsideVehicleEventStruct nextEvent = element.getEvent(nEvent);

				//get resource
				LandsideFacilityLayoutObject* pRes = pLandisde->getObjectList().getObjectByID(nextEvent.resid);
				if(pRes)
				{
					if(!bFromValid && pParameter->ContainFromObject(pRes->getName()))
					{
						bFromValid = true;
						rpVehicle.eStartTime.setPrecisely(nextEvent.time);
					}
					if(bFromValid && pParameter->ContainToObject(pRes->getName()))
					{
						bToValid = true;
						rpVehicle.eEndTime.setPrecisely(nextEvent.time);						
					}
				}				
			}

			if(bFromValid && bToValid)
				vReportVehicles.push_back(rpVehicle);
		}
	}


	LandsideVehicleDelayLog vehicleDelayLog; 
	CString strDelayDescPath = (*pFunc)(LSVehicleDelayLog);
	vehicleDelayLog.LoadData(strDelayDescPath);


	for (int nrp = 0; nrp < static_cast<int>(vReportVehicles.size()); ++nrp)
	{
		//check vehicle is in the list need  to report
		ReportVehicle rpVehicle = vReportVehicles[nrp];
		if(rpVehicle.nVehicleID < 0)
			continue;

		bool vehicleHasDelay = false;
		int nDelayCount = vehicleDelayLog.GetItemCount();
		for (int nItem = 0; nItem < nDelayCount; ++ nItem)
		{
			LandsideVehicleDelayItem* pDelayItem = vehicleDelayLog.GetItem(nItem);

			ASSERT(pDelayItem != NULL);
			if(pDelayItem == NULL)
				continue;
			
			if (rpVehicle.nVehicleID != pDelayItem->m_nVehicleID)
				continue;
			

			if(pParameter->getStartTime() > rpVehicle.eStartTime)
				rpVehicle.eStartTime = pParameter->getStartTime();
			if(pParameter->getEndTime() < rpVehicle.eEndTime)
				rpVehicle.eEndTime = pParameter->getEndTime();

			//check start end time
			if(pDelayItem->m_eEndTime < rpVehicle.eStartTime || 
				pDelayItem->m_eStartTime > rpVehicle.eEndTime)
				continue;


			if(!pParameter->ContainVehicleType(pDelayItem->m_nVehicleType))
				continue;

			//check
			DelayItem dItem;

			dItem.m_nVehicleID = pDelayItem->m_nVehicleID;
		//	dItem.m_nVehicleType = pDelayItem->m_nVehicleType;


		/*	dItem.m_eStartTime = pDelayItem->m_eStartTime;
			dItem.m_eEndTime = pDelayItem->m_eEndTime;*/

			dItem.m_eIncurredTime = pDelayItem->m_eStartTime;
			dItem.m_eDelayTime = pDelayItem->m_eDelayTime;


			dItem.m_nResourceID = pDelayItem->m_nResourceID;
			dItem.m_strResName = pDelayItem->m_strResName;
			if (dItem.m_strResName.IsEmpty())
			{
				LandsideFacilityLayoutObject* pRes = pLandisde->getObjectList().getObjectByID(dItem.m_nResourceID);
				if (pRes)
				{
					dItem.m_strResName = pRes->getName().GetIDString();
				}
			}


			dItem.m_enumReason = pDelayItem->m_enumReason;

			dItem.m_enumOperation = pDelayItem->m_enumOperation;

			dItem.m_strCompleteTypeName = rpVehicle.sVehicleName;

		//	dItem.m_strDesc = pDelayItem->m_strDesc;
			vehicleHasDelay = true;
			CHierachyName vehicleName;
			vehicleName.fromString( rpVehicle.sVehicleName);
			for (int i = 0; i < pParameter->GetVehicleTypeNameCount(); i++)
			{
				CHierachyName name = pParameter->GetVehicleTypeName(i);
				if (vehicleName.fitIn(name))
				{
					dItem.m_strTypeName = name.toString();
					AddDelayItem(dItem);
				}
			}
		}

		//no delay Item
		if (vehicleHasDelay == false)
		{
			DelayItem dItem;
			CHierachyName vehicleName;
			vehicleName.fromString( rpVehicle.sVehicleName);
			dItem.m_nVehicleID = rpVehicle.nVehicleID;
			dItem.m_strCompleteTypeName = rpVehicle.sVehicleName;
			dItem.m_enumReason = LandsideVehicleDelayLog::DR_NoReason;
			dItem.m_enumOperation = LandsideVehicleDelayLog::DO_Normal;
			for (int i = 0; i < pParameter->GetVehicleTypeNameCount(); i++)
			{
				CHierachyName name = pParameter->GetVehicleTypeName(i);
				if (vehicleName.fitIn(name))
				{
					dItem.m_strTypeName = name.toString();
					AddDelayItem(dItem);
				}
			}
		}
	}
	
}

void LSVehicleDelayResult::RefreshReport( LandsideBaseParam * parameter )
{

}

BOOL LSVehicleDelayResult::ReadReportData( ArctermFile& inFile )
{
	m_mapDelayResult.clear();

	while(!inFile.isBlank())
	{
		DelayItem dItem;
		inFile.getInteger(dItem.m_nVehicleID);
	//	inFile.getInteger(dItem.m_nVehicleType);


		inFile.getField(dItem.m_strTypeName.GetBuffer(1024),1024);
		dItem.m_strTypeName.ReleaseBuffer();

		inFile.getField(dItem.m_strCompleteTypeName.GetBuffer(1024),1024);
		dItem.m_strCompleteTypeName.ReleaseBuffer();

		inFile.getTime(dItem.m_eIncurredTime);
	/*	inFile.getTime(dItem.m_eStartTime);
		inFile.getTime(dItem.m_eEndTime);*/

		inFile.getTime(dItem.m_eDelayTime);


		inFile.getInteger(dItem.m_nResourceID);
		inFile.getField(dItem.m_strResName.GetBuffer(1024),1024);
		dItem.m_strResName.ReleaseBuffer();


		int nReason = 0;
		inFile.getInteger(nReason);
		if(nReason >= LandsideVehicleDelayLog::DR_TrafficLight && nReason < LandsideVehicleDelayLog::DR_Count)
			dItem.m_enumReason = (LandsideVehicleDelayLog::EnumDelayReason)nReason;

		int nOperation = 0;
		inFile.getInteger(nOperation);
		if(nOperation >= LandsideVehicleDelayLog::DO_Stop && nOperation < LandsideVehicleDelayLog::DO_Count)
			dItem.m_enumOperation = (LandsideVehicleDelayLog::EnumDelayOperation)nOperation;


// 		inFile.getField(dItem.m_strDesc.GetBuffer(1024),1024);
// 		dItem.m_strDesc.ReleaseBuffer();	

		AddDelayItem(dItem);
		
		inFile.getLine();

	}


	return TRUE;
}

//int m_nVehicleID;
//int m_nVehicleType;
//
////delay at the time start and end
//ElapsedTime m_eStartTime;
//ElapsedTime m_eEndTime;
//
//
////the time delayed
//ElapsedTime m_eDelayTime;
//
////Resource
//int m_nResourceID;
//CString m_strResName;
//
////Reason
//LandsideVehicleDelayLog::EnumDelayReason m_enumReason;
//
////Operation
//LandsideVehicleDelayLog::EnumDelayOperation m_enumOperation;
//
////Description
//CString m_strDesc;

BOOL LSVehicleDelayResult::WriteReportData( ArctermFile& _file )
{

	std::map<int, std::vector<DelayItem> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		std::vector<DelayItem> &vItems = (*iter).second;

		int nCount = static_cast<int>(vItems.size());
		for (int nItem = 0; nItem < nCount; ++ nItem)
		{
			DelayItem& dItem = vItems[nItem];
			
			_file.writeInt(dItem.m_nVehicleID);
	//		_file.writeInt(dItem.m_nVehicleType);

			_file.writeField(dItem.m_strTypeName);

			_file.writeField(dItem.m_strCompleteTypeName);

			_file.writeTime(dItem.m_eIncurredTime,TRUE);
			/*_file.writeTime(dItem.m_eStartTime,TRUE);
			_file.writeTime(dItem.m_eEndTime,TRUE);*/

			_file.writeTime(dItem.m_eDelayTime,TRUE);


			_file.writeInt(dItem.m_nResourceID);
			_file.writeField(dItem.m_strResName);


			_file.writeInt(dItem.m_enumReason);

			_file.writeInt(dItem.m_enumOperation);

		//	_file.writeField(dItem.m_strDesc);
			_file.writeLine();

		}
	}

	return TRUE;
}

void LSVehicleDelayResult::AddDelayItem( DelayItem& dItem )
{
	ASSERT(dItem.m_nVehicleID >= 0);

	m_mapDelayResult[dItem.m_nVehicleID].push_back(dItem);

}


///////////
