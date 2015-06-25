// DlgSelectFixes.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSelectFixes.h"
#include <algorithm>

// CDlgSelectFixes dialog
using namespace std;

IMPLEMENT_DYNAMIC(CDlgSelectFixes, CDialog)

CDlgSelectFixes::CDlgSelectFixes(const std::vector<CString>& vstrAllFixes,
								std::vector<CString>& vstrSelectedFixes,
								LPCTSTR lpszCaption,
								CWnd* pParent)
	: CDialog(CDlgSelectFixes::IDD, pParent)
	, m_vstrAllFixes(vstrAllFixes)
	, m_vstrSelectedFixes(vstrSelectedFixes)
	, m_strCaption(lpszCaption)
{
}

CDlgSelectFixes::~CDlgSelectFixes()
{
}

void CDlgSelectFixes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ALLFIXES, m_wndListAllFixes);
	DDX_Control(pDX, IDC_TREE_SELECTEDFIXES, m_wndTreeSelectedFixes);
}

BEGIN_MESSAGE_MAP(CDlgSelectFixes, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDFIX, OnBnClickedButtonAddfix)
	ON_BN_CLICKED(IDC_BUTTON_DELFIX, OnBnClickedButtonDelfix)
END_MESSAGE_MAP()

class CheckIfFixSelected
{
public:
	CheckIfFixSelected(const vector<CString>& vstrSelectedFix)
		: m_vstrSelectedFix(vstrSelectedFix)
	{
	}

	bool operator()(const CString& strFixName)
	{
		for (vector<CString>::const_iterator iter = m_vstrSelectedFix.begin();
			 iter != m_vstrSelectedFix.end();
			 iter++)
		{
			if (strFixName.CompareNoCase(*iter) == 0)
				return true;
		}
		return false;
	}

private:
	const vector<CString>& m_vstrSelectedFix;
};

BOOL CDlgSelectFixes::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(m_strCaption);

	// fill the list box
	vector<CString>::iterator iterLast = remove_if(m_vstrAllFixes.begin(), m_vstrAllFixes.end(), CheckIfFixSelected(m_vstrSelectedFixes));
	m_vstrAllFixes.erase(iterLast, m_vstrAllFixes.end());

	vector<CString>::const_iterator iter;
	for (iter = m_vstrAllFixes.begin(); iter != m_vstrAllFixes.end(); iter++)
	{
		m_wndListAllFixes.AddString(*iter);
	}

	HTREEITEM hParentItem = TVI_ROOT;
	for (iter = m_vstrSelectedFixes.begin(); iter != m_vstrSelectedFixes.end(); iter++)
	{
		HTREEITEM hItem = m_wndTreeSelectedFixes.InsertItem(*iter, hParentItem);
		m_wndTreeSelectedFixes.Expand(hParentItem, TVE_EXPAND);
		hParentItem = hItem;
	}

	return TRUE;
}

void CDlgSelectFixes::OnBnClickedButtonAddfix()
{
	int nIndex = m_wndListAllFixes.GetCurSel();
	if (nIndex == LB_ERR)
		return;

	CString strAddFix;
	m_wndListAllFixes.GetText(nIndex, strAddFix);
	m_wndListAllFixes.DeleteString(nIndex);

	HTREEITEM hSelItem = m_wndTreeSelectedFixes.GetSelectedItem();
	if (hSelItem == NULL)
	{
		// find the last item
		HTREEITEM hItem = m_wndTreeSelectedFixes.GetRootItem();
		HTREEITEM hChildItem = m_wndTreeSelectedFixes.GetChildItem(hItem);
		while (hChildItem != NULL)
		{
			hItem = hChildItem;
			hChildItem = m_wndTreeSelectedFixes.GetChildItem(hItem);
		}

		m_wndTreeSelectedFixes.InsertItem(strAddFix, hItem);
		m_wndTreeSelectedFixes.Expand(hItem, TVE_EXPAND);
	}
	else
	{
		std::vector<CString> vChildStrings;
		vChildStrings.push_back(strAddFix);
		HTREEITEM hChildItem = m_wndTreeSelectedFixes.GetChildItem(hSelItem);
		while (hChildItem != NULL)
		{
			vChildStrings.push_back(m_wndTreeSelectedFixes.GetItemText(hChildItem));
			hChildItem = m_wndTreeSelectedFixes.GetChildItem(hChildItem);
		}

		HTREEITEM hDelItem = m_wndTreeSelectedFixes.GetChildItem(hSelItem);
		if (hDelItem != NULL)
			m_wndTreeSelectedFixes.DeleteItem(hDelItem);

		HTREEITEM hParentItem = hSelItem;
		vector<CString>::const_iterator iter;
		for (iter = vChildStrings.begin(); iter != vChildStrings.end(); ++iter)
		{
			HTREEITEM hItem = m_wndTreeSelectedFixes.InsertItem(*iter, hParentItem);
			m_wndTreeSelectedFixes.Expand(hParentItem, TVE_EXPAND);
			hParentItem = hItem;
		}
	}
}

void CDlgSelectFixes::OnBnClickedButtonDelfix()
{
	HTREEITEM hItem = m_wndTreeSelectedFixes.GetSelectedItem();
	if (hItem == NULL)
		return;

	m_wndListAllFixes.AddString(m_wndTreeSelectedFixes.GetItemText(hItem));

	// restore the child items
	std::vector<CString> vChildStrings;
	HTREEITEM hChildItem = m_wndTreeSelectedFixes.GetChildItem(hItem);
	while (hChildItem != NULL)
	{
		vChildStrings.push_back(m_wndTreeSelectedFixes.GetItemText(hChildItem));
		hChildItem = m_wndTreeSelectedFixes.GetChildItem(hChildItem);
	}
	HTREEITEM hParentItem = m_wndTreeSelectedFixes.GetParentItem(hItem);
	m_wndTreeSelectedFixes.SetRedraw(FALSE);

	m_wndTreeSelectedFixes.DeleteItem(hItem);
	if (hParentItem == NULL)
		hParentItem = TVI_ROOT;

	vector<CString>::const_iterator iter;
	for (iter = vChildStrings.begin(); iter != vChildStrings.end(); iter++)
	{
		HTREEITEM hItem = m_wndTreeSelectedFixes.InsertItem(*iter, hParentItem);
		m_wndTreeSelectedFixes.Expand(hParentItem, TVE_EXPAND);
		hParentItem = hItem;
	}
	m_wndTreeSelectedFixes.SetRedraw();
}

void CDlgSelectFixes::OnOK()
{
	m_vstrSelectedFixes.clear();
	
	HTREEITEM hItem = m_wndTreeSelectedFixes.GetRootItem();
	while (hItem != NULL)
	{
		m_vstrSelectedFixes.push_back(m_wndTreeSelectedFixes.GetItemText(hItem));
		hItem = m_wndTreeSelectedFixes.GetChildItem(hItem);
	}

	CDialog::OnOK();
}

void CDlgSelectFixes::OnCancel()
{
	CDialog::OnCancel();
}
