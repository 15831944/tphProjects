/*
  Name:         CFIDSView.cpp (implementation)
  Copyright:    Aviation Research Corporation
  Description:  CFIDSView class implementation...
*/

// Pre-procs...
#include "stdafx.h"
#include "TermPlan.h"

#include "TermPlanDoc.h"
#include "results/fltentry.h"
#include "results/fltlog.h"
#include "../Common/AirportDatabase.h"
#include "../Common/AirlineManager.h"
#include "../Common/AirportsManager.h"
#include "CFIDSView.h"

// Random IDE generated mess...

    // Enable CObject-derived classe to be created dynamically at run time...
    IMPLEMENT_DYNCREATE(CFIDSView, CView)
    
    // Message map...
    BEGIN_MESSAGE_MAP(CFIDSView, CView)
    	//{{AFX_MSG_MAP(CFIDSView)
    	ON_WM_SETFOCUS()
    	ON_WM_KILLFOCUS()
    	ON_WM_CREATE()
    	ON_WM_ERASEBKGND()
    	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
    	ON_COMMAND(ID_VIEW_PRINT, OnViewPrint)
    	ON_COMMAND(ID_VIEW_EXPORT, OnViewExport)
        ON_NOTIFY (LVN_GETDISPINFO, CFIDSVIEW_DEPARTURES_IDC, OnGetListDisplayInfo)
        ON_NOTIFY (LVN_GETDISPINFO, CFIDSVIEW_ARRIVALS_IDC, OnGetListDisplayInfo)
    END_MESSAGE_MAP()
    
    // Compiled in debugging mode...
    #ifdef _DEBUG

        void CFIDSView::AssertValid() const
        {
        	CView::AssertValid();
        }
        
        void CFIDSView::Dump(CDumpContext& dc) const
        {
        	CView::Dump(dc);
        }
        
        CTermPlanDoc* CFIDSView::GetDocument()
        {
        	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
        	return (CTermPlanDoc*)m_pDocument;
        }

    #endif

// Constructor...
CFIDSView::CFIDSView()
{
    // Initialize variables to defaults...
    nUnknownImageIndex = 0;
}

