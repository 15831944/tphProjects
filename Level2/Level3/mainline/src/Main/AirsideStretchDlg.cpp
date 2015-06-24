#include "StdAfx.h"
#include "resource.h"
#include "TermPlanDoc.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsidestretchdlg.h"
#include "../common\WinMsg.h"
#include "../InputAirside/StretchSetOption.h"
#include "../Common/ARCUnit.h"
#include "../Common/UnitsManager.h"
#include "AirsideStretchVerticalProfileEditor.h"


IMPLEMENT_DYNAMIC(CAirsideStretchDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(CAirsideStretchDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_ALTOBJECTVERTICAL_EDITVERTICALPROFILE,OnEditVerticalProfile)
	ON_COMMAND(ID_ALTOBJECTNUMBER_DEFINENUMBER,OnDefineNumber)
	ON_COMMAND(ID_ALTOBJECTWIDTH_DEFINEWIDTH,OnDefineWidth)
	ON_COMMAND(ID_SELECT_STRETCH_DIRECT,OnSelectStretchDirect)
	ON_COMMAND(ID_SELECT_STRETCH_CIRCLE,OnSelectStretchCircle)
END_MESSAGE_MAP()
CAirsideStretchDlg::CAirsideStretchDlg(int nStretchID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nStretchID,nAirportID,nProjID,pParent),m_vLevelList(nAirportID)
{
	if (nStretchID!=-1)
	{
		m_pObject = ALTObject::ReadObjectByID(nStretchID);
	}
	else
	{
		m_pObject =  new Stretch();
		m_pObject->setID(nStretchID);
		m_pObject->setAptID(nAirportID);
	}

	m_nStretchID = nStretchID;
	m_nAirportID = nAirportID;
	m_bPathModified = false;
	m_bTypeModify = false;

	CStretchSetOption m_stretchSetOption;
	m_stretchSetOption.ReadData(nProjID);
	m_iLaneNum = m_stretchSetOption.GetLaneNumber();
	m_dLaneWidth = ARCUnit::ConvertLength( m_stretchSetOption.GetLaneWidth(), ARCUnit::M, ARCUnit::CM) ;

	
}

//CAirsideStretchDlg::CAirsideStretchDlg(Stretch* pStretch,int nProjID,CWnd * pParent /*= NULL*/ )
//: CAirsideObjectBaseDlg(pStretch,nProjID,pParent),m_vLevelList(pStretch?pStretch->getAptID():-1)
//{
//	if(pStretch)
//		m_path = pStretch->GetPath();
//	m_bPathModified = false;
//}
//
CAirsideStretchDlg::~CAirsideStretchDlg(void)
{
	m_path.clear();
}
void CAirsideStretchDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}
BOOL CAirsideStretchDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap */0);

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Stretch"));
	}
	else
	{
		SetWindowText(_T("Define Stretch"));
	}
	

	if(GetObject()->getID() != -1)
	{
		if (GetObject()->GetType() == ALT_STRETCH)
		{
			((Stretch *)GetObject())->ReadObject(GetObject()->getID());
			m_path = ((Stretch *)GetObject())->GetPath();
			m_dLaneWidth   = ((Stretch *)GetObject())->GetLaneWidth();
			m_iLaneNum     = ((Stretch *)GetObject())->GetLaneNumber();
		}
		else if (GetObject()->GetType() == ALT_CIRCLESTRETCH)
		{
			((CircleStretch *)GetObject())->ReadObject(GetObject()->getID());
			m_path = ((CircleStretch *)GetObject())->GetPath();
			m_dLaneWidth   = ((CircleStretch *)GetObject())->GetLaneWidth();
			m_iLaneNum     = ((CircleStretch *)GetObject())->GetLaneNumber();
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

	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAirsideStretchDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hLaneNumber)
	{
		m_hRClickItem = hTreeItem;
		OnDefineNumber();
	}

	if(hTreeItem == m_hLaneWidth)
	{
		m_hRClickItem = hTreeItem;
		OnDefineWidth();
	}

}

void CAirsideStretchDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hCtrlPoints)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else if (m_hRClickItem == m_hVerticalProfile)
	{
		pMenu = menuPopup.GetSubMenu( 74 );
	}
	else if (m_hRClickItem == m_hLaneNumber)
	{
		pMenu = menuPopup.GetSubMenu(75);
	}
	else if (m_hRClickItem == m_hLaneWidth)
	{
		pMenu = menuPopup.GetSubMenu(76);
	}
	else if (m_hRClickItem == m_hStretchSort)
	{
		pMenu = menuPopup.GetSubMenu(87);
	}
}


LRESULT CAirsideStretchDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	if( message == WM_INPLACE_SPIN && GetObject())
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (m_hRClickItem == m_hLaneWidth)
		{			
			m_dLaneWidth = ( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			if(m_dLaneWidth <= 0)
				m_dLaneWidth = 1.0;
			m_bPropModified = true;
			if (GetObject()->GetType() == ALT_STRETCH)
			{
				((Stretch *)GetObject())->SetLaneWidth(m_dLaneWidth);
			}
			else if(GetObject()->GetType() == ALT_CIRCLESTRETCH)
			{
				((CircleStretch*)GetObject())->SetLaneWidth(m_dLaneWidth);
			}

		}
		else if (m_hRClickItem == m_hLaneNumber)
		{
			m_iLaneNum = (pst->iPercent);		
			if(m_iLaneNum <= 0)
				m_iLaneNum = 1;
			m_bPropModified = true;
			if (GetObject()->GetType() == ALT_STRETCH)
			{
				((Stretch *)GetObject())->SetLaneNumber(m_iLaneNum);
			}
			else if(GetObject()->GetType() == ALT_CIRCLESTRETCH)
			{
				((CircleStretch*)GetObject())->SetLaneNumber(m_iLaneNum);
			}

		}
		UpdateObjectViews();
		LoadTree();
		return TRUE;
	}
	//else
	//{
	//	HTREEITEM hItem=(HTREEITEM)wParam;
	//	if (hItem == m_hSortDirect)
	//	{
	//		m_bSortDes = (m_coolTree.IsCheckItem(hItem) == 1)? TRUE:FALSE;
	//	}
	//	else if (hItem == m_hSortCircle)
	//	{
	//		m_bSortDes = (m_coolTree.IsCheckItem(hItem) == 1)? FALSE:TRUE;
	//	}
	//}

	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideStretchDlg::GetFallBackReason(void)
{	
	return PICK_MANYPOINTS;
}
void CAirsideStretchDlg::SetObjectPath(CPath2008& path)
{
	if (GetObject()->GetType()==ALT_STRETCH)
	{
		((Stretch *)GetObject())->SetPath(path);
		((Stretch *)GetObject())->SetLaneWidth(m_dLaneWidth);
		((Stretch *)GetObject())->SetLaneNumber(m_iLaneNum);
	}
	else if(GetObject()->GetType()==ALT_CIRCLESTRETCH)
	{
		((CircleStretch *)GetObject())->SetPath(path);
		((CircleStretch *)GetObject())->SetLaneWidth(m_dLaneWidth);
		((CircleStretch *)GetObject())->SetLaneNumber(m_iLaneNum);
	}

	m_path = path;
	m_bPathModified = true;
	UpdateObjectViews();
	LoadTree();
}

void CAirsideStretchDlg::LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path )
{
	m_vLevelList.ReadFullLevelList(); 

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	int iPathCount = path.getCount();
	for(int i = 0;i < iPathCount; i++)
	{
		CString csPoint;
		double FloorAlt = path.getPoint(i).getZ()/SCALE_FACTOR;//floors.getVisibleFloorAltitude(  ) ;

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),path.getPoint(i).getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),path.getPoint(i).getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),FloorAlt);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getX() ), \
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).getY() ), FloorAlt);
		hItemTemp = m_treeProp.InsertItem( csPoint, preItem  );
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}	
}

void CAirsideStretchDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	// control points.
	strLabel = CString( "Control Point(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );
	
	m_hCtrlPoints = m_treeProp.InsertItem(strLabel);
	LoadTreeSubItemCtrlPoints(m_hCtrlPoints,m_path);	
	m_treeProp.Expand( m_hCtrlPoints, TVE_EXPAND );

	//vertical profile.
	strLabel = _T("Vertical Profile");
	m_hVerticalProfile = m_treeProp.InsertItem(strLabel);

	// stretch sort
	//COOLTREE_NODE_INFO cni;
	//CCoolTree::InitNodeInfo(this,cni);
	//m_hStretchSort=m_coolTree.InsertItem("Stretch",cni,FALSE);

	//if (m_bSortDes == TRUE)
	//{
	//	cni.nt=NT_CHECKBOX;
	//	cni.net = NET_NORMAL;
	//	m_hSortDirect=m_coolTree.InsertItem("Direct",cni,FALSE,FALSE,m_hStretchSort);

	//	cni.nt=NT_CHECKBOX; 
	//	cni.net = NET_NORMAL;
	//	m_hSortCircle=m_coolTree.InsertItem("Circle",cni,TRUE,FALSE,m_hStretchSort);
	//}
	//else
	//{
	//	cni.nt=NT_CHECKBOX;
	//	cni.net = NET_NORMAL;
	//	m_hSortDirect=m_coolTree.InsertItem("Direct",cni,TRUE,FALSE,m_hStretchSort);

	//	cni.nt=NT_CHECKBOX; 
	//	cni.net = NET_NORMAL;
	//	m_hSortCircle=m_coolTree.InsertItem("Circle",cni,FALSE,FALSE,m_hStretchSort);
	//}
 //   m_coolTree.Expand(m_hStretchSort,TVE_EXPAND);
	//m_coolTree.SelectItem(m_hStretchSort);
	//m_coolTree.SelectSetFirstVisible(m_hStretchSort);

	CString strSelect = _T("");
	if (GetObject()->GetType() == ALT_STRETCH)
	{
		strSelect.Format(_T("Direct"));
	}
	else if(GetObject()->GetType() == ALT_CIRCLESTRETCH)
	{
		strSelect.Format(_T("Circle"));
	}
	strLabel.Format(_T("Stretch Sorts : %s"),strSelect);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%s"),strSelect);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hStretchSort = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hStretchSort,aoidDataEx);

	//number of lanes.
	strLabel.Format(_T("Number of Lanes : %d"), m_iLaneNum );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%d"),m_iLaneNum);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLaneNumber = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLaneNumber,aoidDataEx);

	//lane width.
	strLabel.Format(_T("Lane Width: %.2f (%s) "),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dLaneWidth) , CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())  );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dLaneWidth));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLaneWidth = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLaneWidth,aoidDataEx);

    m_treeProp.SetRedraw(1);
}


void CAirsideStretchDlg::OnOK(void)
{
	if(GetObject())
	{
//		CPath2008 path = ((Stretch *)GetObject())->GetPath();
		if (m_path.getCount()<=2&&GetObject()->GetType()==ALT_CIRCLESTRETCH || m_path.getCount()<2 )
		{
			MessageBox(_T("Please pick more points of the stretch path from the map."));
			return ;
		}	
		if (GetObject()->GetType() == ALT_STRETCH)
		{
			((Stretch *)GetObject())->SetPath(m_path);
			((Stretch *)GetObject())->SetLaneWidth(m_dLaneWidth);
			((Stretch *)GetObject())->SetLaneNumber(m_iLaneNum);
		}
		else if(GetObject()->GetType() == ALT_CIRCLESTRETCH)
		{
			((CircleStretch *)GetObject())->SetPath(m_path);
			((CircleStretch *)GetObject())->SetLaneWidth(m_dLaneWidth);
			((CircleStretch *)GetObject())->SetLaneNumber(m_iLaneNum);
		}

		/*((Stretch *)GetObject())->DeleteObject(m_nObjID);*/
		/*if(GetObject()->getID() != -1)
			((Stretch *)GetObject())->UpdateObject(GetObject()->getID());
		else
			((Stretch *)GetObject())->SaveObject(m_nAirportID);*/
	}

	CAirsideObjectBaseDlg::OnOK();
    UpdateAddandRemoveObject();
}

bool CAirsideStretchDlg::UpdateOtherData(void)
{
//	CPath2008 path = ((Stretch *)GetObject())->GetPath();
	if (m_path.getCount() ==0)
	{
		MessageBox(_T("Please pick the stretch path from the map."));
		return (false);
	}
	try
	{
		if (m_bPathModified && m_nObjID != -1)
		{
			if (GetObject()->GetType() == ALT_STRETCH)
			{
				((Stretch *)GetObject())->UpdateObject(m_nObjID);
			}
			else if(GetObject()->GetType() == ALT_CIRCLESTRETCH)
			{
				((CircleStretch*)GetObject())->UpdateObject(m_nObjID);
			}
		}

	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't update the Object path."));
		return (false);
	}

	return (true);
}

