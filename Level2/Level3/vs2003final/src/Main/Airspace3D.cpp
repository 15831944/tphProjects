#include "StdAfx.h"
#include ".\airspace3d.h"
#include ".\3dView.h"
#include ".\..\InputAirside\ALTAirspace.h"
#include "WayPoint3D.h"
#include "Hold3D.h"
#include "Sector3D.h"
#include "AirRoute3D.h"
#include "SelectableSettings.h"

CAirspace3D::CAirspace3D(int id ,CAirside3D * pAirside):m_nID(id),m_pAirside(pAirside)
{
	Init();
}

CAirspace3D::~CAirspace3D(void)
{
}

void CAirspace3D::Init()
{
	
	
	std::vector<int> objectIDs;
	// get waypoint Object list
	objectIDs.clear();
	ALTAirspace::GetWaypointList(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){
		CWayPoint3D * pWaypoint3D = new CWayPoint3D(objectIDs[i]);
		m_vWayPoint.push_back(pWaypoint3D);
		pWaypoint3D->Update(m_pAirside);
	}

	//get Hold
	objectIDs.clear();
	ALTAirspace::GetHoldList(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){
		CHold3D  * pHold3D = new CHold3D(objectIDs[i]);
		m_vHold.push_back(pHold3D);
		pHold3D->Update(m_pAirside);
	}

	//get Sectors
	objectIDs.clear();
	ALTAirspace::GetSectorList(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){
		CAirSector3D * pSector3D = new CAirSector3D(objectIDs[i]);
		m_vSector.push_back(pSector3D);
		pSector3D->Update();
	}

	// get Air routes
	objectIDs.clear();
	ALTAirspace::GetAirRouteList(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){
		CAirRoute3D * pAirRoute3D = new CAirRoute3D(objectIDs[i]);
		m_vAirRoute.push_back(pAirRoute3D);
		pAirRoute3D->Update(m_pAirside);
	}
}

ALTObject3D * CAirspace3D::GetObject3D( int id )
{
	for(size_t i=0;i<m_vWayPoint.size();++i){
		if(m_vWayPoint[i]->GetID() == id )return m_vWayPoint[i].get();
    }

	for(size_t i=0;i<m_vSector.size();++i){
		if(m_vSector[i]->GetID() == id) return m_vSector[i].get();
	}

	for(size_t i=0;i<m_vHold.size();++i){
		if(m_vHold[i]->GetID() == id ) return m_vHold[i].get();
	}

	for(size_t i=0;i<m_vAirRoute.size(); ++i){
		if(m_vAirRoute[i]->GetID() == id ) return m_vAirRoute[i].get();
	}

	return NULL;
}

void CAirspace3D::DrawOGL( C3DView * pView )
{
	//draw waypoints
	for(size_t i=0;i<m_vWayPoint.size();++i){
		m_vWayPoint[i]->DrawOGL(pView);
	}
//draw sector
	
	for(size_t i=0;i<m_vSector.size();++i){
		m_vSector[i]->DrawOGL(pView);
	}
//draw hold
	for(size_t i=0;i<m_vHold.size();++i){
		m_vHold[i]->DrawOGL(pView);
	}
//draw airroute
	for(size_t i=0;i<m_vAirRoute.size(); ++i){
		m_vAirRoute[i]->DrawOGL(pView);
	}	

	
	static char g_pszErrorGL[256];

	GLenum errorGL;
		while((errorGL = glGetError()) != GL_NO_ERROR) { 
			sprintf(g_pszErrorGL, "OPENGL ERRORS in %s [%s]\n", (""), gluErrorString(errorGL)); 
			::OutputDebugString(g_pszErrorGL); 
		} 
}

void CAirspace3D::DrawSelectOGL( C3DView *pView ,SelectableSettings& selsetting  )
{
	
	if( selsetting.m_ALTobjectSelectableMap[ALT_WAYPOINT] )
	{
		for(int i=m_vWayPoint.size()-1;i>=0;--i){
			glLoadName( pView->GetSelectionMap().NewSelectable(m_vWayPoint[i].get())  );
			m_vWayPoint[i]->DrawSelect(pView);
		}
	}

	if( selsetting.m_ALTobjectSelectableMap[ALT_SECTOR] )
	{
		for(int i=m_vSector.size()-1;i>=0;--i){
			glLoadName( pView->GetSelectionMap().NewSelectable(m_vSector[i].get()) );
			m_vSector[i]->DrawSelect(pView);
		}
	}
	
	if( selsetting.m_ALTobjectSelectableMap[ALT_HOLD] )
	{
		for(int i=m_vHold.size()-1;i>=0;--i){
			glLoadName( pView->GetSelectionMap().NewSelectable(m_vHold[i].get()) );
			m_vHold[i]->DrawSelect(pView);
		}
	}

	if( selsetting.m_ALTobjectSelectableMap[ALT_AIRROUTE] )
	{
		for(int i=m_vAirRoute.size()-1;i>=0; --i){
			glLoadName( pView->GetSelectionMap().NewSelectable(m_vAirRoute[i].get()) );
			m_vAirRoute[i]->DrawSelect(pView);
		}
	}
	
}

