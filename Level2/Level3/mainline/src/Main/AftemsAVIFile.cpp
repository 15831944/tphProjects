// AftemsAVIFile.cpp: implementation of the CAftemsAVIFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AftemsAVIFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAftemsAVIFile::CAftemsAVIFile()
{
	m_nFrameCounter = 0;
	m_bBusy = false;
}

CAftemsAVIFile::~CAftemsAVIFile()
{
	//just to be safe
	Close();
}

void CAftemsAVIFile::Close()
{
	//
	// Now close the file
	//
	if (m_ps)
	{
		AVIStreamClose(m_ps);
		m_ps = NULL;
	}
	if (m_psCompressed)
	{
		AVIStreamClose(m_psCompressed);
		m_psCompressed = NULL;
	}
	if (m_psText)
	{
		AVIStreamClose(m_psText);
		m_psText = NULL;
	}
	if (m_pfile)
	{
		AVIFileClose(m_pfile);
		m_pfile = NULL;
	}

	AVIFileExit();
}

CString CAftemsAVIFile::GetAviFileNameToOpen(CString suggestedFileName)
{
	CFileDialog fileDlg(FALSE, _T("avi"), _T("*.avi"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("AVI Files (*.avi)|*.avi|All Files (*.*)|*.*||"), NULL);
	if(fileDlg.DoModal() == IDOK) 
	{
		return fileDlg.GetPathName();
	}
	else
	{
		return suggestedFileName;
	}
}

/*
** MakeDib(hbitmap)
**
** Take the given bitmap and transform it into a DIB with parameters:
**
** BitsPerPixel:    8
** Colors:          palette
**
*/
LPBITMAPINFOHEADER CAftemsAVIFile::MakeDib( HBITMAP hbitmap, UINT bits )
{
	HDC                 hdc ;
	BITMAP              bitmap ;
	UINT                wLineLen ;
	DWORD               dwSize ;
	DWORD               wColSize ;
	LPBITMAPINFOHEADER  lpbi ;
	LPBYTE              lpBits ;
	
	GetObject(hbitmap,sizeof(BITMAP),&bitmap) ;

	//
	// DWORD align the width of the DIB
	// Figure out the size of the colour table
	// Calculate the size of the DIB
	//
	wLineLen = (bitmap.bmWidth*bits+31)/32 * 4;
	wColSize = sizeof(RGBQUAD)*((bits <= 8) ? 1<<bits : 0);
	dwSize = sizeof(BITMAPINFOHEADER) + wColSize +
		(DWORD)(UINT)wLineLen*(DWORD)(UINT)bitmap.bmHeight;

	//
	// Allocate room for a DIB and set the LPBI fields
	//
	lpbi = (LPBITMAPINFOHEADER) new char[dwSize];
	if (!lpbi)
		return lpbi ;

	lpbi->biSize = sizeof(BITMAPINFOHEADER) ;
	lpbi->biWidth = bitmap.bmWidth ;
	lpbi->biHeight = bitmap.bmHeight ;
	lpbi->biPlanes = 1 ;
	lpbi->biBitCount = (WORD) bits ;
	lpbi->biCompression = BI_RGB ;
	lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize ;
	lpbi->biXPelsPerMeter = 0 ;
	lpbi->biYPelsPerMeter = 0 ;
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
	lpbi->biClrImportant = 0 ;

	//
	// Get the bits from the bitmap and stuff them after the LPBI
	//
	lpBits = (LPBYTE)(lpbi+1)+wColSize ;

	hdc = CreateCompatibleDC(NULL) ;

	GetDIBits(hdc,hbitmap,0,bitmap.bmHeight,lpBits,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	// Fix this if GetDIBits messed it up....
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
	DeleteDC(hdc) ;
	return lpbi;
}

bool CAftemsAVIFile::Init(CString stTargetFileName, double dbFrameRate, CSize bmpSize)
{
	m_dbFrameRate = dbFrameRate;

	m_pfile = NULL;
	m_ps  = m_psCompressed  = m_psText = NULL;
	
	/* first let's make sure we are running on 1.1 */
	WORD wVer = HIWORD(VideoForWindowsVersion());
	if (wVer < 0x010a)
	{
		 /* oops, we are too old, blow out of here */
		AfxMessageBox("Your windows is too old to make movies");
		return false;
	}
	
	AVIFileInit();

	m_hr = AVIFileOpen(&m_pfile,		    // returned file pointer
		       stTargetFileName,		            // file name
		       OF_WRITE | OF_CREATE,	    // mode to open file with
		       NULL);			    // use handler determined
						    // from file extension....
	if (m_hr != AVIERR_OK)
	{
		Close();
		return false;
	}

	// Fill in the header for the video stream....
	// The video stream will run in nFrameRate of a second....
	memset(&m_strhdr, 0, sizeof(m_strhdr));
	m_strhdr.fccType                = streamtypeVIDEO;// stream type
	m_strhdr.fccHandler             = 0;
	m_strhdr.dwScale                = 1;
	m_strhdr.dwRate                 = 15;		    // 15 fps
	m_strhdr.dwSuggestedBufferSize  = bmpSize.cx*bmpSize.cy;
	SetRect(&m_strhdr.rcFrame, 0, 0,		    // rectangle for stream
	    bmpSize.cx,bmpSize.cy);

	// And create the stream;
	m_hr = AVIFileCreateStream(m_pfile,		    // file pointer
			         &m_ps,		    // returned stream pointer
			         &m_strhdr);	    // stream header
	if (m_hr != AVIERR_OK) 
	{
		Close();
		return false;
	}

	/*
	m_opts.fccType = streamtypeVIDEO;
	m_opts.dwFlags = AVICOMPRESSF_VALID |AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
	m_opts.fccHandler = 0;
	m_opts.dwQuality = (DWORD)ICQUALITY_DEFAULT;
	m_opts.dwKeyFrameEvery = (DWORD)-1; // Default
	m_opts.dwBytesPerSecond = 0;
	m_opts.dwInterleaveEvery = 1;
	*/
	memset(&m_opts, 0, sizeof(m_opts));
	AVICOMPRESSOPTIONS FAR * aopts[1] = {&m_opts};
	if (!AVISaveOptions(NULL, 0, 1, &m_ps, (LPAVICOMPRESSOPTIONS FAR *) &aopts)) {
		Close();
		AfxMessageBox("error setting options");
		return false;
	}

	/*
//#if 0
	AVICOMPRESSOPTIONS* pOption = &m_opts;
	if (!AVISaveOptions(NULL, 0, 1, &m_ps, &pOption))
	{
		Close();
		return false;
	}
//#endif
*/

	m_hr = AVIMakeCompressedStream(&m_psCompressed, m_ps, &m_opts, NULL);
	if (m_hr != AVIERR_OK) 
	{
		Close();
		AfxMessageBox("error creating compressed stream");
		return false;
	}
	m_bBusy = false;
	return true;
}

//do nothing if size is different
bool CAftemsAVIFile::AddBmpOfSameSize(CBitmap& aMap)
{
	if (m_bBusy)
	{
		return true;
	}

	if (m_hr != AVIERR_OK)
	{
		return false;
	}

	m_bBusy = true;
	m_nFrameCounter++;

	LPBITMAPINFOHEADER alpb = MakeDib(aMap, 8);
	
	if (m_nFrameCounter == 1 && alpb) //first time
	{
		m_hr = AVIStreamSetFormat(m_psCompressed, 0,
					   alpb,	    // stream format
					   alpb->biSize +   // format size
					   alpb->biClrUsed * sizeof(RGBQUAD));
		if (m_hr != AVIERR_OK) 
		{
			Close();
			AfxMessageBox("error setting stream format");
			m_bBusy = false;
			delete alpb;
			return false;
		}
	}
	

	if (alpb->biWidth != abs(m_strhdr.rcFrame.right - m_strhdr.rcFrame.left) ||	
		alpb->biHeight != abs(m_strhdr.rcFrame.top - m_strhdr.rcFrame.bottom) )
	{
		AfxMessageBox("error !!");
		m_bBusy = false;
		delete alpb;
		return false;
	}

	//
	// Now write out each video frame
	//
                
	int start = AVIStreamStart(m_psCompressed);
	m_hr = AVIStreamWrite(m_psCompressed,	// stream pointer
		start+m_nFrameCounter-1,			// time of this frame
		1,									// number to write
		(LPBYTE) alpb	+					// pointer to data
			alpb->biSize +
			alpb->biClrUsed * sizeof(RGBQUAD),
		alpb->biSizeImage,					// size of this frame
		AVIIF_KEYFRAME,						// flags....
		NULL,
		NULL);
	if (m_hr != AVIERR_OK) 
	{
		Close();
		AfxMessageBox("error writing to AVI stream");
		m_bBusy = false;
		delete alpb;
		return false;
	}

	delete alpb;
	MessageBeep(MB_OK);
	m_bBusy = false;
	return true;
}

/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (C) 1992 - 1997 Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/
/****************************************************************************
 *
 *  WRITEAVI.C
 *
 *  Creates the file OUTPUT.AVI, an AVI file consisting of a rotating clock
 *  face.  This program demonstrates using the functions in AVIFILE.DLL
 *  to make writing AVI files simple.
 *
 *  This is a stripped-down example; a real application would have a user
 *  interface and check for errors.
 *
 ***************************************************************************/

#if 0
#include "writeavi.h"
//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
// Our movie is 160x120 and 15 frames long
//
#define BITMAP_X	160
#define BITMAP_Y	120
#define N_FRAMES	15

#define TEXT_HEIGHT	20

#define AVIIF_KEYFRAME	0x00000010L // this frame is a key frame.

#define BUFSIZE 260

#define LPLPBI	LPBITMAPINFOHEADER *

//---------------------------------------------------------------------------
// Function declarations
//---------------------------------------------------------------------------
static void FreeFrames(LPLPBI) ;
static void MakeFrames(LPLPBI, UINT, UINT, UINT) ;
static HANDLE MakeDib(HBITMAP, UINT);

//---------------------------------------------------------------------------
// A quick lookup table for Sin and Cos values
//---------------------------------------------------------------------------
//
static int aSin[N_FRAMES] = {
	0,    40,    74,    95,    99,
	86,    58,    20,    -20,    -58,
	-86,    -99,    -95,    -74,    -40,
	} ;

static int aCos[N_FRAMES] = {
	100,    91,    66,    30,    -10,
	-49,    -80,    -97,    -97,    -80,
	-50,    -10,    30,    66,    91,
	} ;

//----------------------------------------------------------------------------
//
// We don't have a window, we just pop up a dialog
// box, write the file, and quit
//
int PASCAL WinMain(
HINSTANCE hInstance,
HINSTANCE hPrevInstance,
LPSTR szCmdLine,
int sw)
{
	LPBITMAPINFOHEADER alpbi[N_FRAMES];
	int i;
	AVISTREAMINFO m_strhdr;
	PAVIFILE m_pfile = NULL;
	PAVISTREAM m_ps = NULL, m_psCompressed = NULL, m_psText = NULL;
	char szText[BUFSIZE];
	int iLen;
	AVICOMPRESSOPTIONS m_opts;
	AVICOMPRESSOPTIONS FAR * aopts[1] = {&m_opts};
	HRESULT m_hr;
	DWORD dwTextFormat;
	WORD wVer;
	char szTitle[BUFSIZE];
	char szMessage[BUFSIZE];

	alpbi[0] = NULL;

	LoadString(hInstance, IDS_APPNAME, szTitle, BUFSIZE );
	LoadString(hInstance, IDS_INTRO, szMessage, BUFSIZE );

	if (MessageBox(NULL, szMessage, szTitle, MB_OKCANCEL) == IDCANCEL)
		return 0;

	//
	// Set up the bitmaps for the file in an array
	//
	MakeFrames(alpbi, 8, BITMAP_X, BITMAP_Y);

	AVIFileInit();

	//
	// Open the movie file for writing....
	//
	LoadString(hInstance, IDS_FILENAME, szTitle, BUFSIZE );
	
	m_hr = AVIFileOpen(&m_pfile,		    // returned file pointer
		       szTitle,		            // file name
		       OF_WRITE | OF_CREATE,	    // mode to open file with
		       NULL);			    // use handler determined
						    // from file extension....
	if (m_hr != AVIERR_OK)
		goto error;

	// Fill in the header for the video stream....

	// The video stream will run in 15ths of a second....

	memset(&m_strhdr, 0, sizeof(m_strhdr));
	m_strhdr.fccType                = streamtypeVIDEO;// stream type
	m_strhdr.fccHandler             = 0;
	m_strhdr.dwScale                = 1;
	m_strhdr.dwRate                 = 15;		    // 15 fps
	m_strhdr.dwSuggestedBufferSize  = alpbi[0]->biSizeImage;
	SetRect(&m_strhdr.rcFrame, 0, 0,		    // rectangle for stream
	    (int) alpbi[0]->biWidth,
	    (int) alpbi[0]->biHeight);

	// And create the stream;
	m_hr = AVIFileCreateStream(m_pfile,		    // file pointer
			         &m_ps,		    // returned stream pointer
			         &m_strhdr);	    // stream header
	if (m_hr != AVIERR_OK) {
		goto error;
	}

	memset(&m_opts, 0, sizeof(m_opts));

	if (!AVISaveOptions(NULL, 0, 1, &m_ps, (LPAVICOMPRESSOPTIONS FAR *) &aopts))
	    goto error;

	m_hr = AVIMakeCompressedStream(&m_psCompressed, m_ps, &m_opts, NULL);
	if (m_hr != AVIERR_OK) {
		goto error;
	}

	m_hr = AVIStreamSetFormat(m_psCompressed, 0,
			       alpbi[0],	    // stream format
			       alpbi[0]->biSize +   // format size
			       alpbi[0]->biClrUsed * sizeof(RGBQUAD));
	if (m_hr != AVIERR_OK) {
	goto error;
	}

	// Fill in the stream header for the text stream....

	// The text stream is in 60ths of a second....

	memset(&m_strhdr, 0, sizeof(m_strhdr));
	m_strhdr.fccType                = streamtypeTEXT;
	m_strhdr.fccHandler             = mmioFOURCC('D', 'R', 'A', 'W');
	m_strhdr.dwScale                = 1;
	m_strhdr.dwRate                 = 60;
	m_strhdr.dwSuggestedBufferSize  = sizeof(szText);
	SetRect(&m_strhdr.rcFrame, 0, (int) alpbi[0]->biHeight,
	    (int) alpbi[0]->biWidth, (int) alpbi[0]->biHeight + TEXT_HEIGHT);

	// ....and create the stream.
	m_hr = AVIFileCreateStream(m_pfile, &m_psText, &m_strhdr);
	if (m_hr != AVIERR_OK) {
		goto error;
	}

	dwTextFormat = sizeof(dwTextFormat);
	m_hr = AVIStreamSetFormat(m_psText, 0, &dwTextFormat, sizeof(dwTextFormat));
	if (m_hr != AVIERR_OK) {
		goto error;
	}

	//
	// Now write out each video frame, along with a text label.
	// The video frames are 2/3 of a second apart, which is 10
	// in the video time scale and 40 in the text stream's time scale.
	//
	for (i = 0; i < N_FRAMES; i++) {
		m_hr = AVIStreamWrite(m_psCompressed,	// stream pointer
			i * 10,				// time of this frame
			1,				// number to write
			(LPBYTE) alpbi[i] +		// pointer to data
				alpbi[i]->biSize +
				alpbi[i]->biClrUsed * sizeof(RGBQUAD),
				alpbi[i]->biSizeImage,	// size of this frame
			AVIIF_KEYFRAME,			 // flags....
			NULL,
			NULL);
		if (m_hr != AVIERR_OK)
			break;
	
		// Make some text to put in the file ...
		LoadString(hInstance, IDS_TEXTFORMAT, szMessage, BUFSIZE );
		
		iLen = wsprintf(szText, szMessage, (int)(i + 1));

		// ... and write it as well.
		m_hr = AVIStreamWrite(m_psText,
				i * 40,
				1,
				szText,
				iLen + 1,
				AVIIF_KEYFRAME,
				NULL,
				NULL);
		if (m_hr != AVIERR_OK)
			break;
	}
	
error:
	//
	// Now close the file
	//
	if (m_ps)
		AVIStreamClose(m_ps);

	if (m_psCompressed)
		AVIStreamClose(m_psCompressed);

	if (m_psText)
		AVIStreamClose(m_psText);

	if (m_pfile)
		AVIFileClose(m_pfile);

	AVIFileExit();
	FreeFrames(alpbi);

	if (m_hr != NOERROR) {
		LoadString(hInstance, IDS_APPERR, szTitle, BUFSIZE );
		LoadString(hInstance, IDS_WRITEERR, szMessage, BUFSIZE );
		
		MessageBox(NULL, szMessage, szTitle, MB_OK);
	}
	return 0;
}




//
// Fill an array of LPBI's with the frames for this movie
//
static void MakeFrames(LPLPBI alpbi, UINT bits, UINT wXSize,UINT wYSize )
{
	HDC         hdc ;
	HDC         hdcMem ;
	HBITMAP     hbitmap,hbitmapOld ;
	HPEN        hpen3,hpen1,hpenwhite,hpenOld ;
	HFONT       hfont,hfontOld ;
	HBRUSH      hbrush,hbrushOld ;
	RECT        rc ;
	RECT        rcFrameNo ;
	int         wXCent,wYCent ;
	int         cxPixInch ;
	int         cyPixInch ;
	int         cxPixels ;
	int         cyPixels ;
	int         radius ;
	int         x0,y0,x1,y1 ;
	int         i,j ;
	char        szNumber[3] ;

	//
	// Make sure our resources are freed
	//
	FreeFrames(alpbi);

	//
	// Find the center of the movie
	//
	wXCent = wXSize/2 ;
	wYCent = wYSize/2 ;

	hdc = GetDC(NULL) ;
	hdcMem = CreateCompatibleDC(NULL) ;

	//
	// We need some gray and white brushes and pens, and a bitmap
	//
	hpen3 = CreatePen(PS_SOLID,3,RGB(128,128,128)) ;
	hpen1 = CreatePen(PS_SOLID,1,RGB(64,64,64));
	hpenwhite = CreatePen(PS_SOLID,1,RGB(255,255,255));
	hpenOld = SelectPen(hdcMem, hpen3);
	hbrush = CreateSolidBrush(RGB(192,192,192)) ;
	hbrushOld = SelectBrush(hdcMem,hbrush) ;
	hbitmap = CreateCompatibleBitmap(hdc,wXSize,wYSize) ;

	cxPixInch = GetDeviceCaps(hdc,LOGPIXELSX) ;
	cyPixInch = GetDeviceCaps(hdc,LOGPIXELSY) ;

	//
	// What radius of circle can we fit in this frame?  Make sure it's round
	// regardless of the aspect ratio
	//
	radius = ( wXSize < wYSize ) ? wXSize : (wYSize*cxPixInch)/cyPixInch ;
	radius = ( radius * 95 ) / 200 ;

	//
	// Make a Rectangle in the center where the number will go
	//
	/* x0 = radius / sqrt(2) */
	x0 = (radius*100)/141 ;
	y0 = (x0*cyPixInch)/cxPixInch ;
	x0 = (x0*9)/10 ;
	y0 = (y0*9)/10 ;
	SetRect( &rcFrameNo,wXCent-x0,wYCent-y0,wXCent+x0,wYCent+y0 ) ;

	//
	// Move the rectangle in a little and make a font big enough for it
	//
	x0 = (x0*9)/10 ;
	y0 = (y0*9)/10 ;

	hfont = CreateFont(
		y0*2,
		x0,
		0,
		0,
		FW_BOLD,
		0,
		0,
		0,
		ANSI_CHARSET,
		OUT_DEVICE_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_SWISS,
		NULL
	);

	hfontOld = SelectFont(hdcMem, hfont);

	//
	// Now walk through and make all the frames
	//
	for ( i=0; i<N_FRAMES; i++ ) 
	{
		hbitmapOld = SelectBitmap(hdcMem, hbitmap);
	
		//
		// Fill the whole frame with white
		//
		SetRect(&rc,0,0,wXSize,wYSize) ;
		FillRect(hdcMem,&rc,GetStockBrush(WHITE_BRUSH)) ;

		//
		// Draw the circle inside the previously calculated radius
		//
		cxPixels = radius ;
		cyPixels = (cxPixels*cyPixInch)/cxPixInch ;

		SelectPen(hdcMem,hpen3) ;
		Ellipse(hdcMem,wXCent-cxPixels,wYCent-cyPixels,wXCent+cxPixels,
			wYCent+cyPixels) ;

		SelectPen(hdcMem,hpen1) ;

		//
		// Draw the number in the previously calculated area
		//
		wsprintf(szNumber,"%02u",i+1) ;

		SetBkColor(hdcMem,RGB(192,192,192)) ;
		SetTextColor(hdcMem,RGB(255,255,255)) ;
		ExtTextOut(
			hdcMem,
			rcFrameNo.left,
			rcFrameNo.top+(rcFrameNo.bottom-rcFrameNo.top)/20,
			ETO_CLIPPED,
			&rcFrameNo,
			szNumber,
			2,
			NULL);

		//
		// Draw tic marks around the inside of the circle in equal divisions
		//
		for ( j=0; j<N_FRAMES; j++ ) {
			x0 = (radius*aSin[j])/100 ;
			y0 = (radius*aCos[j])/100 ;
			x1 = (((radius*aSin[j])/100)*11)/12 ;
			y1 = (((radius*aCos[j])/100)*11)/12 ;

			y0 = -(y0*cyPixInch)/cxPixInch ;
			y1 = -(y1*cyPixInch)/cxPixInch ;

			MoveToEx(hdcMem,wXCent+x0,wYCent+y0,NULL) ;
			LineTo(hdcMem,wXCent+x1,wYCent+y1) ;
		}

		//
		// Now draw the hand of the clock in the appropriate position
		//
		x1 = (((radius*aSin[i])/100)*5)/8 ;
		y1 = (((radius*aCos[i])/100)*5)/8 ;
		y1 = -(y1*cyPixInch)/cxPixInch ;

		MoveToEx(hdcMem,wXCent,wYCent,NULL) ;
		LineTo(hdcMem,wXCent+x1,wYCent+y1) ;

		SelectBitmap(hdcMem, hbitmapOld);
		//
		// Make this into a DIB and stuff it into the array
		//
		alpbi[i] = (LPBITMAPINFOHEADER)GlobalLock(MakeDib(hbitmap, bits));

		//
		// For an error, just duplicate the last frame if we can
		//
		if (alpbi[i] == NULL && i )
			alpbi[i] = alpbi[i-1] ;
	}

	//
	// Select all the old objects back and delete resources
	//
	SelectPen(hdcMem, hpenOld);
	SelectBrush(hdcMem,hbrushOld) ;
	SelectFont(hdcMem,hfontOld) ;
	DeletePen(hpen1) ;
	DeletePen(hpen3) ;
	DeletePen(hpenwhite) ;
	DeleteBrush(hbrush) ;
	DeleteBitmap(hbitmap) ;
	DeleteFont(hfont) ;
	DeleteObject(hdcMem) ;
	ReleaseDC(NULL,hdc) ;
}

//
// Walk through our array of LPBI's and free them
//
static void FreeFrames(LPLPBI alpbi)
{
	UINT        w ;

	if (!alpbi[0])
		return ;
	//
	// Don't free a frame if it's a duplicate of the previous one
	//
	for (w=0; w<N_FRAMES; w++)
		if (alpbi[w] && alpbi[w] != alpbi[w-1])
			GlobalFreePtr(alpbi[w]);
	for (w=0; w<N_FRAMES; w++)
		alpbi[w] = NULL;
}

#endif

