#pragma once

#include "ALTObject.h"
#include "InputAirsideAPI.h"

class CADORecordset;
class ALTObjectIDList;

class INPUTAIRSIDE_API ALTObjectGroup
{
public:
	ALTObjectGroup();
	virtual ~ALTObjectGroup(void);


public:
	void ReadData(int nGroupID);
	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nProjID);
	int  InsertData(int nProjID);
	void UpdateData();
	void DeleteData();

	bool IsFits(const ALTObjectIDList& idList)const;
public:

	void setID(int nID);
	int getID() const;
	int GetProjID() {  return m_nProjID; }
	void SetProjID(int nProjID)  {  m_nProjID = nProjID; } 

	void setType(ALTOBJECT_TYPE ObjType);
	ALTOBJECT_TYPE getType();

	void setName(const ALTObjectID& groupName);
	ALTObjectID getName() const;

	//get the objects based on the group
	bool GetObjects(std::vector< ALTObject>& vObjct);
private:
	int m_nID;
	int m_nProjID;
	ALTOBJECT_TYPE m_enumType;
	ALTObjectID m_groupName;
};
