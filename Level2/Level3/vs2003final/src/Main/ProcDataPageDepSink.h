// ProcDataPageDepSink.h: interface for the CProcDataPageDepSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCDATAPAGEDEPSINK_H__0BBAE52F_696B_400C_8F0C_1AE0D14CC1AD__INCLUDED_)
#define AFX_PROCDATAPAGEDEPSINK_H__0BBAE52F_696B_400C_8F0C_1AE0D14CC1AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProcDataPage.h"


class CProcDataPageDepSink : public CProcDataPage  
{

protected:
	int m_nLastLevelNo;
	
public:
	virtual BOOL OnToolTipText(UINT, NMHDR *pNMHDR, LRESULT *pResult);
//	virtual bool OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnToolbarbuttondel();
	virtual bool SelectLinkProc1(ProcessorID _id);
	virtual void OnToolbarbuttonadd();
	virtual void OnSelchangedTreeData(NMHDR* pNMHDR, LRESULT* pResult);
	void ReloadLinkProc1List( MiscData* _pMiscData );
	BOOL OnInitDialog();
	void AddPro1();
	CProcDataPageDepSink();
	virtual ~CProcDataPageDepSink();
	CProcDataPageDepSink( enum PROCDATATYPE _enumProcDataType, InputTerminal* _pInTerm, const CString& _csProjPath )
    : CProcDataPage(  _enumProcDataType,  _pInTerm,  _csProjPath )
	{
		m_pInTerm = _pInTerm;
		m_csProjPath = _csProjPath;
		m_nProcDataType = _enumProcDataType;
		m_hLinkage=NULL;
		m_hPSS=NULL;		
		m_nLastLevelNo=0;
	}
	

};

#endif // !defined(AFX_PROCDATAPAGEDEPSINK_H__0BBAE52F_696B_400C_8F0C_1AE0D14CC1AD__INCLUDED_)
