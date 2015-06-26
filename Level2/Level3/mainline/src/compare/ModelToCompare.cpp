// ModelToCompare.cpp: implementation of the CModelToCompare class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "ModelToCompare.h"

#include "../Engine/terminal.h"
#include "../Reports/ReportParaOfProcs.h"
#include "../common/SimAndReportManager.h"
#include "../common/AirportDatabase.h"
#include "../common/AirportDatabaseList.h"
#include "..\Database\ARCportADODatabase.h"
#include "..\Database\PartsAndShareTemplateUpdate.h"

#include "../Engine/terminal.h"
#include "Inputs/AreasPortals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModelToCompare::CModelToCompare()
{
	m_terminal = NULL;
	m_bNeedCopy = false;
	m_isChecked = TRUE;

}

CModelToCompare::~CModelToCompare()
{
	if(m_terminal->m_pAirportDB != NULL)
	{
		m_terminal->m_pAirportDB->clearData();
	}
	delete m_terminal;
	m_terminal = NULL;

}

const CString& CModelToCompare::GetModelName() const
{
	return m_strModelName;
}
void CModelToCompare::SetUniqueName(const CString& strName)
{
	m_strUniqueName = strName;

}
const CString& CModelToCompare::GetUniqueName() const
{
	return m_strUniqueName;
}
void CModelToCompare::SetModelName(const CString& strName)
{
	m_strModelName = strName;
}

const CString& CModelToCompare::GetModelLocation() const
{
	return m_strModelLocation;
}

void CModelToCompare::SetModelLocation(const CString& strLocation)
{
	m_strModelLocation = strLocation;
}
const CString& CModelToCompare::GetLocalPath() const
{
	return m_strLocalPath;
}

void CModelToCompare::SetLocalPath(const CString& strLocalPath)
{
	m_strLocalPath = strLocalPath;
}

const CString& CModelToCompare::GetDataBasePath() const
{
	return m_strDatabasePath;
}

void CModelToCompare::SetDataBasePath(const CString& strDataBasePath)
{
	m_strDatabasePath = strDataBasePath;
}

const CString& CModelToCompare::GetLastModifyTime() const
{
	return m_lastModifiedTime;
}

void CModelToCompare::SetLastModifyTime(const CString& strLastTime)
{
	m_lastModifiedTime = strLastTime;
}

Terminal *CModelToCompare::GetTerminal()
{
	//if (m_terminal == NULL)
	//{
	//	m_terminal = InitTerminal();
	//}
	ASSERT(m_terminal != NULL);
	return m_terminal;
}
InputTerminal *CModelToCompare::GetInputTerminal()
{
	//if (m_terminal == NULL)
	//{
	//	m_terminal = InitTerminal();
	//}
	ASSERT(m_terminal != NULL);
	return m_terminal;
}
BOOL CModelToCompare::TransferFiles(const CString& strSource, const CString& strDest,void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR))
{
	CFileOperation fo;
	fo.SetAskIfReadOnly(false);
	fo.SetOverwriteMode(true);
	return fo.NotCopyFolderOfSimResult(strSource, strDest,_ShowCopyInfo);
}


void CModelToCompare::RemoveFiles(const CString &strPath)
{
	CFileOperation fo;
	fo.SetAskIfReadOnly(false);
	fo.Delete(strPath);
}

