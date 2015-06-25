//////////////////////////////////////////////////////////////////////////
//	IDGather.cpp
//	Author: Kevin Chang
//	Last Modified: 
//	Copyright (c) ARC Inc., 2005
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IDGather.h"

//////////////////////////////////////////////////////////////////////////
//	CAirlineID

CAirlineID::CAirlineID()
{
	memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
}

CAirlineID::~CAirlineID()
{
}


const CAirlineID& CAirlineID::operator=(LPCTSTR lpstr)
{
	if (lpstr == NULL)
		memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
	else
		strcpy(m_psz, lpstr);
	return (*this);
}

const CAirlineID& CAirlineID::operator=(const CAirlineID& id)
{
	if (id.m_psz == NULL)
		memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
	else
		strcpy(m_psz, id.m_psz);
	return (*this);
}

BOOL CAirlineID::operator==(LPCTSTR lpstr) const
{
	return (strcmp(m_psz, lpstr) == 0);
}

BOOL CAirlineID::operator==(const CAirlineID& id) const
{
	return (strcmp(m_psz, id.m_psz) == 0);
}

BOOL CAirlineID::operator!=(LPCTSTR lpstr) const
{
	return (strcmp(m_psz, lpstr));
}

BOOL CAirlineID::operator!=(const CAirlineID& id) const
{
	return (strcmp(m_psz, id.m_psz));
}


// remove the "*" and return for the purpose of roster...
CString CAirlineID::GetNonStarValue() const
{
	CString str;
	str = m_psz;
	str.Remove( '*' );
	return str;
}



//////////////////////////////////////////////////////////////////////////
//	CFlightID

CFlightID::CFlightID()
{
	memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
}

CFlightID::~CFlightID()
{
}

BOOL CFlightID::HasValue() const
{
	return strlen(m_psz) > 0;
}


const CFlightID& CFlightID::operator=(LPCTSTR lpstr)
{
	if (lpstr == NULL)
		memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
	else
		strcpy(m_psz, lpstr);
	return (*this);
}

const CFlightID& CFlightID::operator=(const CFlightID& id)
{
	if (id.m_psz == NULL)
		memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
	else
		strcpy(m_psz, id.m_psz);
	return (*this);
}

BOOL CFlightID::operator==(LPCTSTR lpstr) const
{
	return (strcmp(m_psz, lpstr) == 0);
}

BOOL CFlightID::operator==(const CFlightID& id) const
{
	return (strcmp(m_psz, id.m_psz) == 0);
}

BOOL CFlightID::operator!=(LPCTSTR lpstr) const
{
	return (strcmp(m_psz, lpstr));
}

BOOL CFlightID::operator!=(const CFlightID& id) const
{
	return (strcmp(m_psz, id.m_psz));
}

BOOL CFlightID::operator<(LPCTSTR lpstr) const
{
	return (strcmp(m_psz, lpstr) < 0 ? TRUE : FALSE);
}

BOOL CFlightID::operator<(const CFlightID& id) const
{
	return (strcmp(m_psz, id.m_psz) < 0 ? TRUE : FALSE);
}



//////////////////////////////////////////////////////////////////////////
//	CAirportCode

CAirportCode::CAirportCode()
{
	memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
}

CAirportCode::~CAirportCode()
{
}

const CAirportCode& CAirportCode::operator=(LPCTSTR lpstr)
{
	if (lpstr == NULL)
		memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
	else
		strcpy(m_psz, lpstr);
	return (*this);
}

const CAirportCode& CAirportCode::operator=(const CAirportCode& id)
{
	if (id.m_psz == NULL)
		memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
	else
		strcpy(m_psz, id.m_psz);
	return (*this);
}

BOOL CAirportCode::operator==(LPCTSTR lpstr) const
{
	return (strcmp(m_psz, lpstr) == 0);
}

BOOL CAirportCode::operator==(const CAirportCode& id) const
{
	return (strcmp(m_psz, id.m_psz) == 0);
}

BOOL CAirportCode::operator!=(LPCTSTR lpstr) const
{
	return (strcmp(m_psz, lpstr));
}

BOOL CAirportCode::operator!=(const CAirportCode& id) const
{
	return (strcmp(m_psz, id.m_psz));
}



//////////////////////////////////////////////////////////////////////////
//	CAircraftType

CAircraftType::CAircraftType()
{
	memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
}

CAircraftType::~CAircraftType()
{
}

const CAircraftType& CAircraftType::operator=(LPCTSTR lpstr)
{
	if (lpstr == NULL)
		memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
	else
		strcpy(m_psz, lpstr);
	return (*this);
}

const CAircraftType& CAircraftType::operator=(const CAircraftType& id)
{
	if (id.m_psz == NULL)
		memset(m_psz, 0, sizeof(m_psz) / sizeof(char));
	else
		strcpy(m_psz, id.m_psz);
	return (*this);
}

BOOL CAircraftType::operator==(LPCTSTR lpstr) const
{
	return (strcmp(m_psz, lpstr) == 0);
}

BOOL CAircraftType::operator==(const CAircraftType& id) const
{
	return (strcmp(m_psz, id.m_psz)== 0);
}

BOOL CAircraftType::operator!=(LPCTSTR lpstr) const
{
	return (strcmp(m_psz, lpstr));
}

BOOL CAircraftType::operator!=(const CAircraftType& id) const
{
	return (strcmp(m_psz, id.m_psz));
}