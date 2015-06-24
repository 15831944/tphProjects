// DlgActypeAliasDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgActypeAliasDefine.h"
#include ".\dlgactypealiasdefine.h"
#include "../Common/AcTypeAlias.h"
#include "../Common/ACTypesManager.h"
#include "../Common/SuperTypeRelationData.h"
#include <algorithm>
// CDlgActypeAliasDefine dialog
class CAcTypeAlias ;
IMPLEMENT_DYNAMIC(CDlgActypeAliasDefine, CXTResizeDialog)
CDlgActypeAliasDefine::CDlgActypeAliasDefine(CACTypesManager* _ActypeManger ,CAcTypeAlias* _Alias ,CWnd* pParent /*=NULL*/)
	: m_ActypeManager(_ActypeManger),m_ALias(_Alias),m_CurSelect(NULL),CXTResizeDialog(CDlgActypeAliasDefine::IDD, pParent)
{
	if(_Alias != NULL)
	{
	  m_CurSelect = _Alias->GetActypeInstance() ;
	  if(m_CurSelect == NULL)
	  {
		 if( !_Alias->GetAliasNames()->empty())
		 {
			 m_CurSelect = m_ActypeManager->FilterActypeByStrings(*_Alias->GetAliasNames()) ;
			 _Alias->SetActypeInstance(m_CurSelect) ;
		 }
	  }
	}
}

CDlgActypeAliasDefine::~CDlgActypeAliasDefine()
{
}

void CDlgActypeAliasDefine::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ALIASTOOL, m_Static_tool);
	DDX_Control(pDX, IDC_LIST_ACTYPE, m_ActypeListBox);
	DDX_Control(pDX, IDC_LIST_ALIAS, m_ListBoxCtrl);
	DDX_Control(pDX, IDC_STATIC_ALIAS_CAP, m_static_Cap);
	DDX_Control(pDX, IDC_STATIC_ALIAS_SPEC, m_static_Spec);
	DDX_Control(pDX,IDC_STATIC_NAME,m_Static_Name) ;
}


BEGIN_MESSAGE_MAP(CDlgActypeAliasDefine, CXTResizeDialog)
	ON_STN_CLICKED(IDC_STATIC_ALIAS_SPEC, OnStnClickedStaticAliasSpec)
	ON_WM_SIZE() 
	ON_WM_CREATE() 
	ON_LBN_SELCHANGE(IDC_LIST_ACTYPE,OnTvnSelchangedAcTypeListBox)
	ON_LBN_SELCHANGE(IDC_LIST_ALIAS,OnTvnSelchangedAliasListBox)
    ON_WM_CTLCOLOR()
	ON_COMMAND(ID_ACTYPE_ALIAS_ADD,OnAddALiasName) 
	ON_COMMAND(ID_ACTYPE_ALIAS_DEL,OnDelAliasName)
	ON_COMMAND(ID_ACTYPE_ALIAS_EDIT,OnEditAliasName)
