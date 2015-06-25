#include "StdAfx.h"
#include ".\landsideoperatontypeconstraint.h"

CLandSideOperatonTypeConstraint::CLandSideOperatonTypeConstraint(void)
{
}

CLandSideOperatonTypeConstraint::~CLandSideOperatonTypeConstraint(void)
{
}
void CLandSideOperatonTypeConstraint:: SetAirportDB(CAirportDatabase * pAirportDB)
{
	m_pAirportDB = pAirportDB;
}
void CLandSideOperatonTypeConstraint::initDefault()
{

}
int  CLandSideOperatonTypeConstraint::isDefault()const 
{
 return 0;
}
int  CLandSideOperatonTypeConstraint::isEqual(const Constraint *p_const) const
{
	return 0;
}
int  CLandSideOperatonTypeConstraint::getCriteriaCount() const
{
	return 0;

}
int  CLandSideOperatonTypeConstraint::getTotalPrecedence() const
{
	return 0;
}
void CLandSideOperatonTypeConstraint::screenFltPrint( CString& _strLabel )const
{

}
void CLandSideOperatonTypeConstraint::screenPrint(char *p_str, int _nLevel/* =0 */, unsigned p_maxChar /* = 70 */)const
{

}

bool  CLandSideOperatonTypeConstraint::screenPrint(CString& p_str, int _nLevel /* =0  */, int p_maxChar/* =70 */)const
{
 return true;
}



void CLandSideOperatonTypeConstraint::screenTips(CString& strTips)const
{


}
void CLandSideOperatonTypeConstraint::setConstraint( const char *p_str, int _nConstraintVersion )
{

}
void CLandSideOperatonTypeConstraint::WriteSyntaxString( char *p_str ) const
{

}