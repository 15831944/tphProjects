#pragma once

#include "../../InputAirside/TaxiwayConstraint.h"
#include "../../InputAirside/FlightTypeRestrictionlist.h"
#include "../../InputAirside/DirectionalityConstraints.h"
#include "../../InputAirside/SimSettingClosure.h"
#include "../../InputAirside/TaxiSpeedConstraints.h"


typedef CTaxiwayConstraintList WeightWingspanConstraints;

class TaxiwayConstraintsManager
{
public:
	TaxiwayConstraintsManager(void);
	virtual ~TaxiwayConstraintsManager(void);

public:
	
	void ReadData(int nProjID, int nAirportID, CAirportDatabase* pAirportDB);

public:
	WeightWingspanConstraints		m_weightWinspanCons;
	CFlightTypeRestrictionList		m_flightTypeRestrictions;
	DirectionalityConstraints		m_directionalityCons;
	CAirsideSimSettingClosure		*m_simSettingClosure;
	TaxiSpeedConstraints            m_taxiSpeedConstraints;
};