END_MESSAGE_MAP()
void CDlgActypeAliasDefine::OnAddALiasName()
{
	int ndx = m_ListBoxCtrl.AddString(_T("")) ;
	m_ListBoxCtrl.EditItem(ndx) ;
}
void CDlgActypeAliasDefine::OnDelAliasName()
{
	int ndx = m_ListBoxCtrl.GetCurSel() ;
	if(ndx != LB_ERR)
	{
		CString str ;
		m_ListBoxCtrl.GetText(ndx,str) ;

		if(m_ActypeManager->getACTypeItem(str) == m_CurSelect )
		{
			m_CurSelect = NULL ;
			SetActypeText() ;
		}
		m_ListBoxCtrl.DeleteString(ndx) ;
	}
}
void CDlgActypeAliasDefine::OnEditAliasName()
{
	int ndx = m_ListBoxCtrl.GetCurSel() ;
	if(ndx != LB_ERR)
	{
		m_ListBoxCtrl.EditItem(ndx) ;
	}
}
LRESULT CDlgActypeAliasDefine::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_LBITEMCHANGED:
		{

           CString newName ;
		   int length = m_ListBoxCtrl.GetTextLen((int)wParam) ;
		    m_ListBoxCtrl.GetText((int)wParam,newName.GetBuffer(length)) ;
			newName.ReleaseBuffer() ;
         SuperTypeRelationData* relationship =  m_ActypeManager->GetActypeRelationShip() ;
		 if(relationship != NULL)
		 {
			 CString ErrMsg ;
			 BOOL iffind = FALSE ;
			 CString name ;
			 for (int i = 0 ; i < m_ListBoxCtrl.GetCount() ; i++)
			 {
				 length = m_ListBoxCtrl.GetTextLen(i) ;
				 m_ListBoxCtrl.GetText(i,name.GetBuffer(length)) ;
				 if(strcmp(name,newName) == 0 && i != (int)wParam)
				 {
					 iffind = TRUE ;
					 break ;
				 }
				 name.ReleaseBuffer() ;
			 }
			 CAcTypeAlias* PEditNode = NULL ;
			 if(relationship->CheckTheAliasExist(newName,&PEditNode)&& PEditNode !=m_ALias  || iffind)
			 {
				 ErrMsg.Format(_T("%s has existed!"),newName) ;
                MessageBox(ErrMsg,_T("Warning"),MB_OK) ;
				m_ListBoxCtrl.DeleteString((int)wParam) ;
				break ;
			 }
			 std::vector<CString> AliaName ;
			 AliaName.push_back(newName) ;
			 CACType* Pactype = m_ActypeManager->FilterActypeByStrings(AliaName) ;
			 if(Pactype != NULL)
			 {
                  if(m_CurSelect != NULL)
				  {
					  if(FindALiasNameInListBox(m_CurSelect->GetIATACode()) != (int)wParam)
					  {
						ErrMsg.Format(_T("\"%s\" conflict with \"%s\",One AliasNameSet Can't attach two different Actype Instances !"),newName,m_CurSelect->GetIATACode()) ;
						MessageBox(ErrMsg,_T("Warning"),MB_OK) ;
						m_ListBoxCtrl.DeleteString((int)wParam) ;
						break ;
					  }
				  }
					int ndx =   m_ActypeListBox.FindString(0,Pactype->GetIATACode()) ;
					if(ndx != LB_ERR)
					{
						m_ActypeListBox.SetCurSel(ndx) ;
						m_ActypeListBox.SetScrollPos(SB_VERT,ndx,FALSE) ;
						m_CurSelect = Pactype ;
						SetActypeText() ;
					}
			 }else
			 {
				 if(m_CurSelect != NULL)
				 {
					 
					 CString name ;
					 int i = 0 ;
					 for (; i < m_ListBoxCtrl.GetCount() ; i++)
					 {
						 length = m_ListBoxCtrl.GetTextLen(i) ;
						 m_ListBoxCtrl.GetText(i,name.GetBuffer(length)) ;
						 if(strcmp(name,m_CurSelect->GetIATACode()) == 0)
						 {
							 name.ReleaseBuffer();
							 break ;
						 }
						 name.ReleaseBuffer() ;
					 }
					 if(i  == m_ListBoxCtrl.GetCount())
					 {
						 m_CurSelect = NULL ;
						 m_ActypeListBox.SetCurSel(LB_ERR) ;
						 SetActypeText() ;
					 }
				 }
			 }
		 }
		}
    	break;
    default:
        break;
    }
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}
int CDlgActypeAliasDefine::FindALiasNameInListBox(CString& _aliasname)
{
	CString name ;
	int i = 0 ;
	int length = 0 ;
	for (; i < m_ListBoxCtrl.GetCount() ; i++)
	{
		length = m_ListBoxCtrl.GetTextLen(i) ;
		m_ListBoxCtrl.GetText(i,name.GetBuffer(length)) ;
		if(strcmp(name,_aliasname) == 0)
		{
			name.ReleaseBuffer() ;
			return i ;
		}
		name.ReleaseBuffer() ;
	}
	return LB_ERR ;
}
void CDlgActypeAliasDefine::OnTvnSelchangedAcTypeListBox()
{
    int ndx = m_ActypeListBox.GetCurSel() ;
	int AliasNdx = 0 ;
	if(ndx != LB_ERR)
	{
		CAcTypeAlias* pALias = NULL ;
		SuperTypeRelationData* relationship =  m_ActypeManager->GetActypeRelationShip() ;
		if(relationship != NULL)
		{
		  if(relationship->CheckTheAliasExist(((CACType*)(m_ActypeListBox.GetItemData(ndx)))->GetIATACode(),&pALias)&& pALias !=m_ALias )
		  {
			  MessageBox(_T("You have defined alias name for this actype!"),_T("Warning"),MB_OK) ;
			  return ;
		  }
		}
		if(m_CurSelect != NULL)
		{
			AliasNdx = m_ListBoxCtrl.FindStringExact(0,m_CurSelect->GetIATACode()) ;
			m_ListBoxCtrl.DeleteString(AliasNdx) ;
		}
		m_CurSelect = (CACType*)m_ActypeListBox.GetItemData(ndx) ;
		if(m_CurSelect != NULL)
			m_ListBoxCtrl.InsertString(AliasNdx ,m_CurSelect->GetIATACode()) ;
		SetActypeText() ;
	}
}
void CDlgActypeAliasDefine::OnTvnSelchangedAliasListBox()
 {
    int ndx = m_ListBoxCtrl.GetCurSel() ;
	if(ndx != LB_ERR)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,TRUE) ;
		m_toolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,TRUE) ;
	}else
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE) ;
        m_toolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE) ;
	}
 }