void CAirsideStretchDlg::OnEditVerticalProfile(void)
{
	std::vector<double>vXYPos;
	std::vector<double>vZPos;

	if( m_path.getCount() < 2)
	{
		return;
	}

	vXYPos.reserve(m_path.getCount());
	vZPos.reserve(m_path.getCount());		
	vXYPos.push_back(0.0); 
	vZPos.push_back( m_path.getPoint(0).getZ() /SCALE_FACTOR);
	
	double dLensoFar = 0.0;
	
	for(int i=1;i<m_path.getCount();i++)
	{
		ARCVector3 v3D(m_path[i].getX()-m_path[i-1].getX(), m_path[i].getY()-m_path[i-1].getY(), 0.0);
		dLensoFar += UNITSMANAGER->ConvertLength(v3D.Magnitude());
		vXYPos.push_back(dLensoFar);				
		vZPos.push_back( m_path[i].getZ() /SCALE_FACTOR );
	}

	CAirsideStretchVerticalProfileEditor dlg(vXYPos,vZPos, m_path);

	if( dlg.DoModal() == IDOK )
	{	
		ASSERT(m_path.getCount() == (int) vZPos.size());
		vZPos = dlg.m_wndPtLineChart.m_vPointYPos;

		CPoint2008* pPointList = m_path.getPointList();
		ASSERT(NULL != pPointList);
		for(int i=0; i<m_path.getCount(); i++)
		{
			pPointList[i].setZ(vZPos[i] * SCALE_FACTOR);
			//m_vCtrlPoints[i][VZ] = vZPos[i] * SCALE_FACTOR ;//pDoc->GetLandsideDoc()->GetLevels().getVisibleAltitude( vZPos[i] * SCALE_FACTOR ) ;
			m_bPropModified = TRUE;
		}

		LoadTree();
	}
}

void CAirsideStretchDlg::OnDefineNumber(void)
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum(m_iLaneNum);
	m_treeProp.SetSpinRange( 1,40);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideStretchDlg::OnDefineWidth(void)
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dLaneWidth)) );
	m_treeProp.SetSpinRange( 1,360);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CAirsideStretchDlg::OnSelectStretchDirect(void)
{
	m_bSortDes = TRUE;
	if (GetObject()->GetType() == ALT_CIRCLESTRETCH)
	{
		m_bTypeModify = TRUE;
		if (GetObject()->getID()!=-1)
		{
			((CircleStretch*)GetObject())->DeleteObject(GetObject()->getID());
			UpdateAddandRemoveObject();
		}

		m_nObjID = -1;
		m_pObject = new Stretch();
		m_pObject->setID(-1);
		m_pObject->setAptID(m_nAirportID);
		((Stretch *)GetObject())->SetPath(m_path);
		((Stretch *)GetObject())->SetLaneWidth(m_dLaneWidth);
		((Stretch *)GetObject())->SetLaneNumber(m_iLaneNum);
	}

	LoadTree();
}

void CAirsideStretchDlg::OnSelectStretchCircle(void)
{
	m_bSortDes = FALSE;
	if (GetObject()->GetType() == ALT_STRETCH)
	{
		m_bTypeModify = TRUE;
		if (GetObject()->getID()!=-1)
		{
			((Stretch*)GetObject())->DeleteObject(GetObject()->getID());
			UpdateAddandRemoveObject();
		}

		m_nObjID = -1;
	   	m_pObject = new CircleStretch();
		m_pObject->setID(-1);
		m_pObject->setAptID(m_nAirportID);
		((CircleStretch *)GetObject())->SetPath(m_path);
		((CircleStretch *)GetObject())->SetLaneWidth(m_dLaneWidth);
		((CircleStretch *)GetObject())->SetLaneNumber(m_iLaneNum);
	}

	LoadTree();
}

bool CAirsideStretchDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideStretchDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideStretchDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}
