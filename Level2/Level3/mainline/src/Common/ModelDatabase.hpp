#pragma once

#include <algorithm>
template<class T1,class T2>
class CModelDatabase
{
public:
	int AddModel(T1* pNewModel)
	{
		if(pNewModel)
		{
			m_vModels.push_back(pNewModel);
			return m_vModels.size()-1;
		}
		return -1;
	}
	void DelModel(T1* pDelModel)
	{
		TypeModelList::iterator itr = std::find(m_vModels.begin(), m_vModels.end(),pDelModel );
		if( itr!=m_vModels.end()) 
		{
			m_vModels.erase(itr);
			m_vDelModels.push_back(pDelModel);
		}
	}

	void ClearData()
	{
		TypeModelList::iterator ite = m_vModels.begin();
		TypeModelList::iterator iteEn = m_vModels.end();
		for (;ite!=iteEn;ite++)
		{
			delete (*ite);
		}
		m_vModels.clear();

		ite = m_vDelModels.begin();
		iteEn = m_vDelModels.end();
		for (;ite!=iteEn;ite++)
		{
			delete (*ite);
		}
		m_vDelModels.clear();
	}

	T1* GetModel(int idx)const
	{
		bool bIdxValid = (idx >=0 && idx<GetModelCount());
		ASSERT(bIdxValid);
		if(bIdxValid)
			return m_vModels[idx];
		return NULL;
	}
	int GetModelCount()const
	{
		return (int)m_vModels.size();
	}

	virtual BOOL loadDatabase(T2* pAirportDatabase = NULL) = 0;
	virtual BOOL saveDatabase(T2* pAirportDatabase = NULL)
	{
		
		TypeModelList::iterator ite = m_vModels.begin();
		TypeModelList::iterator iteEn = m_vModels.end();
		for (;ite!=iteEn;ite++)
		{
			if( !(*ite)->IsFileSaved() )
			{
				(*ite)->SaveContent();
				(*ite)->SaveFile();
			}
			(*ite)->SaveDataToDB();
		}

		ite = m_vDelModels.begin();
		iteEn = m_vDelModels.end();
		for (;ite!=iteEn;ite++)
		{
			(*ite)->DeleteData();
		}
			
		m_vDelModels.clear();

		return TRUE;
	}
protected:
	typedef std::vector<T1*> TypeModelList;
	TypeModelList m_vModels;
	TypeModelList m_vDelModels;
};