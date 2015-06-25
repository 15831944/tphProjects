#include "StdAfx.h"
#include "Resource.h"
#include "DlgArrivalinit.h"
#include "../Common/elaptime.h"
//#include "../AirsideInput/AirsideInput.h"
//#include "../AirsideInput/FlightPlans.h"
//#include "../Engine/Airside/Flight.h"
//#include "../Engine/Airside/Event.h"
//
//using namespace ns_AirsideInput;
//using namespace airside_engine;


CDlgArrivalInit::CDlgArrivalInit( /*ns_AirsideInput::CFlightPlans* pFlightPlans, */std::vector<FlightArrivalInit>& vArrivalInitPara,CWnd* pParent)
: CToolTipDialog(CDlgArrivalInit::IDD, pParent)
,m_vArrivalInitPara(vArrivalInitPara)
{
}

CDlgArrivalInit::~CDlgArrivalInit() 
{
}


void CDlgArrivalInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ArrivalInit, m_wndListCtrl);
}

BEGIN_MESSAGE_MAP(CDlgArrivalInit, CToolTipDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CNECRelatedBaseDlg message handlers
BOOL CDlgArrivalInit::OnInitDialog()
{
	CDialog::OnInitDialog();

	// set list control window style
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);

	SetListColumns();
	SetListContents();

	return TRUE;
}

void CDlgArrivalInit::OnOK()
{
	CDialog::OnOK();
}

void CDlgArrivalInit::OnCancel()
{
	CDialog::OnCancel();
}

void CDlgArrivalInit::SetListColumns()
{
	m_wndListCtrl.InsertColumn(0, _T("Airline"), LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(1, _T("Arrival ID"), LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(2, _T("Arrival Gate"), LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(3, _T("EnRoute"), LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(4, _T("STAR"), LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(5, _T("Runway"), LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(6, _T("Schedule Arrival Time"), LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(7, _T("Time Offset"), LVCFMT_LEFT, 80);
	m_wndListCtrl.InsertColumn(8, _T("Time in Simulation"), LVCFMT_LEFT, 80);
}

void CDlgArrivalInit::SetListContents()
{
	CString _strvalue;
	_strvalue.Empty();
	ElapsedTime _time;
	for (int i =0;i<int(m_vArrivalInitPara.size());i++)
	{
		m_wndListCtrl.InsertItem(i, m_vArrivalInitPara[i].strAirline);
		m_wndListCtrl.SetItemText(i, 1, m_vArrivalInitPara[i].strArrID);
		m_wndListCtrl.SetItemText(i, 2, m_vArrivalInitPara[i].strArrGate);
		m_wndListCtrl.SetItemText(i, 3, m_vArrivalInitPara[i].strEnroute);
		m_wndListCtrl.SetItemText(i, 4, m_vArrivalInitPara[i].strSTAR);
		m_wndListCtrl.SetItemText(i, 5, m_vArrivalInitPara[i].strRunway);

		_time = m_vArrivalInitPara[i].dScheduleTime/100;
		m_wndListCtrl.SetItemText(i, 6 , _time.printTime());
		_strvalue.Format("%.2f",m_vArrivalInitPara[i].dOffsetTime/100);
		m_wndListCtrl.SetItemText(i, 7 , _strvalue);
		_strvalue.Empty();
		_time = m_vArrivalInitPara[i].dSimTime/100;
		m_wndListCtrl.SetItemText(i, 8 ,_time.printTime());

	}
}

void CDlgArrivalInit::OnSize(UINT nType, int cx, int cy)
{
	CWnd* pWndOK = GetDlgItem(IDOK);
	CWnd* pWndCancel = GetDlgItem(IDCANCEL);
	if (pWndOK == NULL || pWndCancel == NULL)
		return;

	CRect rcCancel;
	pWndCancel->GetClientRect(&rcCancel);
	pWndCancel->MoveWindow(cx - rcCancel.Width() - 10,
							cy - rcCancel.Height() - 10, 
							rcCancel.Width(), 
							rcCancel.Height());

	CRect rcOK;
	pWndOK->GetClientRect(&rcOK);
	pWndOK->MoveWindow(cx - rcCancel.Width() - rcOK.Width() - 20,
						cy - rcOK.Height() - 10, 
						rcOK.Width(),
						rcOK.Height());

	m_wndListCtrl.MoveWindow(10, 35, cx - 20, cy - rcOK.Height() - 57);

	Invalidate();
}
