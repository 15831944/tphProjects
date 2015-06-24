#include "StdAfx.h"
#include ".\aodbimportmanager.h"
#include "../InputAirside/ALTObject.h"
#include "InputAirside/InputAirside.h"
#include "AODBConvertParameter.h"


AODBImportManager::AODBImportManager(void)
{
}

AODBImportManager::~AODBImportManager(void)
{
}

void AODBImportManager::Init( const CString& strScheduleFile, const CString& strRosterFile )
{
	m_scheduleFile.setFileName(strScheduleFile);
	m_rosterFile.setFileName(strRosterFile);

}

void AODBImportManager::OpenMapFile( const CString& strMapFile )
{
	m_mapFile.setFileName(strMapFile);
	m_mapFile.Read();
	
	//convert schedule
	m_scheduleFile.Convert(m_crtParam);


	//convert Roster
	m_rosterFile.Convert(m_crtParam);
}

bool AODBImportManager::SetStartTime(const COleDateTime& time)
{
	COleDateTime checkDate;
	checkDate.SetDate((COleDateTime::GetCurrentTime()).GetYear() + 20,1,1);

	COleDateTime earliestDate; //the start time from the file
	earliestDate = m_scheduleFile.GetStartDate(checkDate);
	earliestDate = m_rosterFile.GetStartDate(earliestDate);

	earliestDate = min(time,earliestDate);

	if (time.m_status == COleDateTime::valid && m_crtParam.oleStartDate != earliestDate)
	{
		m_crtParam.oleStartDate = earliestDate;

		//convert schedule
		m_scheduleFile.Convert(m_crtParam);


		//convert Roster
		m_rosterFile.Convert(m_crtParam);
		
		return true;
	}
	return false;
}

void AODBImportManager::Convert(InputTerminal *pTerminal, int nProjectID)
{
	ASSERT(pTerminal != NULL);
	if(pTerminal == NULL)
	{
		throw AODBException(AODBException::ET_FATEL,"Initialize failed");
	}

	m_scheduleFile.Read();
	m_rosterFile.Read();

	//start data
	COleDateTime checkDate;
	checkDate.SetDate((COleDateTime::GetCurrentTime()).GetYear() + 20,1,1);

	COleDateTime earliestDate; //the start time from the file
	earliestDate = m_scheduleFile.GetStartDate(checkDate);
	earliestDate = m_rosterFile.GetStartDate(earliestDate);

	if(earliestDate == checkDate)
	{
		throw AODBException(AODBException::ET_FATEL,"Cannot find the start date.");
	}
	m_crtParam.oleStartDate = earliestDate;

	std::vector<int> vAirports;
	InputAirside::GetAirportList(nProjectID,vAirports);
	if(vAirports.size() == 0)
	{
		throw AODBException(AODBException::ET_FATEL,"NO airport information.");
	}
	//stand list
	ALTObject::GetObjectNameList(ALT_STAND,vAirports.at(0),m_crtParam.m_vStandList);

	//Map file
	m_crtParam.pTerminal = pTerminal;
	m_crtParam.pMapFile = &m_mapFile;

	
	//convert schedule
	m_scheduleFile.Convert(m_crtParam);


	//convert Roster
	m_rosterFile.Convert(m_crtParam);



	

		
}

bool AODBImportManager::Inport( InputTerminal *pTerminal )
{
	if(m_scheduleFile.ReplaceSchedule(pTerminal) && m_rosterFile.ImportRoster(pTerminal))	
	{
		m_mapFile.Save();
		return true;
	}
	return false;
}
