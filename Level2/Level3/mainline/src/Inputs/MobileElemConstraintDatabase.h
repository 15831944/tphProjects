// MobileElemConstraintDatabase.h: interface for the CMobileElemConstraintDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOBILEELEMCONSTRAINTDATABASE_H__AB994609_6ED7_4232_B31B_35480F05463B__INCLUDED_)
#define AFX_MOBILEELEMCONSTRAINTDATABASE_H__AB994609_6ED7_4232_B31B_35480F05463B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CON_DB.H"
#include "common\elaptime.h"
#include "inputs\MobileElemConstraint.h"
#include "inputs\MobileElemConstraintDatabase.h"

class CMobileElemConstraintDatabase : public ConstraintDatabase  
{
public:
	CMobileElemConstraintDatabase();
	virtual ~CMobileElemConstraintDatabase();



	// find entry that equals p_const
	// returns NULL if no constraint be found
	const ProbabilityDistribution *FindEqual(const CMobileElemConstraint& p_const) const;


	// find first entry that fits p_const
	// returns NULL if no constraint fits the p_const
	const ProbabilityDistribution* FindFit( const CMobileElemConstraint& p_const ) const;
	const ProbabilityDistribution* FindFitEx( const CMobileElemConstraint& p_const, const ElapsedTime& _curTime ) const;

	// find first entry that fits p_const
	// returns NULL if no constraint fits the p_const
	const ConstraintEntry* FindFitEntry(  const CMobileElemConstraint& p_const ) const; 

    // accessors for list elements
	const CMobileElemConstraint* GetConstraint(int p_ndx) const
        { return (CMobileElemConstraint *)(getItem(p_ndx)->getConstraint()); }

    // delete all references to the passed index at p_level
	void deletePaxType (int p_level, int p_index);
	void DeleteMobType( const CMobileElemConstraint* _pConstraint );

	void deletePipe ( int _iPipeIdx );
	bool IfUseThisPipe( int _iPipeIdx ,CMobileElemConstraint*& pConstr)const;

    // text based file I/O
	void readDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readDatabase26 (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void writeDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const;
	void writeDatabaseEx (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const;
	virtual int checkLastOfProbs();
	void InitRules();
};

class CNoPaxConstraintCoutDatabase : public CMobileElemConstraintDatabase
{
public:
	CNoPaxConstraintCoutDatabase();
	virtual ~CNoPaxConstraintCoutDatabase();

	// text based file I/O
	void readDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void writeDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const;
	void readOldDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
};

#endif // !defined(AFX_MOBILEELEMCONSTRAINTDATABASE_H__AB994609_6ED7_4232_B31B_35480F05463B__INCLUDED_)
