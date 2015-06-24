#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsidedeicepaddlg.h"
#include "../InputAirside/DeicePad.h"
#include "../Common/WinMsg.h"

IMPLEMENT_DYNAMIC(CAirsideDeicePadDlg, CAirsideObjectBaseDlg)
CAirsideDeicePadDlg::CAirsideDeicePadDlg(int nDeicePadID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nDeicePadID,nAirportID,nProjID,pParent)
{
	m_pObject = new DeicePad();
	m_pObject->setAptID(nAirportID);
	m_bInConsModified = false;
	m_bOutConsModified = false;
}

// CAirsideDeicePadDlg::CAirsideDeicePadDlg( DeicePad *pDeicePad ,int nProjID, CWnd * pParent /*= NULL*/ ): CAirsideObjectBaseDlg(pDeicePad,nProjID,pParent)
// {
// 	m_bInConsModified = false;
// 	m_bOutConsModified = false;
// }
CAirsideDeicePadDlg::~CAirsideDeicePadDlg(void)
{
	//delete m_pObject;
}
BEGIN_MESSAGE_MAP(CAirsideDeicePadDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_PROCPROP_TOGGLEDBACKUP, OnProcpropToggledBackup)
	ON_COMMAND(ID_DEICEPAD_MODIFYWINGSPAN, OnModifyWingspan)
	ON_COMMAND(ID_DEICEPAD_MODIFYLENGTH, OnModifyLength)
END_MESSAGE_MAP()
BOOL CAirsideDeicePadDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DEICE));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Deicepad"));
	}
	else
	{
		SetWindowText(_T("Define Deicepad"));
	} 

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_4,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_treeProp.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);
	/////

	LoadTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAirsideDeicePadDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hWingSpan)
	{
		m_hRClickItem = hTreeItem;
		OnModifyWingspan();
	}

	if(hTreeItem == m_hLength)
	{
		m_hRClickItem = hTreeItem;
		OnModifyLength();
	}

	if (hTreeItem == m_hAsStand)
	{
		m_hRClickItem = hTreeItem;
		OnModifyAsStand();
	}

}

void CAirsideDeicePadDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem  == m_hServiceLocation ||
		m_hRClickItem == m_hInConstraint)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	
	}
	if (m_hRClickItem == m_hOutConstraint)
	{	
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->EnableMenuItem( ID_PROCPROP_TOGGLEDQUEUEFIXED, MF_GRAYED );
		pMenu->EnableMenuItem( ID_PROCPROP_TOGGLEDBACKUP, MF_ENABLED );
	}

	if (m_hLength == m_hRClickItem)
	{
		pMenu = menuPopup.GetSubMenu(65);
		pMenu->DeleteMenu(ID_DEICEPAD_MODIFYWINGSPAN,MF_BYCOMMAND);
	}

	if (m_hWingSpan == m_hRClickItem)
	{
		pMenu = menuPopup.GetSubMenu(65);
		pMenu->DeleteMenu(ID_DEICEPAD_MODIFYLENGTH,MF_BYCOMMAND);
	}

}

void CAirsideDeicePadDlg::OnModifyWingspan()
{

	double dWingspan = ((DeicePad *)GetObject())->GetWinSpan();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), dWingspan )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideDeicePadDlg::OnModifyLength()
{
	double dLength = ((DeicePad *)GetObject())->GetLength();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), dLength )) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );

}

