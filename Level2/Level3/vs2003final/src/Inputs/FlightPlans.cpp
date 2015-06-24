#include "StdAfx.h"
#include "Flightplans.h"
#include "../Common/ProbabilityDistribution.h"
#include "../InputAirside/AirRoute.h"
#include "../Common/AirportDatabaseList.h"

using namespace ADODB;

//////////////////////////////////////////////////////////////////////////
// class CFlightPlanRouteFix

CFlightPlanRouteFix::CFlightPlanRouteFix()
 : //m_strName(strName)
  m_dAltitude(0.0)
 , m_dSpeed(0.0)
 , m_nID(-1)
 ,m_nFlightPlanID(-1)
 ,m_nAirRouteWaypointID(-1)
 ,m_pARWaypoint(NULL)
{
	
}

//void CFlightPlanRouteFix::setName(const CString& strName)
//{
//	m_strName = strName;
//}
//
//CString CFlightPlanRouteFix::getName() const
//{
//	return m_strName;
//}

void CFlightPlanRouteFix::setAltitude(const double& dAltitude)
{
	m_dAltitude = dAltitude;
}

double CFlightPlanRouteFix::getAltitude() const
{
	return m_dAltitude;
}

void CFlightPlanRouteFix::setSpeed(const double& dSpeed)
{
	m_dSpeed = dSpeed;
}

double CFlightPlanRouteFix::getSpeed() const
{
	return m_dSpeed;
}

//save the flight plan rout fix data into database
BOOL CFlightPlanRouteFix::SaveData(int nOrder, int nfltplanID)
{
	if (m_nID != -1)
		return UpdateData(nOrder);

	CString strSQL = _T("");
	strSQL.Format("INSERT INTO FLIGHTPLANARWP\
		(FLTPLANID, ARWPID, ALTITUDE, SPEED, WPORDER)\
		VALUES (%d,%d,%f,%f,%d)",
		nfltplanID,m_nAirRouteWaypointID,m_dAltitude,m_dSpeed,nOrder);
	
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)(strSQL),&varCount,adOptionUnspecified);
	
		CString strSQLID = _T("SELECT @@Identity AS id");
		_RecordsetPtr pRecordset = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)(strSQLID),&varCount,adOptionUnspecified);

		//move to get record id recordset
		
		_variant_t var;
		if (!pRecordset->GetadoEOF())
		{
			var = pRecordset->GetCollect(L"id");
			if (var.vt != NULL )
			{
				m_nID = (int)var.lVal;
			}
		}
	}
	catch (_com_error& e) 
	{
		CString strError = (char*)_bstr_t(e.Description());
		return FALSE;
	}

	return TRUE;
}

BOOL CFlightPlanRouteFix::UpdateData(int nOrder)
{
	CString strSQL;
	strSQL.Format(_T("UPDATE FLIGHTPLANARWP\
		SET ALTITUDE =%f, SPEED =%f, WPORDER =%d\
		Where ID = %d"),
		m_dAltitude,m_dSpeed,nOrder,m_nID);
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified);
	}
	catch (_com_error& e) 
	{
		CString strError = (char*)_bstr_t(e.Description());
		return FALSE;
	}

	return TRUE;
}

