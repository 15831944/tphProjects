// InputParameter.h: interface for the CInputParameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTPARAMETER_H__237A4BF6_AC33_4921_91D9_ABF8ECBBB17E__INCLUDED_)
#define AFX_INPUTPARAMETER_H__237A4BF6_AC33_4921_91D9_ABF8ECBBB17E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ModelsManager.h"
#include "ReportsManager.h"

class CCmpReportParameter  
{
public:
	CCmpReportParameter();
	virtual ~CCmpReportParameter();

public:

	CModelsManager* GetModelsManager(){return &m_modelsManager;}
	CSingleReportsManager* GetReportsManager(){return &m_reportsManager;}

	BOOL DeleteModel(const CString& strUniqueName);
	BOOL DeleteReport(const CString& strReportName);
public:
	bool LoadData(const CString& strProjName, const CString& strProjPath);

	void SaveData(const CString& strPath);

	CModelToCompare* GetModelByUniqueName(const CString& strUniqueName);

	Terminal *getTerminal();
private:
	CModelsManager			m_modelsManager;
	CSingleReportsManager	m_reportsManager;
};

#endif // !defined(AFX_INPUTPARAMETER_H__237A4BF6_AC33_4921_91D9_ABF8ECBBB17E__INCLUDED_)
