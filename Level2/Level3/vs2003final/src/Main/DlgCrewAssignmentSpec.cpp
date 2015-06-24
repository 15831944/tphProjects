// DlgCrewAssignmentSpec.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgCrewAssignmentSpec.h"
#include ".\dlgcrewassignmentspec.h"
#include "../InputOnBoard/CabinCrewGeneration.h"
//-------------------------------------------------------------------------------------------------------
//CFltRelativeIconListCtrl
#include <algorithm>
IMPLEMENT_DYNAMIC(CCRewPositionIconListCtrl, CReSizeColumnListCtrl)

CCRewPositionIconListCtrl::CCRewPositionIconListCtrl()
{
	//icons map
}

CCRewPositionIconListCtrl::~CCRewPositionIconListCtrl()
{
	m_mapElementIcon.clear() ;
}

BEGIN_MESSAGE_MAP(CCRewPositionIconListCtrl, CReSizeColumnListCtrl)
END_MESSAGE_MAP()

void  CCRewPositionIconListCtrl::CreateInitImage(CabinCrewPhysicalCharacteristics* _Crew,CDC& PDC ,CRect rect)
{
	COLORREF color = GetImage(_Crew,INITPOSITION_ICON) ;
		 HBRUSH bush = CreateSolidBrush(color) ;
	PDC.SelectObject(bush) ;
	PDC.Ellipse(rect.left,rect.top,rect.left+rect.Height(),rect.bottom) ;	
}
void CCRewPositionIconListCtrl::CreateFinalImage(CabinCrewPhysicalCharacteristics* _Crew,CDC& PDC ,CRect rect)
{
	COLORREF color = GetImage(_Crew,FINALPOSITION_ICON) ;
	 HBRUSH bush = CreateSolidBrush(color) ;
	PDC.SelectObject(bush) ;
	PDC.Ellipse(rect.left,rect.top,rect.left+rect.Height(),rect.bottom) ;	
}
void CCRewPositionIconListCtrl::AddImage(CabinCrewPhysicalCharacteristics* _crew)
{
    if(_crew == NULL)
		return ;
	CICONPair pair  ;
    pair.m_InitIcon = GetRandomColor() ;
	pair.m_FinalIcon = GetRandomColor() ;
	m_mapElementIcon.insert(std::make_pair(_crew,pair)) ;
}
COLORREF CCRewPositionIconListCtrl::GetRandomColor()
{
   
	return RGB(random(255)*255,random(255)*255,random(255)*255) ;
}
COLORREF CCRewPositionIconListCtrl::GetImage(CabinCrewPhysicalCharacteristics* _Crew,int pos)
{
	std::map<CabinCrewPhysicalCharacteristics*, CICONPair>::iterator iter = m_mapElementIcon.begin();
	for(; iter!= m_mapElementIcon.end(); iter++)
	{
		if(iter->first == _Crew)
		{
			if(pos == INITPOSITION_ICON)
			  return   iter->second.m_InitIcon ;
			if(pos == FINALPOSITION_ICON)
				return iter->second.m_FinalIcon ;
		}
	}

	return RGB(255,255,255);
}

void CCRewPositionIconListCtrl::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	//COLORREF clrTextSave, clrBkSave;
	CRect rcItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	int nItem = lpdis->itemID;
	LVITEM lvi; 
	lvi.mask = LVIF_PARAM | LVIF_STATE; 
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
	this->GetItem(&lvi); 

	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lvi.state & LVIS_SELECTED) || (lvi.state & LVIS_DROPHILITED);

	CRect rcAllLabels; 
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS); 
	CRect rcRank; 
	this->GetItemRect(nItem, rcRank, LVIR_LABEL);
	CRect rcID;
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcID);
	CRect rcInitPos;
	this->GetSubItemRect(nItem,2,LVIR_LABEL,rcInitPos);
	CRect rcFinalPos ;
    this->GetSubItemRect(nItem,3,LVIR_LABEL,rcFinalPos);
	//AdjustColorBoxRect(rcColor);
	CString strRankName = GetItemText(nItem, 0);
	CString strID = GetItemText(nItem,1);
    
	pDC->DrawText(strRankName,rcRank, DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
    pDC->DrawText(strID,rcID, DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

   CabinCrewPhysicalCharacteristics* crew = (CabinCrewPhysicalCharacteristics*)GetItemData(nItem) ;
  /*   CBitmap* bmp = GetImage(crew,INITPOSITION_ICON) ;
	CDC memdc ;
	memdc.CreateCompatibleDC(pDC) ;
	CBitmap* old = memdc.SelectObject(bmp) ;
	BITMAP info ;
	bmp->GetObject(sizeof(info),&info) ;
	pDC->StretchBlt(rcInitPos.left,rcInitPos.top,rcInitPos.Width(),rcInitPos.Height(),&memdc,0,0,info.bmWidth,info.bmHeight,SRCCOPY) ;*/
	CreateInitImage(crew,*pDC,rcInitPos) ;

	
  /*  bmp = GetImage(crew,FINALPOSITION_ICON) ;
	bmp->GetObject(sizeof(info),&info) ;
	old = memdc.SelectObject(bmp) ;
	pDC->StretchBlt(rcFinalPos.left,rcFinalPos.top,rcFinalPos.Width(),rcFinalPos.Height(),&memdc,0,0,info.bmWidth,info.bmHeight,SRCCOPY) ;*/
    CreateFinalImage(crew,*pDC,rcFinalPos) ;

	
	//pDC->FillSolidRect(rcColor,RGB(111,111,111));

	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(RGB(100,145,111));
		pDC->SetBkColor(RGB(234,11,9));
	}

}

//-------------------------------------------------------------------------------------------------------

// CDlgCrewAssignmentSpec dialog

IMPLEMENT_DYNAMIC(CDlgCrewAssignmentSpec, CDialog)
CDlgCrewAssignmentSpec::CDlgCrewAssignmentSpec(CabinCrewGeneration* _crewlist ,CWnd* pParent /*=NULL*/)
	:m_CrewPaxList(_crewlist),CDialog(CDlgCrewAssignmentSpec::IDD, pParent)
{
	m_CrewPaxList = new CabinCrewGeneration ;
	m_CrewPaxList->ReadData() ;
}

CDlgCrewAssignmentSpec::~CDlgCrewAssignmentSpec()
{
	delete m_CrewPaxList ;
}

void CDlgCrewAssignmentSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CREW, m_ListCrew);
	DDX_Control(pDX, IDC_CHECK_CHECK_DOC, m_CheckDoc);
	DDX_Control(pDX, IDC_CHECKDIRECT, m_CheckDirect);
	DDX_Control(pDX, IDC_CHECK_HELP, m_CheckHelp);
	DDX_Control(pDX, IDC_CHECK_DISTRIBUTE, m_Distribute);
	DDX_Control(pDX, IDC_CHECK3, m_checkBlock);
}


BEGIN_MESSAGE_MAP(CDlgCrewAssignmentSpec, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CREW, OnHdnItemclickListCrew)
	ON_NOTIFY(LVN_BEGINRDRAG, IDC_LIST_CREW, OnLvnBeginrdragListCrew)
	ON_NOTIFY(HDN_ENDDRAG, IDC_LIST_CREW, OnHdnEnddragListCrew)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_CHECK_DOC, OnBnClickedCheckCheckDoc)
	ON_BN_CLICKED(IDC_CHECKDIRECT, OnBnClickedCheckdirect)
	ON_BN_CLICKED(IDC_CHECK_HELP, OnBnClickedCheckHelp)
	ON_BN_CLICKED(IDC_CHECK_DISTRIBUTE, OnBnClickedCheckDistribute)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck3)
END_MESSAGE_MAP()

void CDlgCrewAssignmentSpec::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy) ;
	if(m_ListCrew.GetSafeHwnd())
		m_ListCrew.ResizeColumnWidth() ;
}
void CDlgCrewAssignmentSpec::InitCheckBoxState(CabinCrewPhysicalCharacteristics* _Crew)
{
	if(_Crew == NULL)
		return ;
    CCrewDuties* crewDuty = _Crew->GetCrewDuty() ;
	m_CheckDoc.SetCheck(crewDuty->IsCheckDoc()? BST_CHECKED:BST_UNCHECKED) ;
	m_checkBlock.SetCheck(crewDuty->IsBlockPax()?BST_CHECKED:BST_UNCHECKED) ;
	m_CheckDirect.SetCheck(crewDuty->IsDirectPax()?BST_CHECKED:BST_UNCHECKED);
	m_CheckHelp.SetCheck(crewDuty->IsHelpPax()?BST_CHECKED:BST_UNCHECKED) ;
	m_Distribute.SetCheck(crewDuty->IsDistributableDoc()?BST_CHECKED:BST_UNCHECKED) ;
}
BOOL CDlgCrewAssignmentSpec::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	InitListView() ;
	InitListData() ;
	return TRUE ;
}
void CDlgCrewAssignmentSpec::InitListView()
{
	DWORD dwStyle = m_ListCrew.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListCrew.SetExtendedStyle( dwStyle );
	int nwidth[] = {50,40,95,95} ;
	CString column[] = {"Rank","ID","Initial Position" ,"Final Position"} ;
	for (int i =  0 ; i < 4 ; i++)
	{
		 m_ListCrew.InsertColumn(i,column[i],LVCFMT_CENTER,nwidth[i]) ;
	}
	m_ListCrew.InitColumnWidthPercent() ;
}
void CDlgCrewAssignmentSpec::InitListData()
{
    for (int i = 0 ; i < m_CrewPaxList->getDataCount() ;i++)
    {
		AddItemToList(m_CrewPaxList->getData(i)) ;
    }
}
void CDlgCrewAssignmentSpec::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_CrewPaxList->SaveData() ;
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	CDialog::OnOK() ;
}
void CDlgCrewAssignmentSpec::OnCancel()
{
	CDialog::OnCancel() ;
}
void CDlgCrewAssignmentSpec::AddItemToList(CabinCrewPhysicalCharacteristics* _Crew)
{
  //to be continute 
	if(_Crew == NULL)
		return ;
	m_ListCrew.AddImage(_Crew) ;
   CString id ;
   id.Format("%d",_Crew->getID()) ;
	int ndx = m_ListCrew.InsertItem(0,_Crew->getRank()) ;
	m_ListCrew.SetItemText(ndx,1,id) ;
    m_ListCrew.SetItemData(ndx,(DWORD_PTR)_Crew) ;
}
// CDlgCrewAssignmentSpec message handlers

