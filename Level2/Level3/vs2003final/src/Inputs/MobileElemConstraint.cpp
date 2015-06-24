// MobileElemConstraint.cpp: implementation of the CMobileElemConstraint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MobileElemConstraint.h"
#include <assert.h>
#include "common\exeption.h"
#include "MobileElemTypeStrDB.h"
#include "..\Inputs\IN_TERM.H"
#include "..\Inputs\TrayHost.h"
#include "..\Common\strdict.h"

std::vector<CString> CMobileElemConstraint::m_strList;	// keep all the string


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMobileElemConstraint::CMobileElemConstraint(InputTerminal * pInterm) : CPassengerConstraint(pInterm)
{
	initDefault(); 
}

CMobileElemConstraint::CMobileElemConstraint( const CMobileElemConstraint& _otherConst ) :CPassengerConstraint(_otherConst)
{
	//	m_strList.assign(_otherConst.m_strList.begin(), _otherConst.m_strList.end());
	m_nTypeIndex=_otherConst.m_nTypeIndex;
	m_enumElementType = _otherConst.m_enumElementType;
}
CMobileElemConstraint::~CMobileElemConstraint()
{

}


// initializes a constraint that fits all possible objects
void CMobileElemConstraint::initDefault()
{
	
	CPassengerConstraint::initDefault();
	m_nTypeIndex = FindOrAddString( CS_PASSENGER );	
	assert( m_nTypeIndex == 0 );
	m_enumElementType = enum_MobileElementType_ALL;
}


// checks to see if all values are default
int CMobileElemConstraint::isDefault() const
{
	if( !CPassengerConstraint::isDefault() )
		return FALSE;

	if (m_enumElementType == enum_MobileElementType_ALL)
	{
		return TRUE;
	}
	return FALSE;
	//return m_nTypeIndex == 0;
}


// basic logic that does a full compare of 2 constraints
int CMobileElemConstraint::isEqual (const Constraint* _pConstr) const
{
	if( !CPassengerConstraint::isEqual( _pConstr ) )
		return FALSE;

	CMobileElemConstraint* pConstr = (CMobileElemConstraint *)_pConstr;
	return (m_nTypeIndex == pConstr->m_nTypeIndex)&&(m_enumElementType== pConstr->m_enumElementType);


}


// returns the number of criteria defining the constraint
int CMobileElemConstraint::getCriteriaCount (void) const
{
	int nCount = CPassengerConstraint::getCriteriaCount();

	if( m_nTypeIndex > 0 )
		nCount+=100;			// make sure the no pax item go first.

	return nCount;
}

// returns the combined precedence level of all criteria
// criteria are ordered with 0 as the most specific, or lowest value
// The FlightConstraint component defines precedence up to 7
int CMobileElemConstraint::getTotalPrecedence (void) const
{
	int nPrecedence = CPassengerConstraint::getTotalPrecedence();

	if( m_nTypeIndex > 0 )
		nPrecedence += 13 + m_nTypeIndex;

	return nPrecedence;
}


