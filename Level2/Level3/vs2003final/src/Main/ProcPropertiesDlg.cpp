// ProcPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "NodeView.h"
#include "TermPlanDoc.h"
#include "UnitsManager.h"
#include "ProcPropertiesDlg.h"

#include "engine\hold.h"
#include "engine\lineproc.h"
#include "engine\baggage.h"
#include "engine\dep_srce.h"
#include "engine\dep_sink.h"
#include "engine\gate.h"
#include "engine\barrier.h"
#include "engine\floorchg.h"
#include "engine\station.h"

#include "Fallback.h"
#include "common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProcProperties dialog

CDlgProcProperties::CDlgProcProperties(Processor* pProc, CTermPlanDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProcProperties)
	m_iQueueType = -1;
	m_bUseQueue = FALSE;
	m_bUseServLoc = TRUE;
	m_sInC = _T("(undefined)");
	m_bUseInC = FALSE;
	m_bUseOutC = FALSE;
	m_sOutC = _T("(undefined)");
	m_sQueue = _T("(undefined)");
	m_sServLoc = _T("(undefined)");
	m_sFloorName = _T("");
	m_sProcName = _T("");
	//}}AFX_DATA_INIT

	ASSERT(pProc!=NULL);
	m_pProcessor = pProc;
	m_pParent = pParent;
	m_pPathToSet = NULL;
	m_pStrToSet = NULL;
	m_pDoc = pDoc;

	m_inC.init(m_pProcessor->inConstraintLength(), m_pProcessor->inConstraint()->getPointList());
	m_outC.init(m_pProcessor->outConstraintLength(), m_pProcessor->outConstraint()->getPointList());
	m_queue.init(m_pProcessor->queueLength(), m_pProcessor->queuePath()->getPointList());
	m_servLoc.init(m_pProcessor->serviceLocationLength(), m_pProcessor->serviceLocationPath()->getPointList());
}


void CDlgProcProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProcProperties)
	DDX_Control(pDX, IDC_CMB_TYPE, m_cmbType);
	DDX_Radio(pDX, IDC_RDO_QUEUE, m_iQueueType);
	DDX_Check(pDX, IDC_CHK_USEQUEUE, m_bUseQueue);
	DDX_Check(pDX, IDC_CHK_USESERVLOC, m_bUseServLoc);
	DDX_Text(pDX, IDC_DISP_INC, m_sInC);
	DDX_Check(pDX, IDC_CHK_USEINC, m_bUseInC);
	DDX_Check(pDX, IDC_CHK_USEOUTC, m_bUseOutC);
	DDX_Text(pDX, IDC_DISP_OUTC, m_sOutC);
	DDX_Text(pDX, IDC_DISP_QUEUE, m_sQueue);
	DDX_Text(pDX, IDC_DISP_SERVLOC, m_sServLoc);
	DDX_Text(pDX, IDC_TXT_FLOOR, m_sFloorName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProcProperties, CDialog)
	//{{AFX_MSG_MAP(CDlgProcProperties)
	ON_BN_CLICKED(IDC_CHK_USESERVLOC, OnChkUseServLoc)
	ON_CBN_SELCHANGE(IDC_CMB_TYPE, OnSelChangeCmbType)
	ON_BN_CLICKED(IDC_PFM_SERVLOC, OnPFMServLoc)
	ON_BN_CLICKED(IDC_PFM_INC, OnPFMInC)
	ON_BN_CLICKED(IDC_PFM_OUTC, OnPFMOutC)
	ON_BN_CLICKED(IDC_PFM_QUEUE, OnPFMQueue)
	//}}AFX_MSG_MAP
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
END_MESSAGE_MAP()

BOOL CDlgProcProperties::OnInitDialog()
{
	m_iQueueType = m_pProcessor->queueIsFixed()?0:1;
	
	//fill proc type combo box
	CComboBox* pCmbType = (CComboBox*) GetDlgItem(IDC_CMB_TYPE);
	for(int i=0; i<PROC_TYPES; i++)
		pCmbType->AddString(PROC_TYPE_NAMES[i]);
	pCmbType->SetCurSel(m_pProcessor->getProcessorType());

	//set floor name
	m_sFloorName = m_pDoc->m_floors.m_vFloors[m_pProcessor->getFloor()]->FloorName();
	
	SetMappedVars();

	EnableTypeDepControls();

	return CDialog::OnInitDialog();
	
}

