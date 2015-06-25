// UndoProject.cpp: implementation of the CUndoProject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UndoProject.h"
#include "common\fileman.h"
#include "common\ProjectManager.h"
#include "common\exeption.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoProject::CUndoProject( const CString& _csProjDir, const CString& _csDirLabel )
{
	assert( _csDirLabel.GetLength() == 14 );
	m_csProjDir = _csProjDir;
	m_csDirLabel = _csDirLabel;
}

CUndoProject::~CUndoProject()
{

}

// Description: Copy INUPT / ECONOMIC dir to undo dir.
// Exception:	NO
bool CUndoProject::CreatDirectory()
{
	CString csPath = PROJMANAGER->GetPath( m_csProjDir, UndoDir );
	csPath += "//";
	csPath += m_csDirLabel;

    FileManager fileMan;

	// input
	CString csDestInputPath = PROJMANAGER->GetPath( csPath, InputDir );
	if( !fileMan.MakePath( csDestInputPath ) )
		return false;

	// now copy the content.
	CString csSrcInputPath = PROJMANAGER->GetPath( m_csProjDir, InputDir );
	fileMan.CopyDirectory( csSrcInputPath, csDestInputPath,NULL );		

    int error = GetLastError() ;
	// economic
	CString csDestEconPath = PROJMANAGER->GetPath( csPath, EconomicDir );
	if( !fileMan.MakePath( csDestEconPath ) )
		return false;

	// now copy the content.
	CString csSrcEconPath = PROJMANAGER->GetPath( m_csProjDir, EconomicDir );
	fileMan.CopyDirectory( csSrcEconPath, csDestEconPath ,NULL);		

	return true;
}

void CUndoProject::DeleteDirectory()
{
	CString csPath = PROJMANAGER->GetPath( m_csProjDir, UndoDir );
	csPath += "//";
	csPath += m_csDirLabel;

	FileManager::DeleteDirectory( csPath );
}

// move the undo project into the real project input dir
void CUndoProject::RestoreToProject()
{
    FileManager fileMan;

	CString csUndoPath = PROJMANAGER->GetPath( m_csProjDir, UndoDir );
	csUndoPath += "//";
	csUndoPath += m_csDirLabel;

	// input
	CString csInputPath = PROJMANAGER->GetPath( m_csProjDir, InputDir );
	CString csPath = PROJMANAGER->GetPath( csUndoPath, InputDir );
	fileMan.CopyDirectory( csPath, csInputPath ,NULL);		


	// econ dir
	CString csEconPath = PROJMANAGER->GetPath( m_csProjDir, EconomicDir );
	csPath = PROJMANAGER->GetPath( csUndoPath, EconomicDir );
	fileMan.CopyDirectory( csPath, csEconPath ,NULL);		

	// remove undo dir
//	FileManager::DeleteDirectory( csUndoPath );
}


int CUndoProject::operator < ( const CUndoProject& _proj ) const
{ 
	return ( m_csDirLabel < _proj.m_csDirLabel );
}

COleDateTime CUndoProject::GetTime()
{
	COleDateTime oTime;
	CString csTemp = m_csDirLabel.Left( 4 );
	int nYear = atoi( csTemp );
	csTemp = m_csDirLabel.Mid( 4, 2 );
	int nMonth = atoi( csTemp );
	csTemp = m_csDirLabel.Mid( 6, 2 );
	int nDate = atoi( csTemp );
	csTemp = m_csDirLabel.Mid( 8, 2 );
	int nHour = atoi( csTemp );
	csTemp = m_csDirLabel.Mid( 10, 2 );
	int nMin = atoi( csTemp );
	csTemp = m_csDirLabel.Mid( 12, 2 );
	int nSecond = atoi( csTemp );

	oTime.SetDateTime( nYear, nMonth, nDate, nHour, nMin, nSecond );
	return oTime;

}
