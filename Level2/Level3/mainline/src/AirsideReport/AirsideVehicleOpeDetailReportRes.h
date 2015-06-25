#pragma once
#include "AirsideVehicleOpeReportResult.h"
class CVehicleOperaterDetailReportItem ;
class  CGTCtrlSubItem
{
public:
	ElapsedTime m_start ;
	ElapsedTime m_end ;
	int m_numberOfvehicle ;

	CGTCtrlSubItem():m_numberOfvehicle(0) {} ;
};
class CGTCtrlDataItem
{
public:
	~CGTCtrlDataItem() ;
	CString m_VehicleType ;
	std::vector<CGTCtrlSubItem*> m_IntervalDatas ;
public:
	void AddVehicleCount(ElapsedTime _time) ;
	void InitIntervalTime(int _size,ElapsedTime _interval) ;
};
class CGTCtrlData
{
public:
	CGTCtrlData() {};
	~CGTCtrlData() ;
public:
	CGTCtrlDataItem* AddItemData(CString& _VehicleType) ;
	CGTCtrlDataItem* GetItemData(CString& _VehicleType) ;
	BOOL FindByVehicleType(CString& _vehicleType,CGTCtrlDataItem** _PVehicleItem) ;
	void InitInterval(int _size,ElapsedTime _interval) ;
public:
	std::vector<CGTCtrlDataItem*> m_Data ;
};

class CAirsideVehicleOpeDetailReportRes :
	public CAirsideVehicleOpeReportResult
{


public:
	CAirsideVehicleOpeDetailReportRes(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):m_result(_result),m_pParameter(pParameter),CAirsideVehicleOpeReportResult() {} ;
	virtual ~CAirsideVehicleOpeDetailReportRes(void) {};
public:
	virtual void GenerateResult() = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
protected:
	std::vector<CVehicleOperaterDetailReportItem>* m_result ;
	ElapsedTime m_IntervalTime ;
	int m_Intervals ;
	CGTCtrlData m_ChartResult ;
	CParameters *m_pParameter ;
	CString m_TittleName ;
protected:
	virtual void CalculateTimeInterval() = 0 ;
	virtual void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)  ;
};
class CAirsideVehicleOpeDetailReportResForOnRoadTime : public CAirsideVehicleOpeDetailReportRes
{
public:
	CAirsideVehicleOpeDetailReportResForOnRoadTime(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter)
		:CAirsideVehicleOpeDetailReportRes(_result,pParameter){
		m_TittleName.Format(_T("Total OnRoad Time")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForOnRoadTime() {};
		void GenerateResult() ;
protected:
	void CalculateTimeInterval() ;

};
class CAirsideVehicleOpeDetailReportResForIdleTime : public CAirsideVehicleOpeDetailReportRes
{
public:
	CAirsideVehicleOpeDetailReportResForIdleTime(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportRes(_result,pParameter){
		m_TittleName.Format(_T("Total Idle Time")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForIdleTime() {};
		void GenerateResult() ;
protected:
	void CalculateTimeInterval() ;
};
class  CAirsideVehicleOpeDetailReportResForServerTime : public CAirsideVehicleOpeDetailReportRes
{
public:
	CAirsideVehicleOpeDetailReportResForServerTime(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportRes(_result,pParameter){
		m_TittleName.Format(_T("Total Server Time")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForServerTime() {};
		void GenerateResult() ;
protected:
	void CalculateTimeInterval() ;
};
class CAirsideVehicleOpeDetailReportResForServerNumber : public CAirsideVehicleOpeDetailReportRes
{
public:
	CAirsideVehicleOpeDetailReportResForServerNumber(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportRes(_result,pParameter){
		m_TittleName.Format(_T("Total Server Number")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForServerNumber() {};
	void GenerateResult() ;
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
protected:
	void CalculateTimeInterval() ;
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
};
class CAirsideVehicleOpeDetailReportResForPoolNumber : public CAirsideVehicleOpeDetailReportRes
{
public:
	CAirsideVehicleOpeDetailReportResForPoolNumber(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportRes(_result,pParameter){
		m_TittleName.Format(_T("Total Return To Pool Number")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForPoolNumber() {};
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
	void GenerateResult() ;
protected:
	void CalculateTimeInterval() ;
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
};
class CAirsideVehicleOpeDetailReportResForGas : public CAirsideVehicleOpeDetailReportRes
{
public:
	CAirsideVehicleOpeDetailReportResForGas(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportRes(_result,pParameter){
		m_TittleName.Format(_T("Total Return To Gas Number")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForGas() {};
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter){} ;
	void GenerateResult() {};
protected:
	void CalculateTimeInterval() {};
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) {};
};
class CAirsideVehicleOpeDetailReportResForTotalDistance : CAirsideVehicleOpeDetailReportRes
{
public:
	CAirsideVehicleOpeDetailReportResForTotalDistance(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportRes(_result,pParameter){
		m_TittleName.Format(_T("Total Distance")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForTotalDistance() {};
	void GenerateResult() ;
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
protected:
	void CalculateTimeInterval() ;
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
};
class CAirsideVehicleOpeDetailReportResForFuelConsumed : public CAirsideVehicleOpeDetailReportRes
{
public:
	CAirsideVehicleOpeDetailReportResForFuelConsumed(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportRes(_result,pParameter){
		m_TittleName.Format(_T("Total Fuel Consumed")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForFuelConsumed() {};
	void GenerateResult() ;
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
protected:
	void CalculateTimeInterval() ;
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
} ;
class CAirsideVehicleOpeDetailReportResForAvgSpeed : public CAirsideVehicleOpeDetailReportRes
{
public:
	CAirsideVehicleOpeDetailReportResForAvgSpeed(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportRes(_result,pParameter){
		m_TittleName.Format(_T("Avg Speed")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForAvgSpeed() {};
	virtual void GenerateResult() ;
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
protected:
	virtual void CalculateTimeInterval() ;
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
};
class CAirsideVehicleOpeDetailReportResForMaxSpeed : public CAirsideVehicleOpeDetailReportResForAvgSpeed
{
public:
	CAirsideVehicleOpeDetailReportResForMaxSpeed(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportResForAvgSpeed(_result,pParameter){
		m_TittleName.Format(_T("Max Speed")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForMaxSpeed() {};
	void GenerateResult() ;
protected:
	void CalculateTimeInterval() ;
};
class CAirsideVehicleOpeDetailReportResForServerPerForm : public CAirsideVehicleOpeDetailReportResForAvgSpeed
{
public:
	CAirsideVehicleOpeDetailReportResForServerPerForm(std::vector<CVehicleOperaterDetailReportItem>* _result,CParameters *pParameter):CAirsideVehicleOpeDetailReportResForAvgSpeed(_result,pParameter){
		m_TittleName.Format(_T("Vehicle Services performed")) ;
	} ;
	~CAirsideVehicleOpeDetailReportResForServerPerForm() {};
	void GenerateResult() ;
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
protected:
	void CalculateTimeInterval() ;
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
};
