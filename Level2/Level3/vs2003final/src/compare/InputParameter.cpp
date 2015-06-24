// InputParameter.cpp: implementation of the CInputParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "InputParameter.h"
#include "ModelToCompare.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputParameter::CInputParameter()
{

}

CInputParameter::~CInputParameter()
{

}

const CModelsManager& CInputParameter::GetModelsManager()
{
	return m_modelsManager;
}

void CInputParameter::SetModelsManager(const CModelsManager& modelsManager)
{
	m_modelsManager = modelsManager;
}

const CReportsManager& CInputParameter::GetReportsManager()
{
	return m_reportsManager;
}

void CInputParameter::SetReportsManager(const CReportsManager& reportsManager)
{
	m_reportsManager = reportsManager;
}

BOOL CInputParameter::DeleteModel(const CString& strUniqueName)
{
	std::vector<CModelToCompare *>& vModels = m_modelsManager.GetModelsList();
	std::vector<CModelToCompare *>::iterator iter;
	for (iter = vModels.begin(); iter != vModels.end(); iter++)
	{
		if ( (*iter)->GetUniqueName() == strUniqueName)
		{
			m_reportsManager.DeleteModelParameter((*iter)->GetUniqueName());

			vModels.erase(iter);
			return TRUE;
		}
	}
	return FALSE;
}