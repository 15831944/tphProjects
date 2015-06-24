// PaxBulkListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxBulkListDlg.h"
#include "inputs\paxbulkinfo.h"
#include "engine\terminal.h"
#include "TermPlanDoc.h"
#include "PassengerTypeDialog.h"
#include "paxbulkedit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxBulkListDlg dialog


CPaxBulkListDlg::CPaxBulkListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPaxBulkListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxBulkListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPaxBulk = new CPaxBulkInfo();
}

CPaxBulkListDlg::~CPaxBulkListDlg()
{
}	

void CPaxBulkListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxBulkListDlg)
	DDX_Control(pDX, IDC_STATICTOOLBAR, m_staticToolBar);
	DDX_Control(pDX, IDC_LIST, m_PaxBulkListCtr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxBulkListDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxBulkListDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBTPAXBULKADD, OnProcessorDataAdd)
	ON_COMMAND(ID_TOOLBTPAXBULKDEL, OnToolbtpaxbulkdel)
	ON_COMMAND(ID_TOOLBTPAXBULKEDIT, OnToolbtpaxbulkedit)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_BUTTONSAVE, OnButtonsave)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxBulkListDlg message handlers

BOOL CPaxBulkListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CRect rc;
	m_staticToolBar.GetWindowRect( &rc );
	ScreenToClient(&rc);
 	m_ToolBar.MoveWindow(&rc);
	m_staticToolBar.ShowWindow(SW_HIDE);
	m_ToolBar.ShowWindow(SW_SHOW);

	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKADD,TRUE);	
//	m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBTPAXBULKEDIT );	
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKEDIT,FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKDEL,FALSE);
//////////////////////////////////////////////////////////////////////////
//SetListCtrl start
	DWORD dwStyle = m_PaxBulkListCtr.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_PaxBulkListCtr.SetExtendedStyle( dwStyle );

	CStringList strList;
	strList.AddTail("ARCterm");

//	LV_COLUMNEX	lvc;
	LVCOLUMN lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char* columnLabel[9] = {
							"Passeger Type",
							"Capacity",
							"Frequency ",
							"Start Day",
							"Start Time",
							"End Day",
							"End Time",
							"Range Start",
							"Range End"
						};

	int DefaultColumnWidth[9] = { 105, 70, 70, 40, 70, 40, 70, 70, 70};
	int nFormat[9] = {
					LVCFMT_CENTER|LVCFMT_NOEDIT,    //"Passeger Type",
					LVCFMT_SPINEDIT|LVCFMT_CENTER,  //"Capacity",
					LVCFMT_DATETIME|LVCFMT_CENTER,  //"Frequency ",
					LVCFMT_SPINEDIT|LVCFMT_CENTER,  //"Start Day",
					LVCFMT_DATETIME|LVCFMT_CENTER,  //"Start Time",
					LVCFMT_SPINEDIT|LVCFMT_CENTER,  //"End Day",
					LVCFMT_DATETIME|LVCFMT_CENTER,  //"End Time",
					LVCFMT_DATETIME|LVCFMT_CENTER,  //"Range Start",
					LVCFMT_DATETIME|LVCFMT_CENTER   //"Range End"
				};	
	for( int i=0; i<9; i++ )
	{
		//lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.fmt = nFormat[i];
		m_PaxBulkListCtr.InsertColumn(i, &lvc );
	}	

//	m_PaxBulkListCtr.SetHHmmTimeFormat();
//	m_PaxBulkListCtr.SetSpinDisplayType(1);	
//	m_PaxBulkListCtr.SetEditSpinRange(1,29999);
//SetListCtrl end;
//////////////////////////////////////////////////////////////////////////
	
 	LoadData();
	ReloadList();
	GetDlgItem(IDC_BUTTONSAVE)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CPaxBulkListDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_PAXBULK))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}


void CPaxBulkListDlg::LoadData()
{
   GetInputTerminal()->m_pPaxBulkInfo->loadDataSet(GetProjPath());	
   m_pPaxBulk =	 GetInputTerminal()->m_pPaxBulkInfo;
}

InputTerminal* CPaxBulkListDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

CString CPaxBulkListDlg::GetProjPath()
{
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
	return pDoc->m_ProjInfo.path;
}

