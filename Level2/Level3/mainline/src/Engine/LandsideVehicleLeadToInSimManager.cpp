#include "StdAfx.h"
#include ".\landsidevehicleleadtoinsimmanager.h"
#include "Landside\LandsideResidentRelatedContainer.h"
#include "Landside\LandsideLayoutObject.h"


LandsideVehicleDirection::LandsideVehicleDirection()
{

}

LandsideVehicleDirection::~LandsideVehicleDirection()
{

}

void LandsideVehicleDirection::Insert( const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType,const ALTObjectID& objName )
{
	m_paxType = _paxType;
	m_vehicleType = vehicleType;

	if(std::find(m_vCanGoToLandsideObject.begin(),m_vCanGoToLandsideObject.end(),objName) == m_vCanGoToLandsideObject.end())
	{
		m_vCanGoToLandsideObject.push_back(objName);
	}
}

bool LandsideVehicleDirection::CanLeadTo( const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType,const ALTObjectID& objName ) const
{
	if (!m_paxType.fits(_paxType))
		return false;
	
	if (!m_vehicleType.fitIn(vehicleType))
		return false;
	
	if(std::find(m_vCanGoToLandsideObject.begin(),m_vCanGoToLandsideObject.end(),objName) == m_vCanGoToLandsideObject.end())
		return false;
	
	return true;
}

bool LandsideVehicleDirection::operator==( const LandsideVehicleDirection& vehicleDir ) const
{
	if (m_paxType == vehicleDir.m_paxType && m_vehicleType == vehicleDir.m_vehicleType && m_vCanGoToLandsideObject == vehicleDir.m_vCanGoToLandsideObject)
	{
		return true;
	}
	return false;
}

bool LandsideVehicleDirection::operator<( const LandsideVehicleDirection& lsDirection ) const
{
	return m_paxType < lsDirection.m_paxType;
}

LandsideDirectionConstraint::LandsideDirectionConstraint( const ALTObjectID& objName )
{

}

LandsideDirectionConstraint::~LandsideDirectionConstraint()
{

}

void LandsideDirectionConstraint::Insert( const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType,const ALTObjectID& objName )
{
	LandsideVehicleDirection *pDirection = new LandsideVehicleDirection;
	pDirection->Insert(_paxType,vehicleType,objName);
	if (std::find(m_vDirection.begin(),m_vDirection.end(),pDirection) != m_vDirection.end())
	{
		m_vDirection.addItem(pDirection);
	}
}

bool LandsideDirectionConstraint::CanLeadTo( const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType, const ALTObjectID& objName,const ALTObjectID& objDest )const
{
	if (m_vDirection.empty())
		return true;
	
	if (objName == m_landsideObject)
	{
		for (size_t i = 0; i < m_vDirection.size(); i++)
		{
			const LandsideVehicleDirection* pDirection = m_vDirection.at(i);
			if (pDirection->CanLeadTo(_paxType,vehicleType,objDest))
			{
				return true;
			}
		}
	}
	
	return false;
}

bool LandsideDirectionConstraint::operator==( const LandsideDirectionConstraint& dirCon ) const
{
	bool bEqual = m_landsideObject == dirCon.m_landsideObject ? true : false;

	bEqual = ((m_vDirection == dirCon.m_vDirection) && bEqual) ? true : false;
	
	return bEqual;
}

LandsideVehicleLeadToInSimManager::LandsideVehicleLeadToInSimManager( void )
{

}

LandsideVehicleLeadToInSimManager::~LandsideVehicleLeadToInSimManager( void )
{

}

void LandsideVehicleLeadToInSimManager::Inilization( NonResidentVehicleTypeContainer *pNonResidentFlow, const LandsideFacilityLayoutObjectList& layoutObjectList )
{
	int nCount =  pNonResidentFlow->GetItemCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		NonResidentVehicleTypeData *pVehicleTypeData = pNonResidentFlow->GetItem(nItem);
		if (pVehicleTypeData == NULL)
			continue;

		ProcessPaxTypeData(pVehicleTypeData, layoutObjectList);

	}
}

void LandsideVehicleLeadToInSimManager::ProcessPaxTypeData(NonResidentVehicleTypeData *pVehicleTypeData, const LandsideFacilityLayoutObjectList& layoutObjectList)
{
	CMobileElemConstraint& paxType = pVehicleTypeData->getPaxType();
	CHierachyName vehicleType;
	vehicleType.fromString(pVehicleTypeData->GetVehicleType());

	int nTimeCount = pVehicleTypeData->GetCount();
	for (int nTime = 0; nTime < nTimeCount; ++ nTime)
	{
		NonResidentVehicleTypePlan* pTimePlan = pVehicleTypeData->GetItem(nTime);
		ProcessTimeData(pTimePlan,vehicleType,paxType,layoutObjectList);
	}
}

