#include "stdafx.h"
#include <vector>
#include "commondll.h"
#include "assert.h"
//#include ".\Inputs\flight.h"
#include "common\airportdatabase.h"
#include <algorithm>
#include "FlightManager.h"
#include "template.h"
#include "../Database/ARCportADODatabase.h"
#include "../Common/elaptime.h"


static const CString _strFlightGroupName = "FlightGroup.frp";
//////////////////////////////////////////////////////////////////////////////////

FlightGroup::CFlightGroupFilter::CFlightGroupFilter()
{

}

FlightGroup::CFlightGroupFilter::~CFlightGroupFilter()
{


}
void FlightGroup::CFlightGroupFilter::SetFilter(const CString& strFilter)
{
	ParserFilter(strFilter);
}
CString FlightGroup::CFlightGroupFilter::GetFilter()
{
	return m_strAirlineFilter + _T("-") + m_strADIDFilter + _T("-") + m_strDay + _T("-") + m_startTime + _T("-") + m_endTime + _T("-") + m_minTurnaroundTime + _T("-") + m_maxTurnaroundTime;
}

bool FlightGroup::CFlightGroupFilter::operator ==(const CFlightGroupFilter& fliter)
{
	return (m_strAirlineFilter.CompareNoCase(fliter.m_strAirlineFilter) == 0 &&\
		m_strADIDFilter.CompareNoCase( fliter.m_strADIDFilter) == 0 && \
		m_strDay.CompareNoCase( fliter.m_strDay) == 0 &&\
		m_startTime == fliter.m_startTime && \
		m_endTime == fliter.m_endTime&& \
		m_minTurnaroundTime == fliter.m_minTurnaroundTime&& \
		m_maxTurnaroundTime == fliter.m_maxTurnaroundTime);
}

void FlightGroup::CFlightGroupFilter::ParserFilter(const CString&  strFormatFilter)
{
	CString strFilter = strFormatFilter;
	
	strFilter.Trim();
	int nPos = strFilter.Find(_T("-"));
	if(nPos >= 0)
	{
		m_strAirlineFilter = strFilter.Left(nPos);
		strFilter = strFilter.Right(strFilter.GetLength() - nPos - 1);
		nPos = strFilter.Find(_T("-"));
		if (nPos >= 0)
		{
			m_strADIDFilter = strFilter.Left(nPos);
			strFilter = strFilter.Right(strFilter.GetLength() - nPos - 1);
			nPos = strFilter.Find(_T("-"));
			if (nPos >= 0)
			{
				m_strDay = strFilter.Left(nPos);
				strFilter = strFilter.Right(strFilter.GetLength() - nPos - 1);
				nPos = strFilter.Find(_T("-"));
				if (nPos >= 0)
				{
					m_startTime = strFilter.Left(nPos);
					strFilter = strFilter.Right(strFilter.GetLength() - nPos - 1);
					nPos = strFilter.Find(_T("-"));
					if (nPos >= 0)
					{
						m_endTime = strFilter.Left(nPos);
						strFilter = strFilter.Right(strFilter.GetLength() - nPos - 1);
						nPos = strFilter.Find(_T("-"));
						if (nPos >= 0)
						{
							m_minTurnaroundTime = strFilter.Left(nPos);
							m_maxTurnaroundTime = strFilter.Right(strFilter.GetLength() - nPos - 1);
						}
						else
						{
							m_minTurnaroundTime = _T("*");
							m_maxTurnaroundTime = _T("*");
						}
					}
					else
					{
						m_endTime = strFilter;
						m_minTurnaroundTime = _T("*");
						m_maxTurnaroundTime = _T("*");
					}
				}
				else
				{
					m_startTime = _T("*");
					m_endTime = _T("*");
					m_minTurnaroundTime = _T("*");
					m_maxTurnaroundTime = _T("*");
				}
			}
			else
			{
				m_strDay = strFilter;
				m_startTime = _T("*");
				m_endTime = _T("*");
				m_minTurnaroundTime = _T("*");
				m_maxTurnaroundTime = _T("*");
			}
		}
		else
		{
			m_strADIDFilter = strFilter;
			m_strDay = _T("*");
			m_startTime = _T("*");
			m_endTime = _T("*");
			m_minTurnaroundTime = _T("*");
			m_maxTurnaroundTime = _T("*");
		}
	}
}
void FlightGroup::Clear()
{
	for (unsigned i=0;i<m_vFilter.size();i++)
	{
		delete m_vFilter[i];
	}
	m_vFilter.clear();

}
bool FlightGroup::CFlightGroupFilter::containFlt( const char* pstrAirline ,const char* pstrFltID ,const char* pstrFltDay)
{
	char strAirlineFilter[AIRLINE_LEN];
	strcpy( strAirlineFilter, m_strAirlineFilter.GetBuffer() );
	
	char strFltIDFilter[FLIGHT_LEN];
	strcpy( strFltIDFilter, m_strADIDFilter.GetBuffer() );

	char strFltDay[128];
	strcpy(strFltDay,m_strDay.GetBuffer());

	if(Filter(pstrAirline,strAirlineFilter) && Filter(pstrFltID,strFltIDFilter) && Filter(pstrFltDay,strFltDay))
		return true;

	
	return false;
}

