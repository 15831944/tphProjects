#pragma once

#include "../Reports/ReportType.h"
class ArctermFile;
class Terminal;
class CReportParameter;
class CCompareModel
{
public:
	CCompareModel(void);
	virtual ~CCompareModel(void);


public:
	const CString& GetModelName() const;
	const CString& GetModelLocation() const;
	//   const Terminal* GetTerminal(CCompRepLogBar* pStatus, CString strName, void (CALLBACK* _ShowCopyInfo)(LPCTSTR));
	void SetModelName(const CString& strName);
	void SetUniqueName(const CString& strName);
	const CString& GetUniqueName() const;
	void SetModelLocation(const CString& strLocation);


	const CString& GetLocalPath()const;
	void SetLocalPath(const CString& strLocalPath);
	const CString& GetDataBasePath()const;
	void SetDataBasePath(const CString& strDataBasePath);
	const CString& GetLastModifyTime()const;
	void SetLastModifyTime(const CString& strLastTime);

	void AddSimResult(const CString& strSimResult);
	int GetSimResultCount() const;
	CString GetSimResult(int nIndex) const;
	void ClearSimResult();

	bool IsValid(){ return m_bIsValid;}

public:
	virtual void readData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;

	void GenerateReport(ENUM_REPORT_TYPE enumRepType,int nDetailOrSummary,void *pReportResult,CReportParameter *pReportParam,const CString& strProjPath, Terminal *pTerminal, int nFloorCount);

protected:
	CString		m_strModelName;//model name
	CString		m_strUniqueName;//ServerName.ModelName
	CString     m_strModelLocation;//network path
	CString     m_strLocalPath;//local path
	CString		m_strDatabasePath;//database path
	CString		m_lastModifiedTime;//last modify time

	std::vector<CString> m_vSimResult;

	bool m_bIsValid;
};

class CCompareModelList
{
public:
	CCompareModelList();
	~CCompareModelList();


public:
	void ReadData(const CString& strFile);
	void SaveData(const CString& strFile);

	void AddModel(const CCompareModel& compareModel);
	void DelModel(int nIndex);
	void DelModel(const CString& strModelName);

	void SetModelSelectedSimResult(int nIndex,std::vector<CString>& vSimResilt);
	CCompareModel& GetModel(int nIndex);

	int GetCount();

	void Clear();

	void GenerateReport(ENUM_REPORT_TYPE enumRepType,int nDetailOrSummary,void *pReportResult,CReportParameter *pReportParam,const CString& strProjPath, Terminal *pTerminal, int nFloorCount);
protected:

	float m_fVersion;
	std::vector<CCompareModel> m_vCompareModel;
private:


	void readData(ArctermFile& p_file);
	void writeData(ArctermFile& p_file) const;

	virtual const char* getTitle() const{return "Model Information";}
	virtual const char* getHeaders() const{return "Name, Location";}

};
