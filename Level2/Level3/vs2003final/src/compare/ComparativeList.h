// ComparativeList.h: interface for the CComparativeList class.
#pragma once

class CComparativeReportResult;
class CComparativeQLengthReport;
class CComparativeQTimeReport;
class CComparativeThroughputReport;
class CComparativeSpaceDensityReport;
class CComparativePaxCountReport;
class CComparativeAcOperationReport;
class CComparativeTimeTerminalReport;
class CComparativeDistanceTravelReport;
class Terminal;
class CModelToCompare;

class CComparativeList  
{
public:
	CComparativeList(Terminal* pTerminal, CListCtrl& listCtrl, const std::vector<CModelToCompare*>& vModelList)
	 : m_pTerminal(pTerminal)
	 , m_listCtrl(listCtrl)
	 , m_vModelList(vModelList)
	{
	}

	void RefreshData(const CComparativeReportResult& _reportData);

private:
	Terminal* m_pTerminal;
	CListCtrl& m_listCtrl;
	const std::vector<CModelToCompare*>& m_vModelList;

	void RefreshData(const CComparativeQLengthReport& _reportData);
	void RefreshData(const CComparativeQTimeReport& _reportData);
	void RefreshData(const CComparativeThroughputReport& _reportData);
	void RefreshData(const CComparativeSpaceDensityReport& _reportData);
	void RefreshData(const CComparativePaxCountReport& _reportData);
	void RefreshData(const CComparativeAcOperationReport& _reportData);
	void RefreshData(const CComparativeTimeTerminalReport& _reportData);
	void RefreshData(const CComparativeDistanceTravelReport& _reportData);

	CString GetRegularDateTime(LPCTSTR elaptimestr, bool needsec);
};
