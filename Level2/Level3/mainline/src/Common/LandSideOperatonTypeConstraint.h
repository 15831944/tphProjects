#pragma once
#include "constrnt.h"
class CAirportDatabase;
class CLandSideOperatonTypeConstraint:public Constraint
{
protected:
	//InputTerminal* m_pInTerm;
	CAirportDatabase * m_pAirportDB;
public:
	CLandSideOperatonTypeConstraint(void);
	~CLandSideOperatonTypeConstraint(void);
	void SetAirportDB(CAirportDatabase * pAirportDB);
	virtual void initDefault (void);

	// checks to see if all values are default
	virtual int isDefault (void) const;

	// basic logic that does a full compare of 2 constraints
	virtual int isEqual (const Constraint *p_const) const;

	// returns the number of criteria defining the constraint
	virtual int getCriteriaCount (void) const;

	// returns the combined precedence level of all criteria
	// criteria are ordered with 0 as the most specific, or lowest value
	virtual int getTotalPrecedence (void) const;
	virtual void WriteSyntaxString( char *p_str ) const;
	// constraint string and file I/O
	virtual void setConstraint ( const char *p_str, int _nConstraintVersion ) ;

	virtual void screenPrint (char *p_str, int _nLevel=0, unsigned p_maxChar = 70) const;
	virtual bool screenPrint(CString& p_str, int _nLevel =0 , int p_maxChar=70) const;
	virtual void screenTips( CString& strTips) const;
	void screenFltPrint( CString& _strLabel ) const;
};
