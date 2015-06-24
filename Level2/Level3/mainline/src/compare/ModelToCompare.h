// ModelToCompare.h: interface for the CModelToCompare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELTOCOMPARE_H__27043DE6_2A62_4B94_96C5_B5DF0E42C8B7__INCLUDED_)
#define AFX_MODELTOCOMPARE_H__27043DE6_2A62_4B94_96C5_B5DF0E42C8B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../common/dataset.h"
#include <vector>
#include "../Main/CompRepLogBar.h"
#include "fileoperations.h"
#include "ComparativeReportResultList.h"
#include "..\Common\AirportDatabase.h"

//using namespace std;
class Terminal;
class CModelToCompare  
{
public:
	CModelToCompare();
	~CModelToCompare();

public:
	const CString& GetModelName() const;
	const CString& GetModelLocation() const;
 //   const Terminal* GetTerminal(CCompRepLogBar* pStatus, CString strName, void (CALLBACK* _ShowCopyInfo)(LPCTSTR));
	void SetModelName(const CString& strName);
	void SetUniqueName(const CString& strName);
	const CString& GetUniqueName() const;
	void SetModelLocation(const CString& strLocation);

	Terminal *GetTerminal();
	InputTerminal* GetInputTerminal();
	const CString& GetLocalPath()const;
	void SetLocalPath(const CString& strLocalPath);
	const CString& GetDataBasePath()const;
	void SetDataBasePath(const CString& strDataBasePath);
	const CString& GetLastModifyTime()const;
	void SetLastModifyTime(const CString& strLastTime);

	void SetTerminal(const Terminal& _terminal);

	bool IsNeedToCopy(){ return m_bNeedCopy;}

	CModelToCompare& operator = ( const CModelToCompare& _rhs )
	{
		m_strModelName = _rhs.m_strModelName;
		m_strModelLocation = _rhs.m_strModelLocation;
		m_strLocalPath = _rhs.m_strLocalPath;
		m_strUniqueName = _rhs.m_strUniqueName;
		m_lastModifiedTime = _rhs.m_lastModifiedTime;
		m_strDatabasePath = _rhs.m_strDatabasePath;	
		m_terminal = _rhs.m_terminal;

		return *this;
	}

	BOOL TransferFiles(const CString& strSource, const CString& strDest,void (CALLBACK* _ShowCopyInfo)(LPCTSTR));
	void RemoveFiles(const CString &strPath);


	void AddSimResult(const CString& strSimResult);
	int GetSimResultCount();
	CString GetSimResult(int nIndex);
	void ClearSimResult();

	Terminal*  InitTerminal(CCompRepLogBar* pStatus, CString strName, void (CALLBACK* _ShowCopyInfo)(LPCTSTR));



	BOOL IsLocalModel(const CString &strPath);
private:
	CAirportDatabase* OpenProjectDBForInitNewProject(const CString& sAirportName);

private:
	CString		m_strModelName;//model name
	CString		m_strUniqueName;//ServerName.ModelName
	CString     m_strModelLocation;//network path
	CString     m_strLocalPath;//local path
	CString		m_strDatabasePath;//database path
	CString		m_lastModifiedTime;//last modify time
	
	std::vector<CString> m_vSimResult;
 
	Terminal*   m_terminal;
	bool m_bNeedCopy;
};


class CModelToCompareDataSet : public DataSet
{
public:
	CModelToCompareDataSet();
	virtual ~CModelToCompareDataSet();
	
	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Model Information";}
	virtual const char* getHeaders() const{return "Name, Location";}
	
	
	int GetModels(OUT std::vector<CModelToCompare *>& vModels);
	void SetModels(const std::vector<CModelToCompare *>& vModels);

	const CString& GetProjectName() const{ return m_strProjName;}
	void SetProjName(const CString& strProjName){ m_strProjName = strProjName;}
protected:
	std::vector<CModelToCompare *>	m_vModels;
	CString m_strProjName;
};

#endif // !defined(AFX_MODELTOCOMPARE_H__27043DE6_2A62_4B94_96C5_B5DF0E42C8B7__INCLUDED_)
