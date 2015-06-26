#include "stdafx.h"
#include "TermPlan.h"
#include "DestributionParameterSpecificationDlg.h"
#include "DlgProbDist.h"
#include "Inputs\ProbDistHelper.h"

static const short iMin=-32767, iMax=32767;

IMPLEMENT_DYNAMIC(CDestributionParameterSpecificationDlg, CDialog)

    CDestributionParameterSpecificationDlg::CDestributionParameterSpecificationDlg(CWnd* pParent)
    : CDialog(CDestributionParameterSpecificationDlg::IDD, pParent),
     m_pProbMan(NULL),
     m_pSelProbEntry(NULL),
     m_pInputProb(NULL)
{
}

CDestributionParameterSpecificationDlg::CDestributionParameterSpecificationDlg(CProbDistEntry* pInputEntry, CWnd* pParent)
    : CDialog(CDestributionParameterSpecificationDlg::IDD, pParent),
    m_pProbMan(NULL),
    m_pSelProbEntry(NULL),
    m_pInputEntry(pInputEntry),
    m_pInputProb(NULL)
{

}

CDestributionParameterSpecificationDlg::CDestributionParameterSpecificationDlg( ProbabilityDistribution* pInputProb, CWnd* pParent)
    : CDialog(CDestributionParameterSpecificationDlg::IDD, pParent),
    m_pProbMan(NULL),
    m_pSelProbEntry(NULL),
    m_pInputEntry(NULL),
    m_pInputProb(pInputProb)
{

}


CDestributionParameterSpecificationDlg::~CDestributionParameterSpecificationDlg()
{
}

void CDestributionParameterSpecificationDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_CONSTANT, m_comboConst);
    DDX_Control(pDX, IDC_COMBO_UNIFORM, m_comboUniform);
    DDX_Control(pDX, IDC_COMBO_BETA, m_comboBeta);
    DDX_Control(pDX, IDC_COMBO_TRIANGLE, m_comboTriangle);
    DDX_Control(pDX, IDC_COMBO_ERLANG, m_comboErlang);
    DDX_Control(pDX, IDC_COMBO_EXPONENTIAL, m_comboExponential);
    DDX_Control(pDX, IDC_COMBO_GAMMA, m_comboGamma);
    DDX_Control(pDX, IDC_COMBO_NORMAL, m_comboNormal);
    DDX_Control(pDX, IDC_COMBO_WEIBULL, m_comboWeibull);
    DDX_Control(pDX, IDC_COMBO_BERNOULLI, m_comboBernoulli);
    DDX_Control(pDX, IDC_COMBO_EMPIRICAL, m_comboEmpirical);
    DDX_Control(pDX, IDC_COMBO_HISTOGRAM, m_comboHistogram);
    DDX_Control(pDX, IDC_EDIT_CONSTANTVALUE, m_editConstValue);

    DDX_Control(pDX, IDC_EDIT_UNIFORMMIN, m_editUniformMin);
    DDX_Control(pDX, IDC_EDIT_UNIFORMMAX, m_editUniformMax);
    DDX_Control(pDX, IDC_EDIT_BETAALPHA, m_editBetaAlpha);
    DDX_Control(pDX, IDC_EDIT_BETABETA, m_editBetaBeta);
    DDX_Control(pDX, IDC_EDIT_BETAMAX, m_editBetaMax);
    DDX_Control(pDX, IDC_EDIT_BETAMIN, m_editBetaMin);
    DDX_Control(pDX, IDC_EDIT_TRIANGLEMAX, m_editTriangleMax);
    DDX_Control(pDX, IDC_EDIT_TRIANGLEMIN, m_editTriangleMin);
    DDX_Control(pDX, IDC_EDIT_TRIANGLEMODE, m_editTriangleMode);
    DDX_Control(pDX, IDC_EDIT_ERLANGGAMMA, m_editErlangGamma);
    DDX_Control(pDX, IDC_EDIT_ERLANGBETA, m_editErlangBeta);
    DDX_Control(pDX, IDC_EDIT_ERLANGMU, m_editErlangMu);
    DDX_Control(pDX, IDC_EDIT_EXPOLAMBDA, m_editExpoLambda);
    DDX_Control(pDX, IDC_EDIT_EXPOMEAN, m_editExpoMean);
    DDX_Control(pDX, IDC_EDIT_GAGAMMA, m_editGaGamma);
    DDX_Control(pDX, IDC_EDIT_GAMMABETA, m_editGammaBeta);
    DDX_Control(pDX, IDC_EDIT_GAMMAMU, m_editGammaMu);
    DDX_Control(pDX, IDC_EDIT_NORMALMEAN, m_editNormalMean);
    DDX_Control(pDX, IDC_EDIT_NORMALSTD, m_editNormalStd);
    DDX_Control(pDX, IDC_EDIT_NORMALTRUNAT, m_editNormalTrunat);
    DDX_Control(pDX, IDC_EDIT_WEIALPHA, m_editWeiAlpha);
    DDX_Control(pDX, IDC_EDIT_WEIGAMMA, m_editWeiGamma);
    DDX_Control(pDX, IDC_EDIT_WEIMU, m_editWeiMu);
    DDX_Control(pDX, IDC_EDIT_BER1STVALUE, m_editBer1stValue);
    DDX_Control(pDX, IDC_EDIT_BER2NDVALUE, m_editBer2ndValue);
    DDX_Control(pDX, IDC_EDIT_BER1STPRO, m_editBer1stPro);
}