bool FlightGroup::CFlightGroupFilter::containFlt(const char* pstrAirline ,const char* pstrFltID)
{
	char strAirlineFilter[AIRLINE_LEN];
	strcpy( strAirlineFilter, m_strAirlineFilter.GetBuffer() );

	char strFltIDFilter[FLIGHT_LEN];
	strcpy( strFltIDFilter, m_strADIDFilter.GetBuffer() );

	if(Filter(pstrAirline,strAirlineFilter) && Filter(pstrFltID,strFltIDFilter))
		return true;


	return false;
}
bool FlightGroup::CFlightGroupFilter::Filter( const char* pstrData ,char* pstrFilter )
{

	// replace wildcards
	for (int j = 0; pstrFilter[j] && pstrData[j]; j++)
	{
		if (pstrFilter[j] == '?')
			pstrFilter[j] = pstrData[j];
		else if (pstrFilter[j] == '*')
			strcpy (pstrFilter + j, pstrData + j);
	}
	if( !_strcmpi (pstrData, pstrFilter) )
		return true;

	return false;
}
bool FlightGroup::CFlightGroupFilter::timeWithInTurnaround( const ElapsedTime& tTime )
{
	ElapsedTime flightTime;
	flightTime.SetHour(tTime.GetHour());
	flightTime.SetMinute(tTime.GetMinute());

	//start time is * and end time is *
	if (m_minTurnaroundTime == _T("*") && m_maxTurnaroundTime == _T("*"))
	{
		return true;
	}

	//start time isn't * and end time is *
	if (m_minTurnaroundTime != _T("*") && m_maxTurnaroundTime == _T("*"))
	{
		ElapsedTime tStartTime;
		tStartTime.SetTime(m_minTurnaroundTime);

		if (flightTime < tStartTime)
			return false;
	}

	// start time is * and end time isn't *
	if (m_minTurnaroundTime == _T("*") && m_maxTurnaroundTime != _T("*"))
	{
		ElapsedTime tEndTime;
		tEndTime.SetTime(m_maxTurnaroundTime);

		if (flightTime >= tEndTime)
			return false;
	}

	//start time isn't * and end time isn't *
	if (m_minTurnaroundTime != _T("*") && m_maxTurnaroundTime != _T("*"))
	{
		ElapsedTime tStartTime;
		tStartTime.SetTime(m_minTurnaroundTime);

		if (flightTime < tStartTime)
			return false;

		ElapsedTime tEndTime;
		tEndTime.SetTime(m_maxTurnaroundTime);

		if (flightTime >= tEndTime)
			return false;
	}


	return true;
}

