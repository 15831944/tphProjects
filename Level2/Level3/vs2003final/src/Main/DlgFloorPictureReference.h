#pragma once
#import "../../Lib/ARCImageViewer.tlb" no_auto_exclude

#include "FloorPictureCtrl.h"

class C3DView;
class IRender3DView;
class CAirsideGround;
class CRenderFloor;
class CDlgFloorPictureReference : public CDialog
{
	DECLARE_DYNAMIC(CDlgFloorPictureReference)

public:
	CDlgFloorPictureReference(CString strFilePath,int nAirportID,CRenderFloor* pFloor, IRender3DView* pView, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFloorPictureReference();

public:


	bool DoTempFallBackFinished(WPARAM wParam, LPARAM lParam);
	
protected:

	CToolBar					m_wndToolbar;
	CStatic						m_wndStaticGroup;
	CString						m_strFilePath;
	CRenderFloor				*m_pFloor;
	int							m_nLevelID;
	IRender3DView				*m_p3DView;

	double						m_dLength;
	double						m_dRotation;
	double						m_dOffsetX;
	double						m_dOffsetY;
	bool						m_bInit;

	CFloorPictureCtrl m_wndPictureCtrl;


	void UpdateUIState();

	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	void UpdateTempObjectInfo();
	void DoneEditTempObject();

	// Dialog Data
	enum { IDD = IDD_DIALOG_FLOOR_PICTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC*  pDC); 
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();	
	
	afx_msg void OnBnClickedPickReflineFrom3DView();
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);


	afx_msg void OnPanPicture();
	afx_msg void OnZoomPicture();
	afx_msg void MoveUp();
	afx_msg void MoveDown();
	afx_msg void MoveLeft();
	afx_msg void MoveRight();
	afx_msg void PickOnMap();
	afx_msg void EidtPicture();
	afx_msg LRESULT PickOnMapFromPictureEvent(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_EVENTSINK_MAP()
	void FireGetReferenceLinePositionOnMapEvent();
	afx_msg void OnBnClickedOk();

	afx_msg void OnBnClickedCancel();

	afx_msg void OnEnChangeEdit();
};
