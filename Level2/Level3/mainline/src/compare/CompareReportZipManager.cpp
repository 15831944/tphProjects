#include "StdAfx.h"
#include "CompareReportZipManager.h"
#include ".\Common\ProjectManager.h"
#include "../Common/ZipInfo.h"
#include "../Common/fileman.h"

CompareReportZipManager::CompareReportZipManager(void)
{
}


CompareReportZipManager::~CompareReportZipManager(void)
{
}

bool CompareReportZipManager::DoImportPara(const CString& strFilePath,const CString& strProjectName)
{
	CString strSourceFile = PROJMANAGER->GetAppPath();
	strSourceFile += _T("\\Comparative Report\\") + strProjectName + _T("\\");
	//delete para files
	CString strModelFile = strSourceFile + _T("modelinfo.txt");
	FileManager::DeleteFile(strModelFile);
	CString strProjectFile = strSourceFile + _T("reportinfo.txt");
	FileManager::DeleteFile(strProjectFile);

	return unzipFiles(strFilePath,strSourceFile);
}

bool CompareReportZipManager::DoExportPara(const CString& strFilePath,const CString& strProjectName)
{
	CString strZipFileName[] = 
	{
		_T("modelinfo.txt"),
		_T("reportinfo.txt"),
	};

	int nFileCcunt = sizeof(strZipFileName)/sizeof(strZipFileName[0]);

	std::vector< CString> vZipFiles;
	CString strTempPath = PROJMANAGER->GetAppPath();
	strTempPath += _T("\\Comparative Report\\") + strProjectName+ _T("\\");

	for (int i = 0; i < nFileCcunt; ++i)
	{	
		CString strDestFile = strTempPath +  strZipFileName[i];
		if(PathFileExists(strDestFile))
			vZipFiles.push_back(strDestFile);
	}

	return ZipFiles(strFilePath,vZipFiles);
}

CString CompareReportZipManager::getTempPath( const CString& _strDefault /*= "c:\\" */  )
{
	char _szTempPath[ MAX_PATH + 1];
	if( GetTempPath( MAX_PATH, _szTempPath ) == 0 )
		return  _strDefault;
	return CString( _szTempPath );
}

bool CompareReportZipManager::DoImportProject(const CString& strFilePath,const CString& strProjectName)
{
	CString strSourceFile = PROJMANAGER->GetAppPath();
	strSourceFile += _T("\\Comparative Report\\") + strProjectName + _T("\\");
	//delete para files
	CString strModelFile = strSourceFile + _T("modelinfo.txt");
	FileManager::DeleteFile(strModelFile);
	CString strProjectFile = strSourceFile + _T("reportinfo.txt");
	FileManager::DeleteFile(strProjectFile);

	//delete result folder
	CString strResultFoder = strSourceFile + _T("Results");
	FileManager::DeleteDirectory(strResultFoder);

	if( !unzipFiles( strFilePath, strSourceFile ) )
	{
		return false;
	}	

	//unzip result file
	CString strResultZip = strSourceFile + _T("Results.zip");
	if (PathFileExists((LPCSTR)strResultFoder) == FALSE)
	{
		CreateDirectory(strResultFoder,NULL);
	}

	strResultFoder += _T("\\");
	if(!unzipFiles(strResultZip,strResultFoder))
	{
		return false;
	}
	FileManager::DeleteFile(strResultZip);
	return true;
}
bool CompareReportZipManager::DoExportProject(const CString& strFilePath,const CString& strFileName,const CString& strProjectName)
{
	//zip result file
	CString strDestPath = PROJMANAGER->GetAppPath();
	strDestPath += _T("\\Comparative Report\\") + strProjectName;
	CString strSrcFolder =  strDestPath +  _T("\\Results\\") ;

	CString  strTempZipFile = getTempPath() + _T("Results.zip");
	zipFloder(strSrcFolder,strTempZipFile);

	CString strZipFileName[] = 
	{
		_T("modelinfo.txt"),
		_T("reportinfo.txt"),
	};

	int nFileCcunt = sizeof(strZipFileName)/sizeof(strZipFileName[0]);

	std::vector< CString> vZipFiles;
	CString strTempPath = PROJMANAGER->GetAppPath();
	strTempPath += _T("\\Comparative Report\\") + strProjectName;

	for (int i = 0; i < nFileCcunt; ++i)
	{	
		CString strDestFile = strTempPath + "\\" + strZipFileName[i];
		if(PathFileExists(strDestFile))
			vZipFiles.push_back(strDestFile);
	}

	vZipFiles.push_back(strTempZipFile);
	return ZipFiles(strFilePath,vZipFiles);
}

bool CompareReportZipManager::ZipFiles( const CString& strFilePath, const std::vector<CString>& vZipFiles )
{
	int iFileCount = vZipFiles.size();
	if (iFileCount == 0)
	{
		return true;
	}

	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vZipFiles[i] );
	}

	CZipInfo InfoZip;
	if (!InfoZip.AddFiles(strFilePath, pFiles, iFileCount))
	{
		return false;
	}
	return true;
}

bool CompareReportZipManager::zipFloder( const CString& _strSrcFolder,CString& _strTempZipFile )
{
	std::vector<CString>	vAddFile;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	if (!SetCurrentDirectory(_strSrcFolder))	//	Folder is not exist
	{
		_strTempZipFile.Empty();
		return true;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) {
		if(FindNextFile(hFind, &FindFileData)) {
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				vAddFile.push_back( _strSrcFolder + "\\" + FindFileData.cFileName );
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);

	int iFileCount = vAddFile.size();
	if (iFileCount == 0)
	{
		_strTempZipFile.Empty();
		return true;
	}

	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vAddFile[i] );
	}


	DeleteFile( _strTempZipFile );

	CZipInfo InfoZip;
	if (!InfoZip.AddFiles(_strTempZipFile, pFiles, iFileCount))
	{
		return false;
	}

	return true;
}

bool CompareReportZipManager::CreateTempZipFile(const CString& strSourceFlie,CString& strTempExtractPath)
{
	CString strTempPath = getTempPath();
	strTempPath.TrimRight('\\');
	strTempExtractPath = strTempPath + "\\" + "ARCTempExtractPath";
	FileManager::DeleteDirectory( strTempExtractPath );
	CreateDirectory( strTempExtractPath, NULL );
	// extract zip to temp path
	if( !unzipFiles( strSourceFlie, strTempExtractPath ) )
	{
		return false;
	}	
	return true;
}

bool CompareReportZipManager::unzipFiles( const CString& _strZip, const CString& _strPath )
{
	if(PathFileExists(_strZip) == FALSE)
		return true;

	CZipInfo InfoZip;


	if (!InfoZip.ExtractFiles(_strZip, CString(_strPath + "\\")))
	{
		return false;
	}

	return true;
}