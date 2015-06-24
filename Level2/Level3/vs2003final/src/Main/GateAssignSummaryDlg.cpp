// GateAssignSummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "GateAssignSummaryDlg.h"
#include "inputs\GateAssignmentMgr.h"
#include ".\gateassignsummarydlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGateAssignSummaryDlg dialog
CGateAssignUtility::CGateAssignUtility(CGateAssignmentMgr* _pGateAssignMgr )
: m_pGateAssManager( _pGateAssignMgr )
{
	
}
void CGateAssignUtility::InitData()
{
	m_vGateAssignData.clear();
	int nGateCount = m_pGateAssManager->GetGateCount();
	for( int i=0; i<nGateCount; i++ )
	{
		//tagGateAssignSummary tempSumData;
		//ElapsedTime totalOccupiedTime;
		CAssignGate* pGate = m_pGateAssManager->GetGate(i);

		//tempSumData.m_gateID = pGate->GetProcessID();
		
		int nFlightCount = pGate->GetFlightCount();
		for( int j=0; j<nFlightCount; j++ )
		{
			ElapsedTime timeOccupied;
			CFlightOperationForGateAssign flight = pGate->GetFlight(j);

			timeOccupied = flight.GetEndTime() - flight.GetStartTime();

			char airline[AIRLINE_LEN];
			flight.getFlight()->getAirline( airline );
			AddNewData( pGate->GetName(),airline, timeOccupied );
		}
	}

	//compute from gate viewpoint data
	m_vFromGateViewPoint.clear();
	//caculate m_timeUsage,m_iFrequency,m_fOccupancy
	std::vector<tagGateAssignSummary>::iterator iter = m_vGateAssignData.begin();
	std::vector<tagGateAssignSummary>::iterator iterEnd = m_vGateAssignData.end();
	for( ; iter!=iterEnd; ++iter )
	{
		AddNewDataToGateViewPoint( &(*iter) );
	}

	// caculate totoal frequency
	int iTotalFrequency = 0;
	std::vector<tagGateAssignDataItem>::iterator iterItem = m_vFromGateViewPoint.begin();
	std::vector<tagGateAssignDataItem>::iterator iterItemEnd = m_vFromGateViewPoint.end();
	for(; iterItem != iterItemEnd; ++iterItem )
	{
		iTotalFrequency += iterItem->m_iFrequency;
	}

	if( m_vFromGateViewPoint.size() > 0 )
		iTotalFrequency /= m_vFromGateViewPoint.size();

	// caculate m_strComparativeIndex
	iterItem = m_vFromGateViewPoint.begin();
	for(; iterItem != iterItemEnd; ++iterItem )
	{		
		iterItem->m_strComparativeIndex.Format("%d / %d ", iterItem->m_iFrequency, iTotalFrequency );		
	}	


	// compute from airline viewpoint data	
	m_vFromAirlineViewPoint.clear();
	iter = m_vGateAssignData.begin();
	iterEnd = m_vGateAssignData.end();
	for( ; iter!=iterEnd; ++iter )
	{
		AddNewDataToAirlineViewPoint( &(*iter) );
	}
	
	m_iTotalFrequence =0;
	m_timeTotalOccupied.set(0l);
	// caculate totoal frequency
	iTotalFrequency = 0;
	iterItem = m_vFromAirlineViewPoint.begin();
	iterItemEnd = m_vFromAirlineViewPoint.end();
	for(; iterItem != iterItemEnd; ++iterItem )
	{
		iTotalFrequency += iterItem->m_iFrequency;
		m_timeTotalOccupied += iterItem->m_timeUsage;
	}

	m_iTotalFrequence= iTotalFrequency;

	if( m_vFromAirlineViewPoint.size() > 0 )
		iTotalFrequency /= m_vFromAirlineViewPoint.size();
	// caculate m_strComparativeIndex
	iterItem = m_vFromAirlineViewPoint.begin();
	for(; iterItem != iterItemEnd; ++iterItem )
	{		
		iterItem->m_strComparativeIndex.Format("%d / %d ", iterItem->m_iFrequency, iTotalFrequency );		
	}	

	
}
void CGateAssignUtility::AddNewDataToAirlineViewPoint(tagGateAssignSummary* _pInfo)
{
	std::vector<tagGateAssignDataItem>::iterator iter = m_vFromAirlineViewPoint.begin();
	std::vector<tagGateAssignDataItem>::iterator iterEnd = m_vFromAirlineViewPoint.end();
	for(; iter != iterEnd; ++iter )
	{
		if( _pInfo->m_strAirline == iter->m_strKey )
		{
			iter->m_timeUsage += _pInfo->m_timeOccupied;
			iter->m_iFrequency ++;		
			iter->m_fOccupancy = static_cast<float>(iter->m_timeUsage / ElapsedTime(23*3600+59*60+59l) *100.0);
			return;
		}
	}

	tagGateAssignDataItem temp;
	temp.m_strKey = _pInfo->m_strAirline;
	temp.m_timeUsage = _pInfo->m_timeOccupied;
	temp.m_iFrequency = 1;
	temp.m_fOccupancy = static_cast<float>(temp.m_timeUsage / ElapsedTime(23*3600+59*60+59l) *100.0);
	m_vFromAirlineViewPoint.push_back( temp );	
}
void CGateAssignUtility::AddNewDataToGateViewPoint(tagGateAssignSummary* _pInfo)
{
	std::vector<tagGateAssignDataItem>::iterator iter = m_vFromGateViewPoint.begin();
	std::vector<tagGateAssignDataItem>::iterator iterEnd = m_vFromGateViewPoint.end();
	for(; iter != iterEnd; ++iter )
	{
		if( _pInfo->m_strGateID == iter->m_strKey )
		{
			iter->m_timeUsage += _pInfo->m_timeOccupied;
			iter->m_iFrequency ++;		
			iter->m_fOccupancy = static_cast<float>(iter->m_timeUsage / ElapsedTime(23*3600+59*60+59l) *100.0);
			return;
		}
	}

	tagGateAssignDataItem temp;
	temp.m_strKey = _pInfo->m_strGateID;
	temp.m_timeUsage = _pInfo->m_timeOccupied;
	temp.m_iFrequency = 1;
	temp.m_fOccupancy = static_cast<float>(temp.m_timeUsage / ElapsedTime(23*3600+59*60+59l) *100.0);
	m_vFromGateViewPoint.push_back( temp );	
}
void CGateAssignUtility::AddNewData( const CString& standStr/*const ProcessorID& _gateID*/, CString _strAirline, const ElapsedTime& _occupiedTime )
{
	CString strGateId = standStr;//_gateID.GetIDString();
	std::vector<tagGateAssignSummary>::iterator iter = m_vGateAssignData.begin();
	std::vector<tagGateAssignSummary>::iterator iterEnd = m_vGateAssignData.end();
	for( ; iter!=iterEnd; ++iter )
	{
		if( iter->m_strGateID == strGateId && iter->m_strAirline == _strAirline )
		{
			iter->m_timeOccupied += _occupiedTime;
			iter->m_iNumberOfFlight ++;
			return;
		}
	}


	tagGateAssignSummary tempSumData;
	tempSumData.m_strGateID = strGateId;
	tempSumData.m_iNumberOfFlight =1;
	tempSumData.m_strAirline = _strAirline;
	tempSumData.m_timeOccupied = _occupiedTime;
	m_vGateAssignData.push_back( tempSumData );

}
CGateAssignSummaryDlg::CGateAssignSummaryDlg(CGateAssignmentMgr* _pGateAssignMgr,CWnd* pParent /*=NULL*/)
	: CDialog(CGateAssignSummaryDlg::IDD, pParent),m_utility( _pGateAssignMgr )
{
	//{{AFX_DATA_INIT(CGateAssignSummaryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGateAssignSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGateAssignSummaryDlg)
	DDX_Control(pDX, IDC_LIST_GATA_ASSIGN_SUMMARY, m_listSummary);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BTN_GATEASS_SUMMARY_EXPORT, m_bExport);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGateAssignSummaryDlg, CDialog)
	//{{AFX_MSG_MAP(CGateAssignSummaryDlg)
	ON_BN_CLICKED(IDC_BTN_GATEASS_SUMMARY_EXPORT, OnBtnGateassSummaryExport)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GATA_ASSIGN_SUMMARY, OnLvnItemchangedListGataAssignSummary)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGateAssignSummaryDlg message handlers


