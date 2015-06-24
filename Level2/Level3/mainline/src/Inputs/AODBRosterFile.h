#pragma once
#include "aodbbasefile.h"

class  InputTerminal;
namespace AODB
{

	class RosterItem : public AODB::BaseDataItem
	{
	public:
		enum RosterItemType
		{
			PROC_TYPE,
			START_TIME_TYPE,
			END_TIME_TYPE,
			AIRLINE_TYPE,
			FLIGHTNUM_TYPE
		};
		RosterItem();
		~RosterItem();

	public:
		void ReadData(CSVFile& csvFile);
		COleDateTime GetEearistDate();

		void Convert( ConvertParameter& pParameter );

		bool ImportRoster(InputTerminal *pTerminal);

		MapField* GetMapFile(RosterItemType type);
		bool valid()const;
	public:
		RosterProcMapField m_proc;
		DateTimeMapField m_startTime;
		DateTimeMapField m_endTime;

		AirlineMapField m_strAirline;
		FlightIDMapField m_strFltNumber;


	protected:
	private:
	};



class RosterFile :
	public BaseFile
{
public:
	RosterFile(void);
	~RosterFile(void);

public:
	virtual void ReadData(CSVFile& csvFile);
	virtual void WriteData(CSVFile& csvFile);

public:
	COleDateTime GetStartDate(COleDateTime earliestDate);

	RosterItem *GetItem(int nIndex);

	void Convert( ConvertParameter& pParameter );

	bool ImportRoster(InputTerminal *pTerminal);

};
}