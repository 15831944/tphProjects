#include "StdAfx.h"
#include ".\airsidecontrollerworkloadparam.h"


const CString CAirsideControllerWorkloadParam::weightName[] = 
{
	_T("Push backs"),
		_T("Taxi start"),
		_T("Take off(no onflict)"),
		_T("Take off(conflict)"),
		_T("Landing(no conflict)"),
		_T("Landing(conflict)"),
		_T("Go arround"),
		_T("Vacate runway"),
		_T("Cross active"),
		_T("Handoff ground"),
		_T("Handoff air"),
		_T("Altitude change"),
		_T("Vector air"),
		_T("Holding"),
		_T("Direct route(air)"),
		_T("Reroute ground"),
		_T("Contribution"),
		_T("Extended downwind"),



		//add new before this line
		_T("Count")

};
CAirsideControllerWorkloadParam::CAirsideControllerWorkloadParam(void)
{
	m_nSubType = -1;//total index
}

CAirsideControllerWorkloadParam::~CAirsideControllerWorkloadParam(void)
{
}

void CAirsideControllerWorkloadParam::LoadParameter()
{

}

void CAirsideControllerWorkloadParam::UpdateParameter()
{

}

std::vector<ALTObjectID > CAirsideControllerWorkloadParam::getSector() const
{
	return m_vSector;
}

ALTObjectID CAirsideControllerWorkloadParam::getSector( int nIndex ) const
{
	ASSERT(nIndex >= 0 && nIndex < getSectorCout());
	if(nIndex >= 0 && nIndex < getSectorCout())
		return m_vSector.at(nIndex);

	return ALTObjectID();
}

std::vector<ALTObjectID > CAirsideControllerWorkloadParam::getArea() const
{
	return m_vArea;
}

ALTObjectID CAirsideControllerWorkloadParam::getArea( int nIndex ) const
{
	ASSERT(nIndex >= 0 && nIndex < getAreaCout());
	if(nIndex >= 0 && nIndex < getAreaCout())
		return m_vArea.at(nIndex);

	return ALTObjectID();
}

int CAirsideControllerWorkloadParam::getSectorCout() const
{
	return static_cast<int>(m_vSector.size());
}

void CAirsideControllerWorkloadParam::AddSector( const ALTObjectID& strSector )
{
	m_vSector.push_back(strSector);
}

int CAirsideControllerWorkloadParam::getAreaCout() const
{
	return static_cast<int>(m_vArea.size());
}

void CAirsideControllerWorkloadParam::AddArea( const ALTObjectID& strArea )
{
	m_vArea.push_back(strArea);
}

void CAirsideControllerWorkloadParam::Clear()
{
	m_vSector.clear();
	m_vArea.clear();
}

CString CAirsideControllerWorkloadParam::getWeightName( enumWeight weight ) const
{
	if (weight >= weight_PushBacks && weight < weight_count)
	{
		return weightName[(int)weight];
	}

	return CString();
}