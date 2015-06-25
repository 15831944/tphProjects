#ifndef FLIGHT_DATA_H
#define FLIGHT_DATA_H

#include "inputs\flt_db.h"
#include "inputs\pax_db.h"
#include "common\dataset.h"
#include "common\elaptime.h"
#include "inputs\FlightConWithProcIDDatabase.h"

class BoardingCallTimeAndProportion
{
public:
	ElapsedTime m_time;
	double m_proportion;
	BoardingCallTimeAndProportion(){ m_time = 0L; m_proportion = 0.0; }
	~BoardingCallTimeAndProportion(){}
};
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

	// go through all the file get the size of boarding call stage size;
	int ReadBoardingCallStageNumber( ArctermFile& _file );
	void WriteBoardingCallStageNumber( ArctermFile& _file, int _nNumStages ) const;
	

public:
	void resortBoardingCallDB();
	void DeleteOneStage(ConstraintDatabase* pFCDB);
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
	void DeleteOneStage( int _nStageNumber );

	int getBoardingCallsStageSize(){ return m_vCalls.size(); }

    void setFlightDelay (Flight *p_flight) const;
    void setFlightLoadFactor (Flight *p_flight, char p_mode) const;
    int getCapacity (Flight *p_flight) const;
    const HistogramDistribution *getBoardingCalls (const FlightConstraint &p_type, const ProcessorID& _id, int _nStageNumber ) const;

    FlightConWithProcIDDatabase *getCalls ( int _nStage ){ return m_vCalls[_nStage]; }
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