static CString GetCoordString(Path* pPath)
{
	CString s;
	CString sTmp;
	for(int i=0; i<pPath->getCount(); i++) {
		Point p = pPath->getPoint(i);
		sTmp.Format("(%.2f,%.2f) ", UNITSMANAGER->ConvertLength(p.getX()), UNITSMANAGER->ConvertLength(p.getY()));
		s += sTmp;
	}
	return s;
}

void CDlgProcProperties::SetMappedVars()
{	
	char buff[256];
	m_pProcessor->getID()->printID(buff);
	m_sProcName = buff;

	m_bUseInC = (m_inC.getCount() > 0);
	if(m_bUseInC)
		m_sInC = GetCoordString(&m_inC);
	else
		m_sInC = _T("undefined");
	m_bUseOutC = (m_outC.getCount() > 0);
	if(m_bUseOutC)
		m_sOutC = GetCoordString(&m_outC);
	else
		m_sOutC = _T("undefined");
	m_bUseQueue = (m_queue.getCount());
	if(m_bUseQueue)
		m_sQueue = GetCoordString(&m_queue);
	else
		m_sQueue = _T("undefined");

	m_bUseServLoc = TRUE;
	if(m_bUseServLoc)
		if(m_servLoc.getCount() == 0)
			m_sServLoc = _T("(undefined)");
		else
			m_sServLoc = GetCoordString(&m_servLoc);
	else
		m_sServLoc = _T("undefined");
}

void CDlgProcProperties::EnableTypeDepControls() {
	int procType;
	ASSERT(procType != -1);
	EnableServLocControls(TRUE);
	switch(procType) {
	case PointProc:
	case GateProc:
	case StationProc:
	case FloorChangeProc:
		EnableInCControls(TRUE);
		EnableOutCControls(TRUE);
		EnableQueueControls(TRUE);
		EnableIsQFixedControls(TRUE);
		break;
	case LineProc:
	case BaggageProc:
		EnableInCControls(TRUE);
		EnableOutCControls(TRUE);
		EnableQueueControls(TRUE);
		EnableIsQFixedControls(FALSE);
		break;
	case HoldAreaProc:
	case DepSinkProc:
		EnableInCControls(TRUE);
		EnableOutCControls(TRUE);
		EnableQueueControls(FALSE);
		EnableIsQFixedControls(FALSE);
		break;
	case BarrierProc:
		EnableInCControls(TRUE);
		EnableOutCControls(FALSE);
		EnableQueueControls(FALSE);
		EnableIsQFixedControls(FALSE);
	case DepSourceProc:
		EnableInCControls(TRUE);
		EnableOutCControls(FALSE);
		EnableQueueControls(TRUE);
		EnableIsQFixedControls(TRUE);
		break;
	}
}

void CDlgProcProperties::EnableServLocControls(BOOL bEnable)
{
	((CButton*) GetDlgItem(IDC_CHK_USESERVLOC))->EnableWindow(bEnable);
	((CButton*) GetDlgItem(IDC_PFM_SERVLOC))->EnableWindow(bEnable);
	((CEdit*) GetDlgItem(IDC_DISP_SERVLOC))->EnableWindow(bEnable);
}

void CDlgProcProperties::EnableInCControls(BOOL bEnable)
{
	((CButton*) GetDlgItem(IDC_CHK_USEINC))->EnableWindow(bEnable);
	((CButton*) GetDlgItem(IDC_PFM_INC))->EnableWindow(bEnable);
	((CEdit*) GetDlgItem(IDC_DISP_INC))->EnableWindow(bEnable);
}

void CDlgProcProperties::EnableOutCControls(BOOL bEnable)
{
	((CButton*) GetDlgItem(IDC_CHK_USEOUTC))->EnableWindow(bEnable);
	((CButton*) GetDlgItem(IDC_PFM_OUTC))->EnableWindow(bEnable);
	((CEdit*) GetDlgItem(IDC_DISP_OUTC))->EnableWindow(bEnable);
}

