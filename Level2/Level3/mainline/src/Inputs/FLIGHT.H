#ifndef FLIGHT_H
#define FLIGHT_H

#include <string.h>
#include <CommonData\procid.h>
#include "common\flt_cnst.h"
#include "common\template.h"
#include "common\term_bin.h"
#include "common\containr.h"
#include "common\elaptime.h"
#include "common\util.h"
#include "common\ARCFlight.h"
//#include "BaseFlight.h"
#include "../common/ALTObjectID.h"
#include "..\common\EchoSystem.h"

#include <Inputs/InputsDll.h>

class Terminal;
class FlightGroup;
class CArrFlightOperation;
class CDepFlightOperation;
class CTowOffStandAssignmentDataList;


enum EXTRAAIRPORTTYPE
{
	NO_EXTRA_AIRPORT = 0,
	WITH_FROMTO,
	WITH_STOPOVER	
};

enum FlightFields
{
    AirlineField,
    ArrIDField,
    OrigField,
	ArrStopoverField,
	//ArrDateField, delete by adam 2007-04-24
    ArrTimeField,
    DepIDField,
	DepStopoverField,
    DestField,	
	//DepDateField,  delete by adam 2007-04-24
    DepTimeField,
    ACTypeField,
//	StarField,
//	SIDField,
    GateField,
    ServiceTimeField,	
	ArrGateField,
	DepGateField,
	GateTimeField		//added by emily, 2008-5-20
	//FLIGHT_FIELDS
};

extern char far *flightHeaders[];
#include<map>

class INPUTS_TRANSFER Flight :public ARCFlight
{
protected:
    static int sortBy;
	static int m_nNextUniqueID;	// default is 0, and tell what is the next id should be.

	int m_nUniqueID;			// this is the only key for one flight.

    //FlightDescStruct logEntry;

	ProcessorID baggageDevice;
    ElapsedTime lastCall;
    int curPax;                     // used for calc of deps only

///////////////////////////////////////////////////
//Add by Emily
//2008-5-15
///////////////////////////////////////////////////

	CAirportCode m_arrStopover;  
	CAirportCode m_depStopover; 

	ElapsedTime m_bagFirstBirthTimeInReclaim;
	ElapsedTime m_bagLastLeaveTimeInReclaim;

	double m_arrloadFactor;
	double m_deploadFactor;

	double m_arrLFInput;
	double m_depLFInput;
	int m_capacityInput;

	int m_capacity;
	short m_bagCarosel;

	short m_nDepTransit;		
	short m_nArrTransit;

	short m_nDepTransfer;		
	short m_nArrTransfer;

	ProcessorID m_ArrGate;
	ProcessorID m_DepGate;

	short m_nArrGateIdx;
	short m_nDepGateIdx;

	long m_ataTime;// ATA -1 do not have ata
	long m_atdTime;//ATD -1 do not have atd


	//short m_arrLoad;
	//short m_depLoad;
/////////////////////////////////////////////////////////////
	std::map<int,ElapsedTime>m_mapLastCallOfEveryStage;
	
	Terminal*	m_pTerm;


public:
	static void ResetUniqueID(){ m_nNextUniqueID = 0 ;} ;
    static void getFieldTitles (char *titleLine, int gateField = FALSE);
    static const char *getTitle (int index);
    static void setSortBy (int p_sortBy) { sortBy = p_sortBy; }
    static int getSortBy(void) {return sortBy;}

    Flight ( Terminal* _pTerm );
	Flight(const Flight& _flight);
    virtual ~Flight ();

    const Flight& operator = (const Flight& aFlight);
    int operator == (const Flight& aFlight) const { return this == &aFlight; }
    int operator < (const Flight& aFlight) const;

    /*****
    *
    *   Accessors
    *
    *****/
	void getArrStopover(char *p_str) const;
	void getDepStopover(char *p_str) const;	


	int getFlightIndex()const { return m_nUniqueID;}
	void SetFlightIndex(int _id) { m_nUniqueID = _id ;} ;
	ElapsedTime getLastCall (void) const { return lastCall; }


	ElapsedTime getFirtTime(void) const {return m_bagFirstBirthTimeInReclaim;}
	ElapsedTime getLastTime(void) const {return m_bagLastLeaveTimeInReclaim;}

    FlightConstraint getType (char p_mode) const;
    //FlightConstraint getType (void) const;
	
	const ProcessorID getArrGate (void) const ;
	const ProcessorID getDepGate (void) const ;
    const ProcessorID getBaggageDevice(void) const { return baggageDevice;}

	int getArrGateIndex(void) const ;
	int getDepGateIndex(void) const ;

	int getArrTransit(void){ return m_nArrTransit; }
	int getDepTransit(void){ return m_nDepTransit; }

	int getArrTransfer(void){return m_nArrTransfer; }
	int getDepTransfer(void){return m_nDepTransfer; } 

	int getSpace(char p_mode);

	int getBaggageCarosel(void) const { return m_bagCarosel; }
	ElapsedTime getFlightDelayTime( Terminal* _pTerm, char p_mode );

	const int getArrLoad(void) ;
	const int getDepLoad(void) ;

	void getGateOccupancyTime(ElapsedTime& time1, ElapsedTime& time2) const;

	FlightDescStruct getLogEntry (void) ;
	std::map<int,ElapsedTime>& GetLastCallOfEveryStage(){ return m_mapLastCallOfEveryStage;	}
	virtual enumFlightType getFlightType() { return TerminalFlight;}
    /*****
    *
    *   Modifiers
    *
    *****/

