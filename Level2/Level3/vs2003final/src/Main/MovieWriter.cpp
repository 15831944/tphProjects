// MovieWriter.cpp: implementation of the CMovieWriter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "MovieWriter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void ThrowError(HRESULT hr, CString sFile, int nLine){
	 if(FAILED( hr )){
		 CString s; 
		 s.Format("%s(%d)",sFile,nLine);
		 THROW_STDEXCEPTION( s + _com_error(hr).ErrorMessage()); 
	 }
}

#define THROW_HR_ERROR(hr)  ThrowError(hr,__FILE__,__LINE__);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMovieWriter::CMovieWriter()
{
	m_bIsWriting = FALSE;
	m_pWMWriter = NULL;
}

CMovieWriter::~CMovieWriter()
{
	if(m_pWMWriter) {
		if(m_bIsWriting) {
			m_pWMWriter->EndWriting();
			m_bIsWriting = FALSE;
		}
		m_pWMWriter->Release();
	}
}

#ifndef _NOWMVCORE
HRESULT CMovieWriter::CreateProfile( IWMProfile ** ppIWMProfile )
{
	HRESULT hr = S_OK;

	IWMProfileManager* pProfileManager = NULL;
	hr = WMCreateProfileManager( &pProfileManager );
	THROW_HR_ERROR(hr); //ASSERT(!FAILED( hr ));

	IWMProfile* pProfile = NULL;
	hr = pProfileManager->CreateEmptyProfile( WMT_VER_9_0, &pProfile );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	IWMCodecInfo3* pCodecInfo3;
	hr = pProfileManager->QueryInterface( IID_IWMCodecInfo3, (void**) &pCodecInfo3 );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	DWORD cCodecs = 0;
	hr = pCodecInfo3->GetCodecInfoCount(WMMEDIATYPE_Video, &cCodecs);
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	WCHAR* pwszCodecName = NULL;
    DWORD cchCodecName = 0;
	DWORD dwWM9CodecIdx = -1;
	for(DWORD dwCodecIndex = 0; dwCodecIndex < cCodecs; dwCodecIndex++)
    {
        // Get the codec name:
        // First, get the size of the name.
        hr = pCodecInfo3->GetCodecName(WMMEDIATYPE_Video, 
                                       dwCodecIndex, 
                                       NULL, 
                                       &cchCodecName);
		THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));
		pwszCodecName = new WCHAR[cchCodecName];
		ASSERT(pwszCodecName);

        // Retrieve the codec name.
        hr = pCodecInfo3->GetCodecName(WMMEDIATYPE_Video, 
                                       dwCodecIndex, 
                                       pwszCodecName, 
                                       &cchCodecName);
		THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));
		CString sCodecName(pwszCodecName);
		
		if(sCodecName.CompareNoCase(_T("Windows Media Video 9")) == 0) {
			// TRACE("***Codec %d: %s\n", dwCodecIndex, sCodecName);
			dwWM9CodecIdx = dwCodecIndex;
		}
		else {
			// TRACE("Codec %d: %s\n", dwCodecIndex, sCodecName);
		}

		delete [] pwszCodecName;
		pwszCodecName = NULL;
		cchCodecName = 0;

	}

	if(dwWM9CodecIdx==-1) {
		AfxMessageBox(_T("ERROR\nARCTerm could not load the required Windows Media 9 series codec.\n"
			"The codec is not present or incorrectly installed.\n"
			"Recording cannot continue."));
		return NS_E_INVALIDPROFILE;
	}

	ASSERT(dwWM9CodecIdx != -1);

	const BOOL bUseVBR = TRUE;
	const DWORD dwNumPases = 1;

	/*pCodecInfo3->SetCodecEnumerationSetting(WMMEDIATYPE_Video,
		dwWM9CodecIdx,
		g_wszVBREnabled,
		WMT_TYPE_BOOL,
		(const BYTE*) &bUseVBR,
		sizeof(BOOL));

	pCodecInfo3->SetCodecEnumerationSetting(WMMEDIATYPE_Video,
		dwWM9CodecIdx,
		g_wszNumPasses,
		WMT_TYPE_DWORD,
		(const BYTE*) &dwNumPases,
		sizeof(DWORD));*/

	IWMStreamConfig* pStreamConfig;
	pCodecInfo3->GetCodecFormat(WMMEDIATYPE_Video, dwWM9CodecIdx, 0, &pStreamConfig);

	pStreamConfig->SetBitrate( 100000 );
	pStreamConfig->SetBufferWindow( 3000 );

	IWMMediaProps* pMediaProps = NULL;
	hr = pStreamConfig->QueryInterface( IID_IWMMediaProps, (void **) &pMediaProps );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	DWORD dwMediaTypeLength = 0;
    hr = pMediaProps->GetMediaType( NULL, &dwMediaTypeLength );
    THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

    WM_MEDIA_TYPE* pMediaType = (WM_MEDIA_TYPE*) new BYTE[ dwMediaTypeLength ];
    ASSERT(pMediaType);

    hr = pMediaProps->GetMediaType( pMediaType, &dwMediaTypeLength );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	//ASSERT(WMMEDIATYPE_Video == pMediaType->majortype);
	//ASSERT(WMMEDIASUBTYPE_MSS2 == pMediaType->subtype);
	//ASSERT(WMFORMAT_VideoInfo == pMediaType->formattype);

	WMVIDEOINFOHEADER *pVIH;
	// Configure the WMVIDEOINFOHEADER structure of the media type
	pVIH = (WMVIDEOINFOHEADER*) pMediaType->pbFormat;
	pVIH->dwBitRate = 100000;

	pVIH->rcSource.right = m_dwWidth;
	pVIH->rcSource.bottom = m_dwHeight;
	pVIH->rcTarget.right = m_dwWidth;
	pVIH->rcTarget.bottom = m_dwHeight;
	pVIH->bmiHeader.biWidth = m_dwWidth;
	pVIH->bmiHeader.biHeight = m_dwHeight;

	pVIH->AvgTimePerFrame = (LONGLONG) m_dwNSPerFrame;

	m_dwBPP = pVIH->bmiHeader.biBitCount>>3; // divide by 8

	hr = pMediaProps->SetMediaType( pMediaType );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	IWMVideoMediaProps* pVideoMediaProps = NULL;
	hr = pStreamConfig->QueryInterface( IID_IWMVideoMediaProps, (void **) &pVideoMediaProps );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));
	pVideoMediaProps->SetQuality(m_dwQuality);
	pVideoMediaProps->SetMaxKeyFrameSpacing((LONGLONG) 80000000);

	IWMPropertyVault* pPropertyVault = NULL;
	hr = pStreamConfig->QueryInterface( IID_IWMPropertyVault, (void **) &pPropertyVault );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));
	BOOL bTrue = TRUE;
	DWORD dwZero = 0;
	hr = pPropertyVault->SetProperty(g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE*)&bTrue, sizeof(BOOL) );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));
	hr = pPropertyVault->SetProperty(g_wszVBRQuality, WMT_TYPE_DWORD, (BYTE*)&m_dwQuality, sizeof(DWORD) );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));
	hr = pPropertyVault->SetProperty(g_wszVBRBitrateMax, WMT_TYPE_DWORD, (BYTE*)&dwZero, sizeof(DWORD) );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));
	hr = pPropertyVault->SetProperty(g_wszVBRBufferWindowMax, WMT_TYPE_DWORD, (BYTE*)&dwZero, sizeof(DWORD) );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	hr = pStreamConfig->SetConnectionName(L"video");
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));
	hr = pStreamConfig->SetStreamNumber(1);
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	hr = pProfile->AddStream(pStreamConfig);
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	/*
	WCHAR* pwszToSave;
	DWORD dwToSaveLength;
	hr = pProfileManager->SaveProfile(pProfile, NULL, &dwToSaveLength);
	ASSERT(!FAILED( hr ));
	pwszToSave = new WCHAR[dwToSaveLength];
	ASSERT(pwszToSave);
	hr = pProfileManager->SaveProfile(pProfile, pwszToSave, &dwToSaveLength);
	ASSERT(!FAILED( hr ));

	FILE* f = fopen("test.prx", "w");
	fprintf(f, "%S\n", pwszToSave);
	fclose(f);
	
	delete [] pwszToSave;
	*/

	if( NULL != pProfileManager ) {
        pProfileManager->Release( );
        pProfileManager = NULL;
	}

	*ppIWMProfile = pProfile;
	return hr;
}
#endif

