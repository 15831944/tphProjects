#pragma once
#include <CommonData/procid.h>
#include "..\common\DataSet.h"
#include "MobileElemConstraint.h"

class CInputFlowSyncItem
{
public:
	CInputFlowSyncItem();
	virtual ~CInputFlowSyncItem();

	void SetMobElementConstraint(const CMobileElemConstraint& constraint);
	const CMobileElemConstraint& GetMobElementConstraint()const;

	void SetProcID(const ProcessorID& procID);
	const ProcessorID& GetProcID()const;

	virtual bool readData(ArctermFile& p_file,InputTerminal* pTerm);
	virtual bool writeData(ArctermFile& p_file) const;
private:
	CMobileElemConstraint	m_paxType;
	ProcessorID				m_syncPaxProcID;
	
};

class CInputFlowPaxSyncItem : public CInputFlowSyncItem
{
public:
	CInputFlowPaxSyncItem();
	~CInputFlowPaxSyncItem();

	int GetCount()const;
	void AddItem(CInputFlowSyncItem* pNonPaxSyncItem);
	void DeleteItem(CInputFlowSyncItem* pNonPaxSyncItem);
	CInputFlowSyncItem* GetItem(int idx)const;

	virtual bool readData(ArctermFile& p_file,InputTerminal* pTerm);
	virtual bool writeData(ArctermFile& p_file) const;

	void GetFittestNonSyncItem(const CMobileElemConstraint& nonPaxtype,std::vector<CInputFlowSyncItem*>& nonSyncList)const;

	void SetSyncName(const CString& sSyncName);
	const CString& GetSyncName()const;

	CInputFlowSyncItem* GetNonPaxSyncItem(const CMobileElemConstraint& paxType,const ProcessorID& procID)const;
private:
	void ClearData();
private:
	CString m_strSyncName;
	std::vector<CInputFlowSyncItem*>	m_vNonPaxSyncItem;
};

typedef std::vector<CInputFlowPaxSyncItem*> InputFlowSyncItemList;

class CInputFlowSync : public DataSet
{
public:
	CInputFlowSync();
	~CInputFlowSync();

	int GetCount()const;
	void AddItem(CInputFlowPaxSyncItem* pNonPaxSyncItem);
	void DeleteItem(CInputFlowPaxSyncItem* pNonPaxSyncItem);
	CInputFlowPaxSyncItem* GetItem(int idx)const;

	bool GetFlowPaxSync(const CMobileElemConstraint& paxType,InputFlowSyncItemList& syncList)const;
	CInputFlowPaxSyncItem* GetFlowPaxSync(const CMobileElemConstraint& paxType,const ProcessorID& procID)const;
	CInputFlowPaxSyncItem* GetFlowParentNonPaxSync(const CMobileElemConstraint& nonpaxType,const ProcessorID& procID)const;
	CInputFlowSyncItem* GetFlowNonPaxSync(const CMobileElemConstraint& nonpaxType,const ProcessorID& procID)const;
private:
	void ClearData();
private:
	InputFlowSyncItemList m_vInputFlowSyncItemList;

	// virtual function from DataSet
	virtual void readData(ArctermFile& p_file);
	virtual	void readObsoleteData( ArctermFile& p_file );
	virtual void writeData(ArctermFile& p_file) const;
	virtual void clear();
	virtual const char *getTitle() const;
	virtual const char *getHeaders() const;
};
