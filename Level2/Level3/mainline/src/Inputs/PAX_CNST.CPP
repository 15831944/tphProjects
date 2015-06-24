#include "stdafx.h"
#include "../inputs/pax_cnst.h"
#include "../inputs/typelist.h"
#include "../inputs/MobileElemTypeStrDB.h"
#include "../common/termfile.h"
#include "../common/exeption.h"
#include "..\inputs\in_term.h"
extern int gLevels;
//& PassengerTypeList *CPassengerConstraint::typeList = NULL;
//& StringDictionary *CPassengerConstraint::strDict = NULL;
CPassengerConstraint::CPassengerConstraint (InputTerminal * pInterm)
:m_paxType(NULL)
{
	m_pInTerm = pInterm;	
	if(m_pInTerm) 
	{
		FlightConstraint::SetAirportDB(m_pInTerm->m_pAirportDB);
		m_paxType.SetStringDictionary(m_pInTerm->inStrDict);
	}

	initDefault();

}
CPassengerConstraint::CPassengerConstraint (const CPassengerConstraint& _otherConst)
:FlightConstraint(_otherConst)
,m_paxType(NULL)
{
	m_paxType = _otherConst.m_paxType;

	intrinsicType=_otherConst.intrinsicType;
	//memcpy((void*)userTypes,(const void*)_otherConst.userTypes,sizeof(int)*MAX_PAX_TYPES);
	m_paxType.copyData(_otherConst.m_paxType);
	if(_otherConst.m_pInTerm)
		m_paxType.SetStringDictionary(_otherConst.m_pInTerm->inStrDict);

	m_otherFlightConstraint = _otherConst.getOtherFlightConstraint();
	m_NonPaxCount = _otherConst.m_NonPaxCount;
	m_pInTerm = _otherConst.GetInputTerminal();
}
// creates default constraint (fits all)
void CPassengerConstraint::initDefault (void)
{
	m_otherFlightConstraint.SetAirportDB( m_pAirportDB );
	m_otherFlightConstraint.initDefault();
    FlightConstraint::initDefault();
    intrinsicType = 0;
    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //    userTypes[i] = 0;
	m_paxType.initDefault();

	m_NonPaxCount.init();
}

// checks to see if all values are default
int CPassengerConstraint::isDefault (void) const
{
    if (!FlightConstraint::isDefault())
        return 0;

    if (intrinsicType)
        return 0;

    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //    if (userTypes[i])
    //        return 0;
	if (!m_paxType.isDefault())
		return 0; 

	if( !m_otherFlightConstraint.isDefault())
		return 0;

	if( !m_NonPaxCount.isDefault() )
		return 0;

    return 1;
}

// basic logic that does a full compare of 2 constraints
int CPassengerConstraint::isEqual (const Constraint *p_const) const
{
    if (!FlightConstraint::isEqual (p_const))
        return 0;

	CPassengerConstraint *aConst = (CPassengerConstraint *)p_const;
    if (intrinsicType != aConst->intrinsicType)
        return 0;

/*
	for (int i = 0; i < MAX_PAX_TYPES; i++)
		if (userTypes[i] != aConst->userTypes[i])
			return 0;*/

 //   if(gLevels == MAX_PAX_TYPES)
	//{
 //       for (int i = 0; i < MAX_PAX_TYPES; i++)
 //          if (userTypes[i] != aConst->userTypes[i])
	//	    	return 0;
	//}
	//else
	//{
	//	for (int i = 0; i < gLevels; i++)
	//	  if (userTypes[i] != aConst->userTypes[i])
	//			return 0;
	//}

	if (!m_paxType.IsEqual(((CPassengerConstraint *)p_const)->m_paxType))
		return 0;

	if( !(m_otherFlightConstraint.isEqual(&((CPassengerConstraint*)p_const)->m_otherFlightConstraint )) ) 
		return 0;
	
	if( !m_NonPaxCount.isEqual( ((CPassengerConstraint *)p_const)->m_NonPaxCount ) )
		return 0;

	return 1;
}	

