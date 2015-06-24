#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../Landside/VehicleItemDetailList.h"
#include "../Landside/VehicleItemDetail.h"
//#include "ConDBExListCtrl.h"
//#include "condblistctrl.h"
#include <vector>
#include "../Landside/InputLandside.h"
#include "..\Landside\CLandSidePublicVehicleType.h"
#include "ARCListCtrl.h"
// CDlgVehicleOperation dialog

class InputTerminal;

class CDlgVehicleOperation : public CDialog
{
	DECLARE_DYNAMIC(CDlgVehicleOperation)

public:
	CDlgVehicleOperation(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVehicleOperation();
	CDlgVehicleOperation(int pProjID,Terminal *pTerminal,CWnd*pParent=NULL);
   afx_msg void OnDblclkListBox();
   afx_msg void onSelChange();
    virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnChangeVehicleType( WPARAM wParam, LPARAM lParam);
	 void OnNewItem();
	 void OnDelItem();
	 void OnEditItem();
	 void OnAddEnd();
	afx_msg void OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnSelchangeListBoxElement();
	afx_msg void OnCancel();
	afx_msg void OnSave();
	
	void setToolBar();
	void setControl();
	void setControlContent();
	void SetPublicBusListCtrlHeader();
	void setIterateListCtrHeader();
	void setTaxiListCtrHeader();
	void SetPublicListContent();
	void SetPublicBusStationList(LandSidePublicVehicleType *type);
	void SetIterateListContent();
	void SetTaxiListContent();
	void SetPrivateListContent();
	void ShowPublicBus();
	void ShowRandomTaxi();
	void ShowPrivateVehicle();
	void ShowIterateVehicle();
	void HidePublicBus();
	void HideRandomTaxi();
	void HidePrivateVehicle();
	void HideIterateVehicle();
	void SetListBoxContent();
	void SetComboText(enumVehicleLandSideType vehicleType);
	int GetSelectedListItem();
	void UpdateToolBar();
	void getGuiValue();
	
// Dialog Data
	enum { IDD = IDD_DIALOG_OperationVehicleType };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   
	DECLARE_MESSAGE_MAP()

protected:
	CDateTimeCtrl m_DateStartTime;
	CDateTimeCtrl m_endTime;
	CEdit m_EditInterval;
	CListCtrlEx m_listCtrl;
	CListBox m_listBoxSpecifications;
	CComboBox com_OperationType;
	Terminal *m_pTerminal;
	int m_pProjID;
	CToolBar m_wndToolBar;
	
	ILayoutObjectList* m_pObjlist;

	CButton m_RadionoPark;
public:
	CButton m_shorPark;
	CButton m_longPark;
	BOOL isFirst;
	std::vector<enumVehicleLandSideType> selectTypes;
	std::vector<int>listboxSeletIndex;
	BOOL isSave;
	BOOL isListBoxSelChange;
	InputLandside *m_pLandSide;
	BOOL bIsSize;
protected:
	
	InputTerminal* GetInputTerminal();
	
	
	CStatic m_labelMin;
public:
	CARCListCtrl m_listTaxiAndIterate;
	CStatic m_pStaticficication;
	CStatic m_pStaticOperationType;
	CStatic m_pStaticStartTime;
	CStatic m_pStaticEndTime;
	CStatic m_pStaticInterval;
	CButton m_btnSave;
	CButton m_btnOk;
	CButton m_btnCancel;
	int m_boxWidth;
	int m_staticBoxWidth;
	int m_boxHeight;
	int m_StaticBoxHeight;
	CRect m_noRadioRect;
    CRect m_LongRadioRect;
	CRect m_ShorRadioRect;
	CRect m_btnSaveRect;
	CRect m_btnCancelRect;
	CRect m_btnOkRect;
	int m_cx;
	int m_cy;
	int m_UnitType;
	CComboBox m_comStartDay;
	CComboBox m_ComEndDay;
	afx_msg void OnNMDblclkListOperationdata(NMHDR *pNMHDR, LRESULT *pResult);
};
