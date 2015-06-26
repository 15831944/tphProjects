#pragma once
#include "../Common/termfile.h"
#include <vector>
#include "../InputAirside/ALT_BIN.h"

class AirsideRunwayOperationLog
{

public:
	enum enumOperation
	{
		enumOperation_Landing = 0,
		enumOperation_Takeoff,
		enumOperation_MissApproach,
		enumOperation_Cross
	};
	enum enumEntryOrExit
	{
		enumEntryOrExit_Entry = 0,
		enumEntryOrExit_Exit
	};

	enum enumOpPosition
	{
		enumOpPosition_RunwayPort = 0,
		enumOpPosition_Intersection,

	};
	//this structure stored the airoute that flight comes from
	class RunwayLogAirRoute
	{
	public:
		RunwayLogAirRoute()
		{
			m_nRouteID = -1;
		}
		~RunwayLogAirRoute()
		{

		}

	public:
		//route id
		int m_nRouteID;
		//route name
		CString m_strRouteName;
	protected:
	private:
	};

	//a flights'runway log,
	//entry to exit
	class RunwayLogItem
	{
	public:
		RunwayLogItem();
		~RunwayLogItem();

	public:
		void WriteLog(ArctermFile& outFile);
		static RunwayLogItem* ReadLog(ArctermFile& inFile);


		//data operation
	public:
		void SetTime(ElapsedTime eTime){m_eTime = eTime;}


		void SetRunway(int nRunwayID,RUNWAY_MARK runwayMark,const CString& strMarkName)
		{
			m_nRunwayID = nRunwayID;
			m_enumRunwayMark = runwayMark;
			m_strMarkName = strMarkName;
		}

		void SetFlightIndex(int nIndex){m_nFlightIndex = nIndex;}

		void SetFlightID(const CString& strFlightID){m_strFlightID = strFlightID;}

		void SetFlightType(const CString& strFlightType){ m_strFlightType = strFlightType;}

		void SetEnumOperation(enumOperation enumOp){m_enumOperation = enumOp;}

		void SetEnumEntryOrExit(enumEntryOrExit enumState){m_enumEntryOrExit = enumState;}

		void SetPossibleExitInformation(int nExitID, const CString& strExitName, ElapsedTime eExitTime);

		void AddAirRoute(const RunwayLogAirRoute& airRoute);

		//return airrote name list
		CString GetAirRouteNames();

	public:
		//time
		ElapsedTime m_eTime;
		//runway ID in input
		int m_nRunwayID;

		//runway mark index, first or
		//	RUNWAYMARK_FIRST = 0,
		//	RUNWAYMARK_SECOND,
		RUNWAY_MARK m_enumRunwayMark;

		//runway mark name
		CString m_strMarkName;

		//flight index
		int m_nFlightIndex;

		//flight ID
		CString m_strFlightID;

		//flight type
		CString m_strFlightType;

		//runway operation
		enumOperation m_enumOperation;

		//entry or exit runway
		enumEntryOrExit m_enumEntryOrExit;
		
		//possible exit ID, name
		int m_nPossibleExitID;
		CString m_strPossibleExitName;

		//possible exit time, it is minimum occupy time using max breaking 
		ElapsedTime m_ePossibleTime;

		//the airroute that the flight passed by
		std::vector<RunwayLogAirRoute> m_vAirRoute;
		

		//enumPositionType
		//the object type 
		//enumOpPosition m_enumPosition;

		//PositionID, object
		//the object id entry form or exit to
		//int m_nObjectID;

		//positionName
		//the object entry from or exit to
		//CString m_strOpObject;

	};

public:
	AirsideRunwayOperationLog(void);
	~AirsideRunwayOperationLog(void);


	//open log file, if not exist create one , else clear old data;
	bool CreateLogFile( const CString& _csFullFileName);
	void LoadData(const CString& _csFullFileName);
	void SaveData(const CString& _csFullFileName);


	//write delay to log ,assert log file is open
	void WriteLogItem(RunwayLogItem* pDelay);


	void ClearData();
	int  GetItemCount()const;
	RunwayLogItem* GetItem(int idx);
	void AddItem(RunwayLogItem* pDelay);
protected:
	ArctermFile m_LogFile;
	std::vector<RunwayLogItem*> m_vLogItem;
};
