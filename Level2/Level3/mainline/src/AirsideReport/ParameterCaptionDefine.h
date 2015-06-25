#ifndef __PARAMETERCAPTIONDEFIN__H__
#define __PARAMETERCAPTIONDEFIN__H__


enum enumASReportType_Detail_Summary
{
	ASReportType_Detail = 0,
	ASReportType_Summary

};

const static char* ASReportTypeName[] = 
{
	_T("Detail"),
		_T("Summary")
};

enum ASParamterCaptionEnum
{
	ASParamCapEnum_FlightType = 0,
	ASParamCapEnum_ReportType ,
	ASParamCapEnum_ReportType_Type,
	ASParamCapEnum_ChartType,



	//add before this line
	ASParamCapEnum_Count
};

const static char* ASParamterCaptionName[] =
{
	_T("Flight Type"),
		_T("Report Type"),
		_T("Type"),
		_T("Chart Type")
};

#endif