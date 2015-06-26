#include "stdafx.h"
#include "TermPlan.h"
#include "DestributionParameterSpecificationDlg.h"
#include "TermPlanDoc.h"

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
END_MESSAGE_MAP()

BOOL CDestributionParameterSpecificationDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetAllSpinControlRange();
    DisableAllEditBox();

    CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
    m_pProbMan = pDoc->GetTerminal().m_pAirportDB->getProbDistMan();
    return TRUE;
}

void CDestributionParameterSpecificationDlg::SetAllSpinControlRange()
{
    short iMin=0, iMax=1000;
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
}


void CDestributionParameterSpecificationDlg::OnBnClickedBtnResetinput()
{
}



void CDestributionParameterSpecificationDlg::OnBnClickedRadioConstant()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_CONSTANT)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_CONSTANTVALUE)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_CONSTVALUE)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(CONSTANT);
    AddProbNamesToCombo(&m_comboConst, vProb);
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
    AddProbNamesToCombo(&m_comboUniform, vProb);
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
    AddProbNamesToCombo(&m_comboBeta, vProb);
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
    AddProbNamesToCombo(&m_comboTriangle, vProb);
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
    AddProbNamesToCombo(&m_comboErlang, vProb);
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioExponential()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_EXPONENTIAL)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_EXPOLAMBDA)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_EXPOLAMBDA)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_EXPOMEAN)->EnableWindow(TRUE);
    GetDlgItem(IDC_SPIN_EXPOMEAN)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(EXPONENTIAL);
    AddProbNamesToCombo(&m_comboExponential, vProb);
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
    AddProbNamesToCombo(&m_comboGamma, vProb);
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
    AddProbNamesToCombo(&m_comboNormal, vProb);
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
    AddProbNamesToCombo(&m_comboWeibull, vProb);
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
    AddProbNamesToCombo(&m_comboBernoulli, vProb);
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioEmpirical()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_EMPIRICAL)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(EMPIRICAL);
    AddProbNamesToCombo(&m_comboEmpirical, vProb);
}


void CDestributionParameterSpecificationDlg::OnBnClickedRadioHistogram()
{
    DisableAllEditBox();
    GetDlgItem(IDC_COMBO_HISTOGRAM)->EnableWindow(TRUE);

    CPROBDISTLIST vProb = m_pProbMan->getItemListByType(HISTOGRAM);
    AddProbNamesToCombo(&m_comboHistogram, vProb);
}

void CDestributionParameterSpecificationDlg::AddProbNamesToCombo(CComboBox* pcb, const CPROBDISTLIST &vProb)
{
    pcb->ResetContent();
    CPROBDISTLIST::const_iterator itor = vProb.begin();
    for(; itor!=vProb.end(); ++itor)
    {
        pcb->AddString((*itor)->m_csName);
    }
}


