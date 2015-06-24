#if !defined(AFX_FLOORPROPERTIESDLG_H__2067F606_42CD_11D4_930D_0080C8F982B1__INCLUDED_)
#define AFX_FLOORPROPERTIESDLG_H__2067F606_42CD_11D4_930D_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// FloorPropertiesDlg.h : header file
//
#include "ColorBox.h"
#include "resource.h"

class CFloor2;
class CTermPlanDoc;
class CBaseFloor ; 
/////////////////////////////////////////////////////////////////////////////
// CFloorPropertiesDlg dialog
#define SOURCEUNITS_COUNT 9
#define SOURCEUNITS_DEFAULT 4
#define SOURCEUNITS_EPSILON	0.0001

static const CString SOURCEUNITS_NAMES[] = {
	"inches", "feet", "millimeters", "centimeters", "meters", "kilometers", "nautical miles", "stat. miles", "yards"
};

static const double SOURCEUNITS_TOCENTIMETERS[] = {
	2.54, 30.48, 0.10, 1.0, 100.0, 100000.0, 185200.0, 160934.4, 91.44
};
class CProperiesDlg : public CDialog
{
public:
      CProperiesDlg(CBaseFloor* pFloor,CTermPlanDoc *pDoc, CWnd* pParent = NULL) ;
	  virtual ~CProperiesDlg() {};
	  static int GetScaleIdx(double scale) ;
protected:
	enum { IDD = IDD_FLOORPROPERTIES2 };
	CComboBox	m_cmbSourceUnits;
	CButton	m_cMapFileName;
	CString	m_sFloorName;
	double	m_dRotation;
	double	m_dXOffset;
	double	m_dYOffset;
	double	m_dAlt;
	int		m_nAltRdo;
	CString	m_sUnits1;
	CString	m_sUnits2;
	CString	m_sUnits3;
	BOOL	m_bFloorVisible;
	CString	m_sUnits4;
	double	m_dRealAlt;
	//}}AFX_DATA

	CString m_sMapFileName;
	CString m_sMapPathName;

	CTermPlanDoc* m_pDoc;
	CBaseFloor* m_pFloor;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFloorPropertiesDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
protected:
	BOOL m_bIsCADLoaded;
	BOOL SaveData();
	// Generated message map functions
	//{{AFX_MSG(CFloorPropertiesDlg)
	
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnMapFileName();
	virtual afx_msg  void OnGridOptions();
	virtual afx_msg void OnLayerOptions();
	virtual void OnOK();
	virtual afx_msg void OnButtonRemovemap();
	void UpdateFloorData();
};
class CFloorPropertiesDlg : public CProperiesDlg
{
// Construction
public:
	CFloorPropertiesDlg(CFloor2* pFloor,CTermPlanDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFloorPropertiesDlg() {};
// Dialog Data
	//{{AFX_DATA(CFloorPropertiesDlg)
 // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

	//}}AFX_MSG
	
};


#endif // !defined(AFX_FLOORPROPERTIESDLG_H__2067F606_42CD_11D4_930D_0080C8F982B1__INCLUDED_)
