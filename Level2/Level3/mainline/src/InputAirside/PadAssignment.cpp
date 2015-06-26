#include "StdAfx.h"
#include ".\weatherimpact.h"
#include "PadAssignment.h"
#include "..\Database\DBElementCollection_Impl.h"
/////////////////////////////////////////////////////////////////////////////////
///////////////////PadInfo///////////////////////////////////////////////////////
PadInfo::PadInfo()
:m_nIndex(-1)
,m_nID(0)
,m_nObjectID(0)
,m_AssignmentPercent(0.0)
{

}

PadInfo::~PadInfo()
{

}

void PadInfo::SaveData(int nID)
{
	if (m_nIndex != -1)
	{
		 UpdateData();
	}
	else
	{
		m_nID = nID;
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO PAD_INFO (PADID,PADPERCENT,NID) VALUES (%d,%f,%d)"),m_nObjectID,m_AssignmentPercent,m_nID);
		m_nIndex = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}

void PadInfo::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE PAD_INFO SET PADID = %d,PADPERCENT = %f,NID = %d WHERE (ID = %d)"),m_nObjectID,m_AssignmentPercent,m_nID,m_nIndex);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void PadInfo::DelData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM PAD_INFO\
					 WHERE (ID = %d)"),m_nIndex);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void PadInfo::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nIndex);
	adoRecordset.GetFieldValue(_T("PADID"),m_nObjectID);
	adoRecordset.GetFieldValue(_T("PADPERCENT"),m_AssignmentPercent);
	adoRecordset.GetFieldValue(_T("NID"),m_nID);
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////FlightTypeInfo//////////////////////////////////////////////////
FlightTypeInfo::FlightTypeInfo()
:m_nPadID(-1)
{

}

FlightTypeInfo::~FlightTypeInfo()
{
	std::vector<PadInfo*>::iterator it = m_vObject.begin();
	for (; it != m_vObject.end(); ++it)
	{
		delete (*it);
	}
	it = m_vDelObject.begin();
	for (; it != m_vDelObject.end(); ++it)
	{
		delete (*it);
	}
}

PadInfo* FlightTypeInfo::getPadInfo(int nObjectID)
{
	std::vector<PadInfo*>::iterator it = m_vObject.begin();
	for (; it != m_vObject.end(); ++it)
	{
		if (nObjectID == (*it)->getObjectID())
		{
			return (*it);
		}
	}
	return NULL;
}

void FlightTypeInfo::AddNewItem(PadInfo* pPadInfo)
{
	ASSERT(pPadInfo);

	m_vObject.push_back(pPadInfo);
}

void FlightTypeInfo::DelItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vObject.size());

	m_vDelObject.push_back(m_vObject[nIndex]);
	m_vObject.erase(m_vObject.begin() + nIndex);
}

void FlightTypeInfo::DelItem(PadInfo* pPadInfo)
{
	int nCount  = (int)m_vObject.size();
	int nIndex = 0;
	for (nIndex  ; nIndex < nCount ; nIndex++)
	{
		if (pPadInfo->getID() == m_vObject.at(nIndex)->getID())
		{
			DelItem(nIndex);
			return;
		}
	}
}

void FlightTypeInfo::DelAllItem()
{
	std::vector<PadInfo*>::iterator iter = m_vObject.begin();
	int nIndex = 0;
	for (;iter != m_vObject.end();++iter)
	{
		DelItem(nIndex);
		DelAllItem();
		return;
	}
}

PadInfo* FlightTypeInfo::getItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vObject.size());
	return m_vObject.at(nIndex);
}

void FlightTypeInfo::SaveData(int nFligtID)
{
	if (m_nPadID != -1)
	{
		 UpdateData();
	}
	else
	{
		m_nFlightID = nFligtID;

		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO FLIGHTINFO (FLIGHTTYPE,FLIGHTID) VALUES ('%s',%d)"),m_strFlightType,m_nFlightID);
		m_nPadID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	int nCount = (int)m_vObject.size();
	std::vector<PadInfo*>::iterator it = m_vObject.begin();
	for (; it != m_vObject.end(); ++it)
	{
		(*it)->SaveData(m_nPadID);
	}

	it = m_vDelObject.begin();
	for (;it != m_vDelObject.end(); ++it)
	{
		(*it)->DelData();
		delete (*it);
	}
	m_vDelObject.clear();
}

void FlightTypeInfo::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE FLIGHTINFO\
					 SET FLIGHTTYPE ='%s', FLIGHTID =%d\
					 WHERE (ID = %d)"),m_strFlightType,m_nFlightID,m_nPadID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void FlightTypeInfo::DelData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM FLIGHTINFO\
					 WHERE (ID = %d)"),m_nPadID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<PadInfo*>::iterator it =  m_vDelObject.begin();

	for (;it != m_vDelObject.end(); ++it)
	{
		(*it)->DelData();
		delete (*it);
	}
	m_vDelObject.clear();
}