// returns the number of criteria defining the constraint
int CPassengerConstraint::getCriteriaCount (void) const
{
    int count = FlightConstraint::getCriteriaCount();

    if (intrinsicType)
        count++;

   // for (int i = 0; i < MAX_PAX_TYPES; i++)
   //     if (userTypes[i])
			//count++;
	for (int i = 0; i < MAX_PAX_TYPES; i++)
	{
		if (m_paxType.getUserType(i) > 0)
			count++;
	}

    return count + m_otherFlightConstraint.getCriteriaCount();
}

// returns the combined precedence level of all criteria
// criteria are ordered with 0 as the most specific, or lowest value
// The FlightConstraint component defines precedence up to 7
int CPassengerConstraint::getTotalPrecedence (void) const
{
    int precedence = FlightConstraint::getTotalPrecedence();

    if (intrinsicType)
        precedence += 8;

    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //    if (userTypes[i])
    //        precedence += 9+i;
	for (int i = 0; i < MAX_PAX_TYPES; i++)
	{
		if (m_paxType.getUserType(i) > 0)
			precedence += 9+i;
	}

    return precedence + m_otherFlightConstraint.getTotalPrecedence();
}

// returns TRUE if pt fits within this constraint
// if constraint is default, always returns true
// if constraint is closed, always returns false
// constraint is then tested field by field to try to find conflict
int CPassengerConstraint::fits (const CPassengerConstraint& p_const, bool _bIgnoreStar /*= false*/) const
{
    if (isDefault())
        return 1;

    if (!FlightConstraint::fits (p_const, _bIgnoreStar) )
        return 0;

    if (intrinsicType && intrinsicType != p_const.intrinsicType)
        return 0;

	int i=0;
	
//	const int aryDefault[MAX_PAX_TYPES]={0,0,0,0};   //changed matt
	int aryDefault[MAX_PAX_TYPES];
	for(int nCount=0;nCount < MAX_PAX_TYPES; nCount++) 
		aryDefault[nCount]=0;

	//if(memcmp((const void*)userTypes,(const void*)aryDefault,sizeof(int)*MAX_PAX_TYPES)!=0)
	//{
	//	for(i=0; userTypes[i]==0 && i<MAX_PAX_TYPES ; i++);
	//}

 //   for (/*int i = 0*/; i < MAX_PAX_TYPES; i++)
 //       if (userTypes[i] && userTypes[i] != p_const.userTypes[i])
 //           return 0;
	if (!m_paxType.Fits(p_const.m_paxType))
		return 0;

    return m_otherFlightConstraint.fits( p_const.getOtherFlightConstraint() );
}

int CPassengerConstraint::fitex (const CPassengerConstraint& p_const, bool _bIgnoreStar /*= false*/) const
{
	return fits( p_const, _bIgnoreStar) && m_NonPaxCount.fits( p_const.m_NonPaxCount );
}

//poly two constraint to a constraint
void CPassengerConstraint::polyConstraint( const CPassengerConstraint* p_type)
{
	if( isDefault() && p_type->isDefault() )
		return;

	FlightConstraint::polyConstraint( p_type );

	if( !intrinsicType )
		intrinsicType = p_type->intrinsicType;

	//if( !userTypes[0] && !intrinsicType )
	//{
	//	for( int i=0; i<MAX_PAX_TYPES; i++)
	//		if( !userTypes[i] )
	//			userTypes[i] = p_type->userTypes [i];
	//}

	if( !m_paxType.getUserType(0) && !intrinsicType )
	{
		for( int i=0; i<MAX_PAX_TYPES; i++)
			if( !m_paxType.getUserType(i) )
				m_paxType.setUserType(i,p_type->getUserType(i));	
	}


	//m_otherFlightConstraint.polyConstraint( p_type );
	return;			
}

