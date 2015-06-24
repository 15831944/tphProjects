#pragma once
#include "procdatapage.h"

class CRetailProcDataPage :
	public CProcDataPage
{
	//enum enumTIType
	//{
	//	TI_PAXTYPE= 0,
	//	TI_TIME,
	//	TI_LEN

	//};
	//class TItemData
	//{
	//public:
	//	TItemData( enumTIType ti, MiscRetailProcData::PaxSkipCondition *pC,int nV)
	//	{
	//		tiType		=ti;
	//		pCondition	=pC;
	//		nValue		=nV;
	//	}

	//	enumTIType tiType;
	//	MiscRetailProcData::PaxSkipCondition *pCondition;
	//	int nValue;
	//	

	//};
public:
	CRetailProcDataPage(enum PROCDATATYPE _enumProcDataType, InputTerminal* _pInTerm, const CString& _csProjPath );
	~CRetailProcDataPage(void);

public:
	virtual void SetTree();
	virtual void SetValue( MiscData* _pMiscData );
	virtual void ReLoadDetailed();
	virtual void InitSpecificBehvaior();


protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnSelchangedTreeData(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnToolbarbuttonadd();
	virtual void OnToolbarbuttondel();
	virtual void OnSelchangeListProcessor();

	bool SelectRetailProcItem(ProcessorID _id,HTREEITEM hRootItem, ProcessorIDList* pProcList);
	void ReloadRetailProcList(HTREEITEM hRootItem, ProcessorIDList* pProcList);

	void AddSkipToTree(MiscRetailProcData::PaxSkipCondition *pCondition);

protected:
	HTREEITEM m_hSkipRoot;
	HTREEITEM m_hCheckOutRoot;
	HTREEITEM m_hBypassRoot;

};