// View creation event...
int CFIDSView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    // Variables...
    CTermPlanDoc       *pDocument           = NULL;
    CAirlinesManager   *pAirlinesManager    = NULL;
    CAIRLINELIST       *pAirlineList        = NULL;
    CString             szARCTermRoot;
    unsigned int        unIndex             = 0;
    CAirline           *pAirline            = NULL;
    unsigned short      usImagesFailed      = 0;
    CString             sErrorMessage;
    char                szBuffer[1024]      = {0};
    HBITMAP             hBitmap             = NULL;
    CBitmap             Bitmap;
    int                 nTemp               = 0;

    // Creation failed, abort...
    if(CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create airline image list...

        // How many images are there to load?

            // Get the document object...
            pDocument = GetDocument();

                // Failed...
                if(!pDocument)
                    return -1;

            // Airline manager...
            pAirlinesManager = pDocument->GetTerminal().m_pAirportDB->getAirlineMan();

                // Failed...
                if(!pAirlinesManager)
                    -1;

            // Get airline list...
	        pAirlineList = pAirlinesManager->GetAirlineList();

                // Failed...
                if(!pAirlineList)
                    -1;

                // Empty...
                if(pAirlineList->size() == 0)
                    MessageBox("Empty airline list.", "Flight Information Display",
                               MB_ICONERROR);
    
        // Create, and check for error...
        if(!AirlineImages.Create(CFIDSVIEW_LOGO_WIDTH, CFIDSVIEW_LOGO_HEIGHT, 
                                 ILC_COLOR8, 0, pAirlineList->size()))
        {
            // Alert...
            MessageBox("Unable to create airline image list.", "Error", MB_ICONERROR);

            // Abort...
            return -1;
        }

        // Format path to ARCTerm root...
            
            // Get path to executable...
          //::GetModuleFileName(::GetModuleHandle(NULL), szARCTermRoot, 
          //                    sizeof(szARCTermRoot));
          //  
          //  // Check for last slash...
          //  if(!strrchr(szARCTermRoot, '\\'))
          //      return -1;
          //  
          //  // Terminate at final directory...
          // *strrchr(szARCTermRoot, '\\') = '\x0';
		szARCTermRoot = PROJMANAGER->GetAppPath();

        // Load the unknown image logo...

            // Format path to image...
            sprintf(szBuffer, "%s\\Databases\\AirlineImages\\Airline_Unknown.bmp",
                    szARCTermRoot.GetBuffer());

            // Load image from disk...
            hBitmap = (HBITMAP) ::LoadImage(NULL, szBuffer, IMAGE_BITMAP, 0, 0, 
                                            LR_LOADFROMFILE);

                // Failed...
                if(!hBitmap)
                {
                    // Alert...
                    MessageBox(szBuffer, "File not found", MB_ICONERROR);

                    // Abort...
                    return -1;
                }

            // Resize...
            hBitmap = ResizeBitmap(hBitmap, CFIDSVIEW_LOGO_WIDTH, 
                                   CFIDSVIEW_LOGO_HEIGHT);

                if(!hBitmap)
                    MessageBox("Unknown image resize failed");
            
            // Attach to a CBitmap object for the image list...
            Bitmap.Attach(hBitmap);
                    
            // Add to image list, remember index, and check for error...
            if((nUnknownImageIndex = AirlineImages.Add(&Bitmap, (CBitmap *) NULL))
               == -1)
            {
                // Alert...
                MessageBox("Unable to add the unknown logo to the image list.", 
                           "Flight Information Display", MB_ICONERROR);

                // Abort...
                return -1;
            }
            
            // Done with bitmap...
            Bitmap.Detach();

        // Prepare error control...
        usImagesFailed = 0;
        sErrorMessage = "Unable to load the following airline logos:\n\n";

        // Load each image...
        for(unIndex = 0; unIndex < pAirlineList->size(); unIndex++)
        {
            // Fetch an airline...
            pAirline = (*pAirlineList)[unIndex];

            // Format path to image...
            sprintf(szBuffer, "%s\\Databases\\AirlineImages\\Airline_%s.bmp",
                    szARCTermRoot.GetBuffer(), pAirline->m_sAirlineIATACode);

            // Load image from disk...
            hBitmap = (HBITMAP) ::LoadImage(NULL, szBuffer, IMAGE_BITMAP, 0, 0, 
                                            LR_LOADFROMFILE);

                // Image loaded ok...
                if(hBitmap)
                {
                    // Resize...
                    hBitmap = ResizeBitmap(hBitmap, CFIDSVIEW_LOGO_WIDTH, 
                                           CFIDSVIEW_LOGO_HEIGHT);

                        // Failed...
                        if(!hBitmap)
                            MessageBox("Image resize failed");
                    
                    // Attach to a CBitmap object for the image list...
                    Bitmap.Attach(hBitmap);
                    
                    // Add to image list and check for error...
                    if((nTemp = AirlineImages.Add(&Bitmap, (CBitmap *) NULL)) == -1)
                    {
                        // Alert...
                        MessageBox(szBuffer, "Image list add error", MB_ICONERROR);

                        // Enough loading...
                        break;
                    }

                    // Cleanup...
                    Bitmap.Detach();

                    // Remember image list index...
                    pAirline->unImageIndex = nTemp;
                }

                // Image load failed...
                else
                {
                    // Remember...
                    usImagesFailed++;
                    pAirline->unImageIndex = -1;
                
                    // Too many failed attempts already...
                    if(usImagesFailed == 16)
                    {
                        // Let them know there are more...
                        sErrorMessage += "...\n";

                        // Try next one...
                        continue;
                    }
                
                    // Don't give them anymore details...
                    else if(usImagesFailed > 16)
                        continue;

                    // Give them more details on this one...
                    else
                    {
                        // Add to error message...
                        sErrorMessage += szBuffer;
                        sErrorMessage += "\n";

                        // Try next one...
                        continue;
                    }
                }
        }

        // Some images failed to load from disk, alert user...
        if(usImagesFailed)
            MessageBox(sErrorMessage, "Flight Information Display", MB_ICONERROR);

    // Create departures list control...

        // Create... (dimensions set at OnSize)
        Departures.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT,
                          CRect(0, 0, 0, 0), this, CFIDSVIEW_DEPARTURES_IDC);
        Departures.ShowWindow(SW_SHOW);

        // Populate headers...
        Departures.InsertColumn(CFIDSVIEW_DEPARTURES_ID_LOGO, "Logo", 
                                LVCFMT_LEFT, 85);
        Departures.InsertColumn(CFIDSVIEW_DEPARTURES_ID_AIRLINE, "Airline", 
                                LVCFMT_LEFT, 85);
        Departures.InsertColumn(CFIDSVIEW_DEPARTURES_ID_FLIGHT, "Flight", 
                                LVCFMT_LEFT, 60);
        Departures.InsertColumn(CFIDSVIEW_DEPARTURES_ID_DESTINATION, 
                                "Destination", LVCFMT_LEFT, 90);
        Departures.InsertColumn(CFIDSVIEW_DEPARTURES_ID_TIME, "Time", 
                                LVCFMT_LEFT, 60);
        Departures.InsertColumn(CFIDSVIEW_DEPARTURES_ID_STATUS, "Status", 
                                LVCFMT_LEFT, 70);

        // Set colours...
        Departures.SetTextColor(RGB(0xfc, 0xe8, 0x42));
        Departures.SetTextBkColor(RGB(0x00, 0x00, 0x00));
        Departures.SetBkColor(RGB(0x00, 0x00, 0x00));

        // Associate image list with departures list control...
        Departures.SetImageList(&AirlineImages, LVSIL_SMALL);

    // Create arrivals list control...

        // Create... (dimensions set at OnSize)
        Arrivals.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT,
                        CRect(0, 0, 0, 0), this, CFIDSVIEW_ARRIVALS_IDC);
        Arrivals.ShowWindow(SW_SHOW);

        // Populate headers...
        Arrivals.InsertColumn(CFIDSVIEW_ARRIVALS_ID_LOGO, "Logo", 
                              LVCFMT_LEFT, 85);
        Arrivals.InsertColumn(CFIDSVIEW_ARRIVALS_ID_AIRLINE, "Airline", 
                              LVCFMT_LEFT, 85);
        Arrivals.InsertColumn(CFIDSVIEW_ARRIVALS_ID_FLIGHT, "Flight", 
                              LVCFMT_LEFT, 60);
        Arrivals.InsertColumn(CFIDSVIEW_ARRIVALS_ID_ORIGIN, "Origin", 
                              LVCFMT_LEFT, 90);
        Arrivals.InsertColumn(CFIDSVIEW_ARRIVALS_ID_TIME, "Time", 
                              LVCFMT_LEFT, 60);
        Arrivals.InsertColumn(CFIDSVIEW_ARRIVALS_ID_STATUS, "Status", 
                              LVCFMT_LEFT, 70);

        // Set text foreground colour...
        Arrivals.SetTextColor(RGB(0x42, 0xb8, 0xfc));
        Arrivals.SetTextBkColor(RGB(0x00, 0x00, 0x00));
        Arrivals.SetBkColor(RGB(0x00, 0x00, 0x00));

        // Associate image list with arrivals list control...
        Arrivals.SetImageList(&AirlineImages, LVSIL_SMALL);

    // Update output...
    UpdateOutput();

    // Done ok...
	return 0;
}

