#include "StdAfx.h"
#include ".\onboardelevatordlg.h"

#include "inputonboard/AircaftLayOut.h"
#include "InputOnBoard/OnBoardBaseVerticalDevice.h"
#include <common/WinMsg.h>

IMPLEMENT_DYNAMIC(COnBoardElevatorDlg, COnBoardObjectBaseDlg)

COnBoardElevatorDlg::COnBoardElevatorDlg(COnBoardElevator *pElevator,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/)
:COnBoardObjectBaseDlg(pElevator,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;
}

COnBoardElevatorDlg::~COnBoardElevatorDlg( void )
{

}
void COnBoardElevatorDlg::DoDataExchange(CDataExchange* pDX)
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(COnBoardElevatorDlg,COnBoardObjectBaseDlg)
	ON_COMMAND(ID_TOGGLED,OnDefineToggled)
END_MESSAGE_MAP()

BOOL COnBoardElevatorDlg::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );
	COnBoardElevator& tempElevator = *(COnBoardElevator *)m_pObject;

	if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify Elevator"));
	}
	else
	{
		SetWindowText(_T("Define Elevator"));

		CDeckManager* pDeckman = m_pLayout->GetDeckManager();			
		int nFloorCount= pDeckman->getCount();

		tempElevator.SetMinFloor(0);
		tempElevator.SetMaxFloor(nFloorCount-1);
		tempElevator.GetWaitAreaPos().clear();
		for(int i=tempElevator.GetMinFloor()+1;i<=tempElevator.GetMaxFloor()+1;i++)
		{
			tempElevator.GetWaitAreaPos().push_back(TRUE);
		}
	}

	
	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_1,this);

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

	//init tree properties
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void COnBoardElevatorDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{	
	COnBoardElevator& m_tempElevator = *(COnBoardElevator *)m_pObject;
	CDeckManager* pDeckman = m_pLayout->GetDeckManager();	
	int nFloorCount= pDeckman->getCount();

	if(hTreeItem == m_hWaitAreaWidth)
	{
		m_hRClickItem = hTreeItem;
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(ConvertLength(m_tempElevator.GetPlatFormWidth())) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}	
	else if(hTreeItem == m_hWaitAreaLength)
	{
		m_hRClickItem = hTreeItem;
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(ConvertLength(m_tempElevator.GetPlatFormLength())) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hLiftNOL)
	{
		m_hRClickItem = hTreeItem;
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( m_tempElevator.GetNumberOfLift());
		m_treeProp.SetSpinRange( 1,10);
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hLiftLength)
	{
		m_hRClickItem = hTreeItem;
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(ConvertLength(m_tempElevator.GetElevatorAreaLength())) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hLiftWidth)
	{
		m_hRClickItem = hTreeItem;
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(ConvertLength(m_tempElevator.GetElevatorAreaWidth())) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hMinFloor)
	{
		m_hRClickItem = hTreeItem;
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( m_tempElevator.GetMinFloor()+1);
		ASSERT(nFloorCount>1);
		m_treeProp.SetSpinRange( 1, m_tempElevator.GetMaxFloor()-1+1);
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	else if(hTreeItem == m_hMaxFloor)
	{
		m_hRClickItem = hTreeItem;
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( m_tempElevator.GetMaxFloor()+1);
		ASSERT(nFloorCount>1);
		m_treeProp.SetSpinRange( m_tempElevator.GetMinFloor()+1+1,nFloorCount );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}

}
void COnBoardElevatorDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem)
	{
		m_treeProp.SelectItem(m_hRClickItem);

		if(m_hRClickItem==m_hItemServiceLocation)
		{
			pMenu=menuPopup.GetSubMenu(5);
			pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);
		}		
		if(m_treeProp.GetParentItem(m_hRClickItem)==m_hWaitAreaPos)
			pMenu=menuPopup.GetSubMenu(41);
		else
			return;		
	}
}

