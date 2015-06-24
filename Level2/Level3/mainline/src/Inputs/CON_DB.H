/*****
*
*   Module:     ConstraintDatabase, ConstraintEntry
*   Author:     Lincoln Fellingham
*	Date:		Summer 94
*	Purpose:	ConstraintEntry defines an association between a
*               Constraint of any kind and a probability distribution.
*
*               A ConstraintDatabase is an ordered list of
*               ConstraintEntries.
*
*               Both constraint and value are assumed to be allocated
*               specifically for this object, and will be deallocated
*               when detsroyed. This is mandatory as both classes are
*               abstract.
*
*   Revisions:  Jan 96 - LF - removed methods specific to passenger type
*               constraints to allow use for any Constraint based class.
*               Also corrected allocation scheme to prevent _possible_
*               memory leaks.
*
*****/

#ifndef CONSTRAINT_DATABASE_H
#define CONSTRAINT_DATABASE_H

#include "common\containr.h"
#include "common\template.h"
#include "common\elaptime.h"
#include "common\constrnt.h"
#include "in_term.h"

class INPUTS_TRANSFER ConstraintEntry
{
protected:

	// key component, defines types that the distribution apply to
    Constraint *constraint;

    // ProbabilityDistribution of service times
    // or one row in the transition table
    ProbabilityDistribution *value;
public:
    ConstraintEntry() { constraint = NULL; value = NULL;};
    ConstraintEntry (const ConstraintEntry& p_entry) { *this = p_entry; }
	virtual ~ConstraintEntry() { clear(); };

    void initialize (Constraint *p_const, ProbabilityDistribution *p_prob)
        { clear(); constraint = p_const; value = p_prob; };

	// deallocates existing members
    virtual void clear (void);

	// basic accessors
    Constraint *getConstraint (void) const { return constraint; }
	const ProbabilityDistribution *getValue (void) const { return value; }
	ProbabilityDistribution* getValue (void) { return value; }
    void setValue (ProbabilityDistribution *p_prob);

    ConstraintEntry& operator = (const ConstraintEntry& p_entry);
    int operator == (const ConstraintEntry& p_entry) const
        { return *constraint == *(p_entry.constraint); }
    int operator < (const ConstraintEntry& p_entry) const
        { return *constraint < *(p_entry.constraint); }
	void DeleteValue();
};


class INPUTS_TRANSFER ConstraintEntryEx : public ConstraintEntry
{
protected:
	bool flag;
	ElapsedTime beginTime;
	ElapsedTime endTime;
	
public:
	ConstraintEntryEx():flag(false), beginTime(0l), endTime( 24*60*60l-1l){}
	ConstraintEntryEx( const ConstraintEntryEx& _entry ) : ConstraintEntry( _entry )
	{
		flag		= _entry.flag;
		beginTime	= _entry.beginTime;
		endTime		= _entry.endTime;
	}
	~ConstraintEntryEx(){}
	void initialize( Constraint *p_const, ProbabilityDistribution *p_prob, bool _flag = false, ElapsedTime _begin = 0l, ElapsedTime _end = 24*60*60l-1l )
	{
		ConstraintEntry::initialize( p_const, p_prob );
		flag		= _flag;
		beginTime	= _begin;
		endTime		= _end;
	}
	
	// get & set
	bool getFlag( void ) const { return flag; }
	ElapsedTime getBeginTime( void ) const { return beginTime;	}
	ElapsedTime getEndTime( void ) const { return endTime;	}
	
	void setFlag( bool _flag ) { flag = _flag;	}
	void setBeginTime( const ElapsedTime& _begin ) { beginTime = _begin; }
	void setEndTime( const ElapsedTime& _end ) { endTime = _end; }
	
	// operator
	ConstraintEntryEx& operator = ( const ConstraintEntryEx& _entry )
	{
		if( this != &_entry )
		{
			ConstraintEntry::operator =( _entry );
			flag		= _entry.flag;
			beginTime	= _entry.beginTime;
			endTime		= _entry.endTime;
		}
		return *this;
	}
	
	int operator== (const ConstraintEntryEx& _entry) const
	{
		return ConstraintEntry::operator ==( _entry )
			&& flag == _entry.flag 
			&& beginTime == _entry.beginTime
			&& endTime == _entry.endTime;
	}
	
	// it is important
	int operator< (const ConstraintEntryEx& _entry) const
	{
		if( *constraint == *( _entry.constraint ) )
		{
			// only sort by begin time
			return !(beginTime < _entry.beginTime);
		}
		else
		{
			return *constraint < *(_entry.constraint );
		}
		
	}
	
};


class INPUTS_TRANSFER ConstraintDatabase : public SortedContainer<ConstraintEntry>
{
protected:
	 // defines the units of the associated distributions
	 const char *units;

public:
ConstraintDatabase ();
/* Jan98 : SortedContainer<ConstraintEntry> (8, 0, 8);
{
	ownsElements(1);
	units = NULL;
}; */

	 virtual ~ConstraintDatabase () {};

	 // adds p_entry to the entryList
	 // replaces entry if already in database, deletes old one
	void addEntry (ConstraintEntry *anEntry , bool _bDeleteIfExist = false );
	void clear( void );
	const ProbabilityDistribution *getDist (int p_ndx) const
		{ return getItem(p_ndx)->getValue(); }

	 const char *getUnits (void) const { return units; };
	 void setUnits (const char *p_str) { units = p_str; };

	 // text based file I/O
	 virtual void readDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm ) = 0;
	 virtual void readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm ) = 0;
	 virtual void writeDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const = 0;
	 virtual int checkLastOfProbs() { return 1; };
};

#endif





















