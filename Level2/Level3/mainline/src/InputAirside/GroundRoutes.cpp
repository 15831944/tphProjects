#include "stdafx.h"
#include "GroundRoutes.h"
#include "GroundRoute.h"

GroundRoutes::GroundRoutes(int nAirportID)
{
	m_nAirportID = nAirportID;
	m_vectGroundRoute.clear();
	m_vectDelGroundRoute.clear();
}

GroundRoutes::~GroundRoutes()
{
	GroundRouteIter iter = m_vectGroundRoute.begin();

	for (;iter != m_vectGroundRoute.end(); ++ iter)
	{
		delete *iter;
	}
	m_vectGroundRoute.clear();

	for (iter=m_vectDelGroundRoute.begin(); iter!=m_vectDelGroundRoute.end(); iter++)
	{
		delete *iter;
	}
	m_vectDelGroundRoute.clear();
}

int GroundRoutes::GetAirportID()
{
	return m_nAirportID;
}

//Muse be sequenced by index
void GroundRoutes::ReadData()
{
	CString strSQL;
	
	strSQL.Format(_T("SELECT * FROM ALTOBJECT WHERE (TYPE = %d) AND (APTID = %d)"), (int)ALT_GROUNDROUTE, m_nAirportID);

	CADORecordset adoRecordset;
	long lItemAffect = 0L;

	CADODatabase::ExecuteSQLStatement(strSQL,lItemAffect,adoRecordset);
	while (!adoRecordset.IsEOF())
	{
		GroundRoute *pObject = new GroundRoute(m_nAirportID);

		int nID = -1;
		adoRecordset.GetFieldValue("ID", nID);
		pObject->ReadObject(nID);

		if (pObject->GetFirstItem() == NULL && pObject->GetObjectName().IsBlank())
		{
			m_vectDelGroundRoute.push_back(pObject);
			adoRecordset.MoveNextData();
			continue;
		}

		m_vectGroundRoute.push_back(pObject);

		adoRecordset.MoveNextData();
	}
}

void GroundRoutes::SaveData()
{
	GroundRouteIter iter = m_vectGroundRoute.begin();
	GroundRouteIter iterEnd = m_vectGroundRoute.end();

	for (;iter != iterEnd; ++ iter)
	{
		(*iter)->SaveObject(m_nAirportID);
	}

	for (iter=m_vectDelGroundRoute.begin(); iter!=m_vectDelGroundRoute.end(); iter++)
	{
		(*iter)->DeleteObject((*iter)->getID());
	}

	m_vectDelGroundRoute.clear();
}

int GroundRoutes::GetCount()
{
	return (int)m_vectGroundRoute.size();
}

GroundRoute* GroundRoutes::GetItem(int nIndex)
{
	ASSERT(nIndex >=0 && nIndex < (int)m_vectGroundRoute.size());
	return m_vectGroundRoute[nIndex];
}

GroundRoute* GroundRoutes::FindItem(int nID)
{
	for (GroundRouteIter iter = m_vectGroundRoute.begin(); iter != m_vectGroundRoute.end(); ++iter)
	{
		if ((*iter)->getID() == nID)
			return (*iter);
	}

	return NULL;
}

BOOL GroundRoutes::IsItemExist(GroundRoute* pGroundRoute)
{
	if (NULL == pGroundRoute)
	{
		return FALSE;
	}

	int nID = pGroundRoute->getID();

	for (GroundRouteIter iter = m_vectGroundRoute.begin(); iter != m_vectGroundRoute.end(); ++iter)
	{
		if ((*iter)->getID() == nID)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void GroundRoutes::AddItem(GroundRoute* pGroundRoute)
{
	m_vectGroundRoute.push_back(pGroundRoute);
}

void GroundRoutes::DeleteItem(GroundRoute* pGroundRoute)
{
	ASSERT(pGroundRoute);

	for (GroundRouteIter iter = m_vectGroundRoute.begin(); iter != m_vectGroundRoute.end(); ++iter)
	{
		if ((*iter) == pGroundRoute)
		{
			(*iter)->DeleteContent();
			m_vectDelGroundRoute.push_back(*iter);
			m_vectGroundRoute.erase(iter);

			return;
		}
	}
}


void GroundRoutes::VerifyData()
{
	for (GroundRouteIter iter = m_vectGroundRoute.begin(); iter != m_vectGroundRoute.end(); ++iter)
	{
		GroundRoute *pGroundRoute = *iter;
		pGroundRoute->VerifyData();
		pGroundRoute->SaveObject(m_nAirportID);
	}
}