LRESULT COnBoardElevatorDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	COnBoardElevator& m_tempElevator = *(COnBoardElevator *)m_pObject;

	if(message == WM_INPLACE_SPIN)
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		CDeckManager* pDeckman = m_pLayout->GetDeckManager();			
		int nFloorCount= pDeckman->getCount();

		if(m_hRClickItem==m_hWaitAreaLength)
			m_tempElevator.SetPlatFormLength(UnConvertLength(pst->iPercent));
		else if(m_hRClickItem==m_hWaitAreaWidth)
			m_tempElevator.SetPlatFormWidth(UnConvertLength(pst->iPercent));
		else if(m_hRClickItem==m_hLiftNOL)
			m_tempElevator.SetNumberOfLift(pst->iPercent);
		else if(m_hRClickItem==m_hLiftLength)
			m_tempElevator.SetElevatorAreaLength(UnConvertLength(pst->iPercent));
		else if(m_hRClickItem==m_hLiftWidth)
			m_tempElevator.SetElevatorAreaWidth(UnConvertLength(pst->iPercent));
		else if(m_hRClickItem==m_hMinFloor)
		{
			
			if(pst->iPercent-1<0||pst->iPercent-1>=m_tempElevator.GetMaxFloor())
			{
				CString strMaxFloor;
				strMaxFloor.Format("Minimum floor number's range is from 1 to %d",m_tempElevator.GetMaxFloor());
				AfxMessageBox(strMaxFloor);
				return TRUE;
			}
			m_tempElevator.SetMinFloor(pst->iPercent-1);
			m_tempElevator.GetWaitAreaPos().clear();

			for(int i=m_tempElevator.GetMinFloor()+1;i<=m_tempElevator.GetMaxFloor()+1;i++)
			{
				m_tempElevator.GetWaitAreaPos().push_back(TRUE);
			}
		}
		else if(m_hRClickItem==m_hMaxFloor)
		{			
			if(pst->iPercent-1<=m_tempElevator.GetMinFloor()||pst->iPercent-1>nFloorCount-1)
			{
				CString strMaxFloor;
				strMaxFloor.Format("Maximum floor number's range is from %d to %d",m_tempElevator.GetMinFloor()+1+1,nFloorCount);
				AfxMessageBox(strMaxFloor);
				return TRUE;
			}
			m_tempElevator.SetMaxFloor(pst->iPercent-1);
			m_tempElevator.GetWaitAreaPos().clear();
			for(int i=m_tempElevator.GetMinFloor()+1;i<=m_tempElevator.GetMaxFloor()+1;i++)
			{
				m_tempElevator.GetWaitAreaPos().push_back(TRUE);
			}

		}
		LoadTree();
		return TRUE;
	}
	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
FallbackReason COnBoardElevatorDlg::GetFallBackReason()
{	
	return PICK_ONEPOINT;
}


void COnBoardElevatorDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	COnBoardElevator *pElevator = (COnBoardElevator *)m_pObject;

	CString sUnit = _T("(")+CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	//position	
	CString strPositionLabel =_T("Servive Location")+sUnit;
	m_hItemServiceLocation = m_treeProp.InsertItem(strPositionLabel);

	InsertPathToTree(pElevator->m_sevicePath,m_hItemServiceLocation);	
	m_treeProp.Expand(m_hItemServiceLocation,TVE_EXPAND);


	//Wait Area
	CString csLabel=CString("Wait Area");
	m_hWaitArea=m_treeProp.InsertItem(csLabel);
	//CString sUnit= UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
	csLabel.Format(" ( %.2f )",ConvertLength(pElevator->GetPlatFormLength()) );
	csLabel=CString("Length ")+sUnit+csLabel;
	m_hWaitAreaLength=m_treeProp.InsertItem(csLabel,m_hWaitArea);

	csLabel.Format(" ( %.2f )",ConvertLength(pElevator->GetPlatFormWidth()) );
	csLabel=CString("Width ")+sUnit+csLabel;
	m_hWaitAreaWidth=m_treeProp.InsertItem(csLabel,m_hWaitArea);
	m_treeProp.Expand(m_hWaitArea,TVE_EXPAND);

	//Lift
	csLabel=CString("Lift");
	m_hLift=m_treeProp.InsertItem(csLabel);

	csLabel.Format(" ( %d )",pElevator->m_iNumberOfLift);
	csLabel=CString("Number Of Lift")+csLabel;
	m_hLiftNOL=m_treeProp.InsertItem(csLabel,m_hLift);

	csLabel.Format(" ( %.2f )",ConvertLength(pElevator->GetElevatorAreaLength()));
	csLabel=CString("Length ")+sUnit+csLabel;
	m_hLiftLength=m_treeProp.InsertItem(csLabel,m_hLift);

	csLabel.Format(" ( %.2f )",ConvertLength(pElevator->GetElevatorAreaWidth()));
	csLabel=CString("Width ")+sUnit+csLabel;
	m_hLiftWidth=m_treeProp.InsertItem(csLabel,m_hLift);

	m_treeProp.Expand(m_hLift,TVE_EXPAND);

	//Min\Max Floor
	csLabel.Format(" ( %d )",pElevator->GetMinFloor()+1);
	csLabel=CString("Min Floor")+csLabel;
	m_hMinFloor=m_treeProp.InsertItem(csLabel);
	csLabel.Format(" ( %d )",pElevator->GetMaxFloor()+1);
	csLabel=CString("Max Floor")+csLabel;
	m_hMaxFloor=m_treeProp.InsertItem(csLabel);

	//Wait Area Position
	csLabel=CString("Wait Area Position");
	m_hWaitAreaPos=m_treeProp.InsertItem(csLabel);
	int nn=pElevator->GetWaitAreaPos().size();
	int nFloor  = pElevator->GetMaxFloor()-pElevator->GetMinFloor()+1;
	if(nn<nFloor)
		pElevator->GetWaitAreaPos().resize(nFloor,false);

	for(int i=pElevator->GetMinFloor();i<=pElevator->GetMaxFloor();i++)
	{
		bool b = pElevator->GetWaitAreaPos().at(i-pElevator->GetMinFloor());
		CString strFloor;
		strFloor.Format("Floor %d (%s)",i+1,b?"Front":"Hind");
		HTREEITEM hTempItem=m_treeProp.InsertItem(strFloor,m_hWaitAreaPos);
		m_treeProp.SetItemData(hTempItem,i);
	}
}