BEGIN_MESSAGE_MAP(CDestributionParameterSpecificationDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_OPENDB, OnBnClickedBtnOpendb)
    ON_BN_CLICKED(IDC_BTN_RESETINPUT, OnBnClickedBtnResetinput)
    ON_BN_CLICKED(IDC_RADIO_CONSTANT, OnBnClickedRadioConstant)
    ON_BN_CLICKED(IDC_RADIO_UNIFORM, OnBnClickedRadioUniform)
    ON_BN_CLICKED(IDC_RADIO_BETA, OnBnClickedRadioBeta)
    ON_BN_CLICKED(IDC_RADIO_TRIANGLE, OnBnClickedRadioTriangle)
    ON_BN_CLICKED(IDC_RADIO_ERLANG, OnBnClickedRadioErlang)
    ON_BN_CLICKED(IDC_RADIO_EXPONENTIAL, OnBnClickedRadioExponential)
    ON_BN_CLICKED(IDC_RADIO_GAMMA, OnBnClickedRadioGamma)
    ON_BN_CLICKED(IDC_RADIO_NORMAL, OnBnClickedRadioNormal)
    ON_BN_CLICKED(IDC_RADIO_WEIBULL, OnBnClickedRadioWeibull)
    ON_BN_CLICKED(IDC_RADIO_BERNOULLI, OnBnClickedRadioBernoulli)
    ON_BN_CLICKED(IDC_RADIO_EMPIRICAL, OnBnClickedRadioEmpirical)
    ON_BN_CLICKED(IDC_RADIO_HISTOGRAM, OnBnClickedRadioHistogram)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_CBN_SELCHANGE(IDC_COMBO_CONSTANT, OnCbnSelchangeComboConstant)
    ON_CBN_SELCHANGE(IDC_COMBO_UNIFORM, OnCbnSelchangeComboUniform)
    ON_CBN_SELCHANGE(IDC_COMBO_BETA, OnCbnSelchangeComboBeta)
    ON_CBN_SELCHANGE(IDC_COMBO_TRIANGLE, OnCbnSelchangeComboTriangle)
    ON_CBN_SELCHANGE(IDC_COMBO_ERLANG, OnCbnSelchangeComboErlang)
    ON_CBN_SELCHANGE(IDC_COMBO_EXPONENTIAL, OnCbnSelchangeComboExponential)
    ON_CBN_SELCHANGE(IDC_COMBO_GAMMA, OnCbnSelchangeComboGamma)
    ON_CBN_SELCHANGE(IDC_COMBO_NORMAL, OnCbnSelchangeComboNormal)
    ON_CBN_SELCHANGE(IDC_COMBO_WEIBULL, OnCbnSelchangeComboWeibull)
    ON_CBN_SELCHANGE(IDC_COMBO_BERNOULLI, OnCbnSelchangeComboBernoulli)
    ON_CBN_SELCHANGE(IDC_COMBO_EMPIRICAL, OnCbnSelchangeComboEmpirical)
    ON_CBN_SELCHANGE(IDC_COMBO_HISTOGRAM, OnCbnSelchangeComboHistogram)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CONSTVALUE, OnDeltaposSpinConstvalue)

    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_UNIFORMMIN, OnDeltaposSpinUniformmin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_UNIFORMMAX, OnDeltaposSpinUniformmax)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BETAALPHA, OnDeltaposSpinBetaalpha)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BETABETA, OnDeltaposSpinBetabeta)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BETAMAX, OnDeltaposSpinBetamax)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BETAMIN, OnDeltaposSpinBetamin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRIANGLEMAX, OnDeltaposSpinTrianglemax)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRIANGLEMIN, OnDeltaposSpinTrianglemin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRIANGLEMODE, OnDeltaposSpinTrianglemode)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ERLANGGAMMA, OnDeltaposSpinErlanggamma)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ERLANGBETA, OnDeltaposSpinErlangbeta)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ERLANGMU, OnDeltaposSpinErlangmu)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EXPOLAMBDA, OnDeltaposSpinExpolambda)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EXPOMEAN, OnDeltaposSpinExpomean)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GAGAMMA, OnDeltaposSpinGagamma)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GAMMABETA, OnDeltaposSpinGammabeta)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GAMMAMU, OnDeltaposSpinGammamu)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NORMALMEAN, OnDeltaposSpinNormalmean)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NORMALSTD, OnDeltaposSpinNormalstd)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NORMALTRUNAT, OnDeltaposSpinNormaltrunat)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WEIALPHA, OnDeltaposSpinWeialpha)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WEIGAMMA, OnDeltaposSpinWeigamma)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BER1STVALUE, OnDeltaposSpinBer1stvalue)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BER2NDVALUE, OnDeltaposSpinBer2ndvalue)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BER1STPRO, OnDeltaposSpinBer1stpro)
    ON_EN_CHANGE(IDC_EDIT_CONSTANTVALUE, OnEnChangeEditConstantvalue)
    ON_EN_CHANGE(IDC_EDIT_UNIFORMMIN, OnEnChangeEditUniformmin)
    ON_EN_CHANGE(IDC_EDIT_UNIFORMMAX, OnEnChangeEditUniformmax)
    ON_EN_CHANGE(IDC_EDIT_BETAALPHA, OnEnChangeEditBetaalpha)
    ON_EN_CHANGE(IDC_EDIT_BETABETA, OnEnChangeEditBetabeta)
    ON_EN_CHANGE(IDC_EDIT_BETAMAX, OnEnChangeEditBetamax)
    ON_EN_CHANGE(IDC_EDIT_BETAMIN, OnEnChangeEditBetamin)
    ON_EN_CHANGE(IDC_EDIT_TRIANGLEMAX, OnEnChangeEditTrianglemax)
    ON_EN_CHANGE(IDC_EDIT_TRIANGLEMIN, OnEnChangeEditTrianglemin)
    ON_EN_CHANGE(IDC_EDIT_TRIANGLEMODE, OnEnChangeEditTrianglemode)
    ON_EN_CHANGE(IDC_EDIT_ERLANGGAMMA, OnEnChangeEditErlanggamma)
    ON_EN_CHANGE(IDC_EDIT_ERLANGBETA, OnEnChangeEditErlangbeta)
    ON_EN_CHANGE(IDC_EDIT_ERLANGMU, OnEnChangeEditErlangmu)
    ON_EN_CHANGE(IDC_EDIT_EXPOLAMBDA, OnEnChangeEditExpolambda)
    ON_EN_CHANGE(IDC_EDIT_EXPOMEAN, OnEnChangeEditExpomean)
    ON_EN_CHANGE(IDC_EDIT_GAGAMMA, OnEnChangeEditGagamma)
    ON_EN_CHANGE(IDC_EDIT_GAMMABETA, OnEnChangeEditGammabeta)
    ON_EN_CHANGE(IDC_EDIT_GAMMAMU, OnEnChangeEditGammamu)
    ON_EN_CHANGE(IDC_EDIT_NORMALMEAN, OnEnChangeEditNormalmean)
    ON_EN_CHANGE(IDC_EDIT_NORMALSTD, OnEnChangeEditNormalstd)
    ON_EN_CHANGE(IDC_EDIT_NORMALTRUNAT, OnEnChangeEditNormaltrunat)
    ON_EN_CHANGE(IDC_EDIT_WEIALPHA, OnEnChangeEditWeialpha)
    ON_EN_CHANGE(IDC_EDIT_WEIGAMMA, OnEnChangeEditWeigamma)
    ON_EN_CHANGE(IDC_EDIT_WEIMU, OnEnChangeEditWeimu)
    ON_EN_CHANGE(IDC_EDIT_BER1STVALUE, OnEnChangeEditBer1stvalue)
    ON_EN_CHANGE(IDC_EDIT_BER2NDVALUE, OnEnChangeEditBer2ndvalue)
    ON_EN_CHANGE(IDC_EDIT_BER1STPRO, OnEnChangeEditBer1stpro)
END_MESSAGE_MAP()

BOOL CDestributionParameterSpecificationDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetAllSpinControlRange();
    DisableAllEditBox();
    SetAllComboboxDropWidth();

    m_editBetaAlpha.SetPrecision(0);
    m_editBetaBeta.SetPrecision(0);
    m_editErlangGamma.SetPrecision(0);

    m_pProbMan = GetTermPlanDoc()->GetTerminal().m_pAirportDB->getProbDistMan();

    if(m_pInputProb != NULL)
    {
        ASSERT(m_pInputEntry == NULL);
        InitUIFromProb(m_pInputProb);
    }
    if(m_pInputEntry != NULL)
    {
        ASSERT(m_pInputProb == NULL);
        const CProbDistEntry* pEntry = m_pInputEntry;
        InitUIFromProbEntry(pEntry);
    }
    return TRUE;
}

void CDestributionParameterSpecificationDlg::SetAllComboboxDropWidth()
{
    int nCbDropWidth = m_comboConst.GetDroppedWidth()*2;
    m_comboConst.SetDroppedWidth(nCbDropWidth);
    m_comboUniform.SetDroppedWidth(nCbDropWidth);
    m_comboBeta.SetDroppedWidth(nCbDropWidth);
    m_comboTriangle.SetDroppedWidth(nCbDropWidth);
    m_comboErlang.SetDroppedWidth(nCbDropWidth);
    m_comboExponential.SetDroppedWidth(nCbDropWidth);
    m_comboGamma.SetDroppedWidth(nCbDropWidth);
    m_comboNormal.SetDroppedWidth(nCbDropWidth);
    m_comboWeibull.SetDroppedWidth(nCbDropWidth);
    m_comboBernoulli.SetDroppedWidth(nCbDropWidth);
    m_comboEmpirical.SetDroppedWidth(nCbDropWidth);
    m_comboHistogram.SetDroppedWidth(nCbDropWidth);
}

void CDestributionParameterSpecificationDlg::SetAllSpinControlRange()
{
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_CONSTVALUE))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_UNIFORMMIN))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_UNIFORMMAX))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BETAALPHA))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BETABETA))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BETAMAX))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BETAMIN))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TRIANGLEMAX))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TRIANGLEMIN))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TRIANGLEMODE))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_ERLANGGAMMA))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_ERLANGBETA))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_ERLANGMU))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_EXPOLAMBDA))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_EXPOMEAN))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_GAGAMMA))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_GAMMABETA))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_GAMMAMU))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_NORMALMEAN))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_NORMALSTD))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_NORMALTRUNAT))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WEIALPHA))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WEIGAMMA))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WEIMU))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BER1STVALUE))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BER2NDVALUE))->SetRange(iMin, iMax);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BER1STPRO))->SetRange(iMin, iMax);
}

void CDestributionParameterSpecificationDlg::DisableAllEditBox()
{
    GetDlgItem(IDC_COMBO_CONSTANT)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_UNIFORM)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_BETA)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_TRIANGLE)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_ERLANG)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_EXPONENTIAL)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_GAMMA)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_NORMAL)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_WEIBULL)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_BERNOULLI)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_EMPIRICAL)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_HISTOGRAM)->EnableWindow(FALSE);

    GetDlgItem(IDC_EDIT_CONSTANTVALUE)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_CONSTVALUE)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_UNIFORMMIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_UNIFORMMIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_UNIFORMMAX)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_UNIFORMMAX)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BETAALPHA)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_BETAALPHA)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BETABETA)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_BETABETA)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BETAMAX)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_BETAMAX)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BETAMIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_BETAMIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_TRIANGLEMAX)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_TRIANGLEMAX)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_TRIANGLEMIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_TRIANGLEMIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_TRIANGLEMODE)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_TRIANGLEMODE)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_ERLANGGAMMA)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_ERLANGGAMMA)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_ERLANGBETA)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_ERLANGBETA)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_ERLANGMU)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_ERLANGMU)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_EXPOLAMBDA)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_EXPOLAMBDA)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_EXPOMEAN)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_EXPOMEAN)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_GAGAMMA)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_GAGAMMA)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_GAMMABETA)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_GAMMABETA)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_GAMMAMU)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_GAMMAMU)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_NORMALMEAN)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_NORMALMEAN)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_NORMALSTD)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_NORMALSTD)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_NORMALTRUNAT)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_NORMALTRUNAT)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_WEIALPHA)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_WEIALPHA)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_WEIGAMMA)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_WEIGAMMA)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_WEIMU)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_WEIMU)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BER1STVALUE)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_BER1STVALUE)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BER2NDVALUE)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_BER2NDVALUE)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BER1STPRO)->EnableWindow(FALSE);
    GetDlgItem(IDC_SPIN_BER1STPRO)->EnableWindow(FALSE);

}