BOOL CModelToCompare::IsLocalModel(const CString &strPath)
{
	CString str = _T("(LOCAL)");
	if (strPath.Left(2) == _T("\\\\"))
	{
		int nPos = strPath.Find(_T("\\"), 2);
		if (nPos != -1)
			str = strPath.Mid(2, nPos - 2);
		else
			str = strPath.Mid(2, strPath.GetLength() - 2);
	}

	str.MakeUpper();

	return ((str == _T("(LOCAL)")) ? TRUE : FALSE);

}
Terminal* CModelToCompare::InitTerminal(CCompRepLogBar* pStatus, CString strName, void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR))
{
	if (m_terminal != NULL)
		return m_terminal;

	CString strPath = PROJMANAGER->GetAppPath();
	strPath += _T("\\Comparative Report\\") + strName;
		
	CString strStatus;
	CString strTempPath = strPath;
	CString strDbName = "";
	int	 nProgressStatus = 0;

	if ( !IsLocalModel(m_strModelLocation))// network model data
	{
		//check if the network project is available


		CString strDbPath;   //get the GlobalDBList.ini path
		CString strProjPath = m_strModelLocation; //the project path
		m_strLocalPath = strPath + _T('\\') + m_strModelName;//save the local path

		CString strNetworkProjPath;//the network project file's path 
		int nPos  = strProjPath.ReverseFind('\\');
		strNetworkProjPath = strProjPath.Mid(0,nPos);

		CString strNetGetAppPath;//the network App path
		nPos = strNetworkProjPath.ReverseFind('\\');
        strNetGetAppPath = strNetworkProjPath.Mid(0,nPos);

		//get the project  name
		nPos = m_strModelLocation.ReverseFind('\\');
		CString strModelname = m_strModelLocation.Mid(nPos+1,m_strModelLocation.GetLength()-1);

		//find that if the project exist or not in local
		CString strLocalProjectPath = strPath + _T('\\') + strModelname;


		//to find in network
		CFileFind fileFind; 
		BOOL bFindInNetwork = fileFind.FindFile(m_strModelLocation);
		fileFind.Close();

		//to find it in local
		BOOL bFindInLocal = fileFind.FindFile(strLocalProjectPath,0);
		fileFind.Close();
		if(!bFindInNetwork && !bFindInLocal)
		{
			//cannot find in local and network
			CString strMsg;
			strMsg.Format(_T("Failed to load project %s, can not be accessed both in local or network."),strModelname);
			if(_ShowCopyInfo)
				(*_ShowCopyInfo)(0, strMsg);
			AfxMessageBox(strMsg);

			return NULL;
		}
		//find in local or find in network or both

		//get the projInfo file  , read the dbIndex or dbName.
		if ( bFindInLocal )//project exist in local
		{			
			PROJECTINFO piLocal;
			PROJMANAGER->GetProjectInfo(strModelname, &piLocal, strPath);//get the project information
				
			if(bFindInNetwork)//find in network, check the latest modified time
			{
				PROJECTINFO piNetwork;
				PROJMANAGER->GetProjectInfo(m_strModelName, &piNetwork, strNetworkProjPath);//get the project information
				if ( piNetwork.modifytime != piLocal.modifytime)//if modify,copy project again 
				{
					TransferFiles(m_strModelLocation, strPath,_ShowCopyInfo);
					m_lastModifiedTime = piNetwork.modifytime.Format("%y %m %d %H %M");//save the last modify time
				}
			}
			else //not find in network
			{
				CString strMsg;
				strMsg.Format(_T("The project %s can not be accessed in network."),strModelname);
				if(_ShowCopyInfo)
					(*_ShowCopyInfo)(0, strMsg);

				AfxMessageBox(strMsg);
			}
		}
		else
		{
			PROJECTINFO piNetwork;
			PROJMANAGER->GetProjectInfo(m_strModelName, &piNetwork, strNetworkProjPath);//get the project information

			TransferFiles(m_strModelLocation, strPath,_ShowCopyInfo);//copy the project
			m_lastModifiedTime = piNetwork.modifytime.Format("%y %m %d %H %M");//save the last modify time
		}

		//project exists in local,Load database
		{
			m_strDatabasePath = strPath;
			//get the terminal data
			Terminal *term = new Terminal;
		

			//for project database load
			CString  strProjectDataFilePath(_T(""));
			CString	 strARCDatabaseFilePath(_T(""));

			strProjectDataFilePath.Format("%s\\%s\\%s",strPath,strModelname, "INPUT\\parts.mdb");
			strARCDatabaseFilePath.Format(_T("%s"),PROJMANAGER->GetAppPath() + "\\Databases\\arc.mdb");
			CARCProjectDatabase* pAirportDB = new CARCProjectDatabase(strARCDatabaseFilePath,strProjectDataFilePath);

			term->m_pAirportDB = pAirportDB;
			if( !term->m_pAirportDB->hadLoadDatabase() )
				term->m_pAirportDB->loadDatabase();
			term->loadInputs(strPath + _T("\\") + m_strModelName);
			term->GetSimReportManager()->loadDataSet(strPath + _T("\\") + m_strModelName);
			term->GetSimReportManager()->SetCurrentSimResult(0);
				
			m_terminal = term;
			return term;
		}
	}
	else
	{
		//	local data
		strPath = PROJMANAGER->GetAppPath() + _T("\\Project");
		m_strLocalPath = m_strModelLocation;

		PROJECTINFO pi;	
		PROJMANAGER->GetProjectInfo(m_strModelName, &pi, strPath);

		CAirportDatabase *pAirPortDB = NULL;
		pAirPortDB = AIRPORTDBLIST->getAirportDBByName( pi.dbname );
		if(pAirPortDB == NULL)
		{
			//for project database load
			CString  strProjectDataFilePath(_T(""));
			CString  strFolder(_T(""));
			CString	 strARCDatabaseFilePath(_T(""));
	
			strProjectDataFilePath.Format("%s\\%s", pi.path, "INPUT\\parts.mdb");
			strARCDatabaseFilePath.Format(_T("%s"),PROJMANAGER->GetAppPath() + "\\Databases\\arc.mdb");
			CARCProjectDatabase airportDB(strARCDatabaseFilePath,strProjectDataFilePath);

			airportDB.readAirport();
			airportDB.loadDatabase();

			//create share template database
			pAirPortDB = OpenProjectDBForInitNewProject(airportDB.getName());
		}
		if(pAirPortDB == NULL)
		{
			return NULL;
		}
		Terminal *term = new Terminal;
		term->m_pAirportDB = pAirPortDB;

		if( !term->m_pAirportDB->hadLoadDatabase() )
			term->m_pAirportDB->loadDatabase();

		m_strDatabasePath = PROJMANAGER->GetAppPath() + _T("\\ImportDB\\") + pAirPortDB->getName();
		m_lastModifiedTime = pi.modifytime.Format("%y %m %d %H %M");
						
		//open database connection 
		CString strDBPath;
		strDBPath += _T("\"");
		strDBPath += strPath;
		strDBPath += _T("\\");
		strDBPath += m_strModelName;
		strDBPath += _T("\\input\\arcport.mdb");
		strDBPath += _T("\"");
		DATABASECONNECTION.SetAccessDatabasePath(strDBPath);
		DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_ACCESS);

		term->loadInputs(strPath + _T("\\") + m_strModelName);
		term->GetSimReportManager()->loadDataSet(strPath + _T("\\") + m_strModelName);
		term->GetSimReportManager()->SetCurrentSimResult(0);

		m_terminal = term;

	}
	return m_terminal;
}

