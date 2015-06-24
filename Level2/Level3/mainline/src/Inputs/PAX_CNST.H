/*****
*
*   Module:     CPassengerConstraint
*   Author:     Lincoln Fellingham
*   Date:       Jan 93 - present
*   Purpose:    Subclass of FlightConstraint class, CPassengerConstraint
*               defines a number of criteria that define a set of
*               passengers, based on the type of flight the passenger is
*               travelling on, intrinsic passenger type, user-defined
*               passenger types, or ANY COMBINATION THEREOF.
*
*               Order of precedence for CPassengerConstraint criteria is
*               FlightType, intrinsic type, and then passenger type.
*
*               PassengerConstraints also have define the status NONE,
*               which is a constraint that will not fit any passengers.
*               This is implemented by setting the intrinsic type to -1.
*
*****/

#ifndef PASSENGER_CONSTRAINT_H
#define PASSENGER_CONSTRAINT_H

#include <string.h>
#include <iostream>
#include "assert.h"
#include "../common/template.h"
#include "../common/term_bin.h"
#include "../Common/FLT_CNST.H"
#include "../common/replace.h"
#include "../common/NewPassengerType.h"

class InputTerminal;
//////////////////////////////////////////////////////////////////////////
enum LogicOperator    // <, <=, ==, >=, > , != 
{
	less = 0,
	less_equal,
	Equal,
	more_equal,
	more,
	not_equal,
	other_operator
};

class NONPAXCOUNTITEM
{
public:
	NONPAXCOUNTITEM()
	{
		iNonPaxType = -1;
		iOperator = more_equal;
		iValue = 0;
	}
	~NONPAXCOUNTITEM()
	{

	}
	int iNonPaxType;
	LogicOperator iOperator;
	int iValue;
	// function
	bool operator==( const NONPAXCOUNTITEM& _item ) const
	{
		return iNonPaxType == _item.iNonPaxType
			&&   iOperator == _item.iOperator
			&&      iValue == _item.iValue;
	}
};

// class CNonPaxCount
class CNonPaxCount
{
private:
	std::vector<NONPAXCOUNTITEM> m_vCountItem;
	
public:
	CNonPaxCount();
	~CNonPaxCount();
	//////////////////////////////////////////////////////////////////////////
	int getCount( void ) const { return (int)m_vCountItem.size();	}
	NONPAXCOUNTITEM getItem( int _indx ) const
	{
		assert( _indx>=0 && (unsigned)_indx< m_vCountItem.size() );
		return m_vCountItem[_indx];
	}

	int addItem( const NONPAXCOUNTITEM& _item )
	{
		m_vCountItem.push_back( _item );
		return (int)m_vCountItem.size();
	}
	void modifyItem( int _indx, const NONPAXCOUNTITEM& _item )
	{
		assert( _indx>=0 && (unsigned)_indx< m_vCountItem.size() );
		m_vCountItem[_indx] = _item;
	}

	void removeItem( int _indx )
	{
		assert( _indx>=0 && (unsigned)_indx< m_vCountItem.size() );
		m_vCountItem.erase( m_vCountItem.begin() + _indx );
	}

