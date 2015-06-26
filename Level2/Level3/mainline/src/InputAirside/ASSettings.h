#ifndef __AIRSIDE_SETTINGS_H
#define __AIRSIDE_SETTINGS_H

#import "..\..\lib\msado26.tlb" rename("EOF","adoEOF")
//#include "sqlexecutor.h"

/*
Parameters in accordance with dlgsetting
*/

namespace ns_AirsideInput
{
	//weatherTime settings
	class RunwayInfo;

	struct weatherTime
	{	
		int weatherType;
		double from_tm;
		double to_tm;
		
		int m_nID;          //unique ID	

		weatherTime();	
	};

	class vWeatherTimes : public std::vector <weatherTime> 
	{
	public:
		static CString m_strTableName;
		int m_nSettingsID;

		void ReadData();		
		void SaveData(vWeatherTimes&);
		void InsertData(weatherTime& weathTm);
		void UpdateData(weatherTime& weathTm);
		void DeleteData(weatherTime& weathTm);
		

		vWeatherTimes();
	};

	//dynamic runway assignment
	struct dynamicRWasgn{
		int assignType;         //operation type
		int assigncase;
		double effectiveTmFrom;
		double effectiveTmTo;

		int m_nID;             //self unique id	

		dynamicRWasgn();
	};

	class vRwAsigns :public std::vector <dynamicRWasgn> 
	{
	
	public:
		static CString m_strTableName;
		int m_nSettingsID;

		void ReadData();		
		void SaveData( vRwAsigns&);
		void InsertData(dynamicRWasgn& dyRwasgn);
		void UpdateData(dynamicRWasgn& dyRwasgn);
		void DeleteData(dynamicRWasgn& dyRwasgn);
	

		vRwAsigns();
	};
	
	//runway mode assign
	struct rwMode
	{
//		int Index;
		CString mark;
		int Mode;

		int m_nID;           //unique id

		rwMode();
	};

	class vRwModes :public std::vector<rwMode> 
	{
		static CString m_strTableName;
	public:	
		int m_nSettingsID;

		void ReadData();		
		void SaveData( vRwModes&);
		void InsertData( rwMode& rwMd );
		void UpdateData( rwMode& rwMd );
		void DeleteData( rwMode& rwMd );
	

		vRwModes();
		vRwModes(const RunwayInfo&);
	};

	class CSettings //:public CSQLExecutor
	{
	public:
		CSettings();
		
	public:
		
		//get sql for selection
		virtual CString GetSelectScript() const ;
		//get sql for update
		virtual CString GetUpdateScript() const ;
		//get sql for deletion
		virtual CString GetDeleteScript() const ;
		//get sql for insertion
		virtual CString GetInsertScript() const ;
		virtual void Execute(){}
		//save data
		void SaveData(CSettings&);
		//read the setting parameters from database
		void ReadData();
		//virtual void Execute();
		void DeleteData(){}
		
		bool IsDataValid(CSettings &);

		
		static CString m_strTableName;
		int m_nProjectID;

		vWeatherTimes m_vWeatherTm; 
		vRwAsigns m_vRwasgn;
		vRwModes m_vRwMd;
		//data
	private:
	
	public:
		double m_strt_t ;
		double m_stop_t ;
		double m_minr ;
		double m_headwind ;	
		bool m_virtual_ ;
		int m_hist_int ;
		bool m_usr_sw ;
		int m_nID;    //unique id

	
		void SetStartTime(double startTime){m_strt_t = startTime ;}
		double GetStartTime(){return m_strt_t;}

		void SetEndTime(double endTime){m_stop_t = endTime ;}
		double GetEndTime(){return m_stop_t;}

		void SetSchPercent(double schPercent){ m_minr = schPercent;}
		double GetSchPercent(){ return m_minr ;}

		void SetHeadWind(double headWind){m_headwind = headWind ;}
		double GetHeadWind(){ return m_headwind;}

		void SetDelayCharge(bool delayCharge){m_virtual_ = delayCharge;}
		bool GetDelayCharge(){ return m_virtual_ ; }

		void SetReportInterval(int reportInterval){ m_hist_int = reportInterval ;}
		int GetReportInterval(){ return m_hist_int;}

		bool GetActiveDecline(){ return m_usr_sw;}
		void SetActiveDecline(bool activeDecline){m_usr_sw = activeDecline;}
	};
	
}

#endif
