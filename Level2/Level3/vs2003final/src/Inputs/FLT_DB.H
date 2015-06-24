#ifndef FLIGHT_CONSTRAINT_DATABASE_H
#define FLIGHT_CONSTRAINT_DATABASE_H

#include "common\template.h"
#include "inputs\con_db.h"
#include "../common/flt_cnst.h"


class INPUTS_TRANSFER FlightConstraintDatabase : public ConstraintDatabase
{
public:
    // find first entry that matches paxType
    // returns NULL if no constraint fits the paxType
    const ProbabilityDistribution *lookup (const FlightConstraint& p_type) const;

    // accessors for list elements
    const FlightConstraint *getConstraint (int p_ndx) const
        { return (FlightConstraint *)(getItem(p_ndx)->getConstraint()); }

    const FlightConstraint *getConstraint (ConstraintEntry* p_ndx) const
	{ 
		for(int i=0;i<getCount();i++)
		{
			if(getItem(i)==p_ndx)
				return (FlightConstraint *)(p_ndx->getConstraint()); 
		}
		return NULL; 
	}
	
    virtual void clear (void);

    // text based file I/O
    void readDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
    void writeDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const;
};

class INPUTS_TRANSFER FlightConstraintDatabaseWithSchedData : public FlightConstraintDatabase
{
public:
	FlightConstraintDatabaseWithSchedData();

	void readDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readObsoleteDatabase(ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void writeDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const;

	void SetNeglectSchedData(bool bNeglect){ m_bNeglectSchedData = bNeglect; }
	bool IsNeglectSchedData(){ return m_bNeglectSchedData; }

private:
	bool m_bNeglectSchedData;
};

#endif
