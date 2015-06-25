#pragma once

#include "NECRelatedBaseDlg.h"
#include "../InputAirside/PerformLandings.h"

// CDlgPerformLanding dialog
class CAirportDatabase;
class CDlgPerformLanding : public CNECRelatedBaseDlg
{
	DECLARE_DYNAMIC(CDlgPerformLanding)

public:
	CDlgPerformLanding(int nProjID, std::vector<CString>& vRunwayNames, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPerformLanding();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void ReloadData();
	virtual void SetListColumn();
	virtual void SetListContent();
	virtual void CheckListContent();
	virtual void OnNewItem(FlightConstraint& fltType);
	virtual void OnDelItem();
	//virtual void OnEditItem();
	virtual void OnListItemChanged(int nItem, int nSubItem);
	virtual void OnNECItemChanged(int nItem, int nNewNECTableID);

private:
	BOOL IsRunwayDefineChanged(PerformLanding& perfLand);
	//When Runway Changed we need UpdateData
	void UpdateFlareDistanceIfNeeded(std::vector<PerformLanding>& vPerfLands);
	PerformLandings* m_pPerformLandings;
	std::vector<CString> m_vRunwayNames;

	static int PrevColumnCount;
public:
	virtual BOOL OnInitDialog();
};
