#pragma once

#include <database/ADODatabase.h>
#include <common/CPPUtil.h>
#include <algorithm>

class CDBDataBase
{
public:
	int GetID() const{ return m_nID; }

	virtual void SaveData(int nParentID)=0;
	virtual void DeleteData()=0;//delete data in database	
	virtual void GetDataFromDBRecordset(CADORecordset& )=0;
protected:
	int m_nID; //database id
	CDBDataBase(void){ m_nID = -1; }
	virtual ~CDBDataBase() {}


};


class CDBData : public CDBDataBase
{
public:

	virtual void SaveData(int nParentID);
	virtual void DeleteData();//delete data in database
	virtual void ReadData(int nID);

	virtual CString GetTableName()const=0;
	virtual bool GetInsertSQL(int nParentID, CString& strSQL) const = 0;
	virtual bool GetUpdateSQL(CString& strSQL) const = 0;


	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const ;

};


template <typename Element>
class CDBDataCollectionBase : public cpputil::noncopyable
{
public:
	virtual ~CDBDataCollectionBase(){ CleanData(); }

	typedef std::vector<Element*> DBDataList;

	int GetItemCount()const{ return (int)m_vDatalist.size(); }
	Element* GetItem(int idx)const{ return m_vDatalist[idx]; }

	bool IsHaveItem(Element* pElm)const
	{
		return std::find(m_vDatalist.begin(),m_vDatalist.end(), pElm) != m_vDatalist.end();	
	}


	//move item from the list and add to delete list
	bool DeleteItem(Element* pItem)
	{
		if(RemoveFromList(pItem))
		{
			AddToDeleteList(pItem);
			return true;
		}
		return false;
	}

	bool DeleteItem(int pIndex)
	{
		if ((size_t)pIndex>=m_vDatalist.size())
		{
			return false;
		}
		m_vDataListDelete.push_back(m_vDatalist.at(pIndex));
		m_vDatalist.erase(m_vDatalist.begin()+pIndex);
		return true;
	}

	//assert the item is new 
	void AddNewItem(Element* pItem)
	{
		m_vDatalist.push_back(pItem);
	}

	//if the item exist in the delete list , restore it
	void AddItem(Element* pItem)
	{
		RemoveFromDeleteList(pItem);
		AddToList(pItem);
	}

	//move all items to delete list
	void DeleteData()
	{
		m_vDataListDelete.insert(m_vDataListDelete.end(), m_vDatalist.begin(),m_vDatalist.end() );
		m_vDatalist.clear();
	}

	

	virtual void ReadSelectData(CString strSelectSQL )
	{		
		CleanData();
		
		if (strSelectSQL.IsEmpty())
			return;

		long nRecordCount = -1;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

		while (!adoRecordset.IsEOF())
		{
			Element* element = NewFromDBRecordset(adoRecordset);			
			if(element)
				AddNewItem(element);
			adoRecordset.MoveNextData();
		}
	}

	virtual void SaveData(int nParentID)
	{
		for (DBDataList::iterator iter = m_vDatalist.begin();
			iter != m_vDatalist.end(); ++iter)
		{
			(*iter)->SaveData(nParentID);
		}
		//	
		for (DBDataList::iterator iter = m_vDataListDelete.begin();
			iter != m_vDataListDelete.end(); ++iter)
		{
			(*iter)->DeleteData();
			delete (*iter);
		}

		m_vDataListDelete.clear();
	}
	
	virtual Element* NewFromDBRecordset(CADORecordset& dbRecord)const
	{
		Element* pNewElm = new Element();
		pNewElm->GetDataFromDBRecordset(dbRecord);
		return pNewElm;
	}
	

	Element* GetByID(int id)const{
		for (DBDataList::const_iterator iter = m_vDatalist.begin();
			iter != m_vDatalist.end(); ++iter)
		{
			if( (*iter)->GetID() == id )
				return *iter;
		}
		return NULL;
	}
protected:
	DBDataList m_vDatalist;
	DBDataList m_vDataListDelete;
	//clear all items ,include delete list
	void CleanData()
	{
		for (int i=0;(size_t)i<m_vDatalist.size();i++)
		{
			delete m_vDatalist.at(i);
		}
		for (int i=0;(size_t)i<m_vDataListDelete.size();i++)
		{
			delete m_vDataListDelete.at(i);
		}
		m_vDatalist.clear();
		m_vDataListDelete.clear();
	}
	//safe add check existence
	void AddToDeleteList(Element* pitem)
	{
		DBDataList::iterator itr = std::find(m_vDataListDelete.begin(),m_vDataListDelete.end(), pitem);
		if(itr == m_vDataListDelete.end() )
		{
			//pitem->DeleteData();
			m_vDataListDelete.push_back(pitem);
		}
	}

	void AddToList(Element* pitem)
	{
		DBDataList::iterator itr= std::find(m_vDatalist.begin(),m_vDatalist.end(), pitem );
		if(itr == m_vDatalist.end())
			m_vDatalist.push_back(pitem);
	}

	bool RemoveFromList(Element* pitem)
	{
		DBDataList::iterator itr= std::find(m_vDatalist.begin(),m_vDatalist.end(), pitem );
		if(itr != m_vDatalist.end())
		{
			m_vDatalist.erase(itr);
			return true;
		}
		return false;
	}

	bool RemoveFromDeleteList(Element* pitem)
	{
		DBDataList::iterator itr= std::find(m_vDataListDelete.begin(),m_vDataListDelete.end(), pitem );
		if(itr != m_vDataListDelete.end())
		{
			m_vDataListDelete.erase(itr);
			return true;
		}
		return false;
	}
};


template<typename Element>
class CDBDataCollection : public CDBDataCollectionBase<Element>
{
public:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const = 0;
	
	virtual void ReadData(int nParentID){
		CString strSelectSQL;
		GetSelectElementSQL(nParentID,strSelectSQL);
		strSelectSQL.Trim();
		ReadSelectData(strSelectSQL);
	}
};
