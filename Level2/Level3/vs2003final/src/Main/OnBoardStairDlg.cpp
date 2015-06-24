#include "StdAfx.h"
#include ".\onboardstairdlg.h"

#include "InputOnBoard/OnBoardBaseVerticalDevice.h"
#include "inputonboard/AircaftLayOut.h"
#include "../Common/WinMsg.h"
#include "ChangeVerticalProfileDlg.h"

IMPLEMENT_DYNAMIC(COnBoardStairDlg, COnBoardObjectBaseDlg)

COnBoardStairDlg::COnBoardStairDlg(COnBoardStair *pStair,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/)
:COnBoardObjectBaseDlg(pStair,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;
}

COnBoardStairDlg::~COnBoardStairDlg( void )
{

}
void COnBoardStairDlg::DoDataExchange(CDataExchange* pDX)
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(COnBoardStairDlg,COnBoardObjectBaseDlg)

END_MESSAGE_MAP()

BOOL COnBoardStairDlg::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOOR));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify Stair"));
	}
	else
	{
		SetWindowText(_T("Define Stair"));
	}

	
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


void COnBoardStairDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hItemWidth)
	{
		m_hRClickItem = hTreeItem;
		double dValue = 0.0;
		dValue = ((COnBoardStair *)m_pObject)->GetWidth();
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue )) );
		m_treeProp.SetSpinRange( 1,10000 );
		m_treeProp.SpinEditLabel( m_hRClickItem );
	}


}
void COnBoardStairDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem==m_hItemServiceLocation)
	{		
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_DELETE ,MF_BYCOMMAND);
		pMenu->EnableMenuItem( ID_PROCPROP_DEFINE_Z_POS ,MF_ENABLED );
	}



}

LRESULT COnBoardStairDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	if( message == WM_INPLACE_SPIN )
	{
		if (m_hRClickItem == m_hItemWidth)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			((COnBoardStair*)m_pObject)->SetWidth(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			m_bPropModified = true;
			LoadTree();
		}

		if(m_bPropModified)
			UpdateObjectViews();
	}
	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason COnBoardStairDlg::GetFallBackReason()
{
	return PICK_MANYPOINTS;
}

void COnBoardStairDlg::SetObjectPath(const ARCPath3& path)
{
	if (m_hItemServiceLocation == m_hRClickItem)
	{
		CDeckManager* pDecKMan = m_pLayout->GetDeckManager();
		int iDeck=  pDecKMan->GetActiveDeckIndex();
		//convert 
		Path floorPath = path.getPath();
		for(int i=0;i<floorPath.getCount();i++)
			floorPath[i].setZ(iDeck);		
		((COnBoardStair *)m_pObject)->SetPathService(floorPath);
		m_bPathModified = true;

		UpdateObjectViews();
		LoadTree();
	}
}

void COnBoardStairDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	COnBoardStair *pStair = (COnBoardStair *)m_pObject;

	//position	
	CString strPositionLabel =_T("Servive Location(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemServiceLocation = m_treeProp.InsertItem(strPositionLabel);

	InsertPathToTree(pStair->m_sevicePath,m_hItemServiceLocation);	
	m_treeProp.Expand(m_hItemServiceLocation,TVE_EXPAND);
	//dimension
	CString strDimensionLabel = _T("Dimension(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");

	HTREEITEM hItemDimension = m_treeProp.InsertItem(strDimensionLabel);
	{	
		AirsideObjectTreeCtrlItemDataEx aoidDataEx;
		ItemStringSectionColor isscStringColor;
		CString strTemp = _T("");
		HTREEITEM hItemTemp = NULL;
		//width
		{
			double dWidth = pStair->GetWidth();
			CString csLabel;

			csLabel.Empty();
			csLabel.Format(_T("Width(%.0f)"), CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dWidth));
			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.0f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dWidth));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
			m_hItemWidth =  m_treeProp.InsertItem(csLabel,hItemDimension);
			m_treeProp.SetItemDataEx(m_hItemWidth,aoidDataEx);
		}
		m_treeProp.Expand(hItemDimension,TVE_EXPAND);
	}


}

