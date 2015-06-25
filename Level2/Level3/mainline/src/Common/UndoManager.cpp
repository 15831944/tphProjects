// UndoManager.cpp: implementation of the CUndoManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../Main/UndoOptionHander.h"
#include "UndoManager.h"
#include "common\fileman.h"
#include "common\exeption.h"
#include "common\ProjectManager.h"
#include <algorithm>
extern const CString c_setting_regsectionstring;
extern const CString c_undolength_entry = "Project Undo Length";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUndoManager* CUndoManager::P_singleTone = NULL ;
CUndoManager::CUndoManager( const CString& _csProjPath )
{
	// read the option setting.
	m_csProjPath = _csProjPath;
	int timer = 10 ;
	if(!CUndoOptionHander::ReadData(m_nUndoLength,timer))
	    m_nUndoLength = 10;

	CheckDirectory();
	LoadProjectList();
}

CUndoManager::~CUndoManager()
{
//	CleanDirectory();
}

CUndoManager* CUndoManager::GetInStance(const CString& projpath) 
{
	if(P_singleTone == NULL)
		P_singleTone = new CUndoManager(projpath) ;
	else
	{
		P_singleTone->m_csProjPath = projpath ;
		P_singleTone->CheckDirectory();
		P_singleTone->LoadProjectList();
	}
	return P_singleTone ;
}
// base on the current project, copy to the new undo project.
// Exception:	NO;
bool CUndoManager::AddNewUndoProject()
{
	// create directory with name of the current datetime and copy contect into the direcoty.			
	COleDateTime sysDate = COleDateTime::GetCurrentTime();
	CString csDirName;
	csDirName.Format( "%04d%02d%02d%02d%02d%02d",
						sysDate.GetYear(), 
						sysDate.GetMonth(),
						sysDate.GetDay(),
						sysDate.GetHour(),
						sysDate.GetMinute(),
						sysDate.GetSecond() );

	CUndoProject projUndo( m_csProjPath, csDirName );
	if(projUndo.CreatDirectory())
	{
           m_vUndoProj.push_back(projUndo) ;
		   if((int)m_vUndoProj.size() > m_nUndoLength)
		   {
			   UNDOPROJECT_LIST_ITER  iter = std::min_element(m_vUndoProj.begin(),m_vUndoProj.end());
			   if(iter != m_vUndoProj.end())
			   {
				   (*iter).DeleteDirectory() ;
				   m_vUndoProj.erase(iter) ;
			   }
		   }
		   return TRUE ;
	}else
		return FALSE ;
}


// creat the project list from the directory in the UNDO direcotry.
void CUndoManager::LoadProjectList()
{
	m_vUndoProj.clear() ;
	CString csUndoDir = PROJMANAGER->GetPath( m_csProjPath, UndoDir );
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	SetCurrentDirectory( csUndoDir );
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile( "*", &FindFileData );	
	while( GetLastError() != ERROR_NO_MORE_FILES ) 
	{
		if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
		{
			CString csDirName = FindFileData.cFileName;
			if( csDirName.GetLength() == 14 )
			{
				CUndoProject projUndo( m_csProjPath, csDirName );
				//int nCount = m_vUndoProj.size();
				//for( int i=0; i<nCount; i++ )
				//{
				//	if( projUndo < m_vUndoProj[i] )
				//		break;
				//}
				m_vUndoProj.push_back( projUndo );
			}
		}
		FindNextFile( hFind, &FindFileData );
	}
	
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);
}


// check if the direct exist, create one if not exist. should be used at least for now
// for backwards compatible. later on it should be deleted.
void CUndoManager::CheckDirectory()
{
	// make the dir name and check if the dir exist.
    FileManager fileMan;
	CString csPath = PROJMANAGER->GetPath( m_csProjPath, UndoDir );
	if( !fileMan.IsDirectory( csPath ) )
	{
		if( !fileMan.MakePath( csPath ) )
			throw new TwoStringError( "Unable to create directory ", csPath );
	}
}


// base on the undo length delete the old directory.
void CUndoManager::CleanDirectory()
{
	int nOverSize = m_vUndoProj.size() - m_nUndoLength;
	if( nOverSize <= 0 )
		return;

	for( int i=0; i<nOverSize; i++ )
	{
		m_vUndoProj[i].DeleteDirectory();
	}
}

int CUndoManager::GetProjCount()
{
	return m_vUndoProj.size();
}

CUndoProject CUndoManager::GetProject(int _nIdx)
{
	return m_vUndoProj[_nIdx];
}

// move the undo project and remove the entry in the vector.
void CUndoManager::RestoreTheProject( int _nIdx )
{
	m_vUndoProj[_nIdx].RestoreToProject();
}