// Repainting time...
void CFIDSView::OnDraw(CDC *pDC)
{
	// Variables...
    CRect       Rectangle;
    CBrush      Brush;

    // Get window dimensions...
    GetClientRect(&Rectangle);
    
    // Set background color...
    Brush.CreateSolidBrush(RGB(0x00, 0x00, 0x00));
    pDC->FillRect(&Rectangle, &Brush);

    // Set text foreground color...
    pDC->SetTextColor(RGB(0xff, 0xff, 0xff));
    pDC->SetBkColor(RGB(0x00, 0x00, 0x00));
 
    // Departures caption...
    pDC->TextOut(5, 3, "Departures");

    // Arrivals caption...
    pDC->TextOut(5, (Rectangle.bottom / 2) + 3, "Arrivals");
}



void CFIDSView::OnGetListDisplayInfo(NMHDR* pnmhdr, LRESULT* pResult)
{
    // Variables...
    LV_DISPINFO        *pDisplayInfo        = NULL;
    CTermPlanDoc       *pDocument           = NULL;
    CAirlinesManager   *pAirlinesManager    = NULL;
    CAIRLINELIST       *pAirlineList        = NULL;
    unsigned int        unIndex             = 0;
    CAirline           *pAirline            = NULL;
    //FlightLog          *pLog                = NULL;
    //FlightLogEntry      LogEntry;
    //FlightDescStruct    FlightDescription;
    char                szFltID[256]         = {0};
    char                szShortAirline[256] = {0};

    // Get display header...
    pDisplayInfo = (LV_DISPINFO *) pnmhdr;

    // Get airline list...

        // Get the document object...
        pDocument = GetDocument();

            // Failed...
            if(!pDocument)
                return;

            // Airline manager...
            pAirlinesManager = pDocument->GetTerminal().m_pAirportDB->getAirlineMan();

                // Failed...
                if(!pAirlinesManager)
                    return;

            // Get airline list...
	        pAirlineList = pAirlinesManager->GetAirlineList();

                // Failed...
                if(!pAirlineList)
                    return;

    // Get the flight number of the entry of interest...
        
        // From the departures list control...
        if(pDisplayInfo->hdr.idFrom == CFIDSVIEW_DEPARTURES_IDC)
        {
            // Fetch...
            Departures.GetItemText(pDisplayInfo->item.iItem, 
                                   CFIDSVIEW_DEPARTURES_ID_FLIGHT, szFltID, 
                                   sizeof(szFltID));
        }

        // From arrivals list control...
        else
        {
            // Fetch...
            Arrivals.GetItemText(pDisplayInfo->item.iItem, 
                                 CFIDSVIEW_ARRIVALS_ID_FLIGHT, szFltID, 
                                   sizeof(szFltID));
        }


    // Find the airline...
    
        // Get the flight log object...
		AirsideFlightLog& airsideFlightLog = pDocument->GetAirsideSimLogs().m_airsideFlightLog;
		AirsideFlightLogEntry LogEntry;
		AirsideFlightDescStruct FlightDescription;


        // Search all...
        for(unIndex = 0; unIndex < static_cast<unsigned int>(airsideFlightLog.getCount()); unIndex++)
        {
		    // Get log entry...
            airsideFlightLog.getItem(LogEntry, unIndex);

            // Get flight description...
            LogEntry.initStruct(FlightDescription);

            // Check flight number...
                
                // Departures list control...
                if(pDisplayInfo->hdr.idFrom == CFIDSVIEW_DEPARTURES_IDC)
                {
                    // Found the flight number...
					CFlightID id;
					id = szFltID;
                    if(FlightDescription._depID == id)
                    {
                        // Grab airline...
                        strcpy(szShortAirline, FlightDescription._airline);

                        // Done...
                        break;
                    }
                }

                // Arrivals list control...
                else
                {
                    // Found the flight number...
					CFlightID id;
					id = szFltID;
                    if(FlightDescription._arrID == id)
                    {
                        // Grab airline...
                        strcpy(szShortAirline, FlightDescription._airline);

                        // Done...
                        break;
                    }
                }
        }

    // Did not find airline...
    if(!strlen(szShortAirline))
    {
        // Use unknown logo...
        pDisplayInfo->item.iImage = nUnknownImageIndex;
        return;
    }

    // Now that we know the airline, find the airlines logo image index...
    for(unIndex = 0; unIndex < pAirlineList->size(); unIndex++)
    {
        // Fetch an airline...
        pAirline = (*pAirlineList)[unIndex];

        // Found airline...
        if(strcmp(pAirline->m_sAirlineIATACode, szShortAirline) == 0)
        {
            // Set image and use unknown logo if image was not found before...
            pDisplayInfo->item.iImage 
                = (pAirline->unImageIndex == -1) ? nUnknownImageIndex 
                                                 : pAirline->unImageIndex;

            // Done...
            return;
        }
    }
}

