#pragma once
#include "afxcmn.h"
#include "landside\LandsideBusStation.h"

// CDlgBusStationSelect dialog

class CDlgBusStationSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgBusStationSelect)

	typedef std::map<ALTObjectID,LandsideBusStation *> BusStationMap;
public:
	CDlgBusStationSelect(CWnd* pParent = NULL);   // standard constructor
	CDlgBusStationSelect(LandsideFacilityLayoutObjectList* objlist,CWnd* pParent = NULL);
	virtual ~CDlgBusStationSelect();

// Dialog Data
	enum { IDD = IDD_DIALOG_BUSSTATIONSELECT };

	virtual BOOL OnInitDialog();

	void LoadTree();
	LandsideBusStation *GetSelectBusStation(){return m_pSelectBusStation;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	LandsideFacilityLayoutObjectList* m_pObjlist;

	CTreeCtrl m_treeCtrl;

	BusStationMap m_mapBusStation;

	LandsideBusStation *m_pSelectBusStation;
public:
	afx_msg void OnTvnSelchangedTreeBusstationselect(NMHDR *pNMHDR, LRESULT *pResult);
};
