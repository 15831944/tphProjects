// DlgLandsideVehicleGroupProperty.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLandsideVehicleGroupProperty.h"
#include "DlgSelectLandsideVehicleType.h"
#include "Common/WinMsg.h"
#include "Common/UnitPiece.h"
#include "landside/InputLandside.h"
#include "../Inputs/PROBAB.H"
#include "../common/ProbDistManager.h"
#include "DlgProbDist.h"
//#include "../common/ProbDistEntry.h"
// CDlgLandsideVehicleGroupProperty dialog

IMPLEMENT_DYNAMIC(CDlgLandsideVehicleGroupProperty, CXTResizeDialog)
CDlgLandsideVehicleGroupProperty::CDlgLandsideVehicleGroupProperty(InputLandside* pInput,int prjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgLandsideVehicleGroupProperty::IDD, pParent)	
	,m_nProjID(prjID)
	,m_pInputLandside(pInput)
{
	m_landsideVehicleGroupProperties= new LandsideVehicleGroupProperty();
	m_landsideVehicleGroupProperties->ReadData(prjID);
}

CDlgLandsideVehicleGroupProperty::~CDlgLandsideVehicleGroupProperty()
{
	delete m_landsideVehicleGroupProperties;
	
}

void CDlgLandsideVehicleGroupProperty::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgLandsideVehicleGroupProperty, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_ADD,OnCmdNewItem)
	ON_COMMAND(ID_TOOLBAR_DEL,OnCmdDeleteItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedList)
	ON_MESSAGE(WM_COLLUM_INDEX, OnDoubleClick)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListContents)
END_MESSAGE_MAP()

// CDlgLandsideVehicleGroupProperty message handlers

BOOL CDlgLandsideVehicleGroupProperty::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	/////	
	CUnitPiece::InitializeUnitPiece(m_nProjID,UP_ID_1002,this);

	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_wndListCtrl.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse|ARCUnit_Weight_InUse|ARCUnit_Acceleration_InUse);
	/////

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	
	OnInitToolbar();
	InitListControl();
	UpdateToolBarState();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgLandsideVehicleGroupProperty::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolBar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		return -1;
	}

	return 0;
}

void CDlgLandsideVehicleGroupProperty::UpdateToolBarState()
{
	if (m_wndListCtrl.GetSelectedCount())
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
}

void CDlgLandsideVehicleGroupProperty::InitListControl()
{
	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);