void CDestributionParameterSpecificationDlg::OnBnClickedBtnOpendb()
{
    CProbDistEntry* pPDEntry = NULL;
    CDlgProbDist dlg(GetTermPlanDoc()->GetTerminal().m_pAirportDB, true, this);
    dlg.DoModal();
    ReloadCheckedDistributionComboString();
}


void CDestributionParameterSpecificationDlg::OnBnClickedBtnResetinput()
{
    GetDlgItem(IDC_EDIT_CONSTANTVALUE)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_UNIFORMMIN)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_UNIFORMMAX)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_BETAALPHA)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_BETABETA)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_BETAMAX)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_BETAMIN)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_TRIANGLEMAX)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_TRIANGLEMIN)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_TRIANGLEMODE)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_ERLANGGAMMA)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_ERLANGBETA)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_ERLANGMU)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_EXPOLAMBDA)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_EXPOMEAN)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_GAGAMMA)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_GAMMABETA)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_GAMMAMU)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_NORMALMEAN)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_NORMALSTD)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_NORMALTRUNAT)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_WEIALPHA)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_WEIGAMMA)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_WEIMU)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_BER1STVALUE)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_BER2NDVALUE)->SetWindowText(_T(""));
    GetDlgItem(IDC_EDIT_BER1STPRO)->SetWindowText(_T(""));

    m_comboConst.SetWindowText(_T(""));
    m_comboUniform.SetWindowText(_T(""));
    m_comboBeta.SetWindowText(_T(""));
    m_comboTriangle.SetWindowText(_T(""));
    m_comboErlang.SetWindowText(_T(""));
    m_comboExponential.SetWindowText(_T(""));
    m_comboGamma.SetWindowText(_T(""));
    m_comboNormal.SetWindowText(_T(""));
    m_comboWeibull.SetWindowText(_T(""));
    m_comboBernoulli.SetWindowText(_T(""));
    m_comboEmpirical.SetWindowText(_T(""));
    m_comboHistogram.SetWindowText(_T(""));

    DisableAllEditBox();
    if(GetCheckedDistribution() != -1)
        ((CButton*)GetDlgItem(GetCheckedDistribution()))->SetCheck(FALSE);

    if(m_pInputProb != NULL)
    {
        ASSERT(m_pInputEntry == NULL);
        InitUIFromProb(m_pInputProb);
    }
    if(m_pInputEntry != NULL)
    {
        ASSERT(m_pInputProb == NULL);
        const CProbDistEntry* pEntry = m_pInputEntry;
        InitUIFromProbEntry(pEntry);
    }
}



void CDestributionParameterSpecificationDlg::OnBnClickedRadioConstant()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_CONSTANT)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_CONSTANTVALUE)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_CONSTVALUE)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(CONSTANT);
    LoadComboBoxString(&m_comboConst, vProb);

    CString strEntryName;
    m_comboConst.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(CONSTANT);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioUniform()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_UNIFORM)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_UNIFORMMIN)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_UNIFORMMIN)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_UNIFORMMAX)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_UNIFORMMAX)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(UNIFORM);
    LoadComboBoxString(&m_comboUniform, vProb);

    CString strEntryName;
    m_comboUniform.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(UNIFORM);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioBeta()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_BETA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BETAALPHA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_BETAALPHA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BETABETA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_BETABETA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BETAMAX)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_BETAMAX)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BETAMIN)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_BETAMIN)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(BETA);
    LoadComboBoxString(&m_comboBeta, vProb);

    CString strEntryName;
    m_comboBeta.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(BETA);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioTriangle()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_TRIANGLE)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_TRIANGLEMAX)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_TRIANGLEMAX)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_TRIANGLEMIN)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_TRIANGLEMIN)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_TRIANGLEMODE)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_TRIANGLEMODE)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(TRIANGLE);
    LoadComboBoxString(&m_comboTriangle, vProb);

    CString strEntryName;
    m_comboTriangle.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(TRIANGLE);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioErlang()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_ERLANG)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_ERLANGGAMMA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_ERLANGGAMMA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_ERLANGBETA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_ERLANGBETA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_ERLANGMU)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_ERLANGMU)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(ERLANG);
    LoadComboBoxString(&m_comboErlang, vProb);

    CString strEntryName;
    m_comboErlang.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(ERLANG);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioExponential()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_EXPONENTIAL)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_EXPOLAMBDA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_EXPOLAMBDA)->EnableWindow(TRUE);
//     GetDlgItem(IDC_EDIT_EXPOMEAN)->EnableWindow(TRUE);
//     GetDlgItem(IDC_SPIN_EXPOMEAN)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(EXPONENTIAL);
    LoadComboBoxString(&m_comboExponential, vProb);

    CString strEntryName;
    m_comboExponential.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(EXPONENTIAL);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioGamma()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_GAMMA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_GAGAMMA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_GAGAMMA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_GAMMABETA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_GAMMABETA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_GAMMAMU)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_GAMMAMU)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(GAMMA);
    LoadComboBoxString(&m_comboGamma, vProb);

    CString strEntryName;
    m_comboGamma.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(GAMMA);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioNormal()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_NORMAL)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_NORMALMEAN)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_NORMALMEAN)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_NORMALSTD)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_NORMALSTD)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_NORMALTRUNAT)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_NORMALTRUNAT)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(NORMAL);
    LoadComboBoxString(&m_comboNormal, vProb);

    CString strEntryName;
    m_comboNormal.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(NORMAL);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioWeibull()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_WEIBULL)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_WEIALPHA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_WEIALPHA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_WEIGAMMA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_WEIGAMMA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_WEIMU)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_WEIMU)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(WEIBULL);
    LoadComboBoxString(&m_comboWeibull, vProb);

    CString strEntryName;
    m_comboWeibull.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(WEIBULL);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioBernoulli()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_BERNOULLI)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BER1STVALUE)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_BER1STVALUE)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BER2NDVALUE)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_BER2NDVALUE)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_BER1STPRO)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_BER1STPRO)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(BERNOULLI);
    LoadComboBoxString(&m_comboBernoulli, vProb);

    CString strEntryName;
    m_comboBernoulli.GetWindowText(strEntryName);
    if(strEntryName.IsEmpty())
    {
        ProbabilityDistribution* pNewProb = ProbDistHelper::CreateProbabilityDistribution(BERNOULLI);
        InitUIFromProb(pNewProb);
        delete pNewProb;
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioEmpirical()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_EMPIRICAL)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(EMPIRICAL);
    LoadComboBoxString(&m_comboEmpirical, vProb);
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioHistogram()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_HISTOGRAM)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(HISTOGRAM);
    LoadComboBoxString(&m_comboHistogram, vProb);
}

void CDestributionParameterSpecificationDlg::LoadComboBoxString(CComboBox* pcb, const CPROBDISTLIST &vProb)
{
    ASSERT(pcb != NULL);
    CString strCombo;
    pcb->GetWindowText(strCombo);
    pcb->ResetContent();
    CPROBDISTLIST::const_iterator itor = vProb.begin();
    for(; itor!=vProb.end(); ++itor)
    {
        pcb->AddString((*itor)->m_csName);
    }
    pcb->SetWindowText(strCombo);
}

CTermPlanDoc* CDestributionParameterSpecificationDlg::GetTermPlanDoc()
{
    CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
    ASSERT( pMDIActive != NULL );
    CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
    ASSERT( pDoc!= NULL );
    return pDoc;
}

bool CDestributionParameterSpecificationDlg::GetFloatFromEditbox(CEdit* pEdit, float& fConstValue)
{
    CString strValue;
    pEdit->GetWindowText(strValue);
    fConstValue = (float)atof(strValue);
    return true;
}

