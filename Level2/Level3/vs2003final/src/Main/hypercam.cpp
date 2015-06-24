// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "hypercam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IHyperCam properties

short IHyperCam::GetStartX()
{
	short result;
	GetProperty(0x1, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetStartX(short propVal)
{
	SetProperty(0x1, VT_I2, propVal);
}

short IHyperCam::GetStartY()
{
	short result;
	GetProperty(0x2, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetStartY(short propVal)
{
	SetProperty(0x2, VT_I2, propVal);
}

short IHyperCam::GetWidth()
{
	short result;
	GetProperty(0x3, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetWidth(short propVal)
{
	SetProperty(0x3, VT_I2, propVal);
}

short IHyperCam::GetHeight()
{
	short result;
	GetProperty(0x4, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetHeight(short propVal)
{
	SetProperty(0x4, VT_I2, propVal);
}

double IHyperCam::GetFrameRate()
{
	double result;
	GetProperty(0x5, VT_R8, (void*)&result);
	return result;
}

void IHyperCam::SetFrameRate(double propVal)
{
	SetProperty(0x5, VT_R8, propVal);
}

CString IHyperCam::GetFileName()
{
	CString result;
	GetProperty(0x6, VT_BSTR, (void*)&result);
	return result;
}

void IHyperCam::SetFileName(LPCTSTR propVal)
{
	SetProperty(0x6, VT_BSTR, propVal);
}

double IHyperCam::GetPlaybackRate()
{
	double result;
	GetProperty(0x7, VT_R8, (void*)&result);
	return result;
}

void IHyperCam::SetPlaybackRate(double propVal)
{
	SetProperty(0x7, VT_R8, propVal);
}

BOOL IHyperCam::GetIncrementFileName()
{
	BOOL result;
	GetProperty(0x8, VT_BOOL, (void*)&result);
	return result;
}

void IHyperCam::SetIncrementFileName(BOOL propVal)
{
	SetProperty(0x8, VT_BOOL, propVal);
}

BOOL IHyperCam::GetRecordSound()
{
	BOOL result;
	GetProperty(0x9, VT_BOOL, (void*)&result);
	return result;
}

void IHyperCam::SetRecordSound(BOOL propVal)
{
	SetProperty(0x9, VT_BOOL, propVal);
}

short IHyperCam::GetKeyFrames()
{
	short result;
	GetProperty(0xa, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetKeyFrames(short propVal)
{
	SetProperty(0xa, VT_I2, propVal);
}

short IHyperCam::GetCompQuality()
{
	short result;
	GetProperty(0xb, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetCompQuality(short propVal)
{
	SetProperty(0xb, VT_I2, propVal);
}

short IHyperCam::GetSoundSampSize()
{
	short result;
	GetProperty(0xc, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetSoundSampSize(short propVal)
{
	SetProperty(0xc, VT_I2, propVal);
}

long IHyperCam::GetSoundSampRate()
{
	long result;
	GetProperty(0xd, VT_I4, (void*)&result);
	return result;
}

void IHyperCam::SetSoundSampRate(long propVal)
{
	SetProperty(0xd, VT_I4, propVal);
}

BOOL IHyperCam::GetFlashRect()
{
	BOOL result;
	GetProperty(0xe, VT_BOOL, (void*)&result);
	return result;
}

void IHyperCam::SetFlashRect(BOOL propVal)
{
	SetProperty(0xe, VT_BOOL, propVal);
}

CString IHyperCam::GetCompressor()
{
	CString result;
	GetProperty(0xf, VT_BSTR, (void*)&result);
	return result;
}

void IHyperCam::SetCompressor(LPCTSTR propVal)
{
	SetProperty(0xf, VT_BSTR, propVal);
}

BOOL IHyperCam::GetRecordCursor()
{
	BOOL result;
	GetProperty(0x10, VT_BOOL, (void*)&result);
	return result;
}

void IHyperCam::SetRecordCursor(BOOL propVal)
{
	SetProperty(0x10, VT_BOOL, propVal);
}

BOOL IHyperCam::GetAddClickStarbursts()
{
	BOOL result;
	GetProperty(0x11, VT_BOOL, (void*)&result);
	return result;
}

void IHyperCam::SetAddClickStarbursts(BOOL propVal)
{
	SetProperty(0x11, VT_BOOL, propVal);
}

short IHyperCam::GetStarburstFrames()
{
	short result;
	GetProperty(0x12, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetStarburstFrames(short propVal)
{
	SetProperty(0x12, VT_I2, propVal);
}

short IHyperCam::GetStarburstSize()
{
	short result;
	GetProperty(0x13, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetStarburstSize(short propVal)
{
	SetProperty(0x13, VT_I2, propVal);
}

long IHyperCam::GetLeftClickStarColor()
{
	long result;
	GetProperty(0x14, VT_I4, (void*)&result);
	return result;
}

void IHyperCam::SetLeftClickStarColor(long propVal)
{
	SetProperty(0x14, VT_I4, propVal);
}

long IHyperCam::GetRightClickStarColor()
{
	long result;
	GetProperty(0x15, VT_I4, (void*)&result);
	return result;
}

void IHyperCam::SetRightClickStarColor(long propVal)
{
	SetProperty(0x15, VT_I4, propVal);
}

short IHyperCam::GetClickVolume()
{
	short result;
	GetProperty(0x16, VT_I2, (void*)&result);
	return result;
}

void IHyperCam::SetClickVolume(short propVal)
{
	SetProperty(0x16, VT_I2, propVal);
}

BOOL IHyperCam::GetFlashRectPaused()
{
	BOOL result;
	GetProperty(0x17, VT_BOOL, (void*)&result);
	return result;
}

void IHyperCam::SetFlashRectPaused(BOOL propVal)
{
	SetProperty(0x17, VT_BOOL, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// IHyperCam operations

short IHyperCam::StartRec()
{
	short result;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

short IHyperCam::StopRec()
{
	short result;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

void IHyperCam::ShowWindow()
{
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IHyperCam::HideWindow()
{
	InvokeHelper(0x1b, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

short IHyperCam::PauseRec()
{
	short result;
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

short IHyperCam::GetRecordState()
{
	short result;
	InvokeHelper(0x1d, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
	return result;
}

void IHyperCam::EnableHotKeys()
{
	InvokeHelper(0x1e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IHyperCam::DisableHotKeys()
{
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL IHyperCam::CaptureSingleFrame()
{
	BOOL result;
	InvokeHelper(0x20, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

CString IHyperCam::ListCompressor(short nPos)
{
	CString result;
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x21, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		nPos);
	return result;
}

BOOL IHyperCam::ConfigureCompressor(long hWnd)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x22, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		hWnd);
	return result;
}

void IHyperCam::DeleteTab(short nTabNo)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nTabNo);
}

void IHyperCam::ActivateTab(short nTabNo)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x24, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 nTabNo);
}

void IHyperCam::RestoreAllTabs()
{
	InvokeHelper(0x25, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL IHyperCam::CreateNote(LPCTSTR strText)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x26, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		strText);
	return result;
}

void IHyperCam::DeleteAllNotes()
{
	InvokeHelper(0x27, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IHyperCam::HideAllNotes()
{
	InvokeHelper(0x28, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IHyperCam::ShowAllNotes()
{
	InvokeHelper(0x29, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL IHyperCam::ReadNotes(LPCTSTR FileName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2a, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		FileName);
	return result;
}

BOOL IHyperCam::SaveNotes(LPCTSTR FileName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2b, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		FileName);
	return result;
}

long IHyperCam::GetNumNotes()
{
	long result;
	InvokeHelper(0x2c, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IHyperCam::GetNumNotesVisible()
{
	long result;
	InvokeHelper(0x2d, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

BOOL IHyperCam::ToggleNote(long NoteNum)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x2e, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		NoteNum);
	return result;
}

BOOL IHyperCam::IsNoteVisible(long NoteNum)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x2f, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		NoteNum);
	return result;
}

CString IHyperCam::GetNoteText(long NoteNum)
{
	CString result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x30, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		NoteNum);
	return result;
}

BOOL IHyperCam::SetNoteText(long NoteNum, LPCTSTR Text)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_BSTR;
	InvokeHelper(0x31, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		NoteNum, Text);
	return result;
}

long IHyperCam::GetNoteWindowHandle(long NoteNum)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x32, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		NoteNum);
	return result;
}

BOOL IHyperCam::SetNoteBackColor(long NoteNum, long Color)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x33, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		NoteNum, Color);
	return result;
}

BOOL IHyperCam::SetNoteTextColor(long NoteNum, long Color)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x34, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		NoteNum, Color);
	return result;
}

BOOL IHyperCam::SetNoteFrameColor(long NoteNum, long Color)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x35, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		NoteNum, Color);
	return result;
}

BOOL IHyperCam::SetNoteFrameWidth(long NoteNum, long Width)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x36, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		NoteNum, Width);
	return result;
}

BOOL IHyperCam::SetNoteFontHeight(long NoteNum, long FontHeight)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x37, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		NoteNum, FontHeight);
	return result;
}
