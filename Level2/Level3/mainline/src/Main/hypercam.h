// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IHyperCam wrapper class

class IHyperCam : public COleDispatchDriver
{
public:
	IHyperCam() {}		// Calls COleDispatchDriver default constructor
	IHyperCam(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IHyperCam(const IHyperCam& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	short GetStartX();
	void SetStartX(short);
	short GetStartY();
	void SetStartY(short);
	short GetWidth();
	void SetWidth(short);
	short GetHeight();
	void SetHeight(short);
	double GetFrameRate();
	void SetFrameRate(double);
	CString GetFileName();
	void SetFileName(LPCTSTR);
	double GetPlaybackRate();
	void SetPlaybackRate(double);
	BOOL GetIncrementFileName();
	void SetIncrementFileName(BOOL);
	BOOL GetRecordSound();
	void SetRecordSound(BOOL);
	short GetKeyFrames();
	void SetKeyFrames(short);
	short GetCompQuality();
	void SetCompQuality(short);
	short GetSoundSampSize();
	void SetSoundSampSize(short);
	long GetSoundSampRate();
	void SetSoundSampRate(long);
	BOOL GetFlashRect();
	void SetFlashRect(BOOL);
	CString GetCompressor();
	void SetCompressor(LPCTSTR);
	BOOL GetRecordCursor();
	void SetRecordCursor(BOOL);
	BOOL GetAddClickStarbursts();
	void SetAddClickStarbursts(BOOL);
	short GetStarburstFrames();
	void SetStarburstFrames(short);
	short GetStarburstSize();
	void SetStarburstSize(short);
	long GetLeftClickStarColor();
	void SetLeftClickStarColor(long);
	long GetRightClickStarColor();
	void SetRightClickStarColor(long);
	short GetClickVolume();
	void SetClickVolume(short);
	BOOL GetFlashRectPaused();
	void SetFlashRectPaused(BOOL);

// Operations
public:
	short StartRec();
	short StopRec();
	void ShowWindow();
	void HideWindow();
	short PauseRec();
	short GetRecordState();
	void EnableHotKeys();
	void DisableHotKeys();
	BOOL CaptureSingleFrame();
	CString ListCompressor(short nPos);
	BOOL ConfigureCompressor(long hWnd);
	void DeleteTab(short nTabNo);
	void ActivateTab(short nTabNo);
	void RestoreAllTabs();
	BOOL CreateNote(LPCTSTR strText);
	void DeleteAllNotes();
	void HideAllNotes();
	void ShowAllNotes();
	BOOL ReadNotes(LPCTSTR FileName);
	BOOL SaveNotes(LPCTSTR FileName);
	long GetNumNotes();
	long GetNumNotesVisible();
	BOOL ToggleNote(long NoteNum);
	BOOL IsNoteVisible(long NoteNum);
	CString GetNoteText(long NoteNum);
	BOOL SetNoteText(long NoteNum, LPCTSTR Text);
	long GetNoteWindowHandle(long NoteNum);
	BOOL SetNoteBackColor(long NoteNum, long Color);
	BOOL SetNoteTextColor(long NoteNum, long Color);
	BOOL SetNoteFrameColor(long NoteNum, long Color);
	BOOL SetNoteFrameWidth(long NoteNum, long Width);
	BOOL SetNoteFontHeight(long NoteNum, long FontHeight);
};