void LandsideVehicleLeadToInSimManager::ProcessTimeData(NonResidentVehicleTypePlan *pOrginalTimePlan,const CHierachyName& vehicleType,const CMobileElemConstraint& _paxType, const LandsideFacilityLayoutObjectList& layoutObjectList)
{
	ResidentVehicleRouteFlow* pVehicleRouteFlow = pOrginalTimePlan->GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return;
	
	//int nRouteCount =  pOrginalTimePlan->GetCount();
	ResidentVehicleRouteList* pStartRouteList = pVehicleRouteFlow->GetStartRoute();
	if (pStartRouteList == NULL)
		return;
	
	int nRouteCount = pStartRouteList->GetDestCount();
	std::vector<LandsideFacilityObject> vFacilityObject;
	for (int nRoute = 0; nRoute < nRouteCount; ++ nRoute)
	{
		ResidentVehicleRoute* pRoute = pStartRouteList->GetDestObjectInfo(nRoute);
		ProcessRouteData(pRoute,pVehicleRouteFlow,vehicleType,vFacilityObject,_paxType,layoutObjectList);
	}

}

void LandsideVehicleLeadToInSimManager::ProcessRouteData(ResidentVehicleRoute *pOriginalRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow,const CHierachyName& vehicleType,
														 std::vector<LandsideFacilityObject> vFacilityObject,
														 const CMobileElemConstraint& _paxType,const LandsideFacilityLayoutObjectList& layoutObjectList)
{

	//the route item itself
	if(pOriginalRoute == NULL)
		return;
	LandsideFacilityObject *pObject = pOriginalRoute->GetFacilityObject();
	if(pObject == NULL)
		return;
	
	//check whether has circle
	if (std::find(vFacilityObject.begin(),vFacilityObject.end(),*pObject) != vFacilityObject.end())
		return;

	//its children
	ResidentVehicleRouteList* pChildeRouteList = pVehicleRouteFlow->GetDestVehicleRoute(*pOriginalRoute->GetFacilityObject());
	if (pChildeRouteList == NULL)
		return;	

	int nItemCount = pChildeRouteList->GetDestCount();

	vFacilityObject.push_back(*pObject);	
	if (nItemCount == 0)
	{
		int nCount = (int)vFacilityObject.size();
		for(int i = nCount - 1; i >= 0; i--)
		{
			LandsideFacilityObject pFacility = vFacilityObject[i];
			LandsideFacilityObject::FacilityType enumType = pFacility.GetType();
			if (enumType == LandsideFacilityObject::Landside_Curbside_Facility
				|| enumType == LandsideFacilityObject::Landside_TaxiQueue_Facility)
			{
				int nFacilityID = pFacility.GetFacilityID();
				if (nFacilityID >= 0)
				{
					const LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByID(nFacilityID);
					//ASSERT(pLandsideObject);
					if(pLandsideObject)
					{
						for(int j = i; j >= 0; j--)
						{
							LandsideFacilityObject pNextFacility = vFacilityObject[j];
							int nNextFacilityID = pNextFacility.GetFacilityID();
							if (nNextFacilityID >= 0)
							{
								const LandsideFacilityLayoutObject* pNextObject = layoutObjectList.getObjectByID(nNextFacilityID);
								//ASSERT(pNextObject);
								if(pNextObject)
									Insert(_paxType,vehicleType,pLandsideObject->getName(),pNextObject->getName());
							}
						}
					}
				}
			}
		}
	}

	for ( int nItem = 0; nItem < nItemCount; ++ nItem)
	{
		ResidentVehicleRoute* pItem = pChildeRouteList->GetDestObjectInfo(nItem);
		if(pItem == NULL)
			continue;
		ProcessRouteData(pItem,pVehicleRouteFlow,vehicleType,vFacilityObject,_paxType,layoutObjectList);
	}
	vFacilityObject.pop_back();
}

bool LandsideVehicleLeadToInSimManager::CanLeadTo( const CMobileElemConstraint& _paxType,const CHierachyName& vehicleType, const ALTObjectID& objName,const ALTObjectID& objDest )const
{
	if (m_vLeadTo.empty())
		return true;
	
	for (size_t i = 0 ; i < m_vLeadTo.size(); i++)
	{
		const LandsideDirectionConstraint& leadTo = m_vLeadTo.at(i);
		if (leadTo.CanLeadTo(_paxType,vehicleType,objName,objDest))
		{
			return true;
		}
	}
	return false;
}


void LandsideVehicleLeadToInSimManager::Insert( const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType,const ALTObjectID& srcName,const ALTObjectID& objName )
{
	LandsideDirectionConstraint leadTo(srcName);
	leadTo.Insert(_paxType,vehicleType,objName);

	if (std::find(m_vLeadTo.begin(),m_vLeadTo.end(),leadTo) != m_vLeadTo.end())
	{
		m_vLeadTo.push_back(leadTo);
	}
}