bool FlightGroup::CFlightGroupFilter::timeWithIn( const ElapsedTime& tTime )
{
	ElapsedTime flightTime;
	flightTime.SetHour(tTime.GetHour());
	flightTime.SetMinute(tTime.GetMinute());

	//start time is * and end time is *
	if (m_startTime == _T("*") && m_endTime == _T("*"))
	{
		return true;
	}

	//start time isn't * and end time is *
	if (m_startTime != _T("*") && m_endTime == _T("*"))
	{
		ElapsedTime tStartTime;
		tStartTime.SetTime(m_startTime);

		if (flightTime < tStartTime)
			return false;
	}

	// start time is * and end time isn't *
	if (m_startTime == _T("*") && m_endTime != _T("*"))
	{
		ElapsedTime tEndTime;
		tEndTime.SetTime(m_endTime);

		if (flightTime >= tEndTime)
			return false;
	}

	//start time isn't * and end time isn't *
	if (m_startTime != _T("*") && m_endTime != _T("*"))
	{
		ElapsedTime tStartTime;
		tStartTime.SetTime(m_startTime);

		if (flightTime < tStartTime)
			return false;

		ElapsedTime tEndTime;
		tEndTime.SetTime(m_endTime);

		if (flightTime >= tEndTime)
			return false;
	}


	return true;
}

const CString& FlightGroup::CFlightGroupFilter::GetStartTime() const
{
	return m_startTime;
}

void FlightGroup::CFlightGroupFilter::SetStartTime( const CString& strStartTime )
{
	m_startTime = strStartTime;
}

const CString& FlightGroup::CFlightGroupFilter::GetEndTime() const
{
	return m_endTime;
}

void FlightGroup::CFlightGroupFilter::SetEndTime( const CString& strEndTime )
{
	m_endTime = strEndTime;
}

void FlightGroup::CFlightGroupFilter::SetAirline( const CString& strAirline )
{
	m_strAirlineFilter = strAirline;
}

const CString& FlightGroup::CFlightGroupFilter::GetAirline() const
{
	return m_strAirlineFilter;
}

void FlightGroup::CFlightGroupFilter::SetFlightID( const CString& strFlightID )
{
	m_strADIDFilter = strFlightID;
}

const CString& FlightGroup::CFlightGroupFilter::GetFlightID() const
{
	return m_strADIDFilter;
}

void FlightGroup::CFlightGroupFilter::SetDay( const CString& strDay )
{
	m_strDay = strDay;
}

const CString& FlightGroup::CFlightGroupFilter::GetDay() const
{
	return m_strDay;
}

const CString& FlightGroup::CFlightGroupFilter::GetMinTurnaround() const
{
	return m_minTurnaroundTime;
}

void FlightGroup::CFlightGroupFilter::SetMinTurnaround( const CString& strMinTime )
{
	m_minTurnaroundTime = strMinTime;
}

const CString& FlightGroup::CFlightGroupFilter::GetMaxTurnaround() const
{
	return m_maxTurnaroundTime;
}

void FlightGroup::CFlightGroupFilter::SetMaxTurnaround( const CString& strMaxTime )
{
	m_maxTurnaroundTime = strMaxTime;
}

//bool FlightGroup::containFlt( Flight* pFlight, bool bArrival )
//{
//	//airline
//	CString strAirline;
//	pFlight->getAirline(strAirline.GetBuffer(1024));
//	strAirline.ReleaseBuffer();
//	CString strFlightID;
//	if (bArrival)
//	{
//		pFlight->getArrID(strFlightID.GetBuffer(1024));
//	}
//	else
//	{
//		pFlight->getDepID(strFlightID.GetBuffer(1024));
//	}
//	
//	strFlightID.ReleaseBuffer();
//	return contain(strAirline,strFlightID);
//	//flight id
//
//
//}
/////class CFlightGroup//////////////////////////////////////////////////////////
FlightGroup::FlightGroup():m_ID(-1)
{
}

