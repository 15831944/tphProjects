#pragma once
class CompareReportZipManager
{
public:
	CompareReportZipManager(void);
	~CompareReportZipManager(void);

	static bool DoImportPara(const CString& strFilePath,const CString& strProjectName);
	static bool DoExportPara(const CString& strFilePath,const CString& strProjectName);

	static bool DoImportProject(const CString& strFilePath,const CString& strProjectName);
	static bool DoExportProject(const CString& strFilePath,const CString& strFileName,const CString& strProjectName);

private:
	static bool ZipFiles( const CString& strFilePath, const std::vector<CString>& vZipFiles );
	static bool zipFloder( const CString& _strSrcFolder,CString& _strTempZipFile );
	static CString getTempPath( const CString& _strDefault = "c:\\");
	static bool CreateTempZipFile(const CString& strSourceFlie,CString& strTempExtractPath);
	static bool unzipFiles( const CString& _strZip, const CString& _strPath );
private:
};