	void increaseNonpaxCount( int _nonpaxtype, int _num )
	{
		for( unsigned i=0; i<m_vCountItem.size(); i++ )
		{
			if( m_vCountItem[i].iNonPaxType == _nonpaxtype && 
				m_vCountItem[i].iOperator == Equal )
			{
				m_vCountItem[i].iValue += _num;
				return;
			}
		}

		NONPAXCOUNTITEM item;
		item.iNonPaxType	= _nonpaxtype;
		item.iOperator		= Equal;
		item.iValue			= _num;

		m_vCountItem.push_back( item );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	void init() { m_vCountItem.clear(); }
	bool isDefault() const ;
	bool isEqual( const CNonPaxCount& _other ) const ;								
	void setValueByString( const CString& _str);		// read data from string
	void getValueString( CString& _str ) const;			// write data to string
	// is so important
	bool fits( const CNonPaxCount& _other) const ;

	CString printScreen( InputTerminal* _pInTerm ) const;
	static LogicOperator getOperatorByString( const CString& _strOperator);
	static CString getStringByOperator( LogicOperator _op );

	//if the item does not exist, it will create a new one
	//non pax type == 0
	//and return
	NONPAXCOUNTITEM getItemByType(int nonPaxType) const;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class CPassengerConstraint : public FlightConstraint
{
protected:

    int intrinsicType;
    //int userTypes[MAX_PAX_TYPES];
	CPassengerType m_paxType;

	FlightConstraint m_otherFlightConstraint;
	CNonPaxCount m_NonPaxCount;	// only when paxtype is Passenger, the NonPaxCount is validate
public:
    explicit CPassengerConstraint (InputTerminal * pInterm = NULL);// { initDefault(); m_pInTerm = pInterm; }
    CPassengerConstraint (const CPassengerConstraint& _otherConst);

    virtual ~CPassengerConstraint () {};

	CPassengerConstraint& operator=(const CPassengerConstraint& _otherConst);
//&    static void initTypeList (PassengerTypeList *p_list) { typeList = p_list; }
//&    static void initStrDict (StringDictionary *p_dict) { strDict = p_dict; }
	const CPassengerType* GetPaxType() const;
    // initializes a constraint that fits all possible objects
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

    // returns TRUE if paxType fits within this constraint
    // comparison is done by constructing a string from the paxType that
    // will be completely equal to the constraint, byte by byte
    int fits (const CPassengerConstraint& p_const, bool _bIgnoreStar = false ) const;
	int fitex (const CPassengerConstraint& p_const, bool _bIgnoreStar = false ) const;
	virtual void deletePipe( int _iPipeIdx ){};

	//poly two constraint to a constraint
	void polyConstraint( const CPassengerConstraint* p_type);
    // basic type initialization methods
    void makeSender (void);
    void makeGreeter (void);
    void makeTransiting (void);
    void makeTransferring (void);
    void makeArrival (void);
    void makeDeparture (void);
    void initPaxType (const MobDescStruct &p_struct);

    // general tests for various basic types
    int isArrival (void) const;
    int isDeparture (void) const;
    int isVisitor (void) const { return isSender() || isGreeter(); }
    int isSender (void) const;
	int isGreeter (void) const;
	int isTurround (void) const;

    // basic modifiers
    void setIntrinsicType (int p_type) { intrinsicType = p_type; }
    void setUserType (int p_level, int p_type);;
    void setUserType (const int *p_type);
    int getUserType (int p_level) const;
    void getUserType (int *p_type) const;
    int getIntrinsic (void) const { return intrinsicType; }

    // returns non-zero if the constraint contains p_index at p_level
    int contains (int p_level, int p_index) const;

    // passenger constraint I/O (file or string)
protected:
	virtual void setConstraint ( const char *p_str, int _nConstraintVersion );
	virtual void WriteSyntaxString (char *p_str) const;
public:
	virtual bool screenPrint(CString& p_str, int _nLevel = 0 , int p_maxChar=70) const;
    virtual void screenPrint (char *p_str, int _nLevel = 0, unsigned p_maxChar = 70) const;
	virtual void screenTips( CString& strTips) const;
	void getPassengerType( char *p_str );
	void screenPaxPrint( CString& _strLabel ) const;
//    friend stdostream& operator << (stdostream &s, const CPassengerConstraint &p_const);
	void setOtherFlightConstraint( const FlightConstraint& _otherFilght) { m_otherFlightConstraint = _otherFilght; }
	const FlightConstraint& getOtherFlightConstraint( void ) const { return m_otherFlightConstraint; }
	void setNonpaxCount( const CNonPaxCount& _nonpaxCount ) { m_NonPaxCount = _nonpaxCount; }
	CNonPaxCount& getNonpaxCount( void ) { return m_NonPaxCount; 	}
	

	// init arr flight type for transfer.
	void initOtherFlightType(const FlightDescStruct &p_struct);

public:
	void SetInputTerminal(InputTerminal * pInterm);
	InputTerminal * GetInputTerminal()const{ return m_pInTerm; }
protected:
	InputTerminal * m_pInTerm;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif
