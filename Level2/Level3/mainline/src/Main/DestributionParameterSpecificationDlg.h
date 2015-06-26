#pragma once
#include "afxwin.h"
#include "Common\ProbDistManager.h"
#include "TermPlanDoc.h"
#include "MFCExControl\FloatEdit.h"
#include "d:\work\mainline\src\mfcexcontrol\floatedit.h"

class CDestributionParameterSpecificationDlg : public CDialog
{
    DECLARE_DYNAMIC(CDestributionParameterSpecificationDlg)

public:
    CDestributionParameterSpecificationDlg(CWnd* pParent = NULL);
    CDestributionParameterSpecificationDlg(CProbDistEntry* pInputEntry, CWnd* pParent = NULL);
    CDestributionParameterSpecificationDlg(ProbabilityDistribution* pInputProb, CWnd* pParent = NULL);
    virtual ~CDestributionParameterSpecificationDlg();

    enum { IDD = IDD_DIALOG_DISTRIBUTIONPARAMSPEC };

public:
    CProbDistEntry* GetSelProbEntry() const { return m_pSelProbEntry; }
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    void SetAllComboboxDropWidth();
    DECLARE_MESSAGE_MAP()
private:
    void SetAllSpinControlRange();
    void DisableAllEditBox();
    void LoadComboBoxString(CComboBox* pcb, const CPROBDISTLIST &vProb);
    long GetCheckedDistribution();
    void ReloadCheckedDistributionComboString();
    CTermPlanDoc* GetTermPlanDoc();
    bool GetFloatFromEditbox(CEdit* pEdit, float& fConstValue);
    void SpinChangeEditboxValue(CEdit* pEdit, LPNMUPDOWN pNMUpDown);
    void InitUIFromProb(const ProbabilityDistribution* pNewProb);
    void InitUIFromProbEntry( const CProbDistEntry* pEntry );

    CString GetTempConstDistributionName(CString strConstValue);
    CString GetTempUniformDistributionName(CString strMin, CString strMax);
    CString GetTempBetaDistributionName(CString strAlpha, CString strBeta, CString strMax, CString strMin);
    CString GetTempTriangleDistributionName(CString strMax, CString strMin, CString strMode);
    CString GetTempErlangDistributionName(CString strGamma, CString strBeta, CString strMu);
    CString GetTempExponentialDistributionName(CString strLambda, CString strMean);
    CString GetTempGammaDistributionName(CString strGamma, CString strBeta, CString strMu);
    CString GetTempNormalDistributionName(CString strMean, CString strStd, CString strTrunAt);
    CString GetTempWeibullDistributionName(CString strAlpha, CString strGamma, CString strMu);
    CString GetTempBernoulliDistributionName(CString str1stValue, CString str2ndValue, CString str1stPro);
    CString GetTempEmpiricalDistributionName();
    CString GetTempHistogramDistributionName();
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

    CFloatEdit m_editConstValue;
    CFloatEdit m_editUniformMin;
    CFloatEdit m_editUniformMax;
    CFloatEdit m_editBetaAlpha;
    CFloatEdit m_editBetaBeta;
    CFloatEdit m_editBetaMax;
    CFloatEdit m_editBetaMin;
    CFloatEdit m_editTriangleMax;
    CFloatEdit m_editTriangleMin;
    CFloatEdit m_editTriangleMode;
    CFloatEdit m_editErlangGamma;
    CFloatEdit m_editErlangBeta;
    CFloatEdit m_editErlangMu;
    CFloatEdit m_editExpoLambda;
    CFloatEdit m_editExpoMean;
    CFloatEdit m_editGaGamma;
    CFloatEdit m_editGammaBeta;
    CFloatEdit m_editGammaMu;
    CFloatEdit m_editNormalMean;
    CFloatEdit m_editNormalStd;
    CFloatEdit m_editNormalTrunat;
    CFloatEdit m_editWeiAlpha;
    CFloatEdit m_editWeiGamma;
    CFloatEdit m_editWeiMu;
    CFloatEdit m_editBer1stValue;
    CFloatEdit m_editBer2ndValue;
    CFloatEdit m_editBer1stPro;
protected:
    CProbDistManager* m_pProbMan;
    CProbDistEntry* m_pSelProbEntry;
    const CProbDistEntry* m_pInputEntry;
    const ProbabilityDistribution* m_pInputProb;
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedBtnOpendb();
    afx_msg void OnBnClickedBtnResetinput();
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

    afx_msg void OnDeltaposSpinConstvalue(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinUniformmin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinUniformmax(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinBetaalpha(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinBetabeta(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinBetamax(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinBetamin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinTrianglemax(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinTrianglemin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinTrianglemode(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinErlanggamma(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinErlangbeta(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinErlangmu(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinExpolambda(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinExpomean(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinGagamma(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinGammabeta(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinGammamu(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinNormalmean(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinNormalstd(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinNormaltrunat(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinWeialpha(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinWeigamma(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinBer1stvalue(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinBer2ndvalue(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinBer1stpro(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeEditConstantvalue();
    afx_msg void OnEnChangeEditUniformmin();
    afx_msg void OnEnChangeEditUniformmax();
    void ChangeUniformComboName();

    afx_msg void OnEnChangeEditBetaalpha();
    afx_msg void OnEnChangeEditBetabeta();
    afx_msg void OnEnChangeEditBetamax();
    afx_msg void OnEnChangeEditBetamin();
    void ChangeBetaComboName();

    afx_msg void OnEnChangeEditTrianglemax();
    afx_msg void OnEnChangeEditTrianglemin();
    afx_msg void OnEnChangeEditTrianglemode();
    void ChangeTriangComboName();

    afx_msg void OnEnChangeEditErlanggamma();
    afx_msg void OnEnChangeEditErlangbeta();
    afx_msg void OnEnChangeEditErlangmu();
    void ChangeErlangComboName();

    afx_msg void OnEnChangeEditExpolambda();
    afx_msg void OnEnChangeEditExpomean();
    void ChangeExpoComboName();

    afx_msg void OnEnChangeEditGagamma();
    afx_msg void OnEnChangeEditGammabeta();
    afx_msg void OnEnChangeEditGammamu();
    void ChangeGammaComboName();

    afx_msg void OnEnChangeEditNormalmean();
    afx_msg void OnEnChangeEditNormalstd();
    afx_msg void OnEnChangeEditNormaltrunat();
    void ChangeNormalComboName();

    afx_msg void OnEnChangeEditWeialpha();
    afx_msg void OnEnChangeEditWeigamma();
    afx_msg void OnEnChangeEditWeimu();
    void ChangeWeiComboName();

    afx_msg void OnEnChangeEditBer1stvalue();
    afx_msg void OnEnChangeEditBer2ndvalue();
    afx_msg void OnEnChangeEditBer1stpro();
    void ChangeBernoulliComboName();
};
