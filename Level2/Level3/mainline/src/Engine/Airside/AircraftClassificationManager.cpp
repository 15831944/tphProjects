#include "StdAfx.h"
#include "../../Common/EngineDiagnoseEx.h"
#include "../../Common/ARCExceptionEx.h"
#include "./AircraftClassificationManager.h"
#include "common/CARCUnit.h"

void AircraftClassificationManager::Init( CACTypesManager* pACTypeMan, int prjID )
{
	m_pACTypeManager = pACTypeMan;
	m_vWingSpanClassifications.SetProjectID(prjID);
	m_vWingSpanClassifications.ReadData();
	m_vSurfaceBearingClassifications.SetProjectID(prjID);
	m_vSurfaceBearingClassifications.ReadData();
	m_vApproachSpeedClassifications.SetProjectID(prjID);
	m_vApproachSpeedClassifications.ReadData();
	m_vWakeWotexClassifications.SetProjectID(prjID);
	m_vWakeWotexClassifications.ReadData();	
}

AircraftClassificationManager::AircraftClassificationManager()
{
	m_pACTypeManager = NULL;
	m_vWingSpanClassifications.SetBasisType(WingspanBased);
	m_vSurfaceBearingClassifications.SetBasisType(SurfaceBearingWeightBased);
	m_vApproachSpeedClassifications.SetBasisType(ApproachSpeedBased);
	m_vWakeWotexClassifications.SetBasisType(WakeVortexWightBased);	
}

static bool IsACTypeInMap(const CString& actype, const AircraftClassificationManager::ClassificationMap& map)
{
	return map.end() != map.find(actype); 
}


//Called the GetACClassItem() function but not catch the exception
AircraftClassificationItem* AircraftClassificationManager::GetAircraftClass( const CString& actype, FlightClassificationBasisType basisType)
{
	
	switch(basisType)
	{
	case WingspanBased :
		return GetWingSpanClass(actype);
		break;
	case WakeVortexWightBased :
		return GetWakeWotexClass(actype);
		break;
	case ApproachSpeedBased :
		ASSERT(FALSE);
		TRACE("\r\n call this function: AircraftClassificationItem* AircraftClassificationManager::GetApproachSpeedClass( double dSpeed) \r\n");
		//return GetApproachSpeedClass(dApproachSpeed);
		break;
	case SurfaceBearingWeightBased :
		return GetSurfaceBearingClass(actype);
		break;	
	default :
		//ASSERT(false);
		return GetWakeWotexClass(actype);
	}

	return GetNonebasedClass();
}

AircraftClassificationItem* AircraftClassificationManager::GetWingSpanClass( const CString& actype )
{
	if(IsACTypeInMap(actype,m_mWingSpanClassMap)) //actype in map
	{
		return m_mWingSpanClassMap[actype];
	}
			
	CACType * pACtype = getTypeByName(actype);
	if(pACtype)
	{
		double classVal = pACtype->m_fSpan;
		AircraftClassificationItem * pFitItem = m_vWingSpanClassifications.GetFitInItem(classVal);
		if(pFitItem)
		{
			m_mWingSpanClassMap[actype] = pFitItem;
			return pFitItem;
		}
	}
	
	CString strTime = "";
	CString strID = "";
	AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);		
	CString strDetals;
	strDetals.Format(_T("There is no approprate aircraft classification definition for the aircraft type.Aircraft Type: %s "),actype);
	pNewDiagnose->SetDetails(strDetals);
	throw new AirsideACTypeMatchError(pNewDiagnose);
	return NULL;
}

AircraftClassificationItem* AircraftClassificationManager::GetWakeWotexClass( const CString & actype )
{
	if(IsACTypeInMap(actype,m_mWakeWotexClassMap)) //actype in map
	{
		return m_mWakeWotexClassMap[actype];
	}

	CACType * pACtype = getTypeByName(actype);
	if(pACtype)
	{
		double classVal = CARCWeightUnit::ConvertWeight(AU_WEIGHT_KILOGRAM,DEFAULT_DATABASE_WEIGHT_UNIT,pACtype->m_fMTOW);
		//double classVal = pACtype->m_fMTOW;
		AircraftClassificationItem * pFitItem = m_vWakeWotexClassifications.GetFitInItem(classVal);
		if(pFitItem)
		{
			m_mWakeWotexClassMap[actype] = pFitItem;
			return pFitItem;
		}
	}
	CString strTime = "";
	CString strID = "";
	AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);		
	CString strDetals;
	strDetals.Format(_T("There is no approprate aircraft classification definition for the aircraft type.Aircraft Type: %s "),actype);
	pNewDiagnose->SetDetails(strDetals);
	throw new AirsideACTypeMatchError(pNewDiagnose);
	return NULL;	
}

AircraftClassificationItem* AircraftClassificationManager::GetApproachSpeedClass( double dSpeedInMpS)
{	
	AircraftClassificationItem * pFitItem = m_vApproachSpeedClassifications.GetFitInItem(dSpeedInMpS);
	if(pFitItem)
	{		
		return pFitItem;
	}	
	return NULL;
}

AircraftClassificationItem* AircraftClassificationManager::GetSurfaceBearingClass( const CString& actype )
{
	if(IsACTypeInMap(actype,m_mSurfaceBearingClassMap)) //actype in map
	{
		return m_mSurfaceBearingClassMap[actype];
	}
	
	CACType * pACtype = getTypeByName(actype);
	if(pACtype)
	{
		double classVal =  CARCWeightUnit::ConvertWeight(AU_WEIGHT_KILOGRAM,DEFAULT_DATABASE_WEIGHT_UNIT,pACtype->m_fMLW);
		//double classVal = pACtype->m_fMLW;
		AircraftClassificationItem * pFitItem = m_vSurfaceBearingClassifications.GetFitInItem(classVal);
		if(pFitItem)
		{
			m_mSurfaceBearingClassMap[actype] = pFitItem;
			return pFitItem;
		}
	}
	CString strTime = "";
	CString strID = "";
	AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);		
	CString strDetals;
	strDetals.Format(_T("There is no approprate aircraft classification definition for the aircraft type.Aircraft Type: %s "),actype);
	pNewDiagnose->SetDetails(strDetals);
	throw new AirsideACTypeMatchError(pNewDiagnose);
	return NULL;	//return GetACClassItem(actype,m_pACTypeManager,m_mSurfaceBearingClassMap,m_vSurfaceBearingClassifications);	
}

CACType* AircraftClassificationManager::getTypeByName( CString strName )
{
	if(m_pACTypeManager)
	{
		int typeidx = m_pACTypeManager->FindOrAddEntry(strName);
		if(typeidx >=0 )
		{
			CACType * pACtype = m_pACTypeManager->getACTypeItem(typeidx);
			return pACtype;		
		}
	}
	return NULL;
}
AircraftClassifications* AircraftClassificationManager::GetACClassification( FlightClassificationBasisType type )
{
	if(type == WingspanBased)
	{
		return &m_vWingSpanClassifications;
	}

	if(type ==SurfaceBearingWeightBased)
	{
		return &m_vSurfaceBearingClassifications;
	}

	if(type ==ApproachSpeedBased)
	{
		return &m_vApproachSpeedClassifications;
	}

	if(type ==WakeVortexWightBased)
	{
		return &m_vWakeWotexClassifications;
	}

	return NULL;
}






















