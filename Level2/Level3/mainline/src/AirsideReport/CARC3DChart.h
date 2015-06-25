#ifndef CARC3DCHART_HEADER
#define CARC3DCHART_HEADER	
#pragma once
#include "ChartDataDefine.h"

#define MSCHART //it will use the 3DChart

#ifdef N3DCHART
#include "N3DChart.h"
#define  CArc3DChartPointer	CN3DChart*
#elif defined MSCHART
#include "../AirsideReport/MSChart/mschart.h"
#define CArc3DChartPointer CMSChart*
#else
#define	CArc3DChartPointer    void*
#endif

class C2DChartData;

// CARC3DChart
class __declspec(dllexport) CARC3DChart
{ 

public:
	CARC3DChart();
	virtual ~CARC3DChart();
	
	//////////////////////////////////////////////////////////////////////////
public:
	//draw 2D 3DChart 
	virtual bool DrawChart(C2DChartData & c2dChartData);
	
	//change 3DChart type 
	bool Set3DChartType(Arc3DChartType actype3DChart = Arc3DChartType_2D_Bar);
	
	//print chart
	bool Print3DChart(void);

	//export chart to bitmap file
	bool ExportToBMPFile(void); 

	//edit the chart property
	void PropertyEdit(void);

	void SetAxiLabel(int nInterval);
	//////////////////////////////////////////////////////////////////////////
public:
	CArc3DChartPointer m_p3DChart;//point to 3DChart object. 
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

};

#endif
