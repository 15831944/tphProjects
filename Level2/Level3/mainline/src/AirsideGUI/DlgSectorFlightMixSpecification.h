#pragma once
#include "NodeViewDbClickHandler.h"
#include "..\MFCExControl\CoolTree.h"
#include "..\MFCExControl\ARCTreeCtrl.h"
#include "../InputAirside/ALTObject.h"
#include "Resource.h"
#include "../MFCExControl/XTResizeDialog.h"



class SectorFlightMixSpecification;

class CDlgSectorFlightMixSpecification: public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSectorFlightMixSpecification)

public:
	CDlgSectorFlightMixSpecification(int nProjID, CWnd* pParent = NULL);
	virtual~CDlgSectorFlightMixSpecification(void);

	enum { IDD = IDD_DIALOG_SECTORLIMITATION };

	enum TreeNodeType
	{
		TREENODE_NONE = 0,
		TREENODE_SECTOR,
		TREENODE_ALTITUDE,
		TREENODE_MAXLIMIT,
		TREENODE_ACTYPE,

	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMsgNew();
	afx_msg void OnMsgDel();
	afx_msg void OnMsgEdit();
	afx_msg void OnTvnSelchangedTreeSectorLimit(NMHDR *pNMHDR, LRESULT *pResult);
private:
	void NewSectorLimit();
	void NewAltitudeLimit();
	void NewACTypeLimit();

	void ReadData();
	void InitLimitationTreeCtrl();
	void UpdateState();
	TreeNodeType GetCurACTypePos(HTREEITEM hTreeItem);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	int m_nProjID;
	SectorFlightMixSpecification* m_pSpecification;

	CToolBar	m_wndToolbar;
	CARCTreeCtrl	m_wndTreeLimit;
	std::vector<ALTObject> m_vSectors;

};