void FlightTypeInfo::ReadData(CADORecordset& adoRecordset )
{
	adoRecordset.GetFieldValue(_T("ID"),m_nPadID);
	adoRecordset.GetFieldValue(_T("FLIGHTTYPE"),m_strFlightType);
	adoRecordset.GetFieldValue(_T("FLIGHTID"),m_nFlightID);
	
	ReadData();
}
void FlightTypeInfo::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID,PADID,PADPERCENT,NID FROM PAD_INFO WHERE (NID= %d)"),m_nPadID);
	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	try
	{
		while (!adoRecordset.IsEOF())
		{
			PadInfo* pPadInfo = new PadInfo();
			pPadInfo->ReadData(adoRecordset);

			m_vObject.push_back(pPadInfo);
			adoRecordset.MoveNextData();
		}
	}
	catch (_com_error e)
	{	
		throw CADOException((char *)e.Description());
	}
}

/////////////////////////////////////////////////////////////////////////////////
//////////////////WeatherInfo////////////////////////////////////////////////////
WeatherInfo::WeatherInfo()
:m_nID(-1)
{
	
}

WeatherInfo::~WeatherInfo()
{
	std::vector<FlightTypeInfo*>::iterator it = m_vFlightType.begin();
	for (; it != m_vFlightType.end(); ++it)
	{
		delete (*it);
	}
	it = m_vDelFlightType.begin();
	for (; it != m_vDelFlightType.end(); ++it)
	{
		delete (*it);
	}
}

FlightTypeInfo* WeatherInfo::getFlightTypeByCString(CString strFlightType)
{
	std::vector<FlightTypeInfo*>::iterator it = m_vFlightType.begin();
	for (;it != m_vFlightType.end(); ++it)
	{
		if (strFlightType.CompareNoCase((*it)->getFlightType()) == 0)
		{
			return (*it);
		}
	}
	return NULL;
}

void WeatherInfo::AddNewItem(FlightTypeInfo* pFlightType)
{
	ASSERT(pFlightType);

	m_vFlightType.push_back(pFlightType);
}

void WeatherInfo::DelItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vFlightType.size());

	m_vDelFlightType.push_back(m_vFlightType[nIndex]);
	m_vFlightType.erase(m_vFlightType.begin() + nIndex);
}

void WeatherInfo::DelItem(FlightTypeInfo* pFlightType)
{
	int nCount  = (int)m_vFlightType.size();
	int nIndex = 0;
	for (nIndex  ; nIndex < nCount ; nIndex++)
	{
		if (pFlightType->getPadID() == m_vFlightType.at(nIndex)->getPadID())
		{
			DelItem(nIndex);
			return;
		}
	}
}
FlightTypeInfo* WeatherInfo::getItem(int nIndex)
{
	ASSERT( nIndex < (int)m_vFlightType.size());
	
	return m_vFlightType.at(nIndex);
}

void WeatherInfo::SaveData(int nProjID)
{
	if (m_nID != -1)
	{
	    UpdateData();
	}
	else
	{
		m_nProjID = nProjID;
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO WEATHERINFO (WEATHER,PROJID) VALUES (%d,%d)"),(int)m_emWeatherCondition,m_nProjID);

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

	int nCount = (int)m_vFlightType.size();
	std::vector<FlightTypeInfo*>::iterator it = m_vFlightType.begin();
 	for (; it != m_vFlightType.end(); ++it)
 	{
 		(*it)->SaveData(m_nID);
 	}

	it = m_vDelFlightType.begin();
	for (; it != m_vDelFlightType.end(); ++it)
	{
		(*it)->DelData();
		delete (*it);
	}
	m_vDelFlightType.clear();
}

void WeatherInfo::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE WEATHERINFO\
					 SET WEATHER =%d, PROJID =%d\
					 WHERE (ID = %d)"),(int)m_emWeatherCondition,m_nProjID,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void WeatherInfo::DelData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM WEATHERINFO\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	std::vector<FlightTypeInfo*>::iterator it = m_vDelFlightType.begin();
;
	for (; it != m_vDelFlightType.end(); ++it)
	{
		(*it)->DelData();
		delete (*it);
	}
	m_vDelFlightType.clear();
}

