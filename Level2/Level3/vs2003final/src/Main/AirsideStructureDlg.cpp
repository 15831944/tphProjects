#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsidestructuredlg.h"
#include "../InputAirside/Structure.h"
#include "../Common/WinMsg.h"
#include "../Common/Path2008.h"
#include "../Common/LatLongConvert.h"
#include "../InputAirside/InputAirside.h"

enum emEideType
{
	NoType = 0,
	BaseElevation,
	StructHeight,
	Obstruction,
	SideText,
	SidePath
};
IMPLEMENT_DYNAMIC(CAirsideStructureDlg, CAirsideObjectBaseDlg)
CAirsideStructureDlg::CAirsideStructureDlg(int nStructureID,int nAirportID,int nProjID,CWnd* pParent):
CAirsideObjectBaseDlg(nStructureID,nAirportID,nProjID,pParent)
{
	m_pObject = new Structure;
	std::vector<int> vAirportsID;
	InputAirside::GetAirportList(nAirportID,vAirportsID);


	if(vAirportsID.size())
		m_AirportInfo.ReadAirport(vAirportsID.at(0));

	m_bShow = FALSE;
}

//CAirsideStructureDlg::CAirsideStructureDlg( Structure* pStructure,int nProjID, CWnd* pParent /*= NULL*/ ): CAirsideObjectBaseDlg(pStructure, nProjID,pParent)
//{
//
//}
CAirsideStructureDlg::~CAirsideStructureDlg(void)
{	
	//delete m_pObject;
}
BEGIN_MESSAGE_MAP(CAirsideStructureDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_HOLDARGUMENT_EDIT, OnRotationEdit)
	ON_COMMAND(ID_SIDES_SHOW,OnShowSides)
END_MESSAGE_MAP()

void CAirsideStructureDlg::DoDataExchange(CDataExchange* pDX)
{
	//DDX_Control(pDX, IDC_LIST_PROP, m_wndShapeList);
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}
BOOL CAirsideStructureDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap*/ 0);

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Structure"));
	}
	else
	{
		SetWindowText(_T("Define Structure"));
	}

//	GetDlgItem(IDC_STATIC_PROP)->ShowWindow(SW_SHOW);
//	GetDlgItem(IDC_STATIC_PROP)->SetWindowText(_T("Shape"));

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,/*UM_ID_16*/UM_ID_12,this);
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
//	InitShapleList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CAirsideStructureDlg::InitShapleList()
{
// 	CRect rctree;
// 	m_treeProp.GetWindowRect(&rctree);
// 
// 	ScreenToClient(&rctree);
// 	CRect rclist(0,0,0,0);
// 	rclist.SetRect(rctree.Width()/2 + rctree.left +1,rctree.top,rctree.right,rctree.bottom);
// 
// 	rctree.right = rctree.Width()/2 + rctree.left -1;
// 	m_treeProp.MoveWindow(rctree,true);
// 	m_wndShapeList.MoveWindow(rclist);
// 	m_wndShapeList.ShowWindow(SW_SHOW);
// 
// 
// 	m_wndShapeList.ModifyStyle(0, LVS_ICON|LVS_ALIGNTOP|LVS_SHOWSELALWAYS|LVS_SINGLESEL);
}

void CAirsideStructureDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
// 	if(hTreeItem == m_hRotation)
// 	{
// 		m_hRClickItem = hTreeItem;
// 		OnRotationEdit();
// 	}
	m_hRClickItem = hTreeItem;
	AirsideObjectTreeCtrlItemDataEx* pAoidDataEx = (AirsideObjectTreeCtrlItemDataEx*)m_treeProp.GetItemDataEx(hTreeItem);
	if (pAoidDataEx == NULL)
	{
		return;
	}
	CString strValue = _T("");
	strValue = m_treeProp.GetItemText(m_treeProp.GetParentItem(hTreeItem));
	int nPos = -1;
	int nValue = -1;
	switch(pAoidDataEx->dwptrItemData)
	{
	case BaseElevation:
		{
			UserInputData* pUserData = NULL;
			nPos = strValue.Find(' ');
			if (nPos != -1)
			{
				nValue = atoi(strValue.Right(strValue.GetLength()-nPos-1));
				pUserData = ((Structure*)GetObject())->getUserData(nValue -1);
			}
			ASSERT(pUserData);
			m_treeProp.SetDisplayType( 2 );
			m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pUserData->dElevation )) );
			m_treeProp.SetSpinRange( 1,10000 );
			m_treeProp.SpinEditLabel( m_hRClickItem );
		}
		break;
	case Obstruction:
		{
			UserInputData* pUserData = NULL;
			nPos = strValue.Find(' ');
			if (nPos != -1)
			{
				nValue = atoi(strValue.Right(strValue.GetLength()-nPos-1));
				pUserData = ((Structure*)GetObject())->getUserData(nValue -1);
			}
			ASSERT(pUserData);
			m_treeProp.SetDisplayType( 2 );
			m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pUserData->dObstruction )) );
			m_treeProp.SetSpinRange( 1,10000 );
			m_treeProp.SpinEditLabel( m_hRClickItem );
		}
		break;
	case StructHeight:
		{
			UserInputData* pUserData = NULL;
			nPos = strValue.Find(' ');
			if (nPos != -1)
			{
				nValue = atoi(strValue.Right(strValue.GetLength()-nPos-1));
				pUserData = ((Structure*)GetObject())->getUserData(nValue -1);
			}
			ASSERT(pUserData);
			m_treeProp.SetDisplayType( 2 );
			m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pUserData->dStructHeight )) );
			m_treeProp.SetSpinRange( 1,10000 );
			m_treeProp.SpinEditLabel( m_hRClickItem );
		}
		break;
	case SideText:
		{
			StructFace* pStructData = NULL;
			nPos = strValue.Find(' ');
			if (nPos != -1)
			{
				strValue = strValue.Right(strValue.GetLength()-nPos-1);
				nPos = strValue.Find('-');
				nValue = atoi(strValue.Left(nPos));
				pStructData = ((Structure*)GetObject())->getStructFace(nValue -1);
			}
			else
			{
				int nCount = ((Structure*)GetObject())->GetPath().getCount();
				pStructData = ((Structure*)GetObject())->getStructFace(nCount);
			}
			ASSERT(pStructData);
			m_treeProp.StringEditLabel(m_hRClickItem, pStructData->strFaceName);
		}
		break;
	case SidePath:
		{
			StructFace* pStructData = NULL;
			nPos = strValue.Find(' ');
			if (nPos != -1)
			{
				strValue = strValue.Right(strValue.GetLength()-nPos-1);
				nPos = strValue.Find('-');
				nValue = atoi(strValue.Left(nPos));
				pStructData = ((Structure*)GetObject())->getStructFace(nValue -1);
			}
			else
			{
				int nCount = ((Structure*)GetObject())->GetPath().getCount();
				pStructData = ((Structure*)GetObject())->getStructFace(nCount);
			}
			ASSERT(pStructData);
			CFileDialog fileDlg(TRUE, _T("bmp"),NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Bitmap Files (*.bmp)|*.bmp|Jpeg Files(*.jpg)|*.jpg|All Files (*.*)|*.*||"), NULL);
			CString strFileName = _T("");
			if( fileDlg.DoModal() == IDOK )
			{
				m_bPropModified = true;
				strFileName = fileDlg.GetPathName();
				pStructData->strPicPath = strFileName;
				m_treeProp.SetItemText(m_hRClickItem,strFileName);
			}
		}
		break;
	default:
		break;
	}
}

void CAirsideStructureDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem==m_hServiceLocation)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	
	}
	if (m_hRClickItem == m_hRotation || m_treeProp.GetParentItem(m_hRClickItem) == m_hRotation)
	{
		menuPopup.Detach();
		menuPopup.LoadMenu(IDR_CTX_STRUCT);
		pMenu = menuPopup.GetSubMenu(0);
		if (m_hRClickItem == m_hRotation)
		{
			BOOL bFind = FALSE;
			CPath2008 path = ((Structure*)GetObject())->GetPath();
			if (path.getCount() > 0)
			{
				for (int i = 0; i < path.getCount()+1; i++)
				{
					if(((Structure*)GetObject())->getStructFace(i)->bShow == FALSE)
					{
						pMenu->AppendMenu(MF_STRING, ID_SIDES_SHOW, _T("Show Surface..."));
						pMenu->DeleteMenu(ID_SIDES_SHOW,MF_BYCOMMAND);
						bFind = TRUE;
						m_bShow = TRUE;
						break;
					}
				}
			}

			if (bFind == FALSE)
			{
				pMenu->AppendMenu(MF_STRING, ID_SIDES_SHOW, _T("Hide Surface..."));
				pMenu->DeleteMenu(ID_SIDES_SHOW,MF_BYCOMMAND);
				m_bShow = FALSE;
			}
		}
		else
		{
			CString strValue = _T("");
			strValue = m_treeProp.GetItemText(m_hRClickItem);
			int nPos = -1;
			int nValue = -1;

			StructFace* pStructData = NULL;
			CPath2008 path = ((Structure*)GetObject())->GetPath();
			nPos = strValue.Find(' ');
			if (nPos != -1)
			{
				nValue = atoi(strValue.Right(strValue.GetLength()-nPos-1));
				pStructData = ((Structure*)GetObject())->getStructFace(nValue -1);
				ASSERT(pStructData);
				if (pStructData->bShow == TRUE)
				{
					pMenu->AppendMenu(MF_STRING, ID_SIDES_SHOW, _T("Hide Surface..."));
					pMenu->DeleteMenu(ID_SIDES_SHOW,MF_BYCOMMAND);
				}
				else
				{
					pMenu->AppendMenu(MF_STRING, ID_SIDES_SHOW, _T("Show Surface..."));
					pMenu->DeleteMenu(ID_SIDES_SHOW,MF_BYCOMMAND);
				}
			}
			else
			{
				pStructData = ((Structure*)GetObject())->getStructFace(path.getCount());
				ASSERT(pStructData);
				if (pStructData->bShow == TRUE)
				{
					pMenu->AppendMenu(MF_STRING, ID_SIDES_SHOW, _T("Hide Surface..."));
					pMenu->DeleteMenu(ID_SIDES_SHOW,MF_BYCOMMAND);
				}
				else
				{
					pMenu->AppendMenu(MF_STRING, ID_SIDES_SHOW, _T("Show Surface..."));
					pMenu->DeleteMenu(ID_SIDES_SHOW,MF_BYCOMMAND);
				}
			}
		}
	}
}
void CAirsideStructureDlg::OnOK()
{

	CAirsideObjectBaseDlg::OnOK();
}


