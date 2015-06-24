#pragma once

class CMainFrame;
class CTermPlanDoc;
class CAirportDatabase;



class CProjectExport
{
public:
	CProjectExport(CMainFrame *pMainFrame,CTermPlanDoc *pTermplanDoc);
	~CProjectExport(void);


public:
	void ExportAsInputZip(const CString& strProjName, const CString& strProjPath,bool bNotForceDB,bool bOpenProject = true);
	BOOL ExportAndEmail(const CString& strProjName,const CString& strProjPath,bool bNotForceDB,bool bOpenProject = true);



protected:
	BOOL zipFloder(const CString& strProjName, const CString& _strSrcFolder, CString& _strTempZipFile);
	BOOL ZipAirportInfo(const CString& strProjName,CString projPath,std::vector<CString> &vAirportName);


	BOOL zipFloder(const CString& strProjName, const CString& _strSrcFolder,std::vector<CString> vAddFile,CString& _strTempZipFile);

	bool zipProjectDB( const CString& strProjName, CString& _strTempDBZipFile );
	bool zipTempZipToTermzip( const CString& strProjName, const CString& strProjPath,
		const CString& _strTempInputZip, const CString& _strTempDBZip, 
		const CString& _strTempMathematicZip, const CString& _strTempMathResultZip,const CString& _strTmpEconZip,
		const CString& _strTerminalZip,bool bNotForceDB,CTermPlanDoc* _pTermDoc=NULL );
	void createArcExportIniFile( const CString& _strTempInputZip, const CString& _strTempDBZip, 
		const CString& _strTempMathematicZip, const CString& _strTempMathResultZip,
		CString& _strTmpIniFile ,bool bNotForceDB);

	void clear2VArray( char** _p2VArray, int _xCount );
	CString getTempPath( const CString& _strDefault = "c:\\" );
	bool ExportProjDBInfo(const CString& strProjName,CAirportDatabase*  pDB,const CString& sDestPath,CString& sFullProjInfoFile);

	
	//folder, ../input/airport1/airside
	BOOL ZipInputAirportAirsideFiles(const CString& strProjName, const CString& _strSrcFolder,CString& _strTempZipFile);
	//folder, ../input/airport1/landside
	BOOL ZipInputAirportLandsideFiles(const CString& strProjName, const CString& _strSrcFolder,CString& _strTempZipFile);
	
	//folder,../input/airside
	BOOL ZipInputAirsideFiles(const CString& strProjName,const CString& projPath, CString& strZipFilePath);

	//folder ../input/landside
	BOOL ZipInputlandsideFiles(const CString& strProjName, const CString& projPath, CString& strZipFilePath);

	//../input
	BOOL ZipInputFiles(const CString& strProjName,const CString& projPath, CString& strZipFilePath);
	BOOL FindInputFiles(const CString& strInputFolder, std::vector<CString >& vInputFiles);
	
	//../Mathematic
	BOOL ZipMathmaticFiles(const CString& strProjName,const CString& projPath, CString& strZipFilePath);
	
	//../MathResult
	BOOL ZipMathResultFiles(const CString& strProjName,const CString& projPath, CString& strZipFilePath);

	BOOL ZipEconomicFiles(const CString& strProjName,const CString& projPath, CString& strZipFilePath);
	//************************************
	// Method:    ZipFiles
	// FullName:  CProjectExport::ZipFiles
	// Access:    protected 
	// Returns:   BOOL
	// Qualifier:
	// Parameter: const CString & strProjName, The projectName
	// Parameter: const std::vector<CString> & vFiles, the files to zip
	// Parameter: const CString & _strSrcFolder, the files' Path
	// Parameter: CString & _strTempZipFile, return the temp zip file full path
	//************************************
	BOOL ZipFiles(const CString& strProjName, const std::vector<CString>& vFiles,const CString& _strSrcFolder, CString& _strTempZipFile);
	

	//database file path		:_strSrcFolder
	//the files need to zip		: VFiles
	//project name				: strProjName
	//if zip,return true
	//else return false
	BOOL ZipAirsideDatabaseFiles( const CString& sProjectPath,const CString& strProjName,std::vector<CString>& vInputFiles );


	BOOL ZipImportDB(const CString& strProjName,CString projPath,std::vector<CString> &vAirportName  ) ;
	BOOL ZipImportDBDataFiles(const CString& strProjName, const CString& _strSrcFolder,CString& _strTempZipFile) ;
	BOOL GetFileNameUnderFloder(const CString& _path, std::vector<CString>& _Filenames) ;
protected:
	CMainFrame *m_pMainFrame;
	CTermPlanDoc *m_pTermPlanDoc;

private:
	void BeginWaitCursor();
	void EndWaitCursor();
};
