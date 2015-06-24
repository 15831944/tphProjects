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
	{
	}

	void RefreshData(const CComparativeReportResult& _reportData);

private:
	Terminal* m_pTerminal;
	CListCtrl& m_listCtrl;

	void RefreshData(CComparativeQLengthReport& _reportData);
	void RefreshData(CComparativeQTimeReport& _reportData);
	void RefreshData(CComparativeThroughputReport& _reportData);
	void RefreshData(CComparativeSpaceDensityReport& _reportData);
	void RefreshData(CComparativePaxCountReport& _reportData);
	void RefreshData(CComparativeAcOperationReport& _reportData);
	void RefreshData(CComparativeTimeTerminalReport& _reportData);
	void RefreshData(CComparativeDistanceTravelReport& _reportData);

	CString GetRegularDateTime(LPCTSTR elaptimestr, bool needsec);
};
