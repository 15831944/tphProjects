#include "stdafx.h"
#include "TermPlan.h"
#include "DestributionParameterSpecificationDlg.h"
#include "DlgProbDist.h"

static const short iMin=-32767, iMax=32767;

IMPLEMENT_DYNAMIC(CDestributionParameterSpecificationDlg, CDialog)

    CDestributionParameterSpecificationDlg::CDestributionParameterSpecificationDlg(CWnd* pParent)
    : CDialog(CDestributionParameterSpecificationDlg::IDD, pParent),
     m_pProbMan(NULL)
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
    ON_EN_CHANGE(IDC_EDIT_CONSTANTVALUE, &CDestributionParameterSpecificationDlg::OnEnChangeEditConstantvalue)
END_MESSAGE_MAP()

BOOL CDestributionParameterSpecificationDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetAllSpinControlRange();
    DisableAllEditBox();SetAllComboboxDropWidth();

    m_pProbMan = GetTermPlanDoc()->GetTerminal().m_pAirportDB->getProbDistMan();
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
    m_comboConst.SetCurSel(-1);
    m_comboUniform.SetCurSel(-1);
    m_comboBeta.SetCurSel(-1);
    m_comboTriangle.SetCurSel(-1);
    m_comboErlang.SetCurSel(-1);
    m_comboExponential.SetCurSel(-1);
    m_comboGamma.SetCurSel(-1);
    m_comboNormal.SetCurSel(-1);
    m_comboWeibull.SetCurSel(-1);
    m_comboBernoulli.SetCurSel(-1);
    m_comboEmpirical.SetCurSel(-1);
    m_comboHistogram.SetCurSel(-1);

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

    DisableAllEditBox();
    if(GetCheckedDistribution() != -1)
        ((CButton*)GetDlgItem(GetCheckedDistribution()))->SetCheck(FALSE);
}



void CDestributionParameterSpecificationDlg::OnBnClickedRadioConstant()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_CONSTANT)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_CONSTANTVALUE)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_CONSTVALUE)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(CONSTANT);
    LoadComboBoxString(&m_comboConst, vProb);
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

bool CDestributionParameterSpecificationDlg::IsFloatString(CString str)
{
    int strLen = str.GetLength();
    str.Trim();
    if(str.IsEmpty())
        return false;

    bool dotIsFound = false;
    for(int i=0; i<strLen; i++)
    {
        char c = str[i];
        if(c == '.')
        {
            if(dotIsFound)
                return false;
            else
                dotIsFound = true;
        }
        else if(c == '-')
        {
            if(i != 0)
                return false;
        }
        else if(c < '0' || '9' < c)
        {
            return false;
        }
    }
    return true;
}

bool CDestributionParameterSpecificationDlg::GetFloatFromEditbox(CEdit* pEdit, float& fConstValue)
{
    CString strValue;
    pEdit->GetWindowText(strValue);
    if(!IsFloatString(strValue))
    {
        MessageBox(_T("Invalid input value."));
        pEdit->SetFocus();
        pEdit->SetSel(0, -1);
        return false;
    }
    else
    {
        fConstValue = (float)atof(strValue);
        return true;
    }
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
                ASSERT(pEntry->m_pProbDist->getProbabilityType() == ERLANG);
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
    if(!IsFloatString(strValue))
    {
        MessageBox(_T("Invalid input value."));
        pEdit->SetFocus();
        pEdit->SetSel(0, -1);
        return;
    }
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
    SpinChangeEditboxValue(pEdit, pNMUpDown);
    *pResult = 0;
}
void CDestributionParameterSpecificationDlg::OnDeltaposSpinBetabeta(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BETABETA);
    SpinChangeEditboxValue(pEdit, pNMUpDown);
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

LRESULT CDestributionParameterSpecificationDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case:
        {

        }
        break;
    default:
        break;
    }
    return CDialog::DefWindowProc(message, wParam, lParam);
}



void CDestributionParameterSpecificationDlg::OnEnChangeEditConstantvalue()
{
    
}