FlightGroup::FlightGroup(const FlightGroup& flightGroup)
{
	m_ID = -1;
	m_strGroupName = flightGroup.m_strGroupName;

	m_vFilter.clear();
	for (int i = 0; i < (int)flightGroup.GetFilterCount(); i++)
	{
		FlightGroup::CFlightGroupFilter* pFliter = new FlightGroup::CFlightGroupFilter(*(flightGroup.GetFilterByIndex(i)));
		m_vFilter.push_back(pFliter);
	}
}

FlightGroup::~FlightGroup()
{
	for (unsigned i=0;i<m_vFilter.size();i++)
	{
		delete m_vFilter[i];
	}
	m_vFilter.clear();
}

void FlightGroup::AddFilter(CFlightGroupFilter* groupFilter)
{
	m_vFilter.push_back(groupFilter);
}

void FlightGroup::DelFilter(CFlightGroupFilter* groupFilter)
{
	for(size_t nFilter = 0; nFilter < m_vFilter.size();++nFilter)
	{
		if (m_vFilter[nFilter] == groupFilter)
		{
			delete m_vFilter[nFilter];
			m_vFilter.erase(m_vFilter.begin() + nFilter);
			break;
		}
	}
}
size_t FlightGroup::GetFilterCount()const
{
	return m_vFilter.size();
}
FlightGroup::CFlightGroupFilter* FlightGroup::GetFilterByIndex(size_t nIndex)const
{
	if (nIndex < m_vFilter.size())
	{
		return m_vFilter.at(nIndex);
	}

	return NULL;
}

bool FlightGroup::contain( const char* pstrAirline ,const char* pstrFltID ,const char* pstrFltDay)
{
	for (size_t nFilter = 0; nFilter < m_vFilter.size();++nFilter )
	{
		if (m_vFilter[nFilter]->containFlt(pstrAirline,pstrFltID,pstrFltDay))
			return true;
	}

	return false;
}

bool FlightGroup::contain( const char* pstrAirline ,const char* pstrFltID)
{
	for (size_t nFilter = 0; nFilter < m_vFilter.size(); ++nFilter)
	{
		if (m_vFilter[nFilter]->containFlt(pstrAirline,pstrFltID))
		{
			return true;
		}
	}
	return false;
}

bool FlightGroup::contain( const char* pstrAirline ,const char* pstrFltID,const char* pstrFltDay,const ElapsedTime& tTime,const ElapsedTime& tTurnaroundTime )
{
	for (size_t nFilter = 0; nFilter < m_vFilter.size();++nFilter )
	{
		if (m_vFilter[nFilter]->containFlt(pstrAirline,pstrFltID,pstrFltDay))
		{
			if (m_vFilter[nFilter]->timeWithIn(tTime) && m_vFilter[nFilter]->timeWithInTurnaround(tTurnaroundTime))
			{
				return true;
			}
		}
	}

	return false;
}

// new version database read and save 
bool FlightGroup::loadDatabase(CADORecordset& aodRecordset,CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	if (!aodRecordset.IsEOF())
	{
		aodRecordset.GetFieldValue(_T("GROUP_NAME"),m_strGroupName);
		aodRecordset.GetFieldValue(_T("ID") ,m_ID);

		loadGroupFilter(pAirportDatabase);
	}
	return true;
}

bool FlightGroup::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	if (m_ID == -1)
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_FILGHT_MANAGER (GROUP_NAME) VALUES('%s')"),m_strGroupName);
		m_ID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
	}
	else
	{
		updateDatabase(pAirportDatabase);
	}

	saveGroupFilter(pAirportDatabase);
	return true;
}

bool FlightGroup::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_FILGHT_MANAGER WHERE ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool FlightGroup::updateDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("UPDATE TB_AIRPORTDB_FILGHT_MANAGER SET GROUP_NAME = '%s' WHERE ID = %d"),m_strGroupName,m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());

	return true;
}