// Expand contracted airline name... (AA -> American Airlines)
bool CFIDSView::ExpandAirline(const char *pszContractedName, char *pszBuffer, 
                              unsigned short usBufferSize)
{
    // Variables...
    CTermPlanDoc       *pDocument           = NULL;
    CAirlinesManager   *pAirlinesManager    = NULL;
    CAIRLINELIST       *pAirlineList        = NULL;
    unsigned int        unIndex             = 0;
    CAirline           *pAirline            = NULL;

    // Get the document object...
    pDocument = GetDocument();

        // Failed...
        if(!pDocument)
            return false;

    // Airline manager...
    pAirlinesManager = pDocument->GetTerminal().m_pAirportDB->getAirlineMan();

        // Failed...
        if(!pAirlinesManager)
            return false;

    // Get airline list...
	pAirlineList = pAirlinesManager->GetAirlineList();

        // Failed...
        if(!pAirlineList)
            return false;

    // Scan our airline database list for requested contracted airline name...
	for(unIndex = 0; unIndex < pAirlineList->size(); unIndex++)
    {
		// Fetch an airline...
        pAirline = (*pAirlineList)[unIndex];

        // Match...
        if(_strcmpi(pAirline->m_sAirlineIATACode, pszContractedName) == 0)
        {
            // Save for caller...
            strncpy(pszBuffer, pAirline->m_sLongName, usBufferSize);

            // This is a weird name, use contracted form...
            if(strstr(pszBuffer, "CREATED BY SYSTEM"))
                strcpy(pszBuffer, pszContractedName);

            // Has a period at the end and does not have LTD in it, remove...
            if((pszBuffer[strlen(pszBuffer) - 1] == '.') && !strstr(pszBuffer, "LTD."))
                pszBuffer[strlen(pszBuffer) - 1] = '\x0';

            // Done...
            return true;
        }
	}

    // Not found, use contracted form...
    strcpy(pszBuffer, pszContractedName);

    // Done...
    return false;
}

