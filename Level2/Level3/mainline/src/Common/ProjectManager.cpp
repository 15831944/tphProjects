// ProjectManager.cpp: implementation of the CProjectManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProjectManager.h"
#include "DirectoryUtil.h"
#include "exeption.h"
#include "fileman.h"
#include <io.h>
#include <assert.h>
#include "template.h"

#include <Lmcons.h> // for UNLEN constant

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define VERSION_NUMBER 300

CProjectManager* CProjectManager::m_pInstance = NULL;

char * szFileNames[] = {"ARCTERM.PRJ", "FILELIST.TXT", "CURRENT.PRJ",
    "HELPLIST.TXT"};

char * szDirNames[] = {"INPUT", "ECHO", "LOGS", "STATSPEC",
    "REPORTS", "BACKUP", "ECONOMIC", "FILESPEC", "SYSTEM", "UNDO", "COMMENTS", "TEXTURE", "Comparative Report", "Mathematic", "MathResult","LANDSIDE" };

char * landDirNames[] = {"INPUT", "ECHO", "LOGS",};

const static int LAND_DIR_COUNT = (sizeof(landDirNames) / sizeof(landDirNames[0]));
//#define DIR_COUNT   12     
#define DIR_COUNT   (sizeof(szDirNames) / sizeof(szDirNames[0]))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProjectManager::CProjectManager()
{
}

CProjectManager::~CProjectManager()
{
	cleanSwapDrive ();
}

CProjectManager* CProjectManager::GetInstance()
{
	if(m_pInstance == NULL)
		m_pInstance = new CProjectManager();
	return m_pInstance;
}

void CProjectManager::DeleteInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CProjectManager::GetProjectList(std::vector<CString>& vList)
{ //edit by arlee 2010.11.01
	SetCurrentDirectory(m_csRootProjectPath);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile("*", &FindFileData); 
	while(FindNextFile(hFind, &FindFileData))
	{
	
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{ //if this is a directory			
			WIN32_FIND_DATA FindPIData;
			HANDLE hFindPI;
			TCHAR pathPI[MAX_PATH];
			_tcscpy(pathPI, FindFileData.cFileName);
			_tcscat(pathPI, _T("\\projinfo"));
			if(INVALID_HANDLE_VALUE != (hFindPI = FindFirstFile(pathPI, &FindPIData))) 
			{ //if the directory contains file named "projinfo"
				vList.push_back(FindFileData.cFileName);	
			}
			FindClose(hFindPI);//add by tutu 2002-9-5 
		}		
	}
	FindClose(hFind);
}