// creates a SENDER passenger type
void CPassengerConstraint::makeSender (void)
{
	assert( m_pInTerm );
    intrinsicType = m_pInTerm->inStrDict->findString ("SENDER");
    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //    userTypes[i] = 0;
	m_paxType.initDefault();
}

// creates a GREETER passenger type
void CPassengerConstraint::makeGreeter (void)
{
	assert( m_pInTerm );
    intrinsicType = m_pInTerm->inStrDict->findString ("GREETER");
    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //    userTypes[i] = 0;
	m_paxType.initDefault();
}

void CPassengerConstraint::makeTransiting (void)
{
	assert( m_pInTerm );
    intrinsicType = m_pInTerm->inStrDict->findString ("TRANSIT");
}

void CPassengerConstraint::makeTransferring (void)
{
	assert( m_pInTerm );
    intrinsicType = m_pInTerm->inStrDict->findString ("TRANSFER");
}

void CPassengerConstraint::makeArrival (void)
{
	assert( m_pInTerm );
    intrinsicType = m_pInTerm->inStrDict->findString ("ARRIVAL");
}

void CPassengerConstraint::makeDeparture (void)
{
	assert( m_pInTerm );
    intrinsicType = m_pInTerm->inStrDict->findString ("DEPARTURE");
}

void CPassengerConstraint::initPaxType (const MobDescStruct &p_struct)
{
    intrinsicType = p_struct.intrinsic;
    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //    userTypes[i] = p_struct.paxType[i];
	m_paxType.initPaxType(p_struct);
}

// simple query to test for ARRIVAL
int CPassengerConstraint::isArrival (void) const
{
	assert( m_pInTerm );
    return intrinsicType == m_pInTerm->inStrDict->findString ("ARRIVAL");
}

// simple query to test for DEPARTURE
int CPassengerConstraint::isDeparture (void) const
{
	assert( m_pInTerm );
    return intrinsicType == m_pInTerm->inStrDict->findString ("DEPARTURE");
}

// simple query to test for SENDER
int CPassengerConstraint::isSender (void) const
{
	assert( m_pInTerm );
    return intrinsicType == m_pInTerm->inStrDict->findString ("SENDER");
}

// simple query to test for GREETER
int CPassengerConstraint::isGreeter (void) const
{
	assert( m_pInTerm );
    return intrinsicType == m_pInTerm->inStrDict->findString ("GREETER");
}

int CPassengerConstraint::isTurround (void) const
{
	assert( m_pInTerm );
	return intrinsicType == m_pInTerm->inStrDict->findString("TRANSIT") ||
		   intrinsicType == m_pInTerm->inStrDict->findString("TRANSFER");
}

void CPassengerConstraint::setUserType (const int *p_type)
{
    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //    userTypes[i] = p_type[i];
	m_paxType.setUserType(p_type);
}

void CPassengerConstraint::setUserType( int p_level, int p_type )
{
	//userTypes[p_level] = p_type;
	m_paxType.setUserType(p_level,p_type);
}
void CPassengerConstraint::getUserType (int *p_type) const
{
    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //    p_type[i] = userTypes[i];
	m_paxType.getUserType(p_type);
}

