// ComparativeProject.h: interface for the CComparativeProject class.
// Author: Kevin Chang
// Copyright (c) 2005		ARC Inc.,
// Last Modified: 2005-5-13 11:05
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVEPROJECT_H__60D721DE_EF29_495E_8F6F_2F5D10A58A65__INCLUDED_)
#define AFX_COMPARATIVEPROJECT_H__60D721DE_EF29_495E_8F6F_2F5D10A58A65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef COMPARE_TRANSFER
#if defined COMPARE_EXPORT
#define COMPARE_TRANSFER  __declspec( dllexport ) 
#elif defined COMPARE_IMPORT
#define COMPARE_TRANSFER __declspec( dllimport ) 
#else
#define COMPARE_TRANSFER
#endif
#endif



#include "InputParameter.h"
#include "../Reports/ReportParameter.h"
#include "ComparativeReportResultList.h"
#include "fileoperations.h"
#include "../Main/CompRepLogBar.h"

#define  CMPPROJECTMANAGER CComparativeProjectDataSet::GetInstance()

class CComparativeProject
{
public:
	CComparativeProject();
	virtual ~CComparativeProject();


public:
	const CString& GetName() const;
	void SetName(const CString& strName);
	
	CString GetOriName() const { return m_strOriName; }
	void SetOriName(CString strName) { m_strOriName = strName; }

	const CString& GetDescription() const;
	void SetDescription(const CString& strDesc);

	const CString& GetUser() const;
	void SetUser(const CString& strUser);

	const CString& GetMachine() const;
	void SetMachine(const CString& strMachine);
	
	const CTime& GetCreatedTime() const;
	void SetCreatedTime(const CTime& t);

	const CTime& GetLastModifiedTime() const;
	void SetLastModifiedTime(const CTime& t);

	void SetMatch(BOOL bMatch){m_bMatch = bMatch;}

	CCmpReportParameter*	GetInputParam(){return &m_inputParam;}

	const CCmpReportManager& GetCompReportResultList() const;

	void AddReportResult(CCmpBaseReport* pResult);


protected:


	void AddReportPath(ENUM_REPORT_TYPE rptType, const CString& strPath);
	//void RemoveFiles(const CString& strPath);
	void MergeReports(const CString& sOutputPath);
	void GenerateReportParameter(const CReportParamToCompare& inParam, CReportParameter* pOutParam, CModelToCompare* pModel);
	BOOL TransferLogFiles(CModelToCompare *pCmpModel, const CString& strDest,const CString& strSimResult,void (CALLBACK* _ShowCopyInfo)(LPCTSTR));


public:
	void RemoveTempReport();
	CString SaveTempReport(const CString& strReportPath, int iModelIndex, int iReportIndex,int nSimResult);
	BOOL	LoadData();
	BOOL	IsMatch() const;
	void	AddModel();
	void	RemoveModel();
	void	GetModel();

	//	Transfer file(s)
	BOOL	TransferFiles(const CString& strSource, const CString& strDest,void (CALLBACK *_ShowCopyInfo)(LPCTSTR));
	BOOL	RemoveFolder();


	BOOL	Run(CCompRepLogBar* pWndStatus,void (CALLBACK* _ShowCopyInfo)(LPCTSTR));
	void	Stop(BOOL bStop = TRUE){if (m_bRunning) m_bStop = bStop;}

	
protected:
	BOOL IsLocalModel(const CString& strPath);
	void RemoveTempData();
	CString getProjectPath() const;
	CFileOperation	m_fo;


private:
	//	Input parameter
	CCmpReportParameter	m_inputParam;

	//	Report result
	CCmpReportManager	m_rptList;

	BOOL	m_bMatch;		//	Is match
	BOOL	m_bStop;
	BOOL	m_bRunning;
	CString	m_strName;
	CString m_strOriName;
	CString	m_strDesc;
	CString m_strUser;
	CString m_strMachine;
	CTime	m_tmCreated;
	CTime	m_tmLastModified;
};


class COMPARE_TRANSFER CComparativeProjectDataSet : public DataSet
{
public:
	static CComparativeProjectDataSet *m_pInstance;

public:
	static CComparativeProjectDataSet* GetInstance();
	static void DeleteInstance();

public:
	CComparativeProjectDataSet();
	virtual ~CComparativeProjectDataSet();
	
	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Project Information";}
	virtual const char* getHeaders() const{return "Name, Description, Match";}
	
	
	int GetProjects(OUT std::vector<CComparativeProject *>& vProjs);
	void SetProjects(const std::vector<CComparativeProject *>& vProjs);
	void AddProject(CComparativeProject* proj);

	bool isNameAvailable(const CString& strName) const;

	int getProjectCount() const;
	CComparativeProject *getProject(int nIndex);


	CComparativeProject *getProjectByName(const CString& strName);
	//create new project
	//return the new project
	CComparativeProject *CreateNewProject(const CString& strName, const CString& strDesc);
	
protected:
	std::vector<CComparativeProject *>	m_vProjs;


};

#endif // !defined(AFX_COMPARATIVEPROJECT_H__60D721DE_EF29_495E_8F6F_2F5D10A58A65__INCLUDED_)