void CDlgCrewAssignmentSpec::OnHdnItemclickListCrew(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	InitCheckBoxState(GetCurselCrew()) ;
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgCrewAssignmentSpec::OnLvnBeginrdragListCrew(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgCrewAssignmentSpec::OnHdnEnddragListCrew(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
CabinCrewPhysicalCharacteristics* CDlgCrewAssignmentSpec::GetCurselCrew()
{
	POSITION posi = m_ListCrew.GetFirstSelectedItemPosition() ;
	CabinCrewPhysicalCharacteristics* _Crew = NULL ;
	if(posi!= NULL)
	{
		int ndx = m_ListCrew.GetNextSelectedItem(posi) ;
		_Crew = (CabinCrewPhysicalCharacteristics*)m_ListCrew.GetItemData(ndx);
	}
	return _Crew ;
}
void CDlgCrewAssignmentSpec::OnBnClickedCheckCheckDoc()
{
	// TODO: Add your control notification handler code here
CabinCrewPhysicalCharacteristics* Crew = GetCurselCrew() ;
if(Crew == NULL)
return ;
	if(m_CheckDoc.GetCheck() == BST_CHECKED)
		Crew->GetCrewDuty()->IsCheckDoc(TRUE) ;
	else
        Crew->GetCrewDuty()->IsCheckDoc(FALSE) ;
}

void CDlgCrewAssignmentSpec::OnBnClickedCheckdirect()
{
	// TODO: Add your control notification handler code here
	CabinCrewPhysicalCharacteristics* Crew = GetCurselCrew() ;
	if(Crew == NULL)
		return ;
	if(m_CheckDirect.GetCheck() == BST_CHECKED)
		Crew->GetCrewDuty()->IsDirectPax(TRUE) ;
	else
		Crew->GetCrewDuty()->IsDirectPax(FALSE) ;
}

void CDlgCrewAssignmentSpec::OnBnClickedCheckHelp()
{
	// TODO: Add your control notification handler code here
	CabinCrewPhysicalCharacteristics* Crew = GetCurselCrew() ;
	if(Crew == NULL)
		return ;
	if(m_CheckHelp.GetCheck() == BST_CHECKED)
		Crew->GetCrewDuty()->IsHelpPax(TRUE) ;
	else
		Crew->GetCrewDuty()->IsHelpPax(FALSE) ;
}

void CDlgCrewAssignmentSpec::OnBnClickedCheckDistribute()
{
	// TODO: Add your control notification handler code here
	CabinCrewPhysicalCharacteristics* Crew = GetCurselCrew() ;
	if(Crew == NULL)
		return ;
	if(m_Distribute.GetCheck() == BST_CHECKED)
		Crew->GetCrewDuty()->IsDistributableDoc(TRUE) ;
	else
		Crew->GetCrewDuty()->IsDistributableDoc(FALSE) ;
}

void CDlgCrewAssignmentSpec::OnBnClickedCheck3()
{
	// TODO: Add your control notification handler code here
	CabinCrewPhysicalCharacteristics* Crew = GetCurselCrew() ;
	if(Crew == NULL)
		return ;
	if(m_checkBlock.GetCheck() == BST_CHECKED)
		Crew->GetCrewDuty()->IsBlockPax(TRUE) ;
	else
		Crew->GetCrewDuty()->IsBlockPax(FALSE) ;
}
