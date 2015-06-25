#pragma once
#include <vector>
using namespace std;

class CDetailNodeDelayDataItem
{
public:
	CDetailNodeDelayDataItem(void);
	~CDetailNodeDelayDataItem(void);

public:
	int m_nNodeID;
	int m_nNodeType;
	vector<long> m_lDelayTime;
};
