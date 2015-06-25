#ifndef ZIPINFO_H
#define ZIPINFO_H

#include <io.h>
//#include "XZip.h"
//#include "XUnzip.h"
#include <string>

class CZipInfo
{
public:
	CZipInfo();
	~CZipInfo();

	BOOL AddFiles(const char *pszArchive, char ** paFiles, int iFileCount);
	BOOL ExtractFiles(const char* pszArchive, const char* pszTargetFolder);



	typedef std::string String;
	typedef std::vector<String> StringList;
	BOOL AddFiles(const String& szArcheive,const StringList& files);
	BOOL AddDirectory(const CString& szArcheive, const CString& dir);
protected:
	void Clear();

private:
	//HZIP m_hZipHandle;
	//HZIP m_hUnZipHandle;
};

#endif