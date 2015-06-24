#include "StdAfx.h"
#include "Resource.h"
#include ".\onboardlayoutbar.h"
#include "..\InputOnBoard\cInputOnboard.h"
#include "..\Common\AirportDatabase.h"
#include <InputOnBoard\AircraftComponentModelDatabase.h>
#include ".\DlgGetString.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "ViewMsg.h"

CACComponentModelBar::CACComponentModelBar(void)
{
	m_pInputOnboard = NULL;
	m_pActiveCat = NULL;
	m_pActiveModel = NULL;
	m_bCreated = false;
}

CACComponentModelBar::~CACComponentModelBar(void)
{
}




BEGIN_MESSAGE_MAP(CACComponentModelBar, CToolBar24X24)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(ID_COMBO_ONBOARD_CATEGORY,OnSelChangeActiveCategoryComboBox)
	ON_CBN_SELCHANGE(ID_COMBO_ONBOARD_MODEL,OnSelChangeActiveModelComboBox)
	
	//ON_CONTROL_REFLECT(ID_BUTTON_ONBOARD_DELETE_COM,OnDeleteComponetOrCategory)

	//ON_UPDATE_COMMAND_UI_REFLECT(OnUpdateUIDelCompOrCat)

END_MESSAGE_MAP()

int CACComponentModelBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;


	m_bCreated = true;
	// TODO:  Add your specialized creation code here
	InitImageList(IDB_BITMAP_ONBOARD_VIEW_NORMAL,IDB_BITMAP_ONBOARD_VIEW_HOT,IDB_BITMAP_ONBOARD_VIEW_DISABLE,RGB(255,0,255));
	AppendImageList(IDB_BITMAP_ONBOARD_EDIT_NORMAL,IDB_BITMAP_ONBOARD_EDIT_HOT,IDB_BITMAP_ONBOARD_EDIT_DISABLE,RGB(255,0,255) );
	AppendImageList(IDB_BITMAP_ONBOARD_SETTING_NORMAL, IDB_BITMAP_ONBOARD_SETTING_HOT, IDB_BITMAP_ONBOARD_SETTING_DISABLE, RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_LAYOUT_NORMAL,IDB_TOOLBAR_LAYOUT_HOT,IDB_TOOLBAR_LAYOUT_DISABLE,RGB(255,0,255));
	
	SetImageList();

	TBBUTTON tbb;
	CToolBar24X24::InitTBBUTTON(tbb) ;
	
	
	//m_nCountNotButton = 10;
	//AddTextComboBox("Component: ", &m_combActiveCategory,  CSize(186,200),ID_COMBO_ONBOARD_CATEGORY,CBS_DROPDOWNLIST );
	//AddTextComboBox("  Name: ", &m_combActiveModel, CSize(180,200),ID_COMBO_ONBOARD_MODEL,CBS_DROPDOWNLIST );
	//for(int j=0; j<10; j++)
	//{
	//	tbb.iBitmap = -1;
	//	tbb.idCommand = TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton;
	//	tbb.fsState &= ~TBSTATE_ENABLED;
	//	AddButton(&tbb,0,TRUE);
	//}

	////delete category button
	///*tbb.iBitmap = 0;
	//tbb.idCommand = ID_BUTTON_ONBOARD_SHOW_SECTION;
	//tbb.fsState |= TBSTATE_ENABLED;
	//AddButton(&tbb);*/

	////m_nCountNotButton = 1;

	//
	////delete model button
	//tbb.iBitmap = 10;
	//tbb.idCommand = ID_BUTTON_ONBOARD_DELETE_COM;
	//tbb.fsState |= TBSTATE_ENABLED;
	//AddButton(&tbb);
	//
	TBBUTTON tbsep;
	CToolBar24X24::InitTBBUTTON(tbsep) ;
	tbsep.iBitmap = 0;
	tbsep.idCommand = 0;
	tbsep.fsStyle =BTNS_SEP;
	//AddButton(&tbsep);


	tbb.iBitmap = 0;
	tbb.idCommand = ID_BUTTON_ONBOARD_SHOW_SECTION;
	tbb.fsState |= TBSTATE_ENABLED;
	AddButton(&tbb);

	tbb.iBitmap = 1;
	tbb.idCommand = ID_BUTTON_ONBOARD_SHOW_FRAME;
	AddButton(&tbb);

	tbb.iBitmap = 2;
	tbb.idCommand = ID_BUTTON_ONBOARD_SHOW_SHAPE;
	AddButton(&tbb);	

	AddButton(&tbsep);

	tbb.iBitmap = 3;
	tbb.idCommand = ID_BUTTON_ONBOARD_UNDO;
	AddButton(&tbb);

	tbb.iBitmap = 4;
	tbb.idCommand = ID_BUTTON_ONBOARD_REDO;
	AddButton(&tbb);

	tbb.iBitmap = 12;
	tbb.idCommand = ID_BUTTON_ONBOARD_LOCKVIEW;
	AddButton(&tbb);

	/*tbb.iBitmap = 5;
	tbb.idCommand = ID_ONBOARD_MOVE;
	AddButton(&tbb);	

	tbb.iBitmap = 6;
	tbb.idCommand = ID_ONBOARD_ROTATE;
	AddButton(&tbb);	

	tbb.iBitmap = 7;
	tbb.idCommand = ID_ONBOARD_SCALE;
	AddButton(&tbb);	*/

	//AddButton(&tbsep);

	//tbb.iBitmap = 8;
	//tbb.idCommand = ID_BUTTON_ONBOARD_ADDSECTION;
	//AddButton(&tbb);	

	//tbb.iBitmap = 9;
	//tbb.idCommand = ID_BUTTON_ONBOARD_ADDSECTIONPT;
	//AddButton(&tbb);

	//AddButton(&tbsep);

	//tbb.iBitmap = 10;
	//tbb.idCommand = ID_ONBOARD_GRID_SETTING;
	//AddButton(&tbb);

	
	SetWindowText (_T ("OnBoard Component Bar"));
	EnableDocking(CBRS_ALIGN_ANY);	

	//m_combActiveCategory.EnableWindow(FALSE);	
	//m_combActiveModel.EnableWindow(FALSE);

	//UpdateComboBox();

	return 0;
}