void CDestributionParameterSpecificationDlg::OnBnClickedOk()
{
    if(IsDlgButtonChecked(IDC_RADIO_CONSTANT))
    {
        float fConstValue;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_CONSTANTVALUE), fConstValue))
            return;

        CString strEntryName;
        m_comboConst.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            ConstantDistribution* pNewProb = new ConstantDistribution(fConstValue);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == CONSTANT);
                ((ConstantDistribution*)pEntry->m_pProbDist)->setConstant(fConstValue);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new ConstantDistribution(fConstValue));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_UNIFORM))
    {
        float fUniformMin, fUniformMax;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_UNIFORMMIN), fUniformMin))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_UNIFORMMAX), fUniformMax))
            return;

        CString strEntryName;
        m_comboUniform.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            UniformDistribution* pNewProb = new UniformDistribution(fUniformMin, fUniformMax);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == UNIFORM);
                ((UniformDistribution*)pEntry->m_pProbDist)->setMin(fUniformMin);
                ((UniformDistribution*)pEntry->m_pProbDist)->setMax(fUniformMax);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new UniformDistribution(fUniformMin, fUniformMax));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_BETA))
    {
        float fBetaAlpha, fBetaBeta, fBetaMax, fBetaMin;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_BETAALPHA), fBetaAlpha))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_BETABETA), fBetaBeta))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_BETAMAX), fBetaMax))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_BETAMIN), fBetaMin))
            return;

        CString strEntryName;
        m_comboBeta.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            BetaDistribution* pNewProb = new BetaDistribution(fBetaMin, fBetaMax, fBetaAlpha, fBetaBeta);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == BETA);
                ((BetaDistribution*)pEntry->m_pProbDist)->resetValues(fBetaMin, fBetaMax, (int)fBetaAlpha, (int)fBetaBeta);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new BetaDistribution(fBetaMin, fBetaMax, fBetaAlpha, fBetaBeta));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_TRIANGLE))
    {
        float fTriangleMax, fTriangleMin, fTriangleMode;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_TRIANGLEMAX), fTriangleMax))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_TRIANGLEMIN), fTriangleMin))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_TRIANGLEMODE), fTriangleMode))
            return;

        CString strEntryName;
        m_comboTriangle.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            TriangleDistribution* pNewProb = new TriangleDistribution(fTriangleMax, fTriangleMin, fTriangleMode);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == TRIANGLE);
                ((TriangleDistribution*)pEntry->m_pProbDist)->setMax(fTriangleMax);
                ((TriangleDistribution*)pEntry->m_pProbDist)->setMin(fTriangleMin);
                ((TriangleDistribution*)pEntry->m_pProbDist)->setMode(fTriangleMode);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new TriangleDistribution(fTriangleMax, fTriangleMin, fTriangleMode));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_ERLANG))
    {
        float fErlangGamma, fErlangBeta, fErlangMu;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_ERLANGGAMMA), fErlangGamma))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_ERLANGBETA), fErlangBeta))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_ERLANGMU), fErlangMu))
            return;

        CString strEntryName;
        m_comboErlang.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            ErlangDistribution* pNewProb = new ErlangDistribution((int)fErlangGamma, fErlangBeta, fErlangMu);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == ERLANG);
                ((ErlangDistribution*)pEntry->m_pProbDist)->setGamma((int)fErlangGamma);
                ((ErlangDistribution*)pEntry->m_pProbDist)->setBeta(fErlangBeta);
                ((ErlangDistribution*)pEntry->m_pProbDist)->setMu(fErlangMu);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new ErlangDistribution((int)fErlangGamma, fErlangBeta, fErlangMu));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_EXPONENTIAL))
    {
        float fExpLambda, fExpMean;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_ERLANGGAMMA), fExpLambda))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_ERLANGBETA), fExpMean))
            return;

        CString strEntryName;
        m_comboExponential.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            ExponentialDistribution* pNewProb = new ExponentialDistribution(fExpLambda);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == EXPONENTIAL);
                ((ExponentialDistribution*)pEntry->m_pProbDist)->setLambda(fExpLambda);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new ExponentialDistribution(fExpLambda));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_GAMMA))
    {
        float fGammaGamma, fGammaBeta, fGammaMu;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_GAGAMMA), fGammaGamma))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_GAMMABETA), fGammaBeta))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_GAMMAMU), fGammaMu))
            return;

        CString strEntryName;
        m_comboGamma.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            GammaDistribution* pNewProb = new GammaDistribution(fGammaGamma, fGammaBeta, fGammaMu);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == GAMMA);
                ((GammaDistribution*)pEntry->m_pProbDist)->setGamma(fGammaGamma);
                ((GammaDistribution*)pEntry->m_pProbDist)->setBeta(fGammaBeta);
                ((GammaDistribution*)pEntry->m_pProbDist)->setMu(fGammaMu);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new GammaDistribution(fGammaGamma, fGammaBeta, fGammaMu));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_NORMAL))
    {
        float fNormalMean, fNormalStd, fNormalTrunat;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_NORMALMEAN), fNormalMean))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_NORMALSTD), fNormalStd))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_NORMALTRUNAT), fNormalTrunat))
            return;

        CString strEntryName;
        m_comboNormal.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            NormalDistribution* pNewProb = new NormalDistribution(fNormalMean, fNormalStd, (int)fNormalTrunat);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == GAMMA);
                ((NormalDistribution*)pEntry->m_pProbDist)->setMean(fNormalMean);
                ((NormalDistribution*)pEntry->m_pProbDist)->setStdDev(fNormalStd);
                ((NormalDistribution*)pEntry->m_pProbDist)->setTruncation((int)fNormalTrunat);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new NormalDistribution(fNormalMean, fNormalStd, (int)fNormalTrunat));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_WEIBULL))
    {
        float fWeiAlpha, fWeiGamma, fWeiMu;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_WEIALPHA), fWeiAlpha))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_WEIGAMMA), fWeiGamma))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_WEIMU), fWeiMu))
            return;

        CString strEntryName;
        m_comboWeibull.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            WeibullDistribution* pNewProb = new WeibullDistribution(fWeiAlpha, fWeiGamma, fWeiMu);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == WEIBULL);
                ((WeibullDistribution*)pEntry->m_pProbDist)->setAlpha(fWeiAlpha);
                ((WeibullDistribution*)pEntry->m_pProbDist)->setGamma(fWeiGamma);
                ((WeibullDistribution*)pEntry->m_pProbDist)->setMu(fWeiMu);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new WeibullDistribution(fWeiAlpha, fWeiGamma, fWeiMu));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_BERNOULLI))
    {
        float fBer1stValue, fBer2ndValue, fBer1stPro;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_BER1STVALUE), fBer1stValue))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_BER2NDVALUE), fBer2ndValue))
            return;
        if(!GetFloatFromEditbox((CEdit*)GetDlgItem(IDC_EDIT_BER1STPRO), fBer1stPro))
            return;

        CString strEntryName;
        m_comboWeibull.GetWindowText(strEntryName);
        if(strEntryName.IsEmpty())
        {
            BernoulliDistribution* pNewProb = new BernoulliDistribution(fBer1stValue, fBer2ndValue, fBer1stPro);
            pNewProb->screenPrint(strEntryName.GetBuffer(256)); // generate name by value
            strEntryName.ReleaseBuffer();

            if(m_pProbMan->getItemByName(strEntryName) != NULL) // handle duplicated name
                strEntryName = m_pProbMan->GetCopyName(strEntryName);

            CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, pNewProb);
            m_pProbMan->AddItem(pNewEntry);
            m_pSelProbEntry = pNewEntry;
        }
        else
        {
            CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
            if(pEntry != NULL)
            {
                // update existing distribution entry.
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == BERNOULLI);
                ((BernoulliDistribution*)pEntry->m_pProbDist)->setValue1(fBer1stValue);
                ((BernoulliDistribution*)pEntry->m_pProbDist)->setValue2(fBer2ndValue);
                ((BernoulliDistribution*)pEntry->m_pProbDist)->setProb1(fBer1stPro);
                m_pSelProbEntry = pEntry;
            }
            else
            {
                // generate new distribution entry.
                CProbDistEntry* pNewEntry = new CProbDistEntry(strEntryName, new BernoulliDistribution(fBer1stValue, fBer2ndValue, fBer1stPro));
                m_pProbMan->AddItem(pNewEntry);
                m_pSelProbEntry = pNewEntry;
            }
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_EMPIRICAL))
    {
        CString strEntryName;
        m_comboEmpirical.GetWindowText(strEntryName);
        CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
        if(strEntryName.IsEmpty() || pEntry == NULL)
        {
            MessageBox("Please select an empirical distribution.");
            m_comboEmpirical.SetFocus();
            return;
        }
        else
        {
            m_pSelProbEntry = pEntry;
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_HISTOGRAM))
    {
        CString strEntryName;
        m_comboHistogram.GetWindowText(strEntryName);
        CProbDistEntry* pEntry = m_pProbMan->getItemByName(strEntryName);
        if(strEntryName.IsEmpty() || pEntry == NULL)
        {
            MessageBox("Please select a histogram distribution.");
            m_comboHistogram.SetFocus();
            return;
        }
        else
        {
            m_pSelProbEntry = pEntry;
        }
    }

    CDialog::OnOK();
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboConstant()
{
    CString strSel;
    m_comboConst.GetLBText(m_comboConst.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    ConstantDistribution* pProb = (ConstantDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getConstant());
    GetDlgItem(IDC_EDIT_CONSTANTVALUE)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboUniform()
{
    CString strSel;
    m_comboUniform.GetLBText(m_comboUniform.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    UniformDistribution* pProb = (UniformDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getMin());
    GetDlgItem(IDC_EDIT_UNIFORMMIN)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getMax());
    GetDlgItem(IDC_EDIT_UNIFORMMAX)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboBeta()
{
    CString strSel;
    m_comboBeta.GetLBText(m_comboBeta.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    BetaDistribution* pProb = (BetaDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getAlpha());
    GetDlgItem(IDC_EDIT_BETAALPHA)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getBeta());
    GetDlgItem(IDC_EDIT_BETABETA)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getMax());
    GetDlgItem(IDC_EDIT_BETAMAX)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getMin());
    GetDlgItem(IDC_EDIT_BETAMIN)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboTriangle()
{
    CString strSel;
    m_comboTriangle.GetLBText(m_comboTriangle.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    TriangleDistribution* pProb = (TriangleDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getMax());
    GetDlgItem(IDC_EDIT_TRIANGLEMAX)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getMin());
    GetDlgItem(IDC_EDIT_TRIANGLEMIN)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getMode());
    GetDlgItem(IDC_EDIT_TRIANGLEMODE)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboErlang()
{
    CString strSel;
    m_comboErlang.GetLBText(m_comboErlang.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    ErlangDistribution* pProb = (ErlangDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getGamma());
    GetDlgItem(IDC_EDIT_ERLANGGAMMA)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getBeta());
    GetDlgItem(IDC_EDIT_ERLANGBETA)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getMu());
    GetDlgItem(IDC_EDIT_ERLANGMU)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboExponential()
{
    CString strSel;
    m_comboExponential.GetLBText(m_comboExponential.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    ExponentialDistribution* pProb = (ExponentialDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getLambda());
    GetDlgItem(IDC_EDIT_EXPOLAMBDA)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getMean());
    GetDlgItem(IDC_EDIT_EXPOMEAN)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboGamma()
{
    CString strSel;
    m_comboGamma.GetLBText(m_comboGamma.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    GammaDistribution* pProb = (GammaDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getGamma());
    GetDlgItem(IDC_EDIT_GAGAMMA)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getBeta());
    GetDlgItem(IDC_EDIT_GAMMABETA)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getMu());
    GetDlgItem(IDC_EDIT_GAMMAMU)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboNormal()
{
    CString strSel;
    m_comboNormal.GetLBText(m_comboNormal.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    NormalDistribution* pProb = (NormalDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getMean());
    GetDlgItem(IDC_EDIT_NORMALMEAN)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getStdDev());
    GetDlgItem(IDC_EDIT_NORMALSTD)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getTruncation());
    GetDlgItem(IDC_EDIT_NORMALTRUNAT)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboWeibull()
{
    CString strSel;
    m_comboWeibull.GetLBText(m_comboWeibull.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    WeibullDistribution* pProb = (WeibullDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getAlpha());
    GetDlgItem(IDC_EDIT_WEIALPHA)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getGamma());
    GetDlgItem(IDC_EDIT_WEIGAMMA)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getMu());
    GetDlgItem(IDC_EDIT_WEIMU)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboBernoulli()
{
    CString strSel;
    m_comboBernoulli.GetLBText(m_comboBernoulli.GetCurSel(), strSel);
    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    BernoulliDistribution* pProb = (BernoulliDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%.2f"), pProb->getValue1());
    GetDlgItem(IDC_EDIT_BER1STVALUE)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getValue2());
    GetDlgItem(IDC_EDIT_BER2NDVALUE)->SetWindowText(strTemp);
    strTemp.Format(_T("%.2f"), pProb->getProb1());
    GetDlgItem(IDC_EDIT_BER1STPRO)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboEmpirical()
{
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboHistogram()
{
}

void CDestributionParameterSpecificationDlg::SpinChangeEditboxValue(CEdit* pEdit, LPNMUPDOWN pNMUpDown)
{
    CString strValue;
    pEdit->GetWindowText(strValue);
    float fInput = (float)atof(strValue);
    if(pNMUpDown->iDelta > 0 && fInput < iMax - 1)
    {
        fInput += 1;
    }
    else if(pNMUpDown->iDelta < 0 && fInput > iMin+1)
    {
        fInput -= 1;
    }
    strValue.Format(_T("%.2f"), fInput);
    pEdit->SetWindowText(strValue);
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinConstvalue(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONSTANTVALUE);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinUniformmin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_UNIFORMMIN);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinUniformmax(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_UNIFORMMAX);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}
void CDestributionParameterSpecificationDlg::OnDeltaposSpinBetaalpha(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BETAALPHA);
    CString strValue;
    pEdit->GetWindowText(strValue);
    int iInput = atoi(strValue);
    if(pNMUpDown->iDelta > 0 && iInput < iMax - 1)
    {
        iInput += 1;
    }
    else if(pNMUpDown->iDelta < 0 && iInput > iMin+1)
    {
        iInput -= 1;
    }
    strValue.Format(_T("%d"), iInput);
    pEdit->SetWindowText(strValue);
    *pResult = 0;
}
void CDestributionParameterSpecificationDlg::OnDeltaposSpinBetabeta(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BETABETA);
    CString strValue;
    pEdit->GetWindowText(strValue);
    int iInput = atoi(strValue);
    if(pNMUpDown->iDelta > 0 && iInput < iMax - 1)
    {
        iInput += 1;
    }
    else if(pNMUpDown->iDelta < 0 && iInput > iMin+1)
    {
        iInput -= 1;
    }
    strValue.Format(_T("%d"), iInput);
    pEdit->SetWindowText(strValue);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinBetamax(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BETAMAX);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinBetamin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BETAMIN);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinTrianglemax(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TRIANGLEMAX);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinTrianglemin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TRIANGLEMIN);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinTrianglemode(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_TRIANGLEMODE);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinErlanggamma(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ERLANGGAMMA);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    CString strValue;
    pEdit->GetWindowText(strValue);
    int iInput = atoi(strValue);
    if(pNMUpDown->iDelta > 0 && iInput < iMax - 1)
    {
        iInput += 1;
    }
    else if(pNMUpDown->iDelta < 0 && iInput > iMin+1)
    {
        iInput -= 1;
    }
    strValue.Format(_T("%d"), iInput);
    pEdit->SetWindowText(strValue);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinErlangbeta(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ERLANGBETA);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinErlangmu(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ERLANGMU);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinExpolambda(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_EXPOLAMBDA);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinExpomean(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_EXPOMEAN);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinGagamma(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_GAGAMMA);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinGammabeta(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_GAMMABETA);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinGammamu(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_GAMMAMU);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinNormalmean(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NORMALMEAN);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinNormalstd(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NORMALSTD);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinNormaltrunat(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NORMALTRUNAT);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinWeialpha(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_WEIALPHA);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinWeigamma(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_WEIGAMMA);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinBer1stvalue(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BER1STVALUE);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinBer2ndvalue(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BER2NDVALUE);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

void CDestributionParameterSpecificationDlg::OnDeltaposSpinBer1stpro(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BER1STPRO);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}