void CDestributionParameterSpecificationDlg::OnBnClickedOk()
{
    ProbabilityDistribution* pProb = NULL;
    if(IsDlgButtonChecked(IDC_RADIO_CONSTANT))
    {
        CString strName;
        m_comboConst.GetWindowText(strName.GetBuffer(256), 255);
        strName.ReleaseBuffer();
        CProbDistEntry* pEntry = m_pProbMan->getItemByName(strName);
        if(pEntry != NULL)
        {
            ASSERT(pEntry->m_pProbDist->getProbabilityType() == CONSTANT);
//             ConstantDistribution* pProb = pEntry->m_pProbDist;
//             pProb->setConstant()
        }
    }
    else if(IsDlgButtonChecked(IDC_RADIO_UNIFORM))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_BETA))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_TRIANGLE))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_ERLANG))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_EXPONENTIAL))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_GAMMA))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_NORMAL))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_WEIBULL))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_BERNOULLI))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_EMPIRICAL))
    {

    }
    else if(IsDlgButtonChecked(IDC_RADIO_HISTOGRAM))
    {

    }

    CDialog::OnOK();
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboConstant()
{
    CString strSel;
    m_comboConst.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    ConstantDistribution* pProb = (ConstantDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getConstant());
    GetDlgItem(IDC_EDIT_CONSTANTVALUE)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboUniform()
{
    CString strSel;
    m_comboUniform.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    UniformDistribution* pProb = (UniformDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getMin());
    GetDlgItem(IDC_EDIT_UNIFORMMIN)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getMax());
    GetDlgItem(IDC_EDIT_UNIFORMMAX)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboBeta()
{
    CString strSel;
    m_comboUniform.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    BetaDistribution* pProb = (BetaDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getAlpha());
    GetDlgItem(IDC_EDIT_BETAALPHA)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getBeta());
    GetDlgItem(IDC_EDIT_BETABETA)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getMax());
    GetDlgItem(IDC_EDIT_BETAMAX)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getMin());
    GetDlgItem(IDC_EDIT_BETAMIN)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboTriangle()
{
    CString strSel;
    m_comboUniform.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    TriangleDistribution* pProb = (TriangleDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getMax());
    GetDlgItem(IDC_EDIT_TRIANGLEMAX)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getMin());
    GetDlgItem(IDC_EDIT_TRIANGLEMIN)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getMode());
    GetDlgItem(IDC_EDIT_TRIANGLEMODE)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboErlang()
{
    CString strSel;
    m_comboUniform.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    ErlangDistribution* pProb = (ErlangDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getGamma());
    GetDlgItem(IDC_EDIT_ERLANGGAMMA)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getBeta());
    GetDlgItem(IDC_EDIT_ERLANGBETA)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getMu());
    GetDlgItem(IDC_EDIT_ERLANGMU)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboExponential()
{
    CString strSel;
    m_comboUniform.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    ExponentialDistribution* pProb = (ExponentialDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getLambda());
    GetDlgItem(IDC_EDIT_EXPOLAMBDA)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getMean());
    GetDlgItem(IDC_EDIT_EXPOMEAN)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboGamma()
{
    CString strSel;
    m_comboUniform.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    GammaDistribution* pProb = (GammaDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getGamma());
    GetDlgItem(IDC_EDIT_GAGAMMA)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getBeta());
    GetDlgItem(IDC_EDIT_GAMMABETA)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getMu());
    GetDlgItem(IDC_EDIT_GAMMAMU)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboNormal()
{
    CString strSel;
    m_comboUniform.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    NormalDistribution* pProb = (NormalDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getMean());
    GetDlgItem(IDC_EDIT_NORMALMEAN)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getStdDev());
    GetDlgItem(IDC_EDIT_NORMALSTD)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getTruncation());
    GetDlgItem(IDC_EDIT_NORMALTRUNAT)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboWeibull()
{
    CString strSel;
    m_comboUniform.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    WeibullDistribution* pProb = (WeibullDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getAlpha());
    GetDlgItem(IDC_EDIT_WEIALPHA)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getGamma());
    GetDlgItem(IDC_EDIT_WEIGAMMA)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getMu());
    GetDlgItem(IDC_EDIT_WEIMU)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboBernoulli()
{

    CString strSel;
    m_comboUniform.GetWindowText(strSel.GetBuffer(256), 255);
    strSel.ReleaseBuffer();

    CProbDistEntry* pEntry = m_pProbMan->getItemByName(strSel);
    BernoulliDistribution* pProb = (BernoulliDistribution*)pEntry->m_pProbDist;
    ASSERT(pProb != NULL);
    CString strTemp;
    strTemp.Format(_T("%d"), pProb->getValue1());
    GetDlgItem(IDC_EDIT_BER1STVALUE)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getValue2());
    GetDlgItem(IDC_EDIT_BER2NDVALUE)->SetWindowText(strTemp);
    strTemp.Format(_T("%d"), pProb->getProb1());
    GetDlgItem(IDC_EDIT_BER1STPRO)->SetWindowText(strTemp);
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboEmpirical()
{
}


void CDestributionParameterSpecificationDlg::OnCbnSelchangeComboHistogram()
{
}