// returns TRUE if pt fits within this constraint
// if constraint is default, always returns true
// if constraint is closed, always returns false
// constraint is then tested field by field to try to find conflict
int CMobileElemConstraint::fits( const CMobileElemConstraint& _constr, bool _bIgnoreStar /*= false*/ ) const
{
	if( isNone() )
		return FALSE;
	if(isDefault() )
	{	
		//if(m_enumElementType == enum_MobileElementType_PAX)//passenger
		//{ 
		//	if(m_enumElementType != _constr.m_enumElementType)
		//		return FALSE;
		//}
		//else//non-passenger
		//{
		//	if( m_nTypeIndex != _constr.m_nTypeIndex )
		//		return FALSE;
		//}
		//if( m_nTypeIndex != _constr.m_nTypeIndex )
		//	return FALSE;
		return TRUE;
	}

	if( !CPassengerConstraint::fits( _constr, _bIgnoreStar ) )
		return FALSE;


	if(m_enumElementType == enum_MobileElementType_ALL)//passenger
	{
		return TRUE;
	}
	else if(m_enumElementType == enum_MobileElementType_PAX)//passenger
	{ 
		if(m_enumElementType != _constr.m_enumElementType)
			return FALSE;
	}
	else//non-passenger
	{
		if( m_nTypeIndex != _constr.m_nTypeIndex )
		{
			int nMaxTypeIndex = max(m_nTypeIndex,_constr.m_nTypeIndex);
			int nMinTypeindex = min(m_nTypeIndex,_constr.m_nTypeIndex);
			if(nMaxTypeIndex < ALLPAX_COUNT)
				return FALSE;
			else
			{
				int nCount = m_pInTerm->GetTrayHostList()->GetItemCount();
				if (nCount < nMaxTypeIndex-NOPAX_COUNT)
				{
					return FALSE;
				}
				TrayHost* pTrayHost = m_pInTerm->GetTrayHostList()->GetItem(nMaxTypeIndex-ALLPAX_COUNT);
				if(pTrayHost->FindItem(nMinTypeindex) == -1 && pTrayHost->GetHostIndex() !=nMinTypeindex)
					return FALSE;
			}
		}
	}

	//if( m_nTypeIndex != _constr.m_nTypeIndex )
	//	return FALSE;

	return TRUE;
}
int CMobileElemConstraint::fitsparent( const CMobileElemConstraint& _constr, bool _bIgnoreStar /*= false */ ) const
{
	if( isNone() )
		return FALSE;
	if( isDefault() )
	{
		return TRUE;
	}

	if( !CPassengerConstraint::fits( _constr, _bIgnoreStar ) )
		return FALSE;


	if(m_enumElementType == enum_MobileElementType_ALL)//passenger
	{
		return TRUE;
	}
	else if(m_enumElementType == enum_MobileElementType_PAX)//passenger
	{ 
		//if(m_enumElementType != _constr.m_enumElementType)
		//	return FALSE;
		return TRUE;
	}
	else//non-passenger
	{
		if( m_nTypeIndex != _constr.m_nTypeIndex )
		{
			int nMaxTypeIndex = max(m_nTypeIndex,_constr.m_nTypeIndex);
			int nMinTypeindex = min(m_nTypeIndex,_constr.m_nTypeIndex);
			if(nMaxTypeIndex < ALLPAX_COUNT)
				return FALSE;
			else
			{
				int nCount = m_pInTerm->GetTrayHostList()->GetItemCount();
				if (nCount < nMaxTypeIndex-NOPAX_COUNT)
				{
					return FALSE;
				}
				TrayHost* pTrayHost = m_pInTerm->GetTrayHostList()->GetItem(nMaxTypeIndex-ALLPAX_COUNT);
				if(pTrayHost->FindItem(nMinTypeindex) == -1 && pTrayHost->GetHostIndex() !=nMinTypeindex)
					return FALSE;
			}
		}
	}

	//if( m_nTypeIndex != _constr.m_nTypeIndex )
	//	return FALSE;

	return TRUE;
}
int CMobileElemConstraint::fitex( const CMobileElemConstraint& _constr, bool _bIgnoreStar/* = false*/) const
{
	if( isNone() )
		return FALSE;
	if( isDefault() )
	{
		//if(m_enumElementType == enum_MobileElementType_PAX)//passenger
		//{ 
		//	if(m_enumElementType != _constr.m_enumElementType)
		//		return FALSE;
		//}
		//else//non-passenger
		//{
		//	if( m_nTypeIndex != _constr.m_nTypeIndex )
		//		return FALSE;
		//}
		return TRUE;
	}
	
	if( !CPassengerConstraint::fitex( _constr, _bIgnoreStar) )
		return FALSE;
	
	if(m_enumElementType == enum_MobileElementType_ALL)//passenger
	{
		return TRUE;
	}
	else if(m_enumElementType == enum_MobileElementType_PAX)//passenger
	{ 
		if(m_enumElementType != _constr.m_enumElementType)
			return FALSE;
	}
	else//non-passenger
	{
		if( m_nTypeIndex != _constr.m_nTypeIndex )
		{
			int nMaxTypeIndex = max(m_nTypeIndex,_constr.m_nTypeIndex);
			int nMinTypeindex = min(m_nTypeIndex,_constr.m_nTypeIndex);
			if(nMaxTypeIndex < ALLPAX_COUNT)
				return FALSE;
			else
			{
				int nCount = m_pInTerm->GetTrayHostList()->GetItemCount();
				if (nCount < nMaxTypeIndex-NOPAX_COUNT)
				{
					return FALSE;
				}
				TrayHost* pTrayHost = m_pInTerm->GetTrayHostList()->GetItem(nMaxTypeIndex-ALLPAX_COUNT);
				if(pTrayHost->FindItem(nMinTypeindex) == -1 && pTrayHost->GetHostIndex() !=nMinTypeindex)
					return FALSE;
			}
		}
	}
		//if( m_nTypeIndex != _constr.m_nTypeIndex )
		//	return FALSE;
	
	return TRUE;
}

