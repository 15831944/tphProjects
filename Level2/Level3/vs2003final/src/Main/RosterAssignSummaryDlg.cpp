// RosterAssignSummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "RosterAssignSummaryDlg.h"
#include "..\Inputs\Assign.h"
#include "..\Inputs\Schedule.h"
#include ".\rosterassignsummarydlg.h"

class tagTimeSlice
{
public:
	tagTimeSlice();
	virtual ~tagTimeSlice(){};
	ElapsedTime beginTime;
	ElapsedTime endTime;
};

tagTimeSlice::tagTimeSlice() : beginTime(0L), endTime(0L){}
tagRosterDataItem::tagRosterDataItem() : m_timeUsage(0L), m_fOccupancy(0){}

// CRosterAssignSummaryDlg dialog

IMPLEMENT_DYNAMIC(CRosterAssignSummaryDlg, CDialog)
CRosterAssignSummaryDlg::CRosterAssignSummaryDlg(std::vector<ProcessorID>* procList, 
												 InputTerminal* pInTerm, 
												 CWnd* pParent /*=NULL*/)
	: CDialog(CRosterAssignSummaryDlg::IDD, pParent)
	,m_vProcID(*procList)
	,m_pInTerm(pInTerm)
	,m_timeTotal(0L)
{
}

CRosterAssignSummaryDlg::~CRosterAssignSummaryDlg()
{
}

void CRosterAssignSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ROSTER_ASIGN_SUMMARY, m_summaryListCtrl);
	DDX_Control(pDX, IDC_BTN_GATEASS_SUMMARY_EXPORT, m_btnExport);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}

void CRosterAssignSummaryDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(!IsWindowVisible())
		return; 
	CRect btnrc;
	m_btnExport.GetWindowRect(&btnrc);
	m_summaryListCtrl.MoveWindow(11,11,cx-24,cy-27-btnrc.Height());
	m_btnExport.MoveWindow(11,cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height());
	m_btnOK.MoveWindow(cx-15-2*btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow(cx-10-btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	CRect rect( 0,0,cx,cy );
	InvalidateRect( rect );
}


BEGIN_MESSAGE_MAP(CRosterAssignSummaryDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_GATEASS_SUMMARY_EXPORT, OnBnClickedBtnGateassSummaryExport)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CRosterAssignSummaryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwStyle = m_summaryListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_summaryListCtrl.SetExtendedStyle( dwStyle );

	FlightSchedule* fs = m_pInTerm->flightSchedule;
	if (fs->getCount() < 1)
		return FALSE;

	int nDay = fs->GetFlightScheduleEndTime().GetDay();
	int nHours = nDay * 24;
	ElapsedTime timeTotal = 0L;
	timeTotal.set(nHours, 0, 0);
	m_timeTotal = timeTotal;

	InitRosterData();
	InitRosterAssignSummaryList();
	InsertRosterData();
	return TRUE;
}


// CRosterAssignSummaryDlg message handlers

void CRosterAssignSummaryDlg::InitRosterData()
{
	ProcAssignDatabase* pAssignDB = m_pInTerm->procAssignDB;
	ProcAssignEntry* pEntry = NULL ;
	ProcessorRosterSchedule* pSche = NULL;
	size_t nSize = m_vProcID.size();
	tagRosterDataItem dataItem;
	for (size_t i = 0 ; i < nSize ; i++)
	{
		ProcessorID procID = m_vProcID.at(i);
		dataItem.m_strKey = procID.GetIDString();
		int nIdx = pAssignDB->findEntry(procID);
		if(nIdx ==INT_MAX)
		{
			pAssignDB->addEntry(procID) ;
			nIdx  = pAssignDB->findEntry(procID) ;	
			pEntry = (ProcAssignEntry*)pAssignDB->getItem(nIdx) ;
		}
		else
		{
			pEntry = (ProcAssignEntry*)pAssignDB->getItem(nIdx) ;
		}
		pSche = pEntry->getSchedule();
		GetUsageTimeFromSchedule(pSche, dataItem);
		dataItem.m_fOccupancy = (float)dataItem.m_timeUsage.asSeconds() / (float)m_timeTotal.asSeconds() * 100.0f;
		m_rosterData.push_back(dataItem);
	}
}

