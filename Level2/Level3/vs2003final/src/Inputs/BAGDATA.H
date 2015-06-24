/*****
*
*   Class       BaggageData
*   Author      Lincoln Fellingham
*   Date        May 1994
*   Purpose     Stores databases of information relating to baggage,
*               including baggage delay times and carosel assignments
*
*****/

#ifndef BAGGAGE_DATA_H
#define BAGGAGE_DATA_H

#include "common\template.h"
#include "common\dataset.h"
#include "inputs\flt_db.h"
#include "common\elaptime.h"
#include "GateLagTimeDB.h"

class INPUTS_TRANSFER BaggageData : public DataSet
{
protected:
    // stores distributions of baggage entry times
    FlightConstraintDatabase bagLagTime;

    // stores processor distributions of carosel assignments
    FlightConstraintDatabase assignments;

	
	CGateLagTimeDB m_dbGateLagTime;

public:
    BaggageData();
    virtual ~BaggageData () {};

    // returns a random arrival time for a bag on the passed flight
    ElapsedTime getBagLagTime (int p_ndx, FlightSchedule* _pFlightSchedule) const;

    // database accessors
    FlightConstraintDatabase *getLagTimeDatabase (void) { return &bagLagTime; }
	CGateLagTimeDB* getGateLagTimeDB(){ return &m_dbGateLagTime;}
    FlightConstraintDatabase *getAssignmentDatabase (void) { return &assignments; }

    virtual const char *getTitle (void) const
        { return "Data describing Baggage handling"; }
    virtual const char *getHeaders (void) const
        { return "Data,Flight,Units,Distribution,Parameters"; }

    // reads and writes baggage data to the passed file name in CSV
    virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    // empties the databases
    virtual void clear (void) { bagLagTime.clear(); assignments.clear(); }

	void BuildFlightAssignToBaggageDevice( FlightSchedule* _pFlightSchedule );
	void InputFlightAssignBaggageDevice( FlightSchedule* _pFlightSchedule );
};

#endif
