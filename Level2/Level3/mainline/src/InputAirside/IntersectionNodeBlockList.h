#pragma once
#include "InputAirsideAPI.h"

class CADORecordset;
class INPUTAIRSIDE_API IntersectionNodeBlock
{
public:
	IntersectionNodeBlock(int nNodeID);
	~IntersectionNodeBlock(void);

	void SetBlock(bool bBlock);
	bool IsBlock();

	int GetNodeID();

	void SetNodeName(const CString& strName);
	const CString& GetNodeName();

	void ReadData(CADORecordset& recordSet);
	void SaveData();


private:
	int m_nID;
	int m_nNodeID;
	CString m_strNodeName;
	bool m_bBlock;
};


class INPUTAIRSIDE_API IntersectionNodeBlockList
{
public:
	IntersectionNodeBlockList(void);
	~IntersectionNodeBlockList(void);

	int GetIntersectionNodeBlockCount();
	IntersectionNodeBlock* GetNodeBlockByIdx(int idx);
	IntersectionNodeBlock* GetNodeBlockByNodeId(int nID);

	void InitNodeBlockData(int nAirportID);

	void ReadBlockData();
	void SaveData();
	void DeleteData();

private:
	std::vector<IntersectionNodeBlock*> m_vBlockData;
};
