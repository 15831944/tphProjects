#include "StdAfx.h"
#include ".\dlgaircraftfurnishingnamespec.h"
#include "../InputOnBoard/AircraftFurnishingSection.h"

#include "AircraftFurningFrame.h"
#include "AircraftFurning3DView.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "TermPlan.h"
#include <Renderer\RenderEngine\3ds2mesh.h>
IMPLEMENT_DYNAMIC(CDlgAircraftFurnishingNameSpec, CDialog)
CDlgAircraftFurnishingNameSpec::CDlgAircraftFurnishingNameSpec(CAircraftFurnishingModel* _furnish , CWnd* pParent ):m_Furnishing(_furnish),CDialog(CDlgAircraftFurnishingNameSpec::IDD,pParent)
{
	m_bNew = false;
}

CDlgAircraftFurnishingNameSpec::~CDlgAircraftFurnishingNameSpec(void)
{
}
void CDlgAircraftFurnishingNameSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LEVEL2, m_Combo_level2);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_Combo_level1);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_Combo_level3);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_Combo_level4);
	DDX_Control(pDX,IDC_COMBO_TYPE,m_Combo_Type) ;
	DDX_Text(pDX, IDC_EDIT_IMPORTFILE, m_sImportFile);
	DDX_Control(pDX,IDC_EDIT_CREATOR,m_Creator) ;

}
BEGIN_MESSAGE_MAP(CDlgAircraftFurnishingNameSpec, CDialog)
	ON_STN_CLICKED(IDC_STATIC_LINE3, OnStnClickedStaticLine3)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, OnBnClickedButtonImport)
END_MESSAGE_MAP()

BOOL CDlgAircraftFurnishingNameSpec::OnInitDialog()
{
	CDialog::OnInitDialog() ;
    InitFurnishingType();
	
	//GetDlgItem(IDC_EDIT_FILE)->ShowWindow(m_bNew?SW_SHOW:SW_HIDE);
	//GetDlgItem(IDC_BUTTON_BROWSE)->ShowWindow(m_bNew?SW_SHOW:SW_HIDE);
	
	if(m_bNew)
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

void CDlgAircraftFurnishingNameSpec::InitFurnishingType()
{
	/*CString Type[] = {"Seats(Passenger)","Seats(crew)","Storage device" ,"Galley","Toilet","Service (e.g. bar)","Crew rest","Cockpit"} ;*/
	int selindex = 0 ;
	int index = 0 ;
	for(int i = 0 ; i < CFurnishingDB::GetTypeCount() ; i++)
	{
		index = m_Combo_Type.AddString( CFurnishingDB::GetTypeDisplayString(i) ) ;
		m_Combo_Type.SetItemData(index,(DWORD_PTR)i );
	}

	selindex = m_Combo_Type.FindString( 0,CFurnishingDB::GetTypeDisplayString(m_Furnishing->m_Type) ) ;
	if(selindex == CB_ERR)
		selindex = 0 ;
	m_Combo_Type.SetCurSel(selindex) ;
}
CString CDlgAircraftFurnishingNameSpec::GetCurSelLevel4Name()
{
	CString val ;
	m_Combo_level4.GetWindowText(val) ;
	return val ;
}
CString CDlgAircraftFurnishingNameSpec::GetCurSelLevel1Name()
{
	CString val ;
	m_Combo_level1.GetWindowText(val) ;
	return val ;
}
CString CDlgAircraftFurnishingNameSpec::GetCurSelLevel2Name()
{
	CString val ;
	m_Combo_level2.GetWindowText(val) ;
	return val ;
}
CString CDlgAircraftFurnishingNameSpec::GetCurSelLevel3Name()
{
	CString val ;
	m_Combo_level3.GetWindowText(val) ;
	return val ;
}
CString CDlgAircraftFurnishingNameSpec::GetCreator()
{
	CString val ;
	m_Creator.GetWindowText(val) ;
	return val ;
}
CString CDlgAircraftFurnishingNameSpec::GetType()
{
	CString val ;
	int nSel = m_Combo_Type.GetCurSel();
	int nTypeIdx = (int)m_Combo_Type.GetItemData(nSel);
	return CFurnishingDB::GetTypeString(nTypeIdx);	
}
void CDlgAircraftFurnishingNameSpec::SetTimeString()
{
	CTime tModifiedTime = CTime::GetCurrentTime();
	m_Furnishing->SetCreateTime(tModifiedTime);
	m_Furnishing->SetLastModifiedTime(tModifiedTime) ;
}
void CDlgAircraftFurnishingNameSpec::SaveData()
{
	if(m_Furnishing != NULL)
	{
		m_Furnishing->SetCreator(GetCreator());
		ALTObjectID objID;
		objID.m_val[0] = GetCurSelLevel1Name();
		objID.m_val[1] = GetCurSelLevel2Name();
		objID.m_val[2] = GetCurSelLevel3Name();
		objID.m_val[3] = GetCurSelLevel4Name();
		m_Furnishing->m_sModelName = objID.GetIDString();
		SetTimeString() ;		
		m_Furnishing->m_Type = GetType() ;

		if(m_bNew)
		{
			CTime t= CTime::GetCurrentTime();
			m_Furnishing->SetCreateTime(t);
			m_Furnishing->SetLastModifiedTime(t);
		}


		m_Furnishing->MakeSureFileOpen();
		if( C3DS2Mesh::ImportModel(m_sImportFile,*m_Furnishing,m_Furnishing->msTmpWorkDir,1.0f) )
		{
			m_Furnishing->mbExternalMesh = true;
		}
 	}
}
void CDlgAircraftFurnishingNameSpec::OnCancel()
{
	CDialog::OnCancel() ;
}
void CDlgAircraftFurnishingNameSpec::OnOK()
{
	CWaitCursor wc;
	SaveData() ;
	CDialog::OnOK() ;
}
CDlgAircraftFurnishingNameSpecEdit::CDlgAircraftFurnishingNameSpecEdit(CAircraftFurnishingModel* _furnish , CWnd* pParent /* = NULL */):CDlgAircraftFurnishingNameSpec(_furnish,pParent)
{

}
CDlgAircraftFurnishingNameSpecEdit::~CDlgAircraftFurnishingNameSpecEdit()
{

}
BOOL CDlgAircraftFurnishingNameSpecEdit::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	InitFurnishingType() ;
	ALTObjectID altObjID;
	altObjID.FromString(m_Furnishing->m_sModelName);
 	m_Combo_level1.SetWindowText(altObjID.at(0).c_str()) ;
 	m_Combo_level2.SetWindowText(altObjID.at(1).c_str()) ;
 	m_Combo_level3.SetWindowText(altObjID.at(2).c_str()) ;
 	m_Combo_level4.SetWindowText(altObjID.at(3).c_str()) ;;
	m_Creator.SetWindowText(m_Furnishing->GetCreator()) ;
	return TRUE ;
}
void CDlgAircraftFurnishingNameSpecEdit::SetTimeString()
{
	CTime tModifiedTime = CTime::GetCurrentTime();
	//tModifiedTime.getSystemTime(); 
	
	m_Furnishing->SetLastModifiedTime(tModifiedTime) ;
}
void CDlgAircraftFurnishingNameSpec::OnStnClickedStaticLine3()
{
	// TODO: Add your control notification handler code here
}

void CDlgAircraftFurnishingNameSpec::OnBnClickedButtonImport()
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