int CPassengerConstraint::getUserType( int p_level ) const
{
	//return userTypes[p_level];
	return m_paxType.getUserType(p_level);
}
void CPassengerConstraint::setConstraint ( const char *_str, int _nConstraintVersion )
{
	//char p_str[256];
	char p_str[1024]; //changed by matt

	strcpy( p_str, _str );
	assert( m_pInTerm );
    initDefault();

    char *str = (char *)strchr (p_str, '(');
    if (str) str[0] = '\0';

	CString strFlightCon( p_str );
	int iPos = strFlightCon.Find('#');
	if( iPos == -1 )
	{
		FlightConstraint::setConstraint ( p_str, _nConstraintVersion );
		m_otherFlightConstraint.initDefault();
	}
	else
	{
		CString strFirstFlightCon = strFlightCon.Left( iPos );
		CString strSecondFlightCon = strFlightCon.Mid( iPos + 1);

		FlightConstraint::setConstraint( strFirstFlightCon.GetBuffer(strFirstFlightCon.GetLength()), _nConstraintVersion );
		m_otherFlightConstraint.setConstraint_NotUse( strSecondFlightCon.GetBuffer(strSecondFlightCon.GetLength()), _nConstraintVersion );
	}

    if (!str) return;

    str++;
    char name[32];
    int i, ndx = 0;
    for (i = 0; str[ndx] != '-'; i++, ndx++)
        name[i] = str[ndx];
    name[i] = '\0';
	intrinsicType = (i)? m_pInTerm->inStrDict->findString (name): 0;

    for (int j = 0; j < MAX_PAX_TYPES && str[ndx] != ')'; j++)
	{
		for (i = 0, ndx++; str[ndx] != '-' && str[ndx] != ')' && str[ndx] != '<' && str[ndx] != '>'; i++, ndx++)
			name[i] = str[ndx];
		name[i] = '\0';
		//userTypes[j] = (i)? m_pInTerm->inStrDict->findString (name): 0;
		m_paxType.setUserType(j, (i)? m_pInTerm->inStrDict->findString (name): 0) ;
	}

	//////////////////////////////////////////////////////////////////////////
	// get nopax_count string
	CString strNopaxCount(_str);
	int pos1 = strNopaxCount.Find('<');
	int pos2 = strNopaxCount.Find('>');
	if( pos1== -1 ||  pos2 == -1 )
		return;
	strNopaxCount = strNopaxCount.Mid( pos1+1, pos2-pos1-1 );
	m_NonPaxCount.setValueByString( strNopaxCount );
}