// Expand contracted airport name... (YVR -> Vancouver Canada)
bool CFIDSView::ExpandAirport(const char *pszContractedName, char *pszBuffer, 
                              unsigned short usBufferSize)
{
    // Variables...
    CTermPlanDoc       *pDocument           = NULL;
    CAirportsManager   *pAirportsManager    = NULL;
    CAIRPORTLIST       *pAirportList        = NULL;
    unsigned int        unIndex             = 0;
    CAirport           *pAirport            = NULL;
    
    // Get the document object...
    pDocument = GetDocument();

        // Failed...
        if(!pDocument)
            return false;

    // Airports manager...
    pAirportsManager = pDocument->GetTerminal().m_pAirportDB->getAirportMan();

        // Failed...
        if(!pAirportsManager)
            return false;

    // Get airport list...
	pAirportList = pAirportsManager->GetAirportList();

        // Failed...
        if(!pAirportList)
            return false;

    // Scan our airline database list for requested contracted airline name...
	for(unIndex = 0; unIndex < pAirportList->size(); unIndex++)
    {
		// Fetch an airline...
        pAirport = (*pAirportList)[unIndex];

        // Match...
        if(_strcmpi(pAirport->m_sIATACode, pszContractedName) == 0)
        {
            // Save for caller...
            strncpy(pszBuffer, pAirport->m_sLongName, usBufferSize);

            // This is a weird name, use contracted form...
            if(strstr(pszBuffer, "CREATED BY SYSTEM"))
                strcpy(pszBuffer, pszContractedName);

            // Has a period at the end and a space, remove...
            if((pszBuffer[strlen(pszBuffer) - 2] == '.') && 
               (pszBuffer[strlen(pszBuffer) - 1] == ' '))
                pszBuffer[strlen(pszBuffer) - 2] = '\x0';

            // Done...
            return true;
        }
	}

    // Not found, use contracted form...
    strcpy(pszBuffer, pszContractedName);

    // Done...
    return false;
}

// Is arrivals flight currently listed in list control? (-1 not found)
int CFIDSView::IsArrivalsFlightListed(const CFlightID& sFlightNumber)
{
    // Variables...
    unsigned int    unIndex         = 0;
    char            szBuffer[1024]  = {0};

    // Scan through our list control to see if flight is listed...
    for(unIndex = 0; unIndex < static_cast<unsigned int>(Arrivals.GetItemCount()); unIndex++)
    {
        // Get this entry's flight number...
        Arrivals.GetItemText(unIndex, CFIDSVIEW_ARRIVALS_ID_FLIGHT,
                               szBuffer, sizeof(szBuffer));

        // Match...
        if(strcmp(szBuffer, sFlightNumber) == 0)
            return unIndex;
    }

    // Not found...
    return -1;
}

// Is departure flight currently listed in list control? (-1 not found)
int CFIDSView::IsDeparturesFlightListed(const CFlightID& sFlightNumber)
{
    // Variables...
    unsigned int    unIndex         = 0;
    char            szBuffer[1024]  = {0};

    // Scan through our list control to see if flight is listed...
    for(unIndex = 0; unIndex < static_cast<unsigned int>(Departures.GetItemCount()); unIndex++)
    {
        // Get this entry's flight number...
        Departures.GetItemText(unIndex, CFIDSVIEW_DEPARTURES_ID_FLIGHT,
                               szBuffer, sizeof(szBuffer));
        
        // Match...
        if(strcmp(szBuffer, sFlightNumber.GetValue()) == 0)
            return unIndex;
    }

    // Not found...
    return -1;
}

