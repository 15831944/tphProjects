#pragma once
#import "../../lib/3DChart.tlb" no_auto_exclude
using namespace N3DCHARTLib;

#include "ChartDataDefine.h"
#include <vector>
#include <math.h>
 #include <algorithm>
using namespace std;
// CN3DChart
class C2DChartData;
class AFX_CLASS_EXPORT CN3DChart : public CWnd
{
	DECLARE_DYNAMIC(CN3DChart)

public:
	CN3DChart();
	virtual ~CN3DChart();
protected:
	IDualChartPtr m_pDualChart;
	std::vector<Arc3DChartType> m_vr3DChartType;
protected:
	//initialize 2DChart if no data gave.
	bool InitializeN3DChartState(void);	

	bool AddNN3DChartPresentGroupType(Arc3DChartType actype3DChart);

	COLORREF GetN3DChartColor(long lIndex = 0L); 
public:
	//mouse event,0 - disable;1 - zoom;2 - move;3 - trackball;4 - ContextMenu;
	bool SetMouseMode(int iMouseEventMode = 0);

	//draw 2D 3DChart 
	virtual bool DrawChart(C2DChartData & c2dChartData);

	//change 3DChart type 
	bool ShowN3DChartType(Arc3DChartType actype3DChart = Arc3DChartType_2D_Bar);

	//print chart
	bool PrintN3DChart(void);

	//export chart to bitmap file
	bool ExportToBMPFile(CString strFilePath,BOOL bUseWindowDimensions = TRUE,long lWidth = 500L,long lHeight = 400L,long lBitsPerPixel = 24L);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
};