//poly two constraint to a constraint
void CMobileElemConstraint::polyConstraint( const CMobileElemConstraint* p_type)
{
	if(isDefault() && p_type->isDefault())
		return;
	CPassengerConstraint::polyConstraint( p_type );
	if( m_nTypeIndex == -1 )
		m_nTypeIndex = p_type->m_nTypeIndex;
	if(m_enumElementType == enum_MobileElementType_ALL)
		m_enumElementType = p_type->m_enumElementType;
}

void CMobileElemConstraint::MergeFlightConstraint(const FlightConstraint* p_type)
{
	if(isDefault() && p_type->isDefault())
		return;

	char szFlightInfo[256] = {0};

	// airline
	p_type->getAirline(szFlightInfo);
	setAirline(szFlightInfo);

	// airline group
	p_type->getAirlineGroup(szFlightInfo);
	setAirlineGroup(szFlightInfo);
	
	p_type->getFlightGroup(szFlightInfo);
	setFlightGroup(szFlightInfo);
	// flight ID
	setFlightID(p_type->getFlightID());

	// mode
	SetFltConstraintMode(p_type->GetFltConstraintMode());

	// airport
	p_type->getAirport(szFlightInfo);
	setAirport(szFlightInfo);

	// stopover airport
	p_type->getStopoverAirport(szFlightInfo);
	setStopoverAirport(szFlightInfo);

	// ac type
	p_type->getACType(szFlightInfo);
	setACType(szFlightInfo);

	// from sector
	p_type->getSector(szFlightInfo);
	setSector(szFlightInfo);

	// to sector
	p_type->getStopoverSector(szFlightInfo);
	setStopoverSector(szFlightInfo);

	// to category
	p_type->getCategory(szFlightInfo);
	setCategory(szFlightInfo);
}

// used by processor roster.
void CMobileElemConstraint::makeNone()
{
	initDefault();
	m_nTypeIndex = -1;
	m_enumElementType = enum_MobileElementType_ALL;
}

int CMobileElemConstraint::isNone() const
{
	return m_nTypeIndex == -1;
}


void CMobileElemConstraint::initMobType( const MobDescStruct& _struct )
{
	CPassengerConstraint::initPaxType( _struct );
	m_nTypeIndex = _struct.mobileType;
	if (m_nTypeIndex == 0)// mobile element have no "All" state, it is a pax or non-pax
	{
		m_enumElementType = enum_MobileElementType_PAX;
	}
	else
	{
		m_enumElementType = enum_MobileElementType_NONPAX;
	}
}

