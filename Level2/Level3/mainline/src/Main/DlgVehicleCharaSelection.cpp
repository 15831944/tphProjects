#include "StdAfx.h"
#include "TermPlan.h"
#include "DlgVehicleCharaSelection.h"
#include "../Landside/LandsideVehicleTypeList.h"

IMPLEMENT_DYNAMIC(CDlgVehicleCharaSelection, CDialog)
CDlgVehicleCharaSelection::CDlgVehicleCharaSelection(LandsideVehicleTypeList* pTypeList,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVehicleCharaSelection::IDD, pParent)
	,m_pVehicleTypeList(pTypeList)
{
}


CDlgVehicleCharaSelection::~CDlgVehicleCharaSelection(void)
{
}

void CDlgVehicleCharaSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMLEVEL1,  m_allLevelCombox[0]);
	DDX_Control(pDX, IDC_COMLEVEL2,  m_allLevelCombox[1]);
	DDX_Control(pDX, IDC_COMLEVEL3,  m_allLevelCombox[2]);
	DDX_Control(pDX, IDC_COMLEVEL4,  m_allLevelCombox[3]);
	DDX_Control(pDX, IDC_COMLEVEL5,  m_allLevelCombox[4]);
	DDX_Control(pDX, IDC_COMLEVEL6,  m_allLevelCombox[5]);
	DDX_Control(pDX, IDC_COMLEVEL7,  m_allLevelCombox[6]);
	DDX_Control(pDX, IDC_COMLEVEL8,  m_allLevelCombox[7]);
	DDX_Control(pDX, IDC_COMLEVEL9,  m_allLevelCombox[8]);
	DDX_Control(pDX, IDC_COMLEVEL10, m_allLevelCombox[9]);
	DDX_Control(pDX, IDC_COMLEVEL11, m_allLevelCombox[10]);
	DDX_Control(pDX, IDC_COMLEVEL12, m_allLevelCombox[11]);
	DDX_Control(pDX, IDC_COMLEVEL13, m_allLevelCombox[12]);
	DDX_Control(pDX, IDC_COMLEVEL14, m_allLevelCombox[13]);
	DDX_Control(pDX, IDC_COMLEVEL15, m_allLevelCombox[14]);
	DDX_Control(pDX, IDC_COMLEVEL16, m_allLevelCombox[15]);
}


BEGIN_MESSAGE_MAP(CDlgVehicleCharaSelection, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CDlgVehicleCharaSelection::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_allLevelCombox[0].AddString(_T("ALL"));
	m_allLevelCombox[0].SelectString(-1, _T("ALL"));
	for (int i=1;i<MULTI_VEHICLE_CHARA_COUNT;i++)
	{
		m_allLevelCombox[i].AddString(_T("ALL"));
		m_allLevelCombox[i].SelectString(-1, _T("ALL"));
		m_allLevelCombox[i].EnableWindow(FALSE);
	}

	InsertVehicleType();
	return TRUE;
}

void CDlgVehicleCharaSelection::OnBnClickedOk()
{
	CString strChara;
	m_vSelChara.clear();

	for (int i=0;i<MULTI_VEHICLE_CHARA_COUNT;i++)
	{
		int nIndex = m_allLevelCombox[i].GetCurSel();
		m_allLevelCombox[i].GetLBText(nIndex,strChara);
		for(size_t j = 0; j<m_TypeStrings.size(); j++)
		{
			if(strChara == m_TypeStrings[j].str )
			{
				m_vSelChara.push_back(j);
				break;
			}
		}
	}


	OnOK();
}

void CDlgVehicleCharaSelection::InsertVehicleType()
{
	VehicleCharaString  mhItem;
	m_TypeStrings.clear();

	int nCount = m_pVehicleTypeList->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		LandsideVehicleType* pLandsideVehicleType = m_pVehicleTypeList->GetItem(i);
		int nBranch = pLandsideVehicleType->GetLeafCount();
		if (nBranch == 0)
			break;

		m_allLevelCombox[i].EnableWindow(TRUE);
		for (int j = 0; j < nBranch; j++)
		{
			m_allLevelCombox[i].AddString(pLandsideVehicleType->GetLeaf(j));
			mhItem.str = pLandsideVehicleType->GetLeaf(j);
			mhItem.nLevel = i;
			mhItem.nIdx = j;
			m_TypeStrings.push_back(mhItem);
		}
	}
}
