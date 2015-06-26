#include "StdAfx.h"
#include ".\lsvehicletotaldelayresult.h"
#include "..\Results\LandsideVehicleDelayLog.h"
#include "..\Common\ProgressBar.h"
#include "..\Landside\LandsideVehicleLogEntry.h"
#include "..\Landside\LandsideVehicleLog.h"
#include "..\Landside\InputLandside.h"
#include "LandsideBaseParam.h"

LSVehicleTotalDelayResult::LSVehicleTotalDelayResult(void)
{
}

LSVehicleTotalDelayResult::~LSVehicleTotalDelayResult(void)
{
}

void LSVehicleTotalDelayResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	//make result data clean
	m_vResult.clear();

	//generate result data start
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

	//retrieve delay log to calculate each vehicle delay information
	LandsideVehicleDelayLog vehicleDelayLog; 
	CString strDelayDescPath = (*pFunc)(LSVehicleDelayLog);
	vehicleDelayLog.LoadData(strDelayDescPath);
	int nDelayCount = vehicleDelayLog.GetItemCount();

	ASSERT(pParameter);
	CProgressBar bar( "Generating Vehicle Total Delay Report", 100, vehicleCount, TRUE );


	for (long i = 0; i < vehicleCount; i++)
	{
		bar.StepIt();
		vehicleLog.getItem( element, i );

		bool bFromValid = false;
		bool bToValid = false;		

		TotalDelayItem rpVehicle;
		rpVehicle.m_nVehicleID = element.GetVehicleDesc().vehicleID;

		CHierachyName carName;
		carName.fromString( element.GetVehicleDesc().sName);
		if (!pParameter->FilterName(carName))
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
					rpVehicle.m_eEntryTime.setPrecisely(nextEvent.time);
					rpVehicle.m_strEntryRs = pRes->getName().GetIDString();

				}
				if(bFromValid && pParameter->ContainToObject(pRes->getName()))
				{
					bToValid = true;
					rpVehicle.m_eExitTime.setPrecisely(nextEvent.time);	
					rpVehicle.m_strExitRs = pRes->getName().GetIDString();
				}
			}				
		}

		//find start and exit need add to result
		if(bFromValid && bToValid)
		{
			//calculate total vehicle total delay time
			for (int nItem = 0; nItem < nDelayCount; ++ nItem)
			{
				LandsideVehicleDelayItem* pDelayItem = vehicleDelayLog.GetItem(nItem);

				ASSERT(pDelayItem != NULL);
				if(pDelayItem == NULL)
					continue;

				if (pDelayItem->m_nVehicleID != rpVehicle.m_nVehicleID)
					continue;
				
				//check start end time
				if(pDelayItem->m_eEndTime < rpVehicle.m_eEntryTime || 
					pDelayItem->m_eStartTime > rpVehicle.m_eExitTime)
					continue;

				rpVehicle.m_eTotalDelayTime += pDelayItem->m_eDelayTime;
			}
			
			rpVehicle.m_eIdealArrTime = rpVehicle.m_eExitTime - rpVehicle.m_eTotalDelayTime;
			rpVehicle.m_eActualTimeSpend = rpVehicle.m_eExitTime - rpVehicle.m_eEntryTime;
			rpVehicle.m_eIdealTimeSpent = rpVehicle.m_eActualTimeSpend - rpVehicle.m_eTotalDelayTime;
			rpVehicle.m_strCompleteTypeName = element.GetVehicleDesc().sName;

			CHierachyName vehicleName;
			vehicleName.fromString( element.GetVehicleDesc().sName);
			for (int i = 0; i < pParameter->GetVehicleTypeNameCount(); i++)
			{
				CHierachyName name = pParameter->GetVehicleTypeName(i);
				if (vehicleName.fitIn(name))
				{
					rpVehicle.m_strVehicleTypeName = name.toString();
					m_vResult.push_back(rpVehicle);
				}
			}
		}
	}

}

void LSVehicleTotalDelayResult::RefreshReport( LandsideBaseParam * parameter )
{

}

BOOL LSVehicleTotalDelayResult::ReadReportData( ArctermFile& _file )
{
	//retrieve count of data
	int nDataCount = 0;
	_file.getInteger(nDataCount);
	_file.getLine();

	for (int i = 0; i < nDataCount; i++)
	{
		TotalDelayItem dataItem;
		_file.getInteger(dataItem.m_nVehicleID);
		_file.getField(dataItem.m_strVehicleTypeName.GetBuffer(1024),1024);
		dataItem.m_strVehicleTypeName.ReleaseBuffer();

		_file.getField(dataItem.m_strCompleteTypeName.GetBuffer(1024),1024);
		dataItem.m_strCompleteTypeName.ReleaseBuffer();

		_file.getTime(dataItem.m_eEntryTime);
		_file.getField(dataItem.m_strEntryRs.GetBuffer(1024),1024);
		dataItem.m_strEntryRs.ReleaseBuffer();

		_file.getTime(dataItem.m_eExitTime);
		_file.getField(dataItem.m_strExitRs.GetBuffer(1024),1024);
		dataItem.m_strExitRs.ReleaseBuffer();

		_file.getTime(dataItem.m_eTotalDelayTime);
		_file.getTime(dataItem.m_eIdealArrTime);
		_file.getTime(dataItem.m_eActualTimeSpend);
		_file.getTime(dataItem.m_eIdealTimeSpent);

		m_vResult.push_back(dataItem);
		_file.getLine();
	}
	return TRUE;
}

BOOL LSVehicleTotalDelayResult::WriteReportData( ArctermFile& _file )
{
	//write count of data
	int nDataCount = (int)(m_vResult.size());
	_file.writeInt(nDataCount);
	_file.writeLine();

	//write each data item
	for (int i = 0; i < nDataCount; i++)
	{
		TotalDelayItem& dataItem = m_vResult[i];
		_file.writeInt(dataItem.m_nVehicleID);
		_file.writeField(dataItem.m_strVehicleTypeName);
		_file.writeField(dataItem.m_strCompleteTypeName);
		
		_file.writeTime(dataItem.m_eEntryTime);
		_file.writeField(dataItem.m_strEntryRs);

		_file.writeTime(dataItem.m_eExitTime);
		_file.writeField(dataItem.m_strExitRs);

		_file.writeTime(dataItem.m_eTotalDelayTime);
		_file.writeTime(dataItem.m_eIdealArrTime);
		_file.writeTime(dataItem.m_eActualTimeSpend);
		_file.writeTime(dataItem.m_eIdealTimeSpent);

		_file.writeLine();
	}
	return TRUE;
}

LSVehicleTotalDelayResult::TotalDelayItem::TotalDelayItem()
:m_nVehicleID(-1)
{

}

LSVehicleTotalDelayResult::TotalDelayItem::~TotalDelayItem()
{

}

