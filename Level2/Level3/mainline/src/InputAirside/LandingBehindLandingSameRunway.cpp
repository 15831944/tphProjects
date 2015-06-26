#include "stdafx.h"
#include "LandingBehindLandingSameRunway.h"
/*
int m_nID;
int m_nProjID;
FlightTypeStyle m_emTrail;
FlightTypeStyle m_emLead;
FlightClassificationBasisType    m_emClassifiBasis;
int				m_nMinSeparation;
*/

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//LandingBehindLandingSameRunway
LandingBehindLandingSameRunway::LandingBehindLandingSameRunway(int nProjID)
:AircraftClassificationBasisApproachSeparationItem(nProjID,LandingBehindLandingOnSameRunway)
{
}

LandingBehindLandingSameRunway::~LandingBehindLandingSameRunway(void)
{
}