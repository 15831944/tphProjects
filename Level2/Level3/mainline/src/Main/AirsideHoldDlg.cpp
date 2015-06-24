#include "StdAfx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include ".\airsideholddlg.h"
#include "../InputAirside/AirHold.h"
#include "../Common/WinMsg.h"

IMPLEMENT_DYNAMIC(CAirsideHoldDlg, CAirsideObjectBaseDlg)
CAirsideHoldDlg::CAirsideHoldDlg(int nHoldID,int nAirportID,int nProjID,CWnd* pParent):
CAirsideObjectBaseDlg(nHoldID,nAirportID, nProjID,pParent)
{
	m_pObject = new AirHold;
}

//CAirsideHoldDlg::CAirsideHoldDlg( AirHold * phold ,int nProjID,CWnd * pParent /*= NULL*/ ): CAirsideObjectBaseDlg(phold, nProjID,pParent)
//{
//
//}
CAirsideHoldDlg::~CAirsideHoldDlg(void)
{
	//delete m_pObject;
}
BEGIN_MESSAGE_MAP(CAirsideHoldDlg,CAirsideObjectBaseDlg)
	ON_COMMAND(ID_HOLDARGUMENT_EDIT, OnHoldargumentEdit)
	ON_COMMAND(ID_HOLD_CHOOSEAFIX, OnHoldProcPropCombox)
END_MESSAGE_MAP()


BOOL CAirsideHoldDlg::OnInitDialog()
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_HOLD));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( bitmap );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Hold"));
	}
	else
	{
		SetWindowText(_T("Define Hold"));
	}


	try
	{
		std::vector<ALTObject> vWaypoint;
		ALTObject::GetObjectList(ALT_WAYPOINT,m_nAirportID,vWaypoint);
		
		for (int i = 0; i < static_cast<int>(vWaypoint.size()); ++ i)
		{
			m_vWaypoint.push_back(std::make_pair(vWaypoint[i].GetObjNameString(),vWaypoint[i].getID()));
		}
	}
	catch (CADOException &e)
	{
		CString strError = e.ErrorMessage();
		MessageBox(_T("Cann't load way point."));

	}

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_13,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_treeProp.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse|ARCUnit_Time_InUse);
	/////

	LoadTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAirsideHoldDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hWayPoint || hTreeItem == m_hRight)
	{
		m_hRClickItem = hTreeItem;
		OnHoldProcPropCombox();
		
	}
	else if(hTreeItem == m_hInBound || hTreeItem == m_hOutBoundLegnm || hTreeItem == m_hOutBoundLegmin
		|| hTreeItem == m_hMaxAlt || hTreeItem == m_hMinAlt)
	{		
		m_hRClickItem = hTreeItem;
		OnHoldargumentEdit();
	}

}

void CAirsideHoldDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem)
	{
		m_treeProp.SelectItem(m_hRClickItem);
		if(m_hRClickItem == m_hWayPoint || m_hRClickItem == m_hRight )
		{
			pMenu = menuPopup.GetSubMenu(59); 
		}
		else if (m_hOutbItem == m_hRClickItem)
		{
		}
		else
		{
			pMenu = menuPopup.GetSubMenu(60);
		}
	}

}

LRESULT CAirsideHoldDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{	
	LPSPINTEXT pst = (LPSPINTEXT) lParam;
	if (message == WM_INPLACE_SPIN)
	{
		if(m_hRClickItem == m_hMinAlt)
		{
			double dUnConvertedLength = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent);
			((AirHold *)GetObject())->setMinAltitude(dUnConvertedLength);
			m_bPropModified = true;	
			LoadTree();
				
		}
		else if(m_hRClickItem == m_hMaxAlt)
		{
			double dUnConvertedLength = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent);

			((AirHold *)GetObject())->setMaxAltitude(dUnConvertedLength);
			m_bPropModified = true;
			LoadTree();

		}
		else if(m_hRClickItem == m_hInBound)
		{
			
			((AirHold *)GetObject())->setInBoundDegree((pst->iPercent)*SCALE_FACTOR);
			m_bPropModified = true;

			LoadTree();

		}
		else if(m_hRClickItem == m_hOutBoundLegmin)
		{
			((AirHold *)GetObject())->setOutBoundLegMin(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent));
			m_bPropModified = true;
			LoadTree();
		}
		else if(m_hRClickItem == m_hOutBoundLegnm)
		{
			((AirHold *)GetObject())->setOutBoundLegNm(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent));
			m_bPropModified = true;	
			LoadTree();

		}	
	}
	if (message == WM_INPLACE_COMBO2)
	{	
		int selitem = m_treeProp.m_comboBox.GetCurSel();	
		if(selitem >=0)
		{
			if(m_hRClickItem == m_hWayPoint)
			{
				((AirHold *)GetObject())->SetWayPoint(m_vWaypoint[selitem].second);	
				m_bPropModified = true;
				LoadTree();
			}
			else if(m_hRClickItem == m_hRight)
			{
				bool bRight = selitem==0?true:false;

				((AirHold *)GetObject())->setRightTurn(bRight);
				m_bPropModified = true;
				LoadTree();
			}
		}
	}
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideHoldDlg::GetFallBackReason()
{
	//if (m_hRClickItem == m_hServiceLocation)
	//{
	//	return PICK_ONEPOINT;
	//}
	return PICK_MANYPOINTS;

}
void CAirsideHoldDlg::SetObjectPath(CPath2008& path)
{
	//if (m_hServiceLocation == m_hRClickItem)
	//{
	//	((AirHold *)m_pObject)->SetLocation(path.getPoint(0) );
	//	LoadTree();
	//}
}

