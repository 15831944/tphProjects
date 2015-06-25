// CtrlInterventionSpec.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "CtrlInterventionSpecDlg.h"
#include "InputAirside/CtrlInterventionSpecData.h"
#include "InputAirside//CtrlInterventionSpecDataList.h"
#include ".\ctrlinterventionspecdlg.h"
#include "DlgSelectRunway.h"
#include "InputAirside/ALTAirport.h"
#include "InputAirside//InputAirside.h"
#include "InputAirside/Runway.h"
#include "../Database/DBElementCollection_Impl.h"


namespace
{
	const UINT ID_NEW_RUNWAY  = 10;
	const UINT ID_DEL_RUNWAY  = 11;
}

// CCtrlInterventionSpecDlg dialog

IMPLEMENT_DYNAMIC(CCtrlInterventionSpecDlg, CDialog)
CCtrlInterventionSpecDlg::CCtrlInterventionSpecDlg(int nProjID, CWnd* pParent /*=NULL*/)
	: CDialog(CCtrlInterventionSpecDlg::IDD, pParent)
	, m_nProjID(nProjID)
	, m_pCurCtrlInterventionSpecData(NULL)
{
	m_pCtrlInterventionSpecDataList = new CCtrlInterventionSpecDataList();
	m_pCtrlInterventionSpecDataList->ReadData(m_nProjID);

	DeleteDataWhichRunwayIsNotExist();
	//maybe the data is modify,so save it
	SaveData();
}

CCtrlInterventionSpecDlg::~CCtrlInterventionSpecDlg()
{
	delete m_pCtrlInterventionSpecDataList;
}

void CCtrlInterventionSpecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_StaticToolBar);
	DDX_Control(pDX, IDC_LIST_RUNWAY, m_RunwayListBox);
	DDX_Control(pDX, IDC_COMBO_PRIORITYONE, m_PriorityOneComboBox);
	DDX_Control(pDX, IDC_COMBO_PRIORITYTWO, m_PriorityTwoComboBox);
	DDX_Control(pDX, IDC_COMBO__PRIORITYTHREE, m_PriorityThreeComboBox);
}


BEGIN_MESSAGE_MAP(CCtrlInterventionSpecDlg, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_COMMAND(ID_NEW_RUNWAY, OnCmdNewRunway)
	ON_COMMAND(ID_DEL_RUNWAY, OnCmdDeleteRunway)
	ON_LBN_SELCHANGE(IDC_LIST_RUNWAY, OnLbnSelchangeListRunway)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITYONE, OnCbnSelchangeComboPriorityone)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITYTWO, OnCbnSelchangeComboPrioritytwo)
	ON_CBN_SELCHANGE(IDC_COMBO__PRIORITYTHREE, OnCbnSelchangeCombo)

END_MESSAGE_MAP()


// CCtrlInterventionSpecDlg message handlers
int CCtrlInterventionSpecDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_RunwayToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_RunwayToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_RunwayToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_RUNWAY);
	toolbar.SetCmdID(1, ID_DEL_RUNWAY);

	return 0;
}

BOOL CCtrlInterventionSpecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitToolBar();	
	InitComboBox();
	InitListBox();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	return TRUE;
}

void CCtrlInterventionSpecDlg::DeleteDataWhichRunwayIsNotExist()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);

	int nCount = m_pCtrlInterventionSpecDataList->GetElementCount();

	for (int i=0; i<nCount; i++)
	{
		CCtrlInterventionSpecData *pCtrlInterventionSpecData = m_pCtrlInterventionSpecDataList->GetItem(i);
		int nRunwayID = pCtrlInterventionSpecData->GetRunwayID();

		BOOL bIsExist = FALSE;

		for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); iterAirportID++)
		{
			ALTAirport airport;
			airport.ReadAirport(*iterAirportID);

			std::vector<int> vRunways;
			ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);
			std::vector<int>::iterator iterRunwayID = vRunways.begin();
			for (; iterRunwayID != vRunways.end(); iterRunwayID++)
			{
				if (nRunwayID == *iterRunwayID)
				{
					bIsExist = TRUE;
				}				
			}
		}	

		//not exist
		if (!bIsExist)
		{
			m_pCtrlInterventionSpecDataList->DeleteItem(i);

			i--;			
			nCount--;
		}
	}	
}