CString ShowErrorMessage(LONG hr)
{
	TCHAR szBuf[80] = {0}; 
	LPVOID lpMsgBuf = NULL;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );
	return LPTSTR(lpMsgBuf);
	
}

HRESULT CMovieWriter::InitializeMovie(LPCSTR szFileName, int nWidth, int nHeight, int fps, int quality)
{
	HRESULT hr=S_OK;
#ifndef _NOWMVCORE
	m_dwHeight = nHeight;
	m_dwWidth = nWidth;
	m_dwFPS = fps;
	m_dwNSPerFrame = 10000000 / m_dwFPS;
	m_dwQuality = quality;

	ASSERT(m_dwWidth > 0 && (m_dwWidth%4) == 0);
	ASSERT(m_dwHeight > 0 && (m_dwHeight%4) == 0);
	
	IWMProfile* pProfile;
	hr = CreateProfile( &pProfile );
	THROW_HR_ERROR(hr);//
	/*if(hr==NS_E_INVALIDPROFILE){ 
		THROW_STDEXCEPTION(_T("INVALID WMV PROFILE!")); 
	}*/

	hr = WMCreateWriter( NULL, &m_pWMWriter );
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));if(FAILED(hr)){ THROW_STDEXCEPTION(ShowErrorMessage(hr)); }

	hr = m_pWMWriter->SetProfile(pProfile);
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));	if(FAILED(hr)){ THROW_STDEXCEPTION(ShowErrorMessage(hr)); }

	int count = strlen(szFileName);
	WCHAR* pwszFileName = new WCHAR[count+1];
	ASSERT(pwszFileName);
	mbstowcs(pwszFileName, szFileName, count);
	pwszFileName[count] = L'\0';

	hr = m_pWMWriter->SetOutputFilename(pwszFileName);
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));	if(FAILED(hr)){ THROW_STDEXCEPTION( ShowErrorMessage(hr)); }

	delete [] pwszFileName;
	pwszFileName = NULL;

	hr = m_pWMWriter->BeginWriting();
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));	if(FAILED(hr)){ THROW_STDEXCEPTION(_T("Write WMV File Failed!")); }

	m_bIsWriting = TRUE;
