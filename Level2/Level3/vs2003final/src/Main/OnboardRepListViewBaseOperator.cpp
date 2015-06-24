#include "StdAfx.h"
#include "resource.h"
#include ".\onboardreplistviewbaseoperator.h"
#include "OnboardReportListView.h"
#include "../MFCExControl/ARCGridCtrl.h"
#include "ARCReportManager.h"
#include "TermPlanDoc.h"
#include "Common/SimAndReportManager.h"


COnboardRepListViewBaseOperator::COnboardRepListViewBaseOperator( ARCGridCtrl *pListCtrl, CARCReportManager* pARCReportManager,OnboardReportListView *pListView )
:CRepListViewBaseOperator(pListCtrl,pARCReportManager,pListView)
{
	m_nColumnCount = 0;
	m_sExtension = _T("");
}
COnboardRepListViewBaseOperator::~COnboardRepListViewBaseOperator(void)
{
}
void COnboardRepListViewBaseOperator::LoadReport(LPARAM lHint, CObject* pHint)
{
	if(GetARCReportManager() == NULL)
		return;
	GetFileReportTypeAndExtension();
	LoadListData();
}
CARCReportManager* COnboardRepListViewBaseOperator::GetARCReportManager()
{
	return m_pReportManager;
}
void COnboardRepListViewBaseOperator::GetFileReportTypeAndExtension()
{
}

void COnboardRepListViewBaseOperator::LoadListData()
{
}
//CString COnboardRepListViewBaseOperator::GetAbsDateTime( LPCTSTR elaptimestr, BOOL needsec /*= TRUE*/ )
//{
//	CSimAndReportManager *ptSim = GetTermPlanDoc()->GetTerminal().GetSimReportManager();
//	CStartDate tstartdate = ptSim->GetStartDate();
//	ElapsedTime etime;
//	etime.SetTime(elaptimestr);
//
//	CString retstr, tstr;
//	bool bAbsDate;
//	COleDateTime _tdate, _ttime;
//	int _nDtIdx;
//	tstartdate.GetDateTime(etime, bAbsDate, _tdate, _nDtIdx, _ttime);
//	if(!needsec)
//		tstr = _ttime.Format(" %H:%M");
//	else
//		tstr = _ttime.Format(" %H:%M:%S");
//	if(bAbsDate)
//	{
//		retstr = _tdate.Format("%Y-%m-%d");
//	}
//	else
//	{
//		retstr.Format("Day%d", _nDtIdx + 1 );
//	}
//	retstr += tstr;
//
//	return retstr;
//}
void COnboardRepListViewBaseOperator::SetListHeader()
{
}

void COnboardRepListViewBaseOperator::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	
    switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// Request prepaint notifications for each item.
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
		
    case CDDS_ITEMPREPAINT: // Requested notification
		int nDBIdx = GetListCtrl().GetItemData(lplvcd->nmcd.dwItemSpec);
		
		if( m_vFlag.size() != 0 && m_vFlag[nDBIdx] )
		{
			lplvcd->clrText = RGB(255, 0, 0);
			//lplvcd->clrTextBk = RGB(255,0,0);
		}
		*pResult = CDRF_DODEFAULT;
		break;
	}
}
void COnboardRepListViewBaseOperator::OnListviewExport()
{
	if(m_sExtension.IsEmpty())		//If no items listed, then return;
		return;
	CString upExtension = m_sExtension;
	upExtension.MakeUpper();
	CFileDialog filedlg( FALSE,m_sExtension, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report files (*." + m_sExtension + ") | *." + m_sExtension + ";*." + upExtension + "|All type (*.*)|*.*|", NULL );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

	CString sFileName;
	if( GetARCReportManager()->GetLoadReportType() == load_by_user )	
	{
		sFileName = GetARCReportManager()->GetUserLoadReportFile();
	}
	else				  // load_by_system
	{
		sFileName = GetTermPlanDoc()->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( GetTermPlanDoc()->m_ProjInfo.path );
	}

	CFileFind fileFind;
	if(fileFind.FindFile(csFileName))
	{
		if(MessageBox(NULL,"File already exists, are you sure to overwrite it?", "Warning!", MB_YESNO) == IDNO)
			return;
	}
	CopyFile(sFileName, csFileName, TRUE);
}

OnboardReportListView * COnboardRepListViewBaseOperator::getListView() const
{
	return (OnboardReportListView *)m_pListView;
}