long CDestributionParameterSpecificationDlg::GetCheckedDistribution()
{
    if(((CButton*)GetDlgItem(IDC_RADIO_CONSTANT))->GetCheck())
        return IDC_RADIO_CONSTANT;
    if(((CButton*)GetDlgItem(IDC_RADIO_UNIFORM))->GetCheck())
        return IDC_RADIO_UNIFORM;
    if(((CButton*)GetDlgItem(IDC_RADIO_BETA))->GetCheck())
        return IDC_RADIO_BETA;
    if(((CButton*)GetDlgItem(IDC_RADIO_TRIANGLE))->GetCheck())
        return IDC_RADIO_TRIANGLE;
    if(((CButton*)GetDlgItem(IDC_RADIO_ERLANG))->GetCheck())
        return IDC_RADIO_ERLANG;
    if(((CButton*)GetDlgItem(IDC_RADIO_EXPONENTIAL))->GetCheck())
        return IDC_RADIO_EXPONENTIAL;
    if(((CButton*)GetDlgItem(IDC_RADIO_GAMMA))->GetCheck())
        return IDC_RADIO_GAMMA;
    if(((CButton*)GetDlgItem(IDC_RADIO_NORMAL))->GetCheck())
        return IDC_RADIO_NORMAL;
    if(((CButton*)GetDlgItem(IDC_RADIO_WEIBULL))->GetCheck())
        return IDC_RADIO_WEIBULL;
    if(((CButton*)GetDlgItem(IDC_RADIO_BERNOULLI))->GetCheck())
        return IDC_RADIO_BERNOULLI;
    if(((CButton*)GetDlgItem(IDC_RADIO_EMPIRICAL))->GetCheck())
        return IDC_RADIO_EMPIRICAL;
    if(((CButton*)GetDlgItem(IDC_RADIO_HISTOGRAM))->GetCheck())
        return IDC_RADIO_HISTOGRAM;
    return -1;
}