	void setArrStopover(const char *p_str);    
	void setDepStopover(const char *p_str);
	void setStopoverAirport(const char *p_str, char p_mode = 0);  

    void setLastCall(ElapsedTime p_time) { lastCall = p_time; }
    void setFirstTime(ElapsedTime p_time) { m_bagFirstBirthTimeInReclaim =(long)p_time;}
    void setLastTime(ElapsedTime p_time) {m_bagLastLeaveTimeInReclaim = (long)p_time;}

	void setArrTransit(int nNum){m_nArrTransit = nNum; }
	void setDepTransit(int nNum){m_nDepTransit = nNum; }

	void setArrTransfer(int nNum){m_nArrTransfer = nNum; }
	void setDepTransfer(int nNum){m_nDepTransfer = nNum; }

	void setArrGate(const ProcessorID& p_id) ;
	void setDepGate(const ProcessorID& p_id) ;

	virtual bool isSameGate();

	void setArrGateIndex(int p_ndx);
	void setDepGateIndex(int p_ndx);

	void setBaggageDevice(const ProcessorID&p_id){baggageDevice = p_id;}
	void setBaggageCarosel(int p_ndx) { m_bagCarosel = (short)p_ndx; }

    //void setLoad(int p_count, char p_mode);
	void setArrLoadFactor(double fFactor);
	void setDepLoadFactor(double fFactor);
	void setCapacity(int nCapacity);				

	void setArrLFInput(double fLF);
	void setDepLFInput(double fLF);
	void setCapacityInput(int nCapacity);

	double getArrLFInput() const;
	double getDepLFInput() const;
	int getCapacityInput() const;

	double getArrLoadfactor() const;
	double getDepLoadfactor() const;
	int getCapacity() const;

	//------------------------------------------------------------------------------
	//Summary:
	//		new member get and set property
	//------------------------------------------------------------------------------
	void SetAtaTime(long tTime);
	long GetAtaTime()const;
	
	void SetAtdTime(long tTime);
	long GetAtdTime()const;
	//------------------------------------------------------------------------------

	// create flight arr dep event from flight data
	bool createFlightEvent( Terminal* _pTerm );
	
    // used to calculate space on departure when generating hubbing ops
    int spaceAvail(void) ;
    void addDepPax(int p_count = 1) { curPax += p_count; }


    void readFlightParameter(ArctermFile& p_file, StringDictionary* _pStrDict, BOOL bHasDate = FALSE, CStartDate* psDate = NULL, EXTRAAIRPORTTYPE _enumExtraAirportType = NO_EXTRA_AIRPORT );
	void readObsoleteFlight (ArctermFile& p_file, StringDictionary* _pStrDict, BOOL bHasDate = FALSE, CStartDate* psDate = NULL, EXTRAAIRPORTTYPE _enumExtraAirportType = NO_EXTRA_AIRPORT );
	void readVersion1_0 (ArctermFile& p_file);
	void readVersion2_2 (ArctermFile& p_file ,StringDictionary* _pStrDict );
	void readFlight2_4(ArctermFile& p_file,StringDictionary* _pStrDict);
	void readFlight2_5(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);
	void readFlight2_6(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);    
	void readFlight2_7(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);    
	void readFlight2_8(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);
	//add by adam 2007-04-24
	void readFlight2_9(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);
	//add by emily 2008-5-23
	void readFlight3_0(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);
	//add by emily 2008-6-10
	void readFlight3_1(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);
	//add by ben
	void readFlight3_2(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);
	//add by emily
	void readFlight3_3(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);

	//---------------------------------------------------------------------------------------------
	//Summary:
	//		read latest version of file
	//---------------------------------------------------------------------------------------------
	void readFligt3_4(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);
	void readFligt3_5(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate = NULL);

	//End add by adam 2007-04-24
	//Modify by adam 2007-04-24
	//void writeFlight (ArctermFile& p_file, StringDictionary* _pStrDict, int p_flush, const CStartDate* psDate );
	void writeFlight (ArctermFile& p_file, StringDictionary* _pStrDict, int p_flush);
	//void printFlight (char *p_str, int p_printGate, StringDictionary* _pStrDict, const CStartDate* psDate);
	void printFlight (char *p_str, int p_printGate, StringDictionary* _pStrDict);
	void printFlight (char *p_str, char p_mode, int p_printGate, StringDictionary* _pStrDict);
	//End modify by adam 2007-04-24
	void displayFlight( char *p_str ) const;

	// echo the details of flights
	virtual void echo(EchoLogType key) const;
	bool isBelongToGroup( FlightGroup* pFlightGroup,bool bArrival);

	Terminal* GetTerminal() { return m_pTerm ;} 

	void ClearArrOperation();
	void ClearDepOperation();
private:
	void InitFlightOperationsFromLogEntry(FlightDescStruct& logEntry);
protected:
	ElapsedTime m_RealDepTime ; // the real time when the flight will departure .
	bool m_bNeedCheckRealDepTime;
public:
	ElapsedTime GetRealDepTime()const;
	void SetRealDepTime(const ElapsedTime& _time) { m_RealDepTime = _time ;} ;
	bool RetrieveAirsideDepartureTime()const {return m_bNeedCheckRealDepTime;}
	void NotifyCheckRealDepatureTime(){m_bNeedCheckRealDepTime = true;}

	void SetAirsideFlightDepartured(bool bDeparture);
	bool IsAirsideFlightDepartured() const;

private:
	bool m_bAirsideDepartured;


};

#endif
