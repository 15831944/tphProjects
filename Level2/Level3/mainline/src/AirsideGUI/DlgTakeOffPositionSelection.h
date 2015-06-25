#pragma once

#include "afxcmn.h"
#include "..\InputAirside\RunwayExit.h"
#include "AirsideGUI\MuliSelTreeCtrlEx.h"
 
// CDlgSelectTakeoffPosition dialog
class CRunwayTakeOffRunwayWithMarkData;
class Runway;
class CAirportDatabase;

class CDlgSelectTakeoffPosition : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectTakeoffPosition)

public:
	CDlgSelectTakeoffPosition(int nProjID,int nRunwayID, int nRunwayMarkIndex,CWnd* pParent = NULL,bool bLeftDistFlag=FALSE);   // standard constructor
	virtual ~CDlgSelectTakeoffPosition(); 
	// Dialog Data
	enum { IDD = IDD_SELECTTAKEOFFPOSITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	std::vector<int> m_vrSelID;
	std::vector<RunwayExit> m_vRunwayExitList;
	CStringArray m_arSelName;
	bool m_bSelAll;
	int m_nRwyID;
	int m_nRwyMark;
	bool m_bLeftDistFlag;

protected:
	int m_nProjID;
	//CTreeCtrl m_wndTakeoffPosiTree;
	CMuliSeltTreeCtrlEx m_wndTakeoffPosiTree;
	CAirportDatabase* m_pAirportDB;
	int m_nSelRwyID;
	int m_nSelRwyMark;
	std::vector<RunwayExit*>m_vTreeItemData;

protected:
	void SetTreeContents(void);
	void SetRunwayTakeoffPositionInfoIntoTree(CRunwayTakeOffRunwayWithMarkData* pRwTakeOffRunwayMarkData,Runway* pRunway,HTREEITEM hRunwayMark);
public:
	virtual BOOL OnInitDialog();
	bool IsTaxiwayItem(HTREEITEM hItem);
	void SetAirportDB(CAirportDatabase* pDB);
protected:
	virtual void OnOK();
public:
	afx_msg void OnTvnSelchangedTreeTakeoffposi(NMHDR *pNMHDR, LRESULT *pResult);
};
