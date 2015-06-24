#include "StdAfx.h"
#include "TaxiwayConstraintsManager.h"

TaxiwayConstraintsManager::TaxiwayConstraintsManager(void)
{
	m_simSettingClosure = NULL;
}

TaxiwayConstraintsManager::~TaxiwayConstraintsManager(void)
{
	delete m_simSettingClosure;
}


void TaxiwayConstraintsManager::ReadData(int nProjID, int nAirportID, CAirportDatabase* pAirportDB)
{
	m_weightWinspanCons.ReadWeightWingspanConsData(nProjID);
	m_flightTypeRestrictions.SetAirportDatabase(pAirportDB);
	m_flightTypeRestrictions.ReadData(nProjID);
	m_directionalityCons.ReadData(nAirportID);

	if (m_simSettingClosure)
		delete m_simSettingClosure;

	m_simSettingClosure = new CAirsideSimSettingClosure(nProjID);
	m_simSettingClosure->ReadData();

	m_taxiSpeedConstraints.ReadData(nProjID, pAirportDB);
}