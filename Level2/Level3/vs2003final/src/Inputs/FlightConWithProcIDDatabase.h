// FlightConWithProcIDDatabase.h: interface for the FlightConWithProcIDDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLIGHTCONWITHPROCIDDATABASE_H__1D9A168C_E92E_4F67_9BDB_E8D55BA67E77__INCLUDED_)
#define AFX_FLIGHTCONWITHPROCIDDATABASE_H__1D9A168C_E92E_4F67_9BDB_E8D55BA67E77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CommonData/procid.h>
#include "..\inputs\FLT_DB.H"
#include "..\inputs\MobileElemConstraintDatabase.h"

class INPUTS_TRANSFER ConstraintWithProcIDEntry : public ConstraintEntry
{
protected:
	ProcessorID procID;
public:
	ConstraintWithProcIDEntry()
	{}
	~ConstraintWithProcIDEntry()
	{ clear(); }

	void initialize (Constraint *p_const, ProbabilityDistribution *p_prob, const ProcessorID& id )
	{
		ConstraintEntry::initialize( p_const, p_prob );
		procID = id;
	}
    void clear (void)
	{
		ConstraintEntry::clear();
		procID.init();
	}
	
	// get & set
	const ProcessorID& getProcID( void ) const
	{ return procID;}
	ProcessorID obtainProcID( void ) const
	{ return procID; }
	void setProcID( const ProcessorID& _id )
	{ procID = _id; }

	// operator
	ConstraintWithProcIDEntry& operator=( const ConstraintWithProcIDEntry& _entry )
	{
		clear();
		ConstraintEntry::operator =( _entry );
		procID = _entry.procID;
		return *this;
	}

	int operator==( const ConstraintWithProcIDEntry& _entry ) const
	{
		return *constraint == *(_entry.constraint)
			 && procID == _entry.procID; 
	}

	int operator<( const ConstraintWithProcIDEntry& _entry ) const;
};


class INPUTS_TRANSFER FlightConWithProcIDDatabase : public FlightConstraintDatabase  
{
public:
	FlightConWithProcIDDatabase();
	virtual ~FlightConWithProcIDDatabase();
	
	//////////////////////////////////////////////////////////////////////////
	// find first entry that matches paxType
    // returns NULL if no constraint fits the paxType
    const ProbabilityDistribution *lookup_with_procid (const FlightConstraint& p_type, const ProcessorID& _id ) const;
	
	//
	void addEntry (ConstraintWithProcIDEntry* anEntry, bool _bDeleteIfExist = false );
	
    // text based file I/O
    void readDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readObsoleteDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
    void writeDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const;

	void resortDatebase();
};


class INPUTS_TRANSFER CMobileConWithProcIDDatabase : public CMobileElemConstraintDatabase
{
public:
	CMobileConWithProcIDDatabase();
	virtual ~CMobileConWithProcIDDatabase();
	
	//////////////////////////////////////////////////////////////////////////
	// find first entry that matches paxType
    // returns NULL if no constraint fits the paxType
    const ProbabilityDistribution *lookup_with_procid (const CMobileElemConstraint& _type, const ProcessorID& _id ) const;
	
    // text based file I/O
    void readDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readObsoleteDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
	void readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm );
    void writeDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const;
	
	void resortDatebase();
};
#endif // !defined(AFX_FLIGHTCONWITHPROCIDDATABASE_H__1D9A168C_E92E_4F67_9BDB_E8D55BA67E77__INCLUDED_)





















