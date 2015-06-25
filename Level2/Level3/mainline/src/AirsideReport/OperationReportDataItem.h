#pragma once
#include <vector>
using namespace std;

enum ENUMFLIGHTOPERATIONTYPE
{
	OPERATIONTYPE_ARR,
	OPERATIONTYPE_DEP,
	//OPERATIONTYPE_TA
};

class COperationReportDataItem
{
public:
	COperationReportDataItem(void);
	~COperationReportDataItem(void);

public:
	ENUMFLIGHTOPERATIONTYPE   enumOperationType;
	vector<int>               m_vNumOfFlight;
};