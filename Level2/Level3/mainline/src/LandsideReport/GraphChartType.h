#pragma once

#include <vector>
#include "LandsideReportAPI.h"


class LANDSIDEREPORT_API LSGraphChartType
{
public:
	LSGraphChartType(void);
	~LSGraphChartType(void);
public:
	int m_nEnum;
	CString m_strTitle;

};

class LANDSIDEREPORT_API LSGraphChartTypeList
{
public:
	LSGraphChartTypeList();
	~LSGraphChartTypeList();

	void AddItem(int nEnum, const CString& strName);

	int GetCount() const;

	LSGraphChartType GetItem(int nIndex) const;

	void Clean();

protected:
	std::vector<LSGraphChartType> m_vChartType;

private:
};