void CRosterAssignSummaryDlg::GetUsageTimeFromSchedule( ProcessorRosterSchedule* pSche, tagRosterDataItem& rosterDataItem )
{
	ProcessorRoster* pProcRoster;
	std::vector<tagTimeSlice> vTimeSlice;
	std::vector<tagTimeSlice>::iterator itor;
	tagTimeSlice timeSlice;
	int nScheCount = pSche->getCount();
	for(int i=0; i<nScheCount; i++)
	{
		pProcRoster = pSche->ItemAt(i);
		timeSlice.beginTime = pProcRoster->getOpenTime();
		timeSlice.endTime = pProcRoster->getCloseTime();
		for(itor = vTimeSlice.begin(); itor != vTimeSlice.end(); itor++)
		{
			if(itor->beginTime < timeSlice.beginTime && timeSlice.beginTime < itor->endTime)// the begin moment is in another time slice.
			{
				if(itor->beginTime < timeSlice.endTime && timeSlice.endTime < itor->endTime)// the end moment is in another time slice.
				{
					timeSlice.beginTime = itor->beginTime;
					timeSlice.endTime = itor->endTime;
				}
				else
				{
					timeSlice.beginTime = itor->beginTime;
				}
			}
			else
			{
				if(itor->beginTime < timeSlice.endTime && timeSlice.endTime < itor->endTime)
				{
                    timeSlice.endTime = itor->endTime;
				}
				else
				{
					continue;
				}
			}
			itor->beginTime = 0L;
			itor->endTime = 0L;
		}
		vTimeSlice.push_back(timeSlice);
	}

	ElapsedTime timeUsage = 0L;
	for(itor = vTimeSlice.begin(); itor != vTimeSlice.end(); itor++)
	{
		timeUsage += (itor->endTime - itor->beginTime);
	}
	rosterDataItem.m_timeUsage = timeUsage;
	return;
}

void CRosterAssignSummaryDlg::InitRosterAssignSummaryList()
{
	m_summaryListCtrl.InsertColumn(0, "Summary Item", LVCFMT_LEFT, 240);
	m_summaryListCtrl.InsertColumn(1, "Total(hh:mm)", LVCFMT_LEFT, 120);
	m_summaryListCtrl.InsertColumn(2, "Usage(hh:mm)", LVCFMT_LEFT, 120);
	m_summaryListCtrl.InsertColumn(3, "Occupancy(%)", LVCFMT_LEFT, 120);
}

void CRosterAssignSummaryDlg::InsertRosterData()
{
	std::vector<tagRosterDataItem>::iterator iter = m_rosterData.begin();
	std::vector<tagRosterDataItem>::iterator iterEnd = m_rosterData.end();
	ElapsedTime tempTime = 0L;
	CString tempStr;
	for(; iter!=iterEnd; iter++)
	{
		int ndx = m_summaryListCtrl.InsertItem(0,iter->m_strKey);
		tempTime = m_timeTotal;
		m_summaryListCtrl.SetItemText(ndx, 1, tempTime.printTime(FALSE));
		tempTime = iter->m_timeUsage;
		m_summaryListCtrl.SetItemText(ndx, 2, tempTime.printTime(FALSE));
		tempStr.Format("%.2f", iter->m_fOccupancy);
		m_summaryListCtrl.SetItemText(ndx, 3, tempStr);
	}
}


void CRosterAssignSummaryDlg::OnBnClickedBtnGateassSummaryExport()
{
	CFileDialog dlg(FALSE, "*.CSV", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "CSV file(*.CSV)|*.csv;*.CSV|All files(*.*)|*.*||", this );
	if(dlg.DoModal()==IDOK)
	{
		CString sFileName = dlg.GetFileName();
		ArctermFile file;
		if(file.openFile((LPCTSTR)sFileName, WRITE) == FALSE)
		{
			AfxMessageBox("Open file failed!");
			return;
		}
		//title
		LVCOLUMN lvcol;
		char titleStr[256];
		lvcol.mask = LVCF_TEXT;
		lvcol.pszText = titleStr;
		lvcol.cchTextMax = 256;
		int   nColNum = 0;
		while(m_summaryListCtrl.GetColumn(nColNum, &lvcol))
		{ 
			file.writeField(lvcol.pszText);
			nColNum++;
		} 

		file.writeLine();
		//content
		for(int nRow=0; nRow<m_summaryListCtrl.GetItemCount(); nRow++)
		{
			for(int nCol=0; nCol<nColNum; nCol++)
			{
				file.writeField( (LPCTSTR)m_summaryListCtrl.GetItemText( nRow, nCol));
			}
			file.writeLine();
		}
		file.endFile();
	}
}
