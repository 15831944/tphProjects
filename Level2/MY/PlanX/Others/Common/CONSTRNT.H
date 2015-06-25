/*****
*
*   Module:     Constraint
*   Author:     Lincoln Fellingham
*   Date:       Jan 96
*   Purpose:    Creates a standardized method for defining Constraint
*               classes, or a way classifying objects based on varying
*               criteria.
*
*               Constraints are sortable in decreasing order. The most
*               specific, or restrictive constraint would be the first
*               to appear in any list. Where two constraints are equally
*               specific (both have two criteria to meet to define a
*               "match" or "fit"), the various criteria are ordered
*               according to a precedence that defines which of the
*               criteria is the most specific.
*
*               Any string based criteria that is "flexible", or will
*               match any possible data should be a NULL string: "".
*               Any numeric criteria should have a specific constant
*               defined that will be interpreted as flex, typically 0 or
*               -1.
*
*****/

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

class ArctermFile;
#define VERSION_CONSTRAINT_CURRENT101  101
#define VERSION_CONSTRAINT_CURRENT_102 102
#define VERSION_CONSTRAINT_CURRENT  103
#define VERSION_CONSTRAINT_CURRENT_103  103
enum SAVE_TYPE{FILE_TYPE,DATABASE_TYPE} ;
class Constraint
{
public:
	Constraint () {nConstraintVersion = VERSION_CONSTRAINT_CURRENT;};
	Constraint(int version):nConstraintVersion(version){};
	Constraint (const Constraint& _otherConst) { nConstraintVersion = VERSION_CONSTRAINT_CURRENT ;};
    virtual ~Constraint () {};

	const Constraint& operator=(const Constraint& ){ return *this;};
    // determinants of a constraint that fits all possible objects
    virtual void initDefault (void) = 0;
    virtual int isDefault (void) const = 0;

    // returns the number of criteria defining the constraint
    virtual int getCriteriaCount (void) const = 0;

    // returns the combined precedence level of all criteria
    // criteria are ordered with 0 as the most specific, or lowest value
    virtual int getTotalPrecedence (void) const = 0;

	// basic logic that does a full compare of 2 constraints
    virtual int isEqual (const Constraint *p_const) const = 0;

	// basic logic that does a full compare of 2 constraints
    bool operator == (const Constraint& p_const) const
	{ return isEqual (&p_const) ? true : false; }

    // complex logic to force to correct sorted order in CONSTRDB
    // the intent of the test is to return the tighter constraint, based
    // first on the number of criteria, and second on the precedence of
    // the criteria
    virtual bool operator < (const Constraint& p_const) const;

   

	// need to get version info first and then call set
	void setConstraintWithVersion( char *p_str );
	void setConstraintWithVersion(const CString& p_str) ;
protected:
    virtual void WriteSyntaxString( char *p_str ) const = 0;
	// constraint string and file I/O
	virtual void setConstraint ( const char *p_str, int _nConstraintVersion ) = 0;
public:
	// will add VERSION_CONSTRAINT_CURRENT
    void WriteSyntaxStringWithVersion (char *p_str) const;

    virtual void screenPrint (char *p_str, int _nLevel, unsigned p_maxChar) const = 0;
	virtual bool screenPrint(CString& p_str, int _nLevel , int p_maxChar) const =0;
	virtual void screenTips( CString& strTips) const = 0;
    void readConstraint (ArctermFile& p_file,SAVE_TYPE _type = FILE_TYPE);
    void writeConstraint (ArctermFile& p_file) const;
	void readConstraintWithVersion(const char* _csStr,SAVE_TYPE _type = FILE_TYPE);

protected:
	// assume first 3 chars in the string are the version info, if version >= 100
	// get the version and remove the version info
	int GetAndRemoveConstraintVersion( char* _csStr,SAVE_TYPE _type = DATABASE_TYPE );
protected:
	mutable int nConstraintVersion ;
};

#endif
