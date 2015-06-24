// MultiMobConstraint.h: interface for the CMultiMobConstraint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIMOBCONSTRAINT_H__C9AB94E2_E474_40BB_87F2_E1354B16425A__INCLUDED_)
#define AFX_MULTIMOBCONSTRAINT_H__C9AB94E2_E474_40BB_87F2_E1354B16425A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MobileelemConstraint.h"

class CMultiMobConstraint  
{
protected:
	//int m_nCount;
	//CMobileElemConstraint* m_pConstraints;
	std::vector<CMobileElemConstraint> m_pConstraints;

public:
	CMultiMobConstraint();
	//CMultiMobConstraint( const CMultiMobConstraint& _const );
	// assignment operator (performs deep copy)
    //const CMultiMobConstraint &operator = (const CMultiMobConstraint &p_const);	

	virtual ~CMultiMobConstraint();


    // creates a list of a single default constraint
    void initDefault (InputTerminal* _pInTerm);

    // creates MultiConst from passed values
    void initConst (const CMobileElemConstraint *constList, int num);

    // allocates pointers for num PassengerConstraints
    void initConstCount (int num);

    // set PassengerConstraint pointer at index to aConst
    void setConstraint ( CMobileElemConstraint aConst, int index) ;

    // reallocates pointer list to include passed constraint
    void addConstraint ( CMobileElemConstraint aConst);

    // deallocates MultiConst
    void freeConstraints (void);;

    // removes 1 constraint from list
    void deleteConst (int p_ndx);

    // returns the PassengerConstraint at index num
    const CMobileElemConstraint *getConstraint (int num) const;
    CMobileElemConstraint *getConstraint (int num);
    int getCount (void) const { return (int)m_pConstraints.size(); };

    // test each PassengerConstraint to see if they apply to paxType
    int isMatched (const CMobileElemConstraint& p_const, bool _bIgnoreStar = false ) const;

    // returns non-zero if the constraint contains p_index at p_level
    int contains (int p_level, int p_index) const;

    
	
	bool operator == ( const CMultiMobConstraint &p_const )const;
    // reads a number of PassengerConstraints from a field of the file
    void readConstraints (ArctermFile& file, InputTerminal* _pInTerm );
    void writeConstraints (ArctermFile& file, int p_display = 0) const;


protected:

	// returns TRUE if MultiConst applies to NO passenger type
	int isNone() const;

};

#endif // !defined(AFX_MULTIMOBCONSTRAINT_H__C9AB94E2_E474_40BB_87F2_E1354B16425A__INCLUDED_)