void CAirsideDeicePadDlg::OnModifyAsStand()
{
	std::vector<CString> vStr;
	vStr.clear();
	CString str = _T("Yes");
	vStr.push_back(str);
	str = _T("No");
	vStr.push_back(str);
	m_treeProp.SetComboWidth(60);
	m_treeProp.SetComboString(m_hAsStand,vStr);
}
LRESULT CAirsideDeicePadDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hWingSpan)
		{	
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((DeicePad*)GetObject())->SetWinspan( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
		else if (m_hRClickItem == m_hLength)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((DeicePad*)GetObject())->SetLength( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			UpdateObjectViews();
			LoadTree();
		}
	}

	if (message == WM_INPLACE_COMBO2)
	{
		if (m_hRClickItem == m_hAsStand)
		{
			int nSel = m_treeProp.m_comboBox.GetCurSel();
			if (nSel >= 0)
			{
				if (nSel == 0)
				{
					((DeicePad*)GetObject())->SetAsStand(true);
				}
				else
				{
					((DeicePad*)GetObject())->SetAsStand(false);
				}

				m_bPropModified = true;
				UpdateObjectViews();
				LoadTree();
			}

		}
	}

	//if (message == WM_INPLACE_EDIT)
	//{
	//	if (m_hRClickItem == m_hMarking1)
	//	{	
	//		CString strValue = *(CString*)lParam;
	//		((Runway*)m_pObject)->SetMarking1((const char *)strValue);

	//		LoadTree();
	//	}
	//	else if (m_hRClickItem == m_hMarking2)
	//	{		
	//		CString strValue = *(CString*)lParam;
	//		((Runway*)m_pObject)->SetMarking2((const char *)strValue);

	//		LoadTree();
	//	}
	//}

	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
FallbackReason CAirsideDeicePadDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hServiceLocation)
	{
		return PICK_ONEPOINT;
	}

	if (m_hRClickItem == m_hInConstraint ||		m_hRClickItem == m_hOutConstraint)
	{
		return PICK_MANYPOINTS;
	}
	return PICK_MANYPOINTS;

}
void CAirsideDeicePadDlg::SetObjectPath(CPath2008& path)
{
	if (m_hServiceLocation == m_hRClickItem)
	{
		((DeicePad *)GetObject())->SetServicePoint(path.getPoint(0));
		m_bPropModified = true;
		UpdateObjectViews();
		LoadTree();
	}
	else if (m_hOutConstraint == m_hRClickItem)
	{
		((DeicePad *)GetObject())->SetOutConstrain(path);
		m_bOutConsModified = true;
		UpdateObjectViews();
		LoadTree();
	}
	else if (m_hInConstraint == m_hRClickItem)
	{
		((DeicePad *)GetObject())->SetInContrain(path);
		m_bInConsModified = true;
		UpdateObjectViews();
		LoadTree();
	}

}
void CAirsideDeicePadDlg::LoadTree()
{

	m_treeProp.DeleteAllItems();

	// Service Location
	CString csLabel = CString("Service Location (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hServiceLocation = m_treeProp.InsertItem( csLabel );

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	CPoint2008 point = ((DeicePad *)GetObject())->GetServicePoint();
	
	CPoint2008 ptDefault;
	if (point == ptDefault)
	{
	}
	else
	{
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		CString strPoint = _T("");
		strPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ()));

		hItemTemp = m_treeProp.InsertItem(strPoint,m_hServiceLocation);
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
		m_treeProp.Expand(m_hServiceLocation,TVE_EXPAND);
	}
	csLabel.Empty();
	csLabel = CString("In Constraint(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hInConstraint = m_treeProp.InsertItem( csLabel );

	CPath2008 path = ((DeicePad *)GetObject())->GetInContrain();

	for (int i =0; i <path.getCount();++i)
	{
		CPoint2008 pt = path.getPoint(i);
		
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		CString strPoint = _T("");
		strPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));

		hItemTemp = m_treeProp.InsertItem(strPoint,m_hInConstraint);
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}
	m_treeProp.Expand(m_hInConstraint,TVE_EXPAND);


	csLabel.Empty();
	csLabel = CString("Out Constraint(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hOutConstraint = m_treeProp.InsertItem( csLabel );

	if (((DeicePad *)GetObject())->IsUsePushBack() )
			m_treeProp.InsertItem( _T("Back up"), m_hOutConstraint);

	path = ((DeicePad *)GetObject())->GetOutConstrain();

	for (int i =0; i <path.getCount();++i)
	{
		CPoint2008 pt = path.getPoint(i);

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		CString strPoint = _T("");
		strPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pt.getZ()));

		hItemTemp = m_treeProp.InsertItem(strPoint,m_hOutConstraint);
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}

	m_treeProp.Expand(m_hOutConstraint,TVE_EXPAND);


	//wing span
	csLabel.Empty();
	csLabel.Format(_T("WingSpan ("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"), 
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((DeicePad*)GetObject())->GetWinSpan()));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((DeicePad*)GetObject())->GetWinSpan()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hWingSpan =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hWingSpan,aoidDataEx);


	//Length
	csLabel.Empty();
	csLabel.Format(_T("Length ("+ CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) +")(%.0f)"), 
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((DeicePad*)GetObject())->GetLength()));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),((DeicePad*)GetObject())->GetLength()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLength =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hLength,aoidDataEx);

	CString strAsStand = _T("No");
	if (((DeicePad*)GetObject())->IsAsStand())
		strAsStand = _T("Yes");

	csLabel.Empty();
	csLabel.Format(_T("Used as Stand: ")+ strAsStand);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	isscStringColor.strSection = strAsStand;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hAsStand =  m_treeProp.InsertItem(csLabel);
	m_treeProp.SetItemDataEx(m_hAsStand,aoidDataEx);
}