CAirportDatabase* CModelToCompare::OpenProjectDBForInitNewProject(const CString& sAirportName)
{
	CAirportDatabase* _AirportDB =  AIRPORTDBLIST->getAirportDBByName(sAirportName)  ;
	if(_AirportDB == NULL)
		_AirportDB = AIRPORTDBLIST->addNewDBEntry(sAirportName) ;
	else
	{
		if (!PathFileExists(_AirportDB->getDatabaseFilePath()))
		{
			CString strPartsSourceFile(_T(""));
			CString strPartsDestinationFile(_T(""));

			strPartsSourceFile.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),PROJMANAGER->GetAppPath());
			CopyFile(strPartsSourceFile,_AirportDB->getDatabaseFilePath(),FALSE);

			AirportDatabaseConnection pConnectionPtr(_AirportDB->getDatabaseFilePath());
			CPartsAndShareTemplateUpdate shareTemplateDataBaseUpdater;
			if(!shareTemplateDataBaseUpdater.Update(&pConnectionPtr)) 
			{
				CString strError(_T(""));
				strError.Format(_T("Update the new Template DataBase error."));
				MessageBox(NULL,strError,"Warning",MB_OK);
				return NULL;
			}
		}
	}
	return _AirportDB;
}
void CModelToCompare::AddSimResult(SimResultWithCheckedFlag& simResult)
{
	m_vSimResult.push_back(simResult);
}

void CModelToCompare::AddSimResult(char* pBuf, BOOL isChecked/*= TRUE*/)
{
	SimResultWithCheckedFlag simResult;
	simResult.SetSimResultName(CString(pBuf));
	simResult.SetChecked(isChecked);
	m_vSimResult.push_back(simResult);
}

int CModelToCompare::GetSimResultCount()
{
	return static_cast<int>(m_vSimResult.size());
}

