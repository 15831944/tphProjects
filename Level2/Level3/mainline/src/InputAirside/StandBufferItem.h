#pragma once
#include "ALTObjectGroup.h"
#include "InputAirsideAPI.h"

class CADORecordset;
class INPUTAIRSIDE_API StandBufferItem
{
public:
	StandBufferItem();
	~StandBufferItem();

public:
	void ReadData(CADORecordset& adoDatabase);
	void SaveData();
	void DeleteData();
	void UpdateData();
	
	int getID() const { return m_nID; }
	void setID(int nID) { m_nID = nID; }

	int getStandBufferID() const { return m_nStandBuffID; }
	void setStandBufferID(int nID) { m_nStandBuffID = nID; }

protected:
	int m_nID;
	int m_nStandBuffID;

public:
	ALTObjectGroup m_standGrop;
	int m_nArrivalBuf;
	int m_nDepartureBuf;
};
