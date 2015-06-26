// ModelManager.cpp: implementation of the CModelManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ModelsManager.h"
#include "ModelToCompare.h"

BOOL HasSimResult(const CString &strPath)
{
	CFileFind finder;
	BOOL bFound = finder.FindFile(strPath + _T("\\*.*"));
	while (bFound)
	{
		bFound = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			if (finder.GetFileName() == _T("SimResult"))
				return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModelsManager::CModelsManager()
{
	m_vModels.clear();
}

CModelsManager::~CModelsManager()
{
	std::vector<CModelToCompare *>::iterator iter = m_vModels.begin();
	std::vector<CModelToCompare *>::iterator iterEnd = m_vModels.end();
	for (;iter != iterEnd; ++iter )
	{
		delete *iter;
		*iter = NULL;
	}
	
	m_vModels.clear();
}

void CModelsManager::AddModel(CModelToCompare* model)
{
	m_vModels.push_back(model);
}

void CModelsManager::RemoveModel(int nIndex)
{
	
}

//////////////////////////////////////////////////////////////////////////
//	List All Models
//////////////////////////////////////////////////////////////////////////
int CModelsManager::ListAllModelsByPath(const CString& strPath, OUT std::vector<CString>& vModels)
{
	int iCount = 0;
	vModels.clear();
	CString strFolder = strPath + _T("\\*.*");
	
	CFileFind finder;
	BOOL bFound = finder.FindFile(strFolder);

	while (bFound)
	{
		bFound = finder.FindNextFile();

		if (finder.IsDots())
			continue;
		
		if (finder.IsDirectory() && HasSimResult(finder.GetFilePath()))
		{
			//CModelToCompare model;
			//model.SetModelName(finder.GetFileName());
			//vModels.push_back(model);
			vModels.push_back(finder.GetFileName());
			iCount++;
		}
	}

	finder.Close();
	return iCount;
}

CString  CModelsManager::InitTerminal(CCompRepLogBar* pStatus, CString strName, void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR))
{
	std::vector<CModelToCompare *>::iterator iter = m_vModels.begin();
	std::vector<CModelToCompare *>::iterator iterEnd = m_vModels.end();
	for (;iter != iterEnd; ++iter )
	{
		
		if(((*iter)->InitTerminal(pStatus,strName,_ShowCopyInfo)) == NULL)
			return (*iter)->GetModelName();
	}

	return "";
}

CModelToCompare* CModelsManager::GetModelByUniqueName( const CString& strUniqueName )
{
	CModelToCompare *pModel = NULL;
	std::vector<CModelToCompare *>::size_type nModel = 0;
	for (;nModel< m_vModels.size(); ++nModel)
	{
		if (strUniqueName.CompareNoCase(m_vModels.at(nModel)->GetUniqueName()) == 0)
		{	
			pModel = m_vModels.at(nModel);
			break;
		}
	}
	return pModel;
}
bool CModelsManager::LoadData( const CString& strProjName, const CString& strProjPath )
{
	m_vModels.clear();

	CModelToCompareDataSet dsModel;
	dsModel.SetProjName(strProjName);
	dsModel.loadDataSet(strProjPath);
	if (dsModel.GetModels(m_vModels)) 
	{
		return true;
	}
	
	return false;
}

void CModelsManager::SaveData( const CString& strPath )
{
	CModelToCompareDataSet dsModel;

	dsModel.SetModels(m_vModels);
	dsModel.saveDataSet(strPath, false);
}

Terminal * CModelsManager::getTerminal()
{
	if ((int)m_vModels.size() > 0)
	{
		return m_vModels[0]->GetTerminal();
	}
	return NULL;
}

int CModelsManager::getCount()
{
	return static_cast<int>(m_vModels.size());
}

CModelToCompare* CModelsManager::getModel( int nIndex )
{
	ASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_vModels.size()));
	if(nIndex >=0 && nIndex < getCount())
		return m_vModels.at(nIndex);

	return NULL;
}

void CModelsManager::Clear()
{
	m_vModels.clear();
}

bool CModelsManager::IsAllProjectsSucessfullyLoaded()
{
	for(int nIndex = 0; nIndex < getCount(); ++ nIndex )
	{
		CModelToCompare* pModel = m_vModels.at(nIndex);
		ASSERT(pModel != NULL);
		if(pModel)
		{
			if(pModel->GetTerminal() == NULL)
				return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//	CModelDataSet
CModelDataSet::CModelDataSet() : DataSet(HostInfoFile)
{
	m_vstrFolders.clear();
}

CModelDataSet::~CModelDataSet()
{
}

void CModelDataSet::clear()
{
}

void CModelDataSet::readData(ArctermFile& p_file)
{
	//ASSERT(m_pACCategoryList != NULL && m_pAcMan != NULL);
	char buf[256];
	
	while (p_file.getLine() == 1)
	{
		// read the ACCategory name;
		if (p_file.getField(buf, 255) != 0)
		{
			CString str = buf;
			m_vstrFolders.push_back(str);
		}
	}
}

void CModelDataSet::readObsoleteData(ArctermFile& p_file)
{

}

void CModelDataSet::writeData(ArctermFile& p_file) const
{
	for (unsigned i = 0; i < m_vstrFolders.size(); i++)
	{
		p_file.writeField(m_vstrFolders[i]);
		p_file.writeLine();
	}

	if (m_vstrFolders.empty())
		p_file.writeLine();
}

void CModelDataSet::SetFolders(const std::vector<CString>& vstrFolders)
{
	m_vstrFolders = vstrFolders;
}

void CModelDataSet::AddItem(const CString& strFolder)
{
	m_vstrFolders.push_back(strFolder);
}

int CModelDataSet::GetFolders(OUT std::vector<CString>& vFolders)
{
	vFolders = m_vstrFolders;
	return m_vstrFolders.size();
}


