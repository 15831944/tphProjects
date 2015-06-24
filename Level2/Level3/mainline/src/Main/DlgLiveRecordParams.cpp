// DlgLiveRecordParams.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "DlgLiveRecordParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLiveRecordParams dialog


CDlgLiveRecordParams::CDlgLiveRecordParams(CTermPlanDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLiveRecordParams::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLiveRecordParams)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
}


void CDlgLiveRecordParams::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLiveRecordParams)
	DDX_Control(pDX, IDC_TREE_PARAMS, m_treeParams);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLiveRecordParams, CDialog)
	//{{AFX_MSG_MAP(CDlgLiveRecordParams)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgLiveRecordParams::InitTree()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	cni.net=NET_NORMAL;
	cni.nt=NT_NORMAL;

	HTREEITEM hFileName = m_treeParams.InsertItem("File Name", cni, FALSE);
	HTREEITEM hDimensions = m_treeParams.InsertItem("Video Dimensions", cni, FALSE);
	HTREEITEM hFPS = m_treeParams.InsertItem("Video Framerate", cni, FALSE);
	HTREEITEM hCodec =  m_treeParams.InsertItem("Video Encoder", cni, FALSE);
	//HTREEITEM hRecordFPS = m_treeParams.InsertItem("Recording FPS", cni, FALSE);
	//HTREEITEM hPlaybackFPS = m_treeParams.InsertItem("Playback FPS", cni, FALSE);
	HTREEITEM hMethod = m_treeParams.InsertItem("Encoding Method", cni, FALSE);
	HTREEITEM hParams = m_treeParams.InsertItem("Encoding Parameters", cni, FALSE);

	cni.nt=NT_RADIOBTN;
	
	if(m_pDoc->m_videoParams.GetAutoFileName()) {
		m_treeParams.InsertItem("Auto", cni, TRUE, FALSE, hFileName);
		cni.net=NET_SHOW_DIALOGBOX;
		m_hChooseFile = m_treeParams.InsertItem("Choose File...", cni, FALSE, FALSE, hFileName);
		cni.net=NET_NORMAL;
		//m_treeParams.SetEnableStatus(hChooseFile, FALSE);
	}
	else {
		m_treeParams.InsertItem("Auto", cni, FALSE, FALSE, hFileName);
		cni.net=NET_SHOW_DIALOGBOX;
		m_hChooseFile = m_treeParams.InsertItem(m_pDoc->m_videoParams.GetFileName(), cni, TRUE, FALSE, hFileName);
		cni.net=NET_NORMAL;
	}

	HTREEITEM hCBR = m_treeParams.InsertItem("Constant Bit Rate (CBR)", cni, FALSE, FALSE, hMethod);
	m_treeParams.SetEnableStatus(hCBR, FALSE);
	m_treeParams.InsertItem("Variable Bit Rate (VBR)", cni, TRUE, FALSE, hMethod);

	cni.nt=NT_NORMAL;
	m_treeParams.InsertItem("Windows Media 9", cni, TRUE, FALSE, hCodec);

	cni.net = NET_EDITSPIN_WITH_VALUE;
	CString s;
	int nWidth = m_pDoc->m_videoParams.GetFrameWidth();
	int nHeight = m_pDoc->m_videoParams.GetFrameHeight();
	s.Format("Width (pixels): %d",nWidth);
	m_hWidth = m_treeParams.InsertItem(s, cni, TRUE, FALSE, hDimensions);
	m_treeParams.SetItemData(m_hWidth, (DWORD) nWidth);
	m_treeParams.SetItemValueRange(m_hWidth, 64, 1280, TRUE);
	s.Format("Height (pixels): %d",nHeight);
	m_hHeight = m_treeParams.InsertItem(s, cni, TRUE, FALSE, hDimensions);
	m_treeParams.SetItemData(m_hHeight, (DWORD) nHeight);
	m_treeParams.SetItemValueRange(m_hHeight, 64, 1280, TRUE);

	
	int nFPS = m_pDoc->m_videoParams.GetFPS();
	s.Format("FPS: %d", nFPS);
	m_hFPS = m_treeParams.InsertItem(s, cni, TRUE, FALSE, hFPS);
	m_treeParams.SetItemData(m_hFPS, (DWORD) nFPS);
	m_treeParams.SetItemValueRange(m_hFPS, 1, 30, TRUE);

	int nQuality = m_pDoc->m_videoParams.GetQuality();
	s.Format("Quality (%%): %d", nQuality);
	m_hQuality = m_treeParams.InsertItem(s, cni, TRUE, FALSE, hParams);
	m_treeParams.SetItemData(m_hQuality, (DWORD) nQuality);
	m_treeParams.SetItemValueRange(m_hQuality, 1, 100, TRUE);

	m_treeParams.Expand(hFileName,TVE_EXPAND);
	m_treeParams.Expand(hDimensions,TVE_EXPAND);
	m_treeParams.Expand(hFPS,TVE_EXPAND);
	m_treeParams.Expand(hCodec,TVE_EXPAND);
	m_treeParams.Expand(hMethod,TVE_EXPAND);
	m_treeParams.Expand(hParams,TVE_EXPAND);
	//m_treeParams.Expand(hRecordFPS,TVE_EXPAND);
	//m_treeParams.Expand(hPlaybackFPS,TVE_EXPAND);
	

}

