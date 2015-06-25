#pragma once
#include "parameters.h"
#include "AirsideReportNode.h"
class AIRSIDEREPORT_API CAirsideRunwayUtilizationReportParam :
	public CParameters
{
public:
	enum RunwayOperation
	{
		Both = 0,
		Landing = 1,
		TakeOff = 2
	};
	class AIRSIDEREPORT_API CAirRouteParam
	{
	public:
		CAirRouteParam()
		{
			m_nRouteID = -1;

		}
		~CAirRouteParam()
		{

		}
	public:
		
		int m_nRouteID;
		CString m_nRouteName;
	};

	class AIRSIDEREPORT_API CRunwayOperationParam
	{
	public:
		CRunwayOperationParam()
		{

		}
		~CRunwayOperationParam()
		{

		}
	public:
		//return operation name
		CString GetOperationName();

		bool fitAirRoute(const CAirRouteParam& airRoutePassed);
		bool fitFltCons(const AirsideFlightDescStruct& fltConsPassed, char mode);

		//file operation, save and read parameter
	public:
		virtual void WriteParameter(ArctermFile& _file);
		virtual void ReadParameter(ArctermFile& _file,CAirportDatabase *pAirportDatabase );

	public:
		bool IsDefault();
		bool IsAirRouteDefault();
		bool IsFltConsDefault();
		//landing,take off or both
		RunwayOperation m_enumOperation;

		std::vector<CAirRouteParam> m_vAirRoute;

		std::vector<FlightConstraint> m_vFlightConstraint;
	};
	class AIRSIDEREPORT_API CRunwayMarkParam
	{
	public:
		//return runway mark name, shows in the gui
		CString GetRunwayNames();

		bool IsRunwayMarkFit( CAirsideReportRunwayMark& reportRunwayMark );
	public:
		std::vector<CAirsideReportRunwayMark> m_vRunwayMark;
		std::vector<CRunwayOperationParam> m_vOperationParam;
	

		bool IsDefault();
		//file operation, save and read parameter
	public:
		virtual void WriteParameter(ArctermFile& _file);
		virtual void ReadParameter(ArctermFile& _file,CAirportDatabase *pAirportDatabase );



	};

public:
	CAirsideRunwayUtilizationReportParam(void);
	virtual ~CAirsideRunwayUtilizationReportParam(void);

public:
	virtual void LoadParameter();
	virtual void UpdateParameter();

public:
	std::vector<CRunwayMarkParam> m_vReportMarkParam;

	//file operation, write the data to file
public:
	virtual BOOL ExportFile(ArctermFile& _file) ;
	virtual BOOL ImportFile(ArctermFile& _file) ;	

	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);
	//read and write parameter data
protected:
	virtual CString GetReportParamName();
	
public:
	//check the parameter is default or not
	bool IsRunwayParamDefault();

};
