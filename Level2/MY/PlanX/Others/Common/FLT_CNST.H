/*****
*
*   Module:     FlightConstraint
*   Author:     Lincoln Fellingham
*   Date:       Jan 93; Nov 96
*   Purpose:    Subclass of Constraint class, FlightConstraint defines
*               a number of criteria that define a set of passengers,
*               based on carrier, flight ID, ac type, ac type category,
*               fromAirport or dest airport, sector of fromAirport or dest airport,
*               arriving/departing mode or ANY COMBINATION THEREOF.
*
*               Order of precedence for FlightConstraint criteria is the
*               same sequence that they are listed above.
*
*				Add new type of flight constraint. airline group.
*				Need to integrated to the flight constraint.
*				How current flight constraint works:
*				1. use '-' as each field of the string.
*				2. should have new file version 2.2
*
*
*****/

#ifndef FLIGHT_CONSTRAINT_H
#define FLIGHT_CONSTRAINT_H

#include "constrnt.h"
#include "../Common/IDGather.h"
#include "../common/term_bin.h"
#include "../common/AIRSIDE_BIN.h"
//class InputTerminal;
class CSubairline;
class CSector;
class CACCategory;
class CAirportDatabase;
class FlightGroup;


enum FLTCNSTR_MODE
{ 
	ENUM_FLTCNSTR_MODE_ALL,
	ENUM_FLTCNSTR_MODE_ARR,
	ENUM_FLTCNSTR_MODE_DEP,
	ENUM_FLTCNSTR_MODE_THU
};

class FlightConstraint : public Constraint
{
protected:
	//InputTerminal* m_pInTerm;
	CAirportDatabase * m_pAirportDB;

	FLTCNSTR_MODE m_enumFltCnstrMode;

	CAirlineID	airline;
	CFlightID flightID;
	int		m_nDay;
	int m_nFlightIndex;

	// airport
	CAirportCode m_airport;
	CAirportCode m_stopoverAirport;


	CAircraftType acType;
	CSubairline* m_pAirlineGroup;
	FlightGroup* m_pFlightGroup;

	// sector
	CSector* m_pSector;
	CSector* m_pStopoverSector;

	CACCategory* m_pCategory;

	// check if sector include the _otherFlightContraint
	// return true / false.
	bool IsSectorInclude( const FlightConstraint& _otherFlightContraint, bool _bStopover ) const;
	
	const static  TCHAR  ActypeReplaceChar ; // the char will replace the '-' in actype .
public:
    FlightConstraint(CAirportDatabase  * pAirportDB = NULL) 
	{ 
		//m_pInTerm = NULL;
		m_pAirportDB = pAirportDB;
		initDefault(); 
	}
	FlightConstraint(int version):Constraint(version){}
	FlightConstraint(const FlightConstraint& _otherConst)
		: Constraint(_otherConst)
	{
		//m_pInTerm=_otherConst.m_pInTerm;
		m_pAirportDB = _otherConst.m_pAirportDB;

		airline = _otherConst.airline;
		flightID = _otherConst.flightID;
		m_nDay = _otherConst.m_nDay;
		m_enumFltCnstrMode = _otherConst.m_enumFltCnstrMode;
		m_airport = _otherConst.m_airport;
		m_stopoverAirport = _otherConst.m_stopoverAirport;
		acType = _otherConst.acType;
		m_pAirlineGroup = _otherConst.m_pAirlineGroup;
		m_pSector = _otherConst.m_pSector;
		m_pStopoverSector = _otherConst.m_pStopoverSector;
		m_pCategory = _otherConst.m_pCategory;
		m_pFlightGroup = _otherConst.m_pFlightGroup;
		m_nFlightIndex = _otherConst.m_nFlightIndex;
		nConstraintVersion = _otherConst.nConstraintVersion;
	}
	
    virtual ~FlightConstraint () {};

	//void SetInputTerminal(InputTerminal* _pInTerm)
	//{
	//	m_pInTerm = _pInTerm;
	//}

	//InputTerminal* GetInputTerminal() const	{ return m_pInTerm;	}
	void SetAirportDB(CAirportDatabase * pAirportDB){
		m_pAirportDB = pAirportDB;
	}
	CAirportDatabase * GetAirportDB()const{ return m_pAirportDB; }



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
    int fits(const FlightConstraint& p_type, bool _bIgnoreStar = false) const;

	// check if the flightDesc fit this flightContrain.
	int fits(const FlightDescStruct& _fltDesc) const;

	// check if the flightDesc fit this flightContrain.
	int fits(const AirsideFlightDescStruct& _fltDesc) const;

	//fit with mode
	int fits(const AirsideFlightDescStruct& _fltDesc, char mode)const;
	//poly two constraint to a constraint
	void polyConstraint( const FlightConstraint* p_type);
	
    const FlightConstraint& operator= (const FlightConstraint &p_const);

