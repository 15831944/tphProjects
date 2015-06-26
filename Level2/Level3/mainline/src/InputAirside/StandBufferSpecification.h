#pragma once
#include <vector>
#include "InputAirsideAPI.h"

class FlightTypeStandBufferSpecfic;

class INPUTAIRSIDE_API StandBuffers
{
public:
	typedef std::vector<FlightTypeStandBufferSpecfic*> FltTypeStandBufVector;
	typedef std::vector<FlightTypeStandBufferSpecfic*>::iterator FltTypeStandBufIter;

public:
	StandBuffers(int nProjID);
	~StandBuffers();

public:
	void ReadData();
	void SaveData();
	
	int GeItemsCount();
	FlightTypeStandBufferSpecfic* GetItem(int nIndex);
	void AddFltTypeStandBufferSpecific(FlightTypeStandBufferSpecfic* pItem);
	void DeleteltTypeStandBufferSpecific(FlightTypeStandBufferSpecfic* pItem);
	void RemoveAll();

private:
	int m_nProjID;
	FltTypeStandBufVector m_vectFltTypeStandBuf;
	FltTypeStandBufVector m_vectNeedToDel;
};