void CPassengerConstraint::WriteSyntaxString (char *p_str) const
{

	assert( m_pInTerm );
	FlightConstraint::WriteSyntaxString (p_str);
	if( !m_otherFlightConstraint.isDefault() )
	{
		strcat( p_str, "#");
		m_otherFlightConstraint.WriteSyntaxString_NotUse( p_str+strlen(p_str) );
	}
	
    strcat (p_str, "(");
    if (intrinsicType)
        strcat (p_str, m_pInTerm->inStrDict->getString (intrinsicType));


	strcat (p_str, "-");

	CString strPaxType = m_paxType.GetStringForDatabase();
	strcat(p_str, strPaxType.GetString());

    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //{
    //    strcat (p_str, "-");
    //    if (userTypes[i] > 0)
    //        strcat (p_str, m_pInTerm->inStrDict->getString (userTypes[i]));
    //}
	//////////////////////////////////////////////////////////////////////////
	// get nonpax_count string
	CString strNonpaxCount;
	m_NonPaxCount.getValueString( strNonpaxCount );
	if( !strNonpaxCount.IsEmpty() )
	{
		strNonpaxCount = "<"+ strNonpaxCount +">";
		strcat( p_str, strNonpaxCount );
	}
    strcat (p_str, ")");
}
bool CPassengerConstraint::screenPrint(CString& p_str, int _nLevel , int p_maxChar) const
{
	if( _nLevel == 0 )
		p_str ="";
    if( CPassengerConstraint::isDefault())
    {
		if( _nLevel == 0 )
			 p_str += "DEFAULT" ;
        return true;
    }
	
	
    if (CPassengerConstraint::getCriteriaCount() > FlightConstraint::getCriteriaCount())
	{
		
		if( _nLevel > 0 )
			 p_str +="(";
		CString pTemStr;
		assert( m_pInTerm );
		if (intrinsicType)
		{
			 pTemStr =m_pInTerm->inStrDict->getString (intrinsicType);
			 p_str += pTemStr;
			if(pTemStr.GetLength() + p_str.GetLength() > p_maxChar)
			{
				p_str += "...";
				return false;
			}
		}
	
		if (!m_paxType.screenPrint(p_str,0,p_maxChar - strlen(p_str)))
			return false;

		//for (int i = 0; i < MAX_PAX_TYPES; i++)
		//{
		//	if (userTypes[i] >0 )
		//	{
		//		//if p_str not enough to load the string ,returned  //matt changed
		//		pTemStr = m_pInTerm->inStrDict->getString (userTypes[i]);
  //              if (p_str.GetLength() + pTemStr.GetLength() > p_maxChar)
		//		{
		//			p_str +="...";
		//			return false;
		//		}
		//		
		//		if(p_str && p_str.GetLength()>=1)
		//		{
		//			if (p_str.GetAt(p_str.GetLength()-1) != '(')
		//		    p_str+="-";
		//		}
		//		p_str +=m_pInTerm->inStrDict->getString (userTypes[i]);
		//	}
		//}
		// print non_pax count
		p_str += m_NonPaxCount.printScreen( m_pInTerm );
		
		if( _nLevel > 0 )
			 p_str += ") ";
	}
    if(!FlightConstraint::screenPrint ( p_str, _nLevel+1 ,p_maxChar))
	  return false;

	if( !m_otherFlightConstraint.isDefault() )
	{
		p_str +="<=";
		if(!m_otherFlightConstraint.screenPrint( p_str, _nLevel+1 ,p_maxChar))
			return false;
	}
	return true;
    //if (strlen (p_str) > p_maxChar)
    //        strcpy (p_str + (p_maxChar-4), "...");
    
}
void CPassengerConstraint::screenPrint (char *p_str, int _nLevel, unsigned p_maxChar) const
{
	if( _nLevel == 0 )
		p_str[0] = 0;
	else if (strlen (p_str) >= p_maxChar)		
		return;
	
    if( CPassengerConstraint::isDefault())
    {
		if( _nLevel == 0 )
			strcpy( p_str, "DEFAULT" );
        return;
    }

	
    if (CPassengerConstraint::getCriteriaCount() > FlightConstraint::getCriteriaCount())
	{

		if( _nLevel > 0 )
			strcat( p_str, "(");

		ASSERT( m_pInTerm );
	    if (intrinsicType)
	        strcat (p_str, m_pInTerm->inStrDict->getString (intrinsicType));

		if (!m_paxType.screenPrint(p_str,0,p_maxChar))
			return;

	   // for (int i = 0; i < MAX_PAX_TYPES; i++)
	   // {
	   //     if (userTypes[i] >0 )
	   //     {
				////if p_str not enough to load the string ,returned  //matt changed
				//const char * psNextString = m_pInTerm->inStrDict->getString (userTypes[i]);
                //if ((strlen (p_str) + strlen(psNextString)) > p_maxChar)
				//{
				//	if(strlen(psNextString) >4)
				//	{
				//		strcpy(p_str + strlen(p_str)-4,"...");
				//		return;
				//	}
				//	else
				//	{
				//		strcpy (p_str + (p_maxChar-4), "...");
				//		return;	
				//	}			
				//}
				//

	   //         if (p_str[strlen (p_str) - 1] != '(')
	   //             strcat (p_str, "-");
	   //         strcat (p_str, m_pInTerm->inStrDict->getString (userTypes[i]));
	   //     }
	   // }
		// print non_pax count
		strcat(p_str, m_NonPaxCount.printScreen( m_pInTerm ) );

		if( _nLevel > 0 )
			strcat( p_str, ") ");
	}
    FlightConstraint::screenPrint ( p_str, _nLevel+1 ,p_maxChar);
	if( !m_otherFlightConstraint.isDefault() )
	{
		strcat( p_str, "<=");
		m_otherFlightConstraint.screenPrint( p_str+strlen(p_str), _nLevel+1 ,p_maxChar);
	}
	
    if (strlen (p_str) > p_maxChar)
        strcpy (p_str + (p_maxChar-4), "...");
}


