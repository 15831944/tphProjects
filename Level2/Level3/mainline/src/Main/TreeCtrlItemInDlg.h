#pragma once

#include "AirsideObjectTreeCtrl.h"
#include "Common/CARCUnit.h"

enum ARCUnit_Length;
class IPropDlgTreeItemData
{
public:
	virtual ~IPropDlgTreeItemData(){};
};

class PropDlgTreeItemDataInstance
{
public:
	void Add(IPropDlgTreeItemData* pdata);
	virtual ~PropDlgTreeItemDataInstance(){ Clear(); }
	
	void Clear();
protected:
	std::vector<IPropDlgTreeItemData*> m_vDatalist;
};
//////////////////////////////////////////////////////////////////////////

class CPath2008;
class TreeCtrlItemInDlg
{
public:
	TreeCtrlItemInDlg(CAirsideObjectTreeCtrl& ctrl, HTREEITEM hitem);
	bool isNull()const;

	TreeCtrlItemInDlg GetRootItem()const;
	static TreeCtrlItemInDlg GetRootItem(CAirsideObjectTreeCtrl& ctrl);

	BOOL SetSelect();

	TreeCtrlItemInDlg& operator=(HTREEITEM hitem){ m_hItem = hitem; return *this; }
	bool operator==(HTREEITEM hitem)const{ return m_hItem == hitem; }


	TreeCtrlItemInDlg AddChild(const CString& strNode=_T("None Text"),HTREEITEM HAfter  = TVI_LAST);
	void Expand(BOOL b = TRUE);

	void Destroy();	
	void DeleteAllChild();
	//void RemoveFromParent();

	TreeCtrlItemInDlg GetParent()const;
	operator bool () const { return IsValid(); }
	virtual bool IsValid()const{ return NULL!= m_hItem; }

	AirsideObjectTreeCtrlItemDataEx* GetData()const;
	AirsideObjectTreeCtrlItemDataEx* InitData();
	AirsideObjectTreeCtrlItemDataEx* GetInitData();

	TreeCtrlItemInDlg& SetUserType(int nUserType);
	TreeCtrlItemInDlg& SetUserData(DWORD_PTR udata);
	int GetUserType()const;
	DWORD_PTR GetUserData()const;


	void AddTokenItem(CString strToken, bool bClear = false, COLORREF color=RGB(0,0,255));

	void AddDrivePipePath(const CPath2008& path, ARCUnit_Length curUnit, double dLevel );
	void AddChildPath(const CPath2008& path, ARCUnit_Length curUnit, int nLevel);
	void AddChildPath(const CPath2008& path, ARCUnit_Length curUnit);

	void SetLengthValueText(CString strPrfix, double dValue,ARCUnit_Length curUnit );
	void SetSpeedValueText(CString strPrfix, double dValue,ARCUnit_Velocity curUnit);
	void SetDegreeValueText(CString strPrfix, double dValue);
	void SetIntValueText(CString strPrefix,int iValue);
	void SetStringText(CString strPrix, CString str);
	void SetString(CString str,bool bToken = false);

	static CString GetValueText(double dValue,ARCUnit_Length curUnit);
	static CString GetValueText(double dValue,ARCUnit_Velocity curUnit);

	CAirsideObjectTreeCtrl& GetCtrl()const{ return *m_ctrl; }
public:
	CAirsideObjectTreeCtrl* m_ctrl;
	HTREEITEM m_hItem;

	virtual void ConstructInit(){}
};

#define DLG_TREEIREM_CONVERSION_DEF(TCLASS,PCLASS) TCLASS(CAirsideObjectTreeCtrl& ctrl,HTREEITEM hItem):PCLASS(ctrl,hItem){}\
	TCLASS(const TreeCtrlItemInDlg& item):PCLASS(item){} \
	TCLASS& operator=(const PCLASS& item){ m_hItem = item.m_hItem; return *this; } 