void CDlgProcProperties::EnableQueueControls(BOOL bEnable)
{
	((CButton*) GetDlgItem(IDC_CHK_USEQUEUE))->EnableWindow(bEnable);
	((CButton*) GetDlgItem(IDC_PFM_QUEUE))->EnableWindow(bEnable);
	((CEdit*) GetDlgItem(IDC_DISP_QUEUE))->EnableWindow(bEnable);
}

void CDlgProcProperties::EnableIsQFixedControls(BOOL bEnable)
{
	((CButton*) GetDlgItem(IDC_RDO_QUEUE))->EnableWindow(bEnable);
	((CButton*) GetDlgItem(IDC_RDO_QUEUE2))->EnableWindow(bEnable);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgProcProperties message handlers

void CDlgProcProperties::OnChkUseServLoc() 
{
	m_bUseServLoc = TRUE;
	UpdateData(FALSE);
	AfxMessageBox("All processors must define a service location.");
}


void CDlgProcProperties::OnSelChangeCmbType() 
{
	int sel = m_cmbType.GetCurSel();
	ASSERT(sel != -1);
}

void CDlgProcProperties::OnOK() 
{
	UpdateData(TRUE);
	if(UpdateProcData())
		CDialog::OnOK();
	else
		return; //cancel OK
}


void CDlgProcProperties::OnCancel() 
{	
	CDialog::OnCancel();
}

int CDlgProcProperties::DoModal() 
{
	return CDialog::DoModal();
}

int CDlgProcProperties::DoFakeModal()
{
	if(CDialog::Create(CDlgProcProperties::IDD, m_pParent)) {
		CenterWindow();
		ShowWindow(SW_SHOW);
		GetParent()->EnableWindow(FALSE);
		EnableWindow();
		int nReturn = RunModalLoop();
		DestroyWindow();
		return nReturn;
	}
	else
		return IDCANCEL;
}

void CDlgProcProperties::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgProcProperties::ShowDialog(CWnd* parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}

void CDlgProcProperties::OnPFMServLoc() 
{
	CWnd* wnd = GetParent();
	HideDialog(wnd);

	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	m_pPathToSet = &m_servLoc;
	m_pStrToSet = &m_sServLoc;

	UpdateData(TRUE);
	LPARAM lParam;
	switch(m_cmbType.GetCurSel()-1)
	{
	case PointProc:
	case GateProc:
	case StationProc:
	case DepSinkProc:
	case DepSourceProc:
	case FloorChangeProc:
		lParam = (LPARAM) PICK_ONEPOINT;
		break;
	case LineProc:
	case BaggageProc:
	case HoldAreaProc:
	case BarrierProc:
		lParam = (LPARAM) PICK_MANYPOINTS;
		break;
	};

	if(!wnd->SendMessage(TP_TEMP_FALLBACK, (WPARAM) &data, lParam )) {
		AfxMessageBox("error");
		wnd = GetParent();
		ShowDialog(wnd);
	}

}

void CDlgProcProperties::OnPFMInC() 
{
	CWnd* wnd = GetParent();
	HideDialog(wnd);

	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	m_pPathToSet = &m_inC;
	m_pStrToSet = &m_sInC;

	UpdateData(TRUE);
	LPARAM lParam;
	switch(m_cmbType.GetCurSel()-1)
	{
	case PointProc:
	case GateProc:
	case StationProc:
	case LineProc:
	case BaggageProc:
	case HoldAreaProc:
	case BarrierProc:
	case DepSinkProc:
	case DepSourceProc:
	case FloorChangeProc:
		lParam = (LPARAM) PICK_MANYPOINTS;
		break;
	};

	if(!wnd->SendMessage(TP_TEMP_FALLBACK, (WPARAM) &data, lParam )) {
		AfxMessageBox("error");
		wnd = GetParent();
		ShowDialog(wnd);
	}
}

void CDlgProcProperties::OnPFMOutC() 
{
	CWnd* wnd = GetParent();
	HideDialog(wnd);

	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	m_pPathToSet = &m_outC;
	m_pStrToSet = &m_sOutC;

	UpdateData(TRUE);
	LPARAM lParam;
	switch(m_cmbType.GetCurSel()-1)
	{
	case PointProc:
	case GateProc:
	case StationProc:
	case LineProc:
	case BaggageProc:
	case HoldAreaProc:
	case BarrierProc:
	case DepSinkProc:
	case DepSourceProc:
	case FloorChangeProc:
		lParam = (LPARAM) PICK_MANYPOINTS;
		break;
	};

	if(!wnd->SendMessage(TP_TEMP_FALLBACK, (WPARAM) &data, lParam )) {
		AfxMessageBox("error");
		wnd = GetParent();
		ShowDialog(wnd);
	}
}

void CDlgProcProperties::OnPFMQueue() 
{
	CWnd* wnd = GetParent();
	HideDialog(wnd);

	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	m_pPathToSet = &m_queue;
	m_pStrToSet = &m_sQueue;

	UpdateData(TRUE);
	LPARAM lParam;
	switch(m_cmbType.GetCurSel()-1)
	{
	case PointProc:
	case GateProc:
	case StationProc:
	case LineProc:
	case BaggageProc:
	case HoldAreaProc:
	case BarrierProc:
	case DepSinkProc:
	case DepSourceProc:
	case FloorChangeProc:
		lParam = (LPARAM) PICK_MANYPOINTS;
		break;
	};

	if(!wnd->SendMessage(TP_TEMP_FALLBACK, (WPARAM) &data, lParam )) {
		AfxMessageBox("error");
		wnd = GetParent();
		ShowDialog(wnd);
	}
}

LRESULT CDlgProcProperties::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	CWnd* wnd = GetParent();
	while (!wnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		wnd->ShowWindow(SW_SHOW);
		wnd->EnableWindow(false);
		wnd = wnd->GetParent();
	}
	wnd->EnableWindow(false);
	ShowWindow(SW_SHOW);	
	EnableWindow();

	std::vector<ARCVector3>* pData = static_cast< std::vector<ARCVector3>* >(wParam);

	m_pPathToSet->init(pData->size());
	Point* pPList = m_pPathToSet->getPointList();
	for(int i=0; i<pData->size(); i++) {
		pPList[i].setX(pData->at(i)[X3D]);
		pPList[i].setY(pData->at(i)[Y3D]);
	}

	if(pData->size() > 0)
		*m_pStrToSet = GetCoordString(m_pPathToSet);

	UpdateData(FALSE);
	
	return TRUE;
}

