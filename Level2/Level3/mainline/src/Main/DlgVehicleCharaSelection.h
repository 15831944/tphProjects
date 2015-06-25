#pragma once
#include "afxwin.h"

#define MULTI_VEHICLE_CHARA_COUNT 16
class LandsideVehicleTypeList;

struct VehicleCharaString
{
	int nIdx;
	CString str;
	int nLevel;
};

class CDlgVehicleCharaSelection :
	public CDialog
{
	DECLARE_DYNAMIC(CDlgVehicleCharaSelection)
public:
	CDlgVehicleCharaSelection(LandsideVehicleTypeList* pTypeList,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVehicleCharaSelection();

	// Dialog Data
	enum { IDD = IDD_DIALOG_VEHICLECHARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	void InsertVehicleType();
public:
	virtual BOOL OnInitDialog();
	std::vector<int> m_vSelChara;
	std::vector<VehicleCharaString>  m_TypeStrings;
private:
	CComboBox m_allLevelCombox[MULTI_VEHICLE_CHARA_COUNT];
	LandsideVehicleTypeList* m_pVehicleTypeList;
};

