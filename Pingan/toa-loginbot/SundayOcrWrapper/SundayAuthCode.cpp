#include <windows.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>

#pragma warning(disable:4996)

typedef bool (__stdcall * fnGetCodeFromFile)(int,char * ,char * );

typedef int (__stdcall * fnLoadLibFromFile)(char *, char *);

bool GetCode(HMODULE hLib, int codeIndex, const std::string & ImgPath , std::string & vcode )
{		
	fnGetCodeFromFile pfnGetCodeFromFile = (fnGetCodeFromFile)GetProcAddress(hLib, "GetCodeFromFile");
	if (pfnGetCodeFromFile == NULL){
		return false;
	}
	
	char * pszImagePath = strdup(ImgPath.c_str());
	char szResult[MAX_PATH] = "";
	bool fStat = pfnGetCodeFromFile(codeIndex, pszImagePath, szResult);
	free(pszImagePath);
	vcode = szResult;
	return fStat;
}

HMODULE InitializeOCRLib(const std::string & currdir, const std::string bankid, int * pCodeIndex)
{
	std::string sSundayDllPath = currdir;
	sSundayDllPath += "\\";
	if (!stricmp(bankid.c_str(), "cmb")){
		sSundayDllPath += "SundayCMB.dll";
	}else{
		sSundayDllPath += "Sunday.dll";
	}

	HMODULE hSundyDll = LoadLibraryA(sSundayDllPath.c_str());

	fnLoadLibFromFile pfnLoadLibFromFile = (fnLoadLibFromFile)GetProcAddress(hSundyDll, "LoadLibFromFile");
	if (pfnLoadLibFromFile == NULL){
		FreeLibrary(hSundyDll);
		return NULL;
	}

	std::string codebasedir = currdir;
	codebasedir += "\\";
	codebasedir += "index";
	codebasedir += "\\";
	codebasedir += bankid;

	struct _stat32 st;
	if (_stat32(codebasedir.c_str(), &st) == -1){
		FreeLibrary(hSundyDll);
		return NULL;
	}

	char * tempcodepath = strdup(codebasedir.c_str());
	int codeIndex = pfnLoadLibFromFile(tempcodepath, "123");
	free(tempcodepath);

	if (codeIndex == -1){
		free(hSundyDll);
		return NULL;
	}

	*pCodeIndex = codeIndex;

	return hSundyDll;
}

bool  GetFileList(std::string & path, std::vector<std::string> & filelist) 
{ 
	WIN32_FIND_DATAA wfd = {0 }; 
	std::string pattern = path;
	pattern += "\\";
	pattern += "*";

	HANDLE hFind = FindFirstFileA(pattern.c_str(), &wfd); 
	if (hFind == INVALID_HANDLE_VALUE) 
		return false; 

	while (FindNextFileA(hFind, &wfd)) 
	{ 
		if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, "..")) 
		{
			continue; 
		} 
		std::string filepath = path;
		filepath += "\\";
		filepath += wfd.cFileName;

		filelist.push_back(filepath);
	} 

	FindClose(hFind); 
	hFind = INVALID_HANDLE_VALUE;
	return filelist.size() != 0;
} 