void CDestributionParameterSpecificationDlg::ReloadCheckedDistributionComboString()
{
    CComboBox* pCB = NULL;
    CPROBDISTLIST vProb;
    if(((CButton*)GetDlgItem(IDC_RADIO_CONSTANT))->GetCheck())
    {
        pCB = &m_comboConst;
        vProb = m_pProbMan->getItemListByType(CONSTANT);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_UNIFORM))->GetCheck())
    {
        pCB = &m_comboUniform;
        vProb = m_pProbMan->getItemListByType(UNIFORM);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_BETA))->GetCheck())
    {
        pCB = &m_comboBeta;
        vProb = m_pProbMan->getItemListByType(BETA);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_TRIANGLE))->GetCheck())
    {
        pCB = &m_comboTriangle;
        vProb = m_pProbMan->getItemListByType(TRIANGLE);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_ERLANG))->GetCheck())
    {
        pCB = &m_comboErlang;
        vProb = m_pProbMan->getItemListByType(ERLANG);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_EXPONENTIAL))->GetCheck())
    {
        pCB = &m_comboExponential;
        vProb = m_pProbMan->getItemListByType(EXPONENTIAL);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_GAMMA))->GetCheck())
    {
        pCB = &m_comboGamma;
        vProb = m_pProbMan->getItemListByType(GAMMA);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_NORMAL))->GetCheck())
    {
        pCB = &m_comboNormal;
        vProb = m_pProbMan->getItemListByType(NORMAL);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_WEIBULL))->GetCheck())
    {
        pCB = &m_comboWeibull;
        vProb = m_pProbMan->getItemListByType(WEIBULL);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_BERNOULLI))->GetCheck())
    {
        pCB = &m_comboBernoulli;
        vProb = m_pProbMan->getItemListByType(BERNOULLI);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_EMPIRICAL))->GetCheck())
    {
        pCB = &m_comboEmpirical;
        vProb = m_pProbMan->getItemListByType(EMPIRICAL);
    }
    if(((CButton*)GetDlgItem(IDC_RADIO_HISTOGRAM))->GetCheck())
    {
        pCB = &m_comboHistogram;
        vProb = m_pProbMan->getItemListByType(HISTOGRAM);
    }

    if(pCB != NULL)
        LoadComboBoxString(pCB, vProb);
}