BOOL CDlgProcProperties::UpdateProcData()
{
	int procType = m_cmbType.GetCurSel();
	ASSERT(procType != -1);
	if(m_pProcessor->getProcessorType() != procType) {
		//proc type has been changed
		Processor* pNewProc = NULL;
		//1. create new proc (new type)
		//2. copy releveant data (inC, outC, queue, isFixedQ, servLoc, id)
		switch(procType) {
		case PointProc:
			if(m_servLoc.getCount() == 0) {
				AfxMessageBox(_T("Point processors require a service location."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new Processor();
			pNewProc->initServiceLocation (1, m_servLoc.getPointList());
			pNewProc->initInConstraint(m_inC.getCount(), m_inC.getPointList());
			pNewProc->initOutConstraint (m_outC.getCount(), m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, m_queue.getCount(), m_queue.getPointList());
			break;
		case GateProc:
			if(m_servLoc.getCount() == 0) {
				AfxMessageBox(_T("Gate processors require a service location."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new GateProcessor();
			pNewProc->initServiceLocation (1, m_servLoc.getPointList());
			pNewProc->initInConstraint(m_inC.getCount(), m_inC.getPointList());
			pNewProc->initOutConstraint (m_outC.getCount(), m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, m_queue.getCount(), m_queue.getPointList());
			break;
		case StationProc:
			if(m_servLoc.getCount() == 0) {
				AfxMessageBox(_T("People mover station processors require a service location."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new Station();
			pNewProc->initServiceLocation (1, m_servLoc.getPointList());
			pNewProc->initInConstraint(m_inC.getCount(), m_inC.getPointList());
			pNewProc->initOutConstraint (m_outC.getCount(), m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, m_queue.getCount(), m_queue.getPointList());
			break;
		case LineProc:
			if(m_servLoc.getCount() < 2) {
				AfxMessageBox(_T("Line processors require a service location with at least 2 points."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new LineProcessor();
			pNewProc->initServiceLocation (m_servLoc.getCount(), m_servLoc.getPointList());
			pNewProc->initInConstraint(m_inC.getCount(), m_inC.getPointList());
			pNewProc->initOutConstraint (m_outC.getCount(), m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, m_queue.getCount(), m_queue.getPointList());
			break;
		case BaggageProc:
			if(m_servLoc.getCount() < 3) {
				AfxMessageBox(_T("Baggage device processors require a service location with at least 3 points."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new BaggageProcessor();
			pNewProc->initServiceLocation (m_servLoc.getCount(), m_servLoc.getPointList());
			pNewProc->initInConstraint(m_inC.getCount(), m_inC.getPointList());
			pNewProc->initOutConstraint(m_outC.getCount(), m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, m_queue.getCount(), m_queue.getPointList());
			break;
		case HoldAreaProc:
			if(m_servLoc.getCount() < 3) {
				AfxMessageBox(_T("Holding area processors require a service location with at least 3 points."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new HoldingArea();
			pNewProc->initServiceLocation (m_servLoc.getCount(), m_servLoc.getPointList());
			pNewProc->initInConstraint(m_inC.getCount(), m_inC.getPointList());
			pNewProc->initOutConstraint(m_outC.getCount(), m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, 0, m_queue.getPointList());
			break;
		case BarrierProc:
			if(m_servLoc.getCount() < 2) {
				AfxMessageBox(_T("Barrier processors require a service location with at least 2 points."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new Barrier();
			pNewProc->initServiceLocation (m_servLoc.getCount(), m_servLoc.getPointList());
			pNewProc->initInConstraint(0, m_inC.getPointList());
			pNewProc->initOutConstraint(0, m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, 0, m_queue.getPointList());
			break;
		case DepSinkProc:
			if(m_servLoc.getCount() == 0) {
				AfxMessageBox(_T("Dependent sink processors require a service location."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new DependentSink();
			pNewProc->initServiceLocation (1, m_servLoc.getPointList());
			pNewProc->initInConstraint(m_inC.getCount(), m_inC.getPointList());
			pNewProc->initOutConstraint (m_outC.getCount(), m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, 0, m_queue.getPointList());
			break;
		case DepSourceProc:
			if(m_servLoc.getCount() == 0) {
				AfxMessageBox(_T("Dependent source processors require a service location."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new DependentSource();
			pNewProc->initServiceLocation (1, m_servLoc.getPointList());
			pNewProc->initInConstraint(m_inC.getCount(), m_inC.getPointList());
			pNewProc->initOutConstraint (0, m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, m_queue.getCount(), m_queue.getPointList());
			break;
		case FloorChangeProc:
			if(m_servLoc.getCount() == 0) {
				AfxMessageBox(_T("Floor change processors require a service location."), MB_ICONINFORMATION | MB_OK);
				return FALSE;
			}
			pNewProc = new FloorChangeProcessor();
			pNewProc->initServiceLocation (1, m_servLoc.getPointList());
			pNewProc->initInConstraint(m_inC.getCount(), m_inC.getPointList());
			pNewProc->initOutConstraint (m_outC.getCount(), m_outC.getPointList());
			pNewProc->initQueue(m_iQueueType, m_queue.getCount(), m_queue.getPointList());
			break;
		}
		pNewProc->init(*(m_pProcessor->getID()));
		//3. replace old proc with new one in document
		m_pDoc->GetTerminal().replaceProcessor(m_pProcessor, pNewProc);

	}
	else {
		//copy relevent data (inC, outC, queue, isFixedQ, servLoc)
		m_pProcessor->initServiceLocation(m_servLoc.getCount(), m_servLoc.getPointList());
		m_pProcessor->initInConstraint(m_inC.getCount(), m_inC.getPointList());
		m_pProcessor->initQueue(m_iQueueType == 0, m_queue.getCount(), m_queue.getPointList());
		m_pProcessor->initOutConstraint(m_outC.getCount(), m_outC.getPointList());
	}
	return TRUE;
}