BOOL CGateAssignSummaryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_utility.InitData();

	DWORD dwStyle = m_listSummary.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_listSummary.SetExtendedStyle( dwStyle );
	
	// Insert Column
	m_listSummary.InsertColumn(0,"Summary Item",LVCFMT_LEFT,100);
	m_strTitleList.Add("Summary Item");

	int nAirlineCount=m_utility.GetDataFromAirlineViewPoint().size();
	for(int i=0;i<nAirlineCount;i++)
	{
		CString strAirline=m_utility.GetDataFromAirlineViewPoint().at(i).m_strKey;
		m_listSummary.InsertColumn(m_listSummary.GetHeaderCtrl()->GetItemCount(),strAirline,LVCFMT_LEFT,60);
		m_strTitleList.Add(strAirline);
	}
	// Insert a blank column
	m_listSummary.InsertColumn(m_listSummary.GetHeaderCtrl()->GetItemCount(),"",LVCFMT_LEFT,80);
	m_strTitleList.Add("");
	int nIndexUsage=m_listSummary.InsertColumn(m_listSummary.GetHeaderCtrl()->GetItemCount(),"Usage",LVCFMT_LEFT,80);
	m_strTitleList.Add("Usage");
	int nIndexFrequency=m_listSummary.InsertColumn(m_listSummary.GetHeaderCtrl()->GetItemCount(),"Frequency",LVCFMT_LEFT,80);
	m_strTitleList.Add("Frequency");
	int nIndexOccupancy=m_listSummary.InsertColumn(m_listSummary.GetHeaderCtrl()->GetItemCount(),"Occupancy(%)",LVCFMT_LEFT,80);
	m_strTitleList.Add("Occupancy(%)");
	int nIndexComparative=m_listSummary.InsertColumn(m_listSummary.GetHeaderCtrl()->GetItemCount(),"Comparative Index",LVCFMT_LEFT,90);
	m_strTitleList.Add("Comparative Index");

	// Insert Row
	int nGateCount=m_utility.GetDataFromGateViewPoint().size();
	for(int j=0;j<nGateCount;j++)
	{
		CString strGate=m_utility.GetDataFromGateViewPoint().at(j).m_strKey;
		
		int nIndexNew=m_listSummary.InsertItem(j,strGate);
		CString strTemp;
		strTemp=m_utility.GetDataFromGateViewPoint().at(j).m_timeUsage.printTime();
		strTemp=strTemp.Left(strTemp.GetLength()-3);
		m_listSummary.SetItemText(nIndexNew,nIndexUsage,strTemp);

		strTemp.Format("%d",m_utility.GetDataFromGateViewPoint().at(j).m_iFrequency);
		m_listSummary.SetItemText(nIndexNew,nIndexFrequency,strTemp);
		
		float fOccupancy=m_utility.GetDataFromGateViewPoint().at(j).m_fOccupancy;
		strTemp.Format("%.2f",fOccupancy);
		m_listSummary.SetItemText(nIndexNew,nIndexOccupancy,strTemp);

		strTemp=m_utility.GetDataFromGateViewPoint().at(j).m_strComparativeIndex;
		m_listSummary.SetItemText(nIndexNew,nIndexComparative,strTemp);
		
	}

	// Insert a blank row
	m_listSummary.InsertItem(m_listSummary.GetItemCount(),"");
	// Insert Bottom Part Data
	int nIndexUsage2=m_listSummary.InsertItem(m_listSummary.GetItemCount(),"Usage");	
	int nIndexFrequency2=m_listSummary.InsertItem(m_listSummary.GetItemCount(),"Frequency");	
	int nIndexOccupancy2=m_listSummary.InsertItem(m_listSummary.GetItemCount(),"Occupancy(%)");	
	int nIndexComparative2=m_listSummary.InsertItem(m_listSummary.GetItemCount(),"Comparative Index");	

	
	for(int k=0;k<nAirlineCount;k++)
	{
/*
		CString m_strKey;
		ElapsedTime m_timeUsage;
		int m_iFrequency;
		float m_fOccupancy;
		CString m_strComparativeIndex;
		
*/
		CString strTemp;
		strTemp=m_utility.GetDataFromAirlineViewPoint().at(k).m_timeUsage.printTime();
		strTemp=strTemp.Left(strTemp.GetLength()-3);
		m_listSummary.SetItemText(nIndexUsage2,k+1,strTemp);

		strTemp.Format("%d",m_utility.GetDataFromAirlineViewPoint().at(k).m_iFrequency);
		m_listSummary.SetItemText(nIndexFrequency2,k+1,strTemp);
		
		strTemp.Format("%.2f",m_utility.GetDataFromAirlineViewPoint().at(k).m_fOccupancy);
		m_listSummary.SetItemText(nIndexOccupancy2,k+1,strTemp);
		
		strTemp=m_utility.GetDataFromAirlineViewPoint().at(k).m_strComparativeIndex;
		m_listSummary.SetItemText(nIndexComparative2,k+1,strTemp);



	}
	CString strTemp;
	strTemp=m_utility.GetTotalOccupiedTime().printTime();
	strTemp=strTemp.Left(strTemp.GetLength()-3);
	m_listSummary.SetItemText(nIndexUsage2,nIndexUsage,strTemp);

	strTemp.Format("%d",m_utility.GetTotalFrequence());
	m_listSummary.SetItemText(nIndexFrequency2,nIndexFrequency,strTemp);

	m_listSummary.SetItemText(nIndexComparative2,nIndexComparative,"1");
	
	// Insert Gate and Airline Data