void CDestributionParameterSpecificationDlg::OnEnChangeEditConstantvalue()
{
    CString strName;
    m_comboConst.GetWindowText(strName);
    if(strName.IsEmpty() ||
        (strName.Find(_T("Constant: [")) == 0 && strName[strName.GetLength()-1] == ']'))
    {
        CString strConstValue;
        m_editConstValue.GetWindowText(strConstValue);
        m_comboConst.SetWindowText(GetTempConstDistributionName(strConstValue));
    }
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditUniformmin()
{
    ChangeUniformComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditUniformmax()
{
    ChangeUniformComboName();
}

void CDestributionParameterSpecificationDlg::ChangeUniformComboName()
{
    CString strName;
    m_comboUniform.GetWindowText(strName);
    if(strName.IsEmpty() ||
        strName.Find(_T("Uniform: [")) == 0 && strName[strName.GetLength()-1] == ']')
    {
        CString strMin, strMax;
        m_editUniformMin.GetWindowText(strMin);
        m_editUniformMax.GetWindowText(strMax);
        m_comboUniform.SetWindowText(GetTempUniformDistributionName(strMin, strMax));
    }
}

void CDestributionParameterSpecificationDlg::OnEnChangeEditBetaalpha()
{
    ChangeBetaComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditBetabeta()
{
    ChangeBetaComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditBetamax()
{
    ChangeBetaComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditBetamin()
{
    ChangeBetaComboName();
}

void CDestributionParameterSpecificationDlg::ChangeBetaComboName()
{
    CString strName;
    m_comboBeta.GetWindowText(strName);
    if(strName.IsEmpty() ||
        strName.Find(_T("Beta: [") == 0) && strName[strName.GetLength()-1] == ']')
    {
        CString strAlpha, strBeta, strMax, strMin;
        m_editBetaAlpha.GetWindowText(strAlpha);
        m_editBetaBeta.GetWindowText(strBeta);
        m_editBetaMax.GetWindowText(strMax);
        m_editBetaMin.GetWindowText(strMin);
        m_comboBeta.SetWindowText(GetTempBetaDistributionName(strAlpha, strBeta, strMax, strMin));
    }
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditTrianglemax()
{
    ChangeTriangComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditTrianglemin()
{
    ChangeTriangComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditTrianglemode()
{
    ChangeTriangComboName();
}

void CDestributionParameterSpecificationDlg::ChangeTriangComboName()
{
    CString strName;
    m_comboTriangle.GetWindowText(strName);
    if(strName.IsEmpty() ||
        strName.Find(_T("ssss") == 0) && strName[strName.GetLength()-1] == ']')
    {
        CString strMax, strMin, strMode;
        m_editTriangleMax.GetWindowText(strMax);
        m_editTriangleMin.GetWindowText(strMin);
        m_editTriangleMode.GetWindowText(strMode);
        m_comboTriangle.SetWindowText(GetTempTriangleDistributionName(strMax, strMin, strMode));
    }
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditErlanggamma()
{
    ChangeErlangComboName();
}

void CDestributionParameterSpecificationDlg::OnEnChangeEditErlangbeta()
{
    ChangeErlangComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditErlangmu()
{
    ChangeErlangComboName();
}

void CDestributionParameterSpecificationDlg::ChangeErlangComboName()
{
    CString strName;
    m_comboErlang.GetWindowText(strName);
    if(strName.IsEmpty() ||
        strName.Find(_T("Erlang: [") == 0) && strName[strName.GetLength()-1] == ']')
    {
        CString strGamma, strBeta, strMu;
        m_editErlangGamma.GetWindowText(strGamma);
        m_editErlangBeta.GetWindowText(strBeta);
        m_editErlangMu.GetWindowText(strMu);
        m_comboErlang.SetWindowText(GetTempErlangDistributionName(strGamma, strBeta, strMu));
    }
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditExpolambda()
{
    CString strLambda, strBeta;
    m_editExpoLambda.GetWindowText(strLambda);
    double fLambda = atof(strLambda);
    if(-0.00001 < fLambda && fLambda < 0.00001)
    {
        strBeta = _T("error");
    }
    else
    {
        double fBeta = 1.0f/fLambda;
        strBeta.Format("%.2f", fBeta);
    }
    m_editExpoMean.SetWindowText(strBeta);
    ChangeExpoComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditExpomean()
{
}

void CDestributionParameterSpecificationDlg::ChangeExpoComboName()
{
    CString strName;
    m_comboExponential.GetWindowText(strName);
    if(strName.IsEmpty() ||
        strName.Find(_T("Exponential: [") == 0) && strName[strName.GetLength()-1] == ']')
    {
        CString strLambda, strMean;
        m_editExpoLambda.GetWindowText(strLambda);
        m_editExpoMean.GetWindowText(strLambda);
        m_comboExponential.SetWindowText(GetTempExponentialDistributionName(strLambda, strMean));
    }
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditGagamma()
{
    ChangeGammaComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditGammabeta()
{
    ChangeGammaComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditGammamu()
{
    ChangeGammaComboName();
}

void CDestributionParameterSpecificationDlg::ChangeGammaComboName()
{
    CString strName;
    m_comboGamma.GetWindowText(strName);
    if(strName.IsEmpty() ||
        strName.Find(_T("Gamma: [") == 0) && strName[strName.GetLength()-1] == ']')
    {
        CString strGamma, strBeta, strMu;
        m_editGaGamma.GetWindowText(strGamma);
        m_editGammaBeta.GetWindowText(strBeta);
        m_editGammaMu.GetWindowText(strMu);
        m_comboErlang.SetWindowText(GetTempGammaDistributionName(strGamma, strBeta, strMu));
    }
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditNormalmean()
{
    ChangeNormalComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditNormalstd()
{
    ChangeNormalComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditNormaltrunat()
{
    ChangeNormalComboName();
}

void CDestributionParameterSpecificationDlg::ChangeNormalComboName()
{
    CString strName;
    m_comboNormal.GetWindowText(strName);
    if(strName.IsEmpty() ||
        strName.Find(_T("Normal: [") == 0) && strName[strName.GetLength()-1] == ']')
    {
        CString strMean, strStd, strTrunAt;
        m_editNormalMean.GetWindowText(strMean);
        m_editNormalStd.GetWindowText(strStd);
        m_editNormalTrunat.GetWindowText(strTrunAt);
        m_comboNormal.SetWindowText(GetTempNormalDistributionName(strMean, strStd, strTrunAt));
    }
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditWeialpha()
{
    ChangeWeiComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditWeigamma()
{
    ChangeWeiComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditWeimu()
{
    ChangeWeiComboName();
}

void CDestributionParameterSpecificationDlg::ChangeWeiComboName()
{
    CString strName;
    m_comboWeibull.GetWindowText(strName);
    if(strName.IsEmpty() ||
        strName.Find(_T("Weibull: [") == 0) && strName[strName.GetLength()-1] == ']')
    {
        CString strAlpha, strGamma, strMu;
        m_editWeiAlpha.GetWindowText(strAlpha);
        m_editWeiGamma.GetWindowText(strGamma);
        m_editWeiMu.GetWindowText(strMu);
        m_comboWeibull.SetWindowText(GetTempWeibullDistributionName(strAlpha, strGamma, strMu));
    }
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditBer1stvalue()
{
    ChangeBernoulliComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditBer2ndvalue()
{
    ChangeBernoulliComboName();
}


void CDestributionParameterSpecificationDlg::OnEnChangeEditBer1stpro()
{
    ChangeBernoulliComboName();
}

void CDestributionParameterSpecificationDlg::ChangeBernoulliComboName()
{
    CString strName;
    m_comboBernoulli.GetWindowText(strName);
    if(strName.IsEmpty() ||
        strName.Find(_T("Bernoulli: [") == 0) && strName[strName.GetLength()-1] == ']')
    {
        CString str1stValue, str2ndValue, str1stPro;
        m_editBer1stValue.GetWindowText(str1stValue);
        m_editBer2ndValue.GetWindowText(str2ndValue);
        m_editBer1stPro.GetWindowText(str1stPro);
        m_comboBernoulli.SetWindowText(GetTempBernoulliDistributionName(str1stValue, str2ndValue, str1stPro));
    }
}

CString CDestributionParameterSpecificationDlg::GetTempConstDistributionName(CString strConstValue)
{
    double fValue = atof(strConstValue);
    CString strResult;
    strResult.Format(_T("Constant: [%.2f]"), fValue);
    return strResult;
}

CString CDestributionParameterSpecificationDlg::GetTempUniformDistributionName(CString strMin, CString strMax)
{
    double fMin = atof(strMin);
    double fMax = atof(strMax);
    CString strResult;
    strResult.Format(_T("Uniform: [%.2f, %.2f]"), fMin, fMax);
    return strResult;
}

CString CDestributionParameterSpecificationDlg::GetTempBetaDistributionName(CString strAlpha, CString strBeta, CString strMax, CString strMin)
{
    int iAlpha = atoi(strAlpha);
    int iBeta = atoi(strBeta);
    double fMax = atof(strMax);
    double fMin = atof(strMin);

    CString strResult;
    if(iAlpha == DEFAULT_ALPHA && iBeta == DEFAULT_BETA)
        strResult.Format(_T("Beta: [%.2f,%.2f]"), fMin, fMax);
    else
        strResult.Format(_T("Beta: [%.2f,%.2f;%d;%d]"), fMin, fMax, iAlpha, iBeta);
    return strResult;
}

CString CDestributionParameterSpecificationDlg::GetTempTriangleDistributionName(CString strMax, CString strMin, CString strMode)
{
    double fMin = atof(strMin);
    double fMax = atof(strMax);
    double fMode = atof(strMode);
    CString strResult;
    strResult.Format(_T("Triangle: [%.2f, %.2f, %.2f]"), fMin, fMax, fMode);
    return strResult;
}

CString CDestributionParameterSpecificationDlg::GetTempErlangDistributionName(CString strGamma, CString strBeta, CString strMu)
{
    int iGamma = atoi(strGamma);
    double fBeta = atof(strBeta);
    double fMu = atof(strMu);
    CString strResult;
    strResult.Format(_T("Erlang: [%d, %.2f; %.2f]"), iGamma, fBeta, fMu);
    return strResult;
}

CString CDestributionParameterSpecificationDlg::GetTempExponentialDistributionName(CString strLambda, CString strMean)
{
    double fLambda = atof(strLambda);
    CString strResult;
    strResult.Format(_T("Exponential: [%.2f]"), fLambda);
    return strResult;
}

CString CDestributionParameterSpecificationDlg::GetTempGammaDistributionName(CString strGamma, CString strBeta, CString strMu)
{
    double fGamma = atof(strGamma);
    double fBeta = atof(strBeta);
    double fMu = atof(strMu);
    CString strResult;
    strResult.Format(_T("Gamma: [%.2f, %.2f; %.2f]"), fGamma, fBeta, fMu);
    return strResult;
}

CString CDestributionParameterSpecificationDlg::GetTempNormalDistributionName(CString strMean, CString strStd, CString strTrunAt)
{
    double fMean = atof(strMean);
    double fStd = atof(strStd);
    double iTrun = atof(strTrunAt);
    CString strResult;
    strResult.Format(_T("Normal: [%.2f, %.2f, %d]"), fMean, fStd, iTrun);
    return strResult;
}

CString CDestributionParameterSpecificationDlg::GetTempWeibullDistributionName(CString strAlpha, CString strGamma, CString strMu)
{
    double fAlpha = atof(strAlpha);
    double fGamma = atof(strAlpha);
    double fMu = atof(strAlpha);
    CString strResult;
    strResult.Format(_T("Weibull: [%.2f, %.2f;  %.2f]"), fAlpha, fGamma, fMu);
    return strResult;
}

CString CDestributionParameterSpecificationDlg::GetTempBernoulliDistributionName(CString str1stValue, CString str2ndValue, CString str1stPro)
{
    double f1stValue = atof(str1stValue);
    double f2ndValue = atof(str2ndValue);
    double f1stPro = atof(str1stPro);
    CString strResult;
    strResult.Format(_T("Bernoulli: [%.2f, %.2f;  %.2f, %.2f]"), f1stValue, f2ndValue, f1stPro, 1.0);
    return strResult;
}


CString CDestributionParameterSpecificationDlg::GetTempEmpiricalDistributionName()
{
    return _T("");
}

CString CDestributionParameterSpecificationDlg::GetTempHistogramDistributionName()
{
    return _T("");
}

void CDestributionParameterSpecificationDlg::InitUIFromProb(const ProbabilityDistribution* pNewProb)
{
    switch(pNewProb->getProbabilityType())
    {
    case CONSTANT:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_CONSTANT))->SetCheck(TRUE);
            CString strEntryName;
            pNewProb->screenPrint(strEntryName.GetBuffer(256));
            strEntryName.ReleaseBuffer();
            m_comboConst.SetWindowText(strEntryName);
            OnBnClickedRadioConstant();

            CString strEdit;
            strEdit.Format(_T("%.2f"), ((ConstantDistribution*)pNewProb)->getConstant());
            m_editConstValue.SetWindowText(strEdit);
        }
        break;
    case UNIFORM:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_UNIFORM))->SetCheck(TRUE);
            CString strNewEntryName;
            pNewProb->screenPrint(strNewEntryName.GetBuffer(256));
            strNewEntryName.ReleaseBuffer();
            m_comboUniform.SetWindowText(strNewEntryName);
            OnBnClickedRadioUniform();

            CString strEdit;
            strEdit.Format("%.2f", ((UniformDistribution*)pNewProb)->getMin());
            m_editUniformMin.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((UniformDistribution*)pNewProb)->getMax());
            m_editUniformMax.SetWindowText(strEdit);
        }
        break;
    case BETA:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_BETA))->SetCheck(TRUE);
            CString strNewEntryName;
            pNewProb->screenPrint(strNewEntryName.GetBuffer(256));
            strNewEntryName.ReleaseBuffer();
            m_comboBeta.SetWindowText(strNewEntryName);
            OnBnClickedRadioBeta();

            CString strEdit;
            strEdit.Format("%d", ((BetaDistribution*)pNewProb)->getAlpha());
            m_editBetaAlpha.SetWindowText(strEdit);
            strEdit.Format("%d", ((BetaDistribution*)pNewProb)->getBeta());
            m_editBetaBeta.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((BetaDistribution*)pNewProb)->getMax());
            m_editBetaMax.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((BetaDistribution*)pNewProb)->getMin());
            m_editBetaMin.SetWindowText(strEdit);
        }
        break;
    case TRIANGLE:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_TRIANGLE))->SetCheck(TRUE);
            CString strNewEntryName;
            pNewProb->screenPrint(strNewEntryName.GetBuffer(256));
            strNewEntryName.ReleaseBuffer();
            m_comboTriangle.SetWindowText(strNewEntryName);
            OnBnClickedRadioTriangle();

            CString strEdit;
            strEdit.Format("%.2f", ((TriangleDistribution*)pNewProb)->getMax());
            m_editTriangleMax.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((TriangleDistribution*)pNewProb)->getMin());
            m_editTriangleMin.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((TriangleDistribution*)pNewProb)->getMode());
            m_editTriangleMode.SetWindowText(strEdit);
        }
        break;
    case ERLANG:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_ERLANG))->SetCheck(TRUE);
            CString strNewEntryName;
            pNewProb->screenPrint(strNewEntryName.GetBuffer(256));
            strNewEntryName.ReleaseBuffer();
            m_comboErlang.SetWindowText(strNewEntryName);
            OnBnClickedRadioErlang();

            CString strEdit;
            strEdit.Format("%d", ((ErlangDistribution*)pNewProb)->getGamma());
            m_editErlangGamma.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((ErlangDistribution*)pNewProb)->getBeta());
            m_editErlangBeta.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((ErlangDistribution*)pNewProb)->getMu());
            m_editErlangMu.SetWindowText(strEdit);
        }
        break;
    case EXPONENTIAL:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_EXPONENTIAL))->SetCheck(TRUE);
            CString strNewEntryName;
            pNewProb->screenPrint(strNewEntryName.GetBuffer(256));
            strNewEntryName.ReleaseBuffer();
            m_comboExponential.SetWindowText(strNewEntryName);
            OnBnClickedRadioExponential();

            CString strEdit;
            strEdit.Format("%.2f", ((ExponentialDistribution*)pNewProb)->getLambda());
            m_editExpoLambda.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((ExponentialDistribution*)pNewProb)->getMean());
            m_editExpoMean.SetWindowText(strEdit);
        }
        break;
    case GAMMA:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_GAMMA))->SetCheck(TRUE);
            CString strNewEntryName;
            pNewProb->screenPrint(strNewEntryName.GetBuffer(256));
            strNewEntryName.ReleaseBuffer();
            m_comboGamma.SetWindowText(strNewEntryName);
            OnBnClickedRadioGamma();

            CString strEdit;
            strEdit.Format("%d", ((GammaDistribution*)pNewProb)->getGamma());
            m_editGaGamma.SetWindowText(strEdit);
            strEdit.Format("%d", ((GammaDistribution*)pNewProb)->getBeta());
            m_editGammaBeta.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((GammaDistribution*)pNewProb)->getMu());
            m_editGammaMu.SetWindowText(strEdit);
        }
        break;
    case NORMAL:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_NORMAL))->SetCheck(TRUE);
            CString strNewEntryName;
            pNewProb->screenPrint(strNewEntryName.GetBuffer(256));
            strNewEntryName.ReleaseBuffer();
            m_comboNormal.SetWindowText(strNewEntryName);
            OnBnClickedRadioNormal();

            CString strEdit;
            strEdit.Format("%d", ((NormalDistribution*)pNewProb)->getMean());
            m_editNormalMean.SetWindowText(strEdit);
            strEdit.Format("%d", ((NormalDistribution*)pNewProb)->getStdDev());
            m_editNormalStd.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((NormalDistribution*)pNewProb)->getTruncation());
            m_editNormalTrunat.SetWindowText(strEdit);
        }
        break;
    case WEIBULL:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_WEIBULL))->SetCheck(TRUE);
            CString strNewEntryName;
            pNewProb->screenPrint(strNewEntryName.GetBuffer(256));
            strNewEntryName.ReleaseBuffer();
            m_comboWeibull.SetWindowText(strNewEntryName);
            OnBnClickedRadioWeibull();

            CString strEdit;
            strEdit.Format("%d", ((WeibullDistribution*)pNewProb)->getAlpha());
            m_editWeiAlpha.SetWindowText(strEdit);
            strEdit.Format("%d", ((WeibullDistribution*)pNewProb)->getGamma());
            m_editWeiGamma.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((WeibullDistribution*)pNewProb)->getMu());
            m_editWeiMu.SetWindowText(strEdit);
        }
        break;
    case BERNOULLI:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_BERNOULLI))->SetCheck(TRUE);
            CString strNewEntryName;
            pNewProb->screenPrint(strNewEntryName.GetBuffer(256));
            strNewEntryName.ReleaseBuffer();
            m_comboBernoulli.SetWindowText(strNewEntryName);
            OnBnClickedRadioBernoulli();

            CString strEdit;
            strEdit.Format("%d", ((BernoulliDistribution*)pNewProb)->getValue1());
            m_editBer1stValue.SetWindowText(strEdit);
            strEdit.Format("%d", ((BernoulliDistribution*)pNewProb)->getValue2());
            m_editBer2ndValue.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((BernoulliDistribution*)pNewProb)->getProb1());
            m_editBer1stPro.SetWindowText(strEdit);
        }
        break;
    case EMPIRICAL:
        break;
    case HISTOGRAM:
        break;
    default:
        break;
    }
}

