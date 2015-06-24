// RosterList.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "RosterList.h"
#include ".\rosterlist.h"
#include "termplan.h"

#include "TermPlanDoc.h"
#include "ProcesserDialog.h"
#include "PassengerTypeDialog.h"
#include "AssignFlightDlg.h"
#include "singleEventsDataDlg.h"
#include "rosterwizarddlg.h"
#include "..\inputs\AssignDB.h"
#include "..\inputs\Assign.h"
#include "..\inputs\Schedule.h"
#include "..\common\WinMsg.h"
#include "..\engine\AutoRoster.h"
#include ".\procassigndlg.h"
#include "../engine/proclist.h"
// CRosterList dialog

IMPLEMENT_DYNAMIC(CRosterList, CTabPageSSL)
CRosterList::CRosterList(CProcAssignDlg* Prodlg ,CWnd* pParent /*=NULL*/)
	:m_mainDlg(Prodlg), CTabPageSSL(CRosterList::IDD, pParent)
{
	m_RosterListColNames.push_back(_T("Processor")) ;
	m_RosterListColNames.push_back(_T("Pax Type")) ;
	m_RosterListColNames.push_back(_T("Open Time")) ;
	m_RosterListColNames.push_back(_T("Close Time")) ;
	m_RosterListColNames.push_back(_T("Assignment Relation")) ;
	m_RosterListColNames.push_back(_T("IsDaily")) ;
}

CRosterList::~CRosterList()
{
}

void CRosterList::DoDataExchange(CDataExchange* pDX)
{
	CTabPageSSL::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ROSTERS, m_rosterList);
}


BEGIN_MESSAGE_MAP(CRosterList, CTabPageSSL)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickList3)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROSTERS,OnSelListItem)
	/*ON_COMMAND(ID_PROCASSIGN_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PROCASSIGN_DELETE, OnPeoplemoverDelete)
	ON_COMMAND(ID_PROCASSIGN_EDIT, OnPeoplemoverChange)
*/
	ON_COMMAND(ID_BUTTON_ROSTEREXPORT,ExportFileFromRosterList)
	ON_COMMAND(ID_BUTTON_ROSTERPRINT,OnPrintRosterList)
	ON_WM_CREATE() 
	ON_WM_SIZE()
	//ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipText ) 
END_MESSAGE_MAP()
void CRosterList::OnSelListItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	//m_RosterToolBar.GetToolBarCtrl().EnableButton( ID_PROCASSIGN_DELETE ,TRUE);	
	//m_RosterToolBar.GetToolBarCtrl().EnableButton( ID_PROCASSIGN_EDIT, TRUE);
}
BOOL CRosterList::OnInitDialog() 
{
	CTabPageSSL::OnInitDialog();
	InitRosterToolBar() ;
     InitRosterList() ;


    SetResize(m_RosterToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	 SetResize(m_rosterList.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	 SetResize(IDC_STATIC_HOR,SZ_TOP_LEFT,SZ_TOP_RIGHT) ;
	 return true ;
}
// CRosterList message handlers
int CRosterList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTabPageSSL::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here

	if (!m_RosterToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_PROCASSIGN) ||
		!m_RosterToolBar.LoadToolBar(IDR_TOOLBAR_ROSTER))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}
