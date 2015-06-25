// EchoSystem.cpp: implementation of the CEchoSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "EchoSystem.h"

#include "assert.h"
#include "common\projectmanager.h"
#include "common\exeption.h"
#include "common\UndoManager.h"
#include "Common\CAutoSaveHandlerInterface.h"
#include "HeapObj.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEchoSystem* CEchoSystem::m_pInstance = NULL;

CEchoSystem::CEchoSystem()
{
	m_bEnable = false;
	m_strProjPath = PROJMANAGER->GetAppPath();

	//for (int i = 0; i < LogTypeCount; i++)
	//{
	//	m_mapLog[(EchoLogType)i] = false;
	//}

    loadDataSet( FirstLoad );
	saveDataSet(Disable, FirstSave);

}

CEchoSystem::~CEchoSystem()
{
}

CEchoSystem* CEchoSystem::GetInstance()
{
	if(m_pInstance == NULL)
	{	
		m_pInstance = new CEchoSystem();
	}
	return m_pInstance;
}


void CEchoSystem::DeleteInstance()
{
	if (m_pInstance)
	{
		m_pInstance->saveDataSet();
		delete m_pInstance;
	}
	m_pInstance = NULL;
}


void CEchoSystem::writeData(CLogFile& p_file,bool _bEnable,bool _bFirstLoad) const
{
	char szTemp[10];
	p_file.writeField("ENABLE");
	if ((_bFirstLoad && m_bEnable) || (!_bFirstLoad && _bEnable))
	{
		strcpy(szTemp,"YES");
	}
	else
	{
		strcpy(szTemp,"NO");
	}
	p_file.writeField(szTemp);
	p_file.writeLine();

	p_file.writeInt((int)m_vLog.size());//save the number of the Log
	p_file.writeLine();

	std::vector<echoSetting >::const_iterator it = m_vLog.begin();

	for (int i = 0; i < (int)m_vLog.size() && it != m_vLog.end(); i++,it++)
	{
		p_file.writeField( GetStringByLogType((EchoLogType)i) );
		
		if ( (*it).bEnable == true)
		{
			strcpy(szTemp,"YES");			
		}
		else
		{
			strcpy(szTemp,"NO");
		}
		p_file.writeField(szTemp);

		p_file.writeInt((*it).nElementID);

		p_file.writeLine();

	}
}

void CEchoSystem::readData(CLogFile& p_file)
{
	char szTemp[10];
	p_file.skipField(1);
	p_file.getField(szTemp,10);
	if (strcmp(szTemp,"YES") == 0)
	{
		m_bEnable = TRUE ;
	}
	else
	{
		m_bEnable = FALSE ;
	}

	p_file.getLine();

	int nCountlog;
	p_file.getInteger(nCountlog);//read the number of the log
	p_file.getLine();

	char strLog[64];
	bool bEnableLog;
	
	for (int i = 0; i < nCountlog; i++)
	{
		p_file.getField(strLog,64);//read the EchoLogType
		p_file.getField(szTemp,10);//read the state

		if (strcmp(szTemp,"YES") == 0)
		{
			bEnableLog = true;
		}
		else
		{
			bEnableLog = false;
		}

		echoSetting setting;
		setting.echoType = EchoLogType(i);
		setting.bEnable = bEnableLog;

		p_file.getInteger(setting.nElementID);
		

		m_vLog.push_back(setting);

		p_file.getLine();

	}

}
void CEchoSystem::loadDataSet(bool _bFirstLoad)
{
	if (!_bFirstLoad)
	{
		clear();
	}

	char filename[_MAX_PATH];
	CString filepath = m_strProjPath +_T("\\Log.ini");
	strcpy(filename,filepath);

	CLogFile file;//HEAPOBJ(CLogFile, file, CLogFile());

	try { file.openFile (filename, READ); }
	catch (FileOpenError *exception)
	{
		delete exception;
		initDefaultValues();
		saveDataSet();
		return;
	}
	
	readData (file);
	file.closeIn();
}

void CEchoSystem::saveDataSet(bool _Enable, bool _bFirstLoad) const
{
	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;
	char filename[_MAX_PATH];
	CString filepath = m_strProjPath +_T("\\Log.ini");
	strcpy(filename,filepath);

	
	CLogFile file;//HEAPOBJ(CLogFile, file, CLogFile());
	file.openFile (filename, WRITE);

	writeData (file,_Enable,_bFirstLoad);

	file.closeOut();

}

bool CEchoSystem::IsLogSwithcesOn()
{
	ECHOLOG->loadDataSet();
	bool bResult = ECHOLOG->IsEnable();

	if ( bResult )	// Master log switch is on
	{
		std::vector<echoSetting >  &vLog = ECHOLOG->GetLogTypeData();
		std::vector<echoSetting >::const_iterator it = vLog.begin();

		for (int i = 0; i < ECHOLOG->GetLogTypeCount() && it != vLog.end(); i++,it++)
		{
			bResult = (*it).bEnable;
			if ( bResult )
				break;
		}
	}

	return bResult;

}