void CPassengerConstraint::screenTips( CString& strTips) const
{
	CString strLabel = "";
	FlightConstraint::screenTips( strTips );
	if( !m_otherFlightConstraint.isDefault() )
	{
		strTips += "\r\n";
		m_otherFlightConstraint.screenTips( strTips );
	}
	
    if (CPassengerConstraint::getCriteriaCount() == FlightConstraint::getCriteriaCount())
        return;

	strTips += "  Passenger\r\n";
	if( intrinsicType )
	{
		strLabel = CString ( m_pInTerm->inStrDict->getString (intrinsicType) );
		strTips = strTips +"    " +strLabel+ "\r\n";
	}

	//for(int i = 0; i < MAX_PAX_TYPES; i++)
	//{
	//	if (userTypes[i])
	//	{
	//		strLabel = CString( m_pInTerm->inStrDict->getString (userTypes[i]) );
	//		strTips = strTips + "      " +strLabel;
	//	}		
	//}
	for(int i = 0; i < MAX_PAX_TYPES; i++)
	{
		int nValue = m_paxType.getUserType(i);
		if (nValue >0)
		{
			strLabel = CString( m_pInTerm->inStrDict->getString (nValue) );
			strTips = strTips + "      " +strLabel;
		}		
	}
	// print non_pax count
	strTips = strTips + "\r\n" + m_NonPaxCount.printScreen( m_pInTerm );
}

void CPassengerConstraint::getPassengerType( char *p_str )
{

    if (getCriteriaCount() == FlightConstraint::getCriteriaCount())
	{
        strcpy (p_str, "DEFAULT");
        return;
	}

	assert( m_pInTerm );
    if (intrinsicType)
        strcat (p_str, m_pInTerm->inStrDict->getString (intrinsicType));

	m_paxType.screenPrint(p_str);

    //for (int i = 0; i < MAX_PAX_TYPES; i++)
    //{
    //    if (userTypes[i])
    //    {
    //        if (p_str[strlen (p_str) - 1] != '(')
    //            strcat (p_str, "-");
    //        strcat (p_str, m_pInTerm->inStrDict->getString (userTypes[i]));
    //    }
    //}
}

void CPassengerConstraint::screenPaxPrint( CString& _strLabel ) const
{
	_strLabel = "";

    if (CPassengerConstraint::getCriteriaCount() == FlightConstraint::getCriteriaCount())
        return;

	assert( m_pInTerm );
    if (intrinsicType)
		_strLabel = CString( m_pInTerm->inStrDict->getString (intrinsicType) );

   // for (int i = 0; i < MAX_PAX_TYPES; i++)
   // {
   //     if (userTypes[i])
   //     {
			//if( _strLabel.Right (1)  != "(")
			//	_strLabel += "-";
			//_strLabel += CString( m_pInTerm->inStrDict->getString (userTypes[i]) );
   //     }
   // }
	CString strPaxType = m_paxType.PrintStringForShow();
	if (!strPaxType.IsEmpty())
	{
		if (intrinsicType)
			_strLabel += "-";
		_strLabel += strPaxType;
	}
	
}
/*
stdostream &operator << (stdostream &s, const CPassengerConstraint &p_const)
{
    char str[128];
    p_const.screenPrint (str);
    return s << str;
}

*/


// init arr flight type for transfer.
void CPassengerConstraint::initOtherFlightType(const FlightDescStruct &p_struct)
{
	m_otherFlightConstraint.initFlightType( p_struct, 'A' );
}

void CPassengerConstraint::SetInputTerminal( InputTerminal * pInterm )
{
	m_pInTerm = pInterm;
	m_paxType.SetStringDictionary(pInterm->inStrDict);
	SetAirportDB(pInterm->m_pAirportDB);
}



CPassengerConstraint& CPassengerConstraint::operator=( const CPassengerConstraint& _otherConst )
{
	FlightConstraint::operator =(_otherConst);
	intrinsicType=_otherConst.intrinsicType;
	//memcpy((void*)userTypes,(const void*)_otherConst.userTypes,sizeof(int)*MAX_PAX_TYPES);
	m_paxType.copyData(_otherConst.m_paxType);
	if(_otherConst.m_pInTerm)
	{
		m_paxType.SetStringDictionary(_otherConst.m_pInTerm->inStrDict);
	}
	
	m_otherFlightConstraint = _otherConst.getOtherFlightConstraint();
	m_NonPaxCount = _otherConst.m_NonPaxCount;
	m_pInTerm = _otherConst.m_pInTerm;
	return *this;
}

