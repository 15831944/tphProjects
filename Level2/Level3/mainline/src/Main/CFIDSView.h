/*
  Name:         CFIDSView.h (definition)
  Copyright:    Aviation Research Corporation
  Description:  CFIDSView class definition...
*/

// Pre-procs...
#ifndef _CFIDSVIEW_H_
#define _CFIDSVIEW_H_

    // Includes...
    #include "termplan.h"
    #include "TermPlanDoc.h"
    #include "ActivityDensityData.h"
    #include "ActivityDensityParams.h"
    #include "AnimationTimeManager.h"
    #include "PBuffer.h"
   // #include "glh_extensions.h"
    #include "ChildFrameDensityAnim.h"
    #include "AFTEMSBitmap.h"

// Main term plan class...
class CTermPlanDoc;

// GUI stuff...

    // Departure list control...
    #define CFIDSVIEW_DEPARTURES_IDC    1

    // Departures list control columns...
    enum
    {
        CFIDSVIEW_DEPARTURES_ID_LOGO = 0,
        CFIDSVIEW_DEPARTURES_ID_AIRLINE,
        CFIDSVIEW_DEPARTURES_ID_FLIGHT,
        CFIDSVIEW_DEPARTURES_ID_DESTINATION,
        CFIDSVIEW_DEPARTURES_ID_TIME,
        CFIDSVIEW_DEPARTURES_ID_STATUS
    };

    // Arrivals list control...
    #define CFIDSVIEW_ARRIVALS_IDC      2
    
    // Arrivals list control columns...
    enum
    {
        CFIDSVIEW_ARRIVALS_ID_LOGO = 0,
        CFIDSVIEW_ARRIVALS_ID_AIRLINE,
        CFIDSVIEW_ARRIVALS_ID_FLIGHT,
        CFIDSVIEW_ARRIVALS_ID_ORIGIN,
        CFIDSVIEW_ARRIVALS_ID_TIME,
        CFIDSVIEW_ARRIVALS_ID_STATUS
    };

    // Desired logo dimensions...
    #define CFIDSVIEW_LOGO_WIDTH    75
    #define CFIDSVIEW_LOGO_HEIGHT   25

// CFIDSView class...
class CFIDSView : public CView
{
    // Public stuff...
    public:

        // Enable CObject derived class to be created dynamically at runtime...
        DECLARE_DYNCREATE(CFIDSView)
        
        // Fetch project object...
        CTermPlanDoc       *GetDocument();

        // Printer callback...
        void                OnPrint(CDC *pDC, CPrintInfo *pInfo);
        
        // Update flight schedule...
        bool                UpdateOutput();

    // Protected stuff...
    protected:

        // Variables...
        CImageList          AirlineImages;
        int                 nUnknownImageIndex;
        CListCtrl           Departures;
        CListCtrl           Arrivals;

    	// Constructor...
        CFIDSView();

        // Expand contracted airline name... (AA -> American Airlines)
        bool                ExpandAirline(const char *pszContractedName,
                                          char *pszBuffer, 
                                          unsigned short usBufferSize);

        // Expand contracted airport name... (YVR -> Vancouver Canada)
        bool                ExpandAirport(const char *pszContractedName,
                                          char *pszBuffer, 
                                          unsigned short usBufferSize);

        // Is arrivals flight currently listed in list control? (-1 not found)
        int                 IsArrivalsFlightListed(const CFlightID& sFlightNumber);

        // Is departure flight currently listed in list control? (-1 not found)
        int                 IsDeparturesFlightListed(const CFlightID& sFlightNumber);

        // Resize bitmap to new dimensions...
        HBITMAP             ResizeBitmap(HBITMAP hBitmap1, 
                                         unsigned short usNewWidth,
                                         unsigned short usNewHeight);

    	// Deconstructor...
        virtual            ~CFIDSView();

	// IDE generated mess...

        // ClassWizard generated virtual function overrides
    	//{{AFX_VIRTUAL(CFIDSView)
    	protected:
        	virtual void    OnDraw(CDC* pDC);
    	//}}AFX_VIRTUAL
    
        // Compiling in debug mode...
        #ifdef _DEBUG
        	
            // Check object...
            virtual void    AssertValid() const;
            
            // Dump object context...
        	virtual void   Dump(CDumpContext& dc) const;
    
        #endif
       
    	// Message map function table...
    
    	//{{AFX_MSG(CFIDSView)
    	afx_msg void   OnSetFocus(CWnd* pOldWnd);
    	afx_msg void   OnKillFocus(CWnd* pNewWnd);
    	afx_msg int    OnCreate(LPCREATESTRUCT lpCreateStruct);
    	afx_msg BOOL   OnEraseBkgnd(CDC* pDC);
    	afx_msg void   OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
    	afx_msg void   OnGetListDisplayInfo(NMHDR *pnmhdr, LRESULT *pResult);
        afx_msg void   OnViewPrint();
    	afx_msg void   OnViewExport();
    	DECLARE_MESSAGE_MAP()
};

// Some random stuff...
#ifndef _DEBUG
inline CTermPlanDoc* CFIDSView::GetDocument()
   { return (CTermPlanDoc*)m_pDocument; }
#endif
//{{AFX_INSERT_LOCATION}}

#endif
