#pragma once
#include "Common/elaptime.h"
#include "landsidebaseresult.h"
#include "..\Results\LandsideVehicleDelayLog.h"
class LSVehicleTotalDelayResult : public LandsideBaseResult
{
public:
	class TotalDelayItem
	{
	public:
		TotalDelayItem();
		~TotalDelayItem();

		//vehicle
		int m_nVehicleID;

		bool operator < (const TotalDelayItem& delayItem)const
		{
			if (m_nVehicleID < delayItem.m_nVehicleID)
			{
				return true;
			}
			return false;
		}

		//Vehicle Type define in parameter
		CString m_strVehicleTypeName;
		CString m_strCompleteTypeName;

		//entry&exit time&resource
		ElapsedTime m_eEntryTime;
		CString m_strEntryRs;
		ElapsedTime m_eExitTime;
		CString m_strExitRs;


		//total delayed time
		ElapsedTime m_eTotalDelayTime;

		//Ideal Arrive Time 
		ElapsedTime m_eIdealArrTime;

		//Actual Time Spent
		ElapsedTime m_eActualTimeSpend;

		//Ideal Time Spent
		ElapsedTime m_eIdealTimeSpent;
	};

	LSVehicleTotalDelayResult(void);
	~LSVehicleTotalDelayResult(void);

public:
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);
	virtual BOOL ReadReportData( ArctermFile& _file );
	virtual BOOL WriteReportData( ArctermFile& _file );

protected:
	std::vector<TotalDelayItem> m_vResult;


};
