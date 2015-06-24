#pragma once
#include "../Common/ProjectManager.h"

class CTermPlanDoc;
class CAirportDatabase;
class HandleAirportDatabase
{
public:
	static bool OpenProjectDBForNewProject(CTermPlanDoc* pDoc,CAirportDatabase* _database);
	static bool openGlobalDB(CTermPlanDoc* pDoc);
	static CAirportDatabase* OpenProjectDBForInitNewProject(const CString& sAirportName);

protected:
	static bool setupProjectDatabase(PROJECTINFO* _pinfo);
	static bool loadOldDatabaseFroNewProject(CTermPlanDoc* pDoc,CAirportDatabase* pAirportDatabase);
	static void CopyAircraftFile(CString _path,CString courseDir);
	static bool openOldVersionGlobalDatabase(CTermPlanDoc* pDoc);
};