//load content
//BOOL CACComponentModelBar::UpdateComboBox()
//{
//	bool bDataReady = m_pInputOnboard && m_pInputOnboard->GetAirportDB() && m_pInputOnboard->GetAirportDB()->getACCompModelDB();
//	if(!bDataReady)
//	{		
//		return FALSE;
//	}
//	else
//	{
//		m_combActiveCategory.EnableWindow(TRUE);
//	}
//	
//	//delete all entries
//	m_combActiveCategory.ResetContent();
//	//--- update current category
//	//load categories
//	int nItemIDx = m_combActiveCategory.AddString(_T("New Category ...") );	
//	m_combActiveCategory.SetItemData(nItemIDx, -1);
//	
//	CAircraftComponentModelDatabase* pModelDB =  m_pInputOnboard->GetAirportDB()->getACCompModelDB();
//	int nCurSel = 0;
//	//for(int i=0;i< pModelDB->GetCatCount();++i)
//	//{
//	//	nItemIDx = m_combActiveCategory.AddString( pModelDB->GetCategory(i)->GetName() );
//	//	m_combActiveCategory.SetItemData(nItemIDx,i);
//	//	if(pModelDB->GetCategory(i) == m_pActiveCat)
//	//		nCurSel = nItemIDx;
//	//}
//	//m_combActiveCategory.SetCurSel(nCurSel);
//	//
//	////load models	
//	//if(m_pActiveCat && nCurSel)
//	//{
//	//	m_combActiveModel.EnableWindow(TRUE);
//	//	m_combActiveModel.ResetContent();
//	//	int nItemIDx = m_combActiveModel.AddString(_T("New Model ..."));	
//	//	m_combActiveModel.SetItemData(nItemIDx, -1);
//
//	//	int nCurmodelSel = 0;
//
//	//	for(int i=0;i<m_pActiveCat->GetModelCount();++i)
//	//	{
//	//		nItemIDx = m_combActiveModel.AddString(m_pActiveCat->GetModel(i)->GetName());
//	//		m_combActiveModel.SetItemData(nItemIDx, i);
//	//		if(m_pActiveModel == m_pActiveCat->GetModel(i) )
//	//		{
//	//			nCurmodelSel = nItemIDx;
//	//		}
//	//	}	
//
//	//	if(nCurmodelSel)
//	//		m_combActiveModel.SetCurSel(nCurmodelSel);
//	//}
//	//else{
//	//	m_combActiveModel.EnableWindow(FALSE);
//	//}
//	
//	return TRUE;	
//}
//
void CACComponentModelBar::SetInputOnboard( InputOnboard* pInputOnboard )
{
	if(m_pInputOnboard!= pInputOnboard)
	{
		m_pInputOnboard = pInputOnboard;
		//UpdateComboBox();
	}
}