void CAirsideHoldDlg::LoadTree()
{
	m_treeProp.DeleteAllItems();

	CString strUnitLabel = CString("(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString(")");

	CString strLabel;
	CString strWayPointID;
	int nWaypointID = ((AirHold *)GetObject())->GetWatpoint();
	std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
	for (; iter != m_vWaypoint.end(); ++iter)
	{
		if(iter->second == nWaypointID)
		{
			strWayPointID = iter->first;
			break;
		}
	}

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	strLabel.Format("Waypoint:%s",strWayPointID);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%s"),strWayPointID);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hWayPoint = m_treeProp.InsertItem(strLabel);	
	m_treeProp.SetItemDataEx(m_hWayPoint,aoidDataEx);

	double nInBoundDegree  = ((AirHold *)GetObject())->getInBoundDegree();
	strLabel.Format("InBound Course: %d" ,(int)(nInBoundDegree/SCALE_FACTOR ));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%d"),(int)(nInBoundDegree/SCALE_FACTOR ));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hInBound  = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hInBound,aoidDataEx);
	//outbound leg
	
	double dOutBoundmin = ((AirHold *)GetObject())->getOutBoundLegMin();
	double dOutBoundnm = ((AirHold *)GetObject())->getOutBoundLegNm();
	
	strLabel.Format("OutBound Leg");
	m_hOutbItem = m_treeProp.InsertItem(strLabel);
	strLabel.Format("%d %s",(int)CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dOutBoundnm),strUnitLabel);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%d"),(int)CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dOutBoundnm));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hOutBoundLegnm = m_treeProp.InsertItem(strLabel,m_hOutbItem);
	m_treeProp.SetItemDataEx(m_hOutBoundLegnm,aoidDataEx);

	strLabel.Format("%d (min)",(int)CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dOutBoundmin));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%d"),(int)CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dOutBoundmin));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hOutBoundLegmin = m_treeProp.InsertItem(strLabel,m_hOutbItem);
	m_treeProp.SetItemDataEx(m_hOutBoundLegmin,aoidDataEx);

	m_treeProp.Expand(m_hOutbItem,TVE_EXPAND);
	//right or left turn
	BOOL bRight = ((AirHold *)GetObject())->isRightTurn();
	CString rightorleft = bRight?"Right":"Left";
	strLabel.Format("%s Turn",rightorleft);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%s"),rightorleft);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hRight = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hRight,aoidDataEx);

	//min altitude 
	 double dminAlt = ((AirHold *)GetObject())->getMinAltitude();
	strLabel.Format("Min Altitude%s : %d", strUnitLabel, (int)CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dminAlt));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%d"),(int)CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dminAlt));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hMinAlt = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hMinAlt,aoidDataEx);
	
	//max altitude
	double dmaxAlt = ((AirHold *)GetObject())->getMaxAltitude();
	strLabel.Format("Max Altitude%s : %d", strUnitLabel, (int)CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dmaxAlt));
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%d"),(int)CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dmaxAlt));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hMaxAlt = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hMaxAlt,aoidDataEx);
}

void CAirsideHoldDlg::OnHoldargumentEdit()
{
	m_treeProp.SetDisplayType(2);
	m_treeProp.SetSpinRange(0,INT_MAX);
	if(m_hRClickItem==m_hMinAlt )
	{
		double dminAlt = ((AirHold *)GetObject())->getMinAltitude();
		m_treeProp.SetDisplayNum(int(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dminAlt)));
	}
	else if(m_hRClickItem == m_hMaxAlt)
	{
		double dmaxAlt = ((AirHold *)GetObject())->getMaxAltitude();
		m_treeProp.SetDisplayNum(int(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dmaxAlt)));
	}
	else if( m_hRClickItem == m_hInBound)
	{	
		m_treeProp.SetSpinRange(0,360);
		double nInBoundDegree  = ((AirHold *)GetObject())->getInBoundDegree();
		m_treeProp.SetDisplayNum(int((nInBoundDegree/SCALE_FACTOR)));
	}
	else if(m_hRClickItem == m_hOutBoundLegmin)
	{	
		double dOutBoundmin = ((AirHold *)GetObject())->getOutBoundLegMin();

		m_treeProp.SetDisplayNum(int(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dOutBoundmin)));
	}else if(m_hRClickItem==m_hOutBoundLegnm)
	{
		double dOutBoundnm = ((AirHold *)GetObject())->getOutBoundLegNm();
		m_treeProp.SetDisplayNum(int(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dOutBoundnm)));
	}
	m_treeProp.SpinEditLabel(m_hRClickItem);
}
void CAirsideHoldDlg::OnHoldProcPropCombox()
{
	if(m_hRClickItem == m_hWayPoint)
	{
		std::vector<CString> vWayPointName;
		vWayPointName.clear();
		//const ProcessorList* prolist = GetInputTerminal()->GetProcessorList();
		//for(int i=0;i<prolist->getProcessorCount();i++){
		//	const Processor* proc = prolist->getProcessor(i);
		//	if(proc->getProcessorType() == FixProcessor)m_strfixes.push_back(proc->getIDName());
		//}

		std::vector<std::pair<CString,int> >::iterator iter = m_vWaypoint.begin();
		for (; iter != m_vWaypoint.end(); ++iter)
		{
			vWayPointName.push_back(iter->first);
		}
		m_treeProp.SetComboWidth(200);
		m_treeProp.SetComboString(m_hWayPoint,vWayPointName);
	}
	else if(m_hRClickItem == m_hRight)
	{
		std::vector<CString> svRightLeft;
		svRightLeft.push_back("Right");
		svRightLeft.push_back("Left");
		m_treeProp.SetComboWidth(100);
		m_treeProp.SetComboString(m_hRight,svRightLeft);		
	}
	//	LoadTree();
}

bool CAirsideHoldDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideHoldDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideHoldDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}
