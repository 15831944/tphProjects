#ifndef FLIGHT_TYPE_DISTRIBUTION_H
#define FLIGHT_TYPE_DISTRIBUTION_H

#include "../common/ProbabilityDistribution.h"
#include "../common/template.h"
#include "../common/containr.h"
#include "../common/flt_cnst.h"
#include "TransferFlightConstraint.h"

#define RANDOM      0
#define SEQUENTIAL  1

class FlightTypeDistribution : public EmpiricalDistribution
{
protected:
	UnsortedContainer<CTransferFlightConstraint> destList;
    int destIndex;

private:
	virtual void readDistribution(ArctermFile& p_file);

public:
    FlightTypeDistribution() : destList (4, 0, 4) { destIndex = 0; }
    virtual ~FlightTypeDistribution () {};


//    void initDistribution (FlightConstraint *p_list,
//        const unsigned char *p_percents, int p_count);

  //  void addRow (FlightConstraint *p_group, unsigned char p_percent);
	void addRow (CTransferFlightConstraint *p_group, unsigned char p_percent);
    void deleteRow (int p_ndx);

    const char *getProbabilityName (void) const { return "Flight_Type"; }
    virtual int getProbabilityType (void) const { return FLIGHT_TYPE; }
    
	virtual void readDistribution (ArctermFile& p_file, InputTerminal* _pInTerm);
    virtual void readVersion1 (ArctermFile& p_file, InputTerminal* _pInTerm);
    virtual void writeDistribution (ArctermFile& p_file) const;

    virtual void screenPrint (char *p_str) const;

    // returns a destination group index
    // if the flag RANDOM is passed the probability distribution is used
    // if the flag SEQUENTIAL is used the groups are returned in order
    // when using SEQUENTIAL selection, NULL is returned when all groups
	// in the tranistion row have been returned once
    //const FlightConstraint *getDestinationGroup (int p_flag);
    CTransferFlightConstraint *getDestinationGroup (int p_flag);


    //void setGroup (const FlightConstraint &p_group, int p_ndx)
        //{ *(destList.getItem (p_ndx)) = p_group; }
	void setGroup (const CTransferFlightConstraint &p_group, int p_ndx)
	{ *(destList.getItem (p_ndx)) = p_group; }
    CTransferFlightConstraint *getGroup (int p_ndx) const
		  { return destList.getItem (p_ndx); }

	 //Jan98
    int getLastOfProbs();
	
};

#endif