#endif
	return hr;
}

void CMovieWriter::WriteFrame(UINT nFrameIdx, const BYTE* pBits)
{
	HRESULT hr = S_OK;
#ifndef _NOWMVCORE
	DWORD dwSize = m_dwBPP*m_dwHeight*m_dwWidth;
	INSSBuffer* pINSBuffer = NULL;
	hr = m_pWMWriter->AllocateSample(dwSize, &pINSBuffer);
	THROW_HR_ERROR(hr);//if(FAILED( hr )){ THROW_STDEXCEPTION(_com_error(hr).ErrorMessage()); }

	//copy from input data to INSBuffer
	BYTE* pByteBuffer = NULL;
	hr = pINSBuffer->GetBuffer(&pByteBuffer);
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	memcpy(pByteBuffer, pBits, dwSize);	
	

	pINSBuffer->SetLength(dwSize);

	QWORD cnsSampleTime = m_dwNSPerFrame * nFrameIdx;
	hr = m_pWMWriter->WriteSample(0, cnsSampleTime, NULL, pINSBuffer);
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));

	// TRACE("Recorded frame %d\n", nFrameIdx);
	
	pINSBuffer->Release();
#endif
}

void CMovieWriter::EndMovie()
{
#ifndef _NOWMVCORE
	ASSERT(m_pWMWriter);
	HRESULT hr = m_pWMWriter->EndWriting();
    THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));if(FAILED(hr)){ THROW_STDEXCEPTION(_T("End WMV Writing Failed!") ); }	

	m_bIsWriting = FALSE;
#endif
}

void CMovieWriter::BegineFrameWrite( UINT nFrameIdx ,FrameInfo& info)
{
	info.hr = S_OK;

#ifndef _NOWMVCORE
	info.bufferSize = m_dwBPP*m_dwHeight*m_dwWidth;
	info.pINSBuffer = NULL;
	info.nFrameIdx = nFrameIdx;
	info.pByteBuffer = NULL;

	info.hr = m_pWMWriter->AllocateSample(info.bufferSize, &info.pINSBuffer);
	//ASSERT(!FAILED(info.hr ));
	THROW_HR_ERROR(info.hr);//if(FAILED(info.hr)){ THROW_STDEXCEPTION(_T("Alloate Movie Write Sample Failed!"));	}

	//copy from input data to INSBuffer
	//BYTE* pByteBuffer = NULL;
	info.hr = info.pINSBuffer->GetBuffer(&info.pByteBuffer);
	//ASSERT(!FAILED( info.hr ));
	THROW_HR_ERROR(info.hr);//if(FAILED(info.hr)){ THROW_STDEXCEPTION(_T("Allocate  Movie Buffer Failed!"));	}
	
#endif
	
}



void CMovieWriter::EndFrameWrite( const FrameInfo& info )
{
	if(info.hr!=S_OK)
		return;

	HRESULT hr = S_OK;
	info.pINSBuffer->SetLength(info.bufferSize);
	QWORD cnsSampleTime = (QWORD)m_dwNSPerFrame * info.nFrameIdx;
	hr = m_pWMWriter->WriteSample(0, cnsSampleTime, NULL, info.pINSBuffer);
	
	THROW_HR_ERROR(hr);//ASSERT(!FAILED( hr ));
	//TRACE("Recorded frame %d\n", info.nFrameIdx);	if(FAILED(hr)){ THROW_STDEXCEPTION(_T("Write WMV Frame Failed!") ); }

	info.pINSBuffer->Release();
}
