#ifndef PASSENGER_CONSTRAINT_DATABASE_H
#define PASSENGER_CONSTRAINT_DATABASE_H

#include "common\template.h"
#include "inputs\con_db.h"
#include "inputs\pax_cnst.h"


class INPUTS_TRANSFER CPassengerConstraintDatabase : public ConstraintDatabase
{
public:
	// find entry that equals p_const
	// returns NULL if no constraint fits the p_const
	const ProbabilityDistribution *FindEqual(const CPassengerConstraint& p_const) const;
//	ProbabilityDistribution *FindEqual(const CPassengerConstraint& p_const);
	

    // find first entry that matches p_const
    // returns NULL if no constraint fits the p_const
    const ProbabilityDistribution *lookup (const CPassengerConstraint& p_const) const;

    // accessors for list elements
    const CPassengerConstraint *getConstraint (int p_ndx) const
        { return (CPassengerConstraint *)(getItem(p_ndx)->getConstraint()); }

    // delete all references to the passed index at p_level
    void deletePaxType (int p_level, int p_index);
	void deletePaxType( const CPassengerConstraint* _paxConstraint );

	void deletePipe ( int _iPipeIdx );
	bool IfUseThisPipe( int _iPipeIdx )const;

    // text based file I/O
    void readDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void writeDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const;
	virtual int checkLastOfProbs();
};

#endif