void COnBoardElevatorDlg::OnOK()
{
	if(SetObjectName() == false)
		return;
	CADODatabase::BeginTransaction();
	//
	try
	{
		((COnBoardElevator *)m_pObject)->SetLayOut(m_pLayout);
		((COnBoardElevator *)m_pObject)->SaveData(m_pLayout->GetID());
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(_T("Save data failed"));
		return;
	}

	((COnBoardElevator*)m_pObject)->InitLayout();
	COnBoardObjectBaseDlg::OnOK();
}


bool COnBoardElevatorDlg::UpdateOtherData()
{	
	return true;
}

bool COnBoardElevatorDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool COnBoardElevatorDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool COnBoardElevatorDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void COnBoardElevatorDlg::InsertPathToTree( const Path& path, HTREEITEM hParentItem )
{
	CDeckManager* pDeckList = m_pLayout->GetDeckManager();
	for (int i =0; i < path.getCount();++i)
	{
		Point point = path.getPoint(i);
		CString sDeckName = pDeckList->GetSortDeckName((int)point.getZ());
		InsertPointToTree(point,hParentItem,sDeckName);
	}
}

void COnBoardElevatorDlg::InsertPointToTree( const Point& point, HTREEITEM hParentItem,const CString& strDeckName )
{
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;


	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255);
	strTemp.Format( _T("%.2f"),ConvertLength(point.getX()) );		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	strTemp.Format(_T("%.2f"),ConvertLength(point.getY()) );		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ()));		
	isscStringColor.strSection = strDeckName;//strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	CString strPoint = _T("");
	strPoint.Format( "x = %.2f; y = %.2f; %s",
		ConvertLength(point.getX()),
		ConvertLength(point.getY()),
		strDeckName);

	hItemTemp = m_treeProp.InsertItem(strPoint,hParentItem);
	m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
}


void COnBoardElevatorDlg::SetObjectPath( const ARCPath3& path )
{
	if (m_hItemServiceLocation == m_hRClickItem)
	{
		CDeckManager* pDecKMan = m_pLayout->GetDeckManager();
		int iDeck=  pDecKMan->GetActiveDeckIndex();
		//convert 
		Path floorPath = path.getPath();
		for(int i=0;i<floorPath.getCount();i++)
			floorPath[i].setZ(iDeck);		
		((COnBoardElevator *)m_pObject)->SetPathService(floorPath);
		m_bPathModified = true;

		UpdateObjectViews();
		LoadTree();
	}
}

void COnBoardElevatorDlg::OnDefineToggled()
{
	COnBoardElevator& m_tempElevator = *(COnBoardElevator *)m_pObject;

	DWORD dwItemData=m_treeProp.GetItemData(m_hRClickItem);
	BOOL bValue=m_tempElevator.GetPosOfWaitArea(dwItemData);
	bValue=!bValue;
	m_tempElevator.SetPosOfWaitArea(dwItemData, bValue ? true : false);
	LoadTree();
	m_treeProp.Expand(m_hWaitAreaPos,TVE_EXPAND);
}