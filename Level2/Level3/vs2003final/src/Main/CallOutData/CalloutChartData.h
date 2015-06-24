#pragma once
#include "..\..\Common\elaptime.h"

//the data which one interal has . 
//Ex ; the interval number is 4 , and each interval has 2 column data . then it's a 4 * 2 table 
class CCalloutChartData_ColumnData
{
public:
	int m_ndx ; // the identity id which used to market with the original data .
	double m_Val ; // the data which processor is at one interval .
	CString m_Name ; // column name
	CCalloutChartData_ColumnData(int _id):m_Val(0),m_ndx(_id) {} ;
};

// The interval which the time divide to 
class CCalloutChartData_TimeInterval
{
public:
	//the time which begin
	ElapsedTime m_StartTime ;

	//the time which end
	ElapsedTime m_EndTime ;

	CString GetName()
	{
		CString Msg ;
		Msg.Format(_T("%s-%s"),m_StartTime.printTime(),m_EndTime.printTime()) ;
		return Msg ;
	}
	// if true ,there has data changed else not
	BOOL m_IsNeedRefresh ; 
	std::vector<CCalloutChartData_ColumnData*> m_RowData ;
public:
	~CCalloutChartData_TimeInterval()
	{
		for (int i = 0 ; i < (int)m_RowData.size() ;i++)
		{
			delete m_RowData[i] ;
		}
	}
	CCalloutChartData_TimeInterval():m_IsNeedRefresh(FALSE)
	{

	}
};


class CCalloutChartData
{
public:
	CCalloutChartData(void):m_MaxYValye(50),m_ChartType(TY_Bar),m_TimeNdx(0){};
	~CCalloutChartData();

	enum Chart_Ty { TY_Line ,TY_Bar};
protected:

	// the index which interval the current time in , it will been set in function "GetTheTimeIntervalData" ;
	// in that function , it just would find the interval at the beginning of this index .
	int m_TimeNdx ;

public:
	std::vector<CCalloutChartData_TimeInterval*> m_TimeIntervalData ;

	//axis X title
	CString m_XTitle ; 

	//axis Y title
	CString m_YTitle ;

	//chart title
	CString m_ChartTitle ;

	//max axis Y value
	double m_MaxYValye ;

	//chart type , bar or line
	Chart_Ty m_ChartType ;

public:
	// return interval  which the current time at , if not ,return NULL  
	CCalloutChartData_TimeInterval* GetTheTimeIntervalData(const ElapsedTime& _currentTime,int& _Ndx) ;


	// set refresh flag, at one time ,it should only one interval need refresh,
	void SetTheRefreshFlag( CCalloutChartData_TimeInterval* _timeInterval,BOOL _ref=TRUE); 

};