void CCtrlInterventionSpecDlg::InitToolBar(void)
{
	CRect rect;

	m_StaticToolBar.GetWindowRect( &rect );
	ScreenToClient( &rect );

	m_RunwayToolBar.MoveWindow( &rect );
	m_RunwayToolBar.ShowWindow( SW_SHOW );
	m_StaticToolBar.ShowWindow( SW_HIDE );

	m_RunwayToolBar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY, TRUE);
	m_RunwayToolBar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, FALSE);
}

void CCtrlInterventionSpecDlg::InitListBox(void)
{
	int nCount = m_pCtrlInterventionSpecDataList->GetElementCount();

	for (int i=0; i<nCount; i++)
	{
		CCtrlInterventionSpecData *pCISpecData = m_pCtrlInterventionSpecDataList->GetItem(i);

		int nRunwayID = pCISpecData->GetRunwayID();
		int nMarkingIndex = pCISpecData->GetMarkingIndex();

		Runway altRunway;
		altRunway.ReadObject(nRunwayID);

		CString strMarking;

		//marking 1
		if (nMarkingIndex == 0)
		{
			strMarking.Format("%s", altRunway.GetMarking1().c_str());
		}
		//marking2
		else
		{
			strMarking.Format("%s", altRunway.GetMarking2().c_str());
		}

		CString strRunwayInfo;
		strRunwayInfo.Format("Runway:%s", strMarking);

		int itemIndex = m_RunwayListBox.InsertString(m_RunwayListBox.GetCount(), strRunwayInfo);
		m_RunwayListBox.SetItemData(itemIndex, (DWORD_PTR)pCISpecData);
	}

	if (0 < m_RunwayListBox.GetCount())
	{
		m_RunwayListBox.SetCurSel(0);
		m_pCurCtrlInterventionSpecData = (CCtrlInterventionSpecData*)m_RunwayListBox.GetItemData(0);
		m_RunwayToolBar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, TRUE);
	}

	SetComboBoxContent(m_pCurCtrlInterventionSpecData);
}

void CCtrlInterventionSpecDlg::InitComboBox(void)
{
	int nIndex;
	CString strText;

	//priority one
	strText.LoadString(IDS_SPEEDCTRL);
	nIndex = m_PriorityOneComboBox.AddString(strText);
	m_PriorityOneComboBox.SetItemData(nIndex, (DWORD_PTR)IM_SPEEDCTRL);

	strText.LoadString(IDS_SIDESTEP);
	nIndex = m_PriorityOneComboBox.AddString(strText);
	m_PriorityOneComboBox.SetItemData(nIndex, (DWORD_PTR)IM_SIDESTEP);

	strText.LoadString(IDS_GOAROUND);
	nIndex = m_PriorityOneComboBox.AddString(strText);
	m_PriorityOneComboBox.SetItemData(nIndex, (DWORD_PTR)IM_GOAROUND);

	//priority two
	strText.LoadString(IDS_SPEEDCTRL);
	nIndex = m_PriorityTwoComboBox.AddString(strText);
	m_PriorityTwoComboBox.SetItemData(nIndex, (DWORD_PTR)IM_SPEEDCTRL);

	strText.LoadString(IDS_SIDESTEP);
	nIndex = m_PriorityTwoComboBox.AddString(strText);
	m_PriorityTwoComboBox.SetItemData(nIndex, (DWORD_PTR)IM_SIDESTEP);

	strText.LoadString(IDS_GOAROUND);
	nIndex = m_PriorityTwoComboBox.AddString(strText);
	m_PriorityTwoComboBox.SetItemData(nIndex, (DWORD_PTR)IM_GOAROUND);

	//priority three
	strText.LoadString(IDS_SPEEDCTRL);
	nIndex = m_PriorityThreeComboBox.AddString(strText);
	m_PriorityThreeComboBox.SetItemData(nIndex, (DWORD_PTR)IM_SPEEDCTRL);

	strText.LoadString(IDS_SIDESTEP);
	nIndex = m_PriorityThreeComboBox.AddString(strText);
	m_PriorityThreeComboBox.SetItemData(nIndex, (DWORD_PTR)IM_SIDESTEP);

	strText.LoadString(IDS_GOAROUND);
	nIndex = m_PriorityThreeComboBox.AddString(strText);
	m_PriorityThreeComboBox.SetItemData(nIndex, (DWORD_PTR)IM_GOAROUND);
}