BOOL CFlightPlanRouteFix::DeleteData()
{
	if (m_nID == -1)
		return TRUE;

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM FLIGHTPLANARWP Where ID = %d"), m_nID);

	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified);
	
	}
	catch (_com_error& e) 
	{
		CString strError = (char*)_bstr_t(e.Description());
		return FALSE;

	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//CFlightPlanRoute

CFlightPlanRoute::CFlightPlanRoute(CAirRoute* pAirRoute)
 : m_pAirRoute(pAirRoute)
 , m_nID(-1)
 , m_nFlightPlanID(-1)
{
}

BOOL CFlightPlanRoute::ReadData()
{
	//m_lstRoute.clear();

	//CString strSQL;
	//strSQL.Format(_T("SELECT ID, FixName, Altitude, Speed FROM FlightPlanRouteFix WHERE FltPlanRouteID = %d"), m_nID);
	//
	//try
	//{
	//	_ConnectionPtr& pConnection = DATABASECONNECTION.GetConnection();
	//	if (pConnection.GetInterfacePtr() == NULL)
	//	{
	//		CString strError = "Haven't connected to the database .";
	//		return FALSE;
	//	}

	//	_variant_t varCount;
	//	_RecordsetPtr&  pRecordset = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	//
	//	_variant_t varValue;

	//	while(!pRecordset->GetadoEOF())
	//	{
	//		int nID = -1;
	//		CString strFixName;
	//		double dAltitude =0.0, dSpeed = 0.0;
	//					
	//		varValue = pRecordset->GetCollect(L"ID");
	//		if (varValue.vt == NULL)
	//		{
	//			continue;
	//		}
	//		nID = varValue.intVal;

	//		varValue = pRecordset->GetCollect(L"FixName");
	//		if (varValue.vt != NULL)
	//		{
	//			strFixName = (char *)_bstr_t(varValue.bstrVal);
	//		}

	//		varValue = pRecordset->GetCollect(L"Altitude");
	//		if (varValue.vt != NULL)
	//		{
	//			dAltitude = varValue.dblVal;
	//		}

	//		varValue = pRecordset->GetCollect(L"Speed");
	//		if (varValue.vt != NULL)
	//		{
	//			dSpeed = varValue.dblVal;
	//		}

	//		CFlightPlanRouteFix *pFix = new CFlightPlanRouteFix(strFixName);
	//		pFix->setID(nID);
	//		pFix->setAltitude(dAltitude);
	//		pFix->setSpeed(dSpeed);

	//		m_lstRoute.push_back(pFix);

	//		pRecordset->MoveNext();
	//	}
	//	pRecordset->Close();
	//}
	//catch (_com_error &e)
	//{
	//	CString strError = (char *)_bstr_t(e.Description());
	//	return FALSE;
	//}

	return TRUE;
}

BOOL CFlightPlanRoute::SaveData(int& nOrder,int nFltPlanID)
{
	std::vector<CFlightPlanRouteFix *>::iterator iter = m_lstRoute.begin();
	std::vector<CFlightPlanRouteFix *>::iterator iterEnd = m_lstRoute.end();

	for (;iter != iterEnd; ++ iter)
	{
		if(((*iter)->SaveData(nOrder,nFltPlanID)) == FALSE)
			return FALSE;

		nOrder = nOrder + 1;
	}

	return TRUE;
}

int CFlightPlanRoute::GetCount()
{
	return static_cast<int>(m_lstRoute.size());
}

CFlightPlanRouteFix* CFlightPlanRoute::GetItem(int nIndex)
{
	ASSERT(nIndex < GetCount());
	return m_lstRoute.at(nIndex);
}

BOOL CFlightPlanRoute::DeleteData()
{
	std::vector<CFlightPlanRouteFix *>::iterator iter = m_lstRoute.begin();
	std::vector<CFlightPlanRouteFix *>::iterator iterEnd = m_lstRoute.end();

	for (;iter != iterEnd; ++ iter)
	{
		(*iter)->DeleteData();
	}
	return TRUE;
}
ARWaypoint* CFlightPlanRoute::GetArWaypoint(int nArwpID)
{
	ASSERT(m_pAirRoute != NULL);

	return m_pAirRoute->getARWaypoint(nArwpID);
}
//////////////////////////////////////////////////////////////////////////
//
CFlightPlan::CFlightPlan(const FlightConstraint& FltType):
 // m_strFltType(strFltType)
 m_verticalProfile(NULL),
 m_nID(-1)
{
	m_fltCnst = FltType;
}

CFlightPlan::~CFlightPlan()
{
}

CString CFlightPlan::GetUpdateScript() const
{
	CString strSQL;
	return strSQL;
}

CString CFlightPlan::GetInsertScript() const
{
	CString strSQL;
	return strSQL;
}

CString CFlightPlan::GetDeleteScript() const
{
	CString strSQL;
	return strSQL;
}

BOOL CFlightPlan::SaveData(int nProjID)
{
	if (m_nID != -1)
		return UpdateData();

	TCHAR szDist[128] = { 0 };
	if (m_verticalProfile != NULL)
	{
		m_verticalProfile->printDistribution(szDist);
	}

	CString strSQL;
	strSQL.Format(_T("INSERT INTO FlightPlan \
		(ProjID, FltType, VProfile) \
		VALUES (%d,'%s','%s')"),nProjID,GetFltTypeStr(), szDist);

	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)(strSQL ),&varCount,adOptionUnspecified);


		CString strSQLID = _T("SELECT @@Identity AS id");
		_RecordsetPtr pRecordset = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)(strSQLID),&varCount,adOptionUnspecified);
		//move to get record id recordset
		//pRecordset = pRecordset->NextRecordset(&varCount);
		_variant_t var;
		if (!pRecordset->GetadoEOF())
		{
			var = pRecordset->GetCollect(L"id");
			if (var.vt != NULL )
			{
				int nID = (int)var.lVal;
				setID(nID);
				
				//save air route way point
				int nOrder = 0;
				for (std::vector<CFlightPlanRoute*>::iterator iter = m_vFltPlanRoute.begin(); iter != m_vFltPlanRoute.end(); ++iter)
				{
					(*iter)->SaveData(nOrder,m_nID);
				}
			}
		}

		for (std::vector<CFlightPlanRoute*>::iterator iter = m_vFltPlanRouteNeedDel.begin(); iter != m_vFltPlanRouteNeedDel.end(); ++iter)
		{
			(*iter)->DeleteData();
			delete *iter;
		}
		m_vFltPlanRouteNeedDel.clear();

	}
	catch (_com_error &e)
	{
		CString strError = (char *)_bstr_t(e.Description());
		return FALSE;
	}

	return TRUE;
}