bool FlightGroup::loadGroupFilter(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_FILGHT_MANAGER_FLITER WHERE FLIGHTMANAGER_ID = %d"),m_ID);
	long lCount = 0;
	CADORecordset adoRecordset;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordset,pAirportDatabase->GetAirportConnection());

		CString strValue(_T(""));
		FlightGroup::CFlightGroupFilter* pGroupFilter = NULL;
		std::vector<CString>vGroupFilter;// filter duplicates
		std::vector<int>vGroupFilterID;
		vGroupFilter.clear();
		vGroupFilterID.clear();
		while (!adoRecordset.IsEOF())
		{
			pGroupFilter = new FlightGroup::CFlightGroupFilter ;
			int nID = -1;
			adoRecordset.GetFieldValue(_T("ID"),nID);
			adoRecordset.GetFieldValue(_T("FLITER_NAME"),strValue) ;
			pGroupFilter->SetFilter(strValue) ;

			if (std::find(vGroupFilter.begin(),vGroupFilter.end(),strValue) == vGroupFilter.end())
			{
				vGroupFilter.push_back(strValue);
				AddFilter(pGroupFilter);
			}
			else
			{
				vGroupFilterID.push_back(nID);
			}

			adoRecordset.MoveNextData() ;
		}


		for (size_t i = 0; i < vGroupFilterID.size(); i++)
		{
			strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_FILGHT_MANAGER_FLITER WHERE ID = %d"),vGroupFilterID[i]);
			CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return false;
	}
	return true;
}

bool FlightGroup::saveGroupFilter(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	deleteGroupFilter(pAirportDatabase);
	size_t nFilterCount = GetFilterCount();
	CString strSQL(_T(""));
	for (size_t nFilter = 0; nFilter < nFilterCount; ++nFilter)
	{
		CString strFilter = GetFilterByIndex(nFilter)->GetFilter();
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_FILGHT_MANAGER_FLITER (FLITER_NAME,FLIGHTMANAGER_ID) VALUES('%s',%d)"),strFilter,m_ID);
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	}
	return true;
}

bool FlightGroup::deleteGroupFilter(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_FILGHT_MANAGER_FLITER WHERE FLIGHTMANAGER_ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}
///////////////////////////////////////////////////////////////////////////////
/////class CFlightManager//////////////////////////////////////////////////////

FlightManager::FlightManager()
:m_vGroups(NULL)
{

}

FlightManager::~FlightManager()
{
	for (unsigned i=0;i<m_vGroups.size();i++)
	{
		delete m_vGroups[i];
	}
	m_vGroups.clear();
	ClearDelGroup() ;
}
void FlightManager::Clear()
{
	m_DelvGroup.insert(m_DelvGroup.end(),m_vGroups.begin(),m_vGroups.end());

	m_vGroups.clear();
}
void FlightManager::ClearDelGroup()
{
	for (unsigned i=0;i<m_DelvGroup.size();i++)
	{
		delete m_DelvGroup[i];
	}
	m_DelvGroup.clear();
}
void FlightManager::ResetAllID()
{
	for (unsigned i=0;i<m_vGroups.size();i++)
	{
		 m_vGroups[i]->SetID(-1);
	}
	
}
int FlightManager::findGroupItem(FlightGroup* pGroup)
{
	std::vector<FlightGroup*>::iterator it = m_vGroups.begin();
	int nIndex = 0;
	for (;it != m_vGroups.end();++it)
	{
		if (pGroup->getGroupName().CompareNoCase((*it)->getGroupName()))
		{
			return nIndex;
		}
		nIndex++;
	}
	return -1;
}

