// DlgVehicleOperation.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgVehicleOperation.h"
#include "..\Landside\CLandSideVehicleType.h"
#include "..\Landside\CLandSideIterateVehicle.h"
#include "..\Landside\CLandSidePrivateVehicleType.h"

#include "..\Landside\CLandSideTaxiVehicleType.h"
#include "..\Landside\CLandSideVehicleTypeDistribution.h"
#include "..\Landside\CVehicleTypeStation.h"
#include "../Common/WinMsg.h"
#include "Resource.h"
#include "ConDBExListCtrl.h"
// #include "ProbDistDlg.h"
#include "..\Inputs\IN_TERM.H"
#include "..\common\probdistmanager.h"
#include "TermPlan.h"
#include "TermPlanDoc.h"
#include "..\inputs\paxdata.h"
#include "Landside\LandSideOperationTypeData.h"
#include "Landside\InputLandside.h"
#include "../Landside/VehicleLandSideDefine.h"
#include "DlgBusStationSelect.h"
#include "DlgEntryExitNodeSelect.h"
#include ".\dlgvehicleoperation.h"
#include <common/StringUtil.h>
// CDlgVehicleOperation dialog

IMPLEMENT_DYNAMIC(CDlgVehicleOperation, CDialog)
CDlgVehicleOperation::CDlgVehicleOperation(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVehicleOperation::IDD, pParent)
{
}
CDlgVehicleOperation::CDlgVehicleOperation(int pProjID,Terminal *pTerminal,CWnd* pParent /*=NULL*/)
: CDialog(CDlgVehicleOperation::IDD, pParent)
{
	m_pProjID=pProjID;
	isSave=FALSE;
	bIsSize=FALSE;	
}

