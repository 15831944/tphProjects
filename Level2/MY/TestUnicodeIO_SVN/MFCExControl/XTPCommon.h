#pragma once



class MFCEXCONTROL_API CXTPClientRect : public CRect
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPClientRect object used to retrieve the
	//     client area for the specified window.
	// Parameters:
	//     hWnd - Handle to the window to retrieve the client area for.
	//     pWnd - Points to the window to retrieve the client area for.
	//-----------------------------------------------------------------------
	CXTPClientRect(HWND hWnd);
	CXTPClientRect(const CWnd* pWnd); //<combine CXTPClientRect::CXTPClientRect@HWND>
};

class MFCEXCONTROL_API CXTPWindowRect : public CRect
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPWindowRect object used to retrieve the
	//     screen size for the specified window.
	// Parameters:
	//     hWnd - Handle to the window to retrieve the screen size for.
	//     pWnd - Points to the window to retrieve the screen size for.
	//-----------------------------------------------------------------------
	CXTPWindowRect(HWND hWnd);
	CXTPWindowRect(const CWnd* pWnd); // <combine CXTPWindowRect::CXTPWindowRect@HWND>
};

//===========================================================================
// Summary:
//     CXTPBufferDC is a helper class used to create a memory device
//     context used to draw to an off-screen bitmap.  When destroyed, the
//     class selects the previous bitmap back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class MFCEXCONTROL_API CXTPBufferDC : public CDC
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBufferDC object used to create a memory
	//     device context used to draw to an off-screen bitmap.
	// Parameters:
	//     hDestDC - Handle to the destination device context the memory
	//               device is BitBlt to.
	//     rcPaint - Size of the area to paint.
	//     clrBack - Represents background color for fill, can be COLORREF or gradient values using CXTPPaintManagerColorGradient.
	//     bHorz   - Used when drawing gradient background, TRUE to draw gradient from left to right, otherwise drawn top to bottom.
	//-----------------------------------------------------------------------
	CXTPBufferDC(HDC hDestDC, const CRect& rcPaint);
	//CXTPBufferDC(HDC hDestDC, const CRect& rcPaint, const CXTPPaintManagerColorGradient& clrBack, const BOOL bHorz = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBufferDC object used to create a memory
	//     device context used to draw to an off-screen bitmap.
	// Parameters:
	//     paintDC - Handle to the destination device context the memory
	//-----------------------------------------------------------------------
	CXTPBufferDC(CPaintDC& paintDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPBufferDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBufferDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the valid flag to false
	//     so that the off screen device context will not be drawn.
	//-----------------------------------------------------------------------
	void Discard();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve a CDC pointer to the destination
	//     device context.
	//-----------------------------------------------------------------------
	CDC* GetDestDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to take a snapshot of the screen underneath
	//     the area where the off screen bitmap is to be drawn.
	//-----------------------------------------------------------------------
	void TakeSnapshot();

protected:

	HDC     m_hDestDC;    // Handle to the destination device context.
	CBitmap m_bitmap;     // Bitmap in memory device context
	CRect   m_rect;       // Size of the area to paint.
	HGDIOBJ m_hOldBitmap; // Handle to the previously selected bitmap.
};