    // basic modifiers
	void setFlightIndex(int nFlightIndex){ m_nFlightIndex = nFlightIndex;}
    void setAirline (const char *p_str) { airline = p_str; /*strcpy (airline, p_str);*/ }
	void setFlightID(const char *p_str) { flightID = p_str;}
	void setFlightID(const CFlightID& id){flightID = id;}
	void setDay(int nDay){m_nDay = nDay;}
    void SetFltConstraintMode (FLTCNSTR_MODE _enumFltCnstrMode) { m_enumFltCnstrMode = _enumFltCnstrMode; }
    void setFullID (const char *p_str);
	void setAirport (const char *p_str) { m_airport = p_str;/*strcpy (airport, p_str);*/ }
	void setStopoverAirport (const char *p_str) { m_stopoverAirport = p_str;/*strcpy (airport, p_str);*/ }
    void setACType (const char *p_str) { acType = p_str; /*strcpy (acType, p_str);*/ }
	void setAirlineGroup (const char *p_str);
	void setFlightGroup(const char* p_str);
	void setSector (const char *p_str );
	void setStopoverSector (const char *p_str );
    void setCategory (const char *p_str);
    void initFlightType (const FlightDescStruct &p_struct, char p_mode);
	void initFlightType (const AirsideFlightDescStruct& p_struct, char p_mode);
	void getFltConModeString(CString& strFltMode) const;
    // basic accessors
    void getAirline (char *p_str) const;
	const CAirlineID& getAirline() const { return airline; }
    //int getFlightID (void) const { return flightID; }
	const CFlightID& getFlightID (void) const { return flightID; }
	int GetDay(void)const {return m_nDay;}
    FLTCNSTR_MODE GetFltConstraintMode (void) const { return m_enumFltCnstrMode; }
    void getFullID (char *p_str) const;
	void getAirport (char *p_str) const;
	const CAirportCode& getAirport() const {return m_airport;};
	void getStopoverAirport (char *p_str) const;
	const CAirportCode& getStopoverAirport()const {return m_stopoverAirport;};
    void getACType (char *p_str) const;
	const CAircraftType getACType() const {return acType;}
	int getCurrentVersion(){return nConstraintVersion ;}
	void getAirlineGroup( char* p_str) const;
	void getFlightGroup(char* p_str) const;
	void getSector (char *p_str) const;
	void getStopoverSector (char *p_str) const;
    void getCategory (char *p_str) const;
	int fitsAirline( const char* p_airline ) const;
	BOOL fitsACType(const CAircraftType& _AircraftType)const;
	int fitsAirport (const char *p_airport) const;
	int fitsStopoverAirport (const char *p_stopoverAirport) const;

    // constraint I/O (file or string)
public:
	//just remain for old code , not use any more 
	//////////////////////////////////////////////////////////////////////////
    void setConstraint_NotUse( const char *p_str, int _nConstraintVersion= VERSION_CONSTRAINT_CURRENT );
	void WriteSyntaxString_NotUse(char *p_str) const;
	//////////////////////////////////////////////////////////////////////////
protected:
	virtual void setConstraint ( const char *p_str, int _nConstraintVersion= VERSION_CONSTRAINT_CURRENT );
    virtual void WriteSyntaxString (char *p_str) const;
public:
	//this function not use any more
	//void printConstraintStringToDB(char* p_str,int _ver = VERSION_CONSTRAINT_CURRENT_103)  const;
    virtual void screenPrint (char *p_str, int _nLevel=0, unsigned p_maxChar = 70) const;
	virtual bool screenPrint(CString& p_str, int _nLevel =0 , int p_maxChar=70) const;
	virtual void screenTips( CString& strTips) const;
	void screenFltPrint( CString& _strLabel ) const;
friend class CFlightConstraintReadHandleByVersion ;
};
///////////////////////////////////////////////////////////////////////////
//this class is used for read different version of constraint 
//
//////////////////////////////////////////////////////////////////////////
class CConstraintReadHandle
{
public:
	//read constraint 
	virtual void setConstraint ( const char *p_str, int _nConstraintVersion )=0;

};
class CFlightConstraintReadHandleByVersion : public CConstraintReadHandle
{
protected:
	 FlightConstraint* flightconstraint ;
public:
	CFlightConstraintReadHandleByVersion( FlightConstraint* fli_constraint);
	void  setFlightConstraint(FlightConstraint* fli_constraint) ;
	virtual void setConstraint (const  char *p_str, int _nConstraintVersion );

protected:
	//format airline and flight id string 
	virtual void setAirlineAndFlightID(const char* p_char,int& ndx) ;
	
};
class CFlightConstraintReadHandleByVersion103 : public CFlightConstraintReadHandleByVersion
{
public :
	CFlightConstraintReadHandleByVersion103( FlightConstraint* fli_constraint);
protected:
	void setAirlineAndFlightID(const char* p_char,int& ndx) ;
};
#endif