//#### EXCP: FileFormatError ####
void CMobileElemConstraint::setConstraint (const char *_str, int _nConstraintVersion )
{
	//char p_str[256]; 
	char p_str[1024];
	strcpy( p_str, _str );
	assert( m_pInTerm );
    initDefault();
    if (!strcmp (p_str, "NONE"))
    {
        makeNone();
        return;
    }

    char *str = (char *)strchr (p_str, '{');
    if (str) 
		str[0] = '\0';
    CPassengerConstraint::setConstraint ( p_str, _nConstraintVersion );
    if( !str ) 
		return;

    str++;
	char* pStr = (char*)strchr( str, '}' );
	if( pStr )
	{
		pStr[0] = 0;
	}
	else
	{
		throw new FileFormatError( "Mobile Element Type " );
	}

	// process str.
	CString strPaxType = CString( str );
	if (strPaxType.Find("-") == -1)
	{
		int nIdx;
		if( isNumericString( strPaxType ) )
		{
			nIdx = atoi( strPaxType );
			if( nIdx>=0 && nIdx < m_pInTerm->m_pMobElemTypeStrDB->GetCount() )
			{
				m_nTypeIndex = nIdx;
			}
		}
		else
		{
			nIdx = m_pInTerm->m_pMobElemTypeStrDB->GetIndex( strPaxType );
			if( nIdx<0 )
				throw new FileFormatError( "Mobile Element Type " );
			m_nTypeIndex = nIdx;
		}

		if(m_nTypeIndex == 0)
		{
			m_enumElementType = enum_MobileElementType_ALL;
		}
		else
		{
			m_enumElementType = enum_MobileElementType_NONPAX;
		}


	}
	else
	{
		int nIndex = strPaxType.Find("-");
		CString strNonPaxType = strPaxType.Left(nIndex);

		int nIdx;
		if( isNumericString( strNonPaxType ) )
		{
			nIdx = atoi( strNonPaxType );
			if( nIdx>=0 /*&& nIdx < m_pInTerm->m_pMobElemTypeStrDB->GetCount() */)
			{
				m_nTypeIndex = nIdx;
			}
		}
		else
		{
			nIdx = m_pInTerm->m_pMobElemTypeStrDB->GetIndex( strNonPaxType );
			if( nIdx<0 )
				throw new FileFormatError( "Mobile Element Type " );
			m_nTypeIndex = nIdx;
		}

		CString strElementType = strPaxType.Mid(nIndex +1, strPaxType.GetLength());

		if( isNumericString( strElementType ) )
		{
			int nType = atoi(strElementType);
			if(nType>=enum_MobileElementType_ALL && nType <= enum_MobileElementType_NONPAX)
				m_enumElementType = (enum_MobileElementType)nType;
			else
				m_enumElementType = enum_MobileElementType_ALL;

		}
		else
		{
			m_enumElementType = enum_MobileElementType_ALL;

		}
	}
}


void CMobileElemConstraint::WriteSyntaxString( char *p_str ) const
{
    if( isNone() )	// < 0 
    {
        strcpy( p_str, "NONE" );
        return;
    }

	assert( m_pInTerm );
	CPassengerConstraint::WriteSyntaxString( p_str );

    strcat( p_str, "{" );
	//strcat( p_str, m_pInTerm->m_pMobElemTypeStrDB->GetString( m_nTypeIndex ) );
	CString strPaxType;
	strPaxType.Format("%d",m_nTypeIndex );
	strcat( p_str, strPaxType );
	CString strElementType;
	strElementType.Format("%d",(int)m_enumElementType);
	strcat(p_str,"-");
	strcat(p_str,strElementType);
    strcat( p_str, "}" );
}

