//
#pragma once
#include "../LayerListCtrl.h"
#include <common/CADInfo.h>
/////////////////////////////////////////////////////////////////////////////
// CDlgLayerOptions dialog
class CDlgLayerOptions2 : public CDialog
{
	// Construction
public:
	CDlgLayerOptions2( const LayerInfoList& layerInfos, CWnd* pParent = NULL);   // standard constructor
	BOOL IsLayerDirty() { return m_bLayersChanged; }

	// Dialog Data
	//{{AFX_DATA(CDlgLayerOptions)
	enum { IDD = IDD_LAYEROPTIONS };
	CLayerListCtrl	m_lstLayers;
	//}}AFX_DATA
	LayerInfoList m_layerInfos;


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLayerOptions)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:	
	BOOL m_bLayersChanged;


	// Generated message map functions
	//{{AFX_MSG(CDlgLayerOptions)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};