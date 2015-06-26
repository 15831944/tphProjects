#pragma once
#include <vector>

const static int UPDATEERROR  = -1;
const static int UPDATESUCCESS = 1;
const static int ERRORVERSION = -1;
class DBUpdateItem
{
public:
	DBUpdateItem();
	~DBUpdateItem();

public:
	int getVersion();
public:
	int Update();
	void ReadItem(const CString&strARCTermPath, char* szline);
	bool ParserSQLScriptFile(LPCTSTR lpszSQLFile,std::vector<std::string >& vSQLScript);

protected:
	int m_nVersion;
	CString m_strFullPath;
};

class DBUpdateConfigure
{
public:
	DBUpdateConfigure();
	~DBUpdateConfigure();

public:

	void ReadConfig(const CString& strARCTermPath);
	bool CheckConfig();
	
	int Update(int& nCurVersion);
protected:
	CString m_strFullPath;

	std::vector<DBUpdateItem> m_vUpdateItem;
private:
};

class DBUpdater
{
public:
	DBUpdater(const CString& strArctermPath);
	~DBUpdater(void);

public:
	//return the up-to-date version
	int Update();
	int Version();
protected:
	CString m_strARCTermPath;


};