void COnBoardStairDlg::OnOK()
{
	if(SetObjectName() == false)
		return;
	CADODatabase::BeginTransaction();
	//
	try
	{
		((COnBoardStair *)m_pObject)->SetLayOut(m_pLayout);
		((COnBoardStair *)m_pObject)->SaveData(m_pLayout->GetID());
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(_T("Save data failed"));
		return;
	}


	COnBoardObjectBaseDlg::OnOK();
}

bool COnBoardStairDlg::UpdateOtherData()
{	
	return true;
}

bool COnBoardStairDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool COnBoardStairDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool COnBoardStairDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void COnBoardStairDlg::InsertPathToTree( const Path& path, HTREEITEM hParentItem )
{
	CDeckManager* pDeckList = m_pLayout->GetDeckManager();
	for (int i =0; i < path.getCount();++i)
	{
		Point point = path.getPoint(i);
		CString sDeckName = pDeckList->GetSortDeckName((int)point.getZ());
		InsertPointToTree(point,hParentItem,sDeckName);
	}
}

void COnBoardStairDlg::InsertPointToTree( const Point& point, HTREEITEM hParentItem,const CString& strDeckName )
{
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;


	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255);
	strTemp.Format(_T("%.2f"),ConvertLength(point.getX()));		
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

	strTemp.Format(_T("%.2f"),ConvertLength(point.getY()));		
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

void COnBoardStairDlg::DefineZPos()
{
	if(m_pLayout == NULL)
		return;

	std::vector<double>vDeckAltitudes;
	std::vector<CDeck *> vSortedDeck =  m_pLayout->GetDeckManager()->getSortedDecks();
	for (int nDeck = 0; nDeck < static_cast<int>(vSortedDeck.size()); ++ nDeck)
	{
		vDeckAltitudes.push_back(nDeck);
	}

	std::vector<double>vXPos;
	std::vector<double>vZPos;
	//Path _tmpPath;

	const Path& pPathService = ((COnBoardStair *)m_pObject)->GetPathService();

	int ptCount = pPathService.getCount();
	if(ptCount>0) 
	{
		vXPos.reserve(ptCount);
		vZPos.reserve(ptCount);
		vXPos.push_back(0.0);
		vZPos.push_back( pPathService.getPoint(0).getZ() );
		double dLenSoFar = 0.0;
		for( int i=1; i<ptCount; i++ )
		{			
			Point ptdir = pPathService.getPoint(i)-pPathService.getPoint(i-1);
			ptdir.setZ(0);			
			dLenSoFar +=  ConvertLength(ptdir.length());
			vXPos.push_back(dLenSoFar);				
			vZPos.push_back( pPathService.getPoint(i).getZ());
		}
	}

	CStringArray strArrFloorName;
	strArrFloorName.SetSize(vSortedDeck.size());
	for(int k=0;k<static_cast<int>(vSortedDeck.size());k++)
	{
		strArrFloorName[k]=vSortedDeck.at(k)->GetName();
	}
	CChangeVerticalProfileDlg dlg(vXPos, vZPos, vDeckAltitudes, pPathService ,&strArrFloorName,EnvMode_OnBoard,this);

	if( dlg.DoModal() == IDOK )
	{
		vZPos = dlg.m_wndPtLineChart.m_vPointYPos;
		Path pathServiceTmp(pPathService);

		for( int i=0; i<static_cast<int>(vZPos.size()); i++ )
		{
			pathServiceTmp[ i ].setZ(vZPos[i]);
		}
		((COnBoardEscalator *)m_pObject)->SetPathService(pathServiceTmp);

		//refresh Tree
		LoadTree();
	}
}