bool CEchoSystem::IsEnable()
{
	loadDataSet();
	return m_bEnable;
}

bool CEchoSystem::IsLogEnable(EchoLogType key)
{
	clear();
	loadDataSet();
	for (std::vector<echoSetting >::const_iterator it = m_vLog.begin(); it != m_vLog.end(); it++)
	{
		if (it->echoType == key)
			return (*it).bEnable;
	}

	//m_mapLog[key] = true;//insert the new key
 //   saveDataSet();//save the data to file
	return true;//the key not exist,return true
}
int CEchoSystem::GetLogID(EchoLogType key)
{
	std::vector<echoSetting >& vLog = ECHOLOG->GetLogTypeData();
	for (std::vector<echoSetting >::const_iterator it = vLog.begin(); it != vLog.end(); it++)
	{
		if (it->echoType == key)
			return (*it).nElementID;
	}
	return -1;
}
void CEchoSystem::SetMasterLogDisable()
{
	char filename[_MAX_PATH];
	CString filepath = m_strProjPath +_T("\\Log.ini");

	strcpy(filename,filepath);


	CLogFile file;//HEAPOBJ(CLogFile ,file, CLogFile());
	
	file.openFile (filename, WRITE);

	// turn off the master log switch
	writeData (file, false, false);

	file.closeOut();

}

CString CEchoSystem::GetStringByLogType(EchoLogType key) const
{
	// when you add new log type, please add code here
	CString strLogType;

	switch ( key )
	{
	
	case LoadProject:
		strLogType = _T("LoadProject");
		break;

	case VehicleMoveLog:
		strLogType = _T("VehicleMove");
		break;

	case RailWayLog:
		strLogType = _T("RailWayLog");
		break;

	default:
		strLogType = _T("NoLogType");
		break;

	}

	return strLogType;
	
}

BOOL CEchoSystem::Log(EchoLogType key ,CString content)
{
	CTime t = CTime::GetCurrentTime();
	CString strTime = t.Format("%Y-%m-%d,%H:%M:%S");

	if (m_bEnable == false && RailWayLog != key)
		return FALSE;

	// open or create file
	CString filePath;
	filePath.Format(_T("%s\\Log\\%s.txt"),m_strProjPath,GetStringByLogType(key));
	FILE* pFile = NULL;
	pFile = fopen( filePath, "at" );
	if( !pFile )
		return FALSE;
	fclose( pFile );

	char filename[_MAX_PATH];
	strcpy(filename,filePath);

	CLogFile file;//HEAPOBJ(CLogFile, file, CLogFile());
	
	file.openFile (filename, APPEND);

	//write the time
	file.writeField(strTime);
	//write the content
	file.writeField(content);
	file.writeLine();
	file.closeOut();
	
	return TRUE;
}

BOOL CEchoSystem::Log(EchoLogType key ,int content)
{
	if (m_bEnable == false)
		return FALSE;

	CString csStr;
	csStr.Format( "%d", content );

	return Log(key,csStr);
}

BOOL CEchoSystem::Log(EchoLogType key ,long content)
{
	if (m_bEnable == false)
		return FALSE;

	CString csStr;
	csStr.Format( "%f", content );

	return Log(key,csStr);
}
BOOL CEchoSystem::Log(EchoLogType key ,float content)
{
	if (m_bEnable == false)
		return FALSE;

	CString csStr;
	csStr.Format( "%f", content );

	return Log(key,csStr);
}
BOOL CEchoSystem::Log(EchoLogType key ,ElapsedTime content)
{
	if (m_bEnable == false)
		return FALSE;

	char timeString[20];
	ElapsedTime etSafeTime=content;

	etSafeTime.printTime (timeString,TRUE);

	return Log(key,timeString);
}

BOOL CEchoSystem::Log(EchoLogType key ,const Point& aPoint)
{
	if (m_bEnable == false)
		return FALSE;
	DistanceUnit xCoord, yCoord, zCoord;
	aPoint.getPoint (xCoord, yCoord, zCoord);

	CString csStr;
	csStr.Format( " x = %f , y = %f , z = %f ", xCoord, yCoord, zCoord );
	
	return Log(key,csStr);
}

void CEchoSystem::SetPath(const CString &strPath)
{
	m_strProjPath = strPath;

	CString strFolder = m_strProjPath+_T("\\Log");

	if (!PathFileExists(strFolder))
	{
		CreateDirectory(strFolder,NULL);
	}
	if (m_pInstance)
	{
		m_pInstance->loadDataSet();
	}
}