void CCtrlInterventionSpecDlg::SetComboBoxContent(CCtrlInterventionSpecData *pCtrlInterventionSpecData)
{
	if (NULL == pCtrlInterventionSpecData)
	{
		m_PriorityOneComboBox.EnableWindow(FALSE);
		m_PriorityTwoComboBox.EnableWindow(FALSE);
		m_PriorityThreeComboBox.EnableWindow(FALSE);

		return;
	}

	m_PriorityOneComboBox.EnableWindow(TRUE);
	m_PriorityTwoComboBox.EnableWindow(TRUE);
	m_PriorityThreeComboBox.EnableWindow(TRUE);

	int nIndex = 0;

	//priority one
	nIndex = (int)pCtrlInterventionSpecData->GetPriorityOne();
	for (int i=0; i<m_PriorityOneComboBox.GetCount(); i++)
	{
		int nData = (int)m_PriorityOneComboBox.GetItemData(i);

		if (nData == nIndex)
		{
			m_PriorityOneComboBox.SetCurSel(i);
			break;
		}
	}

	//priority two
	nIndex = (int)pCtrlInterventionSpecData->GetPriorityTwo();
	for (int ii=0; ii<m_PriorityTwoComboBox.GetCount(); ii++)
	{
		int nData = (int)m_PriorityTwoComboBox.GetItemData(ii);

		if (nData == nIndex)
		{
			m_PriorityTwoComboBox.SetCurSel(ii);
			break;
		}
	}

	//priority three
	nIndex = (int)pCtrlInterventionSpecData->GetPriorityThree();
	for (int ii=0; ii<m_PriorityThreeComboBox.GetCount(); ii++)
	{
		int nData = (int)m_PriorityThreeComboBox.GetItemData(ii);

		if (nData == nIndex)
		{
			m_PriorityThreeComboBox.SetCurSel(ii);
			break;
		}
	}

	UpdateData(FALSE);
}

void CCtrlInterventionSpecDlg::SaveData(void)
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pCtrlInterventionSpecDataList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}
}

void CCtrlInterventionSpecDlg::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	SaveData();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CCtrlInterventionSpecDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveData();
	OnOK();
}

