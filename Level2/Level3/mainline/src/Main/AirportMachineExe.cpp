// AirportMachineExe.cpp: implementation of the CAirportMachineExe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AirportMachineExe.h"

#include <Shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAirportMachineExe::CAirportMachineExe()
{

}

CAirportMachineExe::~CAirportMachineExe()
{

}

bool CAirportMachineExe::GetAirportMachineFilePath(CString& strFilePath)
{
	CString strAppPath(PROJMANAGER->GetAppPath());
	strFilePath = strAppPath + _T("\\AirportMachine\\AM3D4_5.exe");
	if (PathFileExists(strFilePath))
	{
		return true;
	}

	HKEY RegKey;
	unsigned char szReigterWord[256];
	unsigned long lDataLength;
	CString strRegSubKey("SOFTWARE\\Aviation Research Corporation\\ARCTerm\\Paths");
	CString strRegValueName("AirportMachineExePath");
	
	if (RegOpenKey(HKEY_LOCAL_MACHINE, strRegSubKey, &RegKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(RegKey, strRegValueName, 0, NULL, szReigterWord, &lDataLength) == ERROR_SUCCESS)
		{
			strFilePath = CString(szReigterWord);
			RegCloseKey(RegKey);
			if (PathFileExists(strFilePath))
				return true;
		}
		RegCloseKey(RegKey);
	}

	// can not find the airport machine path in the registry 
	if(AfxMessageBox(_T("Can't find AirportMachine files.\nDo you want to specify the path of the folder?"), MB_YESNO)
						== IDYES)
	{
		TCHAR szFilter[] = "AirportMachine File (*.exe)|*.exe||";
		CFileDialog file(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, szFilter, NULL);
		if (file.DoModal() == IDOK)
		{
			strFilePath = file.GetPathName();
			if (RegCreateKey(HKEY_LOCAL_MACHINE, strRegSubKey, &RegKey) == ERROR_SUCCESS)
			{
				if(RegSetValueEx(RegKey, strRegValueName, 0, REG_SZ, (const BYTE*)((LPCTSTR)strFilePath), strFilePath.GetLength()) == ERROR_SUCCESS)
				{
					RegCloseKey(RegKey);
					return true;
				}
				RegCloseKey(RegKey);
			}
		}
	}

	return false;
}

bool CAirportMachineExe::GetAirportMachineDataPath(CString& strDataDir)
{
	CString strAppPath(PROJMANAGER->GetAppPath());
	strDataDir = strAppPath + _T("\\AirportMachine\\Data");
	if (PathFileExists(strDataDir))
	{
		return true;
	}

	HKEY RegKey;
	unsigned char szReigterWord[256];
	unsigned long lDataLength;
	CString strRegSubKey("SOFTWARE\\Aviation Research Corporation\\ARCTerm\\Paths");
	CString strRegValueName("AirportMachineDataPath");

	if (RegOpenKey(HKEY_LOCAL_MACHINE, strRegSubKey, &RegKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(RegKey, strRegValueName, 0, NULL, szReigterWord, &lDataLength) == ERROR_SUCCESS)
		{
			strDataDir = CString(szReigterWord);
			RegCloseKey(RegKey);
			if (PathFileExists(strDataDir))
				return true;
		}
		RegCloseKey(RegKey);
	}

	// can not find the airport machine data path in the registry 
	if(AfxMessageBox(_T("Can't find AirportMachine data files.\nDo you want to specify the path of the folder?"), MB_YESNO)
						== IDYES)
	{
		CFileDialog file(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT,	NULL, NULL);
		if (file.DoModal() == IDOK)
		{
			strDataDir = file.GetPathName();
			strDataDir = strDataDir.Left(strDataDir.ReverseFind(_T('\\')));
			if (RegCreateKey(HKEY_LOCAL_MACHINE, strRegSubKey, &RegKey) == ERROR_SUCCESS)
			{
				if(RegSetValueEx(RegKey, strRegValueName, 0, REG_SZ, (const BYTE*)((LPCTSTR)strDataDir), strDataDir.GetLength()) == ERROR_SUCCESS)
				{
					RegCloseKey(RegKey);
					return true;
				}
				RegCloseKey(RegKey);
			}
		}
	}
	return false;
}

bool CAirportMachineExe::FindFileByEx(LPCTSTR DirName, LPCTSTR FileEx, CString& strFindFile)
{
	TCHAR lpBuffer[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, lpBuffer);
	if (lpBuffer != DirName)
		SetCurrentDirectory(DirName);

	WIN32_FIND_DATA	FindFileData;
	HANDLE hFindFile = FindFirstFile(FileEx, &FindFileData);
	if (INVALID_HANDLE_VALUE != hFindFile)
	{
		strFindFile = FindFileData.cFileName;
		FindClose(hFindFile);
	}
	else
	{
		SetCurrentDirectory(lpBuffer);
		return false;
	}

	SetCurrentDirectory(lpBuffer); 
	return true;
}

void CAirportMachineExe::Run()
{
	CString strFilePath;
	CString strDataDir;
	if (!GetAirportMachineFilePath(strFilePath) || !GetAirportMachineDataPath(strDataDir))
		return;

	CString strRun, strRwc, strGts, strSch, strSgs, strRpt, strGrp;
	if (!FindFileByEx(strDataDir, _T("*.run"), strRun))
		return;
	if (!FindFileByEx(strDataDir, _T("*.rwc"), strRwc))
		return;
	if (!FindFileByEx(strDataDir, _T("*.gts"), strGts))
		return;
	if (!FindFileByEx(strDataDir, _T("*.sch"), strSch))
		return;
	if (!FindFileByEx(strDataDir, _T("*.sgs"), strSgs))
		return;
	if (!FindFileByEx(strDataDir, _T("*.rpt"), strRpt))
		return;
	if (!FindFileByEx(strDataDir, _T("*.grp"), strGrp))
		return;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	
	CString strProcess;
	strProcess.Format(_T("\"%s\" %s %s %s %s %s ec %s %s lg null"),
						 strFilePath, strRun, strRwc, strGts, strSch, strSgs, strRpt, strGrp);
	TCHAR szProcess[MAX_PATH + 64];
	lstrcpy(szProcess, strProcess);
	CreateProcess(NULL, szProcess, NULL, NULL, FALSE, 0, NULL, strDataDir, &si, &pi);
}
