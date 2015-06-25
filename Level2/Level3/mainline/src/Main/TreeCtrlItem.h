#pragma once
class CARCBaseTree;


class CTreeCtrlItem
{
public:
	CTreeCtrlItem(CARCBaseTree* ctrl,HTREEITEM hItem);

	void RemoveAllChild();
	CTreeCtrlItem GetFirstChild();
	CTreeCtrlItem GetNextSibling();
	BOOL HasChild()const;

	void SetImage(int nImage, int nSelImage);
	void SetText(const CString& text);
	CString GetText()const;

	void SetData(DWORD data);
	DWORD GetData()const;

	CTreeCtrlItem GetParent()const;
	
	CTreeCtrlItem AddChild(const CString& strNode,HTREEITEM HAfter  = TVI_LAST, int nImage = 0,int nSelectImage = 0);
	CTreeCtrlItem AddChild(int StringID,HTREEITEM HAfter  = TVI_LAST,int nImage = 0,int nSelectImage = 0);

	void Expand();
	void Destroy();

	operator bool () const { return IsValid(); }
	virtual bool IsValid()const{ return NULL!=m_hItem; }

	CARCBaseTree& GetCtrl(){ return *m_ctrl; }
	const CARCBaseTree& GetCtrl()const { return *m_ctrl; }
	HTREEITEM GeHItem()const{ return m_hItem; }
protected:
	CARCBaseTree* m_ctrl;
	HTREEITEM m_hItem;
};


#define TREEIREM_CONVERSION_DEF(TCLASS,PCLASS) TCLASS(CARCBaseTree* ctrl,HTREEITEM hItem):PCLASS(ctrl,hItem){}\
	TCLASS(const CTreeCtrlItem& item):PCLASS(item){} 


							