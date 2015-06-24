#pragma once
#include "aodbbasefile.h"

class Flight;
class InputTerminal;
namespace AODB
{

	class ScheduleItem : public BaseDataItem
	{
	public:
		ScheduleItem();
		~ScheduleItem();

	enum ScheduleItemType
	{
		ARRIVAL_DAY_TYPE,//SDA
		AIRLINE_TYPE,
		ARRID_TYPE,
		ARRINTSTOP_TYPE,
		ORIGIN_TYPE,
		ARRTIME_TYPE,//STA
		ATATIME_TYPE,//ATA
		ADATIME_TYPE,//ADA
		DEPID_TYPE,
		DEPINTSTOP_TYPE,
		DEST_TYPE,
		DEPTIME_TYPE,//STD
		DEPDAY_DAY_TYPE,//SDD
		ATDTIME_TYPE,//ATD
		ADDTIME_TYPE,//ADD
		ACTYPE_TYPE,
		ARRIVAL_STAND_TYPE,
		DTOWOFF_TYPE,//D tow off
		TTOWOFF_TYPE,//T tow off
		DEP_STAND_TYPE,
		INT_STAND_TYPE,
		DEXINT_STAND_TYPE,//D ex int stand
		TEXINT_STAND_TYPE,//T ex int stand
		STAY_TIME_TYPE,
		ARRIVAL_GATE_TYPE,
		DEP_GATE_TYPE,
		BAG_DEVICE_TYPE,
		ARRLOAD_FACTOR_TYPE,
		DEPLOAD_FACTOR_TYPE,
		SEAT_TYPE
	};
	void ReadData(CSVFile& csvFile);
	
	COleDateTime GetEearistDate();

	MapField* GetMapField(ScheduleItemType type);
	void Convert( ConvertParameter& pParameter );

	//check the item is valid or not
	bool ConvertToFlight(ConvertParameter& pParameter );
	bool valid()const;

	Flight* m_pFlight;
	
	public:
		//arrival date
		DateMapField m_arrDate;

		//airline
		AirlineMapField m_strAirline;
		
		//arrival id
		FlightIDMapField m_strArrID;

		//intermediates stop
		AirportMapField m_strArrIntStop;

		//original airport code
		AirportMapField m_strOrigin;

		TimeMapField m_arrTime;

		//departure id
		FlightIDMapField m_strDepID;
	
		AirportMapField m_strDepIntStop;

		//dest airport code
		AirportMapField m_strDest;

		TimeMapField m_depTime;

		DateMapField m_depDate;

		//aircraft type
		ACTypeMapField m_strACType;

		//arrival stand
		StandMapField m_arrStand;
		
		//departure stand
		StandMapField m_depStand;
		
		//intermediate stand
		StandMapField m_intStand;

		/*TimeMapField*/IntMapField m_stayTime;

		ArrGateMapField m_arrGate;

		DepGateMapField m_depGate;

		BaggageDeviceMapField m_bageDevice;

		DoubleMapField m_dArrLoadFactor;

		DoubleMapField m_deDepLoadFactor;
		
		IntMapField m_nSeats;

		//new version
		TimeMapField m_ataTime;//ATA
		DateMapField m_ataDay;//ADA

		TimeMapField m_atdTime;//ATD
		DateMapField m_atdDay;//ADD

		TimeMapField m_towOffTime;//T tow off
		DateMapField m_towOffDay;// D tow off

		TimeMapField m_towOffExTime;//T ex tow off
		DateMapField m_towOffExDay;//D ex tow off

	protected:
		//airline + flight number, if not valid, fatal error
		bool IsCodeValid() const;

		//arrival part
		bool IsArrivalValid() const;

		//departure valid
		bool IsDepartureValid() const;
	};

	//schedule file
	class ScheduleFile :
		public BaseFile
	{
	public:
		ScheduleFile(void);
		~ScheduleFile(void);

	public:
		virtual void ReadData(CSVFile& csvFile);
		virtual void WriteData(CSVFile& csvFile);

	

	public:
		//return the item at index
		//GetCount() to get the count of items
		ScheduleItem *GetItem(int nIndex);

		COleDateTime GetStartDate(COleDateTime earliestDate);

		void Convert( ConvertParameter& pParameter );

		bool ReplaceSchedule(InputTerminal *pTerminal);

	protected:
	};
}




