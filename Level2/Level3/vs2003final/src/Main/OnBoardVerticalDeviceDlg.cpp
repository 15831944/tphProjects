#include "StdAfx.h"
#include "Resource.h"
#include ".\onboardverticaldevicedlg.h"
#include "../InputOnboard/OnBoardBaseVerticalDevice.h"
#include "../Common/WinMsg.h"
#include "common/ARCUnit.h"
#include "InputOnBoard/AircaftLayOut.h"
#include "Common\ARCStringConvert.h"
#include "InputOnBoard\AircraftPlacements.h"
#include "ChangeVerticalProfileDlg.h"
#include "../Common/UnitsManager.h"
#include "../InputOnboard/Deck.h"
#include "../InputOnboard/AircaftLayOut.h"


IMPLEMENT_DYNAMIC(COnBoardVerticalDeviceDlg, COnBoardObjectBaseDlg)
COnBoardVerticalDeviceDlg::COnBoardVerticalDeviceDlg( COnBoardBaseVerticalDevice *pVerticalDevice,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent /*= NULL*/ )
:COnBoardObjectBaseDlg(pVerticalDevice,pParent)
{
	m_nProjID = nProjID;
	m_pLayout = pLayout;

	m_hItemServiceLocation = NULL;
	m_hItemQueue = NULL;
	m_hItemInCons = NULL;
	m_hItemOutCons = NULL;
}
COnBoardVerticalDeviceDlg::~COnBoardVerticalDeviceDlg(void)
{
}


void COnBoardVerticalDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	COnBoardObjectBaseDlg::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(COnBoardVerticalDeviceDlg,COnBoardObjectBaseDlg)


END_MESSAGE_MAP()

BOOL COnBoardVerticalDeviceDlg::OnInitDialog()
{
	COnBoardObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SEAT));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	//show row and column static
	GetDlgItem(IDC_STATIC_ROW)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_COLUMN)->ShowWindow(SW_HIDE);
	CString strType;
	if(m_pObject)
	{

		COnBoardBaseVerticalDevice *pVerticalDevice = (COnBoardBaseVerticalDevice *)m_pObject;
		if(pVerticalDevice)
		{
			if(pVerticalDevice->GetOriginDeck() == NULL)
			{
				pVerticalDevice->SetOriginDeck(m_pLayout->GetDeckManager()->GetActivedDeck());
			}
			if(pVerticalDevice->GetDestDeck() == NULL)
			{
				pVerticalDevice->SetDestDeck(m_pLayout->GetDeckManager()->GetActivedDeck());
			}

		}

		strType = m_pObject->GetType();
	}
	if (m_pObject->GetID() != -1)
	{
		SetWindowText(_T("Modify ") + strType);
	}
	else
	{
		SetWindowText(_T("Define ") + strType);
		
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


void COnBoardVerticalDeviceDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{

}
void COnBoardVerticalDeviceDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem==m_hItemServiceLocation)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_DELETE ,MF_BYCOMMAND);

		const CPath2008& pathService = ((COnBoardBaseVerticalDevice *)m_pObject)->GetPathService();
		//if(pathService.getCount() == 2)
		{
			pMenu->EnableMenuItem( ID_PROCPROP_DEFINE_Z_POS ,MF_ENABLED );
		}

	}
	else if(m_hRClickItem == m_hItemQueue)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	


	}
	else if (m_hRClickItem == m_hItemInCons || m_hRClickItem == m_hItemOutCons)
	{
		pMenu=menuPopup.GetSubMenu(5);
		pMenu->DeleteMenu( 3 ,MF_BYPOSITION);
		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);	
		pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);	
	}

}

LRESULT COnBoardVerticalDeviceDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		

	return COnBoardObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}