void CACComponentModelBar::OnSelChangeActiveCategoryComboBox()
{
	//bool bDataReady = m_pInputOnboard && m_pInputOnboard->GetAirportDB() && m_pInputOnboard->GetAirportDB()->getACCompModelDB();
	//if(!bDataReady)
	//{
	//	m_combActiveCategory.EnableWindow(FALSE);	
	//	m_combActiveModel.EnableWindow(FALSE);
	//	return ;
	//}
	//CAircraftComponentModelDatabase* pModelDB = m_pInputOnboard->GetAirportDB()->getACCompModelDB();
	//m_pActiveCat = NULL;
	//	
	//int nCurSelCat = m_combActiveCategory.GetCurSel();
	//CString strFileFolder = m_pInputOnboard->GetAirportDB()->getFolder();

	//if(nCurSelCat==0)//new category
	//{
	//	CDlgGetString dlgGetStrname(0);
	//	dlgGetStrname.SetTitle(_T("Name New Category"));
	//	if(dlgGetStrname.DoModal() == IDOK)
	//	{
	//		CString newstrName = dlgGetStrname.m_sValue;
	//		bool bNameValid = true;
	//		for(int i=0;i<pModelDB->GetCatCount();++i)
	//		{
	//			if( pModelDB->GetCategory(i)->GetName().CompareNoCase(newstrName)==0 )
	//			{
	//				bNameValid = false;
	//				break;
	//			}
	//		}

	//		if(bNameValid)
	//		{
	//			CAircraftComponentCategory* pnewCat = new CAircraftComponentCategory(newstrName);		
	//			int nCatId = pModelDB->AddCategory(pnewCat);	 
	//			m_pActiveCat = pnewCat;
	//			int ItemIdx =m_combActiveCategory.AddString(pnewCat->GetName());
	//			m_combActiveCategory.SetItemData(ItemIdx, nCatId);
	//			m_combActiveCategory.SetCurSel(ItemIdx);
	//			pModelDB->SaveData(strFileFolder);
	//		}	
	//		else
	//		{
	//			CString errorstr;
	//			errorstr.Format("%s is already exist, please choose another name !", newstrName );
	//			MessageBox(errorstr, NULL);
	//		}
	//	}		
	//}	
	//else //change to a new category
	//{
	//	int nActiveCatidx = m_combActiveCategory.GetItemData(m_combActiveCategory.GetCurSel());
	//	m_pActiveCat = pModelDB->GetCategory(nActiveCatidx);
	//}

	////load active category models
	//m_pActiveModel = NULL;
	//if(m_pActiveCat)
	//{
	//	m_combActiveModel.EnableWindow(TRUE);
	//	m_combActiveModel.ResetContent();
	//	int nItemIDx = m_combActiveModel.AddString(_T("New Model ..."));	
	//	m_combActiveModel.SetItemData(nItemIDx, -1);

	//	m_combActiveModel.SetCurSel(nItemIDx);

	//	for(int i=0;i<m_pActiveCat->GetModelCount();++i)
	//	{
	//		nItemIDx = m_combActiveModel.AddString(m_pActiveCat->GetModel(i)->GetName());
	//		m_combActiveModel.SetItemData(nItemIDx, i);
	//	}		
	//}
	//else{
	//	m_combActiveModel.EnableWindow(FALSE);
	//}

	////
	//CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	//CView* pView=pMF->GetActiveFrame()->GetActiveView();
	//CTermPlanDoc* pDoc = (CTermPlanDoc*)pView->GetDocument();
	//ASSERT(pDoc);
	//if(pDoc){
	//	std::pair<CAirportDatabase*, CComponentMeshModel*> vDbModel;
	//	vDbModel.first = m_pInputOnboard->GetAirportDB();
	//	vDbModel.second = m_pActiveModel;
	//	pDoc->UpdateAllViews(NULL, VM_ONBOARD_COMPONENT_CHANGE_MODEL, (CObject*)&vDbModel );
	//}
}


