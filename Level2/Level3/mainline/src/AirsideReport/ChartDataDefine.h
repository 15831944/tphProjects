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
	CString m_strChartTitle;//ͼ��������
	CString m_strFooter;//ͼ��ҳ��(ע��)
	CString m_strXtitle;//x�����
	CString m_strYtitle;//y�����
	std::vector <std::vector <double> > m_vr2DChartData;//std::vector <double> ����һ�����ݣ��������std::vector <std::vector <double> > �
	std::vector <CString > m_vrLegend;//ÿ�����ݴ���Ķ��󣬶����˳�����m_vr2DChartData�е�һ������ʾ��legend.
	std::vector <CString > m_vrXTickTitle;//x��Ŀ̶ȱ��⣬���û�и���������ʾx��̶�
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