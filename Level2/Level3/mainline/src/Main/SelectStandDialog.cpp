#include "StdAfx.h"
#include "resource.h"
#include ".\selectstanddialog.h"

IMPLEMENT_DYNAMIC(CSelectStandDialog, CSelectALTObjectDialog)
CSelectStandDialog::CSelectStandDialog(int prjID, int airportID, CWnd* pParent /*= NULL */)
	:CSelectALTObjectDialog(prjID,airportID,pParent)
{
}

CSelectStandDialog::~CSelectStandDialog(void)
{
}

void CSelectStandDialog::DoDataExchange(CDataExchange* pDX)
{
	CSelectALTObjectDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectStandDialog, CSelectALTObjectDialog)
	ON_BN_CLICKED(ID_BTN_SELECT, OnBtnSelect)
END_MESSAGE_MAP()

BOOL CSelectStandDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetType(ALT_STAND);

	CString windowText;
	windowText.Format("Select %s",ALTObject::GetTypeName(m_nObjType));
	SetWindowText(_T(windowText));
	GetDlgItem(ID_SELOBJECT_GROUPBOX)->SetWindowText(ALTObject::GetTypeName(m_nObjType));
	GetDlgItem(IDC_EDIT_ALTOBJECT)->SetWindowText(m_strObjID);

	LoadObjectsTree(TVI_ROOT);
	return TRUE;  

}

void CSelectStandDialog::OnBtnSelect()
{

	HTREEITEM hItem = m_ObjectTree.GetSelectedItem();
	if (hItem == NULL) return;

	if (m_ObjectTree.ItemHasChildren(hItem))
	{
		m_strObjID = "";
		MessageBox("Please select a stand!");
		return;
	}

	CString selectStr = GetTreeItemFullString(hItem);
	if( selectStr.IsEmpty()) 
		return;

	CDialog::OnOK();
}