int CPassengerConstraint::contains( int p_level, int p_index ) const
{
	//return userTypes[p_level] == p_index;
	return m_paxType.contains(p_level,p_index);
}

const CPassengerType* CPassengerConstraint::GetPaxType() const
{
	return &m_paxType;
}
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
// class CNonPaxCount
CNonPaxCount::CNonPaxCount()
{
	init();
}

CNonPaxCount::~CNonPaxCount()
{
	
}

bool CNonPaxCount::isDefault() const
{
	return m_vCountItem.size() == 0;
}

bool CNonPaxCount::isEqual( const CNonPaxCount& _other ) const
{
	// check num
	if( m_vCountItem.size() != _other.m_vCountItem.size() )
		return false;
	
	// check each item
	for( unsigned i=0; i< m_vCountItem.size(); i++ )
	{
		unsigned j= 0;
		for( ; j<_other.m_vCountItem.size(); j++ )
		{
			if( m_vCountItem[i] == _other.m_vCountItem[j] )
				break;
		}
		if( j == _other.m_vCountItem.size() )
			return false;
	}

	return true;
}

// read data from string
void CNonPaxCount::setValueByString( const CString& _str)
{
	// the format of the string like this:
	// the num of record | nopaxtype | logic operator | value| nopaxtype logic operator | value | ....
	init();
	
	CString str = _str;
	if( str.IsEmpty() )
		return;

	// get record num
	int pos = str.Find('|');
	if( pos == -1)
		throw new FileFormatError("PassengerConstraint");
	CString strNum = str.Left( pos );
	int iRecordNum = atoi( strNum );

	// get nopaxtype, operator and value
	CString strNopaxType, strOpearator, strValue;
	str = str.Mid( pos+1 );
	for( int i=0; i< iRecordNum; i++ )
	{
		NONPAXCOUNTITEM item;
		pos = str.Find('|');
		if( pos == -1)
			throw new FileFormatError("PassengerConstraint");
		strNopaxType = str.Left( pos );
		item.iNonPaxType = atoi( strNopaxType );
		
		str = str.Mid( pos+1 );
		pos = str.Find('|');
		if( pos == -1)
			throw new FileFormatError("PassengerConstraint");
		strOpearator = str.Left( pos );
		item.iOperator = (LogicOperator) (atoi( strOpearator));

		str = str.Mid( pos+1 );
		pos = str.Find('|');
		if( pos == -1)
			throw new FileFormatError("PassengerConstraint");
		strValue = str.Left( pos );
		item.iValue = atoi( strValue );

		str = str.Mid( pos+1 );
		
		m_vCountItem.push_back( item );
	}
}

// write data to string
void CNonPaxCount::getValueString( CString& _str ) const
{
	if( m_vCountItem.size() == 0 )
		return;

	CString strValue;
	strValue.Format("%d", m_vCountItem.size() );
	_str += strValue;

	for( unsigned i=0; i<m_vCountItem.size(); i++ )
	{
		strValue.Format("|%d|%d|%d", m_vCountItem[i].iNonPaxType, m_vCountItem[i].iOperator, m_vCountItem[i].iValue );
		_str += strValue;
	}
	_str += "|";
}



