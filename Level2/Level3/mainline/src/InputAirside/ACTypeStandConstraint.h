#pragma once
#include "ALTObjectGroup.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API ACTypeStandConstraint
{
public:
	ACTypeStandConstraint(void);
	~ACTypeStandConstraint(void);

	//database operation, not catch exception
	void ReadData(CADORecordset& adoRecord);
	void SaveData();
	void UpdateData();
	void DeleteData();

	const ALTObjectGroup& GetStandGroup();
	const CStringList& GetACTypeList();
	bool Contains(const CString& strACType);
	void SetStandGroupName(ALTObjectID objID);
	void AddACType(CString ACTypeName);
	void RemoveAllACTypes(){ m_strACTypeList.RemoveAll(); }
	CString GetStandGroupIDString();
	void InitACTypesFromString(CString strACTypes);

private:
	int m_nID;							//database key
	ALTObjectGroup m_standGroup;		//stand group
	CStringList m_strACTypeList;		//AC type list
};

/////////////////////////////////////////////////////////////////////////////////
class ARCFlight;
class INPUTAIRSIDE_API ACTypeStandConstraintList
{
public:
	ACTypeStandConstraintList(void);
	~ACTypeStandConstraintList(void);

	//database operation, not catch exception
	void ReadData();
	void SaveData();
	void RemoveAll();

	int GetConstaintCount();
	// will not add to the end, because the sort method is called.
	void AddItem(ACTypeStandConstraint* pConst);
	void DeleteItemByIndex(int nIdx);
	void DeleteItemByConstraint(ACTypeStandConstraint* pConst);
	// delete all pointers in m_vDataList and m_vDelDataList, but not delete the data they pointed to.
	// call this method before destructing temp objects.
	void ClearConstraintPtrs();
	// delete both the pointers in m_vDataList and m_vDelDataList, then delete the data they pointed to.
	void DeleteAllItem();
	// get ACTypeStandConstraint.
	ACTypeStandConstraint* GetItemByIndex(int nIdx);
	bool IsFlightFit(const ARCFlight* flight, const ALTObjectID& gate);
private:
	std::vector<ACTypeStandConstraint*> m_vDataList;		//constraint data need be saved 
	std::vector<ACTypeStandConstraint*> m_vDelDataList;		//constraint data need be deleted
};
