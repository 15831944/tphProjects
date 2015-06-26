#pragma once
#include <Landside/LandsideVehicleTypeNode.h>
#include "MuliSelTreeCtrlEx.h"

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

	virtual void SetTreeContent();
	virtual void InsertTreeItem(HTREEITEM hItem,LandsideVehicleTypeNode* pNode);

	DECLARE_MESSAGE_MAP()

protected:
    LandsideVehicleBaseNode m_vehicleNodeList;
private:
	CString m_strName;
	CTreeCtrl m_wndTreeCtrl;
};

class CDlgMultiSelectLandsideVehicleType : public CDlgSelectLandsideVehicleType
{
public:
    CDlgMultiSelectLandsideVehicleType(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDlgMultiSelectLandsideVehicleType();

public:
    const std::vector<CString>& GetNameList()const{return m_vSelVehicles;}
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void InsertTreeItem(HTREEITEM hItem,LandsideVehicleTypeNode* pNode);
    virtual void SetTreeContent();
    virtual void OnOK();
private:
    std::vector<CString> m_vSelVehicles;
    CMuliSeltTreeCtrlEx m_MultiSelTree;
};

