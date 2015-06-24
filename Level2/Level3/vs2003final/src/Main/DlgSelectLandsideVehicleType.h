#pragma once
#include <Landside/LandsideVehicleTypeNode.h>

// CDlgSelectLandsideVehicleType dialog

class CDlgSelectLandsideVehicleType : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectLandsideVehicleType)

public:
	CDlgSelectLandsideVehicleType(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectLandsideVehicleType();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTLANDSIDEVEHICLETYPE };
public:
	const CString& GetName()const{return m_strName;}
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	void SetTreeContent();
	void InsertTreeItem(HTREEITEM hItem,LandsideVehicleTypeNode* pNode);

	DECLARE_MESSAGE_MAP()

private:
	CString m_strName;
	LandsideVehicleBaseNode m_vehicleNodeList;
	CTreeCtrl m_wndTreeCtrl;
};
