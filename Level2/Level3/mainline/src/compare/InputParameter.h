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

class CInputParameter  
{
public:
	CInputParameter();
	virtual ~CInputParameter();

public:
	const CModelsManager& GetModelsManager();
	void SetModelsManager(const CModelsManager& modelsManager);

	const CReportsManager& GetReportsManager();
	void SetReportsManager(const CReportsManager& reportsManager);

	CModelsManager* GetModelsManagerPtr(){return &m_modelsManager;}
	CReportsManager* GetReportsManagerPtr(){return &m_reportsManager;}

	BOOL DeleteModel(const CString& strUniqueName);


private:
	CModelsManager	m_modelsManager;
	CReportsManager	m_reportsManager;
};

#endif // !defined(AFX_INPUTPARAMETER_H__237A4BF6_AC33_4921_91D9_ABF8ECBBB17E__INCLUDED_)
