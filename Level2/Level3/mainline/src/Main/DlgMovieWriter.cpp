// DlgMovieWriter.cpp : implementation file
//

#include "stdafx.h"
#include "DlgMovieWriter.h"
#include "TermPlanDoc.h"
#include "3DView.h"
#include "Floor2.h"
#include "RecordedCameras.h"
#include "MovieWriter.h"
#include "24bitBMP.h"
#include "Movie.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/PanoramaViewData.h"
#include "MovieRenderEngine.h"
#include "Renderer/RenderEngine/RenderEngine.h"
#include "Render3DView.h"
#include ".\dlgmoviewriter.h"
#include "Inputs\Walkthroughs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline
static BOOL PumpMessages()
{
	//return AfxGetApp()->PumpMessage();
	MSG msg;
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
		{
			return TRUE;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgMovieWriter dialog


CDlgMovieWriter::CDlgMovieWriter(CTermPlanDoc* pDoc, int nType, int nIdx, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMovieWriter::IDD, pParent)
	, m_pWalkthrough(NULL)
{
	//{{AFX_DATA_INIT(CDlgMovieWriter)
	m_sText = _T("");
	//}}AFX_DATA_INIT
	VERIFY(m_pDoc = pDoc);
	//m_pBits = NULL;
//	m_pWriter = NULL;
	m_pParent = pParent;
	m_nMovieIdx = nIdx;
	m_nType = nType;

	m_pPanoramViewData = NULL;
}


void CDlgMovieWriter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMovieWriter)
	DDX_Control(pDX, IDC_PROGRESS_WRITEMOVIE, m_progressMW);
	DDX_Text(pDX, IDC_STATIC_TEXT, m_sText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMovieWriter, CDialog)
	//{{AFX_MSG_MAP(CDlgMovieWriter)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMovieWriter message handlers

BOOL CDlgMovieWriter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_progressMW.SetRange(0,100);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void WriteOneFrame(UINT nFrameIdx, IMovieReader* pReader , CMovieWriter* pWrite)
{
	FrameInfo finfo;
	pWrite->BegineFrameWrite(nFrameIdx,finfo);	
	pReader->RenderScreenToMem(pWrite->GetWidth(),pWrite->GetHeight(),finfo.pByteBuffer);
	pWrite->EndFrameWrite(finfo);	
}

void CDlgMovieWriter::RecordMovie(const CMovie* pMovie, const CVideoParams& videoparams, long starttime, long endtime)
{	
	ASSERT(pMovie);
	int nAnimSpeed = pMovie->GetAnimSpeed();
	const UINT fps = videoparams.GetFPS();
	long nCount = fps*(endtime-starttime)/(10*nAnimSpeed);

	int nBPP = 3;
	int nWidth = videoparams.GetFrameWidth();
	int nHeight = videoparams.GetFrameHeight();
	DWORD dwSize = nWidth*nHeight*nBPP;
	std::vector<BYTE> mBytesBuffer;
	mBytesBuffer.resize(dwSize);


	CString sFileName = GetMovieFileName();
	ASSERT(!sFileName.IsEmpty());

	//init writer
	CMovieWriter mvWriter;
	mvWriter.InitializeMovie(sFileName, nWidth, nHeight, fps, videoparams.GetQuality());

	//start anim
	if(m_pDoc->m_eAnimState == CTermPlanDoc::anim_none) {
		if(!m_pDoc->StartAnimation(FALSE)){
			THROW_STDEXCEPTION(_T("Start Animation Failed!"));
		}
	}
	//start read
	std::auto_ptr<IMovieReader> pMovieReader ( CreateMovieReader());
	if(!pMovieReader.get()){	THROW_STDEXCEPTION(_T("3DView is not Open!"));	}

	m_progressMW.SetPos(0);
	m_progressMW.UpdateWindow();
	m_bIsCancel = FALSE;


	UINT nStep = 10*nAnimSpeed/fps;
	UINT nFrameIdx = 0;

	C3DCamera newCam = 	pMovieReader->BeginRead();

	//IWMWriterAdvanced::SetSyncTolerance(5000);
	for(UINT t=starttime; t<static_cast<UINT>(endtime); t+=nStep) {
		m_pDoc->m_animData.nLastAnimTime = t;
		//update camera
		pMovie->Interpolate(t, newCam);
		pMovieReader->setCamera(newCam);

		BYTE* pBuffer = &mBytesBuffer[0];
		//write frame to 
		pMovieReader->RenderScreenToMem(mvWriter.GetWidth(),mvWriter.GetHeight(),pBuffer);
		mvWriter.WriteFrame(nFrameIdx,pBuffer);
		//WriteOneFrame(nFrameIdx, pMovieReader.get() ,&mvWriter);

		//pump messages
		BOOL bCanceled =PumpMessages();

		if(m_bIsCancel || bCanceled) {
			pMovieReader->EndRead();
			mvWriter.EndMovie();
			EndDialog(IDCANCEL);
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

	pMovieReader->EndRead();
	mvWriter.EndMovie();	
	EndDialog(IDOK);	
}

void CDlgMovieWriter::RecordMovie()
{
	ASSERT(m_pDoc->m_movies.GetMovieCount() > m_nMovieIdx && 0 <= m_nMovieIdx);

	CMovie* pMovie = m_pDoc->m_movies.GetMovie(m_nMovieIdx);
	if(!pMovie)
		return;

	if (pMovie->GetKFCount())
	{
		RecordMovie(pMovie, m_pDoc->m_videoParams, pMovie->GetKFTime(0), pMovie->GetKFTime(pMovie->GetKFCount()-1));
	}
}

void CDlgMovieWriter::BuildMovieFromWalkthrough(const CWalkthrough* pWT, CTermPlanDoc* pDoc, CMovie* movie)
{
	const ARCVector3& viewpointOffset = pWT->GetViewpointOffset();
	const ARCVector3& directionOffset = pWT->GetViewpointDirection();

	//create a movie from the pax hits
	long starttime = pWT->GetStartTime();
	long endtime = pWT->GetEndTime();

	if(EnvMode_Terminal == pWT->GetEnvMode()) {

		MobLogEntry element;
		PaxLog* pPaxLog = pDoc->GetTerminal().paxLog;
		pPaxLog->getItem(element, pWT->GetPaxID());
		element.SetOutputTerminal(&(pDoc->GetTerminal()));
		element.SetEventLog(pDoc->GetTerminal().m_pMobEventLog);
		long nEventCount = element.getCount();

		long entrytime = element.getEntryTime();
		long exittime = element.getExitTime();

		C3DView* pView = pDoc->Get3DView();
		ASSERT(pView);

		movie->SetAnimSpeed(pDoc->m_animData.nAnimSpeed);

		ARCVector3 pos;
		ARCVector3 dir;
		double dAlt;
		long nLastKFIdx=-1;

		//assume min 2 hits
		ASSERT(nEventCount > 1);
		//assume that WT time is within to hit time range
		ASSERT(starttime>=entrytime && endtime<=exittime);
		ASSERT(starttime<endtime);

		long nNextHit = 0;
		if(starttime > entrytime) 
		{
			//find 1st hit with time > starttime starting with hit 1
			long ii=1; 
			for(; ii<nEventCount; ii++) 
			{
				const MobEventStruct& pes = element.getEvent(ii);
				if(pes.time > starttime)
					break;
			}
			ASSERT(ii>0 && ii<nEventCount);
			//interpolate pos based on hit at ii and ii-1

			nNextHit = ii;
		}

		for(long ii=nNextHit; ii<nEventCount; ii++) {
			
			MobEventStruct pes = element.getEvent(ii);
			UINT index = static_cast<UINT>( pes.z/SCALE_FACTOR );
			if (index > (UINT)(pDoc->GetCurModeFloor().GetCount()))
				index = pDoc->GetCurModeFloor().GetCount() -1;
			dAlt=pDoc->GetCurModeFloor().m_vFloors[index]->Altitude();
			pos[VX]=pes.x; pos[VY]=pes.y; pos[VZ]=dAlt;

			//calc direction
			double dMag;
			bool bDirOK = false;
			bool bContinue = true;
			long tmpIdx = ii;

			dir[VZ] = 0.0;

			while(bContinue) {
				tmpIdx++;
				//check if in range
				if(tmpIdx<nEventCount) {
					MobEventStruct pesTemp = element.getEvent(tmpIdx);
					dir[VX]=pesTemp.x - pes.x; dir[VY]=pesTemp.y - pes.y;
					dMag = dir.Magnitude();
					if(dMag>ARCMath::EPSILON) {
						bContinue = false;
						bDirOK = true;
					}
				}
				else
					bContinue = false;
			}

			if(!bDirOK) {
				bContinue = true;
				tmpIdx = ii;
				while(bContinue) {
					tmpIdx--;
					//check if in range
					if(tmpIdx>=0) {
						MobEventStruct pesTemp = element.getEvent(tmpIdx);
						dir[VX]=pes.x - pesTemp.x; dir[VY]= pes.y - pesTemp.y;
						dMag = dir.Magnitude();
						if(dMag>ARCMath::EPSILON) {
							bContinue = false;
							bDirOK = true;
						}
					}
					else bContinue = false;
				}
			}

			ASSERT(bDirOK);
			dir.Normalize();

			// TRACE("[Hit %d] + t=%d + POS: %.2f, %.2f, %.2f  |  DIR: %.2f, %.2f, %.2f\n", ii, pes.time, pos[VX], pos[VY], pos[VZ], dir[VX], dir[VY], dir[VZ]);

			pos[VX]+=viewpointOffset[VX]*dir[VX];
			pos[VY]+=viewpointOffset[VY]*dir[VY];
			pos[VZ]+=viewpointOffset[VZ];
			dir[VX]*=directionOffset[VX];
			dir[VY]*=directionOffset[VY];
			dir[VZ]=directionOffset[VZ];

			C3DCamera cam(*(pView->GetCamera()));
			cam.SetCamera(pos, dir, ARCVector3(0.0,0.0,1.0));
			cam.AutoAdjustCameraFocusDistance(dAlt);

			if(nLastKFIdx!=-1 && movie->GetKFTime(nLastKFIdx) == pes.time) {
				movie->DeleteKF(pes.time);
				nLastKFIdx--;
			}
			movie->InsertKF(cam, pes.time);
			nLastKFIdx++;
		}
	}
	else if(EnvMode_AirSide == pWT->GetEnvMode()) {

		CAirsideSimLogs& airsideSimLogs = pDoc->GetAirsideSimLogs();
		AirsideFlightLog& airsideFlightLog = airsideSimLogs.m_airsideFlightLog;
		ALTAirport altApt;
		int aptID = InputAirside::GetAirportID(pDoc->GetProjectID());
		altApt.ReadAirport(aptID);
		DistanceUnit dAlt = altApt.getElevation();

		AirsideFlightLogEntry element;
		element.SetEventLog(&(airsideSimLogs.m_airsideFlightEventLog));

		airsideFlightLog.getItem(element, pWT->GetPaxID()-C3DView::SEL_FLIGHT_OFFSET);

		long nEventCount = element.getCount();

		long entrytime = element.getEntryTime();
		long exittime = element.getExitTime();


		C3DView* pView = pDoc->Get3DView();
		ASSERT(pView);

		movie->SetAnimSpeed(pDoc->m_animData.nAnimSpeed);

		ARCVector3 pos;
		ARCVector3 dir;
		long nLastKFIdx=-1;

		//assume min 2 hits
		ASSERT(nEventCount > 1);
		//assume that WT time is within to hit time range
		ASSERT(starttime>=entrytime && endtime<=exittime);
		ASSERT(starttime<endtime);

		long nNextHit = 0;
		if(starttime > entrytime) {
			//find 1st hit with time > starttime starting with hit 1
			long ii=1; 
			for(; ii<nEventCount; ii++) 
			{
				const AirsideFlightEventStruct& pes = element.getEvent(ii);
				if(pes.time > starttime)
					break;
			}
			ASSERT(ii>0 && ii<nEventCount);
			//interpolate pos based on hit at ii and ii-1

			nNextHit = ii;
		}

		for(long ii=nNextHit; ii<nEventCount; ii++) {

			const AirsideFlightEventStruct& pes = element.getEvent(ii);
			pos[VX]=pes.x; pos[VY]=pes.y; pos[VZ]=pes.z - dAlt;

			//calc direction
			double dMag;
			bool bDirOK = false;
			bool bContinue = true;
			long tmpIdx = ii;

			dir[VZ] = 0.0;

			while(bContinue) {
				tmpIdx++;
				//check if in range
				if(tmpIdx<nEventCount) {
					const AirsideFlightEventStruct& pesTemp = element.getEvent(tmpIdx);
					dir[VX]=pesTemp.x - pes.x; dir[VY]=pesTemp.y - pes.y;
					dMag = dir.Magnitude();
					if(dMag>ARCMath::EPSILON) {
						bContinue = false;
						bDirOK = true;
					}
				}
				else
					bContinue = false;
			}

			if(!bDirOK) {
				bContinue = true;
				tmpIdx = ii;
				while(bContinue) {
					tmpIdx--;
					//check if in range
					if(tmpIdx>=0) {
						const AirsideFlightEventStruct& pesTemp = element.getEvent(tmpIdx);
						dir[VX]=pes.x - pesTemp.x; dir[VY]= pes.y - pesTemp.y;
						dMag = dir.Magnitude();
						if(dMag>ARCMath::EPSILON) {
							bContinue = false;
							bDirOK = true;
						}
					}
					else bContinue = false;
				}
			}

			ASSERT(bDirOK);
			dir.Normalize();
			if(pes.IsBackUp)
				dir =  -dir;

			// TRACE("[Hit %d] + t=%d + POS: %.2f, %.2f, %.2f  |  DIR: %.2f, %.2f, %.2f\n", ii, pes.time, pos[VX], pos[VY], pos[VZ], dir[VX], dir[VY], dir[VZ]);

			//pos[VX]+=viewpointOffset[VX]*dir[VX];
			//pos[VY]+=viewpointOffset[VY]*dir[VY];
			//pos[VZ]+=viewpointOffset[VZ];

			pos += viewpointOffset[VX] * dir;
			pos += viewpointOffset[VY] * ARCVector3(-dir[VY], dir[VX], dir[VZ]);
			pos[VZ] += viewpointOffset[VZ];

			dir[VX]*=directionOffset[VX];
			dir[VY]*=directionOffset[VY];
			dir[VZ]=directionOffset[VZ];
			
			C3DCamera cam(*(pView->GetCamera()));
			cam.SetCamera(pos, dir, ARCVector3(0.0,0.0,1.0));
			cam.AutoAdjustCameraFocusDistance(0.0);

			if(nLastKFIdx!=-1 && movie->GetKFTime(nLastKFIdx) == pes.time) {
				movie->DeleteKF(pes.time);
				nLastKFIdx--;
			}
			movie->InsertKF(cam, pes.time);
			nLastKFIdx++;
		}
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgMovieWriter::RecordWalkthrough()
{
	CWalkthrough* pWT=NULL;

	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		ASSERT(m_pDoc->m_walkthroughs->GetWalkthroughCount() > m_nMovieIdx && 0 <= m_nMovieIdx);
		pWT = m_pDoc->m_walkthroughs->GetWalkthrough(m_nMovieIdx);
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		ASSERT(m_pDoc->m_walkthroughsFlight->GetWalkthroughCount() > m_nMovieIdx && 0 <= m_nMovieIdx);
		pWT = m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nMovieIdx);
	}
	else if (EnvMode_LandSide == m_pDoc->m_systemMode)
	{
		ASSERT(m_pDoc->m_walkthroughsVehicle->GetWalkthroughCount() > m_nMovieIdx && 0 <= m_nMovieIdx);
		pWT = m_pDoc->m_walkthroughsVehicle->GetWalkthrough(m_nMovieIdx);
	}
	else 
	{
		ASSERT(FALSE);
	}
	CMovie movie("temp");
	CWaitCursor wc;
	BuildMovieFromWalkthrough(pWT, m_pDoc, &movie);
	wc.Restore();
	RecordMovie(&movie, m_pDoc->m_videoParams, pWT->GetStartTime(), pWT->GetEndTime());	
}

void CDlgMovieWriter::RecordLive()
{
	ASSERT(m_pDoc->m_pRecordedCameras);
	UINT fps = m_pDoc->m_pRecordedCameras->GetFPS();
	long starttime = m_pDoc->m_pRecordedCameras->GetStartTime();
	long endtime = m_pDoc->m_pRecordedCameras->GetEndTime();

	
	int nBPP = 3;
	int nWidth = m_pDoc->m_videoParams.GetFrameWidth();
	int nHeight = m_pDoc->m_videoParams.GetFrameHeight();
	//DWORD dwSize = nWidth*nHeight*nBPP;

	CString sFileName = GetMovieFileName();
	ASSERT(!sFileName.IsEmpty());
	

	CMovieWriter mvWriter;
	mvWriter.InitializeMovie(sFileName, nWidth, nHeight, fps, m_pDoc->m_videoParams.GetQuality());	


	std::auto_ptr<IMovieReader> pMovieReader ( CreateMovieReader());
	if(!pMovieReader.get()){	THROW_STDEXCEPTION(_T("3DView is not Open!"));	}
	pMovieReader->BeginRead();
	
	m_progressMW.SetPos(0);
	m_progressMW.UpdateWindow();
	m_bIsCancel = FALSE;

	
	UINT nStep = 0;
	int nCount = 0;
	for(UINT t=starttime; t<static_cast<UINT>(endtime); t+=nStep) {
		int nSpeed = m_pDoc->m_pRecordedCameras->GetSpeed(t).m_speed;
		nStep = 10*nSpeed/fps;
		nCount++;
	}

	UINT nFrameIdx = 0;
	for(UINT t=starttime; t<static_cast<UINT>(endtime); t+=nStep) {
		m_pDoc->m_animData.nLastAnimTime = t;
		int nSpeed = m_pDoc->m_pRecordedCameras->GetSpeed(t).m_speed;
		nStep = 10*nSpeed/fps;	
		
		pMovieReader->setCamera(m_pDoc->m_pRecordedCameras->GateCamera(t).m_cam);
		
		WriteOneFrame(nFrameIdx, pMovieReader.get() ,&mvWriter);	

		//pump messages
		BOOL bCanceled = PumpMessages();

		if(m_bIsCancel || bCanceled) {
			
			pMovieReader->EndRead();
			mvWriter.EndMovie();
			EndDialog(IDCANCEL);
		
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

	m_pDoc->m_animData.nLastAnimTime = endtime;
	
	
	m_progressMW.SetPos(100);
	m_progressMW.UpdateWindow();

	pMovieReader->EndRead();
	mvWriter.EndMovie();	
	EndDialog(IDOK);	

}

void CDlgMovieWriter::RecordWalkthroughRender()
{
	if (!m_pWalkthrough)
		return;

	int fps = m_pDoc->m_videoParams.GetFPS();
	long starttime = m_pWalkthrough->GetStartTime();
	long endtime = m_pWalkthrough->GetEndTime();
	long nStep = 10*m_pDoc->m_animData.nAnimSpeed/fps;


	const int nBPP = 3;
	const int nWidth = m_pDoc->m_videoParams.GetFrameWidth();
	const int nHeight = m_pDoc->m_videoParams.GetFrameHeight();
	const DWORD dwSize = nWidth*nHeight*nBPP;

	CString sFileName = GetMovieFileName();
	ASSERT(!sFileName.IsEmpty());

	CMovieWriter mvWriter;
	mvWriter.InitializeMovie(sFileName, nWidth, nHeight, fps,  m_pDoc->m_videoParams.GetQuality());

	//start anim
	if(m_pDoc->m_eAnimState == CTermPlanDoc::anim_none) {
		if(!m_pDoc->StartAnimation(FALSE)){
			THROW_STDEXCEPTION(_T("Start Animation Failed!"));
		}
	}
	//start read
	std::auto_ptr<IMovieReader> pMovieReader ( CreateMovieReader());
	if(!pMovieReader.get()){	THROW_STDEXCEPTION(_T("3DView is not Open!"));	}
	pMovieReader->BeginRead();

	m_progressMW.SetPos(0);
	m_progressMW.UpdateWindow();
	m_bIsCancel = FALSE;


	UINT nFrameIdx =0;
	long nCount = (endtime - starttime)/nStep + 1;
	for(long i=0; i<nCount; i++)
	{
		m_pDoc->m_animData.nLastAnimTime = starttime + i*nStep;
 		//pRender3DView->UpdateTrackMovieCamera();
		WriteOneFrame(nFrameIdx, pMovieReader.get() ,&mvWriter);	

		//pump messages
		BOOL bCanceled = PumpMessages();

		if(m_bIsCancel || bCanceled) {
			pMovieReader->EndRead();
			mvWriter.EndMovie();
			EndDialog(IDCANCEL);
			return;
		}

		m_sText.Format(_T("Writing file \'%s\'...\nWriting frame %d of %d."), sFileName, i + 1, nCount);
		UpdateData(FALSE);

		m_progressMW.SetPos(100*i/nCount);
		m_progressMW.UpdateWindow();
		nFrameIdx++;
	}

	m_progressMW.SetPos(100);
	m_progressMW.UpdateWindow();

	pMovieReader->EndRead();
	mvWriter.EndMovie();	
	EndDialog(IDOK);	
}


void CDlgMovieWriter::OnCancel() 
{
	m_bIsCancel = TRUE;
	//CDialog::OnCancel();
}

void CDlgMovieWriter::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}




void CDlgMovieWriter::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if(m_bFirstPaint) {
		m_bFirstPaint = FALSE;
		//AfxBeginThread(func_DoRecord,(LPVOID)this);
		DoRecord();	
	}	
	// Do not call CDialog::OnPaint() for painting messages
}

int CDlgMovieWriter::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_bFirstPaint = TRUE;
	
	return 0;
}

void CDlgMovieWriter::OnDestroy() 
{	
	CDialog::OnDestroy();
}

int CDlgMovieWriter::DoFakeModal()
{
	if(CDialog::Create(CDlgMovieWriter::IDD, m_pParent)) {
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

void CDlgMovieWriter::RecordPanorama()
{
	CMovie movie("temp");
	CWaitCursor wc;
	BuildMovieFromPanorama(m_pPanoramViewData, m_pDoc, &movie);
	wc.Restore();
	RecordMovie(&movie, m_pDoc->m_videoParams, m_pPanoramViewData->m_startTime, m_pPanoramViewData->m_endTime );	
}

void CDlgMovieWriter::RecordPanoramaRender()
{
	if (!m_pPanoramViewData)
		return;

	ALTAirport theAirport;
	int aptID = InputAirside::GetAirportID(m_pDoc->GetProjectID());
	theAirport.ReadAirport(aptID);

// 	ARCVector3 camLocation = m_pPanoramViewData->m_pos;
// 	camLocation[VZ] = m_pPanoramViewData->m_dAltitude - theAirport.getElevation();

	double dAngleFrom = 90 -  m_pPanoramViewData->m_dStartBearing - theAirport.GetMagnectVariation().GetRealVariation(); 
	double dAngleTo = 90 - m_pPanoramViewData->m_dEndBearing - theAirport.GetMagnectVariation().GetRealVariation();	

	int fps = m_pDoc->m_videoParams.GetFPS();
	long starttime = m_pPanoramViewData->GetStartTime();
	long endtime = m_pPanoramViewData->GetEndTime();
	long nStep = 10*m_pDoc->m_animData.nAnimSpeed/fps;
	double dAngleStep = m_pPanoramViewData->m_dTurnSpeed*m_pDoc->m_animData.nAnimSpeed/(fps*10);
	long nAngleBackCount = long((dAngleTo - dAngleFrom)/dAngleStep);
	if (nAngleBackCount < 0)
	{
		 // anti-clockwise rotation
		nAngleBackCount = - nAngleBackCount;
		dAngleStep = - dAngleStep;
	}



	const int nBPP = 3;
	const int nWidth = m_pDoc->m_videoParams.GetFrameWidth();
	const int nHeight = m_pDoc->m_videoParams.GetFrameHeight();
	const DWORD dwSize = nWidth*nHeight*nBPP;

	CString sFileName = GetMovieFileName();
	ASSERT(!sFileName.IsEmpty());

	CMovieWriter mvWriter;
	mvWriter.InitializeMovie(sFileName, nWidth, nHeight, fps,  m_pDoc->m_videoParams.GetQuality());

	//start anim
	if(m_pDoc->m_eAnimState == CTermPlanDoc::anim_none) {
		if(!m_pDoc->StartAnimation(FALSE)){
			THROW_STDEXCEPTION(_T("Start Animation Failed!"));
		}
	}

	//start read
	std::auto_ptr<IMovieReader> pMovieReader ( CreateMovieReader());
	if(!pMovieReader.get()){	THROW_STDEXCEPTION(_T("3DView is not Open!"));	}
	pMovieReader->BeginRead();


	m_progressMW.SetPos(0);
	m_progressMW.UpdateWindow();
	m_bIsCancel = FALSE;

	UINT nFrameIdx =0;
	long nCount = (endtime - starttime)/nStep + 1;
	for(long i=0; i<nCount; i++)
	{
		m_pDoc->m_animData.nLastAnimTime = starttime + i*nStep;
		//pRender3DView->UpdatePanoramaMovieCamera(dAngleFrom + (i%nAngleBackCount)*dAngleStep);
		WriteOneFrame(nFrameIdx, pMovieReader.get(),&mvWriter);	

		//pump messages
		BOOL bCanceled = PumpMessages();

		if(m_bIsCancel || bCanceled)
		{			
			pMovieReader->EndRead();
			mvWriter.EndMovie();
			EndDialog(IDCANCEL);
		}
		m_sText.Format(_T("Writing file \'%s\'...\nWriting frame %d of %d."), sFileName, i + 1, nCount);
		UpdateData(FALSE);

		m_progressMW.SetPos(100*i/nCount);
		m_progressMW.UpdateWindow();
		nFrameIdx++;

	}

	m_progressMW.SetPos(100);
	m_progressMW.UpdateWindow();


	pMovieReader->EndRead();
	mvWriter.EndMovie();	
	EndDialog(IDOK);	

	EndDialog(IDOK);

}

void CDlgMovieWriter::RecordPanoramaMovie()
{
	long nStartTime = m_pPanoramViewData->GetStartTime();
	long nEndTime = m_pPanoramViewData->GetEndTime();

	//movie	
	int nAnimSpeed = 100;
	CVideoParams videoparams = m_pDoc->m_videoParams;
	UINT fps = videoparams.GetFPS();
	UINT nStep = 10*nAnimSpeed/fps;
	UINT nCount = (nEndTime-nStartTime)/nStep;	
	int nBPP = 3;
	int nWidth = videoparams.GetFrameWidth();
	int nHeight = videoparams.GetFrameHeight();
	DWORD dwSize = nWidth*nHeight*nBPP;
	std::vector<BYTE> mBytesBuffer;
	mBytesBuffer.resize(dwSize);

	CString sFileName = GetMovieFileName();
	ASSERT(!sFileName.IsEmpty());
	CMovieWriter mvWriter;
	mvWriter.InitializeMovie(sFileName, nWidth, nHeight, fps, videoparams.GetQuality());
	//start anim
	if(m_pDoc->m_eAnimState == CTermPlanDoc::anim_none) {
		if(!m_pDoc->StartAnimation(FALSE)){
			THROW_STDEXCEPTION(_T("Start Animation Failed!"));
		}
	}
	//start read
	std::auto_ptr<IMovieReader> pMovieReader ( CreateMovieReader());
	if(!pMovieReader.get()){	THROW_STDEXCEPTION(_T("3DView is not Open!"));	}

	m_bIsCancel = FALSE;
	m_progressMW.SetPos(0);
	m_progressMW.UpdateWindow();
	C3DCamera newCam = 	pMovieReader->BeginRead();

	ALTAirport theAirport;
	int aptID = InputAirside::GetAirportID(m_pDoc->GetProjectID());
	theAirport.ReadAirport(aptID);

	ARCVector3 camLocation = m_pPanoramViewData->m_pos;
	camLocation[VZ] = m_pPanoramViewData->m_dAltitude - theAirport.getElevation();
	double dAngleFrom = 90 -  m_pPanoramViewData->m_dStartBearing - theAirport.GetMagnectVariation().GetRealVariation(); 
	double dAngleTo = 90 - m_pPanoramViewData->m_dEndBearing - theAirport.GetMagnectVariation().GetRealVariation();
	double dAngleRange = dAngleFrom - dAngleTo;
	DistanceUnit dFarViewLen = 50000;//500 m

	UINT t=nStartTime;
	for(UINT nFrameIdx=0; nFrameIdx<nCount; nFrameIdx++)
	{
		m_pDoc->m_animData.nLastAnimTime = t;
		//update camera
		double dAngle = (t - nStartTime)* m_pPanoramViewData->m_dTurnSpeed/100;
		int nCircle = (int)(dAngle/dAngleRange);
		if(nCircle%2==0)
			dAngle = dAngle - nCircle*dAngleRange;
		else
			dAngle = dAngleRange - (dAngle - nCircle*dAngleRange);

		double rkeyAngle = ARCMath::DegreesToRadians(dAngle);
		double rInc = ARCMath::DegreesToRadians(m_pPanoramViewData->m_dInclination);
		ARCVector3 vLookDir = ARCVector3( cos(rInc) * cos(rkeyAngle), cos(rInc)*sin(rkeyAngle), sin(rInc) ) * dFarViewLen; 
		newCam.SetCamera(camLocation,vLookDir,ARCVector3(0.0,0.0,1.0) );
		pMovieReader->setCamera(newCam);

		//write frame to
		BYTE* pBuffer = &mBytesBuffer[0];
		pMovieReader->RenderScreenToMem(mvWriter.GetWidth(),mvWriter.GetHeight(),pBuffer);
		mvWriter.WriteFrame(nFrameIdx,pBuffer);

		//pump messages
		BOOL bCanceled = PumpMessages();

		if(m_bIsCancel || bCanceled) {
			pMovieReader->EndRead();
			mvWriter.EndMovie();
			EndDialog(IDCANCEL);
			return;
		}

		m_sText.Empty();
		m_sText.Format("Writing file \'%s\'...\nWriting frame %d of %d.",sFileName,nFrameIdx,nCount);
		UpdateData(FALSE);

		int pct = 100*nFrameIdx/nCount;
		m_progressMW.SetPos(pct);
		m_progressMW.UpdateWindow();
		t+=nStep;
	}

	m_progressMW.SetPos(100);
	m_progressMW.UpdateWindow();
	pMovieReader->EndRead();
	mvWriter.EndMovie();
	EndDialog(IDOK);
	return;
}

const DistanceUnit dFarViewLen = 50000; //500 m
void CDlgMovieWriter::BuildMovieFromPanorama( const PanoramaViewData* pPD, CTermPlanDoc* pDoc, CMovie* pMovies )
{
	C3DView* pView = pDoc->Get3DView();
	ASSERT(pView);
	ALTAirport theAirport;
	int aptID = InputAirside::GetAirportID(pDoc->GetProjectID());
	theAirport.ReadAirport(aptID);

	const PanoramaViewData& theData = *pPD;
	ARCVector3 camLocation = theData.m_pos;
	camLocation[VZ] = theData.m_dAltitude - theAirport.getElevation();

	double dAngleFrom = 90 -  theData.m_dStartBearing - theAirport.GetMagnectVariation().GetRealVariation(); 
	double dAngleTo = 90 - theData.m_dEndBearing - theAirport.GetMagnectVariation().GetRealVariation();	


	long tTimeperAngle =   long(100.0 / theData.m_dTurnSpeed); 

	double dRatStep = 1.0/abs(dAngleTo - dAngleFrom);
	double dRat = 0;
	for(long nTime = theData.GetStartTime(); nTime <= theData.GetEndTime(); nTime+= tTimeperAngle)
	{
		double dRatLow = dRat - (int)dRat;
		double rkeyAngle = ARCMath::DegreesToRadians(dAngleFrom *(1.0- dRatLow)+ dAngleTo*dRatLow);
		double rInc = ARCMath::DegreesToRadians(theData.m_dInclination);

		ARCVector3 vLookDir = ARCVector3( cos(rInc) * cos(rkeyAngle), cos(rInc)*sin(rkeyAngle), sin(rInc) ) * dFarViewLen; 
		C3DCamera cam( *(pView->GetCamera()) );
		cam.SetCamera(camLocation,vLookDir,ARCVector3(0.0,0.0,1.0) );
		pMovies->InsertKF(cam, nTime);

		dRat += dRatStep;
	}
}

CString CDlgMovieWriter::GetMovieFileName() const
{
	if (m_pDoc->m_videoParams.GetAutoFileName())
	{
		CString sFileName;
		CFileStatus fs;
		int i=0;
		do {
			sFileName.Format(_T("%s\\%s%.4d.wmv"), m_pDoc->m_ProjInfo.path, m_pDoc->m_ProjInfo.name, i);
			i++;
		} while(CFile::GetStatus(sFileName, fs));
		return sFileName;
	}
	return m_pDoc->m_videoParams.GetFileName();
}



IMovieReader* CDlgMovieWriter::CreateMovieReader()
{
	if(m_pDoc->m_animData.m_AnimationModels.IsOnBoardSel() || m_pDoc->m_animData.m_AnimationModels.IsLandsideSel())
	{
		CRender3DView* pRender3DView = m_pDoc->GetRender3DView();
		if (pRender3DView)
		{
			
			return new MovieNewRenderEngine(pRender3DView);
		}
	}
	else
	{
		C3DView* pView = m_pDoc->Get3DView();
		if (pView)
		{
			return new MovieOldRenderEngine(pView);			
		}
	}
	return NULL;
}

void CDlgMovieWriter::DoRecord()
{

	try
	{
		switch(m_nType) {
		case MOVIE:
			RecordMovie();
			break;
		case WALKTHROUGH:
			{
				if ((m_pDoc->m_animData.m_AnimationModels.IsOnBoardSel() || m_pDoc->m_animData.m_AnimationModels.IsLandsideSel()) && m_pDoc->GetRender3DView())
				{
					RecordWalkthroughRender();
				}
				else
				{
					RecordWalkthrough();
				}
			}
			break;
		case LIVE:
			RecordLive();
			break;
		case PANORAMA :
			{
				if ((m_pDoc->m_animData.m_AnimationModels.IsOnBoardSel() || m_pDoc->m_animData.m_AnimationModels.IsLandsideSel()) && m_pDoc->GetRender3DView())
				{
					RecordPanoramaRender();
				}
				else
				{
					//RecordPanorama();
					RecordPanoramaMovie();
				}
			}
			break;
		}
	}		
	catch (std::exception& e)
	{
		AfxMessageBox(e.what());
		EndDialog(IDCANCEL);
	}		
	
}
