#pragma once
#include "ResizePS\ResizablePage.h"
#include "Resource.h"
#include "afxcmn.h"
#include "..\MFCExControl\CoolTree.h"
#include "..\InputEnviroment\SourceEmissionManager.h"
#include "..\InputEnviroment\FuelProperties.h"
#include "..\InputEnviroment\AirPollutantList.h"


// CPollutantSourcePage dialog

// typedef std::map<int,CString> PollutantIDSymbolMap;

class CPollutantSourcePage : public CResizablePage
{
	DECLARE_DYNAMIC(CPollutantSourcePage)

public:
	CPollutantSourcePage(UINT nIDCaption,FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList,CWnd* pParentWnd=NULL);
// 	CPollutantSourcePage(FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList);
	CPollutantSourcePage();
	virtual ~CPollutantSourcePage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_POLLUTANTSOURCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void InitToolBar();	

	DECLARE_MESSAGE_MAP()
protected:
	CCoolTree m_tree;
	CToolBar m_toolBar;
	CWnd* m_pParentWnd;     // parent window of property sheet
	FuelProperties *m_pFuelProperties;
	CAirPollutantList *m_pAirPollutantList;
// 	PollutantIDSymbolMap *m_mapPollutantIDSymbol;
	
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void ReloadTree(){};
	virtual void OnButtonAdd(){};
	virtual void OnButtonDel(){};
// 	void SetPollutantMap(PollutantIDSymbolMap &IDSymbolMap);
	virtual void OnTvnSelchangedTreePollutantsource(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual void OnContextMenu(CWnd* pWnd, CPoint point);
};
