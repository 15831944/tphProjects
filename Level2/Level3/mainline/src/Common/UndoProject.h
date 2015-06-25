// UndoProject.h: interface for the CUndoProject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOPROJECT_H__420313A9_ACB5_44E8_B5C7_BE57DB166BAD__INCLUDED_)
#define AFX_UNDOPROJECT_H__420313A9_ACB5_44E8_B5C7_BE57DB166BAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUndoProject  
{
public:
	COleDateTime GetTime();
	int operator < ( const CUndoProject& _proj ) const;
	
	void RestoreToProject();

	// base on the direct name delete the directory, for now there is now two times deletion protection.
	void DeleteDirectory();

	// Description: Copy INUPT / ECONOMIC dir to undo dir.
	// Exception:	NO;
	bool CreatDirectory();

	CUndoProject( const CString& _csProjDir, const CString& _csDirLabel );
	virtual ~CUndoProject();

protected:
	CString m_csProjDir;
	CString m_csDirLabel;
};

#endif // !defined(AFX_UNDOPROJECT_H__420313A9_ACB5_44E8_B5C7_BE57DB166BAD__INCLUDED_)