void WeatherInfo::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	int nWeather = 0;
	adoRecordset.GetFieldValue(_T("WEATHER"),nWeather);
	m_emWeatherCondition = (WEATHERCONDITION)nWeather;
	adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
	ReadData();
}

void WeatherInfo::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID,FLIGHTTYPE,FLIGHTID FROM FLIGHTINFO WHERE (FLIGHTID = %d)"),m_nID);
	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		FlightTypeInfo* pFlightType = new FlightTypeInfo();
		pFlightType->ReadData(adoRecordset);

		m_vFlightType.push_back(pFlightType);
		adoRecordset.MoveNextData();
	}
}

void WeatherInfo::InitNewData()
{
	FlightTypeInfo* pFlightType = new FlightTypeInfo();
	pFlightType->setFlightType("DEFAULT");
	m_vFlightType.push_back(pFlightType);
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////WeatherListInfo//////////////////////////////////////////
WeatherListInfo::WeatherListInfo()
{
	
}

WeatherListInfo::~WeatherListInfo()
{
	std::vector<WeatherInfo*>::iterator it = m_vWeather.begin();
	for (; it != m_vWeather.end(); ++it)
	{
		delete (*it);
	}

	it = m_vDelWeather.begin();
	for (;it != m_vDelWeather.begin(); ++it)
	{
		delete (*it);
	}
}

void WeatherListInfo::AddNewItem(WeatherInfo* pWeather)
{
	ASSERT(pWeather);
	m_vWeather.push_back(pWeather);
}

void WeatherListInfo::DelItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vWeather.size());

	m_vDelWeather.push_back(m_vWeather[nIndex]);
	m_vWeather.erase(m_vWeather.begin() + nIndex);
}

WeatherInfo* WeatherListInfo::getItem(int nIndex)
{
	ASSERT( nIndex < (int)m_vWeather.size());
	return m_vWeather.at(nIndex);
}

WeatherInfo* WeatherListInfo::getItemByCondition(WEATHERCONDITION emWeather)
{
	std::vector<WeatherInfo*>::iterator it = m_vWeather.begin();
	for (; it != m_vWeather.end(); ++it)
	{
		if (emWeather == (*it)->getWeatherCondition())
		{
			return (*it);
		}
	}
	return NULL;
}

void WeatherListInfo::SaveData(int nProjID)
{
	std::vector<WeatherInfo*>::iterator it = m_vWeather.begin();
	for (; it != m_vWeather.end(); ++it)
	{
		(*it)->SaveData(nProjID);
	}

	it = m_vDelWeather.begin();
	for (; it != m_vDelWeather.end(); ++it)
	{
		(*it)->DelData();
		delete (*it);
	}
	m_vDelWeather.clear();
}

void WeatherListInfo::ReadData(int nProjID)
{
	CString strSQL = _T("");

	strSQL.Format(_T("SELECT ID,WEATHER,PROJID FROM WEATHERINFO WHERE (PROJID = %d)"),nProjID);
	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		WeatherInfo* pWeatherInfo = new WeatherInfo();
		pWeatherInfo->ReadData(adoRecordset);

		m_vWeather.push_back(pWeatherInfo);
		adoRecordset.MoveNextData();

		if (adoRecordset.IsEOF())
		{
			return ;
		}
	}
	InitNewData(VFR_NONFREEZINGPRECP);
	InitNewData(VFR_FREEZINGDRIZZLE);
	InitNewData(IFR_NONFREEZINGPRECP);
	InitNewData(IFR_FREEZINGDRIZZLE);
	InitNewData(IFR_LIGHTSONW);
	InitNewData(IFR_MEDIUMSNOW);
	InitNewData(IFR_HEAVYSONW);
	InitNewData(CAT_NONFREEZINGPRECP);
	InitNewData(CAT_FREEZINGDRIZZLE);
	InitNewData(CAT_LIGHTSONW);
	InitNewData(CAT_MEDIUMSNOW);
	InitNewData(CAT_HEAVYSONW);

	SaveData(nProjID);
}

void WeatherListInfo::InitNewData(WEATHERCONDITION emWeather)
{
	WeatherInfo* pWeather = new WeatherInfo();
	pWeather->setWeatherCondition(emWeather);
	pWeather->InitNewData();
	m_vWeather.push_back(pWeather);	
}