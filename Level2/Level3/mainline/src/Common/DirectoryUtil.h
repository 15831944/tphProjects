// DirectoryUtil.h: interface for the CDirectoryUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTORYUTIL_H__584E0604_D40A_4553_88D6_A972B1CEEA50__INCLUDED_)
#define AFX_DIRECTORYUTIL_H__584E0604_D40A_4553_88D6_A972B1CEEA50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirectoryUtil  
{
private:
	CDirectoryUtil();
	virtual ~CDirectoryUtil();
//static BOOL PreRemoveDirectory(CString DirName); add by tutu
public:
	static BOOL RemoveDirectoryR(LPCTSTR lpPathName);
	static BOOL EraseAllFilesFromDirectory(LPCTSTR lpPathName);
//	static BOOL RemoveDir(LPCTSTR lpPathName); add by tutu
	
};

#endif // !defined(AFX_DIRECTORYUTIL_H__584E0604_D40A_4553_88D6_A972B1CEEA50__INCLUDED_)
