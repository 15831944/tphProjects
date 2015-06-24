/*
  Name:         CFIDSFrameView.h (definition)
  Copyright:    Aviation Research Corporation
  Description:  CFIDSFrameView class definition...
*/

// Pre-procs...
#ifndef _CFIDSFRAMEVIEW_H_
#define _CFIDSFRAMEVIEW_H_


// CFIDSFrameView class...
class CFIDSFrameView : public CMDIChildWnd
{
	// Enable CObject derived class to be created dynamically at runtime...
    DECLARE_DYNCREATE(CFIDSFrameView)

    // Protected stuff...
    protected:
        
        // Constructor...
        CFIDSFrameView();

        // Deconstructor...
        virtual ~CFIDSFrameView();

        // Variables...
        HICON   hTitleIcon;

    // IDE generated mess...

        // ClassWizard generated virtual function overrides
    	//{{AFX_VIRTUAL(CFIDSFrameView)
        public:
            virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
        protected:
    	//}}AFX_VIRTUAL

// Implementation
protected:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CFIDSFrameView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif

