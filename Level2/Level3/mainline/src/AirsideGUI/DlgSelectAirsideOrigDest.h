#pragma once
#include "afxcmn.h"
#include "common/ALTObjectID.h"
#include "InputAirside/RunwayExit.h"
#include "InputAirside/HoldShortLines.h"
#include "..\MFCExControl\SplitterControl.h"
#include <MFCExControl/XTResizeDialogEx.h>

class ALTObject;
enum RUNWAY_MARK;
class Stand;
class ALTAirport;

class CIn_OutBoundRoute;

// CDlgSelectAirsideOrigDest dialog

class CDlgSelectAirsideOrigDest : public CXTResizeDialogEx
{
	DECLARE_DYNAMIC(CDlgSelectAirsideOrigDest)

	enum RUNWAYEXITTYPE
	{
		RUNWAYEXIT_DEFAULT = 0,
		RUNWAYEXIT_ALLRUNWAY,
		RUNWAYEXIT_LOGICRUNWAY,
		RUNWAYEXIT_ITEM
	};

public:
	CDlgSelectAirsideOrigDest(const UINT nID,int nProjID, const CString& strSelExits, const CString& strSelStands, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectAirsideOrigDest();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEL_ORIGDEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); 
	virtual void OnOK();

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	CString GetOriginName();
	CString GetRunwayExitName();

	void PackIntoData(CIn_OutBoundRoute* pRoute) const;

	typedef CTypedPtrList<CPtrList, HTREEITEM> TreeSelectList;

protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void UpdateSplitterRange();
	void UpdateResize();

	void SetRunwayExitTreeContent(void);
	void SetOriginTreeContent(void);

	void AddAirportALTObjectList( const ALTAirport& airport, const ALTObjectList& vALTObjectList, HTREEITEM hParentItem );
	void AddObjectToTree(HTREEITEM hObjRoot, const ALTObject* pObject);

	HTREEITEM FindChildObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	RUNWAYEXITTYPE GetRunwayExitType(HTREEITEM hRunwayTreeItem);
	int GetCurALTObjectNamePos(HTREEITEM hTreeItem);
	void GetALTObjectFamilyAltID(HTREEITEM hItem, ALTObjectID& objName, CString& strStandFamily);
	void GetRunwayExitNameString(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName);

	bool IsExitSelected(const CString& strExitName);
	bool IsStandSelected(const CString& strStandName);

	void PureOriginList(const TreeSelectList& orgList, TreeSelectList& selects,
		TreeSelectList& deicepads, TreeSelectList& startpositions, TreeSelectList& taxiinterruptlines);
	BOOL RecALTObject( BOOL bHasPrev, BOOL bAll, TreeSelectList &stands, std::vector<int>& recALTObj, ALTOBJECT_TYPE objType, LPCTSTR orgTypeTag );

protected:
	CTreeCtrl m_treelRunwayExit;
	CTreeCtrl m_treeOrg;
	CSplitterControl m_wndSplitterVer;


	std::vector<int> m_vStandSelID;
	std::vector<int> m_vDeiceGroupSelID;
	std::vector<int> m_vStartPositionSelID;
	std::vector<int> m_vTaxiInterruptSelID;

	BOOL m_bAllStand;
	BOOL m_bAllDeice;
	BOOL m_bAllStartPosition;
	BOOL m_bAllTaxiInterruptLine;

	std::vector<int> m_vExitSelID;
	BOOL m_bAllRunwayExit;

	CString m_strOriginName;
	CString m_strRunwayExits;

	int m_nProjID;
	TaxiInterruptLineList m_taxiInterruptLines;

	HTREEITEM m_hStands;
	HTREEITEM m_hDeicepads;
	HTREEITEM m_hStartPositions;
	HTREEITEM m_hTaxiInterruptLines;

	HTREEITEM m_hAllStands;
	HTREEITEM m_hAllDeicepads;
	HTREEITEM m_hAllStartPositions;
	HTREEITEM m_hAllTaxiInterruptLines;

	HTREEITEM m_hAllRunwayExit;

};
