// DlgMovieWriter2.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "3DView.h"
#include "RecordedCameras.h"
#include "MovieWriter.h"
#include "PBuffer.h"
#include "AnimationTimeManager.h"
#include "Movie.h"
#include "DlgMovieWriter2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMovieWriter2 dialog


CDlgMovieWriter2::CDlgMovieWriter2(CTermPlanDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMovieWriter2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMovieWriter2)
	m_sText = _T("");
	//}}AFX_DATA_INIT
	VERIFY(m_pDoc = pDoc);
	m_pBits = NULL;
	m_pWriter = NULL;
	m_pParent = pParent;
}


void CDlgMovieWriter2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMovieWriter2)
	DDX_Control(pDX, IDC_PROGRESS_WRITEMOVIE, m_progressMW);
	DDX_Text(pDX, IDC_STATIC_TEXT, m_sText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMovieWriter2, CDialog)
	//{{AFX_MSG_MAP(CDlgMovieWriter2)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMovieWriter2 message handlers

BOOL CDlgMovieWriter2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_progressMW.SetRange(0,100);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMovieWriter2::RecordMovie(int nMovieIdx)
{
}

void RecordWalkthrough(int nWalkthroughIdx)
{
}

void CDlgMovieWriter2::RecordLive()
{
	ASSERT(m_pDoc->m_pRecordedCameras);
	int nCount = m_pDoc->m_pRecordedCameras->GetCameraCount();
	UINT fps = m_pDoc->m_pRecordedCameras->GetFPS();
	long starttime = m_pDoc->m_pRecordedCameras->GetStartTime();
	long endtime = m_pDoc->m_pRecordedCameras->GetEndTime();
	long diftime = endtime-starttime;
	double dFPS = 10.0*m_pDoc->m_animData.nAnimSpeed*((double)nCount)/diftime;

	CFileStatus fs;
	CString sFileName;
	int i=0;
	do {
		sFileName.Format("%s\\%s%.4d.wmv", m_pDoc->m_ProjInfo.path, m_pDoc->m_ProjInfo.name, i);
		i++;
	} while(CFile::GetStatus(sFileName, fs));

	m_sText.Empty();
	m_sText.Format("Preparing file \'%s\'...",sFileName);
	UpdateData(FALSE);

	PumpMessages();

	delete m_pWriter;
	m_pWriter = new CMovieWriter;
	
	static const int nWidth = 640;
	static const int nHeight = 480;
	static const int nBPP = 3;
	static const DWORD dwSize = nWidth*nHeight*nBPP;

	ASSERT(!sFileName.IsEmpty());
	
	if(m_pWriter->InitializeMovie(sFileName, nWidth, nHeight, fps) != S_OK)
		return;
	if(m_pBits)
		delete [] m_pBits;
	m_pBits = new BYTE[dwSize];
	UINT nStep = 10*m_pDoc->m_animData.nAnimSpeed/fps;
	UINT nFrameIdx = 0;
	PBuffer pbuff(nWidth, nHeight, 0);
	pbuff.Initialize(NULL, true);
	m_progressMW.SetPos(0);
	m_progressMW.UpdateWindow();
	m_bIsCancel = FALSE;
	for(UINT t=starttime; t<endtime && nFrameIdx<nCount; t+=nStep) {
		m_pDoc->m_animData.nLastAnimTime = t;
		C3DView* pView = m_pDoc->Get3DView();
		ASSERT(pView);
		*(pView->GetCamera()) = m_pDoc->m_pRecordedCameras->GetCameraAt(nFrameIdx);
		pView->RenderOffScreen(&pbuff, nWidth, nHeight, m_pBits);
		m_pWriter->WriteFrame(nFrameIdx, m_pBits);

		PumpMessages();

		if(m_bIsCancel) {
			return;
		}

		m_sText.Empty();
		m_sText.Format("Writing file \'%s\'...\nWriting frame %d of %d.",sFileName,nFrameIdx,nCount);
		UpdateData(FALSE);

		int pct = 100*nFrameIdx/nCount;
		m_progressMW.SetPos(pct);
		m_progressMW.UpdateWindow();

		nFrameIdx++;
	}

	m_progressMW.SetPos(100);
	m_progressMW.UpdateWindow();

	m_pWriter->EndMovie();

	DestroyWindow();
}

void CDlgMovieWriter2::PumpMessages()
{
	//pump messages
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CDlgMovieWriter2::OnCancel() 
{
	m_bIsCancel = TRUE;

	DestroyWindow();
}

void CDlgMovieWriter2::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}

void CDlgMovieWriter2::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call CDialog::OnPaint() for painting messages
}

int CDlgMovieWriter2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	ANIMTIMEMGR->EndTimer();
	
	return 0;
}

void CDlgMovieWriter2::OnDestroy() 
{
	delete m_pWriter;
	m_pWriter = NULL;

	if(m_pBits) {
		delete [] m_pBits;
		m_pBits = NULL;
	}

	ANIMTIMEMGR->StartTimer();
	
	CDialog::OnDestroy();
}

void CDlgMovieWriter2::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}
