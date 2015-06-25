#if !defined(AFX_PROCESSORTREECTRL_H__9806D39A_3673_421A_A7AE_BE7A9E6E452E__INCLUDED_)
#define AFX_PROCESSORTREECTRL_H__9806D39A_3673_421A_A7AE_BE7A9E6E452E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessorTreeCtrl.h : header file
//

#include <vector>
#include <CommonData/procid.h>
#include "..\inputs\procdb.h"
#include <afxtempl.h>
#include "ColorTreeCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CProcessorTreeCtrl window

class CProcessorTreeCtrl : public CColorTreeCtrl
{
// Construction
public:
	CString m_strSelectedID;
	CProcessorTreeCtrl();

// Attributes
public:

// Operations
public:
	// not reload. just reset the item data and color.
	void ResetTreeData( HTREEITEM _hItem );

// Overrides 
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessorTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL m_bDisplayAll;
	HTREEITEM hSelItem;
	std::vector<ProcessorID> m_vectID;
	//void LoadData( InputTerminal* _pInTerm, ProcessorDatabase* _pProcDB, int _nProcType = -1 );

	
	// Load Processor of _pProcType ( or stand ), set the data if processor is in the _pProcDB
	// in:
	//   _bAirsideMode, if is airside mode
	//   _pInTerm,  inputTerminal where the proclist stay
	//   _pProcDB,  the related processor related data.
	//   _nProctype,  processor type be loaded
	//   bGateStandOnly,  if stand
	virtual void LoadData( InputTerminal* _pInTerm, ProcessorDatabase* _pProcDB, int _nProcType = -1 ,BOOL bGateStandOnly=FALSE);

	// Load all processor in the _pProcList
	// in:
	// _pProcList,  the processor list where the processor be loaded.
	// _bAirsideMode, if this is airside mode
	virtual void LoadData( ProcessorList* _pProcList );

	virtual void LoadData( ProcessorList* _pProcList , ProcessorDatabase* _pProcDB);

	int GetDBIndex(CString _str,ProcessorDatabase * _pProcDB);
	virtual bool IsSelected( CString strProcess, ProcessorDatabase* _pProcDB );
	void SelectProcessor( ProcessorID _procID );
	virtual ~CProcessorTreeCtrl();
	
	void setACStandGateFlag( bool _bFlag ) { m_bOnlyShowACStandGate = _bFlag; }
	bool getACStandGateFlag() const { return m_bOnlyShowACStandGate; }

	void LoadGateData( InputTerminal* _pInTerm, ProcessorDatabase* _pProcDB,int _gateType ) ;
	BOOL CheckTheGateType(CString _proName , int _type) ;
	void LoadGateChild( HTREEITEM _hItem, CString _csStr,int  _type ) ;
protected:
	int m_nProcType;

	bool m_bAirsideMode;

	ProcessorList* m_pProcList;
	ProcessorDatabase* m_pProcDB;

	HTREEITEM m_selItem;	
	virtual void LoadChild( HTREEITEM _hItem, CString _csStr ,BOOL bGateStandOnly=FALSE);
	void LoadChild2( HTREEITEM _hItem, CString _csStr ,BOOL bGateStandOnly=FALSE);
	// if the flag is false, show all gate in the tree
	// else only show ac_stand gate in the tree
	bool m_bOnlyShowACStandGate;
	
	bool checkACStandFlag( const CString& procIDString );
	//{{AFX_MSG(CProcessorTreeCtrl)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	
protected:
	void DeleteAllChild(HTREEITEM _hItem);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSORTREECTRL_H__9806D39A_3673_421A_A7AE_BE7A9E6E452E__INCLUDED_)
