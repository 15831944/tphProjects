#include "StdAfx.h"
#include ".\airsideflightdistancetravelreportdetailresult.h"
#include "AirsideDistanceTravelParam.h"
#include "AirsideDistanceTravelParam.h"
#include <limits>
#include "../common/UnitsManager.h"

CAirsideFlightDistanceTravelReportDetailResult::CAirsideFlightDistanceTravelReportDetailResult(void)
{	
	m_dMinDist = 0.0;
	m_dMaxDist = 0.0;
	m_dInterval = 0.0;
}

CAirsideFlightDistanceTravelReportDetailResult::~CAirsideFlightDistanceTravelReportDetailResult(void)
{
}
void CAirsideFlightDistanceTravelReportDetailResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{


	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Distance Travel Report(Detailed) ");
	c2dGraphData.m_strYtitle = _T("Number of Airplane");
	CString strXTitle;
	strXTitle.Format("Distance(%s)", pParameter->GetUnitManager()->GetLengthUnitString());
	c2dGraphData.m_strXtitle = strXTitle;	


	CString strLabel = _T("");

	std::vector<double> vrData;
	for (size_t i = 0; i < m_vResult.size(); i++)
	{
		strLabel.Format(_T("%.0f-%.0f"),m_dMinDist + i*m_dInterval ,m_dMinDist + (i+1)*m_dInterval );
		c2dGraphData.m_vrXTickTitle.push_back(strLabel);

		vrData.push_back(static_cast<double>(m_vResult.at(i)));
	}
	c2dGraphData.m_vr2DChartData.push_back(vrData);
	c2dGraphData.m_strFooter = _T(" Distance Travel(Detailed) ;") + pParameter->GetParameterString();

	chartWnd.DrawChart(c2dGraphData);
}
 void CAirsideFlightDistanceTravelReportDetailResult::Draw3DChartFromFile(CARC3DChart& chartWnd, ArctermFile& _file) 
{
	TCHAR str[1024] = {0} ;
    C2DChartData c2dGraphData;
	_file.getLine() ;
	_file.getField(str,1024) ;
	c2dGraphData.m_strChartTitle = str ;
	//xtitile 
    _file.getField(str,1024) ;
	c2dGraphData.m_strXtitle = str;	
	//Ytitle
	_file.getField(str,1024) ;
	c2dGraphData.m_strYtitle = str;	
	//foot
	_file.getField(str,1024) ;
	c2dGraphData.m_strFooter = str;	
    
	double val = 0 ;
	std::vector<double> vrData;
	while(_file.getLine())
	{
	   if(_file.getField(str,1024))
          c2dGraphData.m_vrXTickTitle.push_back(str) ;
	   if(_file.getFloat(val))
          vrData.push_back(val) ;
	}
	c2dGraphData.m_vr2DChartData.push_back(vrData);

	chartWnd.DrawChart(c2dGraphData);
}
 void CAirsideFlightDistanceTravelReportDetailResult::ExportFileFrom3DChart( ArctermFile& _file, CParameters *pParameter)
{
	if(pParameter == NULL)
		return ;
	 _file.writeField(_T("ChartTitle")) ;
	 _file.writeField(_T("XTitle")) ;
	 _file.writeField(_T("Ytitle")) ;
	 _file.writeField(_T("m_strFooter")) ;
	 _file.writeLine() ;

	 _file.writeField(_T(" Distance Travel Report(Detailed) ")) ;
	 CString strXTitle;
	 strXTitle.Format("Distance(%s)", pParameter->GetUnitManager()->GetLengthUnitString());
     _file.writeField(strXTitle) ;
	 _file.writeField(_T("Number of Airplane")) ;
	_file.writeField(_T(" Distance Travel(Detailed) ;") + pParameter->GetParameterString()) ;
	 _file.writeLine() ;

	 CString vrXTickTitle ;
	 for (size_t i = 0; i < m_vResult.size(); i++)
	 {
		 vrXTickTitle.Format(_T("%.0f-%.0f"),m_dMinDist + i*m_dInterval ,m_dMinDist + (i+1)*m_dInterval );
		_file.writeField(vrXTickTitle);
        _file.writeDouble(static_cast<double>(m_vResult.at(i))) ;
		 _file.writeLine() ;
	 }  
}
void CAirsideFlightDistanceTravelReportDetailResult::GenerateResult(std::vector<CAirsideDistanceTravelReport::DistanceTravelReportItem>& vResult,CParameters *pParameter)
{
	m_vResult.clear();
	double dMinDist  = 0.0;
	double dMaxDist  = 0.0;
	//double dInterval = 0.0;


	GetMinMaxTravelDistance(vResult,dMinDist,dMaxDist);

	m_dMinDist = GetConvertValue(dMinDist,pParameter);
	m_dMaxDist = GetConvertValue(dMaxDist,pParameter);

	if (m_dMinDist == m_dMaxDist)
	{
		m_vResult.push_back(vResult.size());
		return;
	}


	ClacValueRange(m_dMaxDist,m_dMinDist,m_dInterval);
	if (m_dInterval<= 0)
		return;

	double dMiddleDist = m_dMinDist;
	while (dMiddleDist < m_dMaxDist)
	{
		int nCount = GetFlightCountTravelDist(vResult,dMiddleDist,dMiddleDist + m_dInterval,pParameter);
		m_vResult.push_back(nCount);

		dMiddleDist += m_dInterval;
	}

}
void CAirsideFlightDistanceTravelReportDetailResult::GetMinMaxTravelDistance(std::vector<CAirsideDistanceTravelReport::DistanceTravelReportItem>& vResult,double& dMinDist, double& dMaxDist )
{
	dMinDist =  (std::numeric_limits<double>::max)();
	dMaxDist = 0L;
	for ( size_t i =0; i < vResult.size(); ++i)
	{
		if (vResult[i].distance > dMaxDist)
		{
			dMaxDist = vResult[i].distance;
		}
		
		if (vResult[i].distance< dMinDist)
		{
			dMinDist = vResult[i].distance;
		}
	}

}
int CAirsideFlightDistanceTravelReportDetailResult::GetFlightCountTravelDist(std::vector<CAirsideDistanceTravelReport::DistanceTravelReportItem>& vResult,double dMinValue, double dMaxValue, CParameters *pParameter)
{
	int nCount = 0;
	for ( size_t i =0; i < vResult.size(); ++i)
	{
		if (GetConvertValue(vResult[i].distance,pParameter) < dMaxValue && GetConvertValue(vResult[i].distance,pParameter) >= dMinValue)
			nCount += 1;
	}

	return nCount;
}
double CAirsideFlightDistanceTravelReportDetailResult::GetConvertValue(double dInput, CParameters *pParameter)
{
	return pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS,pParameter->GetUnitManager()->GetLengthUnits(),dInput);
}






