#pragma once

#include "LLCtrl.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../InputAirside/PanoramaViewData.h"
#include "../InputAirside/ALTAirport.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgPanorama dialog

class CTermPlanDoc;

class CDlgPanorama : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgPanorama)

public:
	CDlgPanorama(int nPrjId, int nAirportID , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPanorama();

// Dialog Data
	enum { IDD = IDD_DIALOG_PANORAMA };

	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);

	virtual void OnOK();
	virtual BOOL OnInitDialog();

	

	void InitListCtrl();
	void InitLatLongBox();

protected:
	CToolBar m_ToolBar;	


	int m_nProjID;
	int m_nAirportID;
	ALTAirport m_AirportInfo;
 
	long m_logfirstTime;
	long m_logEndTime;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	void UpdateToolBar();
	void UpdateListCtrl();


	void UpdateEditData();
	void UpdateEditLatLong();	
	void UpdateEditPosXY();	
	void UpdateEditAltitude();
	void UpdateEditTurnSpeed();
	void UpdateEditStartBearing();
	void UpdateEditEndBearing();
	void UpdateEditInclination();

	void EnableSave(BOOL b);


	void PreviewPanoramaRender( const PanoramaViewData* pData );
	void RecordPanoramaRender( PanoramaViewData* theData );

	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddPanorama();
	afx_msg void OnDeletePanorama();
	afx_msg void OnRecordPanorama();

	afx_msg void OnPreviewPanorama();

	afx_msg void OnPreviewPanoramaWithAnima();
	afx_msg void OnItemChangedListPanoramas(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	

protected:
	PanoramaViewDataList m_dataList;
public:
	CListCtrlEx m_ListCtrl;
	CLLCtrl m_editLat;
	CLLCtrl m_editLong;
	CEdit m_editPosx;
	CEdit m_editPosy;
	CEdit m_editAlt;
	CEdit m_editTurnSpd;
	CEdit m_editStartBearing;
	CEdit m_editEndBearing;
	CEdit m_editInclination;
	CStatic m_staticToolBar;
	CButton m_saveButton;

	/*int m_iInclination;
	int m_iEndBearing;
	int m_iStartBearing;
	int m_iTurnSpeed;
	int m_iAltitude;*/	
	afx_msg void OnEnKillfocusEditTunrspeed();
	afx_msg void OnEnKillfocusEditAltitude();
	afx_msg void OnEnKillfocusEditStartbearing();
	afx_msg void OnEnKillfocusEditEndbearing();
	afx_msg void OnEnKillfocusEditInclination();
	afx_msg void OnLvnEndlabeleditListPanoramaviews(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPosx();
	afx_msg void OnEnChangeEditPosy();
	afx_msg void OnEnChangeEditAltitude();
	afx_msg void OnBnClickedButtonPick();
	afx_msg void OnEnChangeEditTunrspeed();
	afx_msg void OnEnChangeEditStartbearing();
	afx_msg void OnEnChangeEditEndbearing();
	afx_msg void OnEnChangeEditInclination();
	afx_msg void OnBnClickedButtonSave();

	CTermPlanDoc* GetDocument() const;
};