SimResultWithCheckedFlag& CModelToCompare::GetSimResult(int nIndex)
{
	ASSERT(nIndex >=0 && nIndex < GetSimResultCount());
	return m_vSimResult[nIndex];
}

CString CModelToCompare::GetSimResultName(int nIndex)
{
	ASSERT(nIndex >=0 && nIndex < GetSimResultCount());
	return m_vSimResult[nIndex].GetSimResultName();
}
void CModelToCompare::ClearSimResult()
{
	m_vSimResult.clear();
}

bool CModelToCompare::FindSimResultByName(CString strSimName)
{
    size_t nCount = m_vSimResult.size();
    for(size_t i=0; i<nCount; i++)
    {
        if(strSimName.CompareNoCase(m_vSimResult.at(i).GetSimResultName()) == 0)
            return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
//	CModelToCompareDataSet
CModelToCompareDataSet::CModelToCompareDataSet() : DataSet(ModelInfoFile)
{
}

CModelToCompareDataSet::~CModelToCompareDataSet()
{
}

void CModelToCompareDataSet::clear()
{
}

void CModelToCompareDataSet::readData(ArctermFile& p_file)
{
	char buf[256];
	
	m_vModels.clear();
	
	
	while (p_file.getLine() == 1)
	{
		CModelToCompare *model = new CModelToCompare;
		
		// read the Model name;
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			model->SetModelName(buf);

		char cFlag = 'F';
		p_file.getChar(cFlag);
		if(cFlag == 'T')
			model->SetChecked(TRUE);
		else
			model->SetChecked(FALSE);

		int nSimResultCount;
		p_file.getInteger(nSimResultCount);
		for (int i =0; i < nSimResultCount; i++)
		{
			p_file.getField(buf,255);
			char cFlag = 'F';
			p_file.getChar(cFlag);
			if(cFlag == 'T')
				model->AddSimResult(buf, TRUE);
			else
				model->AddSimResult(buf, FALSE);
		}

		// read the unique name;
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			model->SetUniqueName(buf);

		// read the model location
		memset(buf,0,sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			model->SetModelLocation(buf);

		//read the local path
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			model->SetLocalPath(buf);

		//read the database path
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			model->SetDataBasePath(buf);

		//read the last modify time
		memset(buf, 0, sizeof(buf) / sizeof(char));
		if (p_file.getField(buf, 255) != 0)
			model->SetLastModifyTime(buf);

		model->InitTerminal(NULL,m_strProjName,NULL);
		
		m_vModels.push_back(model);
	}
}

void CModelToCompareDataSet::readObsoleteData(ArctermFile& p_file)
{
}

void CModelToCompareDataSet::writeData(ArctermFile& p_file) const
{
	for (unsigned i = 0; i < m_vModels.size(); i++)
	{
		p_file.writeField(m_vModels[i]->GetModelName());//write the model name
		if(m_vModels[i]->GetChecked() == TRUE)
		{
			p_file.writeChar('T');
		}
		else
		{
			p_file.writeChar('F');
		}
		int nSimResultCount = m_vModels[i]->GetSimResultCount();
		p_file.writeInt(nSimResultCount);
		for (int j = 0; j < nSimResultCount; ++j)
		{
			p_file.writeField(m_vModels[i]->GetSimResultName(j));
			SimResultWithCheckedFlag& simResult = m_vModels[i]->GetSimResult(j);
			if(simResult.GetChecked() == TRUE)
				p_file.writeChar('T');
			else
				p_file.writeChar('F');
		}

		p_file.writeField(m_vModels[i]->GetUniqueName());//write the unique name
		p_file.writeField(m_vModels[i]->GetModelLocation());//write the network path
		p_file.writeField(m_vModels[i]->GetLocalPath());//write the local path
		p_file.writeField(m_vModels[i]->GetDataBasePath());//write the database path
		p_file.writeField(m_vModels[i]->GetLastModifyTime());//write the last modify time
		p_file.writeLine();
	}

	if (m_vModels.empty())
		p_file.writeLine();
}

int CModelToCompareDataSet::GetModels(OUT std::vector<CModelToCompare *>& vModels)
{
	vModels = m_vModels;
	return m_vModels.size();
}

void CModelToCompareDataSet::SetModels(const std::vector<CModelToCompare *>& vModels)
{
	m_vModels = vModels;
}
