#pragma once

#include "../MFCExControl/ToolTipDialog.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "Inputs/GateAssignPreferenceMan.h"
#include "Inputs/IN_TERM.H"
#include "Engine/GATE.H"

class InputTerminal;

class DlgGateAdjacency : public CToolTipDialog
{
public:
    DlgGateAdjacency(CGateAssignPreferenceMan* pGateMan, InputTerminal* _pInputTerm, CWnd* pParent = NULL);
    virtual ~DlgGateAdjacency();
    enum { IDD = IDD_DIALOG_GATE_ADJACENCY };

protected:
    DECLARE_MESSAGE_MAP()
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();

    void RefreshLayout(int cx = -1, int cy = -1);
    void UpdateToolBarState();
    void InitListControl();
    void AddGateAdjacencyItem(const CGateAdjacency* pGateAdj);
    void SetListItemContent(int nIndex, const CGateAdjacency* pGateAdj);
    int FindGateAdjacency(const CGateAdjacency& gateAdj);
    CString GetProjPath();
private:
    std::vector<CGateAdjacency*> m_vGateAdjas;
    std::vector<CGateAdjacency*> m_vDelGateAdjas; // Gate Adjacency list going to be deleted.
    CGateAssignPreferenceMan* m_pGateMan;
    InputTerminal* m_pInputTerm;
    CToolBar m_wndToolBar;
    CListCtrlEx m_wndListCtrl;
    GateType m_Type ; // ArrGate / DepGate
protected:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
    afx_msg void OnToolbarAdd();
    afx_msg void OnToolbarDelete();
    afx_msg void OnToolBarEdit();
    afx_msg void OnButtonSave();
    afx_msg LRESULT OnCollumnIndex(WPARAM wParam,  LPARAM lParam);
    afx_msg void OnLvnSelItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);

public:
    GateType GetType() const { return m_Type; }
    void SetType(int nType) { m_Type = (GateType)nType; }
    void SetType(GateType nType) { m_Type = nType; }
};

