// InputParameter.cpp: implementation of the CInputParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "InputParameter.h"
#include "ModelToCompare.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmpReportParameter::CCmpReportParameter()
{

}

CCmpReportParameter::~CCmpReportParameter()
{

}

//const CModelsManager& CProjectReportParameter::GetModelsManager()
//{
//	return m_modelsManager;
//}
//
//void CProjectReportParameter::SetModelsManager(const CModelsManager& modelsManager)
//{
//	m_modelsManager = modelsManager;
//}
//
//const CSingleReportsManager& CProjectReportParameter::GetReportsManager()
//{
//	return m_reportsManager;
//}
//
//void CProjectReportParameter::SetReportsManager(const CSingleReportsManager& reportsManager)
//{
//	m_reportsManager = reportsManager;
//}

BOOL CCmpReportParameter::DeleteModel(const CString& strUniqueName)
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

bool CCmpReportParameter::LoadData( const CString& strProjName, const CString& strProjPath )
{
	 m_modelsManager.LoadData( strProjName,strProjPath );
	 if(!m_modelsManager.IsAllProjectsSucessfullyLoaded())
		 return false;

	 m_reportsManager.LoadData(strProjPath, &m_modelsManager);
	
	return true;
}

CModelToCompare* CCmpReportParameter::GetModelByUniqueName( const CString& strUniqueName )
{
	return m_modelsManager.GetModelByUniqueName(strUniqueName);
}

void CCmpReportParameter::SaveData(const CString& strPath)
{
	//	Save Model Information
	m_modelsManager.SaveData(strPath);

	//	Save Report information
	m_reportsManager.SaveData(strPath);
}

Terminal * CCmpReportParameter::getTerminal()
{
	return m_modelsManager.getTerminal();
}

BOOL CCmpReportParameter::DeleteReport( const CString& strReportName )
{
	return m_reportsManager.DeleteReport(strReportName);
}
