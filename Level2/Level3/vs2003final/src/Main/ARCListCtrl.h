#pragma once
#include <common/elaptime.h>

class ListCtrlItemImpl
{
public:
	virtual ~ListCtrlItemImpl(){}
	virtual void OnDoneEdit(CListCtrl& ctrl, int index, int column ){ }
	virtual void OnSetTo(CListCtrl& ctrl, int index, int column){ }
	virtual void DblClickItem(CListCtrl& ctrl, int index, int column){}
};

class ListCtrlCombomBoxImpl : public ListCtrlItemImpl
{
public:
	ListCtrlCombomBoxImpl(){ m_pCombCtrl = NULL; }
	virtual void DblClickItem(CListCtrl& ctrl, int index, int column);
protected:
	CComboBox* ShowInPlaceList( CListCtrl& ctrl,int nItem, int nCol);
	CStringList strList;  
	CComboBox* m_pCombCtrl;
};

class ListCtrlEditImpl : public ListCtrlItemImpl
{
public:
	ListCtrlEditImpl(){		m_pEdit = NULL;	}	
	virtual void DblClickItem(CListCtrl& ctrl, int index, int column);
	virtual void OnDoneEdit(CListCtrl& ctrl, int index, int column );	
protected:
	CEdit* ShowInPlaceEdit(CListCtrl& ctrl, int index, int column );
	//CPtrArray ddStyleList;
	CEdit* m_pEdit;
	CString strRet;
};

class InputTerminal;
class ListCtrlProbDistImpl : public ListCtrlCombomBoxImpl
{
public:
	ListCtrlProbDistImpl(InputTerminal* pInputTerm);
	virtual void DblClickItem(CListCtrl& ctrl, int index, int column);
	virtual void OnDoneEdit(CListCtrl& ctrl, int index, int column );	
protected:
	InputTerminal * m_pInTerm;	
	//ProbabilityDistribution* pProbDist;
	CString strDist;
};


class ListCtrlTimeRangeImpl : public ListCtrlItemImpl
{
public:
	virtual void DblClickItem(CListCtrl& ctrl, int index, int column);	
protected:
	ElapsedTime startT;
	ElapsedTime endT;
};

class ListCtrlSpinEditImpl : public ListCtrlItemImpl
{
	
};


typedef std::vector<int> IntList;

class CARCListCtrl : public CListCtrl
{
public:	
	afx_msg BOOL OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnClickColumn(NMHDR* pNMHDR, LRESULT* pResult);

	void SetItemImpl(int nItem, int nSubItem, ListCtrlItemImpl* ,bool bManageDel = true);
	ListCtrlItemImpl* GetItemImpl(int nItem, int nSubItem)const;

	int GetSelectedItems(IntList& itemlist)const;
	void SelectItem(int nItem);
	//int GetLastSelectItem()const{ return m_lastSelItem; }
	//int GetLastSelectSubItem()const{ return m_lastSelSubItem; }	
	
	CARCListCtrl();
	virtual ~CARCListCtrl();
	void ClearMangerList();
protected:
	//ListCtrlItemImpl* m_pLastEditImpl;	
	typedef std::map<int, ListCtrlItemImpl*> ItemImplMap;	
	ItemImplMap* GetImplMap(int nItem);
	ItemImplMap* GetImplMap(int nItem)const;
	void Clear();
	
	/*int m_lastSelItem;
	int m_lastSelSubItem;*/

	std::vector<ItemImplMap*> m_vItemMapList; 

	std::vector<ListCtrlItemImpl*> m_vManageImpList;
	void AddManageImp(ListCtrlItemImpl* pImpl);
	// HitTestEx - Determine the row index and column index for a point
	// Returns   - the row index or -1 if point is not over a row
	// point     - point to be tested.
	// col       - to hold the column index
	int HitTestEx(CPoint& point, int* col);
	
	
	DECLARE_MESSAGE_MAP()


};