// Resize bitmap to new dimensions...
HBITMAP CFIDSView::ResizeBitmap(HBITMAP hBitmap1, unsigned short usNewWidth,
                                unsigned short usNewHeight)
{
    // Variables...
    BITMAP  Bitmap1;
    BITMAP  Bitmap2;
    HBITMAP hBitmap2    = NULL;
    HDC     hDC         = NULL;
    HDC     hMemoryDC1  = NULL;
    HDC     hMemoryDC2  = NULL;

    // Bounds check...
    if(!hBitmap1 || !usNewWidth || !usNewHeight)
        return NULL;

    // Get original bitmap's dimensions...
  ::GetObject(hBitmap1, sizeof(BITMAP), &Bitmap1);

    // Dimensions already ok...
    if((Bitmap1.bmWidth == usNewWidth) && 
       (Bitmap1.bmHeight == usNewHeight))
        return hBitmap1;
    
    // Create device contexts for both images...
    hDC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
    hMemoryDC1 = ::CreateCompatibleDC(hDC);
    hMemoryDC2 = ::CreateCompatibleDC(hDC);
  ::DeleteDC(hDC);

    // Prepare second bitmap's dimensions...
    Bitmap2                 = Bitmap1;
    Bitmap2.bmWidth         = usNewWidth;
    Bitmap2.bmHeight        = usNewHeight;
    //Bitmap2.bmWidthBytes    = ((Bitmap2.bmWidth + 15) / 16) * 2;

    // Create a new bitmap of the new size...
    hBitmap2 = ::CreateBitmapIndirect(&Bitmap2);

        // Failed...
        if(!hBitmap2)
        {
            // Cleanup...
            ::DeleteDC(hMemoryDC1);
            ::DeleteDC(hMemoryDC2);
            
            // Abort...
            return NULL;
        }

    // Copy the original and the blank new bitmap to memory...
  ::SelectObject(hMemoryDC1, hBitmap1);
  ::SelectObject(hMemoryDC2, hBitmap2);

    // Resize and check for error...
    if(!::StretchBlt(hMemoryDC2, 0, 0, Bitmap2.bmWidth, Bitmap2.bmHeight,
                     hMemoryDC1, 0, 0, Bitmap1.bmWidth, Bitmap1.bmHeight,
                     SRCCOPY))
    {
        // Cleanup...
      ::DeleteDC(hMemoryDC1);
      ::DeleteDC(hMemoryDC2);
            
        // Abort...
        return NULL;
    }

    // Cleanup...
  ::DeleteDC(hMemoryDC1);
  ::DeleteDC(hMemoryDC2);
  ::DeleteObject(hBitmap1);


    // Return the resized bitmap to the caller...
    return hBitmap2;
}

