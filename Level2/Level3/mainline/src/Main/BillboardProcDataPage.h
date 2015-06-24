#pragma once
#include "procdatapage.h"

class CBillboardProcDataPage :public CProcDataPage
{

public:
	CBillboardProcDataPage( enum PROCDATATYPE _enumProcDataType, InputTerminal* _pInTerm, const CString& _csProjPath );
	virtual ~CBillboardProcDataPage(void);

	class paxTypeTreeItem
	{
	public:
		paxTypeTreeItem()
		{
			m_hPaxType = NULL;
			m_hPropensity = NULL;
			m_hBuy = NULL;
		}
	public:
		HTREEITEM m_hPaxType;
		HTREEITEM m_hPropensity;
		HTREEITEM m_hBuy;	
	};

public:
	virtual void SetTree();
	virtual void SetValue( MiscData* _pMiscData );
	virtual void ReLoadDetailed();
	void AddBillboardProc();
	void AddPaxTypeCoefficient();
	void InsertPaxTypeCoefficient(const CPaxTypeCoefficientItem& item);
	bool SelectBillboardLinkedProc(ProcessorID _id);
	void ReloadBillboardLinkedProcList( MiscBillboardData* _pMiscData );
	void DelBillboardLinkedProc();
	void ReloadPaxTypeCoefficient( MiscBillboardData* _pMiscData );
	void DelPaxTypeCoefficient();
	void NoReloadDefaultPaxType(MiscBillboardData* _pMiscData);

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnSelchangedTreeData(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnToolbarbuttonadd();
	virtual void OnToolbarbuttondel();
	virtual void OnSelchangeListProcessor();
//	virtual	void OnEditOrSpinValue(){};
	HTREEITEM m_hLinkedProc;
	HTREEITEM m_hTimeRemaing;
	HTREEITEM m_hPaxTypeRoot;
	HTREEITEM m_hTimeStop;
//	HTREEITEM m_hDefaultType;
	std::vector<paxTypeTreeItem> m_vPaxTypeTreeItem;
	paxTypeTreeItem m_hDefaultTypeItem;
};
