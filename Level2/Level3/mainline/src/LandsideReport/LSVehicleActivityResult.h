#pragma once
#include "landsidebaseresult.h"
#include "LandsideReportAPI.h"
#include "..\Common\elaptime.h"
#include <vector>

class InputLandside;

class LANDSIDEREPORT_API LSVehicleActivityResult :
	public LandsideBaseResult
{
public:
	enum ChartType
	{
		CT_EntryTime = 0,
		CT_ExitTime,

	};

public:
	class ActItem
	{
	public:
		ActItem()
		{
			nVehicleID = -1;
			dMaxSpeed = 0.0;
			dTotalDistance = 0.0;
			dAverageSpeed = 0.0;
			nPaxLoad = 0;

		}
		~ActItem(){}
	public:
		int			nVehicleID;
		CString		strTypeName;
		CString		sStartRs;//entry resource
		CString		sEndRs;//exit resource
		ElapsedTime eEntryTime;
		ElapsedTime eExitTime; 
		double		dMaxSpeed;
		double		dTotalDistance;
		double		dAverageSpeed;
		int			nPaxLoad;


	};

public:
	LSVehicleActivityResult(void);
	~LSVehicleActivityResult(void);




	virtual void Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);
	//-----------------------------------------------------------------------------------------
	//Summary:
	//		create list control header and set list has sortable
	//Parameter:
	//		cxListCtrl: reference pass, target list ctrl to init
	//		parameter: user input that knows how to create list ctrl header
	//		piSHC: make list ctrl sortable
	//-----------------------------------------------------------------------------------------
	virtual void InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ );

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		insert result of Landside report into list ctrl
	//Parameter:
	//		cxListCtrl: output parameter and target list ctrl to operate
	//		parameter: input parameter let knows how to fill list ctrl
	//------------------------------------------------------------------------------------------
	virtual void FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter );

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

	virtual LSGraphChartTypeList GetChartList() const;

	std::vector<ActItem> m_vActItem;

protected:


	class ChartResult
	{
	public:
		ChartResult(void);
		~ChartResult(void);

	public:
		void AddTime(const ElapsedTime& eTime);

	public:
		void Draw3DChart(const CString& strChartName, CARC3DChart& chartWnd, LandsideBaseParam *pParameter );

	protected:
		//get count in the range
		int GetCount(ElapsedTime& eMinTime, ElapsedTime& eMaxTime) const;
		CString GetXtitle(ElapsedTime& eMinTime, ElapsedTime& eMaxTime) const;

	protected:
		std::vector< ElapsedTime> m_vTime;
	};










};