BOOL CFlightPlan::UpdateData()
{
	if (m_nID == -1)
		return FALSE;

	TCHAR szDist[128] = { 0 };
	if (m_verticalProfile != NULL)
		m_verticalProfile->printDistribution(szDist);

	CString strSQL;
	strSQL.Format(_T("UPDATE FlightPlan \
		SET FltType = '%s', VProfile = '%s' \
		WHERE ID = %d"),GetFltTypeStr(), szDist,m_nID);

	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified);
	
		int nOrder = 0;
		for (std::vector<CFlightPlanRoute*>::iterator iter = m_vFltPlanRoute.begin(); iter != m_vFltPlanRoute.end(); ++iter)
		{
			(*iter)->SaveData(nOrder,m_nID);
		}

		for (std::vector<CFlightPlanRoute*>::iterator iter = m_vFltPlanRouteNeedDel.begin(); iter != m_vFltPlanRouteNeedDel.end(); ++iter)
		{
			(*iter)->DeleteData();
			delete *iter;
		}
		m_vFltPlanRouteNeedDel.clear();
	}
	catch (_com_error &e)
	{
		CString strError = (char *)_bstr_t(e.Description());
		return FALSE;
	}

	return TRUE;
}

BOOL CFlightPlan::DeleteData()
{
	if (m_nID == -1)
		return TRUE;

	CString strSQL;
	strSQL.Format(_T("DELETE FROM FlightPlan WHERE ID = %d   \
					  DELETE FROM FLIGHTPLANARWP WHERE FLTPLANID = %d"),m_nID,m_nID);

	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified);
	}
	catch (_com_error &e)
	{
		CString strError = (char *)_bstr_t(e.Description());
		return FALSE;
	}

	return TRUE;
}