void CMobileElemConstraint::screenPrint (char *p_str, int _nLevel, unsigned p_maxChar) const
{
	p_str[0] = 0;
    if( isNone() )	// < 0 
    {
        strcpy( p_str, "NONE" );
        return;
    }

	if( isDefault() )
	{
        strcpy( p_str, "DEFAULT" );
		//strcpy( p_str, CS_PASSENGER );
        return;
	}
		

	ASSERT( m_pInTerm );

	if( m_enumElementType == enum_MobileElementType_PAX)
	{
		strcat( p_str, CS_PASSENGER );
		CPassengerConstraint::screenPrint( p_str, 1, p_maxChar);
	}
	else
	{
		if( m_nTypeIndex > 0 )
		{
			if (m_nTypeIndex < ALLPAX_COUNT)
			{
				strcat( p_str, m_pInTerm->m_pMobElemTypeStrDB->GetString( m_nTypeIndex ) );
			}
			else
			{
				if (m_nTypeIndex < m_pInTerm->GetTrayHostList()->GetItemCount() + ALLPAX_COUNT)
				{
					strcat(p_str,m_pInTerm->GetTrayHostList()->GetItem(m_nTypeIndex - ALLPAX_COUNT)->GetHostName());
				}
			}
			CPassengerConstraint::screenPrint( p_str, 1, p_maxChar);
		}
		else
		{

			CPassengerConstraint::screenPrint( p_str, 0, p_maxChar);
		}

	}

    if (strlen (p_str) > p_maxChar)
        strcpy (p_str + (p_maxChar-4), "...");
}

//return false means overBuf with maxchar.
bool CMobileElemConstraint::screenPrint(CString &str, int _nLevel /* = 0  */, int p_maxChar/* =70 */) const
{
	str="";

    if( isNone() )	// < 0 
    {
        str= str + "NONE" ;
        return true;
    }	
	if( isDefault() )
	{
        str= str +"DEFAULT" ;
		//str= str +CS_PASSENGER;
        return true;
	}
	assert( m_pInTerm );
	
	if (m_enumElementType == enum_MobileElementType_PAX)
	{
		
		str += CS_PASSENGER;
		if(str.GetLength() > p_maxChar)
		{
			str +="...";
			return false;
		}
		if(!CPassengerConstraint::screenPrint( str, 1, p_maxChar))
			return false;
	}
	else if(m_enumElementType == enum_MobileElementType_NONPAX)
	{
		CString tempStr(_T(""));
		if (m_nTypeIndex < ALLPAX_COUNT)
		{
			tempStr =m_pInTerm->m_pMobElemTypeStrDB->GetString( m_nTypeIndex );
		}
		else
		{
			if (m_nTypeIndex < m_pInTerm->GetTrayHostList()->GetItemCount() + ALLPAX_COUNT)
			{
				tempStr = m_pInTerm->GetTrayHostList()->GetItem(m_nTypeIndex - ALLPAX_COUNT)->GetHostName();
			}
		}
		str += /*m_pInTerm->m_pMobElemTypeStrDB->GetString( m_nTypeIndex );*/tempStr;
		if(tempStr.GetLength() + str.GetLength() > p_maxChar)
		{
			str +="...";
			return false;
		}
		if(!CPassengerConstraint::screenPrint( str, 1, p_maxChar))
			return false;
	}
	else
	{
		if(!CPassengerConstraint::screenPrint( str, 0, p_maxChar)) 
			return false;
	}
	//if( m_nTypeIndex > 0 )
	//{
	//	CString tempStr =m_pInTerm->m_pMobElemTypeStrDB->GetString( m_nTypeIndex );
	//	str += m_pInTerm->m_pMobElemTypeStrDB->GetString( m_nTypeIndex );
	//	if(tempStr.GetLength() + str.GetLength() > p_maxChar)
	//	{
	//		str +="...";
	//		return false;
	//	}
	//	if(!CPassengerConstraint::screenPrint( str, 1, p_maxChar))
	//		return false;
	//}
	//else
	//{
	//	if(!CPassengerConstraint::screenPrint( str, 0, p_maxChar)) 
	//		return false;
	//}

	return true;
}
void CMobileElemConstraint::screenTips( CString& strTips) const
{
	CString strLabel = "";
	strTips = "";
	
	if( isNone() )	// < 0 
    {
        return;
    }

	assert( m_pInTerm );

	screenMobPrint( strLabel );
	if( !strLabel.IsEmpty() )
	{
		strTips = strLabel + "  Owned By:\r\n";
	}

	CPassengerConstraint::screenTips( strTips );

}