void CPaxBulkListDlg::OnOK() 
{
	// TODO: Add extra validation here
	GetListData();	
	m_pPaxBulk->SortData() ;
	m_pPaxBulk->saveDataSet(GetProjPath(),TRUE);
	CDialog::OnOK();
}

void CPaxBulkListDlg::ReloadList()
{
	m_PaxBulkListCtr.DeleteAllItems();
	m_vBulkPercent.clear();
	int nNum=m_pPaxBulk->m_vectBulkInfo.size();
	for( int i=0; i<nNum ; i++ )
	{
		CString strDay;
		ElapsedTime elapsedTime;
		const CPaxBulk& paxBulk = m_pPaxBulk->m_vectBulkInfo[i];
		CString ctemp;//[256];
		paxBulk.m_mobileElemConst.screenPrint(ctemp,0,256);
		m_PaxBulkListCtr.InsertItem(i,ctemp);
		m_PaxBulkListCtr.SetItemText(i,1,itoa(paxBulk.m_nCapacity,ctemp.GetBuffer(0),10));
		m_PaxBulkListCtr.SetItemText(i,2,paxBulk.m_TimeFrequency.printTime());

		elapsedTime = paxBulk.m_TimeStart;
		strDay.Format(_T("%d"), elapsedTime.GetDay());
		elapsedTime.SetDay(1);
		m_PaxBulkListCtr.SetItemText(i,3,strDay);  //"Start Day",
		m_PaxBulkListCtr.SetItemText(i,4,elapsedTime.printTime());

		elapsedTime = paxBulk.m_TimeEnd;
		strDay.Format(_T("%d"), elapsedTime.GetDay());
		elapsedTime.SetDay(1);
		m_PaxBulkListCtr.SetItemText(i,5,strDay);  //"End Day",
		m_PaxBulkListCtr.SetItemText(i,6,elapsedTime.printTime());

		m_PaxBulkListCtr.SetItemText(i,7,paxBulk.m_TimeRangeBegin.printTime());
		m_PaxBulkListCtr.SetItemText(i,8,paxBulk.m_TimeRangeEnd.printTime());

		m_vBulkPercent.push_back(MakeString(paxBulk.m_vBulkPercent));
	}
}

void CPaxBulkListDlg::OnToolbtpaxbulkdel() 
{
	// TODO: Add your command handler code here
	POSITION pos = m_PaxBulkListCtr.GetFirstSelectedItemPosition();
	int idx = m_PaxBulkListCtr.GetNextSelectedItem(pos);
	if(idx != -1) 
	{
		int nPos=0;
		std::vector< CPaxBulk >::iterator iter;
		for(iter=m_pPaxBulk->m_vectBulkInfo.begin();iter!=m_pPaxBulk->m_vectBulkInfo.end();iter++, nPos++)
		{
			if(nPos==idx)
			{
				m_pPaxBulk->m_vectBulkInfo.erase(iter);
				break;
			}
		}
	}
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKDEL,FALSE);	
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKEDIT,FALSE);	
	
	ReloadList();
	GetDlgItem(IDC_BUTTONSAVE)->EnableWindow(TRUE);				
}

void CPaxBulkListDlg::OnToolbtpaxbulkedit() 
{
	// TODO: Add your command handler code here
	EditOneBulk();
}