void CCtrlInterventionSpecDlg::OnCmdNewRunway()
{
	CDlgSelectRunway dlg(m_nProjID);
	if(IDOK != dlg.DoModal())
		return;

	int nRunwayID = dlg.GetSelectedRunway();

	Runway altRunway;
	altRunway.ReadObject(nRunwayID);	

	int nMarkingIndex;
	CString strMarking;
	//marking 2
	if (dlg.GetSelectedRunwayMarkingIndex())
	{
		nMarkingIndex = 1;
		strMarking = altRunway.GetMarking2().c_str();
	}
	//marking 1
	else
	{
		nMarkingIndex = 0;
		strMarking = altRunway.GetMarking1().c_str();
	}

	for (int i=0; i<m_RunwayListBox.GetCount(); i++)
	{
		CCtrlInterventionSpecData *pCtrlInterventionSpecData = (CCtrlInterventionSpecData*)m_RunwayListBox.GetItemData(i);

		if (nRunwayID == pCtrlInterventionSpecData->GetRunwayID()
			&& nMarkingIndex == pCtrlInterventionSpecData->GetMarkingIndex())
		{
			MessageBox(_T("This runway is already exist!"));

			return;
		}
	}

	CString strRunwayInfo;
	strRunwayInfo.Format("Runway:%s", strMarking);
	int itemIndex = m_RunwayListBox.InsertString(m_RunwayListBox.GetCount(), strRunwayInfo);
	m_RunwayListBox.SetCurSel(itemIndex);

	m_pCurCtrlInterventionSpecData = new CCtrlInterventionSpecData();
	m_pCurCtrlInterventionSpecData->SetRunwayID(nRunwayID);
	m_pCurCtrlInterventionSpecData->SetMarkingIndex(nMarkingIndex);
	m_pCurCtrlInterventionSpecData->SetPriorityOne(IM_SPEEDCTRL);
	m_pCurCtrlInterventionSpecData->SetPriorityTwo(IM_SIDESTEP);
	m_pCurCtrlInterventionSpecData->SetPriorityThree(IM_GOAROUND);

	m_pCtrlInterventionSpecDataList->AddNewItem(m_pCurCtrlInterventionSpecData);
	m_RunwayListBox.SetItemData(itemIndex, (DWORD_PTR)m_pCurCtrlInterventionSpecData);

    SetComboBoxContent(m_pCurCtrlInterventionSpecData);
	m_RunwayToolBar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, TRUE);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CCtrlInterventionSpecDlg::OnCmdDeleteRunway()
{
	ASSERT(m_pCurCtrlInterventionSpecData);

	m_RunwayListBox.DeleteString(m_RunwayListBox.GetCurSel());

	m_pCtrlInterventionSpecDataList->DeleteItem(m_pCurCtrlInterventionSpecData);

	int nCount = m_RunwayListBox.GetCount();

	if (0 == nCount)
	{
		m_pCurCtrlInterventionSpecData = NULL;
		m_RunwayToolBar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, FALSE);
	}
	else
	{
		m_RunwayListBox.SetCurSel(nCount-1);
		m_pCurCtrlInterventionSpecData = (CCtrlInterventionSpecData *)m_RunwayListBox.GetItemData(nCount-1);
	}

	SetComboBoxContent(m_pCurCtrlInterventionSpecData);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}
void CCtrlInterventionSpecDlg::OnLbnSelchangeListRunway()
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_RunwayListBox.GetCurSel();

	if (LB_ERR == nCurSel)
	{
		m_pCurCtrlInterventionSpecData = NULL;
		SetComboBoxContent(NULL);
		m_RunwayToolBar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, FALSE);
	}
	else
	{
		m_pCurCtrlInterventionSpecData = (CCtrlInterventionSpecData *)m_RunwayListBox.GetItemData(nCurSel);
		SetComboBoxContent(m_pCurCtrlInterventionSpecData);
		m_RunwayToolBar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, TRUE);
	}
}

