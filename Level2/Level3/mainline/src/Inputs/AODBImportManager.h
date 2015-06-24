#pragma once

#include "AODBScheduleFile.h"
#include "AODBRosterFile.h"
#include "AODBFieldMapFile.h"
#include "AODBException.h"
#include "Common\ALTObjectID.h"
#include "AODBConvertParameter.h"

class InputTerminal;




class AODBImportManager
{
public:



public:
	AODBImportManager(void);
	~AODBImportManager(void);

public:
	void Init(const CString& strScheduleFile, const CString& strRosterFile);

	void OpenMapFile(const CString& strMapFile);


	void Convert(InputTerminal *pTerminal, int nProjectID);

	bool Inport(InputTerminal *pTerminal);

	//getdata
	AODB::ScheduleFile& GetScheduleFile() {return m_scheduleFile;}
	AODB::RosterFile& GetRosterFile() {return m_rosterFile;}
	AODB::FieldMapFile& GetMapFile() {return m_mapFile;}
	AODB::ConvertParameter GetConverParameter() {return m_crtParam;}

	//set data
	void SetMapFile(const AODB::FieldMapFile& mapFile){m_mapFile = mapFile;}

	bool SetStartTime(const COleDateTime& time);
protected:
	AODB::ScheduleFile	m_scheduleFile;
	AODB::RosterFile	m_rosterFile;
	AODB::FieldMapFile	m_mapFile;
    
	AODB::ConvertParameter m_crtParam;
};
