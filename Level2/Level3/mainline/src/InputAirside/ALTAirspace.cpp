#include "StdAfx.h"
#include ".\altairspace.h"
#include "AirWayPoint.h"
#include "AirSector.h"
#include "Airhold.h"
#include "AirRoute.h"


ALTAirspace::ALTAirspace(void)
{
}

ALTAirspace::~ALTAirspace(void)
{
}

void ALTAirspace::GetWaypointList( int nAirspaceID, ALTObjectUIDList& IDList )
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM WAYPOINT_VIEW WHERE APTID = %d",nAirspaceID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			IDList.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirspace::GetSectorList( int nAirspaceID, ALTObjectUIDList& IDList )
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM SECTOR_VIEW WHERE APTID = %d",nAirspaceID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			IDList.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirspace::GetSectorObjList(int nAirspaceID, ALTObjectList& vObjectList)
{
	std::vector<int> vIDs;
	GetSectorList(nAirspaceID,vIDs);
	int nCount = (int)vIDs.size();
	for(int i =0 ;i < nCount; ++i)
	{
		AirSector * pSector = new AirSector();
		pSector->ReadObject(vIDs.at(i));
		vObjectList.push_back(pSector);
	}
}

void ALTAirspace::GetHoldList( int nAirspaceID, ALTObjectUIDList& IDList )
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM HOLD_VIEW WHERE APTID = %d",nAirspaceID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			IDList.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirspace::GetAirRouteList( int nAirspaceID, ALTObjectUIDList& IDList )
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM AIRROUTE_VIEW WHERE APTID = %d",nAirspaceID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			IDList.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

ALTObjectList* ALTAirspace::getWaypointList()
{
	return &m_lstWaypoint;
}

ALTObjectList* ALTAirspace::getAirRouteList()
{
	return &m_lstAirRoute;
}

ALTObjectList* ALTAirspace::getAirHoldList()
{
	return &m_lstAirHold;
}

ALTObjectList* ALTAirspace::getAirSectorList()
{
	return &m_lstAirSector;
}

void ALTAirspace::LoadAirspace(int nAirSpaceID  )
{
	LoadWayPoint(nAirSpaceID);
	LoadAirRoute(nAirSpaceID);
	LoadAirHold(nAirSpaceID);
	LoadAirSector(nAirSpaceID);
}

void ALTAirspace::LoadWayPoint(int nAirSpaceID)
{
	m_lstWaypoint.clear();

	ALTObjectUIDList objectIDs;
	ALTAirspace::GetWaypointList(nAirSpaceID,objectIDs);

	int nCount = (int)objectIDs.size();
	for(int i =0 ;i < nCount; ++i)
	{
		AirWayPoint * pWayPoint = new AirWayPoint();
		pWayPoint->ReadObject(objectIDs.at(i));
		m_lstWaypoint.push_back(pWayPoint);
	}
}

void ALTAirspace::LoadAirRoute(int nAirSpaceID)
{
	m_lstAirRoute.clear();

	ALTObjectUIDList objectIDs;
	ALTAirspace::GetAirRouteList(nAirSpaceID,objectIDs);

	int nCount = (int)objectIDs.size();
	for(int i =0 ;i < nCount; ++i)
	{
		CAirRoute * pAirRoute = new CAirRoute();
		pAirRoute->ReadData(objectIDs.at(i));
		m_lstAirRoute.push_back(pAirRoute);
	}
}

void ALTAirspace::LoadAirHold(int nAirSpaceID)
{
	m_lstAirHold.clear();

	ALTObjectUIDList objectIDs;
	ALTAirspace::GetHoldList(nAirSpaceID,objectIDs);

	int nCount = (int)objectIDs.size();
	for(int i =0 ;i < nCount; ++i)
	{
		AirHold * pAirHold = new AirHold();
		pAirHold->ReadObject(objectIDs.at(i));
		m_lstAirHold.push_back(pAirHold);
	}
}

void ALTAirspace::LoadAirSector(int nAirSpaceID)
{
	m_lstAirSector.clear();
	ALTAirspace::GetSectorObjList(nAirSpaceID,m_lstAirSector);


}

const ALTObject* ALTAirspace::GetWaypoint( const CGuid& guid ) const
{
	return GetObject(m_lstWaypoint,guid);
}

const ALTObject* ALTAirspace::GetAirRoute( const CGuid& guid ) const
{
	return GetObject(m_lstAirRoute,guid);
}

const ALTObject* ALTAirspace::GetAirHold( const CGuid& guid ) const 
{
	return GetObject(m_lstAirHold,guid);
}

const ALTObject* ALTAirspace::GetAirSector( const CGuid& guid ) const
{
	return GetObject(m_lstAirSector,guid);
}

const ALTObject * ALTAirspace::GetObject( const ALTObjectList& objectList, const CGuid& guid ) const
{
	ALTObjectList::const_iterator cons_iter = objectList.begin();
	for (; cons_iter != objectList.end(); ++cons_iter)
	{
		if ((*cons_iter)->getGuid() == guid)
		{
			return (*cons_iter).get();
		}
	}

	return NULL;
}

ALTObject * ALTAirspace::GetObject( ALTObjectList&objectList, int nID )
{
	ALTObjectList::iterator cons_iter = objectList.begin();
	for (; cons_iter != objectList.end(); ++cons_iter)
	{
		if ((*cons_iter)->getID() == nID)
		{
			return (*cons_iter).get();
		}
	}

	return NULL;
}
ALTObject * ALTAirspace::GetObjectByID( ALTOBJECT_TYPE enumObjType, int nID )
{
	ALTObject *pObject = NULL;

	if(enumObjType == ALT_WAYPOINT)
		pObject = GetObject(m_lstWaypoint,nID);
	else if(enumObjType == ALT_AIRROUTE)
		pObject = GetObject(m_lstAirRoute,nID);	
	else if(enumObjType == ALT_SECTOR)
		pObject = GetObject(m_lstAirSector,nID);			
	else if(enumObjType == ALT_HOLD)
		pObject = GetObject(m_lstAirHold,nID);			
	else
	{//if come here, need implement this function, and read the display properties
		ASSERT(0);
	}


	return pObject;
}