/*
	CString m_strGateID;
	CString m_strAirline;
	ElapsedTime m_timeOccupied;
	int m_iNumberOfFlight;
*/
	char arBuf[256];
	LVCOLUMN lvCol;
	lvCol.mask=LVCF_TEXT;
	lvCol.pszText=arBuf;
	lvCol.cchTextMax=256;
	
	int nCount=m_utility.GetDataOfGateAirline().size();
	for(int l=0;l<nGateCount;l++)
	{
		CString strGate=m_listSummary.GetItemText(l,0);
		for(int m=0;m<nAirlineCount;m++)
		{
			m_listSummary.GetColumn(m+1,&lvCol);
			CString strAirline=lvCol.pszText;
			for(int n=0;n<nCount;n++)
			{
				if(m_utility.GetDataOfGateAirline().at(n).m_strGateID==strGate&&
					m_utility.GetDataOfGateAirline().at(n).m_strAirline==strAirline)
				{
					CString strTemp;
					strTemp=m_utility.GetDataOfGateAirline().at(n).m_timeOccupied.printTime();
					strTemp=strTemp.Left(strTemp.GetLength()-3);
					m_listSummary.SetItemText(l,m+1,strTemp);
					break;
				}
			}
		}
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CGateAssignSummaryDlg::OnBtnGateassSummaryExport() 
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
		int nColumns = m_listSummary.GetHeaderCtrl()->GetItemCount();
		HDITEM hdItem;
		memset(&hdItem, sizeof(hdItem), 0);
		char sBuf[256]="";
		hdItem.pszText = sBuf;
		hdItem.cchTextMax = 255;
		for(int i=0; i<nColumns; i++)
		{
			//m_listSummary.GetHeaderCtrl()->GetItem(i, &hdItem);
			file.writeField ( (LPCTSTR)m_strTitleList.GetAt(i) );
		}
		file.writeLine();
		//content
		for(int nRow=0; nRow<m_listSummary.GetItemCount(); nRow++)
		{
			for(int nCol=0; nCol<nColumns; nCol++)
			{
				file.writeField( (LPCTSTR)m_listSummary.GetItemText( nRow, nCol));
			}
			file.writeLine();
		}

	}
	
	
}

void CGateAssignSummaryDlg::OnLvnItemchangedListGataAssignSummary(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CGateAssignSummaryDlg::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	if (!m_btnCancel.GetSafeHwnd())
		return;
	CRect btnrc;
	m_btnCancel.GetWindowRect( &btnrc );

	m_listSummary.MoveWindow(11,11,cx-24,cy-27-btnrc.Height());
	m_bExport.MoveWindow(11,cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height());
	m_btnOk.MoveWindow( cx-15-2*btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
}

void CGateAssignSummaryDlg::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 683;
	lpMMI->ptMinTrackSize.y = 392;
	CDialog::OnGetMinMaxInfo(lpMMI);
}
