#pragma once
#include <vector>
#include "InputAirsideAPI.h"

class StandBufferItem;
class CADORecordset;

class INPUTAIRSIDE_API FlightTypeStandBufferSpecfic
{
public:
	typedef std::vector<StandBufferItem*> StandBufVector;
	typedef std::vector<StandBufferItem*>::iterator StandBufIter;

public:
	FlightTypeStandBufferSpecfic(int nProjID);
	~FlightTypeStandBufferSpecfic(void);

public:
	int getID(){ return m_nID; }
	void setID(int nID){ m_nID = nID; }

	void ReadData(CADORecordset& adoRecordset);
	void ReadData();

	void SaveData();
	void DeleteData();
	void UpdateData();
	void SaveStandBuffList();
	void DeleteStandBuffList();

	int GetStandBufItemCount() const;
	StandBufferItem* GetStandBufItem(int nIndex) const;
	void AddStandBufItem(StandBufferItem* pItem);
	void DeleteStandBufItem(StandBufferItem* pItem);

	LPCTSTR GetFltType() const { return m_strFltType; }
	void SetFltType(LPCTSTR lpszFltType) { m_strFltType = lpszFltType; }
	
	void RemoveAll();

private:

	int m_nID;
	int m_nProjectID;

	CString m_strFltType;

	StandBufVector m_vectStandBuf;

	StandBufVector m_vectNeedtoDel;
};
