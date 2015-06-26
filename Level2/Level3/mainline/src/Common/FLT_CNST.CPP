#include "stdafx.h"
#include "./flt_cnst.h"
#include "category.h"
#include "../common/termfile.h"
#include "../common/exeption.h"
#include "../common/airportdatabase.h"
#include "../common/AirportsManager.h"
#include "../common/ACTypesManager.h"
#include "../Common/FlightManager.h"
#include "../common/aircraft.h"
#include "../common/airlinemanager.h"
#include "../common/term_bin.h"
#include "../common/AIRSIDE_BIN.h"
#include "../Common/SuperTypeRelationData.h"

const TCHAR  FlightConstraint::ActypeReplaceChar('$')  ; // the char will replace the '-' in actype .
//in parameter: need convert word
//out parameter: safe word
void ReplaceKeyWordWithSafeWord(CString& strUnSafeword)
{
	strUnSafeword.Replace(_T("-"),_T("/1"));
}

void ReplaceSaftWordWithKeyWord(CString& strSafeword)
{
	if (strSafeword.Find(_T("$")) != -1)
	{
		strSafeword.Replace(_T("$"),_T("-"));
	}
	else if (strSafeword.Find(_T("/1")) != -1)
	{
		strSafeword.Replace(_T("/1"),_T("-"));
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// creates default constraint (fits all)
void FlightConstraint::initDefault (void)
{
	airline = '\0';
    flightID = 0;
	m_nDay = 0;
    m_enumFltCnstrMode = ENUM_FLTCNSTR_MODE_ALL;
	m_airport = '\0';
	m_stopoverAirport = '\0';
	acType = '\0';
	m_nFlightIndex = 0;
	m_pAirlineGroup = NULL;
	m_pFlightGroup = NULL;
	m_pSector = NULL;
	m_pStopoverSector = NULL;
    m_pCategory = NULL;
}

// checks to see if all values are default
int FlightConstraint::isDefault (void) const
{
	return !(airline.HasValue() || flightID.HasValue() || ( m_enumFltCnstrMode != ENUM_FLTCNSTR_MODE_ALL )  || 
		m_airport.HasValue() || m_stopoverAirport.HasValue() || m_nDay != 0 ||
		acType.HasValue() || m_pAirlineGroup || m_pFlightGroup ||
		m_pSector || m_pStopoverSector || 
		m_pCategory );
}

// basic logic that does a full compare of 2 constraints
int FlightConstraint::isEqual (const Constraint *p_const) const
{
    FlightConstraint *aConst = (FlightConstraint *)p_const;
    if (strcmp (airline, aConst->airline))
		return 0;

    if (flightID != aConst->flightID)
		return 0;

	if (m_nDay != aConst->m_nDay)
		return 0;
	
    if ( m_enumFltCnstrMode != aConst->m_enumFltCnstrMode )
        return 0;

	if (strcmp ( m_airport, aConst->m_airport))
		return 0;

	if (strcmp ( m_stopoverAirport, aConst->m_stopoverAirport ) )
		return 0;

    if (strcmp (acType, aConst->acType))
        return 0;

	if( m_pAirlineGroup != aConst->m_pAirlineGroup )
		return 0;

	if (m_pFlightGroup != aConst->m_pFlightGroup)
	{
		return 0;
	}
	if( m_pSector != aConst->m_pSector )
		return 0;

	if( m_pStopoverSector != aConst->m_pStopoverSector )
		return 0;

    if( m_pCategory != aConst->m_pCategory )
        return 0;

    return 1;
}

// returns the number of criteria defining the constraint
int FlightConstraint::getCriteriaCount (void) const
{
	int count = 0;

	// if field not == default, increment count
    //if (airline[0])
	if (airline.HasValue())
		count++;

    //if (flightID)
	if (flightID.HasValue())
		count++;

    if ( m_enumFltCnstrMode != ENUM_FLTCNSTR_MODE_ALL )
        count++;

    //if (airport[0])
	if (m_airport.HasValue())
		count++;

	if (m_stopoverAirport.HasValue())
		count++;

    //if (acType[0])
	if (acType.HasValue())
        count++;

	if( m_pAirlineGroup )
		count++;

	if (m_pFlightGroup)
	{
		count++;
	}

	if (m_pSector)
		count++;

	if (m_pStopoverSector)
		count++;

    if (m_pCategory)
        count++;

    return count;
}

// returns the combined precedence level of all criteria
// criteria are ordered with 0 as the most specific, or lowest value
int FlightConstraint::getTotalPrecedence (void) const
{
	int precedence = 0;

    if (airline.HasValue()) precedence += 2;
	//	precedence ++;

    if (flightID.HasValue()) precedence += 2;
		//precedence += 2;

    if ( m_enumFltCnstrMode != ENUM_FLTCNSTR_MODE_ALL ) 
		precedence += 2;
        //precedence += 3;

    //if (airport[0]) precedence += 2;
	if (m_airport.HasValue()) precedence += 2;
	if (m_stopoverAirport.HasValue()) precedence += 2;
        //precedence += 4;

    //if (acType[0])  precedence += 2;
	if (acType.HasValue())  precedence += 2;
        //precedence += 5;

	if ( m_pAirlineGroup ) precedence += 1;
		//precedence += 6;
	
	if (m_pFlightGroup)
	{
		precedence += 1;
	}
	if (m_pSector) precedence += 1;
	if (m_pStopoverSector) precedence += 1;
        //precedence += 7;

    if (m_pCategory) precedence += 1;
        //precedence += 8;
          
	return precedence;
}

// returns 1 if pt fits within this constraint
// if constraint is default, always returns true
// if constraint is closed, always returns false
// constraint is then tested field by field to try to find conflict
int FlightConstraint::fits (const FlightConstraint& p_type, bool _bIgnoreStar /*= false*/) const
{
    if (isDefault())
        return 1;

	// if this != 0 (ANY) && flightIDs don't match, return 0
    //if (flightID && flightID != p_type.flightID)
	if (flightID.HasValue() && flightID != p_type.flightID)
        return 0;

	if (m_nDay != 0 && m_nDay != p_type.GetDay())
		return 0;
	
	if (m_enumFltCnstrMode != ENUM_FLTCNSTR_MODE_ALL && m_enumFltCnstrMode != p_type.GetFltConstraintMode())
		return 0;

	    // if this != 0 (ANY) && acTypes don't match, return 0
    //if (acType[0] && strcmp (acType, p_type.acType))
	if (!fitsACType(p_type.acType))
        return 0;
    // if this != 0 (ANY) && categoryList don't match, return 0
    if ( m_pCategory )
	{
		if( p_type.m_pCategory )
		{
			if( ! (*m_pCategory == *p_type.m_pCategory ) )
				return 0;

			//if( p_type.acType[0] && !m_pCategory->contains( p_type.acType ) )
			if( p_type.acType.HasValue() && !m_pCategory->contains( p_type.acType ) )
				return 0;
		}
		else
		{
			//if( !p_type.acType[0] )
			if( !p_type.acType.HasValue() )
				return 0;
			if( !m_pCategory->contains( p_type.acType ) )
				return 0;
		}
	}


	
    // if this != "" (ANY) && airlines don't match, return 0
    //if (airline[0] && strcmp (airline, p_type.airline))

	if (  airline.HasValue() )
	{
		// need to handle AC == AC* == AC**
		if( _bIgnoreStar )
		{
			if( strcmp( airline.GetNonStarValue(), p_type.airline.GetNonStarValue() ) )
				return 0;
		}
		else if( strcmp (airline, p_type.airline) )
			return 0;
	}
	// if this != 0 (ANY) && airlineList don't match, return 0

	if ( m_pAirlineGroup )
	{
		// when this have airline group. the p_type need to have airline group or airline.
		if( p_type.m_pAirlineGroup )
		{
			if( ! ( *m_pAirlineGroup == *p_type.m_pAirlineGroup ) )
				return 0;

			//if( p_type.airline[0] && !m_pAirlineGroup->contains( p_type.airline ) )
//			if( p_type.airline.HasValue() && !m_pAirlineGroup->contains( p_type.airline ) )
//				return 0;

			if( p_type.airline.HasValue() )
			{
				if( _bIgnoreStar )
				{
					if( !m_pAirlineGroup->contains( p_type.airline.GetNonStarValue() ) )
						return 0;
				}
				else
				{
					if( !m_pAirlineGroup->contains( p_type.airline ) )
						return 0;
				}
			}
		}
		else
		{
			//if( !p_type.airline[0] )
			if( !p_type.airline.HasValue() )
				return 0;
			if( _bIgnoreStar )
			{
				if( !m_pAirlineGroup->contains( p_type.airline.GetNonStarValue() ) )
					return 0;
			}
			else
			{
				if( !m_pAirlineGroup->contains( p_type.airline ) )
					return 0;
			}
//			if( !m_pAirlineGroup->contains( p_type.airline ) )
//				return 0;
		}
	}


	if (m_pFlightGroup)
	{
 		if(p_type.m_pFlightGroup)
		{
 			if( m_pFlightGroup->getGroupName().CompareNoCase(p_type.m_pFlightGroup->getGroupName()) != 0  )
 				return 0;
		}
		else
		{
			char szbuff[255];
			sprintf(szbuff,"%d",p_type.GetDay());
			ElapsedTime eTime;
			eTime.setPrecisely(p_type.GetTime());

			ElapsedTime eGateOccupancy;
			eGateOccupancy.setPrecisely(p_type.GetGateOccupancy());
			if (!m_pFlightGroup->contain(p_type.airline.GetValue(),p_type.flightID.GetValue(),szbuff,eTime,eGateOccupancy))
			{
				return 0;
			}
		}
	}
    // if this != 0 (ANY) && airports don't match, return 0
    //if (airport[0] && strcmp (airport, p_type.airport))
	if (m_airport.HasValue() && strcmp (m_airport, p_type.m_airport))
		return 0;
	if (m_stopoverAirport.HasValue() && strcmp (m_stopoverAirport, p_type.m_stopoverAirport))
		return 0;
	// if this != 0 (ANY) && sectorList don't match, return 0
    
	if( !IsSectorInclude( p_type, false ) )
		return 0;
	if( !IsSectorInclude( p_type, true ) )
		return 0;

	return 1;
}


// check if the flightDesc fit this flightContrain.
int FlightConstraint::fits( const FlightDescStruct& _fltDesc ) const
{
    if (isDefault())
        return 1;

	if ((!_fltDesc.arrID.HasValue()) && (!_fltDesc.depID.HasValue()))
		return 0;

	if( m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ALL)
	{	
		FlightConstraint arr_type(m_pAirportDB);
		FlightConstraint dep_type(m_pAirportDB);

		if(_fltDesc.arrID.HasValue() && (!_fltDesc.depID.HasValue()))
		{		
			arr_type.initFlightType(_fltDesc,'A');
			return fits(arr_type);
		}
		if (_fltDesc.depID.HasValue() && (!_fltDesc.arrID.HasValue()))
		{
			dep_type.initFlightType(_fltDesc,'D');
			return fits(dep_type);
		}

		arr_type.initFlightType(_fltDesc,'A');
		dep_type.initFlightType(_fltDesc,'D');

		return (fits(arr_type)|| fits(dep_type));		
	}

	// check m_enumFltCnstrMode
	if( m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR && !_fltDesc.arrID.HasValue())
		return 0;

	if( m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_DEP && !_fltDesc.depID.HasValue())
		return 0;

	if (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_THU)
		return 0;

	// check fli id
	CFlightID fltIDFromDesc = (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR)? _fltDesc.arrID : _fltDesc.depID;
	if( flightID.HasValue() && flightID != fltIDFromDesc )
        return 0;

	long lTime =  (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR)? _fltDesc.arrTime: _fltDesc.depTime;
	ElapsedTime eTime;
	eTime.setPrecisely(lTime);
	
	if (m_nDay != 0 && m_nDay != eTime.GetDay())
		return 0;

	assert( _fltDesc.acType.HasValue() );

	// check actype
	if (!fitsACType(_fltDesc.acType))
        return 0;

	// check ac cat
    if ( m_pCategory && !m_pCategory->contains( _fltDesc.acType ) )
		return 0;

	assert( _fltDesc.airline.HasValue() );
	
	// airline
	if( airline.HasValue() && strcmp (airline, _fltDesc.airline) )
        return 0;

	// airline group
	if( m_pAirlineGroup && !m_pAirlineGroup->contains( _fltDesc.airline ) )
		return 0;

	//flight group
	if(m_pFlightGroup)
	{
		CString ArrDay;
		ElapsedTime arrTime;
		arrTime.setPrecisely (_fltDesc.arrTime);
		ArrDay.Format("%d", arrTime.GetDay());

		CString DepDay;
		ElapsedTime DepTime;
		DepTime.setPrecisely (_fltDesc.depTime);
		DepDay.Format("%d", DepTime.GetDay());

		ElapsedTime eGateOccupancy;
		eGateOccupancy = _fltDesc.gateOccupancy;

		if(!m_pFlightGroup->contain(_fltDesc.airline.GetValue(),(_fltDesc.arrID.HasValue()?_fltDesc.arrID.GetValue():_fltDesc.depID.GetValue()),(_fltDesc.arrTime>0?ArrDay:DepDay),
			(_fltDesc.arrTime>0?arrTime:DepTime),eGateOccupancy))
		{
			return 0;
		}
	}

	CAirportCode airportCodeFromDesc = (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR)? _fltDesc.origin : _fltDesc.destination;

	assert( airportCodeFromDesc.HasValue() );

	// airport
	if( m_airport.HasValue() && strcmp (m_airport, airportCodeFromDesc ) )
        return 0;

	if( m_pSector && !m_pSector->contains( airportCodeFromDesc ) )
		return 0;


	// stopover airport
	airportCodeFromDesc = (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR)? _fltDesc.arrStopover : _fltDesc.depStopover;

	if( m_stopoverAirport.HasValue() && !airportCodeFromDesc.HasValue() )
        return 0;

	if( m_stopoverAirport.HasValue() && strcmp (m_stopoverAirport, airportCodeFromDesc ) )
        return 0;

	if( m_pStopoverSector && !airportCodeFromDesc.HasValue() )
        return 0;

	if( m_pStopoverSector && !m_pStopoverSector->contains( airportCodeFromDesc ) )
		return 0;


    return 1;
}

int FlightConstraint::fits(const AirsideFlightDescStruct& _fltDesc) const
{
	if (isDefault())
		return 1;

	if ((!_fltDesc._arrID.HasValue()) && (!_fltDesc._depID.HasValue()))
		return 0;

	if( m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ALL)
	{	
		FlightConstraint arr_type(m_pAirportDB);
		FlightConstraint dep_type(m_pAirportDB);

		if(_fltDesc._arrID.HasValue() && (!_fltDesc._depID.HasValue()))
		{		
			arr_type.initFlightType(_fltDesc,'A');
			return fits(arr_type);
		}
		if (_fltDesc._depID.HasValue() && (!_fltDesc._arrID.HasValue()))
		{
			dep_type.initFlightType(_fltDesc,'D');
			return fits(dep_type);
		}

		arr_type.initFlightType(_fltDesc,'A');
		dep_type.initFlightType(_fltDesc,'D');

		return (fits(arr_type)|| fits(dep_type));		
	}

	// check m_enumFltCnstrMode
	if( m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR && !_fltDesc._arrID.HasValue() )
		return 0;

	if( m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_DEP && !_fltDesc._depID.HasValue() )
		return 0;

	if (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_THU)
		return 0;

	// check fli id
	CFlightID fltIDFromDesc = (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR)? _fltDesc._arrID : _fltDesc._depID;
	if( flightID.HasValue() && flightID != fltIDFromDesc )
		return 0;

	assert( _fltDesc._acType.HasValue() );

	long lTime =  (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR)? _fltDesc.arrTime: _fltDesc.depTime;
	ElapsedTime eTime;
	eTime.setPrecisely(lTime);

	if (m_nDay != 0 && m_nDay != eTime.GetDay())
		return 0;

	// check actype
	if (!fitsACType(_fltDesc._acType))
		return 0;

	// check ac cat
	if ( m_pCategory && !m_pCategory->contains( _fltDesc._acType ) )
		return 0;

	assert( _fltDesc._airline.HasValue() );

	// airline
	if( airline.HasValue() && strcmp (airline, _fltDesc._airline) )
		return 0;

	// airline group
	if( m_pAirlineGroup && !m_pAirlineGroup->contains( _fltDesc._airline ) )
		return 0;

	CAirportCode airportCodeFromDesc = (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR)? _fltDesc._origin : _fltDesc._destination;

	assert( airportCodeFromDesc.HasValue() );

	// airport
	if( m_airport.HasValue() && strcmp (m_airport, airportCodeFromDesc ) )
		return 0;

	if( m_pSector && !m_pSector->contains( airportCodeFromDesc ) )
		return 0;


	// stopover airport
	airportCodeFromDesc = (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR)? _fltDesc._arrStopover : _fltDesc._depStopover;

	if( m_stopoverAirport.HasValue() && !airportCodeFromDesc.HasValue() )
		return 0;

	if( m_stopoverAirport.HasValue() && strcmp (m_stopoverAirport, airportCodeFromDesc ) )
		return 0;

	if( m_pStopoverSector && !airportCodeFromDesc.HasValue() )
		return 0;

	if( m_pStopoverSector && !m_pStopoverSector->contains( airportCodeFromDesc ) )
		return 0;


	return 1;
}

int FlightConstraint::fits( const AirsideFlightDescStruct& _fltDesc, char mode ) const
{
	FlightConstraint flt_type(m_pAirportDB);
	flt_type.initFlightType(_fltDesc,mode);
	return fits(flt_type);
}

//poly two constraint to a constraint
void FlightConstraint::polyConstraint( const FlightConstraint* p_type)
{
	if(isDefault() && p_type->isDefault())
		return;
   nConstraintVersion = p_type->nConstraintVersion;
	//if this == "" set p_type's value to this
	//if( !airline[0])
	if (!airline.HasValue())
		//strcpy( airline, p_type->airline );
		airline = p_type->airline;

	if( !flightID.HasValue())
		flightID = p_type->flightID;

	if( m_enumFltCnstrMode==ENUM_FLTCNSTR_MODE_ALL)
		m_enumFltCnstrMode = p_type->m_enumFltCnstrMode;

	if( !m_airport.HasValue())
		m_airport = p_type->m_airport;
	if( !m_stopoverAirport.HasValue())
		m_stopoverAirport = p_type->m_stopoverAirport;

	//if( !acType[0])
	if (!acType.HasValue())
		//strcpy( acType, p_type->acType );
		acType = p_type->acType;

	if( !m_pAirlineGroup )
		m_pAirlineGroup = p_type->m_pAirlineGroup;

	if (!m_pFlightGroup)
	{
		m_pFlightGroup = p_type->m_pFlightGroup;
	}
	if( !m_pSector )
		m_pSector = p_type->m_pSector;
	if( !m_pStopoverSector )
		m_pStopoverSector = p_type->m_pStopoverSector;

	if( !m_pCategory)
		m_pCategory = p_type->m_pCategory;
}

const FlightConstraint &FlightConstraint::operator =
    (const FlightConstraint &p_const)
{
	Constraint::operator =(p_const);
    //strcpy (airline, p_const.airline);
	airline = p_const.airline;
    flightID = p_const.flightID;
	m_nDay = p_const.m_nDay;
	m_eTime = p_const.m_eTime;
	m_eGateOccupancy = p_const.m_eGateOccupancy;
    m_enumFltCnstrMode = p_const.m_enumFltCnstrMode;
    //strcpy (acType, p_const.acType);
	acType = p_const.acType;
    //strcpy (airport, p_const.airport);
	m_airport = p_const.m_airport;
	m_stopoverAirport = p_const.m_stopoverAirport;
	m_pAirlineGroup = p_const.m_pAirlineGroup;
	m_pFlightGroup = p_const.m_pFlightGroup;
	m_pSector = p_const.m_pSector;
	m_pStopoverSector = p_const.m_pStopoverSector;
    m_pCategory = p_const.m_pCategory;
	//m_pInTerm = p_const.m_pInTerm;	
	m_pAirportDB = p_const.m_pAirportDB;
	nConstraintVersion = p_const.nConstraintVersion ;
    return *this;
}

void FlightConstraint::setFullID (const char *p_str)
{
    if ( strlen( p_str ) == 0 || !strcmp (p_str, "DEFAULT"))
    {
        //airline[0] = '\0';
		airline = '\0';
        flightID = 0;
        return;
    }

    // copy airline
	// comment by kevin
    //for (int ndx = 0; isalpha (p_str[ndx]) && ndx < AIRLINE_LEN-1; ndx++)
	/*for (int ndx = 0; (isalpha (p_str[ndx]) || p_str[ndx] == '*' || ndx < 2 ) && ndx < AIRLINE_LEN-1; 
		ndx++)
        airline[ndx] = p_str[ndx];
    airline[ndx] = '\0';*/
	char szBuffer[AIRLINE_LEN];
	int ndx=0;
	memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
    for ( ndx = 0; (isalpha (p_str[ndx]) || p_str[ndx] == '*' || ndx < 2 ) && ndx < AIRLINE_LEN-1; 
		ndx++)
        //airline[ndx] = p_str[ndx];
		szBuffer[ndx] = p_str[ndx];
    //airline[ndx] = '\0';
	szBuffer[ndx] = '\0';

	airline = szBuffer;

    // copy flight ID number
    flightID = p_str + ndx;
}


void FlightConstraint::setAirlineGroup (const char *p_str)
{
	//assert( m_pInTerm );
	

	CSubairline findAirlineGroup( m_pAirportDB->getAirlineMan() );
	findAirlineGroup.setCode (p_str);
	int ndx = _g_GetActiveAirlineMan( m_pAirportDB )->findSubairlineItem( &findAirlineGroup );
	m_pAirlineGroup = (ndx == INT_MAX)? NULL: _g_GetActiveAirlineMan( m_pAirportDB )->getSubairlineItem(ndx);
}

void FlightConstraint::setFlightGroup(const char* p_str)
{
	FlightGroup	findFlightGroup;
	findFlightGroup.setGroupName(p_str);
	int ndx = _g_GetActiveFlightMan(m_pAirportDB)->findFlightGroupItem(&findFlightGroup);
	m_pFlightGroup = (ndx == INT_MAX)?NULL:_g_GetActiveFlightMan(m_pAirportDB)->getGroupItem(ndx);
}
void FlightConstraint::setSector (const char *p_str )
{
	assert( m_pAirportDB );

	CSector findSect( m_pAirportDB->getAirportMan() );
	findSect.setCode (p_str);
	int ndx = _g_GetActiveAirportMan( m_pAirportDB )->findSectItem( &findSect );
	m_pSector = (ndx == INT_MAX)? NULL: _g_GetActiveAirportMan( m_pAirportDB )->getSectItem (ndx);
}

void FlightConstraint::setStopoverSector (const char *p_str )
{
	//assert( m_pInTerm );

	CSector findSect( m_pAirportDB->getAirportMan() );
	findSect.setCode (p_str);
	int ndx = _g_GetActiveAirportMan( m_pAirportDB )->findSectItem( &findSect );
	m_pStopoverSector = (ndx == INT_MAX)? NULL: _g_GetActiveAirportMan( m_pAirportDB )->getSectItem (ndx);
}

void FlightConstraint::setCategory (const char *p_str)
{
	//assert( m_pInTerm );

    CACCategory findCat( m_pAirportDB->getAcMan() );
    findCat.setCatName (p_str);
    int ndx = _g_GetActiveACMan( m_pAirportDB )->findCatItem (&findCat);
	m_pCategory = (ndx == INT_MAX)? NULL: _g_GetActiveACMan( m_pAirportDB )->getCatItem (ndx);
}

// Assume p_mode = { 'A', 'D' };
void FlightConstraint::initFlightType (const FlightDescStruct &p_struct, char p_mode)
{
    //strcpy (airline, p_struct.airline);
	airline = p_struct.airline;
    flightID = (p_mode == 'A')? p_struct.arrID: p_struct.depID;
	long lTime =  (p_mode == 'A')? p_struct.arrTime: p_struct.depTime;
	m_eTime.setPrecisely(lTime);
	m_nDay = m_eTime.GetDay();
	m_eGateOccupancy = p_struct.gateOccupancy;
	m_enumFltCnstrMode = (p_mode=='A') ? ENUM_FLTCNSTR_MODE_ARR : ENUM_FLTCNSTR_MODE_DEP;

	m_airport = (p_mode == 'A')? p_struct.origin: p_struct.destination;
	m_stopoverAirport = (p_mode == 'A')? p_struct.arrStopover: p_struct.depStopover;

    //strcpy (acType, p_struct.acType);
	acType = p_struct.acType;
	m_pAirlineGroup = NULL;
	m_pSector = NULL;
	m_pStopoverSector = NULL;
    m_pCategory = NULL;
	m_pFlightGroup = NULL;
}

void FlightConstraint::initFlightType( const AirsideFlightDescStruct& p_struct, char p_mode )
{
	//strcpy (airline, p_struct.airline);
	airline = p_struct._airline;
	flightID = (p_mode == 'A')? p_struct._arrID: p_struct._depID;
	long lTime =  (p_mode == 'A')? p_struct.arrTime: p_struct.depTime;
	m_eTime.setPrecisely(lTime);
	m_nDay = m_eTime.GetDay();
	m_eGateOccupancy.setPrecisely(p_struct.gateOccupancy);

	m_enumFltCnstrMode = (p_mode=='A') ? ENUM_FLTCNSTR_MODE_ARR : ENUM_FLTCNSTR_MODE_DEP;

	m_airport = (p_mode == 'A')? p_struct._origin: p_struct._destination;
	m_stopoverAirport = (p_mode == 'A')? p_struct._arrStopover: p_struct._depStopover;
	
	acType = p_struct._acType;
	m_pAirlineGroup = NULL;
	m_pSector = NULL;
	m_pStopoverSector = NULL;
	m_pCategory = NULL;
	m_pFlightGroup = NULL;
}
void FlightConstraint::getFullID (char *p_str) const
{  
    if (!airline[0])
    {
        strcpy (p_str, "DEFAULT");
        return;
    }
    strcpy (p_str, airline);
    if (flightID.HasValue())
        //itoa (flightID, p_str + strlen (p_str), 10);
		strcat(p_str, flightID);
}

void FlightConstraint::getAirlineGroup( char* p_str) const
{
    if (m_pAirlineGroup)
	{
		CString strAirlineGroup = m_pAirlineGroup->m_sName;
		ReplaceKeyWordWithSafeWord(strAirlineGroup);
		strcpy (p_str,strAirlineGroup.GetString());
	}
    else
        p_str[0] = '\0';
}

void FlightConstraint::getFlightGroup(char* p_str)const
{
	if (m_pFlightGroup)
	{
		CString strFlightGroupName = m_pFlightGroup->getGroupName();
		strcpy(p_str,strFlightGroupName.GetString());
	}
	else
		p_str[0] = '\0';
}
void FlightConstraint::getSector (char *p_str ) const
{
    if (m_pSector)
	{
		CString strSector = m_pSector->m_sName;
        strcpy(p_str,strSector.GetString());
	}
    else
        p_str[0] = '\0';
}

void FlightConstraint::getStopoverSector (char *p_str ) const
{
    if (m_pStopoverSector)
	{
		CString strStopoverSector = m_pStopoverSector->m_sName;
		strcpy(p_str,strStopoverSector.GetString());
	}
    else
        p_str[0] = '\0';
}

void FlightConstraint::getCategory (char *p_str) const
{
    if (m_pCategory)
	{
		CString strCategory = m_pCategory->m_sName;
		strcpy(p_str,strCategory.GetString());
	}
    else
        p_str[0] = '\0';
}

int FlightConstraint::fitsAirline( const char* p_airline ) const
{
    //if( airline[0])
	if( airline.HasValue())
        return !_strcmpi (airline, p_airline);
    else if (m_pAirlineGroup)
        return m_pAirlineGroup->contains (p_airline);
    else
        return 1;
}


int FlightConstraint::fitsAirport (const char *p_airport) const
{
	if (m_airport.HasValue())
        return !_strcmpi (m_airport, p_airport);
    else if (m_pSector)
        return m_pSector->contains (p_airport);
    else
        return 1;
}

int FlightConstraint::fitsStopoverAirport (const char *p_stopoverAirport) const
{
	if (m_stopoverAirport.HasValue())
        return !_strcmpi (m_stopoverAirport, p_stopoverAirport);
    else if (m_pStopoverSector)
        return m_pStopoverSector->contains (p_stopoverAirport);
    else
        return 1;
}
void FlightConstraint::setConstraint_NotUse( const char *p_str, int _nConstraintVersion )
{
        setConstraint(p_str,_nConstraintVersion) ;
}
// assumes file based string with all delimiters
//this function will parse the p_str,if it find p_str is new format version ,it will set the version of FlightConstraint ,and then use new version 
//handle to parse the string and init the class FlightConstraint .
//version 102 format : "Airlineflightid -......"
//version 103 format : "#versionAirline- flightid - ........" this format is only used for Database currently .
//added by cjchen .
//////////////////////////////////////////////////////////////////////////
void FlightConstraint::setConstraint (const char *p_str, int _nConstraintVersion )
{
    //if version < 103 ,use old version handle .
	char str[1024] = {0};
	strcpy(str,p_str);
	if(_nConstraintVersion <= VERSION_CONSTRAINT_CURRENT_102 && str[0] != '#')
	{
	CFlightConstraintReadHandleByVersion constr(this);
	constr.setConstraint(str,_nConstraintVersion);
	}else{
          // this->nConstraintVersion = GetAndRemoveConstraintVersion(&str[0]);
		   switch (_nConstraintVersion)
		   {
		   case VERSION_CONSTRAINT_CURRENT_103:
			   CFlightConstraintReadHandleByVersion103 constraint(this);
			   constraint.setConstraint(&str[0],nConstraintVersion);
		   	break;
		   }
	}

	
}

void FlightConstraint::WriteSyntaxString_NotUse(char *p_str)const
{
   WriteSyntaxString(p_str) ;
}
// creates file based string with all delimiters
void FlightConstraint::WriteSyntaxString (char *p_str) const
{

	if (isDefault())
	{
		strcpy (p_str, "DEFAULT");
		return;
	}
//airline, airport,actype,sector,flight group,category need convert for key word -/////////////////////
	p_str[0] = '\0';
	//if (airline[0])
	if (getAirline().HasValue())
	{
		CString strAirline(getAirline());
		ReplaceKeyWordWithSafeWord(strAirline);
		strcat (p_str,strAirline);
	}
	strcat(p_str,"-");
	if ( getFlightID().HasValue() )
		//itoa (flightID, p_str + strlen (p_str), 10);
	{

		strcat(p_str, getFlightID());
	}

	strcat (p_str, "-");
	if (GetFltConstraintMode() != ENUM_FLTCNSTR_MODE_ALL)
	{
		CString strMode;
		getFltConModeString(strMode);
		strcat(p_str, strMode);
	}

	strcat (p_str, "-");
	//if (airport[0])
	if (getAirport().HasValue())
	{
		CString strAiport(getAirport());
		ReplaceKeyWordWithSafeWord(strAiport);
		strcat (p_str,strAiport);
	}

	strcat (p_str, "-");
	//if (airport[0])
	if (getStopoverAirport().HasValue())
	{
		CString strStopAirport(getStopoverAirport());
		ReplaceKeyWordWithSafeWord(strStopAirport);
		strcat (p_str,strStopAirport);
	}

	strcat (p_str, "-");
	//if (acType[0])
	if (getACType().HasValue())
	{
		CString Pactype(getACType()) ; // if actype is 310-100, it should replace '-' ,otherwise the format is error when save to the database. 
		ReplaceKeyWordWithSafeWord(Pactype);
		strcat (p_str, Pactype);
	}

	strcat (p_str,"-");
	if( m_pAirlineGroup )
	{
		CString strAirlineGroup(m_pAirlineGroup->m_sName);
		ReplaceKeyWordWithSafeWord(strAirlineGroup);
		strcat(p_str,strAirlineGroup);
	}

	strcat (p_str,"-");
	if (m_pFlightGroup)
	{
		CString strFlightGroup(m_pFlightGroup->getGroupName());
		ReplaceKeyWordWithSafeWord(strFlightGroup);
		strcat(p_str,strFlightGroup);
	}

	strcat (p_str, "-");
	if (m_pSector)
	{
		CString strSector(m_pSector->m_sName);
		ReplaceKeyWordWithSafeWord(strSector);
		strcat(p_str,strSector);
	}

	strcat (p_str, "-");
	if (m_pStopoverSector)
	{
		CString strStopoverSector(m_pStopoverSector->m_sName);
		ReplaceKeyWordWithSafeWord(strStopoverSector);
		strcat(p_str,strStopoverSector);
	}

	strcat (p_str, "-");
	if (m_pCategory)
	{
		CString strCategory(m_pCategory->m_sName);
		ReplaceKeyWordWithSafeWord(strCategory);
		strcat(p_str,strCategory);
	}

////////////convert end//////////////////////////////////////////////////////////////////////////
 }

//format P_str  which will be set in Database .
//it should be used when format string by flightconstraint ,and the string will be putted into the DB  ,
//it's a new function after version 103 ;
//get more messages with version format form function setConstraint's note !!
//not use any more 
//void FlightConstraint::printConstraintStringToDB( char* p_str,int _ver)  const
//{
//   nConstraintVersion = _ver ;
//	wsprintf(p_str,"#%3d",nConstraintVersion);
//	WriteSyntaxString(&p_str[4]);
//}
// creates screen based string showing only defined info
void FlightConstraint::screenPrint (char *p_str, int _nLevel, unsigned p_maxChar) const
{

	int len = strlen(p_str) ;
	if( _nLevel == 0 )
		p_str[0] = 0;
	else 
	{
		if (len > (int)p_maxChar || len == (int)p_maxChar)		
			return;
	}


	if(isDefault())
	{
		if( _nLevel == 0 )
			strcpy( p_str, "DEFAULT" );
		return;
	}

	bool bGotFirst = false;

	if( _nLevel > 0 )
		strcat( p_str, "(");
	//if (airline[0])

	if (getAirline().HasValue())
	{
		bGotFirst = true;
		strcat (p_str, getAirline());
		if (getFlightID().HasValue())
		{
			strcat(p_str,"-");
			//itoa (flightID, p_str + strlen (p_str), 10);
			strcat(p_str, getFlightID());
		}
	}

	if (m_enumFltCnstrMode!=ENUM_FLTCNSTR_MODE_ALL)
	{
		if ( bGotFirst)
			strcat (p_str, "-");
		bGotFirst = true;

		CString strMode;
		getFltConModeString(strMode);
		strcat (p_str, strMode);
	}

	//if (airport[0])
	if (m_airport.HasValue())
	{
		if ( bGotFirst)
			strcat (p_str, "-");
		bGotFirst = true;
		strcat (p_str, m_airport);
	}
	if (m_stopoverAirport.HasValue())
	{
		if ( bGotFirst)
			strcat (p_str, "-");
		bGotFirst = true;
		strcat (p_str, m_stopoverAirport);
	}

	//if (acType[0])
	if (acType.HasValue())
	{
		if ( bGotFirst)
			strcat (p_str, "-");
		bGotFirst = true;
		strcat (p_str, acType);
	}

	if( m_pAirlineGroup )
	{
		if ( bGotFirst)
			strcat (p_str, "-");
		bGotFirst = true;
		getAirlineGroup( p_str + strlen (p_str) );
	}

	if (m_pFlightGroup)
	{
		if (bGotFirst)
		{
			strcat(p_str,"-");
		}
		bGotFirst = true;
		getFlightGroup(p_str + strlen(p_str));
	}
	if( m_pSector )
	{
		if ( bGotFirst)
			strcat (p_str, "-");
		bGotFirst = true;
		getSector( p_str + strlen (p_str) );
	}
	if( m_pStopoverSector )
	{
		if ( bGotFirst)
			strcat (p_str, "-");
		bGotFirst = true;
		getStopoverSector( p_str + strlen (p_str) );
	}

	if (m_pCategory)
	{
		if ( bGotFirst)
			strcat (p_str, "-");
		bGotFirst = true;
		getCategory (p_str + strlen (p_str));
	}

	if( _nLevel > 0 )
		strcat( p_str, ") ");
	len = strlen(p_str) ;
	if ( len > (int)p_maxChar)
		strcpy (p_str + (p_maxChar-4), "...");
	
}
bool FlightConstraint::screenPrint(CString &pStr, int _nLevel /* =0  */, int p_maxChar/* =70 */) const
{
	
	char str[1024] = {0};
	FlightConstraint::screenPrint(str,_nLevel,p_maxChar);
	//	p_str.Empty();
	pStr.Append(str);
	return true;
}

void FlightConstraint::screenTips( CString& strTips) const
{
	char szBuf[64];
	CString strLabel;

	if (isDefault())
		return;

	strTips += "  Flight\r\n";

	if( m_enumFltCnstrMode != ENUM_FLTCNSTR_MODE_ALL )
	{
		strLabel;
		getFltConModeString(strLabel);
		strTips  = strTips +"    "+strLabel+"\r\n";
	}

	if( airline.HasValue() )
	{
		strLabel = "    Airline=" + CString( airline);
		if( flightID )
		{
			//_itoa( flightID, szBuf, 10);
			strcpy(szBuf, flightID);
			strLabel =strLabel + CString( szBuf);
		}
		strTips = strTips + strLabel + "\r\n";
	}

	getAirlineGroup( szBuf );
	strLabel = CString( szBuf );
	if( !strLabel.IsEmpty() )
	{
		strLabel ="    AirlineGroup=" + strLabel + "\r\n";
		strTips += strLabel;
	}


	if( m_airport.HasValue() )
	{
		strLabel = "    Airport=" + CString(m_airport) + "\r\n";
		strTips += strLabel;
	}
	if( m_stopoverAirport.HasValue() )
	{
		strLabel = "    Stopover Airport=" + CString(m_stopoverAirport) + "\r\n";
		strTips += strLabel;
	}

	getSector( szBuf );
	strLabel = CString( szBuf );
	if( !strLabel.IsEmpty() )
	{
		strLabel = "    Sector=" + strLabel + "\r\n";
		strTips += strLabel;
	}
	getStopoverSector( szBuf );
	strLabel = CString( szBuf );
	if( !strLabel.IsEmpty() )
	{
		strLabel = "    Stopover Sector=" + strLabel + "\r\n";
		strTips += strLabel;
	}

	getACType( szBuf );
	strLabel = CString( szBuf );
	if( !strLabel.IsEmpty())
	{
		strLabel = "    ACType=" + strLabel + "\r\n";
		strTips += strLabel;
	}

	getCategory( szBuf);
	strLabel = CString(szBuf);
	if( !strLabel.IsEmpty())
	{
		strLabel = "    Category = " + strLabel + "\r\n";
		strTips += strLabel;
	}
	
}


void FlightConstraint::screenFltPrint( CString& _strLabel ) const
{

	CString strTmp;
	_strLabel = "";

	if (isDefault())
		return;

	//if (airline[0])
	//if (flightconstraint->airline.HasValue())
	//{
	//	_strLabel += flightconstraint->airline;
	//	if (flightconstraint->flightID.HasValue())
	//	{
	//		//_itoa( flightID, szTmp,10);
	//		strcpy(szTmp, flightconstraint->flightID);
	//		_strLabel += CString( szTmp );
	//	}   
	//}
	char str[1024] = {0};
	if (getAirline().HasValue())
	{

		strcat (str, getAirline());
		if (getFlightID().HasValue())
		{
			strcat(str,"-");
			//itoa (flightID, p_str + strlen (p_str), 10);
			strcat(str, getFlightID());
		}
	}
	_strLabel.Append(str);
	if (m_enumFltCnstrMode!=ENUM_FLTCNSTR_MODE_ALL)
	{
		if ( !_strLabel.IsEmpty ())
			_strLabel += "-";

		CString strMode;
		getFltConModeString(strMode);
		_strLabel += strMode;
	}

	if (m_airport.HasValue())
	{
		if ( !_strLabel.IsEmpty () )
			_strLabel += "-";
		_strLabel += CString( m_airport );
	}
	if (m_stopoverAirport.HasValue())
	{
		if ( !_strLabel.IsEmpty () )
			_strLabel += "-";
		_strLabel += CString( m_stopoverAirport );
	}

	//if (acType[0])
	if (acType.HasValue())
	{
		if ( !_strLabel.IsEmpty () )
			_strLabel += "-";
		_strLabel += CString(acType);
	}

	if( m_pAirlineGroup )
	{
		if (!_strLabel.IsEmpty ())
			_strLabel += "-";
		getAirlineGroup( strTmp.GetBuffer(1024) );
		strTmp.ReleaseBuffer();
		_strLabel += strTmp;
	}

	if (m_pFlightGroup)
	{
		if (!_strLabel.IsEmpty())
		{
			_strLabel += "-";
		}
		getFlightGroup(strTmp.GetBuffer(1024));
		strTmp.ReleaseBuffer();
		_strLabel += strTmp;
	}
	if (m_pSector)
	{
		if ( !_strLabel.IsEmpty () )
			_strLabel += "-";
		getSector ( strTmp.GetBuffer (1024) );
		strTmp.ReleaseBuffer();
		_strLabel += strTmp;
		//strTmp.ReleaseBuffer ();
	}
	if (m_pStopoverSector)
	{
		if ( !_strLabel.IsEmpty () )
			_strLabel += "-";
		getStopoverSector ( strTmp.GetBuffer (1024) );
		strTmp.ReleaseBuffer();
		_strLabel += strTmp;
		//strTmp.ReleaseBuffer ();
	}

	if (m_pCategory)
	{
		if ( !_strLabel.IsEmpty () )
			_strLabel += "-";
		getCategory (strTmp.GetBuffer (1024));
		strTmp.ReleaseBuffer();
		_strLabel += strTmp;
		//strTmp.ReleaseBuffer ();
	}

}


// check if _enumAirport sector include the _otherFlightContraint
// return true / false.
bool FlightConstraint::IsSectorInclude( const FlightConstraint& _otherFlightContraint, bool _bStopover ) const
{
	CSector* pThisSector;
	CSector* pOtherSector;
	CAirportCode otherAirport;

	if( _bStopover )
	{
		pThisSector = m_pStopoverSector;
		pOtherSector = _otherFlightContraint.m_pStopoverSector;
		otherAirport = _otherFlightContraint.m_stopoverAirport;
	}
	else
	{
		pThisSector = m_pSector;
		pOtherSector = _otherFlightContraint.m_pSector;
		otherAirport = _otherFlightContraint.m_airport;
	}

	if ( !pThisSector )
		return true;

	if( pOtherSector )
	{
		// compare sector
		if( ! ( *pThisSector == *pOtherSector ) )
			return false;

		// if there is airport information need to check if this sector include the airport.
		if( otherAirport.HasValue() && ! pThisSector->contains( otherAirport ) )
			return false;
	}
	else
	{
		// if no airport information should return false.
		if( !otherAirport.HasValue() )
			return false;

		// check if this sector include the airport.
		if( !pThisSector->contains( otherAirport ) )
			return false;
	}

	return true;
}

void FlightConstraint::getFltConModeString(CString& strFltMode) const
{
	strFltMode.Empty();

	if (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_ARR)
		strFltMode = "ARRIVING";
	else if (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_DEP)
		strFltMode = "DEPARTING";
	else if (m_enumFltCnstrMode == ENUM_FLTCNSTR_MODE_THU)
		strFltMode = "THROUGH";
}
BOOL FlightConstraint::fitsACType(const CAircraftType& _AircraftType)const
{
	if(!acType.HasValue())
		return TRUE ;
	if(_AircraftType == acType)
		return TRUE ;
	ASSERT(m_pAirportDB) ;
	if(m_pAirportDB != NULL)
	{
		SuperTypeRelationData* relation = m_pAirportDB->getAcMan()->GetActypeRelationShip() ;
		if(relation == NULL)
			return FALSE ;
		else
			relation->Fit(_AircraftType.GetValue(),acType.GetValue()) ;
	}
	return FALSE ;
}

void FlightConstraint::getAirline( char *p_str ) const
{
	strcpy (p_str, airline);
}

void FlightConstraint::getAirport( char *p_str ) const
{
	strcpy (p_str, m_airport);
}

void FlightConstraint::getStopoverAirport( char *p_str ) const
{
	strcpy (p_str, m_stopoverAirport);
}

void FlightConstraint::getACType( char *p_str ) const
{
	strcpy (p_str, acType);
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////

CFlightConstraintReadHandleByVersion::CFlightConstraintReadHandleByVersion( FlightConstraint* fli_constraint):flightconstraint(fli_constraint)
{

}
void CFlightConstraintReadHandleByVersion::setFlightConstraint(FlightConstraint* fli_constraint)
{
	this->flightconstraint = fli_constraint ;
}


void CFlightConstraintReadHandleByVersion::setAirlineAndFlightID(const char* p_char,int& ndx)
{
	char str[1024] = {0};
	int i  = 0;
	for (i=0; p_char[ndx] && p_char[ndx] != '-'; i++, ndx++)
		str[i] = p_char[ndx];
	str[i] = '\0';
	ndx = strlen( str ) ;
	if ( strlen( str ) == 0 || !strcmp (str, "DEFAULT"))
	{
		//airline[0] = '\0';
		flightconstraint->setAirline('\0');
		flightconstraint->setFlightID( '\0');
		return;
	}
	// copy airline
	// comment by kevin
	//for (int ndx = 0; isalpha (p_str[ndx]) && ndx < AIRLINE_LEN-1; ndx++)
	/*for (int ndx = 0; (isalpha (p_str[ndx]) || p_str[ndx] == '*' || ndx < 2 ) && ndx < AIRLINE_LEN-1; 
	ndx++)
	airline[ndx] = p_str[ndx];
	airline[ndx] = '\0';*/
	char szBuffer[AIRLINE_LEN];
	memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
	for ( i = 0; (isalpha (str[i]) || str[i] == '*' || i < 2 ) && i < AIRLINE_LEN-1; 
		i++)
		//airline[ndx] = p_str[ndx];
		szBuffer[i] = str[i];
	//airline[ndx] = '\0';
	szBuffer[i] = '\0';
	flightconstraint->setAirline(szBuffer);
	// copy flight ID number
	flightconstraint->setFlightID(str + i);
}
void CFlightConstraintReadHandleByVersion::setConstraint( const char *p_str, int _nConstraintVersion )
{
	if(flightconstraint == NULL)
		return ;
	flightconstraint->initDefault();
	if (!strcmp (p_str, "DEFAULT"))
		return;

	int i, ndx = 0;
	char str[1024]={0};

	setAirlineAndFlightID(p_str,ndx);


	for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
		str[i] = p_str[ndx];
	str[i] = '\0';
	switch( str[0] )
	{
	case 0:
		flightconstraint->SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ALL );
		break;
	case 'A':
		flightconstraint->SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ARR );
		break;
	case 'D':
		flightconstraint->SetFltConstraintMode( ENUM_FLTCNSTR_MODE_DEP );
		break;
	case 'U':
		flightconstraint->SetFltConstraintMode( ENUM_FLTCNSTR_MODE_THU );
		break;
	}


	if( _nConstraintVersion < 100 )
	{
		for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
			str[i] = p_str[ndx];
		str[i] = '\0';
		flightconstraint->setAirport( str );
	}
	else if( _nConstraintVersion < 101 )
	{
		/*
		if A
		do orig from

		if have orig
		orig ==> airport
		from ==> stopover
		else
		from ==> airport

		skip to, dest
		else
		skip orig from

		do to, dest

		if have dest
		to ==> stopover
		dest ==> airport
		else
		to ==> airport
		*/
		char str2[1024] = {0};
		if( flightconstraint->GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR )
		{
			// do orig from
			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str2[i] = p_str[ndx];
			str2[i] = '\0';

			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			if( strlen( str2 ) > 0 ) 
			{
				// have orig
				CString strOrig(str2);
				ReplaceSaftWordWithKeyWord(strOrig);
				flightconstraint->setAirport (strOrig.GetString());	// orig ==> airport

				CString strStopover(str);
				ReplaceSaftWordWithKeyWord(strStopover);
				flightconstraint->setStopoverAirport( strStopover.GetString() );	// from ==> stopover
			}
			else
			{
				CString strAiport(str);
				ReplaceSaftWordWithKeyWord(strAiport);
				flightconstraint->setAirport( strAiport.GetString() );	// from ==> airport
			}

			// skip to, dest
			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';
		}
		else
		{
			// skip orig, from
			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			// do to, dest
			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str2[i] = p_str[ndx];
			str2[i] = '\0';
			if( strlen( str2 ) > 0 )
			{
				// 	if have dest
				CString strDest(str2);
				ReplaceSaftWordWithKeyWord(strDest);
				flightconstraint->setAirport (strDest.GetString());	// dest ==> airport

				CString strStopover(str);
				ReplaceSaftWordWithKeyWord(strStopover);
				flightconstraint->setStopoverAirport( strStopover.GetString() );	// to ==> stopover
			}
			else
			{
				CString strAirport(str);
				ReplaceSaftWordWithKeyWord(strAirport);
				flightconstraint->setAirport( strAirport.GetString() );	// to ==> airport
			}
		}
	}
	else
	{
		for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
			str[i] = p_str[ndx];
		str[i] = '\0';
		CString strAirport(str);
		ReplaceSaftWordWithKeyWord(strAirport);
		flightconstraint->setAirport( strAirport.GetString() );

		for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
			str[i] = p_str[ndx];
		str[i] = '\0';
		CString strStopover(str);
		ReplaceSaftWordWithKeyWord(strStopover);
		flightconstraint->setStopoverAirport( strStopover.GetString() );
	}


	for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
		str[i] = p_str[ndx];
	str[i] = '\0';
	CString strActype(str);
	ReplaceSaftWordWithKeyWord(strActype);
	flightconstraint->setACType (strActype.GetString());

	for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
		str[i] = p_str[ndx];
	str[i] = '\0';
	CString strAirlineGroup(str);
	ReplaceSaftWordWithKeyWord(strAirlineGroup);
	flightconstraint->setAirlineGroup (strAirlineGroup.GetString());

	if( _nConstraintVersion >= VERSION_CONSTRAINT_CURRENT_102 )
	{
		for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
			str[i] = p_str[ndx];
		str[i] = '\0';
		CString strFlightGroup(str);
		ReplaceSaftWordWithKeyWord(strFlightGroup);
		flightconstraint->setFlightGroup(strFlightGroup.GetString());
	}

	if( _nConstraintVersion < 100 )
	{
		for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
			str[i] = p_str[ndx];
		str[i] = '\0';
		CString strSector(str);
		ReplaceSaftWordWithKeyWord(strSector);
		flightconstraint->setSector( strSector.GetString() );
	}
	else if( _nConstraintVersion < 101 )
	{

		/*
		if A
		do orig from

		if have orig
		orig ==> airport
		from ==> stopover
		else
		from ==> airport
		else
		do to, dest

		if have dest
		to ==> stopover
		dest ==> airport
		else
		to ==> airport
		*/
		char str2[1024] = {0};
		if( flightconstraint->GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR )
		{
			// do orig from
			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str2[i] = p_str[ndx];
			str2[i] = '\0';

			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			if( strlen( str2 ) > 0 ) 
			{
				// have orig
				CString strOrig(str2);
				ReplaceSaftWordWithKeyWord(strOrig);
				flightconstraint->setSector (strOrig.GetString());	// orig ==> sector

				CString strStopover(str);
				ReplaceSaftWordWithKeyWord(strStopover);
				flightconstraint->setStopoverSector( strStopover.GetString() );	// from ==> stopover
			}
			else
			{
				CString strSector(str);
				ReplaceSaftWordWithKeyWord(strSector);
				flightconstraint->setSector( strSector.GetString() );	// from ==> stopover
			}

			// skip to, dest
			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';
		}
		else
		{
			// skip orig, from
			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			// do to, dest
			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str[i] = p_str[ndx];
			str[i] = '\0';

			for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
				str2[i] = p_str[ndx];
			str2[i] = '\0';
			if( strlen( str2 ) > 0 )
			{
				// 	if have dest
				CString strDest(str2);
				ReplaceSaftWordWithKeyWord(strDest);
				flightconstraint->setSector (strDest.GetString());	// dest ==> airport

				CString strStopover(str);
				ReplaceSaftWordWithKeyWord(strStopover);
				flightconstraint->setStopoverSector( strStopover.GetString() );	// to ==> stopover
			}
			else
			{
				CString strAirport(str);
				ReplaceSaftWordWithKeyWord(strAirport);
				flightconstraint->setSector( strAirport.GetString() );	// to ==> airport
			}
		}
	}
	else
	{
		for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
			str[i] = p_str[ndx];
		str[i] = '\0';
		CString strSector(str);
		ReplaceSaftWordWithKeyWord(strSector);
		flightconstraint->setSector( strSector.GetString() );

		for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
			str[i] = p_str[ndx];
		str[i] = '\0';
		CString strStopoverSector(str);
		ReplaceSaftWordWithKeyWord(strStopoverSector);
		flightconstraint->setStopoverSector( strStopoverSector.GetString() );
	}

	for (i = 0, ndx++; p_str[ndx] && p_str[ndx] != '-'; i++, ndx++)
		str[i] = p_str[ndx];
	str[i] = '\0';
	CString strCategory(str);
	ReplaceSaftWordWithKeyWord(strCategory);
	flightconstraint->setCategory (strCategory.GetString());

}



CFlightConstraintReadHandleByVersion103::CFlightConstraintReadHandleByVersion103( FlightConstraint* fli_constraint):CFlightConstraintReadHandleByVersion(fli_constraint)
{

}
void CFlightConstraintReadHandleByVersion103::setAirlineAndFlightID(const char* p_char,int & ndx)
{
	char str[1024] ={0} ;
	int i =0  ;
	for (i = 0; p_char[ndx] && p_char[ndx] != '-'; i++, ndx++)
		str[i] = p_char[ndx];
	flightconstraint->setAirline(str) ;
    memset(str,0,sizeof(char)*1024);
	for (i = 0,++ndx; p_char[ndx] && p_char[ndx] != '-'; i++, ndx++)
		str[i] = p_char[ndx];
	flightconstraint->setFlightID(str) ;
}