BOOL CFlightPlan::ReadData()
{
	
	if (m_nID == -1)
		return false;

	CString strSQL = _T("");
	
	strSQL.Format(_T("SELECT ID, FLTPLANID, ARWPID, ALTITUDE, SPEED, WPORDER\
		FROM FLIGHTPLANARWP\
		WHERE (FLTPLANID = %d)\
		ORDER BY WPORDER"),m_nID);

	try
	{
		_ConnectionPtr& pConnection = DATABASECONNECTION.GetConnection();
		if (pConnection.GetInterfacePtr() == NULL)
		{
			CString strError = "Haven't connected to the database .";
			return FALSE;
		}
		_variant_t varCount;
		_RecordsetPtr&  pRecordset = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );

		_variant_t varValue;

		while(!pRecordset->GetadoEOF())
		{
			CFlightPlanRouteFix * pFix = new CFlightPlanRouteFix();

			_variant_t varValue;
			varValue = pRecordset->GetCollect(L"ID");
			if (varValue.vt == NULL)
			{
				pRecordset->MoveNext();
				continue;
			}
			int nID = varValue.intVal;
			pFix->setID(nID);

			pFix->setFlightPlanID(m_nID);



			varValue = pRecordset->GetCollect(L"ARWPID");
			if (varValue.vt != NULL )
			{
				pFix->setAirRouteWaypointID(varValue.intVal);
			}

			varValue = pRecordset->GetCollect(L"ALTITUDE");
			if (varValue.vt != NULL )
			{
				pFix->setAltitude(varValue.dblVal);
			}		

			varValue = pRecordset->GetCollect(L"SPEED");
			if (varValue.vt != NULL )
			{
				pFix->setSpeed(varValue.dblVal);
			}

			varValue = pRecordset->GetCollect(L"WPORDER");
			if (varValue.vt != NULL )
			{
				pFix->setOrder(varValue.intVal);
			}
			AddFltPlanWaypoint(pFix);


			pRecordset->MoveNext();
		}

		pRecordset->Close();
	}
	catch (_com_error &e)
	{
		CString strError = (char *)_bstr_t(e.Description());
		return FALSE;
	}

	return TRUE;
}
void CFlightPlan::AddFltPlanWaypoint( CFlightPlanRouteFix * pFltPlanWayPoint)
{
	ASSERT(pFltPlanWayPoint != NULL);
	
	//if the air route is null, add one
	if (static_cast<int>(m_vFltPlanRoute.size()) ==0)
	{
		int nArwpID = pFltPlanWayPoint->getAirRouteWaypointID();
		CAirRoute *pAirRoute = new CAirRoute;
		pAirRoute->ReadDataByARWaypointID(nArwpID);
		AddItem(new CFlightPlanRoute(pAirRoute));
	}
	int nAirRouteCount = static_cast<int>(m_vFltPlanRoute.size()) ;
	ASSERT(nAirRouteCount > 0);
	
	//get  the last air route
	CFlightPlanRoute *pFltPlanRoute = NULL;
	pFltPlanRoute = m_vFltPlanRoute[nAirRouteCount -1];
	//assert the insert item's air route is the previous one
	ARWaypoint *pArWayPoint = NULL;
	pArWayPoint = pFltPlanRoute->GetArWaypoint(pFltPlanWayPoint->getAirRouteWaypointID());
	if (pArWayPoint == NULL)
	{
		int nArwpID = pFltPlanWayPoint->getAirRouteWaypointID();
		CAirRoute *pAirRoute = new CAirRoute;
		pAirRoute->ReadDataByARWaypointID(nArwpID);
		AddItem(new CFlightPlanRoute(pAirRoute));
	}

	//
	nAirRouteCount = static_cast<int>(m_vFltPlanRoute.size()) ;
	ASSERT(nAirRouteCount > 0);
	pFltPlanRoute = m_vFltPlanRoute[nAirRouteCount -1];
	pArWayPoint = pFltPlanRoute->GetArWaypoint(pFltPlanWayPoint->getAirRouteWaypointID());
	if (pArWayPoint != NULL)
	{
		pFltPlanWayPoint->setARWaypoint(pArWayPoint);
		pFltPlanRoute->AddItem(pFltPlanWayPoint);
	}


}
void CFlightPlan::DeleteItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vFltPlanRoute.size());
	m_vFltPlanRoute.erase(m_vFltPlanRoute.begin() + nIndex);

	m_vFltPlanRouteNeedDel.push_back(m_vFltPlanRoute[nIndex]);
}

CFlightPlanRoute* CFlightPlan::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vFltPlanRoute.size());
	return m_vFltPlanRoute.at(nIndex);
}


//////////////////////////////////////////////////////////////////////////
/// Class CFlightPlans
CFlightPlans::CFlightPlans()
 : m_pAirRoutes(NULL)
 , m_nProjID(-1)
 ,m_pAirportDB(NULL)
{
}

CFlightPlans::~CFlightPlans()
{
}

BOOL CFlightPlans::AddItem(CFlightPlan *pFltPlan)
{
	m_vFlightPlan.push_back(pFltPlan);
	return TRUE;
}