void CAirsideDeicePadDlg::OnProcpropToggledBackup()
{

	m_bPropModified = true;
	bool bBackup;

	if (bBackup = ((DeicePad *)GetObject())->IsUsePushBack() )
	{
		((DeicePad *)GetObject())->UsePushBack(false);
	}
	else
	{
		((DeicePad *)GetObject())->UsePushBack(true);	
	}

	//if use backup, StandProcessor's data need update immediately

	CPath2008 path = ((DeicePad *)GetObject())->GetOutConstrain();
	int nCount = path.getCount();
	
	CPoint2008 *pt = new CPoint2008[nCount];

	for (int i=0; i < nCount; ++i)
	{
		pt[nCount-i-1] = path.getPoint(i);
	}

	CPath2008 tempPath;
	tempPath.init(nCount ,pt);
	delete []pt;

	((DeicePad *)GetObject())->SetOutConstrain(tempPath);

	UpdateObjectViews();
	LoadTree();
}
void CAirsideDeicePadDlg::OnOK()
{
	DeicePad* pDeicePad = (DeicePad *)GetObject();
	if( pDeicePad->GetInContrain().getCount()==0)
	{
		MessageBox("Please define In Constraint!");
		return ;
	}
	if(pDeicePad->GetOutConstrain().getCount()==0)
	{
		MessageBox("Please define Out Constraint!");
		return;
	}
	CAirsideObjectBaseDlg::OnOK();
}
bool CAirsideDeicePadDlg::UpdateOtherData()
{
	CPoint2008 point = ((DeicePad *)GetObject())->GetServicePoint();

	CPoint2008 ptDefault;
	if (point == ptDefault)
	{
		MessageBox(_T("Please pick deice pad service location."));
		return false;
	}
	if (m_nObjID != -1)
	{
		//in constraint
		try
		{
			if (m_bInConsModified)
			{
				((DeicePad *)GetObject())->UpdateInConstraint();
			}
		}
		catch (CADOException &e)
		{
			e.ErrorMessage();
			MessageBox(_T("Cann't update the Object in constraint."));
			return false;
		}
		
		//out constraint
		try
		{
			if (m_bOutConsModified)
			{
				((DeicePad *)GetObject())->UpdateOutConstraint();
			}
		}
		catch (CADOException &e)
		{
			e.ErrorMessage();
			MessageBox(_T("Cann't update the Object out constraint."));
			return false;
		}

	}

	return true;
}

bool CAirsideDeicePadDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideDeicePadDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideDeicePadDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}
