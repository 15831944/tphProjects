#include "StdAfx.h"
#include "FloorMapCtrl.h"
//#include "..\..\..\ARCCommon\Skia\include\core\SkBitmap.h"
//#include "..\..\..\ARCCommon\Skia\include/images/SkImageDecoder.h"
//#include "..\..\..\ARCCommon\Skia\include/images/SkImageRef_GlobalPool.h"



//#pragma comment(lib, "bench_timer.lib")
//#pragma comment(lib, "experimental.lib")
//#pragma comment(lib, "libjpeg.lib")
//#pragma comment(lib, "picture_renderer.lib")
//#pragma comment(lib, "picture_utils.lib")
//#pragma comment(lib, "skia_animator.lib")
//#pragma comment(lib, "skia_core.lib")
//#pragma comment(lib, "skia_effects.lib")
//#pragma comment(lib, "skia_gr.lib")
//#pragma comment(lib, "skia_images.lib")
//#pragma comment(lib, "skia_opts.lib")
//#pragma comment(lib, "skia_opts_ssse3.lib")
//#pragma comment(lib, "skia_pdf.lib")
//#pragma comment(lib, "skia_ports.lib")
//#pragma comment(lib, "skia_sfnt.lib")
//#pragma comment(lib, "skia_skgr.lib")
//#pragma comment(lib, "skia_utils.lib")
//#pragma comment(lib, "skia_views.lib")
//#pragma comment(lib, "skia_xml.lib")
//#pragma comment(lib, "skia_xps.lib")
//#pragma comment(lib, "views_animated.lib")
//
//#pragma comment(lib, "usp10.lib")
//#pragma comment(lib, "windowscodecs.lib")
//#pragma comment(lib, "opengl32.lib")

CFloorMapCtrl::CFloorMapCtrl(void)
{
	m_pPicture = NULL;

	/*SkStream* stream = new SkFILEStream(s);
	SkAutoUnref aur(stream);

	SkBitmap* bm;
	if (SkImageDecoder::DecodeStream(stream, &bm, SkBitmap::kNo_Config,
		SkImageDecoder::kDecodeBounds_Mode)) {
			SkPixelRef* pr = new SkImageRef_GlobalPool(stream, bm.config(), 1);
			bm.setPixelRef(pr)->unref();
	}
	return bm;
	bm->*/
}

//
//CFloorMapCtrl::~CFloorMapCtrl(void)
//{
//	delete m_pPicture;
//}