CDlgVehicleOperation::~CDlgVehicleOperation()
{
}
void CDlgVehicleOperation::OnSize(UINT nType, int cx, int cy)
{
	
	if (!bIsSize)
	{
		//bIsSize=true;
		return;
	}
	
	CRect listrect;
	CRect comRect;
	CRect StaticcomRect;
	

	CRect listBoxRect;
    CRect staticBoxRect;
	CRect windowRect;
	CRect btnRect;
    
	CRect  longRadioRect;
	CRect  ShortRadioRect;
	CRect  NoRadioRect;

	CRect  EditIntervalRect;
	CRect  StartTimeRect;
	CRect  EndTimeRect;
	CRect  StaticMinRect;
	CRect  staticIntervalRect;
	CRect  staticStartTimeRect;
	CRect  staticEndTimeRect;
	CRect  ComEndDayRect;
	CRect  ComStartDayRect;
	CRect rectwindow;
   // CRect  staticMinRect;
    if (cx<=0)
    {
		GetWindowRect(&rectwindow);
		cx=rectwindow.Width()-5;
		cy=rectwindow.Height()-30;
		//return ;
    }
	m_UnitType=nType;
	m_cx=cx;
	m_cy=cy;
	GetWindowRect(&windowRect);
	enumVehicleLandSideType m_type=(enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel());
	
	m_pStaticOperationType.GetWindowRect(&StaticcomRect);
	m_listBoxSpecifications.GetWindowRect(&listBoxRect);
	m_pStaticficication.GetWindowRect(&staticBoxRect);
	com_OperationType.GetWindowRect(&comRect);
	int totalHeight=0;
	if(com_OperationType.GetCurSel()>=0)
	{
	
		switch(m_type)
		{
		case VehicleType_Public:
			 
			m_btnSave.GetWindowRect( &btnRect );
			if (cx<700)
			{
				m_pStaticficication.MoveWindow(9,25,m_staticBoxWidth,cy-60-btnRect.Height());
				m_listBoxSpecifications.MoveWindow(10,40,m_boxWidth,cy-75-btnRect.Height());
			}
			else
			{
				m_pStaticficication.MoveWindow(9,25,m_staticBoxWidth+102,cy-60-btnRect.Height());
				m_listBoxSpecifications.MoveWindow(10,40,m_boxWidth+100,cy-75-btnRect.Height());
			}
		

			m_pStaticficication.GetWindowRect(&staticBoxRect);
			ScreenToClient(&staticBoxRect);
			m_pStaticOperationType.MoveWindow(staticBoxRect.right+10,40,StaticcomRect.Width(),StaticcomRect.Height());

			m_pStaticOperationType.GetWindowRect(&StaticcomRect);
			ScreenToClient(&StaticcomRect);
			com_OperationType.MoveWindow(StaticcomRect.right+5,35,comRect.Width(),comRect.Height());
			com_OperationType.GetWindowRect(&comRect);
			ScreenToClient(&comRect);
             
		     m_pStaticStartTime.GetWindowRect(&staticStartTimeRect);
			 m_pStaticStartTime.MoveWindow(staticBoxRect.right+38,comRect.bottom+20+staticStartTimeRect.Height(),staticStartTimeRect.Width(),staticStartTimeRect.Height());
			 m_pStaticStartTime.GetWindowRect(&staticStartTimeRect);
			 ScreenToClient(&staticStartTimeRect);
			 m_comStartDay.GetWindowRect(&ComStartDayRect);
			 m_comStartDay.MoveWindow(staticStartTimeRect.right+6,comRect.bottom+15+staticStartTimeRect.Height(),ComStartDayRect.Width(),ComStartDayRect.Height());

			 m_comStartDay.GetWindowRect(&ComStartDayRect);
			 ScreenToClient(&ComStartDayRect);
			 m_DateStartTime.GetWindowRect(&StartTimeRect);
			 m_DateStartTime.MoveWindow(ComStartDayRect.right+5,comRect.bottom+15+staticStartTimeRect.Height(),StartTimeRect.Width(),StartTimeRect.Height());
             
			
			 m_pStaticEndTime.GetWindowRect(&staticEndTimeRect);
			 m_pStaticEndTime.MoveWindow(staticBoxRect.right+45,comRect.bottom+40+2*staticStartTimeRect.Height(),staticEndTimeRect.Width(),staticEndTimeRect.Height());
			 m_pStaticEndTime.GetWindowRect(&staticEndTimeRect);
			 ScreenToClient(&staticEndTimeRect);
			 m_ComEndDay.GetWindowRect(&ComEndDayRect);
			 m_ComEndDay.MoveWindow(staticEndTimeRect.right+4,comRect.bottom+35+2*staticStartTimeRect.Height(),ComEndDayRect.Width(),ComEndDayRect.Height());
			 m_ComEndDay.GetWindowRect(&ComEndDayRect);
			 ScreenToClient(&ComEndDayRect);
			 m_endTime.GetWindowRect(&EndTimeRect);
			 m_endTime.MoveWindow(ComEndDayRect.right+5,comRect.bottom+35+2*staticStartTimeRect.Height(),EndTimeRect.Width(),EndTimeRect.Height());

			 m_pStaticInterval.GetWindowRect(&staticIntervalRect);
			 m_pStaticInterval.MoveWindow(staticBoxRect.right+49,comRect.bottom+60+3*staticStartTimeRect.Height(),staticIntervalRect.Width(),staticIntervalRect.Height());
			 m_pStaticInterval.GetWindowRect(&staticIntervalRect);
			 ScreenToClient(&staticIntervalRect);
			 m_EditInterval.GetWindowRect(&EditIntervalRect);
			 m_EditInterval.MoveWindow(staticIntervalRect.right+7,comRect.bottom+55+3*staticStartTimeRect.Height(),EditIntervalRect.Width(),EditIntervalRect.Height());

			m_EditInterval.GetWindowRect(&EditIntervalRect);
			ScreenToClient(&EditIntervalRect);
			m_labelMin.GetWindowRect(&StaticMinRect);
			m_labelMin.MoveWindow(EditIntervalRect.right+4,comRect.bottom+60+3*staticStartTimeRect.Height(),StaticMinRect.Width(),StaticMinRect.Height());

		    m_listCtrl.GetWindowRect(&listrect);
			m_listCtrl.MoveWindow(staticBoxRect.right+10,EditIntervalRect.bottom+30,cx-staticBoxRect.right-15, cy-65-btnRect.Height()-EditIntervalRect.bottom);

			m_listCtrl.GetWindowRect(listrect);
			ScreenToClient(listrect);
			listrect.top=listrect.top-26;
			listrect.bottom=listrect.top+22;
		    m_wndToolBar.MoveWindow(listrect);
			m_wndToolBar.BringWindowToTop();
			m_wndToolBar.ShowWindow(SW_SHOW);
			m_btnSave.MoveWindow( cx-40-3*btnRect.Width(),cy-15-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			m_btnOk.MoveWindow( cx-25-2*btnRect.Width(),cy-15-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			m_btnCancel.MoveWindow( cx-10-btnRect.Width(),cy-15-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			 
			break;
		case VehicleType_Iterate:
		case VehicleType_Taxi:
			m_btnSave.GetWindowRect( &btnRect );
			if (cx<700)
			{
				m_pStaticficication.MoveWindow(9,25,m_staticBoxWidth,cy-60-btnRect.Height());
				m_listBoxSpecifications.MoveWindow(10,40,m_boxWidth,cy-75-btnRect.Height());
			}
			else
			{
				m_pStaticficication.MoveWindow(9,25,m_staticBoxWidth+102,cy-60-btnRect.Height());
				m_listBoxSpecifications.MoveWindow(10,40,m_boxWidth+100,cy-75-btnRect.Height());
			}
			 m_pStaticficication.GetWindowRect(&staticBoxRect);
			 ScreenToClient(&staticBoxRect);
			 m_pStaticOperationType.MoveWindow(staticBoxRect.right+10,40,StaticcomRect.Width(),StaticcomRect.Height());
			 
			 m_pStaticOperationType.GetWindowRect(&StaticcomRect);
			 ScreenToClient(&StaticcomRect);
			 com_OperationType.MoveWindow(StaticcomRect.right+5,35,comRect.Width(),comRect.Height());

			 m_listTaxiAndIterate.MoveWindow(StaticcomRect.left,StaticcomRect.bottom+40,cx-staticBoxRect.right-20,cy-113-btnRect.Height()-StaticcomRect.Height());
			
             m_listTaxiAndIterate.GetWindowRect(listrect);
			 ScreenToClient(listrect);
			 listrect.top=listrect.top-26;
			 listrect.bottom=listrect.top+22;
			 m_wndToolBar.MoveWindow(listrect);
			 m_wndToolBar.BringWindowToTop();
			 m_wndToolBar.ShowWindow(SW_SHOW);
			 m_btnSave.MoveWindow( cx-40-3*btnRect.Width(),cy-15-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			 m_btnOk.MoveWindow( cx-25-2*btnRect.Width(),cy-15-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			 m_btnCancel.MoveWindow( cx-10-btnRect.Width(),cy-15-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			
			break;
		
		case VehiclType_Private:
			m_btnSave.GetWindowRect( &btnRect );
			m_listCtrl.ShowWindow(SW_HIDE);
			m_listTaxiAndIterate.ShowWindow(SW_HIDE);
			m_wndToolBar.ShowWindow(SW_HIDE);
			m_pStaticOperationType.ShowWindow(SW_HIDE);

			if (cx<700)
			{
				m_pStaticficication.MoveWindow(9,25,m_staticBoxWidth,cy-60-btnRect.Height());
				m_listBoxSpecifications.MoveWindow(10,40,m_boxWidth,cy-75-btnRect.Height());
			}
			else
			{
				m_pStaticficication.MoveWindow(9,25,m_staticBoxWidth+102,cy-60-btnRect.Height());
				m_listBoxSpecifications.MoveWindow(10,40,m_boxWidth+100,cy-75-btnRect.Height());
			}

			m_pStaticficication.GetWindowRect(&staticBoxRect);
			ScreenToClient(&staticBoxRect);
			m_pStaticOperationType.MoveWindow(staticBoxRect.right+10,40,StaticcomRect.Width(),StaticcomRect.Height());

			m_pStaticOperationType.GetWindowRect(&StaticcomRect);
			ScreenToClient(&StaticcomRect);
			com_OperationType.MoveWindow(StaticcomRect.right+5,35,comRect.Width(),comRect.Height());

// 			m_pStaticficication.GetWindowRect(&staticBoxRect);
// 			ScreenToClient(&staticBoxRect);
// 			m_pStaticOperationType.MoveWindow(staticBoxRect.right+10,24,StaticcomRect.Width(),StaticcomRect.Height());
// 			m_pStaticOperationType.GetWindowRect(&StaticcomRect);
// 			ScreenToClient(&StaticcomRect);
// 			com_OperationType.MoveWindow(StaticcomRect.right+7,20,comRect.Width(),comRect.Height());

			m_RadionoPark.GetWindowRect(&NoRadioRect);
			//m_RadionoPark.MoveWindow(cx-3*btnRect.Width(),cy/2-5-NoRadioRect.Height(),NoRadioRect.Width(),NoRadioRect.Height());
			m_RadionoPark.MoveWindow(staticBoxRect.right+40,cy/2-40-NoRadioRect.Height(),NoRadioRect.Width(),NoRadioRect.Height());

			m_RadionoPark.GetWindowRect(&NoRadioRect);
			ScreenToClient(&NoRadioRect);
			m_shorPark.GetWindowRect(&ShortRadioRect);
			//m_shorPark.MoveWindow(cx-3*btnRect.Width(),cy/2+5+longRadioRect.Height(),ShortRadioRect.Width(),ShortRadioRect.Height());
			m_shorPark.MoveWindow(staticBoxRect.right+40,cy/2-35+NoRadioRect.Height(),ShortRadioRect.Width(),ShortRadioRect.Height());
			m_longPark.GetWindowRect(&longRadioRect);
			//m_longPark.MoveWindow(cx-3*btnRect.Width(),cy/2+10+longRadioRect.Height(),longRadioRect.Width(),longRadioRect.Height());
			m_longPark.MoveWindow(staticBoxRect.right+40,cy/2-25+3*NoRadioRect.Height(),longRadioRect.Width(),longRadioRect.Height());

			m_btnSave.MoveWindow( staticBoxRect.right+215,cy-40-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			m_btnOk.MoveWindow( staticBoxRect.right+230+btnRect.Width(),cy-40-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			m_btnCancel.MoveWindow( staticBoxRect.right+245+2*btnRect.Width(),cy-40-btnRect.Height(),btnRect.Width(),btnRect.Height() );

			m_btnSave.MoveWindow( cx-40-3*btnRect.Width(),cy-15-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			m_btnOk.MoveWindow( cx-25-2*btnRect.Width(),cy-15-btnRect.Height(),btnRect.Width(),btnRect.Height() );
			m_btnCancel.MoveWindow( cx-10-btnRect.Width(),cy-15-btnRect.Height(),btnRect.Width(),btnRect.Height() );

			m_pStaticOperationType.ShowWindow(SW_SHOW);
			
			break;
		default:
			break;
		}
	}
	
	InvalidateRect(NULL);
}
void CDlgVehicleOperation::setToolBar()
{
	CRect listrect;
	CRect comRect;
	CRect listBoxRect;
	//RECT btnRect;
	CRect windowRect;
	CRect btnRect;
	CRect  EditIntervalRect;
	
	GetWindowRect(&windowRect);
	enumVehicleLandSideType m_type=(enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel());

	if(com_OperationType.GetCurSel()>=0)
	{

		switch(m_type)
		{
		case VehicleType_Public:
			
			  m_listCtrl.GetWindowRect(listrect);
			  ScreenToClient(listrect);
			  listrect.top=listrect.top-26;
			  listrect.bottom=listrect.top+22;
			  m_wndToolBar.MoveWindow(listrect);
			  m_wndToolBar.BringWindowToTop();
			  m_wndToolBar.ShowWindow(SW_SHOW);
			break;
		case VehicleType_Iterate:
			
		case VehicleType_Taxi:
			m_listTaxiAndIterate.GetWindowRect(listrect);
			ScreenToClient(listrect);
			listrect.top=listrect.top-26;
			listrect.bottom=listrect.top+22;
			m_wndToolBar.MoveWindow(listrect);
			m_wndToolBar.BringWindowToTop();
			m_wndToolBar.ShowWindow(SW_SHOW);
			break;
		
		case VehiclType_Private:
			m_listCtrl.ShowWindow(SW_HIDE);
			m_listTaxiAndIterate.ShowWindow(SW_HIDE);
			m_wndToolBar.ShowWindow(SW_HIDE);
			break;
		default:
			break;
		}
	}
}
BOOL CDlgVehicleOperation::OnInitDialog()
{
	CDialog::OnInitDialog();
	int i=0;
	isFirst=FALSE;
	
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	InputTerminal *interminal= (InputTerminal*)&pDoc->GetTerminal();
	//ConstraintDatabase *pConDB=interminal->paxDataList->getInStep();
	//ConstraintDatabase* pConDB =pDoc->getARCport()->landSideOperationList->getLandSideConstraint();
	//m_listTaxiAndIterate.SetCurConDB(pConDB);
	if(!m_wndToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)||
		!m_wndToolBar.LoadToolBar(IDR_ACTYPE_ALIASNAME1))
	{
		TRACE(_T("Create tool bar error "));
		return FALSE;
	}


	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);
// 	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_END,FALSE);
	CRect rect;
	m_btnSave.GetWindowRect(&m_btnSaveRect);
	m_btnCancel.GetWindowRect(&m_btnCancelRect);
	m_btnOk.GetWindowRect(&m_btnOkRect);
	m_RadionoPark.GetWindowRect(&m_noRadioRect);
	m_shorPark.GetWindowRect(&m_ShorRadioRect);
	m_longPark.GetWindowRect(&m_LongRadioRect);
	m_listBoxSpecifications.GetWindowRect(&rect);
	m_boxWidth=rect.Width();
	m_boxHeight=rect.Height();
	m_pStaticficication.GetWindowRect(&rect);
	m_StaticBoxHeight=rect.Height();
	m_staticBoxWidth=rect.Width();
	bIsSize=TRUE;
	m_DateStartTime.SetFormat("HH:mm:ss");
	m_endTime.SetFormat("HH:mm:ss");
	for (int nType = VehiclType_Private; nType < VehicleType_LandSideCount; ++ nType)
	{
		com_OperationType.AddString(LandSideVehicleTypeName[i]);
		com_OperationType.SetItemData(i,nType);
		i++;
	}
// 	int typeNum=com_OperationType.GetCount();
// 	com_OperationType.SetDroppedWidth(typeNum);
	isListBoxSelChange=FALSE;
	CString strDays[]={"Day1","Day2","Day3"};
	 for (i=0;i<sizeof(strDays)/sizeof(strDays[0]);i++)
	 {
		 m_ComEndDay.AddString(strDays[i]);
		 m_comStartDay.AddString(strDays[i]);
	 }
	 
	 m_comStartDay.SetCurSel(0);
	 m_ComEndDay.SetCurSel(0);
    m_pLandSide=pDoc->getARCport()->m_pInputLandside;
	m_pObjlist=&(m_pLandSide->getObjectList());
	SetListBoxContent();
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	this->GetWindowRect(&rect);
	rect.bottom-=20;
	this->MoveWindow(rect);

	return true;

}
void CDlgVehicleOperation::setControl()
{
// 	m_listCtrl.ModifyStyle(LVS_SINGLESEL | LVS_EDITLABELS, 0);
	DWORD dwStyle = m_listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrl.SetExtendedStyle(dwStyle);

	m_listTaxiAndIterate.ModifyStyle(LVS_SINGLESEL | LVS_EDITLABELS, 0);
	dwStyle = m_listTaxiAndIterate.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listTaxiAndIterate.SetExtendedStyle(dwStyle);

	/*for (int i=0;i<m_listCtrl.GetHeaderCtrl()->;i++)
	{
			m_listCtrl.DeleteColumn(i);
	}*/
	while(m_listCtrl.DeleteColumn(0))
	{

	}
	while (m_listTaxiAndIterate.DeleteColumn(0))
	{
	}
	
	///CString strText;
	enumVehicleLandSideType m_type=(enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel());
	if(com_OperationType.GetCurSel()>=0)
	{
		/*com_OperationType.GetWindowText(strText);
        m_type=atoi(strText);*/
		switch(m_type)
		{
		case VehicleType_Public:
			 SetPublicBusListCtrlHeader();
			 ShowPublicBus();
			break;
		case VehicleType_Iterate:
			setIterateListCtrHeader();
			ShowIterateVehicle();
			break;
		case VehicleType_Taxi:
			setTaxiListCtrHeader();
			ShowRandomTaxi();
			break;
		case VehiclType_Private:
			ShowPrivateVehicle();
			break;
		default:
			break;
		}
	}
	
}
void CDlgVehicleOperation::SetPublicBusStationList(LandSidePublicVehicleType *type)
{
	m_listCtrl.DeleteAllItems();
	CString str ;
	int j=0;
	
	if (type->GetEntryObjID()==-1)
	{
		return;
	}
	LandsideExternalNode *pEntryNode=new LandsideExternalNode;
	pEntryNode->ReadObject(type->GetEntryObjID(),m_pLandSide);
	str=pEntryNode->getName().GetIDString();
	m_listCtrl.InsertItem(0,str);
	m_listCtrl.SetItemText(0,0,_T("Entry"));
	m_listCtrl.SetItemText(0,1,str);
	m_listCtrl.SetItemData(0,(DWORD)type);
	delete pEntryNode;

	for(j=0;j<type->getElementCount();j++)
	{
		VehicleTypeStation *tmp=type->getStationItem(j);
		if (tmp->GetBusStationID()!=-1)
		{
			LandsideBusStation *tmpBusStation=new LandsideBusStation;
			tmpBusStation->ReadObject(tmp->GetBusStationID(),m_pLandSide);
			str.Format("%s",tmpBusStation->getName().GetIDString());
			m_listCtrl.InsertItem(j+1,str);
			m_listCtrl.SetItemText(j+1,1,str);
			str.Empty();
			str.Format("%d",j+1);
			m_listCtrl.SetItemText(j+1,0,str);
			m_listCtrl.SetItemData(j+1,(DWORD)type);
			delete tmpBusStation;
		}

	}
	
	if (type->GetExitObjID()==-1)
	{
		return;
	}
	LandsideExternalNode *pExitNode=new LandsideExternalNode;
	pExitNode->ReadObject(type->GetExitObjID(),m_pLandSide);

	str=pExitNode->getName().GetIDString();
	m_listCtrl.InsertItem(j+1,str);
	m_listCtrl.SetItemText(j+1,0,_T("Exit"));
	m_listCtrl.SetItemText(j+1,1,str);
	m_listCtrl.SetItemData(j+1,(DWORD)type);
	delete pExitNode;


}
void CDlgVehicleOperation::SetPublicListContent()
{
	m_listCtrl.DeleteAllItems();
    int i=m_listBoxSpecifications.GetCurSel();
	int tmpID=m_listBoxSpecifications.GetItemData(i);
	CVehicleItemDetail *pItem= NULL;//m_pLandSide->getOperationVehicleList().GetItem(i);
	bool b=false;

	for(int k=0;k<pItem->getOperationVehicleCount();k++)
	{
		if(pItem->getOperationVehicleType(k)->getVehicleType()==VehicleType_Public)

		{
			LandSidePublicVehicleType *m_type=(LandSidePublicVehicleType *)pItem->getOperationVehicleType(k);
			COleDateTime tmpOleTime;
			ElapsedTime tmpElaTime;
			tmpElaTime=m_type->getEndTime();
			tmpOleTime.SetTime(tmpElaTime.GetHour(),tmpElaTime.GetMinute(),tmpElaTime.GetSecond());
			m_endTime.SetTime(tmpOleTime);
			m_ComEndDay.SetCurSel(tmpElaTime.GetDay()-1);
			tmpElaTime=m_type->getStartTime();
			tmpOleTime.SetTime(tmpElaTime.GetHour(),tmpElaTime.GetMinute(),tmpElaTime.GetSecond());
			m_DateStartTime.SetTime(tmpOleTime);
			m_comStartDay.SetCurSel(tmpElaTime.GetDay()-1);
			CString strInterval;
			if (m_type->getInterval()<=0)
			{
				m_type->setInterval(15);
			}
			strInterval.Format(_T("%d"),m_type->getInterval());
			m_EditInterval.SetWindowText(strInterval);			
			SetPublicBusStationList(m_type);
			b=true;
			break;
		}
	}
	if (!b)
	{
		LandSidePublicVehicleType *tmpType=new LandSidePublicVehicleType();
		COleDateTime tmpOleTime;
		tmpOleTime.SetTime(23,59,59);
		m_endTime.SetTime(tmpOleTime);
		m_ComEndDay.SetCurSel(0);		
		m_comStartDay.SetCurSel(0);
		tmpOleTime.SetTime(0,0,0);
		m_DateStartTime.SetTime(tmpOleTime);
		m_EditInterval.SetWindowText("15");
		//tmpType->setVehicelSpecificationID(tmpID);
		//tmpType->setVehicleType(VehiclType_Private);
		tmpType->setEndTime(ElapsedTime(0L));
		tmpType->setStartTime(ElapsedTime(long(3600*24-1)));
		tmpType->setInterval(15);
		pItem->AddOperationVehicle(tmpType);

	}

}
void CDlgVehicleOperation::SetPrivateListContent()
{

	BOOL b=FALSE;
	int i=m_listBoxSpecifications.GetCurSel();
	int tmpID=m_listBoxSpecifications.GetItemData(i);
	CVehicleItemDetail *pItem= NULL;ASSERT(FALSE);//m_pLandSide->getOperationVehicleList().GetItem(i);
	m_RadionoPark.SetCheck(0);
	m_shorPark.SetCheck(0);
	m_longPark.SetCheck(0);

	
	LandSidePrivateVehicleType *tmpType=(LandSidePrivateVehicleType*)pItem->getOperation(VehiclType_Private);
	ASSERT(tmpType);
	if(tmpType)
	{
		switch(tmpType->getParkType())
		{
		case NO_Park:
			m_RadionoPark.SetCheck(1);
			break;
		case Short_Park:
			m_shorPark.SetCheck(1);
			break;
		case Long_Park:
			m_longPark.SetCheck(1);
			break;
		}
	}

}

//class VTListItemCount : public  ListCtrlEditImpl
//{
//public:
//	VTListItemCount(LandSideVehicleTypeDistribution *tmp):m_tmp(tmp){}
//	virtual void OnDoneEdit(CListCtrl& ctrl, int index, int column )
//	{
//		__super::OnDoneEdit(ctrl,index,column);
//		int retCount;
//		if( CStringUtil::ToInteger(strRet,retCount))
//		{
//			m_tmp->setCount(retCount);
//		}
//		OnSetTo(ctrl,index,column);
//	}
//	virtual void OnSetTo(CListCtrl& ctrl, int index, int column)
//	{
//		CString strItem = CStringUtil::FromInteger(m_tmp->getCount());
//		ctrl.SetItemText(index,column,strItem);
//	}
//protected:
//	LandSideVehicleTypeDistribution *m_tmp;
//};

class VTListItemTimeRange : public ListCtrlTimeRangeImpl
{
public:
	VTListItemTimeRange(LandSideVehicleTypeDistribution *tmp):m_tmp(tmp){}
	virtual void OnSetTo(CListCtrl& ctrl, int index, int column)
	{
		startT = m_tmp->getRangeF();
		endT = m_tmp->getRangeT();
		SetItemText(ctrl,index,column);	
	}
	virtual void OnDoneEdit(CListCtrl& ctrl, int index, int column )
	{
		m_tmp->setRangF(startT);
		m_tmp->setRangT(endT);		
		SetItemText(ctrl,index,column);
	}	
	
protected:
	LandSideVehicleTypeDistribution *m_tmp;
	void SetItemText(CListCtrl& ctrl, int index, int column)
	{
		CString str;	
		str = startT.PrintDateTime() + " - " + endT.PrintDateTime();
		ctrl.SetItemText(index,column,str);
	}
};
class VTListItemDistribut : public ListCtrlProbDistImpl
{
public:
	VTListItemDistribut(LandSideVehicleTypeDistribution *tmp,InputTerminal* pInterm)
		:ListCtrlProbDistImpl(pInterm){
			m_tmp = tmp;
		}
	virtual void OnSetTo(CListCtrl& ctrl, int index, int column)
	{
		ctrl.SetItemText(index,column,m_tmp->getDistribution() );
	}
	virtual void OnDoneEdit(CListCtrl& ctrl, int index, int column )
	{
		__super::OnDoneEdit(ctrl,index,column);
		m_tmp->setDistribution(strDist);
		ctrl.SetItemText(index,column,m_tmp->getDistribution() );
	}	
protected:
	LandSideVehicleTypeDistribution *m_tmp;

};

void SetCtrlItem(int j, CARCListCtrl&m_listTaxiAndIterate,LandSideVehicleTypeDistribution* pType,InputTerminal *interminal  )
{
	m_listTaxiAndIterate.SetItemImpl(j,0,new VTListItemTimeRange(pType) );
//	m_listTaxiAndIterate.SetItemImpl(j,1,new VTListItemCount(pType) );
	m_listTaxiAndIterate.SetItemImpl(j,1,new VTListItemDistribut(pType,interminal));
}

void CDlgVehicleOperation::SetTaxiListContent()
{
	m_listTaxiAndIterate.DeleteAllItems();
	m_listTaxiAndIterate.ClearMangerList();

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	InputTerminal *interminal =(InputTerminal*)&pDoc->GetTerminal();


	int i=m_listBoxSpecifications.GetCurSel();
	int tmpID=m_listBoxSpecifications.GetItemData(i);
	CVehicleItemDetail *pItem= NULL;//m_pLandSide->getOperationVehicleList().GetItem(i);

	LandSideTaxiVehicleType *m_type=(LandSideTaxiVehicleType *)pItem->getOperation(VehicleType_Taxi);
	ASSERT(m_type);
	if(m_type)
	{
		for(int j=0;j<m_type->getElementCount();j++)
		{
			m_listTaxiAndIterate.InsertItem(j,"");			
			LandSideVehicleTypeDistribution* pType = m_type->getItem(j);
			SetCtrlItem(j,m_listTaxiAndIterate,pType,interminal);
		}
	}	
}


void CDlgVehicleOperation::SetIterateListContent()
{
	m_listTaxiAndIterate.DeleteAllItems();
	m_listTaxiAndIterate.ClearMangerList();
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	InputTerminal *interminal =(InputTerminal*)&pDoc->GetTerminal();

	int i=m_listBoxSpecifications.GetCurSel();
	int tmpID=m_listBoxSpecifications.GetItemData(i);
	
	if( CVehicleItemDetail *pItem = NULL; )//m_pLandSide->getOperationVehicleList().GetItem(i)
	{
		if(LandSideIterateVehicle *m_type = (LandSideIterateVehicle *)pItem->getOperation(VehicleType_Iterate) )
		{
			for(int j=0;j<m_type->getElementCount();j++)
			{	
			
				m_listTaxiAndIterate.InsertItem(j,"");			
				LandSideVehicleTypeDistribution* pType = m_type->getItem(j);
				SetCtrlItem(j,m_listTaxiAndIterate,pType,interminal);
			}
		}
	}	
}

void CDlgVehicleOperation::setControlContent()
{

   enumVehicleLandSideType m_type=(enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel());
	if(com_OperationType.GetCurSel()>=0)
	{
		/*com_OperationType.GetWindowText(strText);
        m_type=atoi(strText);*/
		switch(m_type)
		{
		case VehicleType_Public:
			SetPublicListContent();
			break;
		case VehicleType_Iterate:
			SetIterateListContent();
			break;
		case VehicleType_Taxi:
			SetTaxiListContent();
			break;
		case VehiclType_Private:
			SetPrivateListContent();
			break;
		default:
			break;
		}
	}
}
void CDlgVehicleOperation::SetListBoxContent()
{

	/* size_t totalElement=m_pLandSide->getOperationVehicleList().GetElementCount();
	for (size_t i=0;i<totalElement;i++)
	{
		CVehicleItemDetail *pItem=m_pLandSide->getOperationVehicleList().GetItem((int)i);
		m_listBoxSpecifications.AddString(pItem->getName());
		m_listBoxSpecifications.SetItemData((int)i,pItem->GetID());

	}

	if(totalElement>0)
	{
		m_listBoxSpecifications.SetCurSel(0);
		listboxSeletIndex.push_back(0);
		SetComboText(m_pLandSide->getOperationVehicleList().GetItem(0)->getLandSideBaseType());
	}*/
}
void CDlgVehicleOperation::SetComboText(enumVehicleLandSideType vehicleType)
{
    int i=0;
	int count=com_OperationType.GetCount();
		enumVehicleLandSideType sideType;
	for (i;i<=com_OperationType.GetCount();i++)
	{
		sideType=(enumVehicleLandSideType)com_OperationType.GetItemData(i);
        if(sideType==vehicleType)
		{
			com_OperationType.SetCurSel(i);
			onSelChange();
			break;
		}
	}
}
void CDlgVehicleOperation::SetPublicBusListCtrlHeader()
{

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCFMT_CENTER;
	lvc.pszText = _T("Index");
	lvc.cx = 62;
	lvc.fmt =LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_listCtrl.InsertColumn(0, &lvc);

	for (int nType = VehiclType_Private; nType < VehicleType_LandSideCount; ++ nType)
	{
		strList.InsertAfter(strList.GetTailPosition(),LandSideVehicleTypeName[nType - LandSideTypeBaseValue]);
	}
	lvc.csList = &strList;
	lvc.cx = 110;
	lvc.pszText = _T("Station Type");
	lvc.fmt = LVCFMT_NOEDIT;
	m_listCtrl.InsertColumn(1, &lvc);


	
}
void CDlgVehicleOperation::onSelChange()
{

	if (bIsSize)
	{
		OnSize(m_UnitType,m_cx,m_cy);
	}
	selectTypes.push_back((enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel()));
	 if((int)selectTypes.size()>=2)
	 {
		 if(selectTypes.at(selectTypes.size()-2)==com_OperationType.GetItemData(com_OperationType.GetCurSel()) &&!isListBoxSelChange)
		  return ;
	 }
	setControl();
	if (isFirst)
	{


		getGuiValue();
	}
	else 
	{
		isFirst=TRUE;
	}
	setControlContent();
	setToolBar();
	enumVehicleLandSideType m_type=(enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel());
	
}
void CDlgVehicleOperation::OnDblclkListBox()
{
	CVehicleItemDetail *pItem=m_pLandSide->getOperationVehicleList().GetItem(m_listBoxSpecifications.GetCurSel());
	for ( int sideType=0;sideType<com_OperationType.GetCount();sideType++)
	{
		if (pItem->getLandSideBaseType()==com_OperationType.GetItemData(sideType))
		{
			com_OperationType.SetCurSel(sideType);
			onSelChange();
			break;

		}

	}

} 
void CDlgVehicleOperation::setIterateListCtrHeader()
{
	setTaxiListCtrHeader();
}
void CDlgVehicleOperation::setTaxiListCtrHeader()
{
	CStringList strList;
	LV_COLUMN lvc;
	//time range
	{
		lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCFMT_CENTER;
		lvc.pszText = _T("Time Range");
		lvc.cx = 170;
		lvc.fmt = LVCFMT_NOEDIT;
		//lvc.csList = &strList;
		m_listTaxiAndIterate.InsertColumn(0, &lvc);
	}
	
	//{//count
	//	lvc.fmt = LVCFMT_NUMBER;
	//	lvc.cx = 45;
	//	lvc.pszText= _T("Count");
	//	m_listTaxiAndIterate.InsertColumn(1,&lvc);
	//}
	{//distribution
		CString s;	s.LoadString( IDS_STRING_NEWDIST );
		strList.InsertAfter( strList.GetTailPosition(), s );
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
		InputTerminal *interminal =(InputTerminal*)&pDoc->GetTerminal();
		int nCount =interminal->m_pAirportDB->getProbDistMan()->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = interminal->m_pAirportDB->getProbDistMan()->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		//lvc.csList = &strList;
		lvc.cx = 200;
		lvc.pszText = _T("Count Distribution");
		lvc.fmt = LVCFMT_DROPDOWN;
		m_listTaxiAndIterate.InsertColumn(2, &lvc);
		//m_listTaxiAndIterate.SetInputTerminal(interminal);
	}
}
void CDlgVehicleOperation::ShowPrivateVehicle()
{
   // HideIterateVehicle();
	HidePublicBus();
	HideRandomTaxi();

	m_RadionoPark.ShowWindow(SW_SHOW);
	m_shorPark.ShowWindow(SW_SHOW);
	m_longPark.ShowWindow(SW_SHOW);
	//((CStatic*)GetDlgItem(IDC_STATIC_GROUP))->ShowWindow(SW_SHOW);

}
void CDlgVehicleOperation::ShowPublicBus()
{
	//HideIterateVehicle();
	HidePrivateVehicle();
	HideRandomTaxi();
	m_endTime.ShowWindow(SW_SHOW);
	m_DateStartTime.ShowWindow(SW_SHOW);
	m_EditInterval.ShowWindow(SW_SHOW);
	m_listCtrl.ShowWindow(SW_SHOW);
	m_wndToolBar.ShowWindow(SW_SHOW);
	((CStatic*)GetDlgItem(IDC_STATIC_STARTTIME))->ShowWindow(SW_SHOW);
	((CStatic*)GetDlgItem(IDC_STATIC_ENDTIME))->ShowWindow(SW_SHOW);
	((CStatic*)GetDlgItem(IDC_STATIC_INTERVAL))->ShowWindow(SW_SHOW);
	m_labelMin.ShowWindow(SW_SHOW);
    m_ComEndDay.ShowWindow(SW_SHOW);
	m_comStartDay.ShowWindow(SW_SHOW);
	//m_listCtrl.GetClientRect()

}
void CDlgVehicleOperation::ShowRandomTaxi()
{
	HidePublicBus();
	HidePrivateVehicle();
	m_listTaxiAndIterate.ShowWindow(SW_SHOW);
	
}
void CDlgVehicleOperation::ShowIterateVehicle()
{
     ShowRandomTaxi();
}
void CDlgVehicleOperation::HideIterateVehicle()
{
	HideRandomTaxi();
}
void CDlgVehicleOperation::HidePrivateVehicle()
{
	m_RadionoPark.ShowWindow(SW_HIDE);
	m_shorPark.ShowWindow(SW_HIDE);
	m_longPark.ShowWindow(SW_HIDE);
	((CStatic*)GetDlgItem(IDC_STATIC_GROUP))->ShowWindow(SW_HIDE);
}
void CDlgVehicleOperation::HidePublicBus()
{
	m_endTime.ShowWindow(SW_HIDE);
	m_DateStartTime.ShowWindow(SW_HIDE);
	m_EditInterval.ShowWindow(SW_HIDE);
	m_listCtrl.ShowWindow(SW_HIDE);
	m_wndToolBar.ShowWindow(SW_HIDE);
	((CStatic*)GetDlgItem(IDC_STATIC_STARTTIME))->ShowWindow(SW_HIDE);
	((CStatic*)GetDlgItem(IDC_STATIC_ENDTIME))->ShowWindow(SW_HIDE);
	((CStatic*)GetDlgItem(IDC_STATIC_INTERVAL))->ShowWindow(SW_HIDE);
	m_labelMin.ShowWindow(SW_HIDE);
	m_wndToolBar.ShowWindow(SW_HIDE);
	m_wndToolBar.MoveWindow(0,0,0,0);
	m_ComEndDay.ShowWindow(SW_HIDE);
	m_comStartDay.ShowWindow(SW_HIDE);
}
void CDlgVehicleOperation::HideRandomTaxi()
{
  m_listTaxiAndIterate.ShowWindow(SW_HIDE);
}

void CDlgVehicleOperation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OPERATIONTYPE, com_OperationType);
	DDX_Control(pDX, IDC_DATE_STARTIME, m_DateStartTime);
	DDX_Control(pDX, IDC_DATE_ENDTIME, m_endTime);
	DDX_Control(pDX, IDC_EDIT_INTERVAL, m_EditInterval);
	DDX_Control(pDX, IDC_LIST_OPERATIONDATA, m_listCtrl);
	DDX_Control(pDX, IDC_LIST_SPECIFICATIONDATA, m_listBoxSpecifications);
	DDX_Control(pDX, IDC_RADIO_NOPARK, m_RadionoPark);
	DDX_Control(pDX, IDC_RADIO_SHORTPARK, m_shorPark);
	DDX_Control(pDX, IDC_RADIO_LONGPARK, m_longPark);
	DDX_Control(pDX, IDC_STATIC_MINUTER, m_labelMin);
	DDX_Control(pDX, IDC_LIST_TAXIANDITERATE, m_listTaxiAndIterate);
	DDX_Control(pDX, IDC_STATIC_VEHICLETYPE, m_pStaticficication);
	DDX_Control(pDX, IDC_STATIC_OperationType, m_pStaticOperationType);
	DDX_Control(pDX, IDC_STATIC_STARTTIME, m_pStaticStartTime);
	DDX_Control(pDX, IDC_STATIC_ENDTIME, m_pStaticEndTime);
	DDX_Control(pDX, IDC_STATIC_INTERVAL, m_pStaticInterval);
	DDX_Control(pDX, IDC_BUTTON_VEHICLEOPERATIONSAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnOk);
	DDX_Control(pDX, IDC_BUTTON_CANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_COMBO_STARTDAY, m_comStartDay);
	DDX_Control(pDX, IDC_COMBO_ENDDATE, m_ComEndDay);
}
void CDlgVehicleOperation::UpdateToolBar()
{
	POSITION pos;
	BOOL bEnable = TRUE;
	if (com_OperationType.GetItemData(com_OperationType.GetCurSel())==VehicleType_Public)
	{
		 pos = m_listCtrl.GetFirstSelectedItemPosition();
		 m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_END, true);
	}
	else 
	{
		pos=m_listTaxiAndIterate.GetFirstSelectedItemPosition();
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_END, false);
	}
	
	bEnable = (pos == NULL ? FALSE : TRUE);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL, bEnable);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT, bEnable);
}
int CDlgVehicleOperation::GetSelectedListItem()
{
	POSITION pos;
	if (com_OperationType.GetItemData(com_OperationType.GetCurSel())==VehicleType_Public)
	{
		 pos = m_listCtrl.GetFirstSelectedItemPosition();
		if (pos)
			return m_listCtrl.GetNextSelectedItem(pos);
	}
	else 
	{
		pos = m_listTaxiAndIterate.GetFirstSelectedItemPosition();
		if (pos)
			return m_listTaxiAndIterate.GetNextSelectedItem(pos);
	}
	

	return -1;
}
void CDlgVehicleOperation::OnEditItem()
{
	int nItem = GetSelectedListItem();
	if(com_OperationType.GetItemData(com_OperationType.GetCurSel())==VehicleType_Public)
	{
		//m_listCtrl.EditSubLabel(nItem, 1);
		LandSidePublicVehicleType *type=(LandSidePublicVehicleType *)m_listCtrl.GetItemData(nItem);
		if (type==NULL)
		{
			return;
		}		
		if (nItem==0)
		{
			CDlgEntryExitNodeSelect dlg(m_pObjlist);
			if (dlg.DoModal()==IDOK)
			{
				LandsideExternalNode *pEntryExitNode=dlg.GetEntryExitNode();
				type->SetEntryObjID(pEntryExitNode->getID());
				SetPublicBusStationList(type);
			}

		}else if(nItem==type->getElementCount()+1)
		{
			CDlgEntryExitNodeSelect dlg(m_pObjlist);
			if (dlg.DoModal()==IDOK)
			{
				LandsideExternalNode *pEntryExitNode=dlg.GetEntryExitNode();
				type->SetExitObjID(pEntryExitNode->getID());
				SetPublicBusStationList(type);
			}
		}else
		{
			CDlgBusStationSelect dlg(m_pObjlist);
			if (dlg.DoModal()==IDOK)
			{
				LandsideBusStation *pBusStation=dlg.GetSelectBusStation();
// 				if (type->BusStationExist(pBusStation->getID()))
// 				{
// 					MessageBox(_T("Bus Station Already Exist."));
// 					return;
// 				}
				type->SetBusStation(nItem-1,pBusStation->getID());
				SetPublicBusStationList(type);
			}
		}
	}
	else 
	{
		//m_listTaxiAndIterate.EditSubLabel(nItem,1);
	}
	
	UpdateToolBar();
}
void CDlgVehicleOperation::OnAddEnd()
{
//	VehicleTypeStation *tmpStation;
	int nCurSel=m_listBoxSpecifications.GetCurSel();
	CVehicleItemDetail *pItem = m_pLandSide->getOperationVehicleList().GetItem(nCurSel);
	for(int i=0;i<pItem->getOperationVehicleCount();i++)
	{
		LandSidePublicVehicleType *tmpType=(LandSidePublicVehicleType*)pItem->getOperationVehicleType(i);
		if(tmpType->getVehicleType()==VehicleType_Public)
		{			
			if (tmpType->GetEntryObjID()==-1)
			{
				MessageBox(_T("Please choose Entry first."));
				return;
			}
			CDlgEntryExitNodeSelect dlg(m_pObjlist);
			if (dlg.DoModal()==IDOK)
			{
				LandsideExternalNode *pEntryExitNode=dlg.GetEntryExitNode();
				tmpType->SetExitObjID(pEntryExitNode->getID());
				SetPublicBusStationList(tmpType);
			}			
			break;
		}
	}
}
void  CDlgVehicleOperation::OnNewItem()
{
	CString str;
 	VehicleTypeStation *tmpStation;
	int nCurSel=m_listBoxSpecifications.GetCurSel();
	//LandSideVehicleTypeDistribution *tmpDistribution;
	//LandSideVehicleTypeDistribution *tempDistribution;

	str.Empty();
	 enumVehicleLandSideType m_type=(enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel());
	if (nCurSel<0)
		return;
     CVehicleItemDetail *pItem = m_pLandSide->getOperationVehicleList().GetItem(nCurSel);
	 
	
	 int i=0;
	if(com_OperationType.GetCurSel()>=0)
	{
		switch(m_type)
		{
		case VehicleType_Public:
			 for(i=0;i<pItem->getOperationVehicleCount();i++)
			 {
				 LandSidePublicVehicleType *tmpType=(LandSidePublicVehicleType*)pItem->getOperationVehicleType(i);
				 if(tmpType->getVehicleType()==VehicleType_Public)
				 {
					 if (tmpType->GetEntryObjID()==-1)
					 {
						 CDlgEntryExitNodeSelect dlg(m_pObjlist);
						 if (dlg.DoModal()==IDOK)
						 {
							 LandsideExternalNode *pEntryExitNode=dlg.GetEntryExitNode();
							 tmpType->SetEntryObjID(pEntryExitNode->getID());
							 SetPublicBusStationList(tmpType);
						 }
					 }else
					 {
						CDlgBusStationSelect dlg(m_pObjlist);
						if (dlg.DoModal()==IDOK)
						{
							LandsideBusStation *pBusStation=dlg.GetSelectBusStation();
// 							if (tmpType->BusStationExist(pBusStation->getID()))
// 							{
// 								MessageBox(_T("Bus Station Already Exist."));
// 								return;
// 							}							
							tmpStation=new VehicleTypeStation();
							tmpStation->SetBusStationID(pBusStation->getID());
							tmpType->AddItem(tmpStation);
							SetPublicBusStationList(tmpType);
						}
					 }
					 //tmpType->AddItem(tmpStation);
					 break;
				 }
			 }
			 
			break;
		case VehicleType_Iterate:
			{
				LandSideIterateVehicle *tempType=(LandSideIterateVehicle*)pItem->getOperation(VehicleType_Iterate);
				if(tempType)
				{
					LandSideVehicleTypeDistribution* tempDistribution=new LandSideVehicleTypeDistribution();
					int nItem = m_listTaxiAndIterate.InsertItem(m_listTaxiAndIterate.GetItemCount(),"");					
					SetCtrlItem(nItem,m_listTaxiAndIterate,tempDistribution,GetInputTerminal() );
					tempType->AddItem(tempDistribution);
				}
			}
			break;
		case VehicleType_Taxi:
			{			
				
				LandSideTaxiVehicleType *tempType=(LandSideTaxiVehicleType*)pItem->getOperation(VehicleType_Taxi);
				if(tempType)
				{
					LandSideVehicleTypeDistribution* tempDistribution=new LandSideVehicleTypeDistribution();
					int nItem = m_listTaxiAndIterate.InsertItem(m_listTaxiAndIterate.GetItemCount(),"");					
					SetCtrlItem(nItem,m_listTaxiAndIterate,tempDistribution, GetInputTerminal());
					tempType->AddItem(tempDistribution);
				}
			}		
			break;		
		default:
			break;
		}
	}
	UpdateToolBar();
}



LRESULT CDlgVehicleOperation::OnChangeVehicleType(WPARAM wParam, LPARAM lParam)
{
	
	CStringList strList;
	CString s;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	InputTerminal *interminal= (InputTerminal*)&pDoc->GetTerminal();
	/*s.LoadString( IDS_STRING_NEWDIST );
	strList.InsertAfter( strList.GetTailPosition(), s );*/
	
	LRESULT pResult = 0;
    LV_DISPINFO* pDispInfo=(LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;
   
	int nItem = plvItem->iSubItem;
    
	int nCurSel = m_listBoxSpecifications.GetCurSel();
//	int listandTaxcurSel=m_listTaxiAndIterate.GetLastSelectItem();
	int nSelType = (int)wParam-1;
//	CProbDistEntry* pPBEntry;
  
	assert( interminal->m_pAirportDB );	
	
	if (nCurSel == LB_ERR)
		return FALSE;

     enumVehicleLandSideType m_type=(enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel());
	 if (nCurSel<0)
	 {
		 return FALSE;
	 }

	 CVehicleItemDetail* pItem = m_pLandSide->getOperationVehicleList().GetItem(nCurSel);
	
	 int i=0;
	 CString strText;
	 CString strTaxiAndIterate;
	if(com_OperationType.GetCurSel()>=0)
	{
		
		switch(m_type)
		{
		case VehicleType_Public:
			strText=m_listCtrl.GetItemText(nItem,1);
			
			 for(i=0;i<pItem->getOperationVehicleCount();i++)
			 {
				 LandSidePublicVehicleType *tmpType=(LandSidePublicVehicleType*)pItem->getOperationVehicleType(i);
				 if(tmpType->getVehicleType()==VehicleType_Public)
				 {
					 VehicleTypeStation *tmpStation=tmpType->getStationItem(m_listCtrl.GetCurSel());
					 tmpStation->setStation(atoi(strText));
					 break;
				 }
			 }
			 
			break;		
		default:
			break;
		}
	}

    
    
	return TRUE;
}
void CDlgVehicleOperation::OnBnClickedSave()
{
  OnSave();
  CDialog::OnOK();
  
}
void CDlgVehicleOperation::getGuiValue()
{
	
	int nCurSel = m_listBoxSpecifications.GetCurSel();
	//int listandTaxcurSel=m_listTaxiAndIterate.GetCurSel();
	CVehicleItemDetail* pItem;
	enumVehicleLandSideType m_type;
	if (nCurSel == LB_ERR )
		return ;
	int tmpIndex=0;
	 pItem= m_pLandSide->getOperationVehicleList().GetItem(nCurSel);
	if(isSave)
	{
		m_type=(enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel());
       
	}
	else

	{
		tmpIndex=(int)selectTypes.size()-2;
		if (tmpIndex<0)
		{
			tmpIndex=0;
		}
		m_type=selectTypes.at(tmpIndex);
	}

    if (isListBoxSelChange)

	{
		
		tmpIndex=(int)listboxSeletIndex.size()-2;
		if (tmpIndex<0)
		{
			tmpIndex=0;
		}
		pItem=m_pLandSide->getOperationVehicleList().GetItem(listboxSeletIndex.at(tmpIndex));
	}
	pItem->setLandSideBaseType(m_type);

	int i=0;
	CString strText;
	CString strTaxiAndIterate;
//	int listCount;
	
	switch(m_type)
	{
	case VehicleType_Public:
		for(i=0;i<pItem->getOperationVehicleCount();i++)
		{
			LandSidePublicVehicleType *tmpType=(LandSidePublicVehicleType*)pItem->getOperationVehicleType(i);
			if(tmpType->getVehicleType()==VehicleType_Public)
			{
				//tmpType->setVehicelSpecificationID(pItem->GetID());
				COleDateTime tmpOleTime;
				ElapsedTime tmpElaTime;
				int nDay;

				m_endTime.GetTime(tmpOleTime);
				tmpElaTime.set(tmpOleTime);
				nDay=m_ComEndDay.GetCurSel();
                tmpElaTime.SetDay(nDay+1);
				tmpType->setEndTime(tmpElaTime);

				m_DateStartTime.GetTime(tmpOleTime);
				tmpElaTime.set(tmpOleTime);
				nDay=m_comStartDay.GetCurSel();
				tmpElaTime.SetDay(nDay+1);
				tmpType->setStartTime(tmpElaTime);

				m_EditInterval.GetWindowText(strText);
				int nInterval=atoi(strText);
				if (nInterval<=0)
				{
					MessageBox(_T("Interval must be above 0!"));
					return;
				}
				tmpType->setInterval(nInterval);
// 				for (listCount=0;listCount<m_listCtrl.GetItemCount();listCount++)
// 				{
// 					VehicleTypeStation *tmpStation=tmpType->getStationItem(listCount);
// 					//strText=m_listCtrl.GetItemText(listCount,1);
// 					//tmpStation->setStation(atoi(strText));
// 				}
				
				break;
			}
		}

		break;
	case VehiclType_Private:
		for (i=0;i<pItem->getOperationVehicleCount();i++)
		{
			LandSidePrivateVehicleType *tmpType=(LandSidePrivateVehicleType*)pItem->getOperationVehicleType(i);
			if (tmpType->getVehicleType()==VehiclType_Private)
			{
				//tmpType->setVehicelSpecificationID(pItem->GetID());
				if (m_RadionoPark.GetState())
					tmpType->setParkType(NO_Park);
				if (m_shorPark.GetState())
					tmpType->setParkType(Short_Park);
				if (m_longPark.GetState())
				tmpType->setParkType(Long_Park);
			}
		}

	default:
		break;
	}
	

}
void CDlgVehicleOperation::OnDelItem()

{
	int nCurSel=m_listBoxSpecifications.GetCurSel();
	CVehicleItemDetail* pItem;
	//CVehicleSpecificationItem *pTaxiAndIterate;
    enumVehicleLandSideType m_type=(enumVehicleLandSideType)com_OperationType.GetItemData(com_OperationType.GetCurSel());
	
	if (nCurSel>=0)
	{
		 pItem = m_pLandSide->getOperationVehicleList().GetItem(nCurSel);
	}
	else 
	{
		return;

	}
	

	if(com_OperationType.GetCurSel()>=0)
	{
		switch(m_type)
		{
		case VehicleType_Public:
			nCurSel=m_listCtrl.GetCurSel();
			//m_listCtrl.DeleteItemEx(nCurSel);
			 for(int i=0;i<pItem->getOperationVehicleCount();i++)
			 {
				 LandSidePublicVehicleType *tmpType=(LandSidePublicVehicleType*)pItem->getOperationVehicleType(i);
				 if(tmpType->getVehicleType()==VehicleType_Public)
				 {
					if (nCurSel==0)
					{
						if(tmpType->getElementCount()>0 || tmpType->GetExitObjID()!=-1)
						{
							MessageBox(_T("Delete all bus station and exit first!"));
							return;
						}
                        tmpType->SetEntryObjID(-1);
					}else if (nCurSel>tmpType->getElementCount())
					{
						tmpType->SetExitObjID(-1);
					}else
					{
						tmpType->DeleteItem(nCurSel-1);
					}
					SetPublicBusStationList(tmpType);
					break;
				 }
			 }
			 
			break;
		case VehicleType_Iterate:			
			 if( LandSideIterateVehicle *tmpType=(LandSideIterateVehicle*)pItem->getOperation(VehicleType_Iterate))
			 {				

				IntList ret;
				if( m_listTaxiAndIterate.GetSelectedItems(ret) )
				{
					int sel = *ret.begin();
					m_listTaxiAndIterate.DeleteItem(sel);
					tmpType->DeleteItem(sel);
				}
			 }			
			break;
		case VehicleType_Taxi:			
			 if( LandSideTaxiVehicleType *tmpType=(LandSideTaxiVehicleType*)pItem->getOperation(VehicleType_Taxi))
			 {
				 IntList ret;
				 if( m_listTaxiAndIterate.GetSelectedItems(ret) )
				 {
					 int sel = *ret.begin();
					 m_listTaxiAndIterate.DeleteItem(sel);
					 tmpType->DeleteItem(sel);
				 }
			 }			
			break;
		
		default:
			break;
		}
	}
	UpdateToolBar();

}
void CDlgVehicleOperation::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{

}
void CDlgVehicleOperation::OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateToolBar();
	*pResult = 0;
}
void CDlgVehicleOperation::OnSelchangeListBoxElement()
{

	listboxSeletIndex.push_back(m_listBoxSpecifications.GetCurSel());
	
	isListBoxSelChange=true;
	CVehicleItemDetail *pItem=m_pLandSide->getOperationVehicleList().GetItem(m_listBoxSpecifications.GetCurSel());
	for ( int sideType=0;sideType<com_OperationType.GetCount();sideType++)
	{
		if (pItem->getLandSideBaseType()==com_OperationType.GetItemData(sideType))
		{
			com_OperationType.SetCurSel(sideType);
			onSelChange();
			break;
			
		}
		
	}
	isListBoxSelChange=false;
}
void CDlgVehicleOperation::OnCancel()
{
	m_pLandSide->getOperationVehicleList().ReadDataOperationType(m_pProjID);
	CDialog::OnOK();
}
void CDlgVehicleOperation::OnSave()
{
	isSave=true;
	getGuiValue();
	m_pLandSide->getOperationVehicleList().SaveDataOperationType(m_pProjID);
	
}
BEGIN_MESSAGE_MAP(CDlgVehicleOperation, CDialog)
	ON_WM_SIZE()
	ON_COMMAND(ID_ACTYPE_ALIAS_ADD,OnNewItem)
	ON_COMMAND(ID_ACTYPE_ALIAS_END,OnAddEnd)
	ON_COMMAND(ID_ACTYPE_ALIAS_DEL,OnDelItem)
	ON_COMMAND(ID_ACTYPE_ALIAS_EDIT,OnEditItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON_VEHICLEOPERATIONSAVE,OnSave)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL,OnCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_OPERATIONTYPE,onSelChange)
	ON_LBN_DBLCLK(IDC_LIST_SPECIFICATIONDATA,OnDblclkListBox)
    ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnChangeVehicleType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_OPERATIONDATA, OnLvnItemchangedListContents)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_TAXIANDITERATE,OnLvnItemchangedListContents)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_OPERATIONDATA, OnLvnEndlabeleditListContents)
	ON_LBN_SELCHANGE(IDC_LIST_SPECIFICATIONDATA, OnSelchangeListBoxElement)
	ON_COMMAND(ID_PEOPLEMOVER_NEW,OnNewItem)
	ON_COMMAND(ID_PEOPLEMOVER_CHANGE,OnEditItem)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE,OnDelItem)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPERATIONDATA, OnNMDblclkListOperationdata)
END_MESSAGE_MAP()

void CDlgVehicleOperation::OnNMDblclkListOperationdata(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	OnEditItem();
}

InputTerminal* CDlgVehicleOperation::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	InputTerminal *interminal= (InputTerminal*)&pDoc->GetTerminal();
	return interminal;
}

// CDlgVehicleOperation message handlers
