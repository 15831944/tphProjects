// ComparativeList.h: interface for the CComparativeList class.
#pragma once
#include "MFCExControl\XListCtrl.h"

class CCmpBaseReport;
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
	CComparativeList(Terminal* pTerminal, CXListCtrl& listCtrl)
	 : m_pTerminal(pTerminal)
	 , m_listCtrl(listCtrl)
	{
	}

	void RefreshData(const CCmpBaseReport& _reportData);

private:
	Terminal* m_pTerminal;
	CXListCtrl& m_listCtrl;

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
