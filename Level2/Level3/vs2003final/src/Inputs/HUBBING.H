#ifndef HUBBING_SPECIFICATIONS_H
#define HUBBING_SPECIFICATIONS_H


#include "common\template.h"
#include "common\dataset.h"
#include "common\elaptime.h"
#include "inputs\pax_db.h"
#include "inputs\MobileElemConstraintDatabase.h"


class INPUTS_TRANSFER HubbingSpecs : public DataSet
{
protected:
	CMobileElemConstraintDatabase transiting;
    CMobileElemConstraintDatabase transferring;
    CMobileElemConstraintDatabase minTransferTime;
    CMobileElemConstraintDatabase transferWindow;
    CMobileElemConstraintDatabase transferDatabase;



public:
    HubbingSpecs();
    virtual ~HubbingSpecs() {};

    virtual void initDefaultValues (void);
    virtual void clear (void);

    virtual void readData (ArctermFile& p_file);
    virtual void readObsoleteData( ArctermFile& p_file );
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Data describing operations of transiting and transferring passengers"; }
    virtual const char *getHeaders (void) const
        { return "Variable,Pax Type,Units,Distribution,Parameters"; }

    float getTransitPercent (const CMobileElemConstraint& p_type) const;
    float getTransferPercent (const CMobileElemConstraint& p_type) const;
    ElapsedTime getMinTransferTime (const CMobileElemConstraint& p_type) const;
	ElapsedTime getTransferWindow (const CMobileElemConstraint& p_type) const;

//    Flight *getDepartingFlight (const CMobileElemConstraint& p_type,
//        int p_ndx) const;
    Flight *getRandomDeparture (const CMobileElemConstraint& p_type,
        int p_ndx, FlightSchedule* _pFlightSchedule) const;

    // delete all references to the passed index at p_level
    void deletePaxType (int p_level, int p_index);

    // removes intrinsic type from all constraints
	 void wipeIntrinsic (CMobileElemConstraintDatabase &p_db);

    CMobileElemConstraintDatabase *getTransiting (void) { return &transiting; }
    CMobileElemConstraintDatabase *getTransferring (void) { return &transferring; }
    CMobileElemConstraintDatabase *getMinTransferTime (void) { return &minTransferTime; }
    CMobileElemConstraintDatabase *getTransferWindow (void) { return &transferWindow; }
	CMobileElemConstraintDatabase *getTransferDatabase (void) { return &transferDatabase; }

	 //Jan98
	 virtual CMobileElemConstraintDatabase* getPassengerConstraintDatabase(int);

protected:	
	void ReadVersion10( ArctermFile& p_file );


};
  

#endif