void CPaxBulkListDlg::OnProcessorDataAdd() 
{
	CPassengerTypeDialog dlg(m_pParentWnd);
	if( dlg.DoModal() == IDOK )
	{
		CMobileElemConstraint mobileConstr ( dlg.GetMobileSelection() );

		CPaxBulkEdit editDlg;
		if(editDlg.DoModal()==IDOK)
		{
			CPaxBulk addBulk;
		    addBulk.m_mobileElemConst = dlg.GetMobileSelection();
		    addBulk.m_nCapacity = editDlg.m_Capacity;
			addBulk.m_TimeFrequency.set(editDlg.m_FrequencyTime.GetHour(),editDlg.m_FrequencyTime.GetMinute(),editDlg.m_FrequencyTime.GetSecond());
			addBulk.m_TimeStart.set(editDlg.m_freqStartTime);
			addBulk.m_TimeEnd.set(editDlg.m_freqEndTime);
			if(editDlg.m_BeforeCheck)
			{
				addBulk.m_TimeRangeBegin.set(editDlg.m_BeginRangeTime.GetHour(),editDlg.m_BeginRangeTime.GetMinute(),editDlg.m_BeginRangeTime.GetSecond());
				addBulk.m_TimeRangeEnd.set(editDlg.m_EndRangeTime.GetHour(),editDlg.m_EndRangeTime.GetMinute(),editDlg.m_EndRangeTime.GetSecond());
			    long secBegn = addBulk.m_TimeRangeBegin.asSeconds();
                long secEnd = addBulk.m_TimeRangeEnd.asSeconds();
				addBulk.m_TimeRangeBegin.set(-secBegn);
				addBulk.m_TimeRangeEnd.set(-secEnd);
			}
			else
			{
				addBulk.m_TimeRangeBegin.set(editDlg.m_BeginRangeTime.GetHour(),editDlg.m_BeginRangeTime.GetMinute(),editDlg.m_BeginRangeTime.GetSecond());
				addBulk.m_TimeRangeEnd.set(editDlg.m_EndRangeTime.GetHour(),editDlg.m_EndRangeTime.GetMinute(),editDlg.m_EndRangeTime.GetSecond());
			}
			addBulk.m_vBulkPercent = editDlg.m_vBulkPercent;
			m_pPaxBulk->m_vectBulkInfo.push_back( addBulk );
            
			m_vBulkPercent.push_back(MakeString(editDlg.m_vBulkPercent));
		    ReloadList();
		}
	}		
}

void CPaxBulkListDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_PaxBulkListCtr.GetFirstSelectedItemPosition();
	int idx = m_PaxBulkListCtr.GetNextSelectedItem(pos);
	if(idx != -1) 
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKEDIT,TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKDEL,TRUE);			
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKEDIT,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKDEL,FALSE);			
	}		
	*pResult = 0;
}