bool CNonPaxCount::fits( const CNonPaxCount& _other ) const
{
	if( isDefault() )
		return true;

	//the simple rules is that all the non-pax constraint items(_other) must be obey this non-pax constraint(this).
	//therefore, if the  non-pax type of the item (_other) does not exist, will create one item like this non-pax type == 0
	//if the pax type of item( this) does not exist, ignore this item

	for (int nThis = 0; nThis < getCount(); nThis++)
	{
		NONPAXCOUNTITEM thisItem = getItem( nThis );

		NONPAXCOUNTITEM other_item = _other.getItemByType(thisItem.iNonPaxType);
		
		//int nOther=0;
		//for (; nOther<_other.getCount(); nOther++)
		//{
		//	NONPAXCOUNTITEM other_item = _other.getItem( nOther );
			if( other_item.iOperator != Equal )  // actually only Equal is valid!!!
				return false;

			// check when the two NONPAXCOUNTITEMs talk about the same thing
			// copy from old code
			if( thisItem.iNonPaxType == other_item.iNonPaxType )
			{
				switch( thisItem.iOperator )
				{
				case less:
					if( !(other_item.iValue < thisItem.iValue ) )
						return false;
					break;

				case less_equal:
					if( !(other_item.iValue <= thisItem.iValue ) )
						return false;
					break;

				case Equal:
					if( !(other_item.iValue == thisItem.iValue ) )
						return false;
					break;

				case more_equal:
					if( !(other_item.iValue >= thisItem.iValue ) )
						return false;
					break;

				case more:
					if( !(other_item.iValue > thisItem.iValue ) )
						return false;
					break;

				case not_equal:
					if( !(other_item.iValue != thisItem.iValue ) )
						return false;
					break;

				default:
					{
						ASSERT(FALSE);
					}
					return false;
				}
				break;   // need to break;
			}
		//}
		//if (nOther>=_other.getCount()) // if _other has not the NONPAXCOUNTITEM talks about the same thing
		//{
		//	// we should care about that
		//	// although _other CNonPaxCount has not this item,
		//	// under some certain case,
		//	// it also fits !!!
		//	if ( false ==     // 
		//		(0 == thisItem.iValue && (Equal == thisItem.iOperator || less_equal == thisItem.iOperator || more_equal == thisItem.iOperator))
		//		|| (not_equal == thisItem.iOperator && 0 != thisItem.iValue)
		//		)
		//	{
		//		return false;
		//	}
		//}
	}

	return true;

}


LogicOperator CNonPaxCount::getOperatorByString( const CString& _strOperator)
{
	if( _strOperator == "<" )
		return less;
	if( _strOperator == "<=" )
		return less_equal;
	if( _strOperator == "==" )
		return Equal;
	if( _strOperator == ">=" )
		return more_equal;
	if( _strOperator == ">" )
		return more;
	if( _strOperator == "!=" )
		return not_equal;
	else
		return other_operator;
}

CString CNonPaxCount::getStringByOperator( LogicOperator _op )
{
	switch( _op )
	{
	case less:
		return "<";
		
	case less_equal:
		return "<=";

	case Equal:
		return "==";

	case more_equal:
		return ">=";

	case more:
		return ">";
		
	case not_equal:
		return "!=";
		
	default:
		return "";
	}
}

CString CNonPaxCount::printScreen( InputTerminal* _pInTerm  ) const
{
	assert( _pInTerm );
	CString strReturn, strItem;
	for( int i=0; i<getCount(); i++ )
	{
		NONPAXCOUNTITEM item = m_vCountItem[i];
		strItem.Format("%s%s%d;",  _pInTerm->m_pMobElemTypeStrDB->GetString( item.iNonPaxType ),
								  CNonPaxCount::getStringByOperator( item.iOperator),
								  item.iValue );
		strReturn += strItem;
	}	

	if( !strReturn.IsEmpty() )
		strReturn = "<" + strReturn + ">";
	return strReturn;
}

NONPAXCOUNTITEM CNonPaxCount::getItemByType( int nonPaxType ) const
{
	for( unsigned i=0; i<m_vCountItem.size(); i++ )
	{
		if( m_vCountItem[i].iNonPaxType == nonPaxType)
		{
			return m_vCountItem[i];
		}
	}

	NONPAXCOUNTITEM item;
	item.iNonPaxType	= nonPaxType;
	item.iOperator		= Equal;
	item.iValue			= 0;
	
	return item;

}
