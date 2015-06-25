// ProjectManager.h: interface for the CProjectManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTMANAGER_H__C74E5026_945F_4A90_AD70_80546E823790__INCLUDED_)
#define AFX_PROJECTMANAGER_H__C74E5026_945F_4A90_AD70_80546E823790__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#include <vector>
#include "commondll.h"
#include "template.h"
class CopyFileParam;
#define PROJECT_VERSION 303
#define PROJMANAGER	CProjectManager::GetInstance()
#define DESTROY_PROJMANAGER	CProjectManager::DeleteInstance();

typedef struct _projinfo {
	CString name;
	CString user;
	CString machine;
	CString path;
	CTime	createtime;
	CTime	modifytime;
	int		version;
	int		isDeleted;//add by chris in 3/9/2002
	long	lUniqueIndex;
	CString dbname;
    //add 
	BOOL m_IsChanged ;
	_projinfo(){
     m_IsChanged = FALSE ;
	}
} PROJECTINFO;


class COMMON_TRANSFER CProjectManager  
{
private:
	static CProjectManager* m_pInstance;

protected:
public:
	CString m_csSwapDrive;
	CString m_csProbDistPath;
	CString m_csACTypePath;
	CString m_csAirportPath;
	CString m_csRootProjectPath;
	CString m_csAppPath;
	CString m_csDefaultDBPath;
	CProjectManager();
	virtual ~CProjectManager();

	BOOL CreateProjInfoFile(PROJECTINFO pi);
	BOOL UpdateProjInfoFile( PROJECTINFO pi );

    // combines the passed dir and file enums to return a file name
	int getFullName( const CString& _csProjDir, int dir, int fileType, char *filename, int mode = 1, int airport = 0);
//		mode == enum EnvironmentMode; 0 = EnvMode_LandSide, 1 = EnvMode_Terminal, 2 = EnvMode_AirSide;
//		airport index base 0;

    // extracts name at lineNum of file name listing
	int getCurrentFileName( const CString& _csProjDir, int lineNum, char *str, int mode = 1, int airport = 0);

    void getSwapDrive (void);

    void cleanSwapDrive (void);

	void createSubDirectories ( const char* projectDir);

public:
	CString GetAppPath(){ return m_csAppPath; }
	CString GetGlobalDBListFilePath( void ) const;
	CString GetLogDebugFilePath( void ) const;
	CString GetProbDistFilePath();
	CString GetACTypeFilePath();
	CString GetACCatFilePath();
	CString GetAirportFilePath();
	CString GetSectorFilePath( );
	CString GetAirlineFilePath();
	CString GetSubairlineFilePath();
	const CString& GetDefaultDBPath( void ) const;
	
    void getUniqueFileName (char *p_str);

    long getUniqueFileNumber (void);

    void makeFileName (char *filename, long fileNumber);

    int fileExists (char *filename);

    int getLogFileName (const CString& _csProjPath, int fileType, char *filename)
        { return getFullName ( _csProjPath, LogsDir, fileType, filename); };

    int getBackupFileName (const CString& _csProjPath, int fileType, char *filename)
        { return getFullName ( _csProjPath, BackupDir, fileType, filename); };

    int getEchoFileName (const CString& _csProjPath, int fileType, char *filename)
        { return getFullName ( _csProjPath, EchoDir, fileType, filename); };

    int getInputFileName (const CString& _csProjPath, int fileType, char *filename)
		{ return getFullName ( _csProjPath, InputDir, fileType, filename); };

    int getSpecFileName (const CString& _csProjPath, int fileType, char *filename)
        { return getFullName ( _csProjPath, StatSpecDir, fileType, filename); };

    int getReportFileName (const CString& _csProjPath, int fileType, char *filename)
        { return getFullName ( _csProjPath, ReportDir, fileType, filename); };

	int getFileName(const CString& _csProjPath, int fileType, char *filename, int mode = 1, int airport = 0);
	
	void SetPath(	const CString& _csAppPath, 
					const CString& _csRootProjectPath,
					const CString& _csAirportPath,
					const CString& _csACTypePath,
					const CString& _csProbDistPath,
					const CString& _csDefaultDBPath);

	static CProjectManager* GetInstance();
	static void SetInstance(CProjectManager* pInst);
	static void DeleteInstance();

	void GetProjectList(std::vector<CString>& vList);
	BOOL GetProjectInfo(CString projName, PROJECTINFO* pi, const CString& strNewFolder = _T(""));

	BOOL CreateProject(CString _projName, PROJECTINFO* pi, bool _bNeedToCopyDefFile = true );
	BOOL CreateDefaultProject(PROJECTINFO* pi);

	BOOL DeleteProject(PROJECTINFO pi);

	// Copy current project into new project of projName.
	BOOL CopyProject( CString proName, CString proPath, PROJECTINFO pi,CopyFileParam *pParam );

	// Give a proj path, and dir type, return path
	CString GetPath( const CString& _csProjPath, int _nDirType, int mode = 1, int airport = 0 );

public:
	
	void SetProjectDeletedFlag(CString projectName, bool flag);
};

#endif // !defined(AFX_PROJECTMANAGER_H__C74E5026_945F_4A90_AD70_80546E823790__INCLUDED_)