void CDlgActypeAliasDefine::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
int CDlgActypeAliasDefine::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1 ;
	if(!m_toolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP) ||
		!m_toolBar.LoadToolBar(IDR_ACTYPE_ALIASNAME)) 
	{
		return -1 ;
	}
	return 0 ;
}
BOOL CDlgActypeAliasDefine::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	OnInitActypeListBox() ;
	OnInitAliasListBox() ;
	OnInitToolBar() ;

	SetActypeText() ;
	SetResize(IDC_STATIC_ACTYPE,SZ_TOP_LEFT,SZ_MIDDLE_CENTER) ;
	SetResize(IDC_LIST_ACTYPE,SZ_TOP_LEFT,SZ_MIDDLE_CENTER) ;
	SetResize(IDC_STATIC_ALIAS,SZ_TOP_CENTER,SZ_MIDDLE_RIGHT) ;
	SetResize(IDC_LIST_ALIAS,SZ_TOP_CENTER,SZ_MIDDLE_RIGHT) ;
	SetResize(IDC_STATIC_ATTACH,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_STATIC_NAME,SZ_TOP_CENTER,SZ_TOP_RIGHT) ;
	SetResize(m_toolBar.GetDlgCtrlID(),SZ_TOP_CENTER,SZ_TOP_CENTER) ;
    SetResize(IDC_STATIC_HX,SZ_TOP_CENTER,SZ_TOP_RIGHT) ;
	SetResize(IDC_STATIC_SPEC,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_STATIC_ALIAS_CAP,SZ_MIDDLE_LEFT,SZ_MIDDLE_LEFT) ;
	SetResize(IDC_STATIC_ALIAS_SPEC,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT) ;

	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;



	return TRUE ;
}
void CDlgActypeAliasDefine::OnInitToolBar()
{
	CRect rect ;
	m_Static_tool.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;

	m_toolBar.MoveWindow(&rect) ;

	m_toolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE) ;
	m_toolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE) ;
	m_toolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE) ;
    m_Static_tool.EnableWindow(FALSE) ;
	m_Static_tool.ShowWindow(SW_HIDE) ;
	m_toolBar.ShowWindow(SW_SHOW) ;
}
HBRUSH CDlgActypeAliasDefine::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	//switch(pWnd->GetDlgCtrlID())
	//{
	//case IDC_STATIC_ALIAS_CAP:
	//	//LOGFONT lgf ;
	//	//CFont* pFont= new CFont ;
	//	//CFont* def_font = pDC->SelectObject(pFont);

	//	//def_font->GetLogFont(&lgf) ;
	//	//lgf.lfWeight = 600 ;

	//	//pFont->CreateFontIndirect(&lgf) ;
	//	//m_static_Cap.SetFont(pFont,TRUE) ;

	//	// def_font = pDC->SelectObject(pFont);
	//	//m_static_Cap.SetWindowText("Actype Alias");
	//	//pDC->TextOut(5, 5, "Actype Alias");
	//	//pDC->SelectObject(def_font);
	//	// m_Static_Cap.SetWindowText(_T("Actype RelationShip"));
	////	delete pFont ;
	//}
	return CXTResizeDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO: Return a different brush if the default is not desired
}
void CDlgActypeAliasDefine::OnInitActypeListBox()
{
	if(m_ActypeManager == NULL)
		return ;
	CACTYPELIST* Actypelist = m_ActypeManager->GetACTypeList() ;
	int ndx = 0 ;
	int cursel = LB_ERR ;
	for (int i = 0 ; i < (int)Actypelist->size() ;i++)
	{
		ndx = m_ActypeListBox.AddString((*Actypelist)[i]->GetIATACode()) ;
		m_ActypeListBox.SetItemData(ndx,(DWORD_PTR)(*Actypelist)[i]) ;
		if(m_ALias->GetActypeInstance() == (*Actypelist)[i])
			cursel = ndx ;
	}
	if(m_ALias->GetActypeInstance() != NULL)
	   cursel = m_ActypeListBox.FindString(0,m_ALias->GetActypeInstance()->GetIATACode()) ;
	if(LB_ERR != cursel)
	{
		m_ActypeListBox.SetCurSel(cursel) ;
		m_ActypeListBox.SetScrollPos(SB_VERT,cursel,FALSE) ;
	}
}
void CDlgActypeAliasDefine::OnInitAliasListBox()
{
   if(m_ALias == NULL)
	   return ;
   std::vector<CString>* ALiasNames = m_ALias->GetAliasNames() ;
   int ndx = 0 ;
   for (int i = 0 ; i < (int)ALiasNames->size() ;i++)
   {
	   ndx = m_ListBoxCtrl.AddString((*ALiasNames)[i]) ;
   }
}
void CDlgActypeAliasDefine::SetActypeText()
{
   if(m_CurSelect != NULL)
	  m_Static_Name.SetWindowText(m_CurSelect->GetIATACode()) ;
   else
      m_Static_Name.SetWindowText(_T("No Actype ")) ;
}
// CDlgActypeAliasDefine message handlers

void CDlgActypeAliasDefine::OnStnClickedStaticAliasSpec()
{
	// TODO: Add your control notification handler code here
}
void CDlgActypeAliasDefine::OnOK()
{
	m_ALias->SetActypeInstance(m_CurSelect) ;
	m_ALias->GetAliasNames()->clear() ;
	int length = 0 ;
	CString Name  ;
	for (int i = 0 ; i < m_ListBoxCtrl.GetCount() ; i++)
	{
		length = m_ListBoxCtrl.GetTextLen(i) ;
		m_ListBoxCtrl.GetText(i,Name.GetBuffer(length)) ;
       m_ALias->AddAliasName(CString(Name)) ;
	   Name.ReleaseBuffer() ;
	}
	CXTResizeDialog::OnOK() ;
}