BOOL CProjectManager::GetProjectInfo(CString projName, PROJECTINFO* pi, const CString& strFolder)
{
	CString projFilePath;
	if (strFolder.IsEmpty())
		projFilePath = m_csRootProjectPath + _T("\\") + projName + _T("\\projinfo");
	else
		projFilePath = strFolder + _T("\\") + projName + _T("\\projinfo");
	CFile* pFile = NULL;
	pi->name = projName;
	if (strFolder.IsEmpty())
		pi->path = m_csRootProjectPath + _T("\\") + projName;
	else
		pi->path = strFolder + _T("\\") + projName;

	try {
		pFile = new CFile(projFilePath, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		char line[513];
		ar.ReadString(line, 512); // read user name line
		pi->user = line;
		ar.ReadString(line, 512); // read machine name line
		pi->machine = line;
		ar.ReadString(line, 512); // read create date
		char* b = line;
		char* p = NULL;
		int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMin = 0, nSec = 0;
		int c = 1;
		while((p = strchr(b, ' ')) != NULL) {
			*p = '\0';
			switch(c)
			{
			case 1: //year
				nYear = atoi(b);
				break;
			case 2: //month
				nMonth = atoi(b);
				break;
			case 3: //day
				nDay = atoi(b);
				break;
			case 4: //hour
				nHour = atoi(b);
				break;
			case 5: //min
				nMin = atoi(b);
				break;
			}
			b = ++p;
			c++;
		}
		nSec = atoi(b);
		CTime t(nYear, nMonth, nDay, nHour, nMin, nSec);
		pi->createtime = t;

		ar.ReadString(line, 512); // read create date
		b = line;
		p = NULL;
		c = 1;
		while((p = strchr(b, ' ')) != NULL) {
			*p = '\0';
			switch(c)
			{
			case 1: //year
				nYear = atoi(b);
				break;
			case 2: //month
				nMonth = atoi(b);
				break;
			case 3: //day
				nDay = atoi(b);
				break;
			case 4: //hour
				nHour = atoi(b);
				break;
			case 5: //min
				nMin = atoi(b);
				break;
			}
			b = ++p;
			c++;
		}
		nSec = atoi(b);
		CTime t2(nYear, nMonth, nDay, nHour, nMin, nSec);
//		// TRACE("%s\n", t2.Format("%Y, %a %b %d"));
		pi->modifytime = t2;

		ar.ReadString(line, 512); // read version
		pi->version = atoi( line );
		
		ar.ReadString(line, 512); // read deleted flag
		pi->isDeleted = atoi( line );
		
		CString _strUniqueIdx;
		if( ar.ReadString( _strUniqueIdx ) ) // read unique index
			pi->lUniqueIndex = atol( _strUniqueIdx );
		else
			pi->lUniqueIndex = 0l;

		if (ar.IsBufferEmpty())
		{
			pi->dbname = "";
		}
		else
		{
			//dbname,add by hans
			ar.ReadString(line, 512);
			if (strlen(line))
			{
				CString strdb = line;
				int nFind = strdb.Find(',');
				if (nFind == -1)
				{
					pi->dbname =line ;
				}
				else
				{
					pi->dbname =strdb.Mid(nFind+1);
				}
			}
			else
			{
				pi->dbname = "";
			}
		}
		
		ar.Close();
		delete pFile;
		return TRUE;		
	}
	catch(CException* e)
	{
		AfxMessageBox("Error reading project info file for project " + projName + ".", MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		return FALSE;
	}
	return TRUE;
}

BOOL CProjectManager::CreateProject(CString _projName, PROJECTINFO* pi,bool _bNeedToCopyDefFile )
{
	TCHAR unbuf[UNLEN+1];
	DWORD nSize = UNLEN;
	GetUserName(unbuf, &nSize);
	pi->user = unbuf;
	TCHAR mnbuf[MAX_COMPUTERNAME_LENGTH + 1];
	nSize = MAX_COMPUTERNAME_LENGTH;
	GetComputerName(mnbuf, &nSize);
	pi->machine = mnbuf;
	pi->createtime = pi->modifytime = CTime::GetCurrentTime();
	pi->version = PROJECT_VERSION;
	pi->isDeleted=0; 
	//pi->lUniqueIndex = 0l;

	CString projName = pi->name = _projName;

	CString projDir = m_csRootProjectPath + _T("\\") + projName;


	// now set env so the old code could be used.
	//m_terminal.loadInputs(0);

	pi->path = projDir;
	if(CreateDirectory(projDir, NULL)) 
	{
	    createSubDirectories( LPCTSTR( projDir ) );
		
		// copy default files to input file directory
		if( _bNeedToCopyDefFile )
		{
			char source[128];
			FileManager fileMan;

			strcpy( source, m_csAppPath );
			strcat( source, "\\" );
			strcat( source, szDirNames[FileSpecsDir]);

			char dest[128];
			strcpy (dest, projDir);
			strcat (dest, "\\");
			strcat (dest, szDirNames[InputDir]);
			if( !fileMan.CopyDirectory (source, dest,NULL) )
				return FALSE;

			char bakdest[128];
			strcpy(bakdest, dest);

			strcat(dest, "\\AirPort1\\AirSide"); // \input\airport1\airside
			if( !fileMan.CopyDirectory (source, dest,NULL) )
				return FALSE;

			strcpy( dest, bakdest);
			strcat(dest, "\\AirPort1\\LandSide"); // \input\airport1\landside
			if( !fileMan.CopyDirectory (source, dest,NULL) )
				return FALSE;
		}

		if(CreateProjInfoFile(*pi))
			return TRUE;
	}
	return FALSE;
}

/*
BOOL CProjectManager::CreateDefaultProject(PROJECTINFO* pi)
{
	TCHAR unbuf[UNLEN+1];
	DWORD nSize = UNLEN;
	GetUserName(unbuf, &nSize);
	pi->user = unbuf;
	TCHAR mnbuf[MAX_COMPUTERNAME_LENGTH + 1];
	nSize = MAX_COMPUTERNAME_LENGTH;
	GetComputerName(mnbuf, &nSize);
	pi->machine = mnbuf;
	pi->createtime = pi->modifytime = CTime::GetCurrentTime();
	pi->version = VERSION_NUMBER;
	CString projName = pi->name = pi->user + pi->createtime.Format("-%m-%d-%y_%H%M%S");

	CString projDir = m_csRootProjectPath + _T("\\") + projName;


	// now set env so the old code could be used.
	//m_terminal.loadInputs(0);

	pi->path = projDir;
	if(CreateDirectory(projDir, NULL)) 
	{
	    createSubDirectories( LPCTSTR( projDir ) );
		
		// copy default files to input file directory
		char source[128];
	    FileManager fileMan;

		strcpy( source, m_csAppPath );
		strcat( source, "\\" );
		strcat( source, szDirNames[FileSpecsDir]);

		char dest[128];
		strcpy (dest, projDir);
		strcat (dest, "\\");
		strcat (dest, szDirNames[InputDir]);

		if( !fileMan.CopyDirectory (source, dest) )
			return FALSE;

		if(CreateProjInfoFile(*pi))
			return TRUE;
	}
	return FALSE;
}
*/


// create subdirectories
void CProjectManager::createSubDirectories ( const char* projectDir)
{
    char dir[_MAX_PATH], bakdir[_MAX_PATH];
    strcpy (dir, projectDir);
    int dirLength = strlen (dir);
    FileManager fileMan;

    for (int i = 0; i < DIR_COUNT; i++)
    {
        strcpy (dir + dirLength, "\\");
        strcat (dir, szDirNames[i]);
        if (!fileMan.IsDirectory (dir))
            if (!fileMan.MakePath (dir))
                throw new TwoStringError ("Unable to create directory ", dir);
		if( InputDir == i)
		{
			//strcat (dir, szDirNames[i]); // "...\input\airport1\"
			strcpy(bakdir,dir);
			strcat(dir,"\\Airside");
			if (!fileMan.IsDirectory (dir))
				if (!fileMan.MakePath (dir))
					throw new TwoStringError ("Unable to create directory ", dir);

			strcpy(dir,bakdir);
			strcat (dir, "\\AirPort1");
			if (!fileMan.IsDirectory (dir))
				if (!fileMan.MakePath (dir))
					throw new TwoStringError ("Unable to create directory ", dir);
			
			strcpy(bakdir, dir);
			strcat (dir, "\\AirSide");// "...\input\airport1\airside"
			if (!fileMan.IsDirectory (dir))
				if (!fileMan.MakePath (dir))
					throw new TwoStringError ("Unable to create directory ", dir);
			
			strcpy(dir, bakdir);
			strcat (dir, "\\LandSide");// "...\input\airport1\landside"
			if (!fileMan.IsDirectory (dir))
				if (!fileMan.MakePath (dir))
					throw new TwoStringError ("Unable to create directory ", dir);
		}		
    }
	//create landside directories
	std::string landdir(projectDir); landdir +="\\LANDSIDE";
	for (int i = 0; i < LAND_DIR_COUNT; i++)
	{
		std::string sublanddir = landdir + "\\"+ landDirNames[i];
		if (!fileMan.IsDirectory (sublanddir.c_str()))
			if (!fileMan.MakePath (sublanddir.c_str()))
				throw new TwoStringError ("Unable to create directory ", dir);
	}
}

BOOL CProjectManager::CreateProjInfoFile(PROJECTINFO pi)
{
	CString projDir = m_csRootProjectPath + _T("\\") + pi.name;
	CString piFilePath = projDir + _T("\\projinfo");
	CFile* pFile = NULL;
	try {
		pFile = new CFile(piFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
		CArchive ar(pFile, CArchive::store);
		ar.WriteString(pi.user + "\n" + pi.machine + "\n");
		ar.WriteString(pi.createtime.Format("%Y %m %d %H %M %S\n"));
		ar.WriteString(pi.modifytime.Format("%Y %m %d %H %M %S\n"));
		CString csStr;
		csStr.Format( "%d\n", pi.version );
		ar.WriteString( csStr );

		csStr.Format( "%d\n", pi.isDeleted );
		ar.WriteString( csStr );
		
		csStr.Format("%ld\n", pi.lUniqueIndex );
		ar.WriteString( csStr );
		//write the dbName
		ar.WriteString(_T("Database,")+pi.dbname);

		ar.Close();
		delete pFile;
		return TRUE;
	}
	catch(CException* e)
	{
		AfxMessageBox("Error writing project info file for project" + pi.name, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		return FALSE;
	}
	return TRUE;
}

BOOL CProjectManager::UpdateProjInfoFile( PROJECTINFO pi )
{
	CString proDir = m_csRootProjectPath + _T("\\") + pi.name;
	CString piFilePath = proDir + _T("\\projinfo");

	// first remove the old project info file
	FileManager fm;
	int _iDelResult = fm.DeleteFile( piFilePath );
	
	// then re create the project info file
	return CreateProjInfoFile( pi ); 
}

BOOL CProjectManager::DeleteProject(PROJECTINFO pi)
{
	CString m_csPath = m_csRootProjectPath + _T("\\") + pi.name;
//	CString m_csPath = m_csRootProjectPath ;

	//return CDirectoryUtil::RemoveDir(m_csPath);
	return CDirectoryUtil::RemoveDirectoryR(m_csPath);
	/*
	if(CDirectoryUtil::RemoveDirectoryR(m_csPath))

		return CDirectoryUtil::RemoveDir(m_csPath);
	else
		return false;
		*/

}


/*
Terminal& CProjectManager::GetTerminal()
{
	return m_terminal;
}
*/



void CProjectManager::SetPath(	const CString& _csAppPath, 
								const CString& _csRootProjectPath,
								const CString& _csAirportPath,
								const CString& _csACTypePath,
								const CString& _csProbDistPath,
								const CString& _csDefaultDBPath)
{
	m_csAppPath			= _csAppPath;
	m_csRootProjectPath = _csRootProjectPath;
	m_csAirportPath		= _csAirportPath;
	m_csACTypePath		= _csACTypePath;
	m_csProbDistPath	= _csProbDistPath;
	m_csDefaultDBPath	= _csDefaultDBPath;
}

int CProjectManager::getFileName( const CString& _csProjPath, int fileType, char *filename, int mode, int airport)
{
	if( AirsideProcessorLayoutFile == fileType || AirsidePlacementsFile == fileType
		|| AirsideFloorsFile == fileType || AirsideContourTreeFile == fileType
		|| AirsideStructureFile == fileType || AirsideAircraftCategory == fileType
		|| AirsideCamerasFile == fileType )
	{
		mode = 2;//EnvMode_AirSide;
		airport = 0;
	}
    if( fileType >= LandsideFloorsFile )
	{
		mode = 0; airport = 0;
	}

	switch( fileType )
	{
    case SpecificFlightPaxFlowFile:
	case StationPaxFlowFile:
	case GatePriorityInfoFile:
	case FlightPriorityInfoFile:
	case ConstraintSpecFile:
	case GateAssignConFile:
	case AdjacencyGateConFile:
	case IntermediateStandAssignFile:
	case BagGateAssignFile:
	case AreasFile:
	case NonPaxRelativePosFile:
	case PeopleMoverFile:
	case PortalsFile:
	case FloorsFile:
	case DefaultDisplayPropertiesFile:
	case PlacementsFile:
	case PaxDisplayPropertiesFile:
	case AircraftDisplayPropertiesFile:
	case PaxTagsFile:
	case ProcTagsFile:
	case FlightTagsFile:
	case ActivityDensityFile:
	case LayerInfoFile:
	case VRInfoFile:
	case RosterRulesFile:
		return getFullName( _csProjPath, InputDir, fileType, filename, mode, airport);
	case CamerasFile:
	case DisplayPropOverridesFile:
	case StationLayoutFile:
	case AllCarsScheduleFile:
	case SubFlowList:
	case MovingSideWalKFile:
	case MovingSideWalkPaxDistFile:
	case PipeDataSetFile:
	case TerminalProcessorTypeSetFile:
	case MobileElemTypeStrDBFile:
	case ProcessorSwitchTimeFile:
	case GateMappingDBFile:
	case ReportParameterFile:
	case SimAndReportFile:
	case MoviesFile:
	case WalkthroughsFile:
	case AirsideWalkthroughFile:
	case LandsideVehicleWalkthroughsFile:
	case VideoParamsFile:
	case UserNameListFile:
	case FlightScheduleCriteriaFile:
	case ResourecPoolFile:
	case ProcessorToResourcePoolFile:
	case ResourceDisplayPropertiesFile:
	case PassengerBulkFile:
	case AirPortInfoFile:
	case AirsideFloorsFile:
	case AirsideContourTreeFile:
	case AirsideStructureFile:
	case TerminalStructureFile:
	case AircraftTagsFile:
	case LandsideFloorsFile:
	case LandsideCamerasFile:
	case LandsideProcessorFile:
	case FlowSyncFile:
		return getFullName( _csProjPath, InputDir, fileType, filename, mode, airport);

	case TrainLogEchoFile:
        return getFullName ( _csProjPath, EchoDir, fileType, filename, mode, airport);

	case TrainDescFile:
	case TrainEventFile:

		return getFullName( _csProjPath, LogsDir, fileType, filename, mode, airport);
	case LandsideOperationTypeFile:
		strcpy(filename,_csProjPath);
		strcat(filename,"\\input\\landsideOperationTypeInfo.txt");
         return true;
	case AircraftDatabaseFile:
		assert( 0 );
		return TRUE;
	case AircraftCategoryFile:
		strcpy(filename, _csProjPath + _T("\\category.cat"));
		return TRUE;

	case ProbabilityDistributionFile:
		strcpy( filename, GetProbDistFilePath() );
		return TRUE;

	case  GlobalDBListFile:
		strcpy( filename, GetGlobalDBListFilePath() );
		return TRUE;
	case LogDebugFile :
		strcpy( filename, GetLogDebugFilePath() );
		return TRUE;
	case HostInfoFile:
	case ProjectInfoFile:
		return getFullName(_csProjPath, ComparativeDir, fileType, filename, mode, airport);

	case ModelInfoFile:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\modelinfo.txt");
		return TRUE;

	case ReportInfoFile:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\reportinfo.txt");
		return TRUE;
	case MMProcessInfoFile:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\Process.txt");
		return TRUE;
	case MMFlowInfoFile:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\Flow.txt");
		return TRUE;

	case MathQTimeResult:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\QTime.txt");
		return TRUE;

	case MathQLengthResult:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\QLength.txt");
		return TRUE;

	case MathThroughputResult:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\Throughput.txt");
		return TRUE;

	case MathPaxCountResult:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\PaxCount.txt");
		return TRUE;

	case MathUtilizationResult:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\Utilization.txt");
		return TRUE;

	case HistogramInputFile:
		strcpy(filename, _csProjPath);
		return TRUE;
	case CongestionParamFile:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\CongestionParam.txt");
		return TRUE;	
	case VehicleDistributionFile:
		strcpy(filename,_csProjPath);
		strcat(filename,"\\input\\Landside\\VEHICLE.DST");
		return TRUE;
	case BridgeConnectorPaxDataFile:
		strcpy(filename, _csProjPath);
		strcat(filename, "\\input\\BCPAXDATA.TXT");
		return TRUE;
	}


	if( fileType >= AirsideProcessorLayoutFile )
	{
		return getFullName ( _csProjPath, InputDir, fileType, filename, mode, airport);
	}
	else if( fileType >= BuildingDeprecCost )	// make sure there is no new file type after economic.
		return getFullName( _csProjPath, EconomicDir, fileType, filename , mode, airport);
    else if (fileType == LogBackupFile)
        return getFullName ( _csProjPath, BackupDir, fileType, filename, mode, airport);
    else if (fileType < PaxDescFile)
        return getFullName ( _csProjPath, InputDir, fileType, filename, mode, airport);
    else if (fileType < PaxLogEchoFile)
        return getFullName ( _csProjPath, LogsDir, fileType, filename, mode, airport);
    else if (fileType < DistanceSpecFile)
        return getFullName ( _csProjPath, EchoDir, fileType, filename, mode, airport);
    else if (fileType < DistanceReportFile)
        return getFullName ( _csProjPath, StatSpecDir, fileType, filename, mode, airport);
    else
        return getFullName ( _csProjPath, ReportDir, fileType, filename, mode, airport);

}


int CProjectManager::getFullName( const CString& _csProjDir,
								 int dir, int fileType, char *filename, int mode, int airport)
{
    if( _csProjDir.IsEmpty() )
        throw new StringError ("No project currently selected");

    char name[64];
    if (!getCurrentFileName ( _csProjDir, fileType, name, mode, airport))
        throw new StringError ("File type not found in environment file");

    if (!_stricmp (name, "NONE"))
        return FALSE;

	if( InputDir == dir)
	{			
		char tmpstr[50];
		strcpy (filename, _csProjDir);
		strcat (filename, "\\");
		strcat (filename, szDirNames[dir]);
		strcat (filename, "\\");

		switch( mode )
		{// mode == enum EnvironmentMode; 
			//	0 = EnvMode_LandSide, 1 = EnvMode_Terminal, 2 = EnvMode_AirSide;
		case 0:
			{
				sprintf(tmpstr, "AirPort%d\\LandSide\\", airport + 1);
				strcat (filename, tmpstr);
				strcat (filename, name);
			}
			break;
		case 1:
			{	
				strcat (filename, name);
			}
			break;
		case 2:
			{
				sprintf(tmpstr, "AirPort%d\\AirSide\\", airport + 1);
				strcat (filename, tmpstr);
				strcat (filename, name);
			}
			break;
		default:
			strcat (filename, name);
			break;
		}
	}
	else
	{
		strcpy (filename, _csProjDir);
		strcat (filename, "\\");
		strcat (filename, szDirNames[dir]);
		strcat (filename, "\\");
		strcat (filename, name);
	}

    return TRUE;
}


int CProjectManager::getCurrentFileName( const CString& _csProjDir, int lineNum, char *str, int mode, int airport)
{
	if( lineNum == AirPortInfoFile)
	{
		strcpy( str, "AirPortInfo.txt");
		return TRUE;
	}

	if( lineNum == ProcessorToResourcePoolFile )
	{
		strcpy( str, "proctoresource.in");
		return TRUE;
	}

	if( lineNum == ResourecPoolFile )
	{
		strcpy( str, "ResourcePool.in");
		return TRUE;
	}
	
	if( lineNum == UserNameListFile )
	{
		strcpy( str, "UserNameList.txt");
		return TRUE;
	}

	if( lineNum == SimAndReportFile )
	{
		strcpy( str, "SimAndReport.sim" );
		return TRUE;
	}
	
	if( lineNum == GateMappingDBFile )
	{
		strcpy( str, "GateMappingDB.txt");
		return TRUE;
	}
	
	if( lineNum == ProcessorSwitchTimeFile )
	{
		strcpy( str, "ProSwitchTimeDB.txt");
		return TRUE;
	}
	
	if( lineNum == 	MobileElemTypeStrDBFile )
	{
		strcpy( str , "MobElemTypeStrDB.TXT" );
		return TRUE;
	}

	if( lineNum == SpecificFlightPaxFlowFile )
	{
		strcpy( str , "SpecificFlightPaxFlow.IN" );
		return TRUE;
	}
	if( lineNum == StationPaxFlowFile )
	{
		strcpy( str , "STATIONPAXFLOW.TXT" );
		return TRUE;
	}

	if( lineNum == TrainLogEchoFile )
	{
		strcpy( str , "TRAINDESC.ECH" );
		return TRUE;
	}

	if( lineNum == TrainDescFile )
	{
		strcpy( str , "TRAINDESC.BIN" );
		return TRUE;
	}

	if( lineNum == TrainEventFile )
	{
		strcpy( str , "TRAINEVENT.BIN" );
		return TRUE;
	}

	if( lineNum == BridgeConnectorDescFile )
	{
		strcpy( str , "BRIDGECONNECTORDESC.BIN" );
		return TRUE;
	}

	if( lineNum == BridgeConnectorEventFile )
	{
		strcpy( str , "BRIDGECONNECTOREVENT.BIN" );
		return TRUE;
	}

	if ( lineNum == AllCarsScheduleFile )
	{
		char strFileName[64] = "AllCarSchedule.IN";
		strcpy(str, strFileName);
		return TRUE;

	}

	if( lineNum == FlightScheduleCriteriaFile )
	{
		char strFileName[64] = "FlightSchduleCriteria.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if ( lineNum == SubFlowList )
	{
		char strFileName[64] = "ProcessUnitInternalFlow.IN";
		strcpy(str, strFileName);
		return TRUE;

	}
	

	if ( lineNum == PipeDataSetFile )
	{
		char strFileName[64] = "PipeDataFile.IN";
		strcpy(str, strFileName);
		return TRUE;

	}
	
	if (lineNum == TerminalProcessorTypeSetFile)
	{
		char strFileName[64] = "TerminalProcessorTypeDataFile.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if ( lineNum == MovingSideWalKFile )
	{
		char strFileName[64] = "MovingSideWalK.IN";
		strcpy(str, strFileName);
		return TRUE;

	}

	if( lineNum == ReportParameterFile )
	{
		char strFileName[64] = "ReportParameter.db";
		strcpy(str, strFileName);
		return TRUE;
	}
	if ( lineNum == MovingSideWalkPaxDistFile )
	{
		char strFileName[64] = "MovingSideWalkPaxDist.IN";
		strcpy(str, strFileName);
		return TRUE;

	}
	
	if ( lineNum == StationLayoutFile )
	{
		char strFileName[64] = "StationLaout.IN";
		strcpy(str, strFileName);
		return TRUE;
	}
	if(lineNum == BagGateAssignFile)
	{
		char strFileName[64] = "BAGGATE.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == GatePriorityInfoFile)
	{
		char strFileName[64] = "GATEPRIO.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == FlightPriorityInfoFile)
	{
		char strFileName[64] = "FLTPRIO.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == IntermediateStandAssignFile)
	{
		char strFileName[64] = "INTERMEDIATESTANDASSIGN.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == ConstraintSpecFile)
	{
		char strFileName[64] = "ConstraintSpec.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == GateAssignConFile)
	{
		char strFileName[64] = "GateAssignCon.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == AdjacencyGateConFile)
	{
		char strFileName[64] = "AdjacencyGateCon.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == BagDeliveryTimeSpecFile)
	{
		char strFileName[64] = "BAGDELTIME.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if( lineNum == FireEvacuationReportFile )
	{
		strcpy( str, "FireEvacution.fir" );
		return TRUE;
	}

	if(lineNum == BagDeliveryTimeReportFile)
	{
		char strFileName[64] = "BAGDELTIME.BDT";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == BagDistSpecFile)
	{
		char strFileName[64] = "BAGDIST.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == BagDistReportFile)
	{
		char strFileName[64] = "BAGDIST.BDR";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == AcOperationsSpecFile)
	{
		char strFileName[64] = "FLTOPS.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == AcOperationsReportFile)
	{
		char strFileName[64] = "FLTOPS.AOR";
		strcpy(str, strFileName);
		return TRUE;
	}
	if(lineNum == CollisionReportFile)
	{
		char strFileName[64] = "COLLISION.CRF";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == CollisionSpecFile)
	{
		char strFileName[64] = "COLLISION.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == SpaceThroughputReportFile)
	{
		char strFileName[64] = "STHRUPUT.STD";
		strcpy(str, strFileName);
		return TRUE;
	}

	if(lineNum == SpaceThroughputSpecFile)
	{
		char strFileName[64] = "STHRUPUT.IN";
		strcpy(str, strFileName);
		return TRUE;
	}

	if( lineNum == AreasFile )
	{
		strcpy( str , "Areas.txt" );
		return TRUE;
	}

	if(lineNum == NonPaxRelativePosFile)
	{
		strcpy(str,"NonPaxRelPos.txt");
		return TRUE;
	}
	if( lineNum == PortalsFile )
	{
		strcpy( str , "Portals.txt" );
		return TRUE;
	}
	if( lineNum == FloorsFile || AirsideFloorsFile == lineNum || LandsideFloorsFile == lineNum )
	{
		strcpy( str , "Floors.txt" );
		return TRUE;
	}

	if( AirsideStructureFile == lineNum )
	{
		strcpy(str, "AirsideStructure.txt");
		return TRUE;
	}

	if (TerminalStructureFile == lineNum)
	{
		strcpy(str, "TerminalStructure.txt");
		return TRUE;
	}
	if( AirsideWallShapeFile == lineNum )
	{
		strcpy(str, "AirsideWallShape.txt");
		return TRUE;
	}

	if (TerminalWallShapeFile == lineNum)
	{
		strcpy(str, "TerminalWallShape.txt");
		return TRUE;
	}

	if( AirsideContourTreeFile == lineNum )
	{
		strcpy(str, "ContourTreeStruct.txt");
		return TRUE;
	}

	if( lineNum == DefaultDisplayPropertiesFile )
	{
		strcpy( str , "DefDisp.txt" );
		return TRUE;
	}
	if( lineNum == PlacementsFile || lineNum == AirsidePlacementsFile || lineNum == LandsidePlacementsFile)
	{
		strcpy( str , "Placements.txt" );
		return TRUE;
	}
	if( lineNum == PaxDisplayPropertiesFile )
	{
		strcpy( str, "MobileElementsDisplay.txt" );
		return TRUE;
	}
	if( lineNum == ResourceDisplayPropertiesFile )
	{
		strcpy( str, "ResourceDisplay.txt" );
		return TRUE;
	}
	if( lineNum == AircraftDisplayPropertiesFile )
	{
		strcpy( str, "AircraftDisplay.txt" );
		return TRUE;
	}
	if(lineNum == PaxTagsFile )
	{
		strcpy( str, "MobileElementTags.txt" );
		return TRUE;
	}
	if(lineNum == ProcTagsFile )
	{
		strcpy( str, "ProcTags.txt" );
		return TRUE;
	}
	if(lineNum == FlightTagsFile )
	{
		strcpy( str, "FlightTags.txt" );
		return TRUE;
	}
	if( lineNum == AircraftTagsFile	)
	{
		strcpy( str, "AircraftTags.txt" );
		return TRUE;

	}
	if(lineNum == MoviesFile )
	{
		strcpy( str, "Movies.txt" );
		return TRUE;
	}
	if(lineNum == WalkthroughsFile )
	{
		strcpy( str, "Walkthroughs.txt" );
		return TRUE;
	}
	if(lineNum == AirsideWalkthroughFile)
	{
		strcpy( str, "AirsideWalkthroughFile.txt");
		return TRUE;
	}
	if (lineNum == LandsideVehicleWalkthroughsFile)
	{
		strcpy( str, "LandsideVehicleWalkthroughsFile.txt");
		return TRUE;
	}
	if(lineNum == VideoParamsFile )
	{
		strcpy( str, "VideoParams.txt" );
		return TRUE;
	}
	if(lineNum == ActivityDensityFile )
	{
		strcpy( str, "ActivityDensityParams.txt" );
		return TRUE;
	}
	if(lineNum == LayerInfoFile )
	{
		strcpy( str, "LayerInfo" );
		return TRUE;
	}
	if(lineNum == CamerasFile || lineNum == AirsideCamerasFile || lineNum == LandsideCamerasFile)
	{
		strcpy( str, "Views.txt" );
		return TRUE;
	}	
	
	if(lineNum == DisplayPropOverridesFile )
	{
		strcpy( str, "DisplayPropOverrides.txt" );
		return TRUE;
	}
	if(lineNum == VRInfoFile)
	{
		strcpy( str, "VRInfo");
		return TRUE;
	}

	if ( lineNum == PassengerBulkFile )
	{
		strcpy(str, "PassengerBulkFile.txt");
		return TRUE;		
	}

	//	add by kevin
	if (lineNum == HostInfoFile)
	{
		strcpy(str, "HostInfo.txt");
		return TRUE;
	}

	if (lineNum == ProjectInfoFile)
	{
		strcpy(str, "ProjInfo.txt");
		return TRUE;
	}

	if (lineNum == Mult_HostInfoFile)
	{
		strcpy(str, "Mult_HostInfo.txt");
		return TRUE;
	}

	if (lineNum == Mult_ProjectInfoFile)
	{
		strcpy(str, "Mult_ProjInfo.txt");
		return TRUE;
	}

	if (lineNum == AirsideAircraftCategory)
	{
		strcpy(str, "AircraftCategory.txt");
		return TRUE;
	}

	if (lineNum == LogDebugFile)
	{
		strcpy(str,"Log.ini");
		return TRUE;
	}

	if (lineNum == FlowSyncFile)
	{
		strcpy(str, "FlowSyncFile.txt");
		return TRUE;
	}
	
	if (lineNum == Stand2GateMapping)
	{
		strcpy(str, "Stand2GateMapping.txt");
		return TRUE;
	}
    if(lineNum == RosterRulesFile)
	{
		strcpy(str , "RosterRules.txt");
		return TRUE ;
	}

	if(lineNum == PeopleMoverFile)
	{
		strcpy(str , "PeopleMove.MVR");
		return TRUE ;
	}
	CString csFileName[] = 
	{
		"BuildingDeprecCost.txt",
		"CaptIntrCost.txt",
		"InsuranceCost.txt",
		"ContractsCost.txt",
		"ParkingLotsDeprecCost.txt",
		"LandsideFacilitiesCost.txt",
		"AirsideFacilitiesCost.txt",
		"ProcessorCost.txt",
		"PassengerCost.txt",
		"AircraftCost.txt",
		"UtilitiesCost.txt",
		"LaborCost.txt",
		"MaintenanceCost.txt",
		"LeasesRevenue.txt",
		"AdvertisingRevenue.txt",
		"ContractsRevenue.txt",
		"PaxAirportFeeRevenue.txt",
		"LandingFeesRevenue.txt",
		"GateUsageRevenue.txt",
		"ProcUsageRevenue.txt",
		"RetailPercRevenue.txt",
		"ParkingFeeRevenue.txt",
		"LandsideTransRevenue.txt",
		"WaitingCostFactors.txt",
		"CapitalBudgetingDecisions.txt",
		"AnnualActivityDeduction.txt"
	};

	if( AirsideProcessorLayoutFile == lineNum || LandsideProcessorFile == lineNum )
	{
		mode = 1;//EnvMode_Terminal; for useing the same file named filelist.txt
		airport = 0;
		lineNum = ProcessorLayoutFile;
	}
	else
	{
		if( lineNum >= BuildingDeprecCost )
		{
			strcpy( str, csFileName[lineNum-BuildingDeprecCost] );
			return TRUE;
		}
	}


    CString csInputFileList;

//	csInputFileList.Format( "%s\\%s\\%s", _csProjDir, szDirNames[InputDir], szFileNames[InputFileList] );
	switch( mode )
	{// mode == enum EnvironmentMode; 
		//	0 = EnvMode_LandSide, 1 = EnvMode_Terminal, 2 = EnvMode_AirSide;
	case 0:
		{
			csInputFileList.Format( "%s\\%s\\AirPort%d\\LandSide\\%s",
				_csProjDir, szDirNames[InputDir], airport + 1, szFileNames[InputFileList] );
		}
		break;
	case 1:
		{	
			csInputFileList.Format( "%s\\%s\\%s", _csProjDir, szDirNames[InputDir], szFileNames[InputFileList] );
		}
		break;
	case 2:
		{
			csInputFileList.Format( "%s\\%s\\AirPort%d\\AirSide\\%s",
				_csProjDir, szDirNames[InputDir], airport + 1, szFileNames[InputFileList] );
		}
		break;
	default:
		csInputFileList.Format( "%s\\%s\\%s", _csProjDir, szDirNames[InputDir], szFileNames[InputFileList] );
	}
	

    ifsstream fileList (csInputFileList);
    if (fileList.bad())
        throw new FileOpenError (csInputFileList);

    char line[64];
    fileList.getline (line, 64);
    for (int i = 0; i <= lineNum && !fileList.eof(); i++)
        fileList.getline (line, 64);
    fileList.close();

    if (fileList.eof())
        return FALSE;

    strcpy (str, line);
    return TRUE;
}

// returns TRUE if file already exists
int CProjectManager::fileExists (char *filename)
{
    FileManager fileMan;
    return fileMan.IsFile (filename);
}


void CProjectManager::getSwapDrive (void)
{
    FileManager fileMan;
    m_csSwapDrive = m_csAppPath + "\\swap";

	if (!fileMan.IsDirectory (m_csSwapDrive))
        if (!fileMan.MakePath (m_csSwapDrive))
            throw new DirNotFoundError (m_csSwapDrive);
}

void CProjectManager::cleanSwapDrive (void)
{
	 FileManager fileMan;
	 char tempPath[64];
	 strcpy (tempPath, m_csSwapDrive);
	 strcat (tempPath, "\\*.tmp");
	 fileMan.DeleteFile(tempPath);
}

void CProjectManager::makeFileName (char *filename, long fileNumber)
{
	getSwapDrive();
    strcpy (filename, m_csSwapDrive);
	strcat( filename, "\\" );
    _ltoa (fileNumber, filename + strlen (filename), 10);
    strcat (filename, ".tmp");
}

long CProjectManager::getUniqueFileNumber (void)
{
    char filename[_MAX_PATH];
    static long fileNum = 0;
    do {
        if (fileNum == 99999999l)
            fileNum = 0l;
        makeFileName (filename, ++fileNum);
    } while (!_access (filename, 0));
    return fileNum;
}

void CProjectManager::getUniqueFileName (char *p_str)
{
    long num = getUniqueFileNumber();
    makeFileName (p_str, num);
}


CString CProjectManager::GetSectorFilePath()
{
	return m_csAirportPath + "\\Sector.sec";
}


CString CProjectManager::GetAirportFilePath()
{
	return m_csAirportPath + "\\Airports.dat";
}

CString CProjectManager::GetAirlineFilePath()
{
	return m_csAirportPath + "\\Airlines.dat";
}

CString CProjectManager::GetSubairlineFilePath()
{
	return m_csAirportPath + "\\subairlines.sal";
}


CString CProjectManager::GetACTypeFilePath()
{
	return m_csACTypePath + "\\acdata.acd";
}


CString CProjectManager::GetACCatFilePath()
{
	return m_csACTypePath + "\\category.cat";
}

CString CProjectManager::GetProbDistFilePath()
{
	return m_csProbDistPath + "\\probdist.pbd";
}

CString CProjectManager::GetGlobalDBListFilePath( void ) const
{
	return m_csAppPath + "\\GlobalDBList.ini";
}
CString CProjectManager::GetLogDebugFilePath( void ) const
{
	return m_csAppPath + "\\Log.ini";
}
const CString& CProjectManager::GetDefaultDBPath( void ) const
{
	return m_csDefaultDBPath;
}
// Copy current project into new project of projName.
BOOL CProjectManager::CopyProject( CString _strProjName, CString _csProjPath, PROJECTINFO pi,CopyFileParam *pParam )
{
	
	// check if the project exist	

	FileManager fm;
	if( fm.IsDirectory( _csProjPath ) )
	{
		AfxMessageBox( "A project with the specified name already exists", MB_ICONEXCLAMATION | MB_OK );
		return FALSE;
	}

	// copy the project.
	if( !fm.MakePath( _csProjPath ) )
		return FALSE;

//	fm.CopyDirectory( pi.path, _csProjPath ,_ShowCopyProgress);
	if(!fm.CopyDirectoryWithProgress( pi.path, _csProjPath,pParam))
		return FALSE;
	// update project modify time
	pi.name			= _strProjName;
	pi.path			= _csProjPath;
	pi.modifytime	= CTime::GetCurrentTime();
	pi.createtime	= CTime::GetCurrentTime();
	UpdateProjInfoFile( pi );
	return TRUE; 
}


CString CProjectManager::GetPath( const CString& _csProjPath, int _nDirType, int mode, int airport )
{
	CString csPath;
	if( InputDir == _nDirType )
	{
		switch( mode )
		{// mode == enum EnvironmentMode; 
			//	0 = EnvMode_LandSide, 1 = EnvMode_Terminal, 2 = EnvMode_AirSide;
		case 0:
			{
				csPath.Format( "%s\\AirPort%d\\LandSide\\%s", _csProjPath, airport + 1, szDirNames[_nDirType] );
			}
			break;
		case 1:
			{	
				csPath.Format( "%s\\%s", _csProjPath, szDirNames[_nDirType] );
			}
			break;
		case 2:
			{
				csPath.Format( "%s\\AirPort%d\\AirSide\\%s", _csProjPath, airport + 1, szDirNames[_nDirType] );
			}
			break;
		default:
			csPath.Format( "%s\\%s", _csProjPath, szDirNames[_nDirType] );
		}

	}
	else
	{
		csPath.Format( "%s\\%s", _csProjPath, szDirNames[_nDirType] );
	}

	return csPath;
}


void CProjectManager::SetProjectDeletedFlag(CString projectName, bool flag)
{

	CString projFilePath = m_csRootProjectPath + _T("\\") + projectName + _T("\\projinfo");
	CFile* pFile = NULL;

	CString userName;
	CString machineName;
	CString createDate1;
	CString createDate2;
	CString version;
//	CString 
	
	try {
		pFile = new CFile(projFilePath, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		
		ar.ReadString(userName); // read user name line
		
		ar.ReadString(machineName); // read machine name line
	
		ar.ReadString(createDate1); // read create date
	
		ar.ReadString(createDate2); // read create date
	
		ar.ReadString(version); // read version
		
		
		//ar.ReadString(line, 512); // read deleted flag
		
		
		ar.Close();
		delete pFile;
				
	}
	catch(CException* e)
	{
		AfxMessageBox("Error reading project info file for project " + projectName + ".", MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		
	}
	
	


	try {
		pFile = new CFile(projFilePath, CFile::modeWrite  | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::store);
		
		ar.WriteString(userName+"\n"); // read user name line
		
		ar.WriteString(machineName+"\n"); // read machine name line
	
		ar.WriteString(createDate1+"\n"); // read create date
	
		ar.WriteString(createDate2+"\n"); // read create date
	
		ar.WriteString(version+"\n"); // read version
		
		if(flag)
			ar.WriteString("1");
		else
			ar.WriteString("0");

		//ar.ReadString(line, 512); // read deleted flag
		
		
		ar.Close();
		delete pFile;
				
	}
	catch(CException* e)
	{
		AfxMessageBox("Error reading project info file for project " + projectName + ".", MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		
	}

}

void CProjectManager::SetInstance( CProjectManager* pInst )
{
	m_pInstance = pInst;
}