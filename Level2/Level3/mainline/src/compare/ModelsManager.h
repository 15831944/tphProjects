// ModelManager.h: interface for the CModelManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELMANAGER_H__C11539EF_1110_49A0_98B7_79A5E1E1250A__INCLUDED_)
#define AFX_MODELMANAGER_H__C11539EF_1110_49A0_98B7_79A5E1E1250A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "../common/template.h"
#include "../Common/dataset.h"
class CCompRepLogBar;

class CModelToCompare;
//using namespace std;

class CModelsManager  
{
public:
	CModelsManager();
	virtual ~CModelsManager();

	static int ListAllModelsByPath(const CString& strPath, OUT std::vector<CString>& vModels);

	CModelsManager& operator = ( const CModelsManager& _rhs )
	{
		m_vModels = _rhs.m_vModels;
		return *this;
	}
	//stop using this method if you have no other way
	//instead with getCount() and getModel()
	std::vector<CModelToCompare *>& GetModelsList(){return m_vModels;}
public:
	int getCount();
	CModelToCompare* getModel(int nIndex);

	//	Model
	void	AddModel(CModelToCompare* model);
	void	RemoveModel(int nIndex);

	
	CString  InitTerminal(CCompRepLogBar* pStatus, CString strName, void (CALLBACK* _ShowCopyInfo)(LPCTSTR));

	CModelToCompare* GetModelByUniqueName( const CString& strUniqueName );
	
	
	bool LoadData( const CString& strProjName, const CString& strProjPath );
	void SaveData(const CString& strPath);
	Terminal * getTerminal();

	void Clear();

private:
	std::vector<CModelToCompare*>	m_vModels;
};


class CModelDataSet : public DataSet
{
public:
	CModelDataSet();
	virtual ~CModelDataSet();

	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Host Information";}
	virtual const char* getHeaders() const{return "Folder Path";}
	
	void SetFolders(const std::vector<CString>& vstrFolders);
	void AddItem(const CString& strFolder);

	int GetFolders(OUT std::vector<CString>& vFolders);

	std::vector<CString>& GetFolders(){return m_vstrFolders;}
	
protected:
	std::vector<CString>	m_vstrFolders;
};

#endif // !defined(AFX_MODELMANAGER_H__C11539EF_1110_49A0_98B7_79A5E1E1250A__INCLUDED_)