BOOL FlightManager::LoadData(const CString& _strDBPath)
{
	m_strDBPath = _strDBPath;

	CFile* pFile = NULL;
	CString sFileName = _strDBPath + "\\" + _strFlightGroupName;
	BOOL bRet = TRUE;
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
	//	char line[10240];
		std::string line;
		CString csLine;
		ar.ReadString( csLine );
		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
//		strcpy( line, csLine );
		line = csLine;
//		if(stricmp(line, "GROUPS DATABASE") == 0)
		if (line == _T("GROUPS DATABASE"))
		{
			//skip the column names
			ar.ReadString( csLine );
			//read the values
			//read a line
			ar.ReadString( csLine );
			csLine.MakeUpper();
//			strcpy( line, csLine );
			line = csLine;
	//		while( *line != '\0')
			while(line.length() != 0)
			{
				
				CString strGroup = CString(csLine);
				strGroup.Trim();
				
			
				FlightGroup *pCurGroup = new FlightGroup();
				


				//parser group string
				int nCommaPos = -1;
				nCommaPos = strGroup.Find(_T(","));
				CString strGroupName = _T("");
				if (nCommaPos < 0)
					strGroupName = strGroup;
				else
				    strGroupName = strGroup.Left(nCommaPos);
				pCurGroup->setGroupName(strGroupName);
				
				strGroup = strGroup.Right(strGroup.GetLength() - nCommaPos);
				strGroup.TrimLeft(_T(","));
				while(( nCommaPos =strGroup.Find(_T(","))) >  0)
				{
					CString strFilter = strGroup.Left(nCommaPos);
					FlightGroup::CFlightGroupFilter *pFilter = new  FlightGroup::CFlightGroupFilter();
					if(strFilter.Find(_T("-")) >= 0)
					{
						pFilter->SetFilter(strFilter);
						pCurGroup->AddFilter(pFilter);	
					}
			
					strGroup = strGroup.Right(strGroup.GetLength() - nCommaPos);
					strGroup.TrimLeft(_T(","));

				}
				//the last one
				CString strFilter = strGroup;
				FlightGroup::CFlightGroupFilter *pFilter = new  FlightGroup::CFlightGroupFilter();
				if(strFilter.Find(_T("-")) >= 0)
				{			
					pFilter->SetFilter(strFilter);
					pCurGroup->AddFilter(pFilter);					
				}

				m_vGroups.push_back(pCurGroup);

				ar.ReadString( csLine );
				csLine.MakeUpper();
				line = csLine;
			}
		}
		else
			bRet = FALSE;
		ar.Close();
		delete pFile;
	}
	catch(CException* e) {
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}
	return bRet;
}

void FlightManager::AddFlightGroup(FlightGroup* pGroup)
{
	m_vGroups.push_back(pGroup);
}

void FlightManager::DelItem(FlightGroup* pGroup)
{
	std::vector<FlightGroup*>::iterator it = m_vGroups.begin();
	for (;it != m_vGroups.end();++it)
	{
		if (pGroup->getGroupName().CompareNoCase((*it)->getGroupName())==0)
		{
			m_DelvGroup.push_back(*it) ;
			m_vGroups.erase(it);
			break;
		}
	}
}