// Update flight schedule...
bool CFIDSView::UpdateOutput()
{
    // Variables...
    CTermPlanDoc           *pDocument                       = NULL;
	AnimationData          *pAnimData                       = NULL;
    long                    lCurrentTime                    = 0;
    int                     nCurrentHour                    = 0;
    int                     nCurrentMinute                  = 0;
    int                     nCurrentSecond                  = 0;
    int                     nItem                           = 0;
    unsigned int            unIndex                         = 0;
    unsigned int            unFlights                       = 0;
    //FlightLog              *pLog                            = NULL;
    //FlightLogEntry          LogEntry;
    //FlightDescStruct        FlightDescription;     //uncomment for not using the log now
    char                    szBuffer[1024]                  = {0};
    ElapsedTime             CurrentTime;
    ElapsedTime             EntryTime;
    CRect                   ItemRectangle;
    CSize                   ScrollSize;


    // Get the document object...
    pDocument = GetDocument();

    // Animation not running, abort...
    if(pDocument->m_eAnimState == CTermPlanDoc::anim_none)
        return false;
	
	AirsideFlightLog& airsideFlightLog = pDocument->GetAirsideSimLogs().m_airsideFlightLog;
	AirsideFlightLogEntry LogEntry;
	AirsideFlightDescStruct FlightDescription;

    // Calculate current animation time...
    lCurrentTime = pDocument->m_animData.nLastAnimTime;
	nCurrentHour = lCurrentTime / 360000;
	nCurrentMinute = (lCurrentTime - nCurrentHour * 360000) / 6000;
	nCurrentSecond = (lCurrentTime - nCurrentHour * 360000 - nCurrentMinute * 6000) / 100;
    CurrentTime.set(nCurrentHour, nCurrentMinute, nCurrentSecond);

    // Get the flight log object...
    // pLog = pDocument->GetTerminal().flightLog;

    // Get number of flights...
    unFlights = airsideFlightLog.getCount();

    // Process each log entry, if necessary...
    for(unIndex = 0; unIndex < unFlights; unIndex++)
    {
		// Get log entry...
        airsideFlightLog.getItem(LogEntry, unIndex);

        // Get flight description...
        LogEntry.initStruct(FlightDescription);

        // Get list control item ID, if already listed...
        nItem = IsDeparturesFlightListed(FlightDescription._depID);
        
        // This is a departure flight...
        if(FlightDescription._depID)
        {
            // When are we leaving?
            EntryTime.setPrecisely(FlightDescription.depTime);
            
            // Flight has not left, or does not leave, between 30 minutes ago
            //  and 90 minutes into the future...
            if(!(
                 ((CurrentTime.asMinutes() + 90) > EntryTime.asMinutes()) &&
                
                 ((CurrentTime.asMinutes() - 30) < EntryTime.asMinutes())
                )
              )
            {
                // If already listed, remove...
                if(nItem != -1)
                    Departures.DeleteItem(nItem);
                
                // Skip...
                continue;
            }

            // Add to departures list, if not already there...
            if(nItem == -1)
            {
                // Logo...
                nItem = Departures.InsertItem(LVIF_IMAGE, Departures.GetItemCount(),
                                              FlightDescription._airline, 0, 0,
                                              I_IMAGECALLBACK, 0L);

/* 
http://support.microsoft.com/kb/q141834/
*/

                // Airline...
                ExpandAirline(FlightDescription._airline, szBuffer, sizeof(szBuffer));
                Departures.SetItemText(nItem, CFIDSVIEW_DEPARTURES_ID_AIRLINE, 
                                       szBuffer);

                // Departure ID...
//                sprintf(szBuffer, "%d", FlightDescription.depID);
				sprintf(szBuffer, "%s", FlightDescription._depID.GetValue());
                Departures.SetItemText(nItem, CFIDSVIEW_DEPARTURES_ID_FLIGHT, 
                                       szBuffer);

                // Destination...
				ExpandAirport(FlightDescription._destination, szBuffer, sizeof(szBuffer));
                Departures.SetItemText(nItem, CFIDSVIEW_DEPARTURES_ID_DESTINATION, 
                                       szBuffer);

                // Leaving time...
                EntryTime.setPrecisely(FlightDescription.depTime);
                EntryTime.printTime(szBuffer, false);
                Departures.SetItemText(nItem, CFIDSVIEW_DEPARTURES_ID_TIME, 
                                       szBuffer);

                // Status...

                    // Waiting...
                    if(CurrentTime < EntryTime)
                        Departures.SetItemText(nItem, CFIDSVIEW_DEPARTURES_ID_STATUS, 
                                               "On Time");

                    // Departed...
                    else
                        Departures.SetItemText(nItem, CFIDSVIEW_DEPARTURES_ID_STATUS,
                                               "Departed");
            }
        }

        // Get list control item ID, if already listed...
        nItem = IsArrivalsFlightListed(FlightDescription._arrID);

        // This is (also?) an arrival flight...
        if(FlightDescription._arrID)
        {
            // When are we arriving?
            EntryTime.setPrecisely(FlightDescription.arrTime);
            
            // Flight has not left, or does not leave, between 30 minutes ago
            //  and 90 minutes into the future...
            if(!(
                 ((CurrentTime.asMinutes() + 60) > EntryTime.asMinutes()) &&
                
                 ((CurrentTime.asMinutes() - 60) < EntryTime.asMinutes())
                )
              )
            {
                // If already listed, remove...
                if(nItem != -1)
                    Arrivals.DeleteItem(nItem);
                
                // Skip...
                continue;
            }

            // Add to arrivals list, if not already there...
            if(nItem == -1)
            {
                // Logo...
                nItem = Arrivals.InsertItem(LVIF_IMAGE, Arrivals.GetItemCount(),
                                            FlightDescription._airline, 0, 0,
                                            I_IMAGECALLBACK, 0L);

                // Airline...
                ExpandAirline(FlightDescription._airline, szBuffer, sizeof(szBuffer));
                Arrivals.SetItemText(nItem, CFIDSVIEW_ARRIVALS_ID_AIRLINE, 
                                     szBuffer);

                // Arrival ID...
                sprintf(szBuffer, "%s", FlightDescription._arrID.GetValue());
                Arrivals.SetItemText(nItem, CFIDSVIEW_ARRIVALS_ID_FLIGHT, 
                                       szBuffer);

                // Origin...
				ExpandAirport(FlightDescription.origin, szBuffer, sizeof(szBuffer));
                Arrivals.SetItemText(nItem, CFIDSVIEW_ARRIVALS_ID_ORIGIN, 
                                     szBuffer);

                // Arrival time...
                EntryTime.setPrecisely(FlightDescription.arrTime);
                EntryTime.printTime(szBuffer, false);
                Arrivals.SetItemText(nItem, CFIDSVIEW_ARRIVALS_ID_TIME, 
                                     szBuffer);

                // Status...

                    // Waiting...
                    if(CurrentTime < EntryTime)
                        Arrivals.SetItemText(nItem, CFIDSVIEW_ARRIVALS_ID_STATUS, 
                                             "On Time");

                    // Arrived...
                    else
                        Arrivals.SetItemText(nItem, CFIDSVIEW_ARRIVALS_ID_STATUS,
                                             "Arrived");
            }
        }
    }

    // Done...
    return true;
}