/////////////////////////////////////////////////////////////////////////////
// CDlgLiveRecordParams message handlers

BOOL CDlgLiveRecordParams::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitTree();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgLiveRecordParams::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message) {
		case UM_CEW_STATUS_CHANGE: //radio btn change
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			if(hItem == m_hChooseFile) {
				BOOL bChooseFile = m_treeParams.IsRadioItem(m_hChooseFile);
				
				if(bChooseFile && m_pDoc->m_videoParams.GetAutoFileName()) {
	
					// TRACE("Choose file selected\n");
					m_pDoc->m_videoParams.SetAutoFileName(FALSE);
					this->SendMessage(UM_CEW_SHOW_DIALOGBOX_BEGIN, (WPARAM)m_hChooseFile, (LPARAM)NULL);
				}
			}
			else {
				m_pDoc->m_videoParams.SetAutoFileName(TRUE);
			}

			break;
		}
		case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			ASSERT(hItem == m_hChooseFile);
			CFileDialog fileDlg(FALSE, _T("wmv"), _T("*.wmv"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Windows Media 9 files (*.wmv)|*.wmv|All Files (*.*)|*.*||"), NULL, 0, FALSE );
			if(fileDlg.DoModal() == IDOK) 
			{
				m_pDoc->m_videoParams.SetFileName((LPCTSTR)fileDlg.GetPathName());
				m_pDoc->m_videoParams.SetAutoFileName(FALSE);
				m_treeParams.SetItemText( m_hChooseFile, (LPCTSTR)fileDlg.GetPathName());
				m_treeParams.SetRadioStatus(m_hChooseFile, TRUE);
			}
			break;
		}
		case UM_CEW_SHOW_DIALOGBOX_END:
			break;
			
		case UM_CEW_EDITSPIN_END:
		case UM_CEW_EDIT_END:
		{	
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pInfo = m_treeParams.GetItemNodeInfo( hItem );
			if( hItem ==  m_hFPS ) 
			{
				CString strValue = *((CString*)lParam);
				int nFPS;
				if( strValue == "" )
					nFPS = 20;
				else
					nFPS = atoi( strValue );
					
				m_pDoc->m_videoParams.SetFPS(nFPS);
				m_treeParams.SetItemText( m_hFPS, "FPS:  "+strValue );
				m_treeParams.SetItemData( m_hFPS, (DWORD) nFPS);				
			}
			else if( hItem == m_hQuality)
			{
				CString strValue = *((CString*)lParam);
				int nQuality;
				if( strValue == "" )
					nQuality = 100;
				else
					nQuality = atoi( strValue );
					
				m_pDoc->m_videoParams.SetQuality(nQuality);
				m_treeParams.SetItemText( m_hQuality, "Quality (%):  "+strValue );
				m_treeParams.SetItemData( m_hQuality, (DWORD) nQuality);

			}
			else if( hItem == m_hWidth )
			{
				CString strValue = *((CString*)lParam);
				int nWidth;
				if( strValue == "" )
					nWidth = 640;
				else
					nWidth = atoi( strValue );

				int nRem;
				if((nRem=(nWidth%4)) != 0) {
					AfxMessageBox(_T("The width of the video must be a multiple of 4.\nThe width you entered has been asjusted accordingly."));
					nWidth+=(4-nRem);
					strValue.Format("%d", nWidth);
				}
					
				m_pDoc->m_videoParams.SetFrameWidth(nWidth);
				m_treeParams.SetItemText( m_hWidth, "Width (pixels):  "+strValue );
				m_treeParams.SetItemData( m_hWidth, (DWORD) nWidth);
			}
			else if( hItem == m_hHeight )
			{
				CString strValue = *((CString*)lParam);
				int nHeight;
				if( strValue == "" )
					nHeight = 480;
				else
					nHeight = atoi( strValue );

				int nRem;
				if((nRem=(nHeight%4)) != 0) {
					AfxMessageBox(_T("The height of the video must be a multiple of 4.\nThe height you entered has been asjusted accordingly."));
					nHeight+=(4-nRem);
					strValue.Format("%d", nHeight);
				}
					
				m_pDoc->m_videoParams.SetFrameHeight(nHeight);
				m_treeParams.SetItemText( m_hHeight, "Height (pixels):  "+strValue );
				m_treeParams.SetItemData( m_hHeight, (DWORD) nHeight);
			}
			break;
		}
	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgLiveRecordParams::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDblClk(nFlags, point);
}
