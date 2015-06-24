#ifndef FLIGHT_SCHEDULE_H
#define FLIGHT_SCHEDULE_H

#include "common\template.h"
#include "common\dataset.h"
#include "common\containr.h"
#include "inputs\flight.h"
#include "inputs\fltlist.h"

#include "../Reports/SimGeneralPara.h"
#include "..\common\EchoSystem.h"

class ALTObjectID;
class CTowOffStandAssignmentDataList;

class INPUTS_TRANSFER FlightSchedule : public UnsortedContainer<Flight>,
    public DataSet, public ListOfFlights
{
private:
	float m_fileVersion;
	CTowOffStandAssignmentDataList* m_pTowingData;

	bool GetMinStartDateFromFile(ArctermFile& p_file, CStartDate& psDate);
	bool GetStartDateEndDateFromFileFor2_9Versioin(ArctermFile& p_file, CStartDate& pBenchMarkDate, 
		                                           CStartDate& psDate, CStartDate& pEndDate, ElapsedTime& estFlightScheduleEndTime);   //add by adam 2007-05-29

protected:
	CStartDate m_startDate;
	CStartDate m_EndDate; // add by aivin.
	ElapsedTime m_UserStartTime;
	ElapsedTime m_UserEndTime;
	ElapsedTime m_estFlightScheduleEndTime;  //add by adam 2007-05-31
	//int nPrjID;


public:
	BOOL NeedSimulateFlight(const Flight* _pFlight, const COleDateTime& _lhs, const COleDateTime& _rhs);
	BOOL NeedSimulateFlight(const Flight* _pFlight, long _lStart, long _lEnd, int _nDayIndex, int& _nOutput);
	BOOL RegenerateSchedule(const CSimGeneralPara* pSimPara, FlightSchedule& fs);
	void SetStartDate(const CStartDate& _date);
	const CStartDate& GetStartDate() const;
	const CStartDate& GetEndDate() const;
	void SetEndDate(const CStartDate &_date);
	
	//add by adam 2007-05-31
	void SetFlightScheduleEndTime(const ElapsedTime& estEndTime);
	ElapsedTime& GetFlightScheduleEndTime();
	//End add by adam 2007-05-31

    FlightSchedule();
	    //End Modify by adam 2007-04-24
    virtual ~FlightSchedule();

	virtual void loadDataSet (const CString& _pProjPath);

	bool IsWantToAssignedBagDevice(void) { return m_fileVersion <= 2.3;}
    // CSV and binary file I/O
    virtual void readData (ArctermFile& p_file);
    virtual void readObsoleteData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Flight Schedule file"; }
    virtual const char *getHeaders (void) const;

    virtual void clear (void) { UnsortedContainer<Flight>::clear(); }

    void echoFlightSchedule (const char *p_filename) const;
    void writeBinarySchedule (FlightLog *p_list) const;

    // sets passenger loads and flight delays base on the random number
    void updateFlights (const FlightData *data);
	void updateScheduleEndDay();

    // assigns baggage carosels to all arriving flights
    void assignBaggageCarosels (const BaggageData *bagData,InputTerminal *_inTerm);

    // sorts all flights
    void sortFlights (void);
    void sortFlights(SortedContainer<Flight>&flightSortSchedule);
    // switches locations of flights at passed indexes
    void swapFlights (int firstIndex, int secondIndex);

    void getFlightID (char *p_id, int p_ndx, char _flightMode = 'T' ) const;
    int getFlightIndex (const char *flightID, char _flightMode = 'T'  ) const;
    void setFlight (const Flight *aFlight, int flightIndex);
    int getNextFlightSameCarrier (int index) const;

    // to be checked
    int getAllCarriersFlights (const char *carrier, int *indexes,
        ElapsedTime start, ElapsedTime end) const;

    Flight *getRandomDeparture (ElapsedTime start, ElapsedTime end) const;

    Flight *getDeparture (ElapsedTime p_start, ElapsedTime p_end,
        const FlightConstraint &p_type) const;

    // methods used to fill out FlightSelectionTool lists
    void getFlightList (StringList& p_strDict,
        const FlightConstraint &p_type);

	CTowOffStandAssignmentDataList* getTowOffDataList();

    // gate assignment methods
	//// replaced with new stand assign
 //   void clearAllGates ( InputTerminal* pInput);
 //   void clearGates (const ProcessorID& id);
 //   void getAllGates (StringDictionary& strDict) const;
	bool IfUsedThisGate( const ProcessorID& _gateID )const;
	void initGateIndexes (const ProcessorList *procList/*,const ProcessorList *airSideProcList*/);	
	//int assignFlightToGate (Flight *p_flight, const ProcessorID& p_gate,
 //       ElapsedTime p_buffer);
	bool getUsedProcInfo(const ProcessorID&p_id,InputTerminal *_term,std::vector<CString>&_stringList);
 //   // removes and deallocates all entries that fit passed procID
    void removeAllEntries (const ProcessorID& p_id,InputTerminal *_pTerm);
 //  
 //   // finds all entries that fit passed oldID and changes them to newID
	void replaceAllEntries (const ProcessorID& p_old,const ProcessorID& p_new,InputTerminal* _term);
	
	//new stand assign
	void clearAllStands( );
	void clearStands (const ProcessorID& id);	
	bool IfUsedThisStand( const ALTObjectID& _standID )const;
	int assignFlightToStand (Flight *p_flight, const ALTObjectID& p_stand,ElapsedTime p_buffer);
	//bool getUsedProcInfo(const ProcessorID&p_id,InputTerminal *_term,std::vector<CString>&_stringList);
	void initStandIndexes(int nAirportID, int nPrjID) ;	
	//end new stand assign

	
	// create FlightArrDepEvent from flightschedule
	int createFlightEvent( Terminal* _pTerm );

	void SetUserStartTime(const ElapsedTime& _date);
	const ElapsedTime& GetUserStartTime() const;
	void SetUserEndTime(const ElapsedTime& _date);
	const ElapsedTime& GetUserEndTime() const;
//	void SetStartDate(const COleDateTime &_date);

	// echo the details of flights
	virtual void echo(EchoLogType key) const;
	Flight* GetFlightByFlightIndex(int Fli_Index) ;
	
};


#endif // FLIGHT_SCHEDULE_H