BOOL CFlightPlans::DelItem(CFlightPlan* pFltPlan)
{
	std::vector<CFlightPlan*>::iterator iter = m_vFlightPlan.begin();//std::find(m_vFlightPlan.begin(),m_vFlightPlan.end(),pFltPlan);
	for(;iter != m_vFlightPlan.end(); ++iter)
	{
		if ( pFltPlan == *iter)
		{
			m_vFlightPlan.erase(iter);
			m_vFlightPlanNeedDel.push_back(pFltPlan);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFlightPlans::ReadData()
{
	m_vFlightPlan.clear();

	CString strSQL;
	strSQL.Format(_T("SELECT ID, FltType, Operation, VProfile \
		FROM FlightPlan \
		WHERE ProjID = %d"), m_nProjID);

	try
	{
		_ConnectionPtr& pConnection = DATABASECONNECTION.GetConnection();
		if (pConnection.GetInterfacePtr() == NULL)
		{
			CString strError = "Haven't connected to the database .";
			return FALSE;
		}
		_variant_t varCount;
		_RecordsetPtr&  pRecordset = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );

		_variant_t varValue;

		while(!pRecordset->GetadoEOF())
		{
			int nID;
			CString strFltType;
			int nOperation;
			CString strVProfile;
			
			
			_variant_t varValue;
			varValue = pRecordset->GetCollect(L"ID");
			if (varValue.vt == NULL)
			{
				pRecordset->MoveNext();
				continue;
			}
			 nID = varValue.intVal;
			
			varValue = pRecordset->GetCollect(L"FltType");
			if (varValue.vt != NULL )
			{
				strFltType = (char *)_bstr_t(varValue.bstrVal);
			}

			varValue = pRecordset->GetCollect(L"Operation");
			
			if (varValue.vt != NULL)
			{
				nOperation = varValue.intVal;
			}

			varValue = pRecordset->GetCollect(L"VProfile");
			if (varValue.vt != NULL)
			{
				strVProfile = (char *)_bstr_t(varValue.bstrVal);
			}
			FlightConstraint fltcnst;
			fltcnst.SetAirportDB( (m_pAirportDB) );
			fltcnst.setConstraintWithVersion(strFltType);
			CFlightPlan *pFltPlan = new CFlightPlan(fltcnst);
			pFltPlan->setID(nID);
			pFltPlan->SetVerticalProfile(NULL);
			pFltPlan->ReadData();

			m_vFlightPlan.push_back(pFltPlan);
			pRecordset->MoveNext();
		}

		pRecordset->Close();
	}
	catch (_com_error &e)
	{
		CString strError = (char *)_bstr_t(e.Description());
		return FALSE;
	}

	return TRUE;
}

BOOL CFlightPlans::SaveData()
{
	std::vector<CFlightPlan*>::iterator iter = m_vFlightPlan.begin();
	std::vector<CFlightPlan*>::iterator iterEnd = m_vFlightPlan.end();
	for (;iter != iterEnd; ++iter)
	{
		if(!(*iter)->SaveData(m_nProjID))
			return FALSE;
	}

	std::vector<CFlightPlan*>::iterator iterDel = m_vFlightPlanNeedDel.begin();
	std::vector<CFlightPlan*>::iterator iterDelEnd = m_vFlightPlanNeedDel.end();
	for (;iterDel != iterDelEnd; ++iterDel)
	{
		if(!(*iterDel)->DeleteData())
			return FALSE;
	}
	m_vFlightPlanNeedDel.clear();


	return TRUE;
}

int CFlightPlans::GetItemCount()
{
	return static_cast<int>(m_vFlightPlan.size());
}

CFlightPlan* CFlightPlans::GetItem(int nIndex)
{
	ASSERT(nIndex <static_cast<int>(m_vFlightPlan.size()));
	return m_vFlightPlan.at(nIndex);

}

CFlightPlanRouteFix * CFlightPlanRoute::GetPlanRouteFix( int nArwpID )
{	
	for(int i=0;i< GetCount();i++){
		CFlightPlanRouteFix * pRet  = GetItem(i);
		if( pRet->getAirRouteWaypointID() == nArwpID  )return pRet;
	}
	return NULL;	
}

CString CFlightPlan::GetFltTypeStr() const
{
	char str[1024];
	m_fltCnst.WriteSyntaxStringWithVersion(str);
	return CString(str);
}

void CFlightPlan::SetFltType( const CString& strFltType )
{
	m_fltCnst.setConstraintWithVersion(strFltType);
}