bool CAirspace3D::RemoveObject( int id )
{
	//find from the waypoint
	for(ALTObject3DList::iterator itr = m_vWayPoint.begin(); itr!=m_vWayPoint.end();++itr){
		if ( (*itr)->GetID() == id ) { m_vWayPoint.erase(itr); return true; }
	}
	//find from the hold
	for(ALTObject3DList::iterator itr = m_vHold.begin(); itr!=m_vHold.end();++itr){
		if ( (*itr)->GetID() == id ) { m_vHold.erase(itr); return true; }
	}
	//find from the sector
	for(ALTObject3DList::iterator itr = m_vSector.begin(); itr!=m_vSector.end();++itr){
		if ( (*itr)->GetID() == id ) { m_vSector.erase(itr); return true; }
	}
	//find from the waypoint
	for(ALTObject3DList::iterator itr = m_vAirRoute.begin(); itr!=m_vAirRoute.end();++itr){
		if ( (*itr)->GetID() == id ) { m_vAirRoute.erase(itr); return true; }
	}
	return false;
}

void CAirspace3D::UpdateAddorRemoveObjects()
{
	
	std::vector<int> objectIDs;


	//update Way point
	{
		ALTAirspace::GetWaypointList(m_nID,objectIDs);

		ALTObject3DList newWayPointList;
		for(size_t i=0;i<objectIDs.size();++i){
			ALTObject3DList::iterator theItr;
			if(  m_vWayPoint.end() == (theItr=find_if( m_vWayPoint.begin(),m_vWayPoint.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
				CWayPoint3D * pWaypoint3D = new CWayPoint3D(objectIDs[i]);
				newWayPointList.push_back(pWaypoint3D);
				pWaypoint3D->Update(m_pAirside);
			}else {
				newWayPointList.push_back(*theItr);
			}
		}
		m_vWayPoint = newWayPointList;
	}
	
	// update hold
	{
		objectIDs.clear();
		ALTAirspace::GetHoldList(m_nID,objectIDs);
		ALTObject3DList newHoldList;
		for(size_t i=0;i<objectIDs.size();++i){
			ALTObject3DList::iterator theItr;
			if(  m_vHold.end() == (theItr=find_if( m_vHold.begin(),m_vHold.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
				CHold3D * pHold3D = new CHold3D(objectIDs[i]);
				newHoldList.push_back(pHold3D);
				pHold3D->Update(m_pAirside);
			}else {
				newHoldList.push_back(*theItr);
			}
		}
		m_vHold = newHoldList;
	}
	

	//update Sector
	{
		objectIDs.clear();
		ALTAirspace::GetSectorList(m_nID,objectIDs);
		ALTObject3DList newSectorList;
		for(size_t i=0;i<objectIDs.size();++i){
			ALTObject3DList::iterator theItr;
			if(  m_vSector.end() == (theItr=find_if( m_vSector.begin(),m_vSector.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
				CAirSector3D * pSector3D = new CAirSector3D(objectIDs[i]);
				newSectorList.push_back(pSector3D);
				pSector3D->Update();
			}else {
				newSectorList.push_back(*theItr);
			}
		}
		m_vSector = newSectorList;
	}

	//update AirRoute
	{
		objectIDs.clear();
		ALTAirspace::GetAirRouteList(m_nID,objectIDs);
		ALTObject3DList newAirRouteList;
		for(size_t i=0;i<objectIDs.size();++i){
			ALTObject3DList::iterator theItr;
			if(  m_vSector.end() == (theItr=find_if( m_vSector.begin(),m_vSector.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
				CAirRoute3D * pAirRoute3D = new CAirRoute3D(objectIDs[i]);
				newAirRouteList.push_back(pAirRoute3D);
				pAirRoute3D->Update(m_pAirside);
			}else {
				newAirRouteList.push_back(*theItr);
				CAirRoute3D * pAirRoute3D = (CAirRoute3D*)(theItr->get());
				pAirRoute3D->Update(m_pAirside);
			}
		}
		m_vAirRoute = newAirRouteList;
	}	
	
}

void CAirspace3D::GetObject3DList( ALTObject3DList& objList ) const
{
	objList.insert(objList.end(),m_vAirRoute.begin(),m_vAirRoute.end());
	objList.insert(objList.end(),m_vSector.begin(),m_vSector.end() );
	objList.insert(objList.end(),m_vHold.begin(),m_vHold.end());
	objList.insert(objList.end(),m_vWayPoint.begin(),m_vWayPoint.end() );
}



ALTObject3D* CAirspace3D::AddObject( ALTObject* pObj )
{
	ALTObject3D * pNewObj3D = NULL;
	switch(pObj->GetType())
	{
	case ALT_WAYPOINT:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vWayPoint.push_back(pNewObj3D);
		break;
	case ALT_SECTOR:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vSector.push_back(pNewObj3D);
		break;
	case ALT_HOLD:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vHold.push_back(pNewObj3D);
		break;
	case ALT_AIRROUTE:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vAirRoute.push_back(pNewObj3D);
		break;
	default:
		ASSERT(false);
	}
	return pNewObj3D;
}

void CAirspace3D::UpdateAirRoute( int nAirRouteID )
{
	for(int i=0;i<(int)m_vAirRoute.size();i++)
	{
		CAirRoute3D* pTheRoute =(CAirRoute3D*) m_vAirRoute.at(i).get();
		if( pTheRoute->GetID() == nAirRouteID )
		{
			pTheRoute->Update(m_pAirside);
		}
	}

}