LRESULT CAirsideStructureDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LPSPINTEXT pst = (LPSPINTEXT) lParam;

	if (message == WM_INPLACE_SPIN)
	{
		AirsideObjectTreeCtrlItemDataEx* pAoidDataEx = (AirsideObjectTreeCtrlItemDataEx*)m_treeProp.GetItemDataEx(m_hRClickItem);
		if (pAoidDataEx == NULL)
		{
			return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
		}
		CString strValue = _T("");
		strValue = m_treeProp.GetItemText(m_treeProp.GetParentItem(m_hRClickItem));
		int nPos = -1;
		int nValue = -1;

		UserInputData* pUserData = NULL;
		nPos = strValue.Find(' ');
		if (nPos != -1)
		{
			nValue = atoi(strValue.Right(strValue.GetLength()-nPos-1));
			pUserData = ((Structure*)GetObject())->getUserData(nValue -1);
		}
		ASSERT(pUserData);
		CString strTemp = _T("");
		switch((emEideType)pAoidDataEx->dwptrItemData)
		{
		case BaseElevation:
			{
				pUserData->dElevation = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent);
				strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pUserData->dElevation));
				strTemp = CString("Base elevation (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())\
					+ CString( ")" ) + CString(":") + strTemp;
				UpdateObjectViews();
			}
			break;
		case Obstruction:
			{
				pUserData->dObstruction = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent);
				strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pUserData->dObstruction));
				strTemp = CString("Obstruction (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())\
					+ CString( ")" ) + CString(":") + strTemp;
				UpdateObjectViews();
			}
			break;
		case StructHeight:
			{
				pUserData->dStructHeight = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent);
				strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pUserData->dStructHeight));
				strTemp = CString("Height (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())\
					+ CString( ")" ) + CString(":") + strTemp;
				UpdateObjectViews();
			}
			break;
		default:
			break;
		}
		m_bPropModified = true;
		m_treeProp.SetItemText(m_hRClickItem,strTemp);
	}
	if (message == WM_INPLACE_EDIT)
	{
		AirsideObjectTreeCtrlItemDataEx* pAoidDataEx = (AirsideObjectTreeCtrlItemDataEx*)m_treeProp.GetItemDataEx(m_hRClickItem);
		if (pAoidDataEx == NULL)
		{
			return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
		}
		CString strValue = _T("");
		strValue = m_treeProp.GetItemText(m_treeProp.GetParentItem(m_hRClickItem));
		int nPos = -1;
		int nValue = -1;

		CString strData = *(CString*)lParam;
		StructFace* pStructData = NULL;
		nPos = strValue.Find(' ');
		if (nPos != -1)
		{
			strValue = strValue.Right(strValue.GetLength()-nPos-1);
			nPos = strValue.Find('-');
			nValue = atoi(strValue.Left(nPos));
			pStructData = ((Structure*)GetObject())->getStructFace(nValue -1);
		}
 		else
 		{
 			int nCount = ((Structure*)GetObject())->GetPath().getCount();
 			pStructData = ((Structure*)GetObject())->getStructFace(nCount);
 		}
		ASSERT(pStructData);
		if ((emEideType)pAoidDataEx->dwptrItemData == SideText )
		{
			pStructData->strFaceName = strData;
			UpdateObjectViews();
		}
 		else
 		{
 			pStructData->strPicPath = strData;
			UpdateObjectViews();
 		}
		m_treeProp.SetItemText(m_hRClickItem,strData);
		m_bPropModified = true;
	}
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideStructureDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hServiceLocation)
	{
		m_bPropModified = true;
		return PICK_MANYPOINTS;
	}
	return PICK_MANYPOINTS;

}
void CAirsideStructureDlg::SetObjectPath(CPath2008& path)
{
	if (m_hServiceLocation == m_hRClickItem)
	{
		((Structure *)GetObject())->clear();
		((Structure *)GetObject())->SetPath(path);
		for(int i = 0; i < path.getCount(); i++)
		{
			UserInputData* pUserData = new UserInputData();
			((Structure *)GetObject())->AddUserItem(pUserData);

			StructFace* pStructData = new StructFace();
			((Structure *)GetObject())->AddStructItem(pStructData);
		}
		StructFace* pStructData = new StructFace();
		((Structure *)GetObject())->AddStructItem(pStructData);
		LoadTree();
		UpdateObjectViews();
	}
}

void CAirsideStructureDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	// Service Location
	CString csLabel = CString("Base vertices (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	m_hServiceLocation = m_treeProp.InsertItem( csLabel );


//	CPoint2008 pt = ((Structure *)GetObject())->GetLocation();
	CPoint2008 ptDefault;
	CPath2008 path = ((Structure *)GetObject())->GetPath();
	for (int i =0; i <path.getCount();++i)
	{
		CPoint2008 point = path.getPoint(i);
		CLatitude fixlatitude;
		CLongitude fixlongitude;
		ConvertPosToLatLong(point,fixlatitude, fixlongitude);

		CString strCorner = _T("");
		strCorner.Format(_T("Corner %d"),i+1);
		HTREEITEM hCorner = m_treeProp.InsertItem(strCorner,m_hServiceLocation);
		CString strPoint = _T("");
		strPoint.Format( "x = %.2f; y = %.2f; z=%.2f",
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ()));

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = NoType;
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

		HTREEITEM hxyz = m_treeProp.InsertItem(strPoint,hCorner);	
		m_treeProp.SetItemDataEx(hxyz,aoidDataEx);

		//lat long
		CString strLL;
		CString strLat,strLong;
		fixlatitude.GetValue(strLat);
		fixlongitude.GetValue(strLong);

		strLL.Format("Latitude = %s ; Longitude = %s",strLat,strLong);

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = NoType;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 		
		isscStringColor.strSection = strLat;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		isscStringColor.strSection = strLong;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		HTREEITEM hFixLL= m_treeProp.InsertItem(strLL,hCorner);
		m_treeProp.SetItemDataEx(hFixLL,aoidDataEx);

		CString strValue = _T("");
		UserInputData* pUserData = ((Structure *)GetObject())->getUserData(i);

		CString strEleva =_T("");
		strEleva.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pUserData->dElevation));
		strValue = CString("Base elevation (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())\
			+ CString( ")" ) + CString(":") + strEleva;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = BaseElevation;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 		
		isscStringColor.strSection = strEleva;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
		
		HTREEITEM hELeva = m_treeProp.InsertItem(strValue,hCorner);
		m_treeProp.SetItemDataEx(hELeva,aoidDataEx);

		CString strHeight =_T("");
		strHeight.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pUserData->dStructHeight));
		strValue = CString("Height (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())\
			+ CString( ")" ) + CString(":") + strHeight;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = StructHeight;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 		
		isscStringColor.strSection = strHeight;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		HTREEITEM hHight = m_treeProp.InsertItem(strValue,hCorner);
		m_treeProp.SetItemDataEx(hHight,aoidDataEx);

		CString strObstruction =_T("");
		strObstruction.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pUserData->dObstruction));
		strValue = CString("Obstruction (") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())\
			+ CString( ")" ) + CString(":") + strObstruction;
		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = Obstruction;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 		
		isscStringColor.strSection = strObstruction;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		HTREEITEM hObstruction = m_treeProp.InsertItem(strValue,hCorner);
		m_treeProp.SetItemDataEx(hObstruction,aoidDataEx);

		m_treeProp.Expand(m_hServiceLocation,TVE_EXPAND);
		m_treeProp.Expand( hCorner, TVE_EXPAND );
	}

	CString strSides = _T("Sides");
	HTREEITEM hSides = m_treeProp.InsertItem(strSides);
	m_hRotation = hSides;
	if (path.getCount() > 0)
	{
		for (i = 0; i < path.getCount()+1; i++)
		{
			if (i != path.getCount())
			{
				CString strSideName = _T("");
				if (i == path.getCount()-1)
				{
					strSideName.Format(_T("Face %d-1"),i+1);
				}
				else
				{
					strSideName.Format(_T("Face %d-%d"),i+1,i+2);
				}
				
				HTREEITEM hName = m_treeProp.InsertItem(strSideName,hSides);
				StructFace* pStructData = ((Structure *)GetObject())->getStructFace(i);
				ASSERT(pStructData);
				aoidDataEx.lSize = sizeof(aoidDataEx);
				aoidDataEx.dwptrItemData = SideText;
				aoidDataEx.vrItemStringSectionColorShow.clear();
				isscStringColor.colorSection = RGB(0,0,255); 
				isscStringColor.strSection = pStructData->strFaceName;
				aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
				HTREEITEM hText = m_treeProp.InsertItem(pStructData->strFaceName,hName);
				m_treeProp.SetItemDataEx(hText,aoidDataEx);

				pStructData = ((Structure *)GetObject())->getStructFace(i);
				ASSERT(pStructData);
				aoidDataEx.lSize = sizeof(aoidDataEx);
				aoidDataEx.dwptrItemData = SidePath;
				aoidDataEx.vrItemStringSectionColorShow.clear();
				isscStringColor.colorSection = RGB(0,0,255); 
				isscStringColor.strSection = pStructData->strPicPath;
				aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
				HTREEITEM hPath = m_treeProp.InsertItem(pStructData->strPicPath,hName);
				m_treeProp.SetItemDataEx(hPath,aoidDataEx);

				m_treeProp.Expand(hName,TVE_EXPAND);
			}
			else
			{
				StructFace* pStructData = ((Structure *)GetObject())->getStructFace(path.getCount());
				ASSERT(pStructData);
				CString strSideName = _T("Top");
				HTREEITEM hName = m_treeProp.InsertItem(strSideName,hSides);
				aoidDataEx.lSize = sizeof(aoidDataEx);
				aoidDataEx.dwptrItemData = SideText;
				aoidDataEx.vrItemStringSectionColorShow.clear();
				isscStringColor.colorSection = RGB(0,0,255); 
				isscStringColor.strSection = pStructData->strFaceName;
				aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
				HTREEITEM hText = m_treeProp.InsertItem(pStructData->strFaceName,hName);
				m_treeProp.SetItemDataEx(hText,aoidDataEx);

				aoidDataEx.lSize = sizeof(aoidDataEx);
				aoidDataEx.dwptrItemData = SidePath;
				aoidDataEx.vrItemStringSectionColorShow.clear();
				isscStringColor.colorSection = RGB(0,0,255); 
				isscStringColor.strSection = pStructData->strPicPath;
				aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
				HTREEITEM hPath = m_treeProp.InsertItem(pStructData->strPicPath,hName);
				m_treeProp.SetItemDataEx(hPath,aoidDataEx);

				m_treeProp.Expand(hName,TVE_EXPAND);
			}
		}
		m_treeProp.Expand(hSides,TVE_EXPAND);
	}
}
void CAirsideStructureDlg::OnRotationEdit()
{	
	m_treeProp.SetDisplayType(2);
	m_treeProp.SetSpinRange(0,360);
//	double dRotation = ((Structure *)GetObject())->GetRotation();
//	m_treeProp.SetDisplayNum(int(dRotation));
	m_treeProp.SpinEditLabel(m_hRClickItem);
}