FallbackReason COnBoardVerticalDeviceDlg::GetFallBackReason()
{
	if (m_hRClickItem == m_hItemServiceLocation)
	{
		if(m_pObject->GetType() == COnBoardStair::TypeString)
		{
			return PICK_MANYPOINTS;		
		}
		if(m_pObject->GetType() == COnBoardElevator::TypeString)
		{
			return PICK_ONEPOINT;
		}
		if(m_pObject->GetType() == COnBoardEscalator::TypeString)
		{
			return PICK_TWOPOINTS;
		}
	}
	
	else if (m_hRClickItem == m_hItemInCons || m_hRClickItem == m_hItemOutCons || m_hRClickItem == m_hItemQueue)
	{
		return PICK_MANYPOINTS;
	}
	return PICK_ONEPOINT;

}
void COnBoardVerticalDeviceDlg::SetObjectPath(CPath2008& path)
{
	if (m_hItemServiceLocation == m_hRClickItem)
	{
		if(path.getCount() > 0)
			((COnBoardBaseVerticalDevice *)m_pObject)->SetPathService(path);

		m_bPathModified = true;

		UpdateObjectViews();
		LoadTree();
	}
	else  if (m_hRClickItem == m_hItemInCons)
	{
		if(path.getCount() > 0)
			((COnBoardBaseVerticalDevice *)m_pObject)->SetPathInCons(path);

		m_bPathModified = true;

		UpdateObjectViews();
		LoadTree();
	}
	else if(m_hRClickItem == m_hItemOutCons )
	{
		if(path.getCount() > 0)
		{
			//set the out constrain path

			CDeck *pDestDeck = ((COnBoardBaseVerticalDevice *)m_pObject)->GetDestDeck();
			if(pDestDeck)
			{
				double dDestAlt = pDestDeck->GetGroundVal();
				
				int nPathCount = path.getCount();
				for (int nPath = 0; nPath < nPathCount; ++nPath)
				{
					path[nPath].setZ(dDestAlt);
				}
			}

			((COnBoardBaseVerticalDevice *)m_pObject)->SetPathOutCons(path);
		}
		m_bPathModified = true;

		UpdateObjectViews();
		LoadTree();
	}
	else if(m_hRClickItem == m_hItemQueue)
	{
		if(path.getCount() > 0)
			((COnBoardBaseVerticalDevice *)m_pObject)->SetPathQueue(path);

		m_bPathModified = true;

		UpdateObjectViews();
		LoadTree();
	}

}

void COnBoardVerticalDeviceDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	COnBoardBaseVerticalDevice *pVerticalDevice = (COnBoardBaseVerticalDevice *)m_pObject;
	ASSERT(pVerticalDevice != NULL);
	if(pVerticalDevice == NULL)
		return;

	
	//position
	CString strPositionLabel =_T("Servive Location(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemServiceLocation = m_treeProp.InsertItem(strPositionLabel);
	
	CString strOriginDeckName = _T("");
	if(pVerticalDevice->GetOriginDeck())
		 strOriginDeckName =pVerticalDevice->GetOriginDeck()->GetName();

	//const CPath2008& pathService = pVerticalDevice->GetPathService();
	//InsertPathToTree(pathService,m_hItemServiceLocation,strOriginDeckName);
	InsertServicePathToTree(m_hItemServiceLocation,pVerticalDevice);
	m_treeProp.Expand(m_hItemServiceLocation,TVE_EXPAND);



	CString strQueueLabel =_T("Queue Points(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemQueue = m_treeProp.InsertItem(strQueueLabel);
	const CPath2008& pathQueue = pVerticalDevice->GetPathQueue();
	InsertPathToTree(pathQueue,m_hItemQueue,strOriginDeckName);

	if(pVerticalDevice->GetQueueFixed() == true)
	{
		m_treeProp.InsertItem(_T("Fixed"),m_hItemQueue);
	}
	else
	{
		m_treeProp.InsertItem(_T("Non Fixed"),m_hItemQueue);
	}
	if(pathQueue.getCount() > 0)
		m_treeProp.Expand(m_hItemQueue,TVE_EXPAND);
	

	CString strInConsLabel =_T("In Constraint(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemInCons = m_treeProp.InsertItem(strInConsLabel);
	const CPath2008& pathIncons= pVerticalDevice->GetPathInCons();
	InsertPathToTree(pathIncons,m_hItemInCons,strOriginDeckName);
	m_treeProp.Expand(m_hItemInCons,TVE_EXPAND);


	CString strDestDeckName = _T("");
	if(pVerticalDevice->GetDestDeck())
		strDestDeckName =pVerticalDevice->GetDestDeck()->GetName();

	CString strOutConsLabel =_T("Out Constraint(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + _T(")");
	m_hItemOutCons = m_treeProp.InsertItem(strOutConsLabel);
	const CPath2008& pathOutcons= pVerticalDevice->PathOutCons();
	InsertPathToTree(pathOutcons,m_hItemOutCons,strDestDeckName);
	m_treeProp.Expand(m_hItemOutCons,TVE_EXPAND);


}

void COnBoardVerticalDeviceDlg::OnOK()
{
	if(SetObjectName() == false)
		return;

	//m_pObject->SetIDName(m_objName);

	//if(m_pObject->GetIDName().m_val == m_objName.m_val)
	//{
	//	m_bNameModified = false;
	//}

	COnBoardBaseVerticalDevice *pVerticalDevice = (COnBoardBaseVerticalDevice *)m_pObject;
	if(pVerticalDevice)
	{
		if(pVerticalDevice->GetOriginDeck() == NULL || pVerticalDevice->GetDestDeck() == NULL)
		{
			MessageBox(_T("The object has no deck information."));
			return ;
		}

	}

	CADODatabase::BeginTransaction();
	try
	{
		((COnBoardBaseVerticalDevice *)m_pObject)->SetLayOut(m_pLayout);
		((COnBoardBaseVerticalDevice *)m_pObject)->SaveData(m_pLayout->GetID());
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

bool COnBoardVerticalDeviceDlg::UpdateOtherData()
{

	return true;
}

bool COnBoardVerticalDeviceDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool COnBoardVerticalDeviceDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool COnBoardVerticalDeviceDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}

void COnBoardVerticalDeviceDlg::InsertPointToTree( const CPoint2008& point, HTREEITEM hParentItem,const CString& strDeckName)
{
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = NULL;


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

	//strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ()));		
	isscStringColor.strSection = strDeckName;//strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);



	CString strPoint = _T("");
	strPoint.Format( "x = %.2f; y = %.2f; %s",
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getX()),
		CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getY()),
		strDeckName
		/*CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),point.getZ())*/);

	hItemTemp = m_treeProp.InsertItem(strPoint,hParentItem);
	m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);

}

void COnBoardVerticalDeviceDlg::InsertPathToTree( const CPath2008& path, HTREEITEM hParentItem, const CString& strDeckName )
{
	for (int i =0; i < path.getCount();++i)
	{
		CPoint2008 point = path.getPoint(i);
		InsertPointToTree(point,hParentItem,strDeckName);
	}
}

void COnBoardVerticalDeviceDlg::ToggleQueueFixed()
{
	if(m_hRClickItem == m_hItemQueue)
	{
		bool bQueueFixed = ((COnBoardBaseVerticalDevice *)m_pObject)->GetQueueFixed();
		((COnBoardBaseVerticalDevice *)m_pObject)->SetQueueFixed(!bQueueFixed);

		m_bPathModified = true;
		LoadTree();
	}
}

void COnBoardVerticalDeviceDlg::DefineZPos()
{
	if(m_pLayout == NULL)
		return;

	std::vector<double>vDeckAltitudes;
	
	std::vector<CDeck *> vSortedDeck =  m_pLayout->GetDeckManager()->getSortedDecks();
	for (int nDeck = 0; nDeck < static_cast<int>(vSortedDeck.size()); ++ nDeck)
	{
		double dDeckAltitudes = vSortedDeck.at(nDeck)->GetGroundVal();
		vDeckAltitudes.push_back(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,dDeckAltitudes));
	}

	std::vector<double>vXPos;
	std::vector<double>vZPos;
	Path _tmpPath;

	const CPath2008& pPathService = ((COnBoardBaseVerticalDevice *)m_pObject)->GetPathService();
	CPoint2008* ptList = pPathService.getPointList();
	int ptCount = pPathService.getCount();
	if(ptCount>0) 
	{
		vXPos.reserve(ptCount);
		vZPos.reserve(ptCount);
		vXPos.push_back(0.0);
		vZPos.push_back(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,ptList[0].getZ()) );
		double dLenSoFar = 0.0;
		for( int i=1; i<ptCount; i++ )
		{			
			ARCVector3 v3D(ptList[i].getX()-ptList[i-1].getX(), ptList[i].getY()-ptList[i-1].getY(), 0.0);
			dLenSoFar +=  CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,v3D.Magnitude());
			vXPos.push_back(dLenSoFar);				
			vZPos.push_back( CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,ptList[i].getZ()));
		}
	}

	// init temp path
	_tmpPath = pPathService;

	_tmpPath.init(pPathService.getCount());
	for (int nTmpPath = 0; nTmpPath < _tmpPath.getCount(); ++ nTmpPath)
	{
		_tmpPath[nTmpPath].SetPoint(Point(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,pPathService.getPoint(nTmpPath).getX()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,pPathService.getPoint(nTmpPath).getY()),
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,pPathService.getPoint(nTmpPath).getZ())));
	}

	CStringArray strArrFloorName;
	strArrFloorName.SetSize(vSortedDeck.size());
	for(int k=0;k<static_cast<int>(vSortedDeck.size());k++)
	{
		strArrFloorName[k]=vSortedDeck.at(k)->GetName();
	}
	CChangeVerticalProfileDlg dlg(vXPos, vZPos, vDeckAltitudes, _tmpPath ,&strArrFloorName,EnvMode_OnBoard,this);

	if( dlg.DoModal() == IDOK )
	{
		vZPos = dlg.m_wndPtLineChart.m_vPointYPos;
		CPath2008 pathServiceTmp(pPathService);

		for( int i=0; i<static_cast<int>(vZPos.size()); i++ )
		{
			pathServiceTmp[ i ].setZ(CARCLengthUnit::ConvertLength(AU_LENGTH_METER,DEFAULT_DATABASE_LENGTH_UNIT,vZPos[i]));
		}
		((COnBoardBaseVerticalDevice *)m_pObject)->SetPathService(pathServiceTmp);
		//out constraint
		const CPath2008& pathOutCons = ((COnBoardBaseVerticalDevice *)m_pObject)->PathOutCons();

		CPath2008 pathOutConsTmp(pathOutCons);
		for (int nOut = 0; nOut < pathOutConsTmp.getCount();++nOut)
		{
			pathOutConsTmp[nOut].setZ(CARCLengthUnit::ConvertLength(AU_LENGTH_METER,DEFAULT_DATABASE_LENGTH_UNIT,vZPos[vZPos.size()-1]));
		}
		((COnBoardBaseVerticalDevice *)m_pObject)->SetPathOutCons(pathOutConsTmp);
		

		//set object origin deck and dest deck
		
		int nPathServiceCount = pPathService.getCount();
		if(nPathServiceCount > 0)
		{
			CPoint2008 pointDest = pPathService.getPoint(nPathServiceCount -1);

			
			std::vector<CDeck *> vSortedDeck =  m_pLayout->GetDeckManager()->getSortedDecks();
			for (int nDeck = 0; nDeck < static_cast<int>(vSortedDeck.size()); ++ nDeck)
			{
				double dDeckAltitudes = vSortedDeck.at(nDeck)->GetGroundVal();
				if(abs(dDeckAltitudes - pointDest.getZ()) < 0.00001)//miscalculate less than 0.1
				{	
					((COnBoardBaseVerticalDevice *)m_pObject)->SetDestDeck(vSortedDeck.at(nDeck));
					break;
				}
			}
		}
		//refresh Tree
		LoadTree();
	}
}

