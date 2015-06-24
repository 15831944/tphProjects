#ifndef PASSENGER_DATA_H
#define PASSENGER_DATA_H

#include "common\template.h"
#include "common\dataset.h"
#include "inputs\pax_db.h"
#include "inputs\MobileElemConstraintDatabase.h"

class CMobileConWithProcIDDatabase;

#define PAX_DATA_COUNT  12        //8	//9

enum PaxDataTypes
{
    //BagCountDatabase,
    //CartCountDatabase,
    GroupSizeDatabase,
    InStepDatabase,
    //LeadLagTimeDatabase,
    SideStepDatabase,
    SpeedDatabase,
    //VisitorCountDatabase,
    VisitTimeDatabase,
	ResponseTimeDatabase,
	ImpactEmerDatabase,
	PushPropensityDatabase,
	ImpactSpeedDatabase,
	ImpactInStepDatabase,
	ImpactSideStepDatabase,            //add by adam 2007-05-14
	VisitorSTATrigger
};


class PassengerData : public DataSet
{
protected:
    static char * far paxDataName[PAX_DATA_COUNT+1];
    static char * far units[PAX_DATA_COUNT+1];

    typedef CMobileElemConstraintDatabase * CMobileElemConstraintDatabasePointer;
    CMobileElemConstraintDatabasePointer paxData[PAX_DATA_COUNT];
	// LeadLagTimeDatabae
	CMobileConWithProcIDDatabase* pLeadLagData;					// add by bird 2003/8/14

	typedef CNoPaxConstraintCoutDatabase* CNoPaxConstraintCoutDatabasePointer;
	CNoPaxConstraintCoutDatabasePointer* nopaxData;				//add by bird 2003-1-9
	int iNopaxCount;											//add by bird 2003-1-9
public:															
	void readNoPaxData(ArctermFile& p_file);					//add by bird 2003-1-9
	void readObsoleteNoPaxData( ArctermFile& p_file, float _fVersion ) ;
	void readErrorNoPaxData( ArctermFile& p_file, float _fVersion  );
	void writeNoPaxData(ArctermFile& p_file) const;				//add by bird 2003-1-9

    PassengerData();
    virtual ~PassengerData();

	CMobileElemConstraintDatabase *getNopaxData(int ndx) const; //add by bird 2003-1-9

    CMobileElemConstraintDatabase *getPaxData (int ndx) const {  assert( ndx>=0 && ndx< PAX_DATA_COUNT ); return paxData[ndx]; }
    const char *getDataName (int ndx) const { assert( ndx>=0 && ndx< PAX_DATA_COUNT ); return paxDataName[ndx]; }
    char **getDataNames (void) const { return paxDataName; }

//? no one use it.    int getHelp (int p_ndx) const;
    int getHelpPages (int) const { return 4; }
//? no one use it.    char getMode (int p_ndx) const;

    // delete all references to the passed index at p_level
    void deletePaxType (int p_level, int p_index);

    //PassengerConstraintDatabase *getBags (void) const
    //    { return paxData[BagCountDatabase]; }
    //PassengerConstraintDatabase *getCarts (void) const
    //    { return paxData[CartCountDatabase]; }
    CMobileElemConstraintDatabase *getGroups (void) const
        { return paxData[GroupSizeDatabase]; }
    CMobileElemConstraintDatabase *getInStep (void) const
        { return paxData[InStepDatabase]; }
    //CMobileElemConstraintDatabase *getLeadLagTime (void) const
    //    { return paxData[LeadLagTimeDatabase]; }
	CMobileConWithProcIDDatabase* getLeadLagTime (void) const
		{ return pLeadLagData; }
    CMobileElemConstraintDatabase *getSideStep (void) const
        { return paxData[SideStepDatabase]; }
    CMobileElemConstraintDatabase *getSpeed (void) const
        { return paxData[SpeedDatabase]; }
    //PassengerConstraintDatabase *getVisitors (void) const
    //    { return paxData[VisitorCountDatabase]; }
    CMobileElemConstraintDatabase *getVisitTime (void) const
        { return paxData[VisitTimeDatabase]; }
	CMobileElemConstraintDatabase *getResponseTime( void ) const
		{ return paxData[ResponseTimeDatabase];}
	CMobileElemConstraintDatabase *getImpactEmer(void) const
		{return paxData[ImpactEmerDatabase];}
	CMobileElemConstraintDatabase *getPushPropensity(void) const
		{return paxData[PushPropensityDatabase];}

/***********************************************************/
//add by porter
    CMobileElemConstraintDatabase *getImpactSpeed(void) const
	{
		return paxData[ImpactSpeedDatabase];
	}

	CMobileElemConstraintDatabase *getImpactInStep(void) const
	{
		return paxData[ImpactInStepDatabase];
	}
/*********************************************************/

	//add by adam 2007-05-14
	CMobileElemConstraintDatabase *getImpactSideStep(void) const
	{
		return paxData[ImpactSideStepDatabase];
	}
	//end add by adam 2007-05-14

	CMobileElemConstraintDatabase *getVisitorSTATrigger(void) const
	{
		return paxData[VisitorSTATrigger];
	}
    virtual void initDefaultValues (void);
    virtual void clear (void);

    virtual void readData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;
	virtual void readObsoleteData ( ArctermFile& p_file );

    virtual const char *getTitle (void) const
        { return "Miscellaneous Passenger Data"; }
    virtual const char *getHeaders (void) const
        { return "Variable,Pax Type,Units,Distribution,Parameters"; }

protected:
	void initNoPaxData();				//add by bird 2003-1-9
	void delNoPaxData();				//add by bird 2003-1-9
};

#endif