BOOL FlightManager::CheckFlightGroup(FlightGroup* pGroup)
{
	std::vector<FlightGroup*>::iterator it = m_vGroups.begin();
	for(; it != m_vGroups.end();++it)
	{
		if (pGroup->getGroupName().CompareNoCase((*it)->getGroupName()) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

int FlightManager::findFlightGroupItem(FlightGroup* pGroup)
{
	int nCount = m_vGroups.size();
	for( int i=0; i<nCount; i++ )
	{
		if( m_vGroups[i]->getGroupName().CompareNoCase(pGroup->getGroupName()) == 0)
			return i;
	}
	return INT_MAX;
}
FlightManager* _g_GetActiveFlightMan(CAirportDatabase * pAirportDB)
{
	assert(pAirportDB);
	return pAirportDB->getFlightMan();
}

bool FlightManager::loadDatabase(CAirportDatabase* pAirportDatabase)
{
	Clear();
	
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_FILGHT_MANAGER"));
	CADORecordset adoRecordset;
	long lCount = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordset,pAirportDatabase->GetAirportConnection()) ;

		while(!adoRecordset.IsEOF())
		{
			CString strValue(_T(""));
			int nID = -1;
			FlightGroup* pFlightGroup = new FlightGroup;
			pFlightGroup->loadDatabase(adoRecordset,pAirportDatabase);
			m_vGroups.push_back(pFlightGroup);
			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}
	return true;
}

bool FlightManager::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	FLIGHTGROUPLIST::iterator iter = m_vGroups.begin();
	for (; iter != m_vGroups.end(); ++iter)
	{
		(*iter)->saveDatabase(pAirportDatabase);
	}

	iter = m_DelvGroup.begin();
	for (; iter != m_DelvGroup.end(); ++iter)
	{
		(*iter)->deleteDatabase(pAirportDatabase);
	}
	ClearDelGroup();
	return true;
}

bool FlightManager::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	//FlightGroup::deleteDatabase(pAirportDatabase);

	FLIGHTGROUPLIST::iterator iter = m_vGroups.end();
	for (; iter != m_vGroups.end(); ++iter)
	{
		(*iter)->deleteDatabase(pAirportDatabase);
		(*iter)->deleteGroupFilter(pAirportDatabase);
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
//the inerface for DB
//////////////////////////////////////////////////////////////////////////
void FlightManager::WriteDataToDB(int _airportDBID,DATABASESOURCE_TYPE type )
{

		FlightGroup::WriteDataToDB(m_vGroups,_airportDBID,type) ;
		FlightGroup::DeleteDataFromDB(m_DelvGroup,_airportDBID,type) ;
	ClearDelGroup() ;
	
}
void FlightManager::ReadDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type )
{
	Clear();
	FlightGroup::ReadDataFromDB(m_vGroups,_airportDBID,type) ;
}
void FlightManager::DeleteAllData(int _airportDBID,DATABASESOURCE_TYPE type /* = DATABASESOURCE_TYPE_ACCESS */)
{
	FlightGroup::DeleteAllData(_airportDBID,type) ;
}
void FlightGroup::DeleteAllData(int _airportDBID,DATABASESOURCE_TYPE type)
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_FILGHT_MANAGER WHERE AIRPORTDB_ID = %d"),_airportDBID) ;
	try
	{
		CADODatabase::BeginTransaction(type) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
		CADODatabase::CommitTransaction(type) ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation(type);
		e.ErrorMessage() ;
		return ;
	}
}
void FlightGroup::ReadDataFromDB(FLIGHTGROUPLIST& _dataSet,int _airportDBID,DATABASESOURCE_TYPE type )
{
	if(_airportDBID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_FILGHT_MANAGER WHERE AIRPORTDB_ID = %d") , _airportDBID) ;
	CADORecordset recordset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,type) ;

		CString str ;
		FlightGroup* pFlightGroup = NULL ;
		int id = 0 ;
		while(!recordset.IsEOF())
		{
			pFlightGroup = new FlightGroup ;
			recordset.GetFieldValue(_T("GROUP_NAME"),str) ;
			pFlightGroup->setGroupName(str) ;
			recordset.GetFieldValue(_T("ID") , id) ;
			pFlightGroup->SetID(id) ;

			FlightGroup::ReadGroupFliter(pFlightGroup,type) ;

			_dataSet.push_back(pFlightGroup) ;
			recordset.MoveNextData() ;
		}
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}
void FlightGroup::WriteDataToDB(FLIGHTGROUPLIST& _dataSet,int _airportDBID,DATABASESOURCE_TYPE type )
{
	if(_airportDBID == -1)
		return ;
	FLIGHTGROUPLIST::iterator iter = _dataSet.begin() ;
	for ( ; iter != _dataSet.end() ;iter++)
	{
        if((*iter)->GetID() == -1)
			FlightGroup::WriteData(*iter,_airportDBID,type) ;
		else
			FlightGroup::UpdateData(*iter,_airportDBID,type) ;
		FlightGroup::WriteGroupFliter(*iter,type) ;
	}
}
void FlightGroup::DeleteDataFromDB(FLIGHTGROUPLIST& _dataSet,int _airportDBID,DATABASESOURCE_TYPE type )
{
	if(_airportDBID == -1)
		return ;
	FLIGHTGROUPLIST::iterator iter = _dataSet.begin() ;
	for ( ; iter != _dataSet.end() ;iter++)
	{
		FlightGroup::DeleteData(*iter,_airportDBID,type) ;
	}
}
void FlightGroup::DeleteData(FlightGroup* _group,int _airportDBID,DATABASESOURCE_TYPE type )
{
	if(_group == NULL || _group->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_FILGHT_MANAGER WHERE ID = %d"),_group->GetID()) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;

}
void FlightGroup::WriteData(FlightGroup* _group,int _airportDBID,DATABASESOURCE_TYPE type )
{
   if(_group == NULL || _airportDBID == -1)
	   return ;
   CString SQL ;
   SQL.Format(_T("INSERT INTO TB_AIRPORTDB_FILGHT_MANAGER (GROUP_NAME,AIRPORTDB_ID) VALUES('%s',%d)"),_group->getGroupName(),_airportDBID) ;

	  int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
	  _group->SetID(id) ;
}
void FlightGroup::UpdateData(FlightGroup* _group,int _airportDBID,DATABASESOURCE_TYPE type )
{
	if(_group == NULL || _airportDBID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("UPDATE TB_AIRPORTDB_FILGHT_MANAGER SET GROUP_NAME = '%s' WHERE ID = %d AND AIRPORTDB_ID = %d"),_group->getGroupName(),_group->GetID(),_airportDBID) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
}
void FlightGroup::ReadGroupFliter(FlightGroup* _group,DATABASESOURCE_TYPE type  )
{
	if(_group == NULL || _group->GetID() == -1)
		return ;
    CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_FILGHT_MANAGER_FLITER WHERE FLIGHTMANAGER_ID = %d"),_group->GetID()) ;
	long count = 0 ;
	CADORecordset recordset ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,type) ;
		
		CString str ;
		FlightGroup::CFlightGroupFilter* GroupFilter ;
		std::vector<CString>vGroupFilter;// filter duplicates
		std::vector<int>vGroupFilterID;
		vGroupFilter.clear();
		vGroupFilterID.clear();
		while (!recordset.IsEOF())
		{
			GroupFilter = new FlightGroup::CFlightGroupFilter ;
			int nID = -1;
			recordset.GetFieldValue(_T("ID"),nID);
			recordset.GetFieldValue(_T("FLITER_NAME"),str) ;
			GroupFilter->SetFilter(str) ;

			if (std::find(vGroupFilter.begin(),vGroupFilter.end(),str) == vGroupFilter.end())
			{
				vGroupFilter.push_back(str);
				_group->AddFilter(GroupFilter);
			}
			else
			{
				vGroupFilterID.push_back(nID);
			}

			recordset.MoveNextData() ;
		}


		for (size_t i = 0; i < vGroupFilterID.size(); i++)
		{
			CString SQL(_T(""));
			SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_FILGHT_MANAGER_FLITER WHERE ID = %d"),vGroupFilterID[i]);
			CADODatabase::ExecuteSQLStatement(SQL,type);
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
}
void FlightGroup::WriteGroupFliter(FlightGroup* _group ,DATABASESOURCE_TYPE type )
{
	if(_group == NULL || _group->GetID() == -1)
		return ;
    CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_FILGHT_MANAGER_FLITER WHERE FLIGHTMANAGER_ID = %d"),_group->GetID()) ;
    CADODatabase::ExecuteSQLStatement(SQL,type) ;

		size_t nFilterCount = _group->GetFilterCount();
		for (size_t nFilter = 0; nFilter < nFilterCount; ++nFilter)
		{
			CString strFilter = _group->GetFilterByIndex(nFilter)->GetFilter();
			SQL.Format(_T("INSERT INTO TB_AIRPORTDB_FILGHT_MANAGER_FLITER (FLITER_NAME,FLIGHTMANAGER_ID) VALUES('%s',%d)"),strFilter,_group->GetID()) ;
			CADODatabase::ExecuteSQLStatement(SQL,type) ;
		}
}















//////////////////////////////////////////////////////////////////////////