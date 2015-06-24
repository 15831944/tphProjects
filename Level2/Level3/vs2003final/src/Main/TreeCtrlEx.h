#if !defined(AFX_TREECTRLEX_H__BB2217CC_24E2_4D93_A522_471ED9ED2B8B__INCLUDED_)
#define AFX_TREECTRLEX_H__BB2217CC_24E2_4D93_A522_471ED9ED2B8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeCtrlEx.h : header file
//
//#include "..\MFCExControl\MySpin.h"
//#include "..\MFCExControl\InPlaceDateTimeCtrl.h"	// Added by ClassView
//#include "InPlaceComboBox.h"	// Added by ClassView
//#include "InPlaceEdit2.h"
//#include "..\inputs\probab.h"
//#include "..\MFCExControl\CoolTreeEditSpin.h"
//#include "./InplaceComboBox2.h"
#include "../MFCExControl/ARCTreeCtrlEx.h"
#define MODIFYVALUE				WM_USER + 1000


//////////////////////////////////////////////////////////////////////////
//
//
//	Please do not use this class anymore except use "ProbabilityDistribution" function
//
//	this control has been move to MFCExControl, name CARCTreeCtrlEx
//
//
//	
//
//
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////

class InputTerminal;
/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx window
class CTreeCtrlEx : public CARCTreeCtrlEx
{
// Construction
public:
	CTreeCtrlEx( void );

// Attributes
public:
protected:
	InputTerminal*	m_pInTerm;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlEx)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		
	//}}AFX_VIRTUAL

// Implementation
public:
	//void EditLabel( HTREEITEM _hItem );
	//void EditTime( HTREEITEM _hItem, COleDateTime _oTime );
	void EditProbabilityDistribution( HTREEITEM _hItem ,bool bOverlapItem = true);
	//CEdit* SpinEditLabel( HTREEITEM hItem ,CString sDisplayTitle="" );
	virtual ~CTreeCtrlEx();
	//void SetDisplayType( int _type ){ m_iSpinEditType = _type ;/* m_spinEdit.SetDisplayType( _type );*/};
	//void SetDisplayNum ( int _num ){ m_iNum = _num; /*m_spinEdit.SetPercent( _num );*/};
	//void SetSpinRange( int _nLower, int _nUpper ){ m_nSpinEditLower = _nLower; m_nSpinEditUpper = _nUpper;};
	void EditProbabilityDistribution(InputTerminal*_pInTerm ,HTREEITEM _hItem,bool bOverlapItem);



	void setInputTerminal( InputTerminal* _pInTerm )
	{
		m_pInTerm = _pInTerm;
	}

	void InitComboBox();
	
	////Combo box
	//CEdit* SetComboString(HTREEITEM _hItem,const std::vector<CString>& strlist );
	//void SetComboWidth(int nWidth);
	// Generated message map functions
public:
	//CEdit* StringEditLabel(HTREEITEM hItem, CString sDisplayTitle="");
	//CInPlaceEdit2 m_inPlaceEdit;
	//CInPlaceComboBox m_inPlaceComboBox;
	//CInPlaceDateTimeCtrl m_inPlaceDateTimeCtrl;
	//CCoolTreeEditSpin m_spinEdit;
	//HTREEITEM m_hEditedItem;
	//int m_iSpinEditType;
	//int m_iNum;
	//int m_nSpinEditLower;
	//int m_nSpinEditUpper;

	//CInplaceComboBox2 m_comboBox;
	//int m_nComboSelectItem;
	//int m_nWidth;
	//BOOL m_bEableEdit;
	////{{AFX_MSG(CTreeCtrlEx)
	//	
	////}}AFX_MSG
	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREECTRLEX_H__BB2217CC_24E2_4D93_A522_471ED9ED2B8B__INCLUDED_)
