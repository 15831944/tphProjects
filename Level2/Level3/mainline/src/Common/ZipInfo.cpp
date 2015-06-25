#include "stdafx.h"
#include "ZipInfo.h"
#include "7ziplib\SevenZipLibrary.h"
#include "7ziplib\SevenZipCompressor.h"
#include "7ziplib\SevenZipExtractor.h"

///////////////////////////////////////////////
CZipInfo::CZipInfo()
{
	/*m_hZipHandle = NULL;
	m_hUnZipHandle = NULL;*/
}

CZipInfo::~CZipInfo()
{
	Clear();
}
using namespace SevenZip;
SevenZipLibrary lib;

BOOL CZipInfo::AddFiles(const char *pszArchive, char ** paFiles, int iFileCount)
{	
	try
	{
		lib.Load();
		StringList fileList(paFiles,paFiles+iFileCount);
		SevenZipCompressor compress(lib, pszArchive);
		compress.CompressFiles(fileList);
	}
	catch (SevenZipException& e)
	{		
		TString s = e.GetMessage();
		return FALSE;
	}	
	return TRUE;


	/*m_hZipHandle = CreateZip((void *)pszArchive, 0, ZIP_FILENAME);
	if (m_hZipHandle == NULL)
	{
	return FALSE;
	}

	for (int i = 0; i < iFileCount; i++)
	{
	TCHAR * pszName = (TCHAR *)_tcsrchr(paFiles[i], _T('\\'));
	if (pszName == NULL)
	pszName = (TCHAR *) paFiles[i];
	else
	pszName++;

	if (ZipAdd(m_hZipHandle,pszName,paFiles[i],0,ZIP_FILENAME) != ZR_OK)
	{
	return FALSE;
	}
	}

	CloseZip(m_hZipHandle);
	return TRUE;*/
}

BOOL CZipInfo::AddFiles( const String& szArcheive,const StringList& files )
{
	
	try
	{		
		lib.Load();
		SevenZipCompressor compress(lib, szArcheive);
		compress.CompressFiles(files);
	}
	catch (SevenZipException& e)
	{		
		TString s = e.GetMessage();
		return FALSE;
	}
	return TRUE;

	/*m_hZipHandle = CreateZip((void *)szArcheive.c_str(), 0, ZIP_FILENAME);
	if (m_hZipHandle == NULL)
	{
	return FALSE;
	}
	int iFileCount = (int)files.size();
	for (int i = 0; i < iFileCount; i++)
	{
	String filePath = files[i];
	TCHAR * pszName = (TCHAR *)_tcsrchr(filePath.c_str(), _T('\\'));
	if (pszName == NULL)
	pszName = (TCHAR *) filePath.c_str();
	else
	pszName++;

	if (ZipAdd(m_hZipHandle,pszName,(void*)filePath.c_str(),0,ZIP_FILENAME) != ZR_OK)
	{
	return FALSE;
	}
	}

	CloseZip(m_hZipHandle);
	return TRUE;*/
}
BOOL CZipInfo::ExtractFiles(const char* pszArchive, const char* pszTargetFolder)
{
	try
	{
		lib.Load();
		SevenZipExtractor extrator(lib, pszArchive);
		extrator.ExtractArchive(pszTargetFolder);//CompressFiles(paFiles,iFileCount);
	}
	catch (SevenZipException& e)
	{		
		//lib.Free();
		TString s = e.GetMessage();
		return FALSE;
	}
	//lib.Free();
	return TRUE;
	/*HZIP m_hUnZipHandle = OpenZip((void *)pszArchive, 0, ZIP_FILENAME);
	if (m_hUnZipHandle == NULL)
	{
	return FALSE;
	}

	#ifdef _UNICODE
	ZIPENTRYW ze;
	#else
	ZIPENTRY ze; 
	#endif
	memset(&ze, 0, sizeof(ze));
	int nIndex = -1;
	ZRESULT zr = 0;

	GetZipItem(m_hUnZipHandle, -1, &ze); 

	int iFileCount = ze.index;
	for (int i = 0; i < iFileCount; i++)
	{
	GetZipItem(m_hUnZipHandle, i, &ze);

	if(FindZipItem(m_hUnZipHandle, ze.name, TRUE, &nIndex, &ze)!= ZR_OK)
	return FALSE;

	TCHAR targetname[MAX_PATH];
	_tcscpy(targetname, _T(""));
	_tcscat(targetname, pszTargetFolder);

	_tcscat(targetname,ze.name);
	UnzipItem(m_hUnZipHandle, nIndex, targetname, 0, ZIP_FILENAME);
		SetFileAttributes(targetname,FILE_ATTRIBUTE_NORMAL );
	}

	CloseZip(m_hUnZipHandle);
	return TRUE;*/
}

void CZipInfo::Clear()
{
	/*if (m_hUnZipHandle != NULL)
	{
	CloseZip(m_hUnZipHandle);
	}

	if (m_hZipHandle != NULL)
	{
	CloseZip(m_hZipHandle);
	}*/
}



BOOL CZipInfo::AddDirectory( const CString& szArcheive, const CString& dir )
{
	CFileFind finder;
	// build a string with wildcards
	CString strWildcard(dir);
	strWildcard += _T("\\*.*");

	StringList strlist;
	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;

		CString sFileName = finder.GetFileName();
		strlist.push_back( (dir + sFileName).GetString());
	}

	return AddFiles(szArcheive.GetString(),strlist);
}