// 	CStringList strList;
// 	CProbDistManager* pProbDistMan = 0;	
// 	CProbDistEntry* pPDEntry = 0;
// 	int nCount = -1;
// 	if(m_pInputLandside)
// 		pProbDistMan = m_pInputLandside->GetAirPortDataBase()->getProbDistMan();
// 	strList.AddTail(_T("New Operating Distribution..."));
// 	if(pProbDistMan)
// 		nCount = pProbDistMan->getCount();
// 	for( int i=0; i< nCount; i++ )
// 	{
// 		pPDEntry = pProbDistMan->getItem( i );
// 		if(pPDEntry)
// 			strList.AddTail(pPDEntry->m_csName);
// 	}
	//Group name
	LV_COLUMNEX	lvc;
	CStringList strList;
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Group name");
	lvc.cx = 140;
	m_wndListCtrl.InsertColumn(0,&lvc);

	//Length
	CString strLenght;
	strLenght.Format(_T("L(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strLenght.GetBuffer();
	strLenght.ReleaseBuffer();
	lvc.cx = 50;
	m_wndListCtrl.InsertColumn(1,&lvc);

	//width
	CString strWidth;
	strWidth.Format(_T("Width(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strWidth.GetBuffer();
	strWidth.ReleaseBuffer();
	lvc.cx = 60;
	m_wndListCtrl.InsertColumn(2,&lvc);

	//Height
	CString strHeight;
	strHeight.Format(_T("H(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strHeight.GetBuffer();
	strHeight.ReleaseBuffer();
	lvc.cx = 50;
	m_wndListCtrl.InsertColumn(3,&lvc);

	//Weight
	CString strWeight;
	strWeight.Format(_T("Wt(%s)"),CARCWeightUnit::GetWeightUnitString(CUnitPiece::GetCurSelWeightUnit()));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strWeight.GetBuffer();
	strHeight.ReleaseBuffer();
	lvc.cx = 50;
	m_wndListCtrl.InsertColumn(4,&lvc);

	//Capacity
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Capacity");
	lvc.cx = 60;
	m_wndListCtrl.InsertColumn(5,&lvc);

	//Parallel Park buffer
	CString strParkBuffer;
	strParkBuffer.Format(_T("Parallel Park buffer(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strParkBuffer.GetBuffer();
	strParkBuffer.ReleaseBuffer();
	lvc.cx = 120;
	m_wndListCtrl.InsertColumn(6,&lvc);

	//Acc(m/s^2)
	CString strAcc;
	strAcc.Format(_T("Acc(%s)"),CARCAccelerationUnit::GetAccelerationUnitString(CUnitPiece::GetCurSelAccelerationUnit()));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strAcc.GetBuffer();
	strAcc.ReleaseBuffer();
	lvc.cx = 80;
	m_wndListCtrl.InsertColumn(7,&lvc);

	//D(m/s^2)
	CString strDec;
	strDec.Format(_T("D(%s)"),CARCAccelerationUnit::GetAccelerationUnitString(CUnitPiece::GetCurSelAccelerationUnit()));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strDec.GetBuffer();
	strDec.ReleaseBuffer();
	lvc.cx = 70;
	m_wndListCtrl.InsertColumn(8,&lvc);

	//Range
	//CString strRange;
	//strRange.Format(_T("Range(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Range(km)");
	//strRange.ReleaseBuffer();
	lvc.cx = 70;
	m_wndListCtrl.InsertColumn(9,&lvc);

	//Fuel
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Fuel(l/km)");
	lvc.cx = 70;
	m_wndListCtrl.InsertColumn(10,&lvc);

	//seperation in Q
	CString seperation;
	seperation.Format(_T("seperation in Q(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = seperation.GetBuffer();
	seperation.ReleaseBuffer();
	lvc.cx = 140;
	m_wndListCtrl.InsertColumn(11,&lvc);
	
	//longitudinal deviation
	CString longitudinal;
	longitudinal.Format(_T("longitudinal deviation(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
//	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = longitudinal.GetBuffer();
	longitudinal.ReleaseBuffer();
	lvc.cx = 140;
	m_wndListCtrl.InsertColumn(12,&lvc);

	SetListContent();
}

void CDlgLandsideVehicleGroupProperty::OnCmdNewItem()
{
	CDlgSelectLandsideVehicleType dlg;
	int nCount = m_wndListCtrl.GetItemCount();
	if(dlg.DoModal() == IDOK)
	{
		CString strGroupName = dlg.GetName();
		if (strGroupName.IsEmpty())
		{
			MessageBox(_T("Please define vehicle type name in Vehicle Type Definition Dialog!."),_T("Warning"),MB_OK);
		}
		else if (m_landsideVehicleGroupProperties->ExistSameName(strGroupName))
		{
			MessageBox(_T("Exist the same group name!."),_T("Warning"),MB_OK);
		}
		else
		{
			LandsideVehicleProperty* pVehicleProperty = new LandsideVehicleProperty();
			pVehicleProperty->SetGroupName(strGroupName);
			m_landsideVehicleGroupProperties->AddNewItem(pVehicleProperty);
			m_wndListCtrl.InsertItem(nCount,strGroupName);
			//length
			double dLength = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetLenght());
			CString strLength;
			strLength.Format(_T("%.2f"),dLength);
			m_wndListCtrl.SetItemText(nCount,1,strLength);

			//width
			double dWidth = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetWidth());
			CString strWidth;
			strWidth.Format(_T("%.2f"),dWidth);
			m_wndListCtrl.SetItemText(nCount,2,strWidth);

			//height
			double dHeigth = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetHeight());
			CString strHeigth;
			strHeigth.Format(_T("%.2f"),dHeigth);
			m_wndListCtrl.SetItemText(nCount,3,strHeigth);

			//weight
			double dWeight = CARCWeightUnit::ConvertWeight(DEFAULT_DATABASE_WEIGHT_UNIT,CUnitPiece::GetCurSelWeightUnit(),pVehicleProperty->GetWeigth());
			CString strWeight;
			strWeight.Format(_T("%.2f"),dWeight);
			m_wndListCtrl.SetItemText(nCount,4,strWeight);

			//Capacity
			CString strCapacity;
			strCapacity.Format(_T("%d"),pVehicleProperty->GetCapacity());
			m_wndListCtrl.SetItemText(nCount,5,strCapacity);

			//park buffer
			double dParkBuffer = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetParkBuff());
			CString strParkBuffer;
			strParkBuffer.Format(_T("%.2f"),dParkBuffer);
			m_wndListCtrl.SetItemText(nCount,6,strParkBuffer);

			//acc
			CString strAcc;
			double dAcc = CARCAccelerationUnit::ConvertAcceleration(DEFAULT_DATABASE_ACCELERATION_UNIT,CUnitPiece::GetCurSelAccelerationUnit(),pVehicleProperty->GetAcc());
			strAcc.Format(_T("%.2f"),dAcc);
			m_wndListCtrl.SetItemText(nCount,7,strAcc);

			//dec
			CString strDec;
			double dDec = CARCAccelerationUnit::ConvertAcceleration(DEFAULT_DATABASE_ACCELERATION_UNIT,CUnitPiece::GetCurSelAccelerationUnit(),pVehicleProperty->GetDec());
			strDec.Format(_T("%.2f"),dDec);
			m_wndListCtrl.SetItemText(nCount,8,strDec);

			//range
			//double dRange = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetRange());
			CString strRange;
			strRange.Format(_T("%.2f"),pVehicleProperty->GetRange());
			m_wndListCtrl.SetItemText(nCount,9,strRange);

			//fuel
			CString strFuel;
			strFuel.Format(_T("%.2f"),pVehicleProperty->GetFuel());
			m_wndListCtrl.SetItemText(nCount,10,strFuel);
			
			//seperation in Q
			CString strSeperation;
			double dseperation = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetSeperation());
			strSeperation.Format(_T("%.2f"),dseperation);
			m_wndListCtrl.SetItemText(nCount,11,strSeperation);

			//longitudinal deviation
			char strlongitudinal[1024];
			pVehicleProperty->GetDistrubution()->GetProbDistribution()->screenPrint(strlongitudinal);
			m_wndListCtrl.SetItemText(nCount,12,strlongitudinal);

			m_wndListCtrl.SetItemState(nCount, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_wndListCtrl.SetItemData(nCount,(DWORD)pVehicleProperty);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
}

void CDlgLandsideVehicleGroupProperty::OnCmdDeleteItem()
{
	int nCurSel = m_wndListCtrl.GetCurSel();
	if (nCurSel == 0)
	{
		MessageBox(_T("Can't delete default data"),_T("Warning"),MB_OK);
		return;
	}

	if (nCurSel != LB_ERR)
	{
		LandsideVehicleProperty* pLandsideVehiclePro = (LandsideVehicleProperty*)m_wndListCtrl.GetItemData(nCurSel);
		m_landsideVehicleGroupProperties->DeleteItem(pLandsideVehiclePro);
		m_wndListCtrl.DeleteItem(nCurSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgLandsideVehicleGroupProperty::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		m_wndListCtrl.SetCurSel(pnmv->iItem);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
	else
		ASSERT(0);
}

LRESULT CDlgLandsideVehicleGroupProperty::OnDoubleClick( WPARAM wParam, LPARAM lParam )
{
	int nCollumn = lParam;
	if (nCollumn == 12) 
	{
		CProbDistManager* pProbDistMan = m_pInputLandside->GetAirPortDataBase()->getProbDistMan();
		CProbDistEntry* pPDEntry = 0;
		int nCount;
		LVCOLDROPLIST* pList =  m_wndListCtrl.GetColumnStyle(12);
		pList->List.RemoveAll();
		pList->List.AddTail(_T("New Operating Distribution..."));
		if(pProbDistMan)
			nCount = pProbDistMan->getCount();
		for( int i=0; i< nCount; i++ )
		{
			pPDEntry = pProbDistMan->getItem( i );
			if(pPDEntry)
				pList->List.AddTail(pPDEntry->m_csName.MakeUpper());
		}
	}
	return 0;
}

void CDlgLandsideVehicleGroupProperty::OnLvnEndlabeleditListContents( NMHDR *pNMHDR, LRESULT *pResult )
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	int nCurSel = pDispInfo->item.iItem;
	CString strSel;
	strSel = pDispInfo->item.pszText;
	if (nCurSel == LB_ERR)
		return;

	LandsideVehicleProperty* pVehicleProperty = (LandsideVehicleProperty*)m_wndListCtrl.GetItemData(nCurSel);
	CProbDistManager* pProbDistMan = m_pInputLandside->GetAirPortDataBase()->getProbDistMan();
	if (pVehicleProperty == NULL)
		return;
	
	switch (pDispInfo->item.iSubItem)
	{
	case 1:
		{
			double dLength = atof(pDispInfo->item.pszText);
			dLength = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dLength);
			pVehicleProperty->SetLength(dLength);
		}
		break;
	case 2:
		{
			double dWidth = atof(pDispInfo->item.pszText);
			dWidth = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dWidth);
			pVehicleProperty->SetWidth(dWidth);
		}
		break;
	case 3:
		{
			double dHeight = atof(pDispInfo->item.pszText);
			dHeight = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dHeight);
			pVehicleProperty->SetHeight(dHeight);
		}
		break;
	case 4:
		{
			double dWeight = atof(pDispInfo->item.pszText);
			dWeight = CARCWeightUnit::ConvertWeight(CUnitPiece::GetCurSelWeightUnit(),DEFAULT_DATABASE_WEIGHT_UNIT,dWeight);
			pVehicleProperty->SetWeigth(dWeight);
		}
		break;
	case 5:
		{
			int nCapacity = atoi(pDispInfo->item.pszText);
			pVehicleProperty->SetCapacity(nCapacity);
		}
		break;
	case 6:
		{
			double dParkBuffer = atof(pDispInfo->item.pszText);
			dParkBuffer = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dParkBuffer);
			pVehicleProperty->SetParkBuff(dParkBuffer);
		}
		break;
	case 7:
		{
			double dAcc = atof(pDispInfo->item.pszText);
			dAcc = CARCAccelerationUnit::ConvertAcceleration(CUnitPiece::GetCurSelAccelerationUnit(),DEFAULT_DATABASE_ACCELERATION_UNIT,dAcc);
			pVehicleProperty->SetAcc(dAcc);
		}
		break;
	case 8:
		{
			double dDec = atof(pDispInfo->item.pszText);
			dDec = CARCAccelerationUnit::ConvertAcceleration(CUnitPiece::GetCurSelAccelerationUnit(),DEFAULT_DATABASE_ACCELERATION_UNIT,dDec);
			pVehicleProperty->SetDec(dDec);
		}
		break;
	case 9:
		{
			double dRange = atof(pDispInfo->item.pszText);
			//dRange = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dRange);
			pVehicleProperty->SetRange(dRange);
		}
		break;
	case 10:
		{
			double dFuel = atof(pDispInfo->item.pszText);
			pVehicleProperty->SetFuel(dFuel);
		}
		break;
	case 11:
		{
			double dseperation = atof(pDispInfo->item.pszText);
			dseperation = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dseperation);
			pVehicleProperty->SetSeperation(dseperation);
		}
		break;
	case 12:
		{
			if (strSel == _T("New Operating Distribution..."))
			{
				CProbDistEntry* pPDEntry = NULL;
				CDlgProbDist dlg(m_pInputLandside->GetAirPortDataBase(), false);
				if(dlg.DoModal() == IDOK)
				{
					pPDEntry = dlg.GetSelectedPD();
					if(pPDEntry == NULL)
					{
						return;
					}
					pVehicleProperty->GetDistrubution()->SetProDistrubution(pPDEntry);
					//char strlongitudinal[1024];
					m_wndListCtrl.SetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem,pPDEntry->m_pProbDist->screenPrint());
				}
			}
			else
			{
				CProbDistEntry* pPDEntry = NULL;

				int nCount = pProbDistMan->getCount();
				for( int i=0; i<nCount; i++ )
				{
					pPDEntry = pProbDistMan->getItem( i );
					if(pPDEntry->m_csName == strSel)
						break;
				}
				pVehicleProperty->GetDistrubution()->SetProDistrubution(pPDEntry);
				m_wndListCtrl.SetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem,pPDEntry->m_pProbDist->screenPrint());
			}
		}
	default:
		break;
	}


	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	*pResult = 0;
}

void CDlgLandsideVehicleGroupProperty::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_wndToolBar.MoveWindow(&rect);

	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_ADD, FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_DEL, FALSE);
}

bool CDlgLandsideVehicleGroupProperty::ConvertUnitFromDBtoGUI( void )
{
	return (false);
}

bool CDlgLandsideVehicleGroupProperty::RefreshGUI( void )
{
	InitListControl();
	return (true);
}	

bool CDlgLandsideVehicleGroupProperty::ConvertUnitFromGUItoDB( void )
{
	return (false);
}

void CDlgLandsideVehicleGroupProperty::SetListContent()
{
	int nCount = m_landsideVehicleGroupProperties->GetElementCount();
	for (int i = 0; i < nCount; i++)
	{
		LandsideVehicleProperty* pVehicleProperty = m_landsideVehicleGroupProperties->GetItem(i);

		m_wndListCtrl.InsertItem(i,pVehicleProperty->GetGroupName());
		//length
		CString strLenght;
		double dLength = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetLenght());
		strLenght.Format(_T("%.2f"),dLength);
		m_wndListCtrl.SetItemText(i,1,strLenght);

		//width
		CString strWidth;
		double dWidth = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetWidth());
		strWidth.Format(_T("%.2f"),dWidth);
		m_wndListCtrl.SetItemText(i,2,strWidth);

		//Height
		CString strHeight;
		double dHeight = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetHeight());
		strHeight.Format(_T("%.2f"),dHeight);
		m_wndListCtrl.SetItemText(i,3,strHeight);
	
		//Weight
		CString strWeigth;
		double dWeigth = CARCWeightUnit::ConvertWeight(DEFAULT_DATABASE_WEIGHT_UNIT,CUnitPiece::GetCurSelWeightUnit(),pVehicleProperty->GetWeigth());
		strWeigth.Format(_T("%.2f"),dWeigth);
		m_wndListCtrl.SetItemText(i,4,strWeigth);

		//Capacity
		CString strCapacity;
		strCapacity.Format(_T("%d"),pVehicleProperty->GetCapacity());
		m_wndListCtrl.SetItemText(i,5,strCapacity);

		//Park buffer
		CString strParkBuffer;
		double dParkBuffer = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetParkBuff());
		strParkBuffer.Format(_T("%.2f"),dParkBuffer);
		m_wndListCtrl.SetItemText(i,6,strParkBuffer);

		//Acc
		CString strAcc;
		double dAcc = CARCAccelerationUnit::ConvertAcceleration(DEFAULT_DATABASE_ACCELERATION_UNIT,CUnitPiece::GetCurSelAccelerationUnit(),pVehicleProperty->GetAcc());
		strAcc.Format(_T("%.2f"),dAcc);
		m_wndListCtrl.SetItemText(i,7,strAcc);

		//Dec
		CString strDec;
		double dDec = CARCAccelerationUnit::ConvertAcceleration(DEFAULT_DATABASE_ACCELERATION_UNIT,CUnitPiece::GetCurSelAccelerationUnit(),pVehicleProperty->GetDec());
		strDec.Format(_T("%.2f"),dDec);
		m_wndListCtrl.SetItemText(i,8,strDec);

		//Range
		CString strRange;
		//double dRange = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetRange());
		strRange.Format(_T("%.2f"),pVehicleProperty->GetRange());
		m_wndListCtrl.SetItemText(i,9,strRange);

		//Fuel
		CString strFuel;
		strFuel.Format(_T("%.2f"),pVehicleProperty->GetFuel());
		m_wndListCtrl.SetItemText(i,10,strFuel);
		
		//seperation in Q
		CString strSeperation;
		double dseperation = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pVehicleProperty->GetSeperation());
		strSeperation.Format(_T("%.2f"),dseperation);
		m_wndListCtrl.SetItemText(i,11,strSeperation);
		
		//longitudinal deviation
		char strlongitudinal[1024];
		pVehicleProperty->GetDistrubution()->GetProbDistribution()->screenPrint(strlongitudinal);
		m_wndListCtrl.SetItemText(i,12,strlongitudinal);

		m_wndListCtrl.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pVehicleProperty);
	}

	if (m_wndListCtrl.GetItemCount() > 0)
	{
		m_wndListCtrl.SetCurSel(0);
		m_wndListCtrl.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}

void CDlgLandsideVehicleGroupProperty::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_landsideVehicleGroupProperties->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgLandsideVehicleGroupProperty::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_landsideVehicleGroupProperties->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	CXTResizeDialog::OnOK();
}
