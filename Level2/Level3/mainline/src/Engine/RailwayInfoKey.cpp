// RailwayInfoKey.cpp: implementation of the RailwayInfoKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RailwayInfoKey.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RailwayInfoKey::RailwayInfoKey()
{

}

RailwayInfoKey::~RailwayInfoKey()
{

}
bool RailwayInfoKey::operator == ( const RailwayInfoKey& _another) const 
{
	if( m_iFirstStationIndex == _another.m_iFirstStationIndex 
		&& m_iSecondStationIndex == _another.m_iSecondStationIndex 
		&& m_iTypeFromFirstViewPoint == _another.m_iTypeFromFirstViewPoint )

		return true;
	else
		return false;
}