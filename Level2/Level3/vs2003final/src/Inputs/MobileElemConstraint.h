// MobileElemConstraint.h: interface for the CMobileElemConstraint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOBILEELEMCONSTRAINT_H__759EF03B_DBD7_4B22_B97A_F9E890EA5D01__INCLUDED_)
#define AFX_MOBILEELEMCONSTRAINT_H__759EF03B_DBD7_4B22_B97A_F9E890EA5D01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "PAX_CNST.H"	

#define CS_PASSENGER "PASSENGER"

enum enum_MobileElementType
{
	enum_MobileElementType_ALL = 0,
	enum_MobileElementType_PAX,
	enum_MobileElementType_NONPAX
};



class CMobileElemConstraint : public CPassengerConstraint  
{
protected:
	static std::vector<CString> m_strList;	// keep all the string
	int m_nTypeIndex;						// index to the string list. 0 as default, -1 as none

	enum_MobileElementType m_enumElementType;
	// get the index of the string, if new add into it and return index.
	static int FindOrAddString( CString _csStr );
public:

	void SetMobileElementType(enum_MobileElementType enumType);
	enum_MobileElementType GetMobileElementType();


	//std::vector<TypeString>  m_vector;
	
	explicit CMobileElemConstraint(InputTerminal * pInterm = NULL);
	CMobileElemConstraint(const CMobileElemConstraint& _otherConst);

	virtual ~CMobileElemConstraint();

	const CMobileElemConstraint& operator=(const CMobileElemConstraint& _otherConst);
	int GetTypeIndex()const { return m_nTypeIndex;};
	void SetTypeIndex( const int _nTypeIndex )  
	{ 
		m_nTypeIndex = _nTypeIndex; 
		//only when pax_type is Passenger, the NonPaxCount is validate
		if( m_nTypeIndex != 0 )
			m_NonPaxCount.init();
	}
    // initializes a constraint that fits all possible objects
    virtual void initDefault();

    // checks to see if all values are default
    virtual int isDefault() const;

    // basic logic that does a full compare of 2 constraints
    virtual int isEqual( const Constraint* _pConstr ) const;

    // returns the number of criteria defining the constraint
    virtual int getCriteriaCount() const;

    // returns the combined precedence level of all criteria
    // criteria are ordered with 0 as the most specific, or lowest value
    virtual int getTotalPrecedence() const;

    // returns TRUE if paxType fits within this constraint
    // comparison is done by constructing a string from the paxType that
    // will be completely equal to the constraint, byte by byte
    int fits( const CMobileElemConstraint& _constr, bool _bIgnoreStar = false ) const;
    int fitsparent( const CMobileElemConstraint& _constr, bool _bIgnoreStar = false ) const;
	int fitex( const CMobileElemConstraint& _constr, bool _bIgnoreStar = false ) const;
	//poly two constraint to a constraint
	void polyConstraint(const CMobileElemConstraint* p_type);
	void MergeFlightConstraint(const FlightConstraint* p_type);
	// used by processor roster.
    void makeNone();

	

    int isNone() const;

	void initMobType( const MobDescStruct& _struct );
	
    // passenger constraint I/O (file or string)
protected:
	virtual void setConstraint ( const char *p_str, int _nConstraintVersion );
	virtual void WriteSyntaxString (char *p_str) const;
public:
    virtual void screenPrint (char *p_str, int _nLevel = 0, unsigned p_maxChar = 70) const;
	virtual bool screenPrint(CString& p_str, int _nLevel = 0 , int p_maxChar=70) const;
	virtual void screenTips( CString& strTips) const;
	void getPaxTypeSynaxString( CString& p_str );
	void getFltTypeSynaxString( char *p_str );
    friend stdostream& operator << (stdostream &s, const PassengerConstraint &p_const);
	void screenMobPrint(CString& _strLabel) const;	
private:
	bool isNumericString( const CString& _str ) const;
};

#endif // !defined(AFX_MOBILEELEMCONSTRAINT_H__759EF03B_DBD7_4B22_B97A_F9E890EA5D01__INCLUDED_)