void CCtrlInterventionSpecDlg::OnCbnSelchangeComboPriorityone()
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_PriorityOneComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodOne = (INTERVENTIONMETHOD)m_PriorityOneComboBox.GetItemData(nCurSel);

	nCurSel = m_PriorityTwoComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodTwo = (INTERVENTIONMETHOD)m_PriorityTwoComboBox.GetItemData(nCurSel);

	nCurSel = m_PriorityThreeComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodThree = (INTERVENTIONMETHOD)m_PriorityThreeComboBox.GetItemData(nCurSel);

	if (iMethodTwo == iMethodOne)
	{
		SetDiffMethod(m_PriorityTwoComboBox, iMethodOne, iMethodThree);

	}
	else
	{
		SetDiffMethod(m_PriorityThreeComboBox, iMethodOne, iMethodTwo);
	}

	UpdateInterventionMethod();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CCtrlInterventionSpecDlg::OnCbnSelchangeComboPrioritytwo()
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_PriorityOneComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodOne = (INTERVENTIONMETHOD)m_PriorityOneComboBox.GetItemData(nCurSel);

	nCurSel = m_PriorityTwoComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodTwo = (INTERVENTIONMETHOD)m_PriorityTwoComboBox.GetItemData(nCurSel);

	nCurSel = m_PriorityThreeComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodThree = (INTERVENTIONMETHOD)m_PriorityThreeComboBox.GetItemData(nCurSel);

	if (iMethodTwo == iMethodOne)
	{
		SetDiffMethod(m_PriorityOneComboBox, iMethodTwo, iMethodThree);

	}
	else
	{
		SetDiffMethod(m_PriorityThreeComboBox, iMethodOne, iMethodTwo);
	}

	UpdateInterventionMethod();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CCtrlInterventionSpecDlg::OnCbnSelchangeCombo()
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_PriorityOneComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodOne = (INTERVENTIONMETHOD)m_PriorityOneComboBox.GetItemData(nCurSel);

	nCurSel = m_PriorityTwoComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodTwo = (INTERVENTIONMETHOD)m_PriorityTwoComboBox.GetItemData(nCurSel);

	nCurSel = m_PriorityThreeComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodThree = (INTERVENTIONMETHOD)m_PriorityThreeComboBox.GetItemData(nCurSel);

	if (iMethodThree == iMethodOne)
	{
		SetDiffMethod(m_PriorityOneComboBox, iMethodTwo, iMethodThree);

	}
	else
	{
		SetDiffMethod(m_PriorityTwoComboBox, iMethodOne, iMethodThree);
	}

	UpdateInterventionMethod();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CCtrlInterventionSpecDlg::SetDiffMethod(CComboBox &cbxPriority, INTERVENTIONMETHOD iMethodOne, INTERVENTIONMETHOD iMethodTwo)
{
	INTERVENTIONMETHOD iChangeMethod;

	if (iMethodOne!=IM_SPEEDCTRL
		&& iMethodTwo!=IM_SPEEDCTRL)
	{
		iChangeMethod = IM_SPEEDCTRL;
	}
	else if (iMethodOne!=IM_SIDESTEP
		&& iMethodTwo!=IM_SIDESTEP)
	{
		iChangeMethod = IM_SIDESTEP;
	}
	else
	{
		iChangeMethod = IM_GOAROUND;
	}

	for (int i=0; i<cbxPriority.GetCount(); i++)
	{
		INTERVENTIONMETHOD iMethod = (INTERVENTIONMETHOD)cbxPriority.GetItemData(i);

		if (iMethod == iChangeMethod)
		{
			cbxPriority.SetCurSel(i);

			break;
		}
	}
}

void CCtrlInterventionSpecDlg::UpdateInterventionMethod()
{
	ASSERT(m_pCurCtrlInterventionSpecData);

	int nCurSel = m_PriorityOneComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodOne = (INTERVENTIONMETHOD)m_PriorityOneComboBox.GetItemData(nCurSel);
	m_pCurCtrlInterventionSpecData->SetPriorityOne(iMethodOne);

	nCurSel = m_PriorityTwoComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodTwo = (INTERVENTIONMETHOD)m_PriorityTwoComboBox.GetItemData(nCurSel);
	m_pCurCtrlInterventionSpecData->SetPriorityTwo(iMethodTwo);

	nCurSel = m_PriorityThreeComboBox.GetCurSel();
	INTERVENTIONMETHOD iMethodThree = (INTERVENTIONMETHOD)m_PriorityThreeComboBox.GetItemData(nCurSel);
	m_pCurCtrlInterventionSpecData->SetPriorityThree(iMethodThree);
}
