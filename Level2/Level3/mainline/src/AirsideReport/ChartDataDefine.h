#pragma once

#include <vector>
#include <math.h>
using namespace std;
class __declspec(dllexport) C2DChartData 
{
public:
	C2DChartData(void);
	C2DChartData(const C2DChartData & srC2DChartData);
	virtual ~C2DChartData(void);
	C2DChartData & operator = (const C2DChartData & srC2DChartData);
public:
	CString m_strChartTitle;//图表主标题
	CString m_strFooter;//图表页脚(注脚)
	CString m_strXtitle;//x轴标题
	CString m_strYtitle;//y轴标题
	std::vector <std::vector <double> > m_vr2DChartData;//std::vector <double> 代表一组数据，多组放在std::vector <std::vector <double> > 里。
	std::vector <CString > m_vrLegend;//每组数据代表的对象，对象的顺序跟在m_vr2DChartData中的一样，显示于legend.
	std::vector <CString > m_vrXTickTitle;//x轴的刻度标题，如果没有给出，则不显示x轴刻度
};

enum __declspec(dllexport) Arc3DChartType //actype
{
	Arc3DChartType_2D_Bar = 0, //2D Bar
	Arc3DChartType_3D_Bar ,//3D Bar
	Arc3DChartType_2D_Line ,//2D line 
	Arc3DChartType_3D_Line,//3D Line
	Arc3DChartType_2D_Area,//2D Area
	Arc3DChartType_3D_Area,//3D Area
};