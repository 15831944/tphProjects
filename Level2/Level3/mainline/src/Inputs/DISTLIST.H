#ifndef PASSENGER_DISTRIBUTION_LIST_H
#define PASSENGER_DISTRIBUTION_LIST_H

#include "common\dataset.h"
#include "inputs\flt_db.h"

class PassengerDistributionList : public DataSet, public FlightConstraintDatabase
{
public:
    PassengerDistributionList () : DataSet (PassengerDistributionFile, (float)2.5){};//2.4) {};
    virtual ~PassengerDistributionList () {};
    void getDistName (int p_ndx, char *p_str) const;

    void initDefaultValues (void);
    void clear (void) { FlightConstraintDatabase::clear(); }

    virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Passenger Distributions file"; }
    virtual const char *getHeaders (void) const;

public:
	//init the data for engine
	//to avoid the duplicate initialize
	void ClearEngineInitializeFlag();
};

#endif
