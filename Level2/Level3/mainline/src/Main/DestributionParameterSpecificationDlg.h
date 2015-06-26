#pragma once
#include "afxwin.h"
#include "Common\ProbDistManager.h"

class CDestributionParameterSpecificationDlg : public CDialog
{
    DECLARE_DYNAMIC(CDestributionParameterSpecificationDlg)

public:
    CDestributionParameterSpecificationDlg(CWnd* pParent = NULL);
    virtual ~CDestributionParameterSpecificationDlg();


    enum { IDD = IDD_DIALOG_DISTRIBUTIONPARAMSPEC };

public:
    afx_msg void OnBnClickedBtnOpendb();
    afx_msg void OnBnClickedBtnResetinput();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
private:
    void SetAllSpinControlRange();
    void DisableAllEditBox();
    void AddProbNamesToCombo(CComboBox* pcb, const CPROBDISTLIST &vProb);
protected:
    CComboBox m_comboConst;
    CComboBox m_comboUniform;
    CComboBox m_comboBeta;
    CComboBox m_comboTriangle;
    CComboBox m_comboErlang;
    CComboBox m_comboExponential;
    CComboBox m_comboGamma;
    CComboBox m_comboNormal;
    CComboBox m_comboWeibull;
    CComboBox m_comboBernoulli;
    CComboBox m_comboEmpirical;
    CComboBox m_comboHistogram;
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedRadioConstant();
    afx_msg void OnBnClickedRadioUniform();
    afx_msg void OnBnClickedRadioBeta();
    afx_msg void OnBnClickedRadioTriangle();
    afx_msg void OnBnClickedRadioErlang();
    afx_msg void OnBnClickedRadioExponential();
    afx_msg void OnBnClickedRadioGamma();
    afx_msg void OnBnClickedRadioNormal();
    afx_msg void OnBnClickedRadioWeibull();
    afx_msg void OnBnClickedRadioBernoulli();
    afx_msg void OnBnClickedRadioEmpirical();
    afx_msg void OnBnClickedRadioHistogram();
    afx_msg void OnCbnSelchangeComboConstant();
    afx_msg void OnCbnSelchangeComboUniform();
    afx_msg void OnCbnSelchangeComboBeta();
    afx_msg void OnCbnSelchangeComboTriangle();
    afx_msg void OnCbnSelchangeComboErlang();
    afx_msg void OnCbnSelchangeComboExponential();
    afx_msg void OnCbnSelchangeComboGamma();
    afx_msg void OnCbnSelchangeComboNormal();
    afx_msg void OnCbnSelchangeComboWeibull();
    afx_msg void OnCbnSelchangeComboBernoulli();
    afx_msg void OnCbnSelchangeComboEmpirical();
    afx_msg void OnCbnSelchangeComboHistogram();
protected:
    CProbDistManager* m_pProbMan;
};
