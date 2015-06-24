#pragma once

#include "../../InputAirside/AircraftClassifications.h"
#include "../../Common/ACTypesManager.h"

//return actype classifications
class AircraftClassificationManager
{
public:
	typedef std::map<CString, AircraftClassificationItem*> ClassificationMap;
public:
	AircraftClassificationItem* GetWingSpanClass(const CString& actype);
	AircraftClassificationItem* GetWakeWotexClass(const CString & actype);
	AircraftClassificationItem* GetApproachSpeedClass(double dSpeed);
	AircraftClassificationItem* GetSurfaceBearingClass(const CString& actype);
	AircraftClassificationItem* GetNonebasedClass(){ return &m_NoneBasedClassfication; }


	void Init(CACTypesManager* pACTypeMan, int prjID);

	AircraftClassificationManager();

	//Called the GetACClassItem() function but not catch the exception , not include approach speed based type
	AircraftClassificationItem* GetAircraftClass(const CString& actype, FlightClassificationBasisType basisType);
	
	CACType* getTypeByName(CString strName);
	AircraftClassifications* GetACClassification(FlightClassificationBasisType type);
protected:
	CACTypesManager * m_pACTypeManager;
	AircraftClassifications m_vWingSpanClassifications;
	AircraftClassifications m_vWakeWotexClassifications;
	AircraftClassifications m_vApproachSpeedClassifications;
	AircraftClassifications m_vSurfaceBearingClassifications;
	AircraftClassificationItem m_NoneBasedClassfication;
	//AircraftClassifications m_vNoneBaseClassification;

	ClassificationMap m_mWingSpanClassMap;
	ClassificationMap m_mWakeWotexClassMap;
	//ClassificationMap m_mApproachSpeedClassMap;
	ClassificationMap m_mSurfaceBearingClassMap;

	


};