void CDestributionParameterSpecificationDlg::InitUIFromProbEntry(const CProbDistEntry* pEntry)
{
    const ProbabilityDistribution* pInProb = pEntry->m_pProbDist;
    CString strEntryName = pEntry->m_csName;
    if(pInProb == NULL)
        return;
    switch(pInProb->getProbabilityType())
    {
    case CONSTANT:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_CONSTANT))->SetCheck(TRUE);
            m_comboConst.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format(_T("%.2f"), ((ConstantDistribution*)pInProb)->getConstant());
            m_editConstValue.SetWindowText(strEdit);
        }
        break;
    case UNIFORM:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_UNIFORM))->SetCheck(TRUE);
            m_comboUniform.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format("%.2f", ((UniformDistribution*)pInProb)->getMin());
            m_editUniformMin.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((UniformDistribution*)pInProb)->getMax());
            m_editUniformMax.SetWindowText(strEdit);
        }
        break;
    case BETA:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_BETA))->SetCheck(TRUE);
            m_comboBeta.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format("%d", ((BetaDistribution*)pInProb)->getAlpha());
            m_editBetaAlpha.SetWindowText(strEdit);
            strEdit.Format("%d", ((BetaDistribution*)pInProb)->getBeta());
            m_editBetaBeta.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((BetaDistribution*)pInProb)->getMax());
            m_editBetaMax.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((BetaDistribution*)pInProb)->getMin());
            m_editBetaMin.SetWindowText(strEdit);
        }
        break;
    case TRIANGLE:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_TRIANGLE))->SetCheck(TRUE);
            m_comboTriangle.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format("%.2f", ((TriangleDistribution*)pInProb)->getMax());
            m_editTriangleMax.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((TriangleDistribution*)pInProb)->getMin());
            m_editTriangleMin.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((TriangleDistribution*)pInProb)->getMode());
            m_editTriangleMode.SetWindowText(strEdit);
        }
        break;
    case ERLANG:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_ERLANG))->SetCheck(TRUE);
            m_comboErlang.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format("%d", ((ErlangDistribution*)pInProb)->getGamma());
            m_editErlangGamma.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((ErlangDistribution*)pInProb)->getBeta());
            m_editErlangBeta.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((ErlangDistribution*)pInProb)->getMu());
            m_editErlangMu.SetWindowText(strEdit);
        }
        break;
    case EXPONENTIAL:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_EXPONENTIAL))->SetCheck(TRUE);
            m_comboExponential.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format("%.2f", ((ExponentialDistribution*)pInProb)->getLambda());
            m_editExpoLambda.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((ExponentialDistribution*)pInProb)->getMean());
            m_editExpoMean.SetWindowText(strEdit);
        }
        break;
    case GAMMA:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_GAMMA))->SetCheck(TRUE);
            m_comboGamma.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format("%d", ((GammaDistribution*)pInProb)->getGamma());
            m_editGaGamma.SetWindowText(strEdit);
            strEdit.Format("%d", ((GammaDistribution*)pInProb)->getBeta());
            m_editGammaBeta.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((GammaDistribution*)pInProb)->getMu());
            m_editGammaMu.SetWindowText(strEdit);
        }
        break;
    case NORMAL:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_NORMAL))->SetCheck(TRUE);
            m_comboNormal.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format("%d", ((NormalDistribution*)pInProb)->getMean());
            m_editNormalMean.SetWindowText(strEdit);
            strEdit.Format("%d", ((NormalDistribution*)pInProb)->getStdDev());
            m_editNormalStd.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((NormalDistribution*)pInProb)->getTruncation());
            m_editNormalTrunat.SetWindowText(strEdit);
        }
        break;
    case WEIBULL:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_WEIBULL))->SetCheck(TRUE);
            m_comboWeibull.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format("%d", ((WeibullDistribution*)pInProb)->getAlpha());
            m_editWeiAlpha.SetWindowText(strEdit);
            strEdit.Format("%d", ((WeibullDistribution*)pInProb)->getGamma());
            m_editWeiGamma.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((WeibullDistribution*)pInProb)->getMu());
            m_editWeiMu.SetWindowText(strEdit);
        }
        break;
    case BERNOULLI:
        {
            ((CButton*)GetDlgItem(IDC_RADIO_BERNOULLI))->SetCheck(TRUE);
            m_comboBernoulli.SetWindowText(strEntryName);
            CString strEdit;
            strEdit.Format("%d", ((BernoulliDistribution*)pInProb)->getValue1());
            m_editBer1stValue.SetWindowText(strEdit);
            strEdit.Format("%d", ((BernoulliDistribution*)pInProb)->getValue2());
            m_editBer2ndValue.SetWindowText(strEdit);
            strEdit.Format("%.2f", ((BernoulliDistribution*)pInProb)->getProb1());
            m_editBer1stPro.SetWindowText(strEdit);
        }
        break;
    case EMPIRICAL:
        break;
    case HISTOGRAM:
        break;
    default:
        break;
    }
}

