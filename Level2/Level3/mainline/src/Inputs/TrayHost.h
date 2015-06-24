#pragma once
#include "..\Common\DBData.h"

class TrayHost : public CDBData
{
public:
	TrayHost();
	virtual ~TrayHost();

public:
	virtual void GetDataFromDBRecordset(CADORecordset& recordset);
	virtual bool GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual bool GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const; 
	virtual CString GetTableName()const;
	CString FormatNonPaxList(const std::vector<int>&vData)const;
	void ConveyToNonPaxList(CString& strNonPaxList);

	void SetHostName(const CString& sName) {m_sHost = sName;}
	const CString& GetHostName()const {return m_sHost;}

	void SetHostIndex(int nHostIndex){m_nHostIndex = nHostIndex;}
	int GetHostIndex()const {return m_nHostIndex;}

	void SetRadius(int nRadius){m_nRadius = nRadius;}
	int GetRadius()const {return m_nRadius;}

	void AddItem(int nNonPaxIndex){m_vNonPaxIndex.push_back(nNonPaxIndex);}
	bool RemoveItem(int nNonPaxIndex);
	int GetItem(int nIndex);
	void SetItem(int nIndex, int nNonPaxIndex);
	int GetCount()const{return (int)m_vNonPaxIndex.size();}
	int FindItem(int nNnPaxIndex);
	const std::vector<int>& GetNonPaxIndexVector()const {return m_vNonPaxIndex;}

	bool IsEqual(const std::vector<int>& vNonPaxIndex);

private:
	CString m_sHost;
	int     m_nHostIndex;
	int		m_nRadius;
	std::vector<int>m_vNonPaxIndex;
};

class TrayHostList : public CDBDataCollection<TrayHost>
{
public:
	TrayHostList();
	virtual ~TrayHostList();

	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	void ReadData(int nParentID);
	int FindItem(const CString& sHostName);
	bool FindHostItem(int nNonPaxIndex);
	bool IfNonPaxExsit(int nNonPaxIndex);
	bool IfNonHostPaxExsit(int nNonPaxIndex);
	int FindItem(int nNonPaxHostIndex,std::vector<int>vNonPaxIndex);
};