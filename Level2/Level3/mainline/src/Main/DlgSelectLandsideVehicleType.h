#pragma once
#include <Landside/LandsideVehicleTypeNode.h>

// CDlgSelectLandsideVehicleType dialog

class CDlgSelectLandsideVehicleType : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectLandsideVehicleType)

public:
	CDlgSelectLandsideVehicleType(bool bEnableMultiSel = false, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectLandsideVehicleType();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTLANDSIDEVEHICLETYPE };
public:
    CString GetName();
    const std::vector<CString>& GetNameList()const{return m_vSelVehicles;}
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	void SetTreeContent();
	void InsertTreeItem(HTREEITEM hItem,LandsideVehicleTypeNode* pNode);

	DECLARE_MESSAGE_MAP()

private:
	std::vector<CString> m_vSelVehicles;
	LandsideVehicleBaseNode m_vehicleNodeList;
    bool m_bEnableMultiSel;
    CTreeCtrl* m_pTree;
};
