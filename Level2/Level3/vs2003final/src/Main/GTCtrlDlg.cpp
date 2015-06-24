// GTCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "GTCtrlDlg.h"
#include "ProcAssignDlg.h"
// CGTCtrlDlg dialog

IMPLEMENT_DYNAMIC(CGTCtrlDlg, CTabPageSSL)
CGTCtrlDlg::CGTCtrlDlg(CProcAssignDlg* _prodlg,CWnd* pParent /*=NULL*/)
	:m_mainDlg(_prodlg), CTabPageSSL(CGTCtrlDlg::IDD, pParent)
	
{
 nDay = 1 ;
}

CGTCtrlDlg::~CGTCtrlDlg()
{
}

void CGTCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabPageSSL::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GTCHARTCTRL, m_gtCtrl);
}


BEGIN_MESSAGE_MAP(CGTCtrlDlg, CTabPageSSL)
	ON_WM_CREATE() 
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CGTCtrlDlg::OnInitDialog() 
{
	CTabPageSSL::OnInitDialog() ;
	InitGTCtrl() ;
	//SetResize(m_gtCtrl.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	return true ;
}
void CGTCtrlDlg::OnSize(UINT nType, int cx, int cy) 
{
	CTabPageSSL::OnSize(nType, cx, cy);
	CRect rc ;
	if(::IsWindow(this->GetSafeHwnd()))
	{
	  GetClientRect(&rc) ;
	  if(::IsWindow(m_gtCtrl.GetSafeHwnd()))
		  m_gtCtrl.MoveWindow(rc) ;
	}
	//cx
	//	Specifies the new width of the client area.

	//cy
	//	Specifies the new height of the client area.

}
// CGTCtrlDlg message handlers
void CGTCtrlDlg::InitGTCtrl()
{

	m_gtCtrl.InitControl( (long)AfxGetThread() );
	m_gtCtrl.SetMoveItemSnapLineable(false);
	m_gtCtrl.SetItemColorGradual(TRUE) ;
	m_gtCtrl.ShowBtn(0,false);
	m_gtCtrl.ShowBtn(2,false);
	m_gtCtrl.ShowBtn(5,false);
	m_gtCtrl.ShowBtn(6,false);
	m_gtCtrl.ShowBtn(9,false);
	m_gtCtrl.ShowBtn(10,false);
	m_gtCtrl.ShowBtn(11,false);
	m_gtCtrl.ShowBtn(12,false);
	m_gtCtrl.ShowBtn(15,false);
	m_gtCtrl.ShowBtn(16,false);
	m_gtCtrl.ShowBtn(17,false);
	FlightSchedule* fs = m_mainDlg->GetInputTerminal()->flightSchedule;
	if(fs == NULL)
		return ;
	if (fs->getCount() < 1)
		return;
	nDay =fs->GetFlightScheduleEndTime().GetDay();
	int nHours = nDay * 24;
	m_gtCtrl.SetRulerMax(nHours);
}
void CGTCtrlDlg::InitAllProcessorData()
{
	m_gtCtrl.Clear( false );
	HTREEITEM root = m_mainDlg->m_treeProc.GetRootItem() ;
	HTREEITEM child = m_mainDlg->m_treeProc.GetChildItem(root);
	while(child)
	{
		InitGTCtrlData(child) ;
		child = m_mainDlg->m_treeProc.GetNextSiblingItem(child) ;
	}
}
void CGTCtrlDlg::InitGTCtrlData()
{
	m_gtCtrl.Clear(FALSE) ;
   InitGTCtrlData(m_mainDlg->m_treeProc.GetSelectedItem()) ; 
}
void CGTCtrlDlg::InitGTCtrlData(HTREEITEM ProcessNode)
{
	if(ProcessNode == NULL)
		return;
	// sort processor name in gt_ctrl
	std::vector<ProcessorID> vectProcID;
	if(!m_mainDlg->m_treeProc.ItemHasChildren(ProcessNode))
	{
		InsertRosterOfProcessor(ProcessNode) ;
		return ;
	}else
	{
		HTREEITEM pchild = m_mainDlg->m_treeProc.GetChildItem(ProcessNode) ;
		while(pchild)
		{ 
			InitGTCtrlData(pchild) ;
			pchild = m_mainDlg->m_treeProc.GetNextSiblingItem(pchild) ;
		}
	}
}
void CGTCtrlDlg::InsertRosterOfProcessor(HTREEITEM ProcessNode )
{
	ProcessorID _id ;
	if(!m_mainDlg->GetCurrentSelProcessID(ProcessNode,_id))
		return ;
	ProcessorRosterSchedule* _schedule = NULL ;
	int ndx =  m_mainDlg->GetInputTerminal()->procAssignDB->findEntry(_id) ;
	if(ndx == INT_MAX)
	{
		ProcessorID id ;
		if(!m_mainDlg->FindDefinedRosterProcessor(ProcessNode,id) )
		{
			ElapsedTime time ;
			ProcessorRoster _ass ;
			CMultiMobConstraint assign;	
			assign.addConstraint(CMobileElemConstraint(m_mainDlg->GetInputTerminal())) ;
			_ass.setAbsOpenTime(time) ;
			CMobileElemConstraint constraint(m_mainDlg->GetInputTerminal());
			_ass.setAssignment(assign);
			time = time + (WholeDay-1);
			_ass.setAbsCloseTime(time) ;
			_ass.setAssRelation(Inclusive) ;
			_ass.IsDaily(TRUE) ;
			AddLineToGT(&_ass,_id) ;
			return ;
		}else
		{
			ndx =  m_mainDlg->GetInputTerminal()->procAssignDB->findEntry(id) ;
			ASSERT(ndx != INT_MAX);
			if(ndx == INT_MAX)
				return ;
			_schedule = m_mainDlg->GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
		}
	}else
	{
		ndx =  m_mainDlg->GetInputTerminal()->procAssignDB->findEntry(_id) ;
		ASSERT(ndx != INT_MAX);
		_schedule =  m_mainDlg->GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
		if(_schedule->getCount() == 0)
		{
	        ProcessorID curID;
			if(!m_mainDlg->FindDefinedRosterProcessor(ProcessNode,curID) )
			{
				ElapsedTime time ;
				ProcessorRoster _ass ;
				CMultiMobConstraint assign;	
				assign.addConstraint(CMobileElemConstraint(m_mainDlg->GetInputTerminal())) ;
				_ass.setAbsOpenTime(time) ;
				CMobileElemConstraint constraint(m_mainDlg->GetInputTerminal());
				_ass.setAssignment(assign);
				time = time + (WholeDay-1);
				_ass.setAbsCloseTime(time) ;
				_ass.setAssRelation(Inclusive) ;
				_ass.IsDaily(TRUE) ;
				AddLineToGT(&_ass,_id) ;
				return ;
			}else
			{
				ndx =  m_mainDlg->GetInputTerminal()->procAssignDB->findEntry(curID) ;
				ASSERT(ndx != INT_MAX);
				if(ndx == INT_MAX)
					return ;
				_schedule = m_mainDlg->GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
			}
		}
	};
	if(_schedule == NULL)
		return ;
	AddProcessor(_schedule,_id) ;
	/*ProcAssignment* _ass = NULL;
	for (int i =0 ; i < _schedule->getCount() ; i++)
	{
		_ass = _schedule->getItem(i) ;
		if(_ass->getFlag() != 0)
		 AddLineToGT(_ass,_id) ;
	}*/
}
void CGTCtrlDlg::AddProcessor(ProcessorRosterSchedule* _schedule,ProcessorID& id)
{
		ProcessorRoster* _ass = NULL;
   	int lLineIdx = m_gtCtrl.AddLine( id.GetIDString(),NULL);
	for (int i =0 ; i < _schedule->getCount() ; i++)
	{
		_ass = _schedule->getItem(i) ;
		if(_ass->getFlag() != 0)
		{
			m_gtCtrl.SetLineData( lLineIdx , (long)id.getID() );
			m_gtCtrl.SetLineModifyable( lLineIdx,true);
			AddItemToGT(lLineIdx,_ass) ;
		}
	}
}
void CGTCtrlDlg::AddLineToGT(ProcessorRoster* _Ass,ProcessorID& id)
{
	int lLineIdx = m_gtCtrl.AddLine( id.GetIDString(),NULL);
	m_gtCtrl.SetLineData( lLineIdx , (long)id.getID() );
	m_gtCtrl.SetLineModifyable( lLineIdx,true);
	AddItemToGT(lLineIdx,_Ass) ;


}
void CGTCtrlDlg::AddItemToGT(int lineId , ProcessorRoster* _Ass)
{
	if(_Ass == NULL)
		return ;
	const CMultiMobConstraint* pMultiConst = _Ass->getAssignment();	
	const CMobileElemConstraint* pConst = pMultiConst->getConstraint( 0 );
	CString  paxTy ;
	pConst->screenPrint( paxTy );
	long ndx = 0 ;
	if(_Ass->IsDaily())
	{
	for(int i = 0 ; i < nDay ; i++)
	   m_gtCtrl.AddItem(&ndx,lineId,paxTy.GetBuffer(0),NULL,ElapsedTime(i*WholeDay).asMinutes() +  _Ass->getAbsOpenTime().asMinutes(),(long)(_Ass->getAbsCloseTime() - _Ass->getAbsOpenTime()).asMinutes(),false ,RGB(255,255,255),RGB(132,163,195)) ;
	}else
		m_gtCtrl.AddItem(&ndx,lineId,paxTy.GetBuffer(0),NULL, _Ass->getAbsOpenTime().asMinutes(),(long)(_Ass->getAbsCloseTime() - _Ass->getAbsOpenTime()).asMinutes(),false ,RGB(255,255,255),RGB(132,163,195)) ;

	m_gtCtrl.SetItemData(ndx,(long)_Ass) ;
}
