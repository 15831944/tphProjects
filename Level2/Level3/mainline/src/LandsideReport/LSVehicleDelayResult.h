#pragma once
#include "landsidebaseresult.h"
#include "..\Results\LandsideVehicleDelayLog.h"
#include <map>


class LANDSIDEREPORT_API LSVehicleDelayResult :
	public LandsideBaseResult
{
public:


	class DelayItem
	{
	public:
		DelayItem();
		~DelayItem();

		//vehicle
		int m_nVehicleID;
	//	int m_nVehicleType;

		CString m_strTypeName;
		CString m_strCompleteTypeName;

		//delay at the time start and end
		/*ElapsedTime m_eStartTime;
		ElapsedTime m_eEndTime;*/

		//delay incurred time
		ElapsedTime m_eIncurredTime;

		//the time delayed
		ElapsedTime m_eDelayTime;

		//Resource
		int m_nResourceID;
		CString m_strResName;

		//Reason
		LandsideVehicleDelayLog::EnumDelayReason m_enumReason;

		//Operation
		LandsideVehicleDelayLog::EnumDelayOperation m_enumOperation;

		//Description
	//	CString m_strDesc;


	protected:
	private:
	};


public:
	LSVehicleDelayResult(void);
	~LSVehicleDelayResult(void);



protected:
	std::map<int, std::vector<DelayItem> >m_mapDelayResult;

public:
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		load data from file
	//parameter:
	//		_file: input parameter and point to Landside result file
	//Return:
	//		TURE: success load
	//		FALSE: failed to load
	//-----------------------------------------------------------------------------------------
	virtual BOOL ReadReportData( ArctermFile& _file );

	//-----------------------------------------------------------------------------------------
	//Summary: 
	//		write parameter and report result into Landside//Landside.rep
	//Parameter:
	//		_file: input parameter and point to Landside result file
	//Return:
	//		TURE: success write
	//		FALSE: failed	
	//-----------------------------------------------------------------------------------------
	virtual BOOL WriteReportData( ArctermFile& _file );

protected:
	void AddDelayItem(DelayItem& dItem);


protected:
	class ReportVehicle
	{
	public:
		ReportVehicle(){nVehicleID = -1;}
		~ReportVehicle(){}

		int nVehicleID;
		CString sVehicleName;
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;

	};











};