void CPaxBulkListDlg::EditOneBulk()
{
	POSITION pos = m_PaxBulkListCtr.GetFirstSelectedItemPosition();
	int idx = m_PaxBulkListCtr.GetNextSelectedItem(pos);
	if(idx>=0)
	{
		CString strCapacity = m_PaxBulkListCtr.GetItemText( idx, 1 );
		CString strFrequecy = m_PaxBulkListCtr.GetItemText(idx,2);
		CString strStartDay = m_PaxBulkListCtr.GetItemText(idx,3);  // "Start Day",
		CString strStartTime = m_PaxBulkListCtr.GetItemText(idx,4);
		CString strEndDay = m_PaxBulkListCtr.GetItemText(idx,5);    // "End Day",
		CString strEndTime = m_PaxBulkListCtr.GetItemText(idx,6);
		CString strRangeBegin = m_PaxBulkListCtr.GetItemText(idx,7);
		CString strRangeEnd =m_PaxBulkListCtr.GetItemText(idx,8);

		CPaxBulkEdit editDlg;
		editDlg.m_Capacity = atoi(strCapacity);
		editDlg.m_FrequencyTime.ParseDateTime(strFrequecy);

		editDlg.m_freqStartTime.ParseDateTime(strStartTime);
		editDlg.m_freqStartTime.m_dt += (atoi(strStartDay) - 1);// "Start Day",
		ASSERT(COleDateTime::valid == editDlg.m_freqStartTime.m_status);

		editDlg.m_freqEndTime.ParseDateTime(strEndTime);
		editDlg.m_freqEndTime.m_dt += (atoi(strEndDay) - 1);// "End Day",
		ASSERT(COleDateTime::valid == editDlg.m_freqEndTime.m_status);

		editDlg.m_BeforeCheck = false;
		if(strRangeBegin.GetAt(0)=='-')
		{
			editDlg.m_BeforeCheck = true;
			CString temp =strRangeBegin.Right(strRangeBegin.GetLength()-1);
			editDlg.m_BeginRangeTime.ParseDateTime(temp);
		}
		else
		{
			editDlg.m_BeginRangeTime.ParseDateTime(strRangeBegin);
		}
		if(strRangeEnd.GetAt(0)=='-')
		{
			editDlg.m_BeforeCheck = true;
			CString temp;
			temp =strRangeEnd.Right(strRangeEnd.GetLength()-1);	
			editDlg.m_EndRangeTime.ParseDateTime(temp);
		}
		else
		{
			editDlg.m_EndRangeTime.ParseDateTime(strRangeEnd);
		}
		editDlg.m_vBulkPercent = ParseString(m_vBulkPercent[idx]);
		if(editDlg.DoModal()==IDOK)
		{
			char temp[20];
			itoa(editDlg.m_Capacity,temp,10);
			strCapacity =temp;
			strFrequecy = editDlg.m_FrequencyTime.Format("%H:%M:%S");

			COleDateTime dateTime;
			int day = 1;

			dateTime = editDlg.m_freqStartTime;
			day = (int)(dateTime.m_dt);
			strStartDay.Format(_T("%d"), day + 1); // "Start Day",
			dateTime.m_dt -= day;
			strStartTime = dateTime.Format("%H:%M:%S");

			dateTime = editDlg.m_freqEndTime;
			day = (int)(dateTime.m_dt);
			strEndDay.Format(_T("%d"), day + 1); // "End Day",
			dateTime.m_dt -= day;
			strEndTime = dateTime.Format("%H:%M:%S");

			if(editDlg.m_BeforeCheck)
			{
				strRangeBegin =editDlg.m_BeginRangeTime.Format("-%H:%M:%S");
				strRangeEnd = editDlg.m_EndRangeTime.Format("-%H:%M:%S");
			}
			else
			{
				strRangeBegin =editDlg.m_BeginRangeTime.Format("%H:%M:%S");
				strRangeEnd = editDlg.m_EndRangeTime.Format("%H:%M:%S");
			}
			
			m_vBulkPercent[idx] = MakeString(editDlg.m_vBulkPercent);
			m_PaxBulkListCtr.SetItemText(idx, 1,strCapacity );
			m_PaxBulkListCtr.SetItemText(idx,2,strFrequecy);
			m_PaxBulkListCtr.SetItemText(idx,3,strStartDay); // "Start Day",
			m_PaxBulkListCtr.SetItemText(idx,4,strStartTime);
			m_PaxBulkListCtr.SetItemText(idx,5,strEndDay); // "End Day",
			m_PaxBulkListCtr.SetItemText(idx,6,strEndTime);
			m_PaxBulkListCtr.SetItemText(idx,7,strRangeBegin);
			m_PaxBulkListCtr.SetItemText(idx,8,strRangeEnd);
			
		}
    }
}
void CPaxBulkListDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	EditOneBulk();
	GetDlgItem(IDC_BUTTONSAVE)->EnableWindow(TRUE);									
	*pResult = 0;
}


void CPaxBulkListDlg::OnButtonsave() 
{
	// TODO: Add your control notification handler code here
	GetListData();
	m_pPaxBulk->saveDataSet(GetProjPath(),TRUE);	
	GetDlgItem(IDC_BUTTONSAVE)->EnableWindow(FALSE);	
}

void CPaxBulkListDlg::GetListData()
{
	for(int nItem=0;nItem< m_PaxBulkListCtr.GetItemCount(); nItem++)
	{
		CPaxBulk& paxBulk = m_pPaxBulk->m_vectBulkInfo[nItem];
		CString strCapacity;
		CString strDay;
		int day = 1;
		CString strTime;

		// "Capacity"
		int nCol =1;
		strCapacity = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		paxBulk.m_nCapacity = atoi(strCapacity);
		// "Frequency"
		nCol++;
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		paxBulk.m_TimeFrequency.SetTime(strTime);

		// "Start Day",
		nCol++;
		strDay = m_PaxBulkListCtr.GetItemText(nItem, nCol);
		day = atoi(strDay);
		nCol++;
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		paxBulk.m_TimeStart.SetTime(strTime);
		paxBulk.m_TimeStart.SetDay(day);
		// "End Day",
		nCol++;
		strDay = m_PaxBulkListCtr.GetItemText(nItem, nCol);
		day = atoi(strDay);
		nCol++;		
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		paxBulk.m_TimeEnd.SetTime(strTime);
		paxBulk.m_TimeEnd.SetDay(day);

		// "Range Start Time"
		nCol++;		
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		paxBulk.m_TimeRangeBegin.SetTime(strTime);
		// "Range End Time"
		nCol++;		
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		paxBulk.m_TimeRangeEnd.SetTime(strTime);

		paxBulk.m_vBulkPercent = ParseString(m_vBulkPercent[nItem]);
	}	
}

void CPaxBulkListDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
/*
	POSITION pos = m_PaxBulkListCtr.GetFirstSelectedItemPosition();
	int idx = m_PaxBulkListCtr.GetNextSelectedItem(pos);
	if(idx != -1) 
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKDEL,TRUE);			
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBTPAXBULKDEL,FALSE);			
	}		
	
/*	int nItem;//=//m_PaxBulkListCtr.GetLastSel();
	//////////////////////////////////////////////////////////////////////////
	//check the Input data
	if ( nItem >= 0)
	{	
		int nCol =1;
		CString strCapacity;
		strCapacity = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		int nCap = atoi(strCapacity);	
		if (nCap <= 0) 
		{
			MessageBox( "Capacity Less One !","Input Data error",MB_ICONWARNING);
			return ;		
		}		
		nCol++;
		CString strTime;
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		COleDateTime timeFrq;
		timeFrq.ParseDateTime(strTime);

		nCol++;		
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		COleDateTime timeStart;
		timeStart.ParseDateTime(strTime);

		nCol++;		
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		COleDateTime timeEnd;
		timeEnd.ParseDateTime(strTime);

		nCol++;		
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		COleDateTime timeRangeB;
		timeRangeB.ParseDateTime(strTime);

		nCol++;		
		strTime = m_PaxBulkListCtr.GetItemText( nItem, nCol );
		if (strcmp( strTime ,"") == 0)	strTime="00:00:00";
		strTime=strTime+":0";
		COleDateTime timeRangeE;
		timeRangeE.ParseDateTime(strTime);

		CString strErr="";
		strErr.Format("Error at %d row \n",nItem+1);
		
		if (timeFrq.GetHour() == 0 && timeFrq.GetMinute() <= 0)
		{
			MessageBox( strErr+"Frequence should more than One Minute!","Input Data invalid",MB_ICONWARNING);
        //////////////////////////////////////////////////////////////////////////
		//undo , reload the last correct data to the ListCtrl
			ReloadList();  
			return ;		
		}
		if (timeStart >= timeEnd)
		{
			MessageBox( strErr+"Start Time should less than End Time","Input Data invalid",MB_ICONWARNING);
			ReloadList();
			return ;				
		}
		if (timeRangeB >= timeRangeE)
		{
			MessageBox( strErr+"TimeRange Begin should less than TimeRange End","Input Data invalid",MB_ICONWARNING);
			ReloadList();
			return ;						
		}
		GetListData(); //Save current data
	}*/
	CDialog::OnLButtonDown(nFlags, point);

}

void CPaxBulkListDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnLButtonDown(nFlags,point) ;
	CDialog::OnRButtonDown(nFlags, point);
}

CString CPaxBulkListDlg::MakeString( const std::vector<int>& vPercent )
{
	CString strPercent;
	int nCount = (int)vPercent.size();
	for (int i = 0; i < nCount; i++)
	{
		int iPercent = vPercent.at(i);
		if (strPercent.IsEmpty())
		{
			strPercent.Format(_T("%d"),iPercent);
		}
		else
		{
			CString strValue;
			strValue.Format(_T("%d"),iPercent);
			strPercent += CString(_T(",")) + strValue;
		}
	}
	return strPercent;
}

std::vector<int> CPaxBulkListDlg::ParseString( const CString& strPercent )
{
	CString strLeft;
	CString strRight = strPercent;
	std::vector<int> vPercent;
	int nPos = strPercent.Find(',');
	while(nPos != -1)
	{
		strLeft = strRight.Left(nPos);
		vPercent.push_back(atoi(strLeft));
		strRight = strRight.Right(strRight.GetLength() - nPos - 1);
		nPos = strRight.Find(',');
	}

	if (strRight.IsEmpty() == FALSE)
	{
		vPercent.push_back(atoi(strRight));
	}
	return vPercent;
}
