// DlgAircraftModelItemDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftModelItemDefine.h"
#include <InputOnboard\ComponentModel.h>
#include <InputOnBoard\AircraftComponentModelDatabase.h>
#include ".\dlgaircraftmodelitemdefine.h"
#include <Renderer\RenderEngine\3ds2mesh.h>
// CDlgAircraftModelItemDefine dialog

IMPLEMENT_DYNAMIC(CDlgAircraftModelItemDefine, CDialog)
CDlgAircraftModelItemDefine::CDlgAircraftModelItemDefine(CAircraftComponentModelDatabase* pAircraftComponentModelDB,CComponentMeshModel* pModel,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAircraftModelItemDefine::IDD, pParent)
	,m_pModel(pModel)
	,m_pAircraftComponentModelDB(pAircraftComponentModelDB)
	, m_strCreator(_T(""))
{
	m_strCreator = pModel->GetCreator();
	m_bNewComponent = false;
}

CDlgAircraftModelItemDefine::~CDlgAircraftModelItemDefine()
{
}

void CDlgAircraftModelItemDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_cmbName1);
	DDX_Control(pDX, IDC_COMBO_LEVEL2, m_cmbName2);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_cmbName3);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_cmbName4);
	DDX_Text(pDX, IDC_EDIT_CREATOR, m_strCreator);
	DDX_Text(pDX, IDC_EDIT_IMPORTFILE, m_sImportFile);
}


BEGIN_MESSAGE_MAP(CDlgAircraftModelItemDefine, CDialog)
	ON_CBN_KILLFOCUS(IDC_COMBO_LEVEL1, OnCbnKillfocusComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL1, OnCbnDropdownComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL2, OnCbnDropdownComboNameLevel2)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL3, OnCbnDropdownComboNameLevel3)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL4, OnCbnDropdownComboNameLevel4)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, OnBnClickedButtonImport)
END_MESSAGE_MAP()


// CDlgAircraftModelItemDefine message handlers

BOOL CDlgAircraftModelItemDefine::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i = 0; i < m_pAircraftComponentModelDB->GetModelCount(); i++)
	{
		CComponentMeshModel* pMode = m_pAircraftComponentModelDB->GetModel(i);
		ALTObjectID altObj;
		altObj.FromString(pMode->m_sModelName);
		m_lstExistObjectName.push_back(altObj);
	}
	
	m_cmbName1.ResetContent();
	m_cmbName2.ResetContent();
	m_cmbName3.ResetContent();
	m_cmbName4.ResetContent();

	ALTObjectID altobjID;
	if (!m_pModel->m_sModelName.IsEmpty())
	{
		altobjID.FromString(m_pModel->m_sModelName);
		m_cmbName1.SetWindowText(altobjID.at(0).c_str());
		m_cmbName2.SetWindowText(altobjID.at(1).c_str());
		m_cmbName3.SetWindowText(altobjID.at(2).c_str());
		m_cmbName4.SetWindowText(altobjID.at(3).c_str());

		m_strCreator = m_pModel->GetCreator();
	}
	
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

void CDlgAircraftModelItemDefine::OnCbnKillfocusComboNameLevel1()
{
	// TODO: Add your control notification handler code here
}

void CDlgAircraftModelItemDefine::OnCbnDropdownComboNameLevel1()
{

}

void CDlgAircraftModelItemDefine::OnCbnDropdownComboNameLevel2()
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

void CDlgAircraftModelItemDefine::OnCbnDropdownComboNameLevel3()
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
void CDlgAircraftModelItemDefine::OnCbnDropdownComboNameLevel4()
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

void CDlgAircraftModelItemDefine::InitComboBox()
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

void CDlgAircraftModelItemDefine::OnOK()
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
	else  
	{
		m_pModel->SetCreator(m_strCreator);   
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
	
	m_pModel->m_sModelName = m_objName.GetIDString();

	if(m_bNewComponent)
	{
		CTime t= CTime::GetCurrentTime();
		m_pModel->SetCreateTime(t);
		m_pModel->SetLastModifiedTime(t);
	}

	CWaitCursor wc;
	m_pModel->MakeSureFileOpen();
	if( C3DS2Mesh::ImportModel(m_sImportFile,*m_pModel,m_pModel->msTmpWorkDir,1.0f ) )
	{
		m_pModel->mbExternalMesh = true;
	}

	//ElapsedTime tModifiedTime;
	//tModifiedTime.getSystemTime(); 	
	
	CDialog::OnOK();
}
void CDlgAircraftModelItemDefine::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CDlgAircraftModelItemDefine::OnBnClickedButtonImport()
{
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