// Focus changed event...
void CFIDSView::OnSetFocus(CWnd *pOldWnd) 
{
	// Restore focus to original window...
    CView::OnSetFocus(pOldWnd);
    
    // Repaint us...
	Invalidate(FALSE);
}

// Lost focus event...
void CFIDSView::OnKillFocus(CWnd* pNewWnd) 
{
	// Pass to MFC...
    CView::OnKillFocus(pNewWnd);
}

// Erase background event...
BOOL CFIDSView::OnEraseBkgnd(CDC *pDC) 
{
	// Do not erase the background...
	return TRUE;
}

void CFIDSView::OnSize(UINT nType, int cx, int cy) 
{
	// Resize departures list control...
  ::MoveWindow(Departures.m_hWnd, 0, 20, cx, (cy / 2) - 20, TRUE);

    // Resize arrivals list control...
  ::MoveWindow(Arrivals.m_hWnd, 0, 20 + (cy / 2), cx, (cy / 2) - 20, TRUE);
}

// Window being destroyed event...
void CFIDSView::OnDestroy() 
{
	// Destroy...
    CView::OnDestroy();
}

// View print event...
void CFIDSView::OnViewPrint()
{
	// Variables...
    CDC            DeviceContext;
	CPrintDialog   PrintDialog(FALSE);
	DOCINFO        DocumentInfo;
	BOOL           bPrintingOK         = FALSE;
	CPrintInfo     PrintInfo;

	// Display printing dialog and check for user abort...
    if(PrintDialog.DoModal() == IDCANCEL)
		return;

    // Bind printer and window device contexts...
    DeviceContext.Attach(PrintDialog.GetPrinterDC());
	DeviceContext.m_bPrinting = TRUE;

	// Initialize...
  ::ZeroMemory(&DocumentInfo, sizeof(DOCINFO));
	DocumentInfo.cbSize = sizeof(DOCINFO);
	DocumentInfo.lpszDocName = "View";
	
	// Start printing...
    bPrintingOK = DeviceContext.StartDoc(&DocumentInfo);
	
    // Select printing rectangle...
	PrintInfo.m_rectDraw.SetRect(0, 0, DeviceContext.GetDeviceCaps(HORZRES),
                                 DeviceContext.GetDeviceCaps(VERTRES));
	
    // Let MFC know we are about to print...
	OnBeginPrinting(&DeviceContext, &PrintInfo);

    // Repaint window...
	Invalidate(FALSE);

    // Let MFC know we are printing...
	OnPrint(&DeviceContext, &PrintInfo);
	bPrintingOK = TRUE;

    // Let MFC know we are done printing...
	OnEndPrinting(&DeviceContext, &PrintInfo);
	
    // Printing ended ok, finish...
	if(bPrintingOK)
        DeviceContext.EndDoc();

    // Printing failed, abort...
	else
		DeviceContext.AbortDoc();

    // Unbind device contexts...
	DeviceContext.Detach();
}

// Print event...
void CFIDSView::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
	//get bitmap of view
	CBitmap bmp;
	CAFTEMSBitmap::SaveWndToBmp(bmp,this);

	//print the bitmap
	// Create a compatible memory DC
	CDC memDC;
	memDC.CreateCompatibleDC( pDC );
	memDC.SelectObject( &bmp );
	
	BITMAP bm;
	bmp.GetBitmap( &bm );

	CRect r = pInfo->m_rectDraw;

	double dBMAspect = ((double)bm.bmWidth)/bm.bmHeight;
	double dPAspect = ((double)r.Width())/r.Height();
	if(dBMAspect > dPAspect) {
		//bm relatively wider
		pDC->StretchBlt(0, 0, r.Width(), static_cast<int>(r.Width()/dBMAspect), &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	}
	else {
		pDC->StretchBlt(0, 0, static_cast<int>(r.Height()*dBMAspect), r.Height(), &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	}
	
	//pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &memDC, 0, 0,SRCCOPY);
	memDC.DeleteDC();
	
}

// Export view to disk...
void CFIDSView::OnViewExport()
{
	// Prompt user for filename...
	CFileDialog FileDialog(FALSE, _T("bmp"), _T("*.bmp"), 
                           OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                           _T("Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), 
                           NULL, 0, NULL );

        // Aborted...
        if(FileDialog.DoModal() != IDOK)
            return;

    // Repaint...
    Invalidate(FALSE);

    // Write to disk...
    CAFTEMSBitmap::WriteWindowToDIB((LPCTSTR) FileDialog.GetPathName(), this);
}

// Deconstructor...
CFIDSView::~CFIDSView()
{

}


