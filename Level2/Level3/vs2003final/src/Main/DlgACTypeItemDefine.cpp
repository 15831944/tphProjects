// DlgACTypeItemDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgACTypeItemDefine.h"
#include "../Common/ACTypesManager.h"
#include ".\dlgactypeitemdefine.h"


// CDlgACTypeItemDefine dialog

IMPLEMENT_DYNAMIC(CDlgACTypeItemDefine, CDialog)
CDlgACTypeItemDefine::CDlgACTypeItemDefine(CACTypesManager* _pAcMan,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgACTypeItemDefine::IDD, pParent)
	,m_pACMan(_pAcMan)
{
	m_bNewComponent= false;
}

CDlgACTypeItemDefine::~CDlgACTypeItemDefine()
{
}

// CDlgACTypeItemDefine message handlers
void CDlgACTypeItemDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_cmbName1);
	DDX_Control(pDX, IDC_COMBO_LEVEL2, m_cmbName2);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_cmbName3);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_cmbName4);
	DDX_Text(pDX, IDC_EDIT_CREATOR, m_strCreator);
	DDX_Text(pDX, IDC_EDIT_IMPORTFILE, m_sImportFile);

}


BEGIN_MESSAGE_MAP(CDlgACTypeItemDefine, CDialog)
	ON_CBN_KILLFOCUS(IDC_COMBO_LEVEL1, OnCbnKillfocusComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL1, OnCbnDropdownComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL2, OnCbnDropdownComboNameLevel2)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL3, OnCbnDropdownComboNameLevel3)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL4, OnCbnDropdownComboNameLevel4)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, OnBnClickedButtonImport)
END_MESSAGE_MAP()


// CDlgAircraftModelItemDefine message handlers

BOOL CDlgACTypeItemDefine::OnInitDialog()
{
	CDialog::OnInitDialog();

	CACTYPELIST* pvList = NULL;
	int nCount = -1;
	pvList = m_pACMan->GetACTypeList();
	for(int i=0; i<static_cast<int>(pvList->size()); i++)
	{
		CACType* pACT = (*pvList)[i];
		ALTObjectID objID(pACT->GetIATACode());
		m_lstExistObjectName.push_back(objID);
	}

	m_cmbName1.ResetContent();
	m_cmbName2.ResetContent();
	m_cmbName3.ResetContent();
	m_cmbName4.ResetContent();

	InitComboBox();
	UpdateData(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	if(m_bNewComponent)
	{
		GetDlgItem(IDC_BUTTON_IMPORT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_IMPORTFILE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_IMPORT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_IMPORTFILE)->ShowWindow(SW_HIDE);
	}
	return TRUE;
}

void CDlgACTypeItemDefine::OnCbnKillfocusComboNameLevel1()
{
	// TODO: Add your control notification handler code here
}

void CDlgACTypeItemDefine::OnCbnDropdownComboNameLevel1()
{

}

void CDlgACTypeItemDefine::OnCbnDropdownComboNameLevel2()
{	
	m_cmbName2.ResetContent();
	SortedStringList levelName;	

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;

	m_lstExistObjectName.GetLevel2StringList(csLevel1,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName2.AddString(*iter);
	}
}

void CDlgACTypeItemDefine::OnCbnDropdownComboNameLevel3()
{
	m_cmbName3.ResetContent();

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_cmbName2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;	
	SortedStringList levelName;	
	m_lstExistObjectName.GetLevel3StringList(csLevel1,csLevel2,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName3.AddString(*iter);
	}
}
void CDlgACTypeItemDefine::OnCbnDropdownComboNameLevel4()
{
	m_cmbName4.ResetContent();

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_cmbName2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;
	CString csLevel3;
	m_cmbName3.GetWindowText( csLevel3 );
	if( csLevel3.IsEmpty() )
		return;
	SortedStringList levelName;	
	m_lstExistObjectName.GetLevel4StringList(csLevel1,csLevel2,csLevel3,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName4.AddString(*iter);
	}
}

void CDlgACTypeItemDefine::InitComboBox()
{
	SortedStringList levelName;	
	m_lstExistObjectName.GetLevel1StringList(levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName1.AddString(*iter);
	}
	levelName.clear();
}

void CDlgACTypeItemDefine::OnOK()
{
	UpdateData(TRUE);
	CString strName;
	m_cmbName1.GetWindowText(strName);
	strName.Trim();
	if (strName.IsEmpty())
	{
		MessageBox("please input Aircraft Model name.");
		return ;
	}

	if (m_strCreator.IsEmpty())
	{
		MessageBox("please input creator name");
		return;
	}

	m_objName.m_val[0] = strName.MakeUpper();

	m_cmbName2.GetWindowText(strName);
	strName.Trim();
	m_objName.m_val[1] = strName.MakeUpper();

	m_cmbName3.GetWindowText(strName);
	strName.Trim();
	m_objName.m_val[2] = strName.MakeUpper();

	m_cmbName4.GetWindowText(strName);
	strName.Trim();
	m_objName.m_val[3] = strName.MakeUpper();

	if (m_objName.GetIDString().GetLength() > 12)
	{
		MessageBox("ACType name is invalid");
		return;
	}

	CDialog::OnOK();
}
void CDlgACTypeItemDefine::OnBnClickedButtonImport()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( TRUE,"", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"3DS Files (*.3ds)|*.3ds|All Files (*.*)|*.*||", NULL );
	filedlg.m_ofn.lpstrTitle = "Import Furnishing Shape File" ;
	if(filedlg.DoModal()!=IDOK)
		return;
	m_sImportFile = filedlg.GetPathName();
	UpdateData(FALSE);
}
