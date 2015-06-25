//////////////////////////////////////////////////////////////////////////
//	ID Gather
//	airlineid, flightid, airportcode, airportcodetype
//	Author: Kevin Chang
//	Last Modified: 2005-3-29 11:31
//	Copyright (c) ARC Inc., 2005
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "template.h"

//	AirlineID
class CAirlineID
{
public:
	CAirlineID();
	~CAirlineID();

private:
	char	m_psz[AIRLINE_LEN];

public:
	const char* GetValue() const {return m_psz;}
	BOOL HasValue() const{return (strlen(m_psz) ? TRUE : FALSE);}
	operator LPCTSTR() const{return m_psz;}

	// remove the "*" and return for the purpose of roster...
	CString GetNonStarValue() const;

public:
	const CAirlineID& operator=(LPCTSTR lpstr);
	const CAirlineID& operator=(const CAirlineID& id);
	
	BOOL operator==(LPCTSTR lpstr) const;
	BOOL operator==(const CAirlineID& id) const;

	BOOL operator!=(LPCTSTR lpstr) const;
	BOOL operator!=(const CAirlineID& id) const;
};


//	FlightID
class CFlightID
{
public:
	CFlightID();
	~CFlightID();
	
private:
	char	m_psz[FLIGHT_LEN];

public:
	const char* GetValue() const {return m_psz;}
	BOOL HasValue() const;
	operator LPCTSTR() const{return m_psz;}
	
public:
	const CFlightID& operator=(LPCTSTR lpstr);
	const CFlightID& operator=(const CFlightID& id);

	BOOL operator==(LPCTSTR lpstr) const;
	BOOL operator==(const CFlightID& id) const;

	BOOL operator!=(LPCTSTR lpstr) const;
	BOOL operator!=(const CFlightID& id) const;

	BOOL operator<(LPCTSTR lpstr) const;
	BOOL operator<(const CFlightID& id) const;
};


//	AirportCode
class CAirportCode
{
public:
	CAirportCode();
	~CAirportCode();

private:
	char	m_psz[AIRPORT_LEN];

public:
	const char* GetValue() const {return m_psz;}
	BOOL HasValue() const{return (strlen(m_psz) ? TRUE : FALSE);}
	operator LPCTSTR() const{return m_psz;}

//	Operators
public:
	const CAirportCode& operator=(LPCTSTR lpstr);
	const CAirportCode& operator=(const CAirportCode& id);

	BOOL operator==(LPCTSTR lpstr) const;
	BOOL operator==(const CAirportCode& id) const;

	BOOL operator!=(LPCTSTR lpstr) const;
	BOOL operator!=(const CAirportCode& id) const;
};


//	AirportCodeType
class CAircraftType
{
public:
	CAircraftType();
	~CAircraftType();

private:
	char	m_psz[AC_TYPE_LEN];

public:
	const char* GetValue() const {return m_psz;}
	BOOL HasValue() const{return (strlen(m_psz) ? TRUE : FALSE);}
	operator LPCTSTR() const{return m_psz;}

public:
	const CAircraftType& operator=(LPCTSTR lpstr);
	const CAircraftType& operator=(const CAircraftType& id);

	BOOL operator==(LPCTSTR lpstr) const;
	BOOL operator==(const CAircraftType& id) const;

	BOOL operator!=(LPCTSTR lpstr) const;
	BOOL operator!=(const CAircraftType& id) const;
};