bool CAirsideStructureDlg::UpdateOtherData()
{
//	CPoint2008 pt = ((Structure *)GetObject())->GetLocation();
	CPoint2008 ptDefault;
// 	if (pt == ptDefault)
// 	{
// 		MessageBox(_T("Please input the structure name."));
// 		return false;
// 	}

	return true;
}

bool CAirsideStructureDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideStructureDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideStructureDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void CAirsideStructureDlg::ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude )
{
	CLatLongConvert convent;
	CLatitude clat;
	CLongitude cLong;
	clat.SetValue(m_AirportInfo.getLatitude());
	cLong.SetValue(m_AirportInfo.getLongtitude());	
	CPoint2008 refPoint = m_AirportInfo.getRefPoint() * SCALE_FACTOR;
	convent.Set_LatLong_Origin(&clat,&cLong,0,0);

	LatLong_Structure convented(&latitude,&longitude);

	float deltaX = (float)(pos.getX() - refPoint.getX());
	float deltaY = (float)(pos.getY() - refPoint.getY());
	deltaX = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaX);
	deltaY = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaY);

	convent.Point_LatLong(deltaX,deltaY,&convented);
}
void CAirsideStructureDlg::ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos )
{
	CLatLongConvert convent;
	CLatitude clat;
	CLongitude cLong;
	clat.SetValue(m_AirportInfo.getLatitude());
	cLong.SetValue(m_AirportInfo.getLongtitude());

	CPoint2008 refPoint = m_AirportInfo.getRefPoint() * SCALE_FACTOR;
	convent.Set_LatLong_Origin(&clat,&cLong,refPoint.getX(),refPoint.getY());

	LatLong_Structure Origin(&clat, &cLong);
	LatLong_Structure Convented((CLatitude*)&latitude,(CLongitude*)&longtitude);

	float xdist,ydist,dist;
	convent.Distance_LatLong(&Convented,xdist,ydist,dist);
	xdist = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_NAUTICALMILE,AU_LENGTH_CENTIMETER,xdist);
	ydist = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_NAUTICALMILE,AU_LENGTH_CENTIMETER,ydist);

	pos.setPoint(refPoint.getX() + xdist,refPoint.getY()+ydist,pos.getZ());

}

void CAirsideStructureDlg::OnShowSides()
{
	if (m_hRClickItem == m_hRotation)
	{
		CPath2008 path = ((Structure*)GetObject())->GetPath();
		if (path.getCount() > 0)
		{
			for (int i = 0; i < path.getCount()+1; i++)
			{
				((Structure*)GetObject())->getStructFace(i)->bShow = m_bShow;
				((Structure*)GetObject())->UpdateObject(((Structure*)GetObject())->getID());
				UpdateObjectViews();
			}
		}
	}
	else
	{
		CString strValue = _T("");
		strValue = m_treeProp.GetItemText(m_hRClickItem);
		int nPos = -1;
		int nValue = -1;

		StructFace* pStructData = NULL;
		nPos = strValue.Find(' ');
		if (nPos != -1)
		{
			nValue = atoi(strValue.Right(strValue.GetLength()-nPos-1));
			pStructData = ((Structure*)GetObject())->getStructFace(nValue -1);

			pStructData->bShow = !(pStructData->bShow);
			((Structure*)GetObject())->UpdateObject(((Structure*)GetObject())->getID());
			UpdateObjectViews();
		}
		else
		{
			CPath2008 path = ((Structure*)GetObject())->GetPath();
			pStructData = ((Structure*)GetObject())->getStructFace(path.getCount());

			pStructData->bShow = !(pStructData->bShow);
			((Structure*)GetObject())->UpdateObject(((Structure*)GetObject())->getID());
			UpdateObjectViews();
		}
	}
}




















