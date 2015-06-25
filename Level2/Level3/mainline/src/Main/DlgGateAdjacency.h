#pragma once
#include "../MFCExControl/XTResizeDialog.h"
#include "Inputs\GateAssignPreferenceMan.h"
#include <afxdialogex.h>

class DlgGateAdjacency : public CDialogEx
{
	DECLARE_DYNAMIC(DlgGateAdjacency)

public:
	DlgGateAdjacency(std::vector<CGateAdjacency>* pAdjacency, CWnd* pParent = NULL);
	virtual ~DlgGateAdjacency();
	enum { IDD = IDD_DIALOG_GATE_ADJACENCY };

private:
    std::vector<CGateAdjacency>* m_pAdjacency;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};