void CRosterList::OnSize(UINT nType, int cx, int cy) 
{
	CTabPageSSL::OnSize(nType, cx, cy);
	//cx
	//	Specifies the new width of the client area.

	//cy
	//	Specifies the new height of the client area.
	if(m_rosterList.m_hWnd)
	{
		m_rosterList.ResizeColumnWidth() ;
	}
}
void CRosterList::InitRosterToolBar()
{
	CRect rc ; 
	m_rosterList.GetWindowRect(&rc) ;
	ScreenToClient(&rc);
	rc.top -= 30;
	rc.bottom = rc.top + 22;
	rc.left += 4;
	m_RosterToolBar.MoveWindow(&rc) ;
	m_RosterToolBar.ShowWindow(SW_SHOW) ;
	m_RosterToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_ROSTEREXPORT ,TRUE);	
	m_RosterToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_ROSTERPRINT ,TRUE);	

}
void CRosterList::OnHdnItemclickList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
void CRosterList::InitRosterList()
{
	InitRosterListView() ;
	InitRosterListData();
	
}
void CRosterList::InitRosterListView()
{

	DWORD dwStyle = m_rosterList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_rosterList.SetExtendedStyle( dwStyle );
	int nwidth[] = {200,200,120,120,140,85} ;
	for (int i = 0 ; i < (int)m_RosterListColNames.size() ; i++)
	{
		m_rosterList.InsertColumn(i,m_RosterListColNames[i],LVCFMT_CENTER,nwidth[i]) ;
	}
	m_rosterList.InitColumnWidthPercent();
}
void CRosterList::InitALLProcessorData()
{
	m_rosterList.DeleteAllItems() ;
	HTREEITEM root = m_mainDlg->m_treeProc.GetRootItem() ;
	HTREEITEM child = m_mainDlg->m_treeProc.GetChildItem(root);
	while(child)
	{
		InitRosterListData(child) ;
		child = m_mainDlg->m_treeProc.GetNextSiblingItem(child) ;
	}
   m_rosterList.SetFocus() ;
}
void CRosterList::InitRosterListData()
{
		m_rosterList.DeleteAllItems() ;
        InitRosterListData(m_mainDlg->m_treeProc.GetSelectedItem()) ;
}
void CRosterList::InitRosterListData(HTREEITEM ProcessNode )
{
	if(ProcessNode == NULL)
		return;
	// sort processor name in gt_ctrl
	std::vector<ProcessorID> vectProcID;
	if(m_mainDlg->m_treeProc.GetChildItem(ProcessNode) == NULL)
	{
		 InsertRosterOfProcessor(ProcessNode) ;
		 return ;
	}else
	{
		HTREEITEM pchild = m_mainDlg->m_treeProc.GetChildItem(ProcessNode) ;
		while(pchild)
		{ 
            InitRosterListData(pchild) ;
			pchild = m_mainDlg->m_treeProc.GetNextSiblingItem(pchild) ;
		}
	}
	m_rosterList.SetFocus() ;
}
void CRosterList::InsertRosterOfProcessor(HTREEITEM ProcessNode )
{
    ProcessorID _id ;
	if(!m_mainDlg->GetCurrentSelProcessID(ProcessNode,_id))
		return ;
 	ItemRosterList _data ;
		ProcessorRosterSchedule* _schedule = NULL ;
	int ndx = GetInputTerminal()->procAssignDB->findEntry(_id) ;
	if(ndx == INT_MAX)
	{
		 ProcessorID id ;
		if(!m_mainDlg->FindDefinedRosterProcessor(ProcessNode,id) )
		{
			ElapsedTime time ;
			_data.c_Processor = _id.GetIDString() ;
			_data.c_PaxTy = _T("DEFAULT") ;
			_data.c_openTime = time.printTime() ;
			time = time + (WholeDay-1);
			_data.c_closeTime = time.printTime() ;
			_data.c_relation =m_mainDlg->GetRelationName(Inclusive) ;
			_data.c_isDaily = m_mainDlg->GetIsDailyName(TRUE) ;
			AddLineToRosterList(_data,NULL) ;
			return ;
		}else
		{
           ndx = GetInputTerminal()->procAssignDB->findEntry(id) ;
		   ASSERT(ndx != INT_MAX);
		   if(ndx == INT_MAX)
			   return ;
           _schedule = GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
		}
	}else
	{
		ASSERT(ndx != INT_MAX);
		_schedule = GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
		if(_schedule->getCount() == 0)
		{
			ProcessorID curID;
			if(!m_mainDlg->FindDefinedRosterProcessor(ProcessNode,curID) )
			{
				ElapsedTime time ;
				_data.c_Processor = _id.GetIDString() ;
				_data.c_PaxTy = _T("DEFAULT") ;
				_data.c_openTime = time.printTime() ;
				time = time + (WholeDay-1);
				_data.c_closeTime = time.printTime() ;
				_data.c_relation =m_mainDlg->GetRelationName(Inclusive) ;
				_data.c_isDaily = m_mainDlg->GetIsDailyName(TRUE) ;
				AddLineToRosterList(_data,NULL) ;
				return ;
			}else
			{
				ndx = GetInputTerminal()->procAssignDB->findEntry(curID) ;
				ASSERT(ndx != INT_MAX);
				if(ndx == INT_MAX)
					return ;
				_schedule = GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
			}
		}
	}
	if(_schedule == NULL)
		return ;
	ProcessorRoster* _ass = NULL;
	for (int i =0 ; i < _schedule->getCount() ; i++)
	{

		_ass = _schedule->getItem(i) ;
		if(_ass->getFlag() == 0)
			break ;
		_data.c_Processor = _id.GetIDString() ;
		_ass->getAssignment()->getConstraint(0)->screenPrint( _data.c_PaxTy) ;
		_data.c_closeTime = _ass->getAbsCloseTime().printTime() ;
		_data.c_openTime = _ass->getAbsOpenTime().printTime() ;
		_data.c_relation =m_mainDlg->GetRelationName(_ass->getAssRelation()) ;
		_data.c_isDaily = m_mainDlg->GetIsDailyName(_ass->IsDaily()) ;
		AddLineToRosterList(_data,_ass) ;

// 		m_mainDlg->m_treeProc.SetItemColor( ProcessNode, RGB(53,151,53) );
// 		m_mainDlg->m_treeProc.SetItemBold( ProcessNode, true );
	}

}
void CRosterList::AddLineToRosterList( ItemRosterList& _data,ProcessorRoster* _PAss) 
{
	int ndx = m_rosterList.InsertItem(0,_data.c_Processor) ;
	m_rosterList.SetItemText(ndx,1,_data.c_PaxTy) ;
	m_rosterList.SetItemText(ndx,2,_data.c_openTime) ;
	m_rosterList.SetItemText(ndx,3,_data.c_closeTime ) ;
	m_rosterList.SetItemText(ndx,4,_data.c_relation) ;
	m_rosterList.SetItemText(ndx,5,_data.c_isDaily) ;
	m_rosterList.SetItemData(ndx,(DWORD_PTR)_PAss) ;
}
//void CRosterList::DeleteSelLineFromRosterList() 
//{
//	POSITION posi = m_rosterList.GetFirstSelectedItemPosition() ;
//	if(posi!= NULL)
//	{
//		int ndx = m_rosterList.GetNextSelectedItem(posi) ;
//		m_rosterList.DeleteItem(ndx) ;
//	}
//}
//void CRosterList::EditSelRosterFromRosterList(ItemRosterList& _data) 
//{
//	POSITION posi = m_rosterList.GetFirstSelectedItemPosition() ;
//	if(posi!= NULL)
//	{
//		int ndx = m_rosterList.GetNextSelectedItem(posi) ;
//        m_rosterList.SetItemText(ndx,0,_data.c_Processor) ;
//		m_rosterList.SetItemText(ndx,1,_data.c_PaxTy) ;
//		m_rosterList.SetItemText(ndx,2,_data.c_openTime) ;
//		m_rosterList.SetItemText(ndx,3,_data.c_closeTime ) ;
//		m_rosterList.SetItemText(ndx,4,_data.c_relation) ;
//		m_rosterList.SetItemText(ndx,5,_data.c_isDaily) ;
//	}
//}
//
//void CRosterList::OnPeoplemoverNew() 
//{
//	ProcessorID id ;
//	if(!m_mainDlg->GetCurrentSelProcessID(id))
//		return ;
//	ProcAssignment* pProcAssn = new ProcAssignment( GetInputTerminal() );
//	pProcAssn->setCloseTime((ElapsedTime)(86400.0 *3 -1));
//	pProcAssn->setAbsCloseTime((ElapsedTime)(86400.0 *3 -1));
//
//	int nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
//	assert( nIdx != INT_MAX );
//	if(nIdx == INT_MAX)
//		GetInputTerminal()->procAssignDB->addEntry(id ) ;
//   nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
//	Processor* p_pro = GetInputTerminal()->GetProcessorList()->getProcessor(id) ;
//	if(p_pro == NULL)
//		return ;
//	ProcAssignSchedule* pSchd = GetInputTerminal()->procAssignDB->getDatabase( nIdx );
//	CsingleEventsDataDlg dlg( m_pParentWnd ,TRUE ,p_pro,pSchd);
//	dlg.SetDlgData( id.GetIDString(), *pProcAssn );
//	if( dlg.DoModal()!=IDOK)
//	{
//		delete pProcAssn;
//		return;
//	}
//	dlg.GetDlgData( *pProcAssn);
//	//////////////////////////////////////////////////////////////////////////
//	if( (!pSchd->processorIsClose() && pProcAssn->isOpen() == 0)			//(open->close) now delete all other event
//		|| (pSchd->processorIsClose() && pProcAssn->isOpen()) )			//(close->open) now delete all old event( close event ) 
//	{
//		pSchd->clear();
//		pSchd->ClearAllDailyData() ;
//	}
//	else if( pSchd->processorIsClose() && pProcAssn->isOpen() == 0 )	//(close->close) can't do this
//	{
//		MessageBox("The processor already closed!",NULL,MB_OK|MB_ICONINFORMATION );
//		delete pProcAssn;	
//		return;
//	}
//	pSchd->addItem(pProcAssn) ;
//	ItemRosterList _RosterItem ;
//	_RosterItem.c_Processor = id.GetIDString() ;
//	const CMultiMobConstraint* pMultiConst = pProcAssn->getAssignment();	
//	const CMobileElemConstraint* pConst = pMultiConst->getConstraint( 0 );
//	pConst->screenPrint(_RosterItem.c_PaxTy) ;
//	_RosterItem.c_openTime = pProcAssn->getAbsOpenTime().printTime() ;
//	_RosterItem.c_closeTime = pProcAssn->getAbsCloseTime().printTime() ;
//	_RosterItem.c_relation = m_mainDlg->GetRelationName(pProcAssn->getAssRelation()) ;
//	_RosterItem.c_isDaily = m_mainDlg->GetIsDailyName(pProcAssn->IsDaily()) ;
//	AddLineToRosterList(_RosterItem ,pProcAssn) ;
//}
//
//
//void CRosterList::OnPeoplemoverDelete() 
//{
//
//	POSITION  posi = m_rosterList.GetFirstSelectedItemPosition() ;
//	ProcAssignment* p_Ass = NULL ;
//	if(posi == NULL)
//		return ;
//	int ndx = 0 ;
//	ndx =  m_rosterList.GetNextSelectedItem(posi) ;
//	p_Ass = (ProcAssignment*)m_rosterList.GetItemData(ndx) ;
//	if(p_Ass == NULL)
//		//delete GT 
//	{
//		return ;
//	}
//	ProcAssignSchedule* pProcSchd = GetCurSelProcSchdFromRosterList();
//	if( !pProcSchd )
//		return;
//	// delete the event
//	int index = pProcSchd->findItem(p_Ass) ;
//	if(index == INT_MAX)
//		return ;
//	pProcSchd->RemoveProcAssignment(p_Ass) ;
//	DeleteSelLineFromRosterList() ;
//}
//
//void CRosterList::OnPeoplemoverChange() 
//{
//	POSITION  posi = m_rosterList.GetFirstSelectedItemPosition() ;
//	ProcAssignment* p_procAssn = NULL ;
//	if(posi == NULL)
//		return ;
//	int ndx = 0 ;
//	ndx =  m_rosterList.GetNextSelectedItem(posi) ;
//	p_procAssn = (ProcAssignment*)m_rosterList.GetItemData(ndx) ;
//	if(p_procAssn == NULL)
//		//delete GT 
//	{
//		return ;
//	}
//	ProcAssignSchedule* pProcSchd = GetCurSelProcSchdFromRosterList();
//	if( !pProcSchd )
//		return;
//	if(pProcSchd->findItem(p_procAssn) == INT_MAX )
//		return ;
//	if( pProcSchd->processorIsClose() )
//		return;
//	ProcessorID id;
//	if(!m_mainDlg->GetCurrentSelProcessID(id))
//		return ;
//	Processor* p_pro = GetInputTerminal()->GetProcessorList()->getProcessor(id) ;
//	// edit the event
//
//	CsingleEventsDataDlg dlg( m_pParentWnd,FALSE,p_pro, pProcSchd);
//	dlg.SetDlgData( id.GetIDString(), *p_procAssn );
//	if( dlg.DoModal()!=IDOK)	return;
//	dlg.GetDlgData(*p_procAssn);
//	ItemRosterList _RosterItem ;
//	_RosterItem.c_Processor = id.GetIDString() ;
//	const CMultiMobConstraint* pMultiConst = p_procAssn->getAssignment();	
//	const CMobileElemConstraint* pConst = pMultiConst->getConstraint( 0 );
//	pConst->screenPrint(_RosterItem.c_PaxTy) ;
//	_RosterItem.c_openTime = p_procAssn->getAbsOpenTime().printTime() ;
//	_RosterItem.c_closeTime = p_procAssn->getAbsCloseTime().printTime() ;
//	_RosterItem.c_relation = m_mainDlg->GetRelationName(p_procAssn->getAssRelation()) ;
//	_RosterItem.c_relation = m_mainDlg->GetIsDailyName(p_procAssn->IsDaily()) ;
//	EditSelRosterFromRosterList(_RosterItem) ;
//}
ProcessorRosterSchedule* CRosterList::GetCurSelProcSchdFromRosterList()
{
	POSITION _pos = m_rosterList.GetFirstSelectedItemPosition() ;
	if(_pos == NULL)
		return NULL;
	int inx = m_rosterList.GetNextSelectedItem(_pos) ;
	TCHAR name[1024] = {0} ;
	m_rosterList.GetItemText(inx,0,name,1024) ;
	ProcessorID id ;
	id.SetStrDict(GetInputTerminal()->inStrDict) ;
	id.setID(name) ;
	inx = GetInputTerminal()->procAssignDB->findEntry(id) ;
	if(inx == INT_MAX)
		return NULL ;
	return  GetInputTerminal()->procAssignDB->getDatabase(inx) ;
}
InputTerminal* CRosterList::GetInputTerminal()
{
	
	return m_mainDlg->GetInputTerminal() ;
}
void CRosterList::SaveData()
{

}
//void CRosterList::ImportFileForRosterList() 
//{
//	CFileDialog filedlg( TRUE,"csv", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
//		"CSV File (*.csv)|*.csv;*.CSV|All type (*.*)|*.*|", NULL );
//	filedlg.m_ofn.lpstrTitle = "Import Roster"  ;
//	if(filedlg.DoModal()!=IDOK)
//		return;
//	CString csFileName=filedlg.GetPathName();
//	if( !ReadTextFromFile( csFileName ) )
//		return;
//	InitRosterListData() ;
//}
void CRosterList::ExportFileFromRosterList()
{
	CFileDialog filedlg( FALSE,"csv", NULL, OFN_CREATEPROMPT| OFN_SHAREAWARE| OFN_OVERWRITEPROMPT , \
		"CSV File (*.csv)|*.csv;*.CSV|All type (*.*)|*.*|", NULL );
	filedlg.m_ofn.lpstrTitle = "Export Roster"  ;
    
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();
	if( WriteRosterToFile( csFileName ) )
		return;
}
BOOL CRosterList::WriteRosterToFile(const CString& filename)
{
	CStdioFile file(filename,CFile::modeCreate | CFile::modeWrite | CFile::typeText ) ;
	CStringArray strItemText;
	GetStringArrayFromRosterList(strItemText) ;
	for (int i = 0 ;i<strItemText.GetCount() ; i++)
		file.WriteString(strItemText.GetAt(i)) ;
	file.Close() ;
	return TRUE ;
}
void CRosterList::GetStringArrayFromRosterList(CStringArray& strArray)
{
	
	CString strlen ;
	CString processId ;
	CString paxTy ;
	CString OpenTime ;
	CString CloseTime ;
	CString relation ;
	CString isdaily ;
	for (int i = 0 ; i < m_rosterList.GetItemCount() ; i++)
	{
		processId.Format( m_rosterList.GetItemText(i,0) );
		paxTy.Format(m_rosterList.GetItemText(i,1)) ;
		OpenTime.Format(m_rosterList.GetItemText(i,2)) ;
		CloseTime.Format(m_rosterList.GetItemText(i,3)) ;
		relation.Format(m_rosterList.GetItemText(i,4)) ;
		isdaily.Format(m_rosterList.GetItemText(i,5)) ;
		strlen.Format(_T("%s,%s,%s,%s,%s,%s"),processId,paxTy,OpenTime,CloseTime,relation,isdaily) ;
		strlen.Append(_T("\n"));
		strArray.Add(strlen);
	}
}
//bool CRosterList::ReadTextFromFile(const CString &_szFileName)
//{
//	CStdioFile file;
//	if( !file.Open( _szFileName, CFile::modeRead) )
//	{
//		CString strMsg;
//		strMsg.Format("Can not open the file!\r\n%s", _szFileName );
//		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
//		return false;
//	}
//
//
//	CString strLine;
//	std::vector<CStringArray*> AllLine ;
//	CStringArray* strItemText = NULL;
//	while( file.ReadString(strLine) )
//	{
//		if( strLine.IsEmpty() )
//			continue;
//		strItemText = new CStringArray ;
//		int iColumnNum = getColNumFromStr( strLine, ",", *strItemText );
//		AllLine.push_back(strItemText) ;
//	}
//
//	//remove all 
//	ProcessorID id ;
//	id.SetStrDict(GetInputTerminal()->inStrDict) ;
//	ProcAssignSchedule* pSchd = NULL ;
//	for (int i =0 ; i <(int)AllLine.size() ;i++)
//	{
//          id.setID(AllLine[i]->GetAt(0)) ;
//		  int nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
//		  if( nIdx != INT_MAX )
//		  {
//			pSchd = GetInputTerminal()->procAssignDB->getDatabase( nIdx );
//			pSchd->clear() ;
//		  }	  
//	}
//
//	for (int i =0 ; i <(int)AllLine.size() ;i++)
//	{
//        AddRosterFromStrList(*AllLine[i]) ;	
//		delete AllLine[i] ;
//	}
//
//	file.Close() ;
//	return true;
//}
//void CRosterList::AddRosterFromStrList(const CStringArray& strItem)
//{
//	InputTerminal* m_pInTerm = GetInputTerminal() ; 
//	ProcessorID id ;
//	id.SetStrDict(GetInputTerminal()->inStrDict) ;
//	id.setID(strItem.GetAt(0)) ;
//
//	ElapsedTime opentime ;
//	ElapsedTime closetime ;
//	opentime.SetTime(strItem.GetAt(2)) ;
//    closetime.SetTime(strItem.GetAt(3)) ;
//
//	CMobileElemConstraint mob(GetInputTerminal());
//	//mob.SetInputTerminal( m_pInTerm );
//
//	CString strAirLine = strItem.GetAt(1);
//	strAirLine.Remove('(') ;
//	strAirLine.Remove(')') ;
//	strAirLine.MakeUpper(); strAirLine.TrimLeft(), strAirLine.TrimRight();
//	if( !strAirLine.IsEmpty() && strAirLine.CompareNoCase("default") != 0 )
//	{
//		if( strAirLine.GetLength() >= AIRLINE_LEN )
//		{
//			CString  strMsg ;
//			strMsg.Format("Airline: %s\r\n is too length( more than %d )",strAirLine, AIRLINE_LEN );
//			MessageBox( strMsg, "ERROR", MB_OK | MB_ICONINFORMATION );
//			return ;
//		}
//		mob.setAirline( strAirLine.Left( AIRLINE_LEN) );
//	}
//	CString CIsDaily = strItem.GetAt(5) ;
//	//////////////////////////////////////////////////////////////////////////
//	// create assign event
//	ProcAssignment* pProcAssn = new ProcAssignment( GetInputTerminal() );
//	pProcAssn->setFlag(1 );
//	pProcAssn->setOpenTime( opentime );
//	pProcAssn->setCloseTime( closetime );
//	pProcAssn->setAbsOpenTime(opentime) ;
//	pProcAssn->setAbsCloseTime(closetime);
//	CMultiMobConstraint multiMob;
//	multiMob.addConstraint( mob );
//	pProcAssn->setAssignment(multiMob );
//	if(CIsDaily == "FALSE")
//		pProcAssn->IsDaily(FALSE) ;
//	else
//		pProcAssn->IsDaily(TRUE) ;
//	pProcAssn->setAssRelation(m_mainDlg->GetRelationByName(strItem.GetAt(4))) ;
//	int nIdx = m_pInTerm->procAssignDB->findEntry( id );
//	if( nIdx == INT_MAX )
//	{
//		m_pInTerm->procAssignDB->addEntry( id );
//		nIdx = m_pInTerm->procAssignDB->findEntry( id );
//		assert( nIdx != INT_MAX  );
//	}
//	ProcAssignSchedule* pSchd = m_pInTerm->procAssignDB->getDatabase( nIdx );
//	pSchd->addItem(pProcAssn) ;
//}
//
//int CRosterList::getColNumFromStr(CString strText, CString strSep, CStringArray &strArray)
//{	
//	strArray.RemoveAll();
//	int nColNum=1;
//	int nPos=strText.Find(strSep,0);
//	while(nPos!=-1)
//	{
//		nColNum++;
//		strArray.Add(strText.Left(nPos));
//		strText=strText.Right(strText.GetLength()-nPos-1);
//		nPos=strText.Find(strSep,0);
//	}
//	strArray.Add(strText);
//	return nColNum;
//}
//BOOL CRosterList::OnToolTipText(UINT,NMHDR* pNMHDR,LRESULT* pResult)
//{
//	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
//
//	// if there is a top level routing frame then let it handle the message
//	if (GetRoutingFrame() != NULL) return FALSE;
//
//	// to be thorough we will need to handle UNICODE versions of the message also !!
//	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
//	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
//	TCHAR szFullText[512];
//	CString strTipText;
//	UINT nID = pNMHDR->idFrom;
//
//	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
//		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
//	{
//		// idFrom is actually the HWND of the tool 
//		nID = ::GetDlgCtrlID((HWND)nID);
//	}
//
//	if (nID!= 0) // will be zero on a separator
//	{
//
//		AfxLoadString(nID, szFullText);
//		strTipText=szFullText;
//
//#ifndef _UNICODE
//		if (pNMHDR->code == TTN_NEEDTEXTA)
//		{
//			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
//		}
//		else
//		{
//			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
//		}
//#else
//		if (pNMHDR->code == TTN_NEEDTEXTA)
//		{
//			_wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
//		}
//		else
//		{
//			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
//		}
//#endif
//
//		*pResult = 0;
//
//		// bring the tooltip window above other popup windows
//		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
//			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
//
//		return TRUE;
//	}
//
//	return FALSE;
//}
void CRosterList::OnPrintRosterList()
{
	ProcessorID id ;
	m_mainDlg->GetCurrentSelProcessID(id) ;
	TCHAR procname[1024] = {0} ;
	id.printID(procname) ;
	CString header ;
	header.Format(_T("Roster For Processor : %s"),id.GetIDString());
	m_rosterList.PrintList(header,CString(),header);
}