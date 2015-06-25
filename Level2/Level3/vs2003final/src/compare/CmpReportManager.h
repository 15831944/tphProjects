#pragma once

#include "cmpreport.h"
#include "ThreeDChartType.h"

class CCmpReportCtrl;
class CCmpDisplay;

class CCmpReportManager 
{
public:
	CCmpReportManager(CCmpReportCtrl* pReportCtrl);
	~CCmpReportManager(void);

	CCmpReport* GetCmpReport() { return &m_cmpReport; }
	//members
protected:
	CCmpDisplay* m_pDisplay;		//report display setting
	CCmpReportCtrl* m_pReportCtrl;
	CCmpReport m_cmpReport;
	//operation
public:
	void InitUpdate();
	void UpdateAllWnds();
	void GenerateResult();
	void DisplayReport();
	void Update3DChart();
	void Change3DChart(ThreeDChartType enmChartType);
	//PRINT GRAPH ,EXPORT GRAPH
	void PrintGraph();
	void ExportGraph(BSTR bstrFileName);

	void PrintListData();
};