void CACComponentModelBar::OnSelChangeActiveModelComboBox()
{	
	//bool bDataReady = m_pInputOnboard && m_pInputOnboard->GetAirportDB() && m_pInputOnboard->GetAirportDB()->getACCompModelDB();
	//if(!bDataReady)
	//{
	//	m_combActiveCategory.EnableWindow(FALSE);	
	//	m_combActiveModel.EnableWindow(FALSE);
	//	return ;
	//}
	//CAircraftComponentModelDatabase* pModelDB = m_pInputOnboard->GetAirportDB()->getACCompModelDB();
	//CString strFileFolder = m_pInputOnboard->GetAirportDB()->getFolder();
	//
	//
	//int nCatIdx = m_combActiveCategory.GetItemData(m_combActiveCategory.GetCurSel() );

	//CAircraftComponentCategory* pActiveCat = pModelDB->GetCategory(nCatIdx);
	//if(!pActiveCat)
	//{
	//	return;
	//}
	//m_pActiveModel = NULL;
	//
	//int nCurSelModel = m_combActiveModel.GetCurSel();
	//if(nCurSelModel == 0)
	//{
	//	CDlgGetString dlgGetStrname(0);
	//	CString strTitle;
	//	strTitle.Format("Name new %s ", pActiveCat->GetName() );
	//	dlgGetStrname.SetTitle(strTitle);

	//	if(dlgGetStrname.DoModal() == IDOK)
	//	{
	//		CString newstrName = dlgGetStrname.m_sValue;
	//		bool bNameValid = true;
	//		for(int i=0;i<pActiveCat->GetModelCount();++i)
	//		{
	//			if( pActiveCat->GetModel(i)->GetName().CompareNoCase(newstrName)==0 )
	//			{
	//				bNameValid = false;
	//				break;
	//			}
	//		}
	//		if(bNameValid)
	//		{
	//			CComponentMeshModel* pnewModel = new CComponentMeshModel(newstrName);
	//			int nModelId = pActiveCat->AddModel(pnewModel);		
	//			m_pActiveModel = pnewModel;
	//			int nItemIDx = m_combActiveModel.AddString(pnewModel->GetName());
	//			m_combActiveModel.SetItemData(nItemIDx, nModelId);
	//			m_combActiveModel.SetCurSel(nItemIDx);
	//			pModelDB->SaveData(strFileFolder);
	//		}
	//		else
	//		{
	//			CString errorstr;
	//			errorstr.Format("%s is already exist, please choose another name !", newstrName );
	//			MessageBox(errorstr, NULL);
	//		}
	//	}
	//		
	//}
	//else
	//{
	//	int nItemData = m_combActiveModel.GetItemData( m_combActiveModel.GetCurSel() );
	//	m_pActiveModel = pActiveCat->GetModel(nItemData);
	//}

	////update item
	//
	//
	////notify views to update 
	//CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	//CView* pView=pMF->GetActiveFrame()->GetActiveView();
	//CTermPlanDoc* pDoc = (CTermPlanDoc*)pView->GetDocument();
	//ASSERT(pDoc);
	//if(pDoc){
	//	std::pair<CAirportDatabase*, CComponentMeshModel*> vDbModel;
	//	vDbModel.first = m_pInputOnboard->GetAirportDB();
	//	vDbModel.second = m_pActiveModel;
	//	pDoc->UpdateAllViews(NULL, VM_ONBOARD_COMPONENT_CHANGE_MODEL, (CObject*)&vDbModel );
	//}
	
	
}

void CACComponentModelBar::OnDeleteComponetOrCategory()
{
	CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	CView* pView=pMF->GetActiveFrame()->GetActiveView();
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pView->GetDocument();
	ASSERT(pDoc);
	
	//if(m_pActiveModel && m_pActiveCat)
	//{
	//	CString strMsg;
	//	strMsg.Format("Are you sure to delete Component %s", m_pActiveModel->GetName().GetString() );
	//	if( AfxMessageBox(strMsg,MB_YESNO | MB_ICONQUESTION) == IDYES )
	//	{
	//		m_pActiveCat->DelModel(m_pActiveModel);
	//		std::pair<CAirportDatabase*, CComponentMeshModel*> vDbModel;
	//		vDbModel.first = m_pInputOnboard->GetAirportDB();
	//		vDbModel.second = NULL;
	//		pDoc->UpdateAllViews(NULL, VM_ONBOARD_COMPONENT_CHANGE_MODEL, (CObject*)&vDbModel );
	//		m_pActiveModel = NULL;
	//		UpdateComboBox();
	//	}
	//}
	//else if(!m_pActiveModel && m_pActiveCat && m_pInputOnboard && m_pInputOnboard->GetAirportDB() )
	//{
	//	CString strMsg;
	//	strMsg.Format("Are you sure to delete Category %s", m_pActiveCat->GetName());

	//	if( AfxMessageBox(strMsg,MB_YESNO | MB_ICONQUESTION) == IDYES )
	//	{
	//		m_pInputOnboard->GetAirportDB()->getACCompModelDB()->DelCategory(m_pActiveCat);
	//		m_pActiveCat = NULL;
	//		UpdateComboBox();
	//	}
	//}


}


void CACComponentModelBar::OnUpdateUIDelCompOrCat(CCmdUI* pCmdUI)
{
	if(!m_pActiveModel && !m_pActiveCat)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}
