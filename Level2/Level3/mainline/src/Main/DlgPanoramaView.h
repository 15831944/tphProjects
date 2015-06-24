#pragma once

#include "../MFCExControl/ListCtrlEx.h"
#include "Fallback.h"

class PanoramaViewDataList;

class CDlgPanoramaView : public CXTResizeDialog
{

	DECLARE_DYNAMIC(CDlgPanoramaView)

public:
	enum { IDD = IDD_DIALOG_PANORAMA };

public:
	CDlgPanoramaView(PanoramaViewDataList& viewData,  CWnd* pParent = NULL);
	virtual ~CDlgPanoramaView(void);


protected:
	virtual void DoDataExchange(CDataExchange* pDx);


	DECLARE_MESSAGE_MAP()
	afx_msg void OnNewItem();
	afx_msg void OnDelItem();

	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


	BOOL InitToolBar();

protected:
    CToolBar m_ToolBar;
	int m_nSelectItem;
public:
	PanoramaViewDataList& m_theDataList;	
	
	
};
