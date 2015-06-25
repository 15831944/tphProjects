#ifndef FLIGHT_DATA_H
#define FLIGHT_DATA_H

#include "inputs\flt_db.h"
#include "inputs\pax_db.h"
#include "common\dataset.h"
#include "common\elaptime.h"
#include "inputs\FlightConWithProcIDDatabase.h"
#include "BoardingCallFlightTypeDatabase.h"

class INPUTS_TRANSFER FlightData : public DataSet
{
protected:
    FlightConstraintDatabase delays;
    FlightConstraintDatabaseWithSchedData loads;
    FlightConstraintDatabaseWithSchedData capacity;
	std::vector<FlightConWithProcIDDatabase*> m_vCalls;	// 0: stage 1
														// 1: stage 2
														// 2: stage 3
														// ......
	std::vector<BoardingCallFlightTypeDatabase*> m_vBoardingCallStages;
	// go through all the file get the size of boarding call stage size;
	int ReadBoardingCallStageNumber( ArctermFile& _file );
	void WriteBoardingCallStageNumber( ArctermFile& _file, int _nNumStages ) const;
	

public:
	int GetStageCount(){ return m_vBoardingCallStages.size(); }
	BoardingCallFlightTypeDatabase* GetFlightTypeDBInStage(int _index){ return m_vBoardingCallStages[_index]; }
	void resortBoardingCallDB();
    FlightData () : DataSet (FlightDataFile,(float)2.6 )
    {	
		delays.setUnits ("MINUTES"); 
		loads.setUnits ("PERCENT");
		capacity.setUnits ("COUNT"); 
	}
    virtual ~FlightData ();


	// Add one more stages with default value
	void AddStage();

	// remove one stage data
	void DeleteOneStageByIndex(int _stageIndex);
	BOOL DeleteOneStageByFlightConstDB(ConstraintDatabase* pFCDB);

	int getBoardingCallsStageSize(){ return m_vCalls.size(); }

    void setFlightDelay (Flight *p_flight) const;
    void setFlightLoadFactor (Flight *p_flight, char p_mode) const;
    int getCapacity (Flight *p_flight) const;
    const HistogramDistribution *getBoardingCalls (const FlightConstraint &p_type, const ProcessorID& _id, int _nStageNumber ) const;

    FlightConWithProcIDDatabase *getCalls ( int m_nStage ){ return m_vCalls[m_nStage-1]; }
    FlightConstraintDatabase *getDelays (void) { return &delays; }
    FlightConstraintDatabase *getLoads (void) { return &loads; }
    FlightConstraintDatabase *getCapacity (void) { return &capacity; }

    virtual void initDefaultValues (void);
    virtual void clear (void);


    virtual void readData (ArctermFile& p_file);
    virtual void readObsoleteData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Flight Data File"; }
    virtual const char *getHeaders (void) const
        { return "Data,Flight,Units,Distribution,Parameters"; }
};

#endif