void COnBoardVerticalDeviceDlg::OnDeletePath()
{
	if (m_hItemServiceLocation == m_hRClickItem)
	{

	}
	else  if (m_hRClickItem == m_hItemInCons)
	{
		CPath2008 emptyPath;
		((COnBoardBaseVerticalDevice *)m_pObject)->SetPathInCons(emptyPath);

		m_bPathModified = true;

		HTREEITEM hChildItem = m_treeProp.GetChildItem(m_hItemInCons);
		while( hChildItem != NULL)
		{
			m_treeProp.DeleteItem(hChildItem);
			hChildItem = m_treeProp.GetChildItem(m_hItemInCons);
		}

	}
	else if(m_hRClickItem == m_hItemOutCons )
	{
		CPath2008 emptyPath;
		((COnBoardBaseVerticalDevice *)m_pObject)->SetPathOutCons(emptyPath);

		m_bPathModified = true;

		HTREEITEM hChildItem = m_treeProp.GetChildItem(m_hItemOutCons);
		while( hChildItem != NULL)
		{
			m_treeProp.DeleteItem(hChildItem);
			hChildItem = m_treeProp.GetChildItem(m_hItemOutCons);
		}

	}
	else if(m_hRClickItem == m_hItemQueue)
	{	
		CPath2008 emptyPath;

		((COnBoardBaseVerticalDevice *)m_pObject)->SetPathQueue(emptyPath);

		m_bPathModified = true;
		
		HTREEITEM hChildItem = m_treeProp.GetChildItem(m_hItemQueue);
		while( hChildItem != NULL)
		{
			m_treeProp.DeleteItem(hChildItem);
			hChildItem = m_treeProp.GetChildItem(m_hItemQueue);
		}

	}
}

void COnBoardVerticalDeviceDlg::InsertServicePathToTree(HTREEITEM hParentItem,COnBoardBaseVerticalDevice *pVerticalDevice )
{

	if(pVerticalDevice == NULL)
		return;

	CString strOriginDeckName = _T("");
	if(pVerticalDevice->GetOriginDeck())
		strOriginDeckName = pVerticalDevice->GetOriginDeck()->GetName();

	CString strDestDeckName = _T("");
	if(pVerticalDevice->GetDestDeck())
		strDestDeckName = pVerticalDevice->GetDestDeck()->GetName();

	const CPath2008& pathService = pVerticalDevice->GetPathService();
	for (int i =0; i < pathService.getCount();++i)
	{
		CPoint2008 point = pathService.getPoint(i);
		if(i == pathService.getCount() - 1)//last one
		{
			InsertPointToTree(point,hParentItem,strDestDeckName);
		}
		else
		{
			InsertPointToTree(point,hParentItem,strOriginDeckName);
		}
	}



}