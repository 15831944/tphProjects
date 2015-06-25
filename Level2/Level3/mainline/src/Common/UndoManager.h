// UndoManager.h: interface for the CUndoManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOMANAGER_H__A986F029_54FA_40EF_948D_CFEAC73CD53B__INCLUDED_)
#define AFX_UNDOMANAGER_H__A986F029_54FA_40EF_948D_CFEAC73CD53B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
#include "UndoProject.h"


class CUndoManager  
{
public:
	void RestoreTheProject( int _nIdx );
	CUndoProject GetProject( int _nIdx );
	int GetProjCount();

	// base on the current project, copy to the new undo project.
	// Exception:	NO;
	bool AddNewUndoProject();

   static  CUndoManager* GetInStance(const CString& projpath) ;
	virtual ~CUndoManager();
private:
	CUndoManager( const CString& _csProjPath );
private:
   static CUndoManager*  P_singleTone ;
protected:
	void CleanDirectory();
	void CheckDirectory();
	void LoadProjectList();

	CString m_csProjPath;
	int m_nUndoLength;
	typedef std::vector<CUndoProject> UNDOPROJECT_LIST ;
	typedef UNDOPROJECT_LIST::iterator UNDOPROJECT_LIST_ITER ;
	UNDOPROJECT_LIST m_vUndoProj;
};

#endif // !defined(AFX_UNDOMANAGER_H__A986F029_54FA_40EF_948D_CFEAC73CD53B__INCLUDED_)