void CMobileElemConstraint::screenMobPrint(CString& _strLabel) const
{
	_strLabel = "";

	if( isNone() )	// < 0 
    {
        _strLabel = "NONE";
        return;
    }

	assert( m_pInTerm );

 //   if( m_nTypeIndex == 0 )
	//	return;

	//_strLabel = m_pInTerm->m_pMobElemTypeStrDB->GetString( m_nTypeIndex );

	if (m_enumElementType == enum_MobileElementType_PAX)
	{
		_strLabel = CS_PASSENGER;
	}
	else if(m_enumElementType == enum_MobileElementType_NONPAX)
	{
		if(m_nTypeIndex > 0)
		{
			if (m_nTypeIndex < ALLPAX_COUNT)
			{
				_strLabel = m_pInTerm->m_pMobElemTypeStrDB->GetString( m_nTypeIndex );
			}
			else
			{
				if (m_nTypeIndex < m_pInTerm->GetTrayHostList()->GetItemCount() + ALLPAX_COUNT)
				{
					_strLabel = m_pInTerm->GetTrayHostList()->GetItem(m_nTypeIndex - ALLPAX_COUNT)->GetHostName();
				}
			}
		}
	}

}

stdostream& operator << (stdostream &s, const CPassengerConstraint &p_const)
{
    char str[128];
    p_const.screenPrint (str,0,128);
    return s << str;
}


//##################################################
//  proteced
//##################################################

// get the index of the string, if new add into it and return index.
int CMobileElemConstraint::FindOrAddString( CString _csStr )
{
	int nSize = m_strList.size();
	for( int i=0; i<nSize; i++ )
	{
		if( m_strList[i] == _csStr )
			return i;
	}
	m_strList.push_back( _csStr );
	return nSize;
}


bool CMobileElemConstraint::isNumericString( const CString& _str ) const
{
	for( int i=0; i<_str.GetLength(); i++ )
		if( !isNumeric( _str[i] ) )
			return false;
	return true;
}



const CMobileElemConstraint& CMobileElemConstraint::operator=( const CMobileElemConstraint& _otherConst )
{
	CPassengerConstraint::operator =(_otherConst);
	//	m_strList.assign(_otherConst.m_strList.begin(), _otherConst.m_strList.end());
	m_nTypeIndex=_otherConst.m_nTypeIndex;
	m_enumElementType = _otherConst.m_enumElementType;
	return *this;
}

void CMobileElemConstraint::SetMobileElementType( enum_MobileElementType enumType )
{
	m_enumElementType = enumType;
}

enum_MobileElementType CMobileElemConstraint::GetMobileElementType()
{
	return m_enumElementType;
}

void CMobileElemConstraint::getPaxTypeSynaxString( CString& p_str)
{
	//if (getCriteriaCount() == FlightConstraint::getCriteriaCount())
	//{
	//	strcpy (p_str, "DEFAULT");
	//	return;
	//}

	//for (int i = 0; i < MAX_PAX_TYPES; i++)
	//{
	//	if (userTypes[i] > 0)
	//		strcat (p_str, m_pInTerm->inStrDict->getString (userTypes[i]));
	//	strcat (p_str, "-");		
	//}
	p_str = m_paxType.GetStringForDatabase();
}

void CMobileElemConstraint::getFltTypeSynaxString( char *p_str )
{
	sprintf( p_str, "%3d", VERSION_CONSTRAINT_CURRENT );
	char tempStr[1024];
	FlightConstraint::WriteSyntaxString( tempStr );
	strcat( p_